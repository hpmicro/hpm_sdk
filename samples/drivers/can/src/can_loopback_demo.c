/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"

static volatile bool has_new_rcv_msg;
static volatile bool has_sent_out;
static volatile can_receive_buf_t s_can_rx_buf;

static void can_loopback_isr(CAN_Type *ptr)
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

bool can_loopback_test(CAN_Type *base)
{
    uint32_t error_cnt = 0;
    bool result = false;
    can_transmit_buf_t tx_buf = { 0 };
    can_receive_buf_t rx_buf = { 0 };

    tx_buf.id = 0x7f;
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);
    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CAN loopback test for standard frame %s\n", result ? "passed" : "failed");

    tx_buf.extend_id = 1U;
    tx_buf.id = 0x12345678U;

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);

    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CAN loopback test for extend frame %s\n", result ? "passed" : "failed");

    return (error_cnt < 1U);
}

bool canfd_loopback_test(CAN_Type *base)
{
    uint32_t error_cnt = 0;
    bool result = false;
    can_transmit_buf_t tx_buf = { 0 };
    can_receive_buf_t rx_buf = { 0 };

    tx_buf.id = 0x7f;
    tx_buf.dlc = can_payload_size_64;
    tx_buf.bitrate_switch = 1;
    tx_buf.canfd_frame = 1;

    uint32_t msg_bytes = can_get_payload_size_from_dlc((uint8_t) tx_buf.dlc);
    for (uint32_t i = 0; i < msg_bytes; i++) {
        tx_buf.data[i] = (uint8_t) i;
    }

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);
    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CANFD loopback test for standard frame %s\n", result ? "passed" : "failed");

    tx_buf.extend_id = 1U;
    tx_buf.id = 0x12345678U;

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);

    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CANFD loopback test for extend frame %s\n", result ? "passed" : "failed");

    return (error_cnt < 1U);
}

void can_loopback_test_for_all_cans(void)
{
    can_config_t can_config;
    hpm_stat_t status;

    for (uint32_t i = 0; i < g_can_info_count; i++) {
        can_get_default_config(&can_config);
        can_config.baudrate = 1000000;
        can_config.mode = can_mode_loopback_internal;
        status = can_init(s_can_info[i].can_base, &can_config, s_can_info[i].clock_freq);
        assert(status == status_success);
        intc_m_disable_irq(s_can_info[i].irq_index);
        (void) status;

        bool can_result = can_loopback_test(s_can_info[i].can_base);

        can_config.baudrate_fd = 2000000;
        can_config.enable_canfd = true;
        status = can_init(s_can_info[i].can_base, &can_config, s_can_info[i].clock_freq);
        assert(status == status_success);

        bool canfd_result = canfd_loopback_test(s_can_info[i].can_base);
        printf("    CAN%d loopback test %s\n", i, (can_result && canfd_result) ? "PASSED" : "FAILED");
    }
}

void board_can_loopback_test_in_interrupt_mode(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 1000000;
    can_config.mode = can_mode_loopback_internal;
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
    can_install_isr_callback(can_loopback_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        can_send_message_nonblocking(ptr, &tx_buf);
        while (!has_sent_out) {
        }
        while (!has_new_rcv_msg) {
        }

        has_new_rcv_msg = false;
        has_sent_out = false;
        printf("New message received, ID=%08x\n", s_can_rx_buf.id);
    }
}