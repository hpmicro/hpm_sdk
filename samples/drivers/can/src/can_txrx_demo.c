/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"

void board_can_send_multiple_can_messages(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000;
    can_config.mode = can_mode_normal;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = 8;
    uint32_t msg_len = can_get_payload_size_from_dlc((uint8_t) tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = (uint8_t) (i | (i << 4));
    }

    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        status = can_send_message_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent message failed, error_code:%d\n", status);
            return;
        }
    }

    printf("Sent CAN messages with ID from 0 to 2047 out\n");
}

void board_can_send_multiple_canfd_messages(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
#if defined(USE_BAUDRATE_FOR_CANFD) && (USE_BAUDRATE_FOR_CANFD == 1)
    can_config.baudrate = 500000;
    can_config.baudrate_fd = 5000000;
#else
    can_config.use_lowlevel_timing_setting = true;
    can_config.can_timing.num_seg1 = 60;
    can_config.can_timing.num_seg2 = 20;
    can_config.can_timing.num_sjw = 16;
    can_config.can_timing.prescaler = 2;
    can_config.canfd_timing.num_seg1 = 12;
    can_config.canfd_timing.num_seg2 = 4;
    can_config.canfd_timing.num_sjw = 4;
    can_config.canfd_timing.prescaler = 1;
#endif
    can_config.mode = can_mode_normal;
    can_config.enable_canfd = true;
    can_config.enable_tdc = true;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = can_payload_size_64;
    tx_buf.bitrate_switch = 1;
    tx_buf.canfd_frame = 1;

    uint32_t msg_len = can_get_payload_size_from_dlc((uint8_t) tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = (uint8_t) i;
    }

    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        status = can_send_message_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent message failed, error_code:%d\n", status);
            return;
        }
    }

    printf("Sent CANFD messages with ID from 0 to 2047 out\n");
}