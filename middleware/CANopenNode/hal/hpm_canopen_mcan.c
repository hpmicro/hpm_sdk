/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_canopen_mcan.h"
#include "user_config.h"

extern const struct can_driver_api hpm_can_driver_api;
extern hpm_can_config_t hpm_canopen_config[MAX_CANOPEN_DEVICE];
extern hpm_can_data_t hpm_canopen_data[MAX_CANOPEN_DEVICE];
extern struct device hpm_canopen_dev[MAX_CANOPEN_DEVICE];

static uint32_t hpm_mcan_get_first_filter_index(const mcan_rx_message_t *buf,
                                        const hpm_can_data_t *data)
{
    int32_t filter_index = -EINVAL;
    uint32_t ext_filter_count = data->ext_filter_count;
    uint32_t std_filter_count = data->std_filter_count;

    if (buf->use_ext_id) {
        for (uint32_t i = 0; i < ext_filter_count; i++) {
            const mcan_filter_elem_t *filter = &data->ext_filters[i].filters;
            if ((buf->ext_id & filter->filter_mask) == (filter->filter_id & filter->filter_mask)) {
                filter_index = i;
                break;
            }
        }
    } else {
        for (uint32_t i = 0; i < std_filter_count; i++) {
            const mcan_filter_elem_t *filter = &data->std_filters[i].filters;
            if ((buf->std_id & filter->filter_mask) == (filter->filter_id & filter->filter_mask)) {
                filter_index = i;
                break;
            }
        }
    }
    return filter_index;
}

/* Get MCAN message from RX FIFO */
void hpm_mcan_get_message_from_rxfifo(const struct device *dev, uint32_t fifo_index)
{
    hpm_can_config_t *cfg = dev->config;
    MCAN_Type *can = cfg->base;
    hpm_can_data_t *data = dev->data;
    mcan_rx_message_t s_can_rx_buf = {0};
    can_rx_callback_t cb;
    struct can_frame frame;
    void *cb_arg;
    memset(&s_can_rx_buf, 0, sizeof(s_can_rx_buf));
    mcan_read_rxfifo(can, fifo_index, (mcan_rx_message_t *) &s_can_rx_buf);

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
        uint32_t rtr_reg = s_can_rx_buf.use_ext_id ? data->filter_rtr_ext : data->filter_rtr_std;
        uint32_t rtr_mask_reg = s_can_rx_buf.use_ext_id ? data->filter_rtr_mask_ext : data->filter_rtr_mask_std;
        bool rtr_filter_mask = (rtr_mask_reg & BIT(filter_index)) != 0;
        bool rtr_filter = (rtr_reg & BIT(filter_index)) != 0;
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
    hpm_can_config_t *cfg = canopendevice->config;
    hpm_can_data_t *data = canopendevice->data;
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

    /* Transmit completed */
    if ((flags & MCAN_EVENT_TRANSMIT) != 0U) {
        data->has_sent_out = true;
    }

    if ((flags & MCAN_IR_RF0L_MASK) != 0U) {
        mcan_clear_interrupt_flags(can, MCAN_IR_RF0L_MASK);
        /* ACK FIFO0 */
        can->RXF0A = 0;
    }

    if ((flags & MCAN_IR_RF1L_MASK) != 0U) {
        mcan_clear_interrupt_flags(can, MCAN_IR_RF1L_MASK);
        /* ACK FIFO1 */
        can->RXF1A = 0;
    }

    /* Error happened */
    if ((flags & MCAN_EVENT_ERROR) != 0) {
    }

    mcan_clear_interrupt_flags(can, flags);
}

int mcan_add_rx_filter(const struct device *dev,
                            can_rx_callback_t callback,
                            void *user_data,
                            const struct can_filter *filter, bool is_ext)
{
    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
    MCAN_Type *can = cfg->base;
    hpm_stat_t status = status_invalid_argument;
    mcan_filter_elem_t *filter_tmp;

    if (callback == NULL) {
        return -EINVAL;
    }

    uint32_t idx;
    uint32_t unique_id;

    if (is_ext) {
        if (data->ext_filter_count >= HPM_CAN_EXT_FILTER_NUM_MAX) {
            return -ENOSPC;
        }
        idx = data->ext_filter_count;
        data->ext_filter_count++;
        filter_tmp = &data->ext_filters[idx].filters;
        unique_id = data->next_filter_id;
        data->ext_filters[idx].filters_index = unique_id;
        data->next_filter_id++;
        filter_tmp->can_id_type = MCAN_CAN_ID_TYPE_EXTENDED;
    } else {
        if (data->std_filter_count >= HPM_CAN_STD_FILTER_NUM_MAX) {
            return -ENOSPC;
        }
        idx = data->std_filter_count;
        data->std_filter_count++;
        filter_tmp = &data->std_filters[idx].filters;
        unique_id = data->next_filter_id;
        data->std_filters[idx].filters_index = unique_id;
        data->next_filter_id++;
        filter_tmp->can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    }

    filter_tmp->filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    filter_tmp->filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    filter_tmp->filter_id = filter->id;
    filter_tmp->filter_mask = filter->mask;

    /* Per-type RTR tracking (bit position = per-type index, no collision) */
    uint32_t *rtr, *rtr_mask;
    if (is_ext) {
        rtr = &data->filter_rtr_ext;
        rtr_mask = &data->filter_rtr_mask_ext;
    } else {
        rtr = &data->filter_rtr_std;
        rtr_mask = &data->filter_rtr_mask_std;
    }
    if ((filter->flags & CAN_FRAME_RTR) != 0) {
        *rtr |= (1U << idx);
        *rtr_mask |= (1U << idx);
    } else {
        *rtr &= ~(1U << idx);
        *rtr_mask &= ~(1U << idx);
    }

    status = mcan_begin_reconfig(can);
    if (status != status_success) {
        printf("reconfig mcan failed, error code: %d\n", status);
    }

    status = mcan_set_filter_element(can, filter_tmp, idx);
    if (status != status_success) {
        printf("add rxfilt failed, error code: %d\n", status);
    }

    status = mcan_end_reconfig(can);
    if (status != status_success) {
        printf("reconfig mcan failed, error code: %d\n", status);
    }

    if (is_ext) {
        data->rx_cb_ext[idx] = callback;
        data->rx_cb_arg_ext[idx] = user_data;
    } else {
        data->rx_cb_std[idx] = callback;
        data->rx_cb_arg_std[idx] = user_data;
    }

    return (int)unique_id;
}

int hpm_mcan_add_rx_filter(const struct device *dev,
                            can_rx_callback_t callback,
                            void *user_data,
                            const struct can_filter *filter)
{
    int filter_id;

    if ((filter->flags & CAN_FILTER_IDE) != 0U) {
        filter_id = mcan_add_rx_filter(dev, callback, user_data, filter, true);
    } else {
        filter_id = mcan_add_rx_filter(dev, callback, user_data, filter, false);
    }

    return filter_id;
}

bool mcan_remove_filter(MCAN_Type *ptr, hpm_can_data_t *data, int filter_id, bool is_ext)
{
    bool find_index = false;
    hpm_stat_t status;
    uint32_t count;
    mcan_filter_t *filters;
    uint32_t found_idx = 0;
    bool is_last_index = false;

    if (is_ext) {
        count = data->ext_filter_count;
        filters = data->ext_filters;
    } else {
        count = data->std_filter_count;
        filters = data->std_filters;
    }

    if (count == 0) {
        return false;
    }

    for (uint32_t i = 0; i < count; i++) {
        if (filters[i].filters_index == (uint32_t)filter_id) {
            find_index = true;
            found_idx = i;
            break;
        }
    }
    if (!find_index) {
        return false;
    }

    uint32_t last_idx = count - 1;
    is_last_index = (found_idx == last_idx);
    uint16_t moved_unique_id = filters[last_idx].filters_index;

    if (!is_last_index) {
        /* Move last filter element to the vacated position */
        filters[found_idx].filters_index = moved_unique_id;
        memcpy(&filters[found_idx].filters, &filters[last_idx].filters, sizeof(mcan_filter_elem_t));
    }
    filters[last_idx].filters_index = 0;

    status = mcan_begin_reconfig(ptr);
    if (status != status_success) {
        printf("reconfig mcan failed, error code: %d\n", status);
    }

    /* Write the (possibly moved) filter to HW at the vacated per-type index (not the unique_id) */
    status = mcan_set_filter_element(ptr, &filters[found_idx].filters, found_idx);
    if (status != status_success) {
        printf("remove rx filter failed, error code: %d\n", status);
        return false;
    }

    if (!is_last_index) {
        /* Clear the old last slot in HW */
        mcan_filter_elem_t empty;
        memset(&empty, 0, sizeof(empty));
        status = mcan_set_filter_element(ptr, &empty, last_idx);
        if (status != status_success) {
            printf("remove rx filter failed, error code: %d\n", status);
            return false;
        }
    }

    status = mcan_end_reconfig(ptr);
    if (status != status_success) {
        printf("reconfig mcan failed, error code: %d\n", status);
    }

    return true;
}

void hpm_mcan_remove_rx_filter(const struct device *dev, int filter_id)
{
    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
    MCAN_Type *can = cfg->base;
    bool state = false;

    if (filter_id < 0) {
        printf("filter ID %d out of bounds\n", filter_id);
        return;
    }

    if (data->std_filter_count > 0) {
        state = mcan_remove_filter(can, data, filter_id, false);
        if (state == true) {
            data->std_filter_count -= 1;
            return;
        }
    }

    if (data->ext_filter_count > 0) {
        state = mcan_remove_filter(can, data, filter_id, true);
        if (state == true) {
            data->ext_filter_count -= 1;
            return;
        }
    }

    printf("cannot find matched filter-id\n");
}

ATTR_WEAK void board_can_transceiver_phy_set(MCAN_Type *ptr, bool enable)
{
    ARG_UNUSED(ptr);
    ARG_UNUSED(enable);
}

int hpm_mcan_start(const struct device *dev)
{
    hpm_can_config_t *config = dev->config;
    hpm_can_data_t *data = dev->data;
    MCAN_Type *mcan = config->base;

    if (data->started) {
        return -EALREADY;
    }

    /* enable mcan controller */
    board_can_transceiver_phy_set(mcan, true);

    data->started = true;

    return 0;
}

int hpm_mcan_stop(const struct device *dev)
{
    hpm_can_config_t *config = dev->config;
    hpm_can_data_t *data = dev->data;
    MCAN_Type *mcan = config->base;

    if (!data->started) {
        return -EALREADY;
    }

    /* disable mcan controller */
    board_can_transceiver_phy_set(mcan, false);

    data->started = false;

    return 0;
}

int hpm_mcan_get_state(const struct device *dev,
                             enum can_state *state,
                             struct can_bus_err_cnt *err_cnt)
{
    int ret = 0;
    mcan_protocol_status_t protocol_status;
    hpm_can_config_t *cfg = dev->config;
    MCAN_Type *can = cfg->base;
    hpm_can_data_t *data = dev->data;
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
    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
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

void hpm_mcan_init(struct canopen_context *candriverstate, can_info_t *can_info, uint32_t baudrate, int index)
{
    hpm_stat_t  status;
    mcan_deinit(can_info->can_base);
    mcan_config_t can_config;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
    }
#endif
    mcan_get_default_config(can_info->can_base, &can_config);
    can_config.mode = mcan_mode_normal;

    can_config.baudrate = baudrate;
    uint32_t can_clk_freq = board_init_can_clock(can_info->can_base);
    board_init_can(can_info->can_base);

    can_config.interrupt_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT | MCAN_EVENT_ERROR;
    can_config.txbuf_trans_interrupt_mask = ~0UL;

    status = mcan_init(can_info->can_base, &can_config, can_clk_freq);

    if (status != status_success) {
        printf("canopen init failed, error code: %d\n", status);
    }

    intc_m_enable_irq_with_priority(can_info->irq_num, can_info->priority);

    hpm_canopen_config[index].base = can_info->can_base;
    hpm_canopen_data[index].config = can_config;
    hpm_canopen_data[index].started = false;
    hpm_canopen_data[index].next_filter_id = 0;
    hpm_canopen_data[index].filter_rtr_ext = 0;
    hpm_canopen_data[index].filter_rtr_mask_ext = 0;
    hpm_canopen_data[index].filter_rtr_std = 0;
    hpm_canopen_data[index].filter_rtr_mask_std = 0;
    hpm_canopen_data[index].ext_filter_count = 0;
    hpm_canopen_data[index].std_filter_count = 0;
    hpm_canopen_dev[index].config = &hpm_canopen_config[index];
    hpm_canopen_dev[index].data = &hpm_canopen_data[index];
    hpm_canopen_dev[index].api = &hpm_can_driver_api;

    candriverstate->dev = &hpm_canopen_dev[index];
}

void hpm_canopen_reboot(void)
{
    /*Enable software reset*/
    ppor_reset_mask_set_source_enable(HPM_PPOR, (uint32_t)ppor_reset_software);
    ppor_sw_reset(HPM_PPOR, 1000);
    while (1) {
    }
}