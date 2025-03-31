/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_canopen_mcan.h"
#include "hpm_canopen_test.h"

extern struct hpm_master_receive_buf canopen_rx_buf;

static uint32_t hpm_mcan_get_first_filter_index(const mcan_rx_message_t *buf,
                                        const struct hpm_can_data *data)
{
    int32_t filter_index = -EINVAL;
    uint32_t ext_filter_count = data->ext_filter_count;
    uint32_t std_filter_count = data->std_filter_count;

    if (buf->use_ext_id) {
        for (uint32_t i = 0; i < ext_filter_count; i++) {
            const mcan_filter_elem_t *filter = &data->ext_filters[i];
            if ((buf->ext_id | filter->filter_mask) == (filter->filter_id | filter->filter_mask)) {
                filter_index = i;
                break;
            }
        }
    } else {
        for (uint32_t i = 0; i < std_filter_count; i++) {
            const mcan_filter_elem_t *filter = &data->std_filters[i];
            if ((buf->std_id | filter->filter_mask) == (filter->filter_id | filter->filter_mask)) {
                filter_index = i;
                break;
            }
        }
    }
    return filter_index;
}

/* Get MCAN message from RX BUF */
void hpm_mcan_get_message_from_rxbuf(const struct device *dev, uint32_t buf_index)
{
    const struct hpm_can_config *cfg = dev->config;
    MCAN_Type *can = cfg->base;
    struct hpm_can_data *data = dev->data;
    mcan_rx_message_t s_can_rx_buf = {0};
    can_rx_callback_t cb;
    struct can_frame frame;
    void *cb_arg;
    memset(&s_can_rx_buf, 0, sizeof(s_can_rx_buf));
    mcan_read_rxbuf(can, buf_index, (mcan_rx_message_t *) &s_can_rx_buf);

    canopen_rx_buf.has_received_message = true;
    memcpy(&canopen_rx_buf.rx_buf, &s_can_rx_buf, sizeof(s_can_rx_buf));

    uint32_t filter_index = hpm_mcan_get_first_filter_index(&s_can_rx_buf, data);
    memset(&frame, 0, sizeof(frame));
    if (s_can_rx_buf.use_ext_id) {
        frame.id = s_can_rx_buf.ext_id;
    } else {
        frame.id = s_can_rx_buf.std_id;
    }
    frame.flags = 0;
    if (s_can_rx_buf.canfd_frame) {
        frame.flags |= CAN_FRAME_FDF;
    }
    if (s_can_rx_buf.rtr) {
        frame.flags |= CAN_FRAME_RTR;
    }
    if (s_can_rx_buf.use_ext_id) {
        frame.flags |= CAN_FRAME_IDE;
    }
    if (s_can_rx_buf.bitrate_switch) {
        frame.flags |= CAN_FRAME_BRS;
    }
    frame.dlc = s_can_rx_buf.dlc;

    size_t data_len = can_dlc_to_bytes(frame.dlc);
    for (uint32_t i = 0; i < data_len; i++) {
        frame.data[i] = s_can_rx_buf.data_8[i];
    }

    /* Handle RX filter callback */
    if (filter_index != (uint32_t)-EINVAL) {
        /* If RTR bit does not match filter RTR mask and bit, drop current frame */
        bool rtr_filter_mask = (data->filter_rtr_mask & BIT(filter_index)) != 0;
        bool rtr_filter = (data->filter_rtr & BIT(filter_index)) != 0;
        bool rtr_in_frame = (frame.flags & CAN_FRAME_RTR);
        if ((rtr_filter_mask != rtr_in_frame) || (rtr_filter != rtr_in_frame)) {
            return;
        }

        if (s_can_rx_buf.use_ext_id) {
            cb = data->rx_cb_ext[filter_index];
            cb_arg = data->rx_cb_arg_ext[filter_index];
        } else {
            cb = data->rx_cb_std[filter_index];
            cb_arg = data->rx_cb_arg_std[filter_index];
        }

        if (cb != NULL) {
            cb(dev, &frame, cb_arg);
        } else {
            printf("cb missing\n");
        }
    }
}

/* Get MCAN message from RX BUF */
void hpm_mcan_get_message_from_rxfifo(const struct device *dev, uint32_t fifo_index)
{
    const struct hpm_can_config *cfg = dev->config;
    MCAN_Type *can = cfg->base;
    struct hpm_can_data *data = dev->data;
    mcan_rx_message_t s_can_rx_buf = {0};
    can_rx_callback_t cb;
    struct can_frame frame;
    void *cb_arg;
    memset(&s_can_rx_buf, 0, sizeof(s_can_rx_buf));
    mcan_read_rxfifo(can, fifo_index, (mcan_rx_message_t *) &s_can_rx_buf);

    canopen_rx_buf.has_received_message = true;
    memcpy(&canopen_rx_buf.rx_buf, &s_can_rx_buf, sizeof(s_can_rx_buf));

    uint32_t filter_index = hpm_mcan_get_first_filter_index(&s_can_rx_buf, data);
    memset(&frame, 0, sizeof(frame));
    if (s_can_rx_buf.use_ext_id) {
        frame.id = s_can_rx_buf.ext_id;
    } else {
        frame.id = s_can_rx_buf.std_id;
    }
    frame.flags = 0;
    if (s_can_rx_buf.canfd_frame) {
        frame.flags |= CAN_FRAME_FDF;
    }
    if (s_can_rx_buf.rtr) {
        frame.flags |= CAN_FRAME_RTR;
    }
    if (s_can_rx_buf.use_ext_id) {
        frame.flags |= CAN_FRAME_IDE;
    }
    if (s_can_rx_buf.bitrate_switch) {
        frame.flags |= CAN_FRAME_BRS;
    }
    frame.dlc = s_can_rx_buf.dlc;
    size_t data_len = can_dlc_to_bytes(frame.dlc);
    for (uint32_t i = 0; i < data_len; i++) {
        frame.data[i] = s_can_rx_buf.data_8[i];
    }

    /* Handle RX filter callback */
    if (filter_index != (uint32_t)-EINVAL) {
        /* If RTR bit does not match filter RTR mask and bit, drop current frame */
        bool rtr_filter_mask = (data->filter_rtr_mask & BIT(filter_index)) != 0;
        bool rtr_filter = (data->filter_rtr & BIT(filter_index)) != 0;
        bool rtr_in_frame = (frame.flags & CAN_FRAME_RTR);
        if ((rtr_filter_mask != rtr_in_frame) || (rtr_filter != rtr_in_frame)) {
            return;
        }

        if (s_can_rx_buf.use_ext_id) {
            cb = data->rx_cb_ext[filter_index];
            cb_arg = data->rx_cb_arg_ext[filter_index];
        } else {
            cb = data->rx_cb_std[filter_index];
            cb_arg = data->rx_cb_arg_std[filter_index];
        }

        if (cb != NULL) {
            cb(dev, &frame, cb_arg);
        } else {
            printf("cb missing\n");
        }
    }
}

void canopen_irq_handler(struct device *canopendevice)
{
    struct hpm_can_config *cfg = canopendevice->config;
    struct hpm_can_data *data = canopendevice->data;
    MCAN_Type *can = cfg->base;
    uint32_t flags = mcan_get_interrupt_flags(can);
    /* New message is available in RXFIFO0 */
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0) {
        hpm_mcan_get_message_from_rxfifo(canopendevice, 0);
    }
    /* New message is available in RXFIFO1 */
    if ((flags & MCAN_INT_RXFIFO1_NEW_MSG) != 0U) {
        hpm_mcan_get_message_from_rxfifo(canopendevice, 1);
    }
    /* New message is available in RXBUF */
    if ((flags & MCAN_INT_MSG_STORE_TO_RXBUF) != 0U) {
        /* NOTE: Below code is for demonstration purpose, the performance is not optimized
         *       Users should optimize the performance according to real use case.
         */
        for (uint32_t buf_index = 0; buf_index < MCAN_RXBUF_SIZE_CAN_DEFAULT; buf_index++) {
            if (mcan_is_rxbuf_data_available(can, buf_index)) {
                hpm_mcan_get_message_from_rxbuf(canopendevice, buf_index);
                mcan_clear_rxbuf_data_available_flag(can, buf_index);
            }
        }
    }

    /* Transmit completed */
    if ((flags & MCAN_EVENT_TRANSMIT) != 0U) {
        data->has_sent_out = true;
    }

    /* Error happened */
    if ((flags & MCAN_EVENT_ERROR) != 0) {
    }

    mcan_clear_interrupt_flags(can, flags);
}

int hpm_mcan_add_rx_filter_ext(const struct device *dev,
                            can_rx_callback_t callback,
                            void *user_data,
                            const struct can_filter *filter)
{
    const struct hpm_can_config *cfg = dev->config;
    struct hpm_can_data *data = dev->data;
    MCAN_Type *can = cfg->base;
    int filter_id = -ENOSPC;

    if (callback == NULL) {
        return -EINVAL;
    }

    if (data->ext_filter_count < HPM_CAN_EXT_FILTER_NUM_MAX) {
        filter_id = data->ext_filter_count;
        data->ext_filter_count++;
    } else {
        return -ENOSPC;
    }

    mcan_config_t *can_config = &data->config;
    mcan_filter_elem_t *filter_ext = &data->ext_filters[filter_id];
    data->ext_filters_index[filter_id] = filter_id;
    filter_ext->filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    filter_ext->filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    filter_ext->can_id_type = MCAN_CAN_ID_TYPE_EXTENDED;
    filter_ext->filter_id = filter->id;
    filter_ext->filter_mask = ~filter->mask;

    can_config->all_filters_config.ext_id_filter_list.mcan_filter_elem_count = data->ext_filter_count;
    can_config->all_filters_config.ext_id_filter_list.filter_elem_list = data->ext_filters;

    if ((filter->flags & CAN_FRAME_RTR) != 0) {
		data->filter_rtr |= (1U << filter_id);
	} else {
		data->filter_rtr &= ~(1U << filter_id);
	}

	if ((filter->flags & CAN_FRAME_RTR) != 0) {
		data->filter_rtr_mask |= (1U << filter_id);
	} else {
		data->filter_rtr_mask &= ~(1U << filter_id);
	}

    uint32_t can_clk_freq = board_init_can_clock(can);

    hpm_stat_t status = mcan_init(can, can_config, can_clk_freq);

    if (status != status_success) {
        printf("add rxfilt ext failed, error code: %d\n", status);
        return 0;
    }

    data->rx_cb_ext[filter_id] = callback;
    data->rx_cb_arg_ext[filter_id] = user_data;

    return filter_id;
}

int hpm_mcan_add_rx_filter_std(const struct device *dev,
                            can_rx_callback_t callback,
                            void *user_data,
                            const struct can_filter *filter)
{
    const struct hpm_can_config *cfg = dev->config;
    struct hpm_can_data *data = dev->data;
    MCAN_Type *can = cfg->base;
    int filter_id = -ENOSPC;

    if (callback == NULL) {
        return -EINVAL;
    }

    if (data->std_filter_count < HPM_CAN_STD_FILTER_NUM_MAX) {
        filter_id = data->std_filter_count;
        data->std_filter_count++;
    } else {
        return -ENOSPC;
    }

    mcan_config_t *can_config = &data->config;
    mcan_filter_elem_t *filter_std = &data->std_filters[filter_id];
    data->std_filters_index[filter_id] = filter_id;
    filter_std->filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    filter_std->filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    filter_std->can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    filter_std->filter_id = filter->id;
    filter_std->filter_mask = ~filter->mask;

    can_config->all_filters_config.std_id_filter_list.mcan_filter_elem_count = data->std_filter_count;
    can_config->all_filters_config.std_id_filter_list.filter_elem_list = data->std_filters;

    if ((filter->flags & CAN_FRAME_RTR) != 0) {
		data->filter_rtr |= (1U << filter_id);
	} else {
		data->filter_rtr &= ~(1U << filter_id);
	}

	if ((filter->flags & CAN_FRAME_RTR) != 0) {
		data->filter_rtr_mask |= (1U << filter_id);
	} else {
		data->filter_rtr_mask &= ~(1U << filter_id);
	}

    uint32_t can_clk_freq = board_init_can_clock(can);

    hpm_stat_t status = mcan_init(can, can_config, can_clk_freq);

    if (status != status_success) {
        printf("add rxfilt std failed, error code: %d\n", status);
        return 0;
    }

    data->rx_cb_std[filter_id] = callback;
    data->rx_cb_arg_std[filter_id] = user_data;

    return filter_id;
}

void hpm_mcan_remove_rx_filter(const struct device *dev, int filter_id)
{
    const struct hpm_can_config *cfg = dev->config;
    struct hpm_can_data *data = dev->data;
    MCAN_Type *can = cfg->base;

    mcan_config_t *config = &data->config;

    if (data->ext_filter_count > 0) {
        uint16_t last_filter_index = data->ext_filters_index[data->ext_filter_count - 1];
        mcan_filter_elem_t *last_filter_elem = &data->ext_filters[data->ext_filter_count - 1];
        for (uint32_t i = 0; i < data->ext_filter_count; i++) {
            if (data->ext_filters_index[i] == filter_id) {
                if (i != data->ext_filter_count - 1U) {
                    mcan_filter_elem_t *current_filter_elem = &data->ext_filters[i];
                    data->ext_filters_index[i] = last_filter_index;
                    current_filter_elem->filter_id = last_filter_elem->filter_id;
                    current_filter_elem->filter_mask = last_filter_elem->filter_mask;
                    current_filter_elem->filter_type = last_filter_elem->filter_type;
                    current_filter_elem->filter_config = last_filter_elem->filter_config;
                    current_filter_elem->can_id_type = last_filter_elem->can_id_type;
                }
                data->ext_filter_count -= 1;
                uint32_t can_clk_freq = board_init_can_clock(can);
                config->all_filters_config.ext_id_filter_list.filter_elem_list = data->ext_filters;
                config->all_filters_config.ext_id_filter_list.mcan_filter_elem_count = data->ext_filter_count;
                hpm_stat_t status = mcan_init(can, config, can_clk_freq);

                if (status != status_success) {
                    printf("remove rx filter failed, error code: %d\n", status);
                }
                break;
            }
        }
    }

    if (data->std_filter_count > 0) {
        uint16_t last_filter_index = data->std_filters_index[data->std_filter_count - 1];
        mcan_filter_elem_t *last_filter_elem = &data->std_filters[data->std_filter_count - 1];
        for (uint32_t i = 0; i < data->std_filter_count; i++) {
            if (data->std_filters_index[i] == filter_id) {
                if (i != data->std_filter_count - 1U) {
                    mcan_filter_elem_t *current_filter_elem = &data->std_filters[i];
                    data->std_filters_index[i] = last_filter_index;
                    current_filter_elem->filter_id = last_filter_elem->filter_id;
                    current_filter_elem->filter_mask = last_filter_elem->filter_mask;
                    current_filter_elem->filter_type = last_filter_elem->filter_type;
                    current_filter_elem->filter_config = last_filter_elem->filter_config;
                    current_filter_elem->can_id_type = last_filter_elem->can_id_type;
                }
                data->std_filter_count -= 1;
                uint32_t can_clk_freq = board_init_can_clock(can);
                config->all_filters_config.std_id_filter_list.filter_elem_list = data->std_filters;
                config->all_filters_config.std_id_filter_list.mcan_filter_elem_count = data->std_filter_count;
                hpm_stat_t status = mcan_init(can, config, can_clk_freq);

                if (status != status_success) {
                    printf("remove rx filter failed, error code: %d\n", status);
                }
                break;
            }
        }
    }
}

int hpm_mcan_set_timing(const struct device *dev, const struct can_timing *timing)
{
    int ret = 0;

    const struct hpm_can_config *cfg = dev->config;
    struct hpm_can_data *data = dev->data;
    MCAN_Type *mcan = cfg->base;

    if (data->started) {
		return -EBUSY;
	}

    mcan_config_t *config = &data->config;
    config->use_lowlevel_timing_setting = false;
    mcan_bit_timing_param_t *timing_param = &config->can_timing;
    timing_param->prescaler = timing->prescaler;
    /* num_seg1 in CAST_CAN = Tsync_seq + Tprop_seg + Tphase_seg1  */
    timing_param->num_seg1 = 1 + timing->prop_seg + timing->phase_seg1;
    timing_param->num_seg2 = timing->phase_seg2;
    timing_param->num_sjw = timing->sjw;

    uint32_t mcan_clk_freq = board_init_can_clock(mcan);
    hpm_stat_t status = mcan_init(mcan, config, mcan_clk_freq);
    if (status != status_success) {
        ret = -EAGAIN;
    }

    return ret;
}

int hpm_mcan_set_mode(const struct device *dev, can_mode_t mode)
{
    const struct hpm_can_config *cfg = dev->config;
    struct hpm_can_data *data = dev->data;
    mcan_config_t *config = &data->config;
    MCAN_Type *mcan = cfg->base;
    struct can_driver_data *common = (struct can_driver_data *)&data->common;

    if (data->started) {
		return -EBUSY;
	}

    if ((mode & CAN_MODE_LOOPBACK) != 0) {
        config->mode = mcan_mode_loopback_internal;
        mcan->CCCR |= MCAN_CCCR_MON_MASK | MCAN_CCCR_TEST_MASK;
        mcan->TEST |= MCAN_TEST_LBCK_MASK;
    } else if ((mode & CAN_MODE_LISTENONLY) != 0) {
        config->mode = mcan_mode_listen_only;
        mcan->CCCR |= MCAN_CCCR_MON_MASK;
    } else {
        config->mode = mcan_mode_normal;
        mcan->CCCR &= ~(MCAN_CCCR_MON_MASK | MCAN_CCCR_TEST_MASK);
    }

    common->mode = mode;

    return 0;
}

int hpm_mcan_start(const struct device *dev)
{
    const struct hpm_can_config *config = dev->config;
    struct hpm_can_data *data = dev->data;
    mcan_config_t *can_config = &data->config;
    MCAN_Type *mcan = config->base;

    if (data->started) {
        return -EALREADY;
    }

    uint32_t can_clk_freq = board_init_can_clock(mcan);
    hpm_stat_t status = mcan_init(mcan, can_config, can_clk_freq);

    if (status != status_success) {
        printf("start canopen failed, error code: %d\n", status);
        return 0;
    }

    mcan_enable_interrupts(BOARD_APP_CAN_BASE, MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT | MCAN_EVENT_ERROR);
    mcan_enable_txbuf_transmission_interrupt(BOARD_APP_CAN_BASE, ~0UL);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    data->started = true;

    return 0;
}

int hpm_mcan_get_state(const struct device *dev,
                             enum can_state *state,
                             struct can_bus_err_cnt *err_cnt)
{
    int ret = 0;
    mcan_protocol_status_t protocol_status;
    const struct hpm_can_config *cfg = dev->config;
    MCAN_Type *can = cfg->base;
    const struct hpm_can_data *data = dev->data;
    mcan_error_count_t error_count;
    memset(&error_count, 0, sizeof(mcan_error_count_t));

    if (!data->started) {
        *state = CAN_STATE_STOPPED;
    } else {
        if (state != NULL) {
            uint8_t flags = mcan_get_interrupt_flags(can);
            if ((flags & MCAN_EVENT_ERROR) != 0) {
                mcan_parse_protocol_status(can->PSR, &protocol_status);
                mcan_get_error_counter(can, &error_count);
                if (protocol_status.in_bus_off_state) {
                    *state = CAN_STATE_BUS_OFF;
                } else if (protocol_status.in_warning_state) {
                    *state = CAN_STATE_ERROR_WARNING;
                } else if (protocol_status.in_error_passive_state) {
                    *state = CAN_STATE_ERROR_PASSIVE;
                } else {
                    *state = CAN_STATE_ERROR_ACTIVE;
                }
            } else {
                *state = CAN_STATE_ERROR_ACTIVE;
            }
        }
    }

    if (err_cnt != NULL) {
        err_cnt->tx_err_cnt = error_count.transmit_error_count;
        err_cnt->rx_err_cnt = error_count.receive_error_count;
    }

    return ret;
}

void convert_can_frame_to_mcan_frame(const struct can_frame *frame,
                                     mcan_tx_frame_t *tx_buf)
{
    if (frame->flags & CAN_FRAME_IDE) {
        tx_buf->ext_id = frame->id;
        tx_buf->use_ext_id = 1;
    } else {
        tx_buf->std_id = frame->id;
    }
    tx_buf->dlc = frame->dlc;
    tx_buf->bitrate_switch = (uint16_t)((frame->flags & CAN_FRAME_BRS) != 0);
    tx_buf->rtr = (uint16_t)((frame->flags & CAN_FRAME_RTR) != 0);
    tx_buf->canfd_frame = (uint16_t)((frame->flags & CAN_FRAME_FDF) != 0);

    size_t msg_len = can_dlc_to_bytes(tx_buf->dlc);

    for (size_t i = 0; i < msg_len; i++) {
        tx_buf->data_8[i] = frame->data[i];
    }
}

int hpm_mcan_send(const struct device *dev,
                        const struct can_frame *frame)
{
    const struct hpm_can_config *cfg = dev->config;
    struct hpm_can_data *data = dev->data;
    MCAN_Type *can = cfg->base;
    enum can_state state;
    hpm_stat_t status;

	if ((frame->flags & (CAN_FRAME_FDF | CAN_FRAME_BRS | CAN_FRAME_ESI)) != 0) {
		printf("unsupported CAN frame flags 0x%02x\n", frame->flags);
		return -ENOTSUP;
	}

    if (frame->dlc > CAN_MAX_DLC) {
        printf("DLC of %d exceeds maximum (%d)\n", frame->dlc, CAN_MAX_DLC);
        return -EINVAL;
    }

    if (!data->started) {
		return -ENETDOWN;
	}

    (void) hpm_mcan_get_state(dev, &state, NULL);
    if (state == CAN_STATE_BUS_OFF) {
        printf("Transmit failed, bus-off\n");
        return -ENETUNREACH;
    }

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    convert_can_frame_to_mcan_frame(frame, &tx_buf);

    status = mcan_transmit_via_txbuf_nonblocking(can, 0, &tx_buf);

    if (status != 0) {
        return -EIO;
    }

    while (!data->has_sent_out) {
    }

    data->has_sent_out = false;

    return 0;
}