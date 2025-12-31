/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_canopen_can.h"
#include "user_config.h"

extern const struct can_driver_api hpm_can_driver_api;
extern hpm_can_config_t hpm_canopen_config[MAX_CANOPEN_DEVICE];
extern hpm_can_data_t hpm_canopen_data[MAX_CANOPEN_DEVICE];
extern struct device hpm_canopen_dev[MAX_CANOPEN_DEVICE];

#if defined(CONFIG_CANOPEN_MASTER)
extern hpm_master_receive_buf_t canopen_rx_buf;
#endif

void hpm_can_get_message(const struct device *dev);

void canopen_irq_handler(struct device *canopendevice)
{
    hpm_can_config_t *cfg = canopendevice->config;
    hpm_can_data_t *data = canopendevice->data;
    CAN_Type *can = cfg->base;

    uint8_t tx_rx_flags = can_get_tx_rx_flags(can);
    uint8_t error_flags = can_get_error_interrupt_flags(can);

    /* Handle TX/RX flags */
    if ((tx_rx_flags & CAN_EVENT_RECEIVE) != 0U) {

        while (can_is_data_available_in_receive_buffer(can)) {
            hpm_can_get_message(canopendevice);
        }
        can_clear_tx_rx_flags(can, CAN_EVENT_RECEIVE);
    }

    if ((tx_rx_flags & (CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF )) != 0) {
        data->has_sent_out = true;
        can_clear_tx_rx_flags(can, CAN_EVENT_TX_PRIMARY_BUF);
    }

    if ((tx_rx_flags & CAN_EVENT_ERROR) != 0) {
        can_clear_tx_rx_flags(can, CAN_EVENT_ERROR);
    }

    /* Handle error flags */
    if ((error_flags & (CAN_ERRINT_EPIE_MASK | CAN_ERRINT_ALIE_MASK | CAN_ERRINT_BEIE_MASK)) != 0U) {
        can_clear_error_interrupt_flags(can, error_flags);
    }
}

uint32_t hpm_can_get_first_filter_index(const can_receive_buf_t *buf,
                                        const hpm_can_data_t *data)
{
    int32_t filter_index = -EINVAL;
    for (uint32_t i = 0; i < data->can_filter_count; i++) {
        const can_filter_config_t *filter = &data->filter_list[i];

        /* NOTE: if corresponding bit field is 1, it means "don't care" */
        if (((buf->extend_id == 1) && (filter->id_mode == can_filter_id_mode_extended_frames)) ||
            ((buf->extend_id == 0) && (filter->id_mode == can_filter_id_mode_standard_frames))) {
            if ((buf->id | filter->mask) == (filter->code | filter->mask)) {
                filter_index = i;
                break;
            }
        }
    }
    return filter_index;
}

/* Get CAN message from CAN device */
void hpm_can_get_message(const struct device *dev)
{
    hpm_can_config_t *cfg = dev->config;
    CAN_Type *can = cfg->base;
    hpm_can_data_t *data = dev->data;
    can_rx_callback_t cb;
    struct can_frame frame;
    void *cb_arg;
    can_receive_buf_t rx_buf;
    memset(&rx_buf, 0, sizeof(rx_buf));
    can_read_received_message(can, &rx_buf);

#if defined(CONFIG_CANOPEN_MASTER)
    memset(&canopen_rx_buf, 0, sizeof(canopen_rx_buf));
    canopen_rx_buf.has_received_message = true;
    memcpy(&canopen_rx_buf.rx_buf, &rx_buf, sizeof(rx_buf));
#endif

    uint32_t filter_index = hpm_can_get_first_filter_index(&rx_buf, data);
    frame.id = rx_buf.id;
    frame.flags = 0;
    if (rx_buf.canfd_frame) {
        frame.flags |= CAN_FRAME_FDF;
    }
    if (rx_buf.remote_frame) {
        frame.flags |= CAN_FRAME_RTR;
    }
    if (rx_buf.extend_id) {
        frame.flags |= CAN_FRAME_IDE;
    }
    if (rx_buf.bitrate_switch) {
        frame.flags |= CAN_FRAME_BRS;
    }
    frame.dlc = rx_buf.dlc;

    size_t data_len = can_dlc_to_bytes(frame.dlc);
    for (uint32_t i = 0; i < data_len; i++) {
        frame.data[i] = rx_buf.data[i];
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

        cb = data->rx_cb[filter_index];
        cb_arg = data->rx_cb_arg[filter_index];
        if (cb != NULL) {
            cb(dev, &frame, cb_arg);
        } else {
            return;
        }
    }
}

int hpm_can_add_rx_filter(const struct device *dev,
                            can_rx_callback_t callback,
                            void *user_data,
                            const struct can_filter *filter)
{
    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
    CAN_Type *can = cfg->base;
    int filter_id;

    if (callback == NULL) {
        return -EINVAL;
    }

    if (data->can_filter_count < HPM_CAN_FILTER_NUM_MAX) {
        filter_id = data->can_filter_count;
        data->can_filter_count++;
    } else {
        return -ENOSPC;
    }

    can_config_t *config = &data->config;
    can_filter_config_t *can_filter = &data->filter_list[filter_id];
    can_filter->index = filter_id;
    can_filter->id_mode = ((filter->flags & CAN_FILTER_IDE) != 0) ? can_filter_id_mode_extended_frames : can_filter_id_mode_standard_frames;
    can_filter->enable = true;
    can_filter->code = filter->id;
    /* NOTE: the filter mask definition in this CAN IP is different from standard definition */
    can_filter->mask = ~filter->mask;

    config->filter_list_num = data->can_filter_count;
    config->filter_list = data->filter_list;

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

    hpm_stat_t status = can_init(can, config, can_clk_freq);
    if (status != status_success) {
        printf("add rxfilt std failed, error code: %d\n", status);
        return 0;
    }

    data->rx_cb[filter_id] = callback;
    data->rx_cb_arg[filter_id] = user_data;

    return filter_id;
}

void hpm_can_remove_rx_filter(const struct device *dev, int filter_id)
{
    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
    CAN_Type *can = cfg->base;

    can_config_t *config = &data->config;
    if (data->can_filter_count > 0) {
        can_filter_config_t *last_filter = &data->filter_list[data->can_filter_count - 1];
        for (uint32_t i = 0; i < data->can_filter_count; i++) {
            if (data->filter_list[i].index == filter_id) {

                /* Move the last filter to current filter location */
                if (i != data->can_filter_count - 1U) {
                    can_filter_config_t *current_filter = &data->filter_list[i];
                    current_filter->index = last_filter->index;
                    current_filter->id_mode = last_filter->id_mode;
                    current_filter->enable = last_filter->enable;
                    current_filter->code = last_filter->code;
                    current_filter->mask = last_filter->mask;
                }
                /* Remove the last filter */
                memset(last_filter, 0, sizeof(can_filter_config_t));
                data->can_filter_count -= 1U;

                uint32_t can_clk_freq = board_init_can_clock(can);
                config->filter_list_num = data->can_filter_count;
                config->filter_list = data->filter_list;
                hpm_stat_t status = can_init(can, config, can_clk_freq);
                if (status != status_success) {
                    printf("remove rxfilt std failed, error code: %d\n", status);
                }
            }
        }
    }
}

ATTR_WEAK void board_can_transceiver_phy_set(CAN_Type *ptr, bool enable)
{
    ARG_UNUSED(ptr);
    ARG_UNUSED(enable);
}

int hpm_can_start(const struct device *dev)
{
    hpm_can_config_t *config = dev->config;
    hpm_can_data_t *data = dev->data;
    CAN_Type *can = config->base;

    if (data->started) {
        return -EALREADY;
    }

    /* enable can controller */
    board_can_transceiver_phy_set(can, true);

    data->started = true;

    return 0;
}

int hpm_can_stop(const struct device *dev)
{
    hpm_can_config_t *config = dev->config;
    hpm_can_data_t *data = dev->data;
    CAN_Type *can = config->base;

    if (!data->started) {
        return -EALREADY;
    }

    /* disable can controller */
    board_can_transceiver_phy_set(can, false);

    data->started = false;

    return 0;
}

int hpm_can_get_state(const struct device *dev,
                             enum can_state *state,
                             struct can_bus_err_cnt *err_cnt)
{
    int ret = 0;

    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
    CAN_Type *can = cfg->base;

    if (!data->started) {
        *state = CAN_STATE_STOPPED;
    } else {
        if (state != NULL) {
            uint8_t error_flags = can_get_error_interrupt_flags(can);
            if (can_is_in_bus_off_mode(can)) {
                *state = CAN_STATE_BUS_OFF;
            } else if ((error_flags & CAN_ERROR_WARNING_LIMIT_FLAG) != 0U) {
                *state = CAN_STATE_ERROR_WARNING;
            } else if ((error_flags & CAN_ERROR_PASSIVE_MODE_ACTIVE_FLAG) != 0U) {
                *state = CAN_STATE_ERROR_PASSIVE;
            } else if (error_flags == 0U) {
                *state = CAN_STATE_ERROR_ACTIVE;
            }
        }
    }

    if (err_cnt != NULL) {
        err_cnt->tx_err_cnt = can_get_transmit_error_count(can);
        err_cnt->rx_err_cnt = can_get_receive_error_count(can);
    }

    return ret;
}


void convert_can_frame_to_can_frame(const struct can_frame *frame,
                                     can_transmit_buf_t *tx_buf)
{
    tx_buf->buffer[0] = 0;
    tx_buf->buffer[1] = 0;

    tx_buf->id = frame->id;
    tx_buf->dlc = frame->dlc;
    tx_buf->bitrate_switch = (uint16_t)((frame->flags & CAN_FRAME_BRS) != 0);
    tx_buf->remote_frame = (uint16_t)((frame->flags & CAN_FRAME_RTR) != 0);
    tx_buf->canfd_frame = (uint16_t)((frame->flags & CAN_FRAME_FDF) != 0);
    tx_buf->extend_id = (uint16_t)((frame->flags & CAN_FRAME_IDE) != 0);

    size_t msg_len = can_dlc_to_bytes(tx_buf->dlc);

    for (size_t i = 0; i < msg_len; i++) {
        tx_buf->data[i] = frame->data[i];
    }
}

int hpm_can_send(const struct device *dev,
                        const struct can_frame *frame)
{
    hpm_can_config_t *cfg = dev->config;
    hpm_can_data_t *data = dev->data;
    CAN_Type *can = cfg->base;
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

    data->has_sent_out = false;

    (void) hpm_can_get_state(dev, &state, NULL);
    if (state == CAN_STATE_BUS_OFF) {
        printf("Transmit failed, bus-off\n");
        return -ENETUNREACH;
    }

    can_transmit_buf_t tx_buf;
    convert_can_frame_to_can_frame(frame, &tx_buf);

    uint32_t fifo_idx = 0;
    if (!can_is_primary_transmit_buffer_full(can)) {
        fifo_idx = 0;
    } else {
        fifo_idx = 1;
    }

    if (fifo_idx == 0) {
        status = can_send_high_priority_message_nonblocking(can, &tx_buf);
    } else {
        status = can_send_message_nonblocking(can, &tx_buf);
    }
    if (status != 0) {
        return -EIO;
    }

    while (!data->has_sent_out) {
    }

    data->has_sent_out = false;

    return 0;
}


void hpm_can_init(struct canopen_context *candriverstate, can_info_t *can_info, uint32_t baudrate, int index)
{
    hpm_stat_t  status;
    can_deinit(can_info->can_base);
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.mode = can_mode_normal;

    can_config.baudrate = baudrate;
    uint32_t can_clk_freq = board_init_can_clock(can_info->can_base);
    board_init_can(can_info->can_base);

    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF | CAN_EVENT_ERROR;

    status = can_init(can_info->can_base, &can_config, can_clk_freq);

    if (status != status_success) {
        printf("canopen init failed, error code: %d\n", status);
    }

    intc_m_enable_irq_with_priority(can_info->irq_num, can_info->priority);

    hpm_canopen_config[index].base = can_info->can_base;
    hpm_canopen_data[index].config = can_config;
    hpm_canopen_data[index].started = false;
    hpm_canopen_data[index].filter_rtr = 0;
    hpm_canopen_data[index].filter_rtr_mask = 0;
    hpm_canopen_data[index].can_filter_count = 0;
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