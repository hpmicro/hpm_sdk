/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"
#include "hpm_ptpc_drv.h"

static volatile bool has_new_rcv_msg;
static volatile bool has_sent_out;
static volatile can_receive_buf_t s_can_rx_buf;

static void can_timestamp_isr(CAN_Type *ptr)
{
    uint8_t flags = can_get_tx_rx_flags(ptr);

    if ((flags & CAN_EVENT_RECEIVE) != 0U) {
        can_read_received_message(ptr, (can_receive_buf_t *) &s_can_rx_buf);
        has_new_rcv_msg = true;
    }

    if ((flags & (CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF)) != 0U) {
        has_sent_out = true;
    }

    can_clear_tx_rx_flags(ptr, flags);
}

void board_can_timestamp_test(void)
{
    clock_add_to_group(clock_ptpc, 0);

    ptpc_config_t ptpc_cfg;
    static bool ptpc_inited = false;
    if (!ptpc_inited) {
        ptpc_get_default_config(HPM_PTPC, &ptpc_cfg);
        ptpc_cfg.src_frequency = clock_get_frequency(clock_ptpc);
        ptpc_cfg.capture_keep = false;
        ptpc_cfg.coarse_increment = false;
        ptpc_init(HPM_PTPC, 0, &ptpc_cfg);
        ptpc_init_timer(HPM_PTPC, PTPC_PTPC_0);
        ptpc_inited = true;
    }

    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 1000000;
    can_config.mode = can_mode_loopback_internal;
    can_config.time_stamping_position = CAN_TIME_STAMPING_POSITION_EOF;
    can_config.enable_time_stamping = true;
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    has_new_rcv_msg = false;
    has_sent_out = false;
    can_install_isr_callback(can_timestamp_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.transmit_timestamp_enable = 1;
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    tx_buf.id = 0x123;
    can_send_message_nonblocking(ptr, &tx_buf);
    while (!has_sent_out) {
    }

    can_timestamp_value_t timestamp;
    status = can_get_timestamp_for_transmitted_message(ptr, &timestamp);
    if (status != status_success) {
        printf("Failed to get timestamp for transmitted CAN message\n");
        return;
    }
    printf("Timestamp for transmitted CAN message is %u.%u second\n", timestamp.second, timestamp.nano_sec);

    while (!has_new_rcv_msg) {
    }
    status = can_get_timestamp_from_received_message(ptr, (const can_receive_buf_t *) &s_can_rx_buf, &timestamp);
    if (status != status_success) {
        printf("Failed to get timestamp for received CAN message\n");
        return;
    }
    printf("Timestamp for received CAN message is %u.%u second\n", timestamp.second, timestamp.nano_sec);
    has_new_rcv_msg = false;
    has_sent_out = false;

    can_get_default_config(&can_config);
    can_config.baudrate = 500000;
    can_config.baudrate_fd = 2000000;
    can_config.enable_canfd = true;
    can_config.mode = can_mode_loopback_internal;
    can_config.time_stamping_position = CAN_TIME_STAMPING_POSITION_EOF;
    can_config.enable_time_stamping = true;
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF;

    board_init_can(ptr);
    status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    tx_buf = (can_transmit_buf_t) { 0 };
    tx_buf.transmit_timestamp_enable = 1;
    tx_buf.bitrate_switch = 1;
    tx_buf.canfd_frame = 1;
    tx_buf.extend_id = 1;
    tx_buf.id = 0x123456;
    tx_buf.dlc = can_payload_size_64;

    for (uint32_t i = 0; i < 64; i++) {
        tx_buf.data[i] = (uint8_t) i;
    }

    can_send_message_nonblocking(ptr, &tx_buf);
    while (!has_sent_out) {
    }
    status = can_get_timestamp_for_transmitted_message(ptr, &timestamp);
    if (status != status_success) {
        printf("Failed to get timestamp for transmitted CANFD message\n");
        return;
    }
    printf("Timestamp for transmitted CANFD message is %u.%u second\n", timestamp.second, timestamp.nano_sec);

    while (!has_new_rcv_msg) {
    }
    status = can_get_timestamp_from_received_message(ptr, (const can_receive_buf_t *) &s_can_rx_buf, &timestamp);
    if (status != status_success) {
        printf("Failed to get timestamp for received CANFD message\n");
        return;
    }
    printf("Timestamp for received CANFD message is %u.%u second\n", timestamp.second, timestamp.nano_sec);
    has_new_rcv_msg = false;
    has_sent_out = false;
}