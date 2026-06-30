/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"

static volatile bool has_new_rcv_msg;
static volatile can_receive_buf_t s_can_rx_buf;

static void can_echo_isr(CAN_Type *ptr)
{
    uint8_t flags = can_get_tx_rx_flags(ptr);

    if ((flags & CAN_EVENT_RECEIVE) != 0U) {
        can_read_received_message(ptr, (can_receive_buf_t *) &s_can_rx_buf);
        has_new_rcv_msg = true;
    }

    can_clear_tx_rx_flags(ptr, flags);
}

void board_can_echo_test_initiator(void)
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

    has_new_rcv_msg = false;
    can_install_isr_callback(can_echo_isr);
    can_enable_tx_rx_irq(ptr, CAN_EVENT_RECEIVE);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = 8;
    tx_buf.id = 0x123;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    printf("Can Echo test: Initiator is sending message out...\n");
    status = can_send_message_blocking(ptr, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }

    printf("Waiting for echo message...\n");
    while (!has_new_rcv_msg) {
    }

    has_new_rcv_msg = false;
    show_received_can_message((const can_receive_buf_t *) &s_can_rx_buf);
}

void board_can_echo_test_responder(void)
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

    has_new_rcv_msg = false;
    can_install_isr_callback(can_echo_isr);
    can_enable_tx_rx_irq(ptr, CAN_EVENT_RECEIVE);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    printf("CAN echo test: Responder is waiting for echo message...\n");

    while (!has_new_rcv_msg) {
    }

    has_new_rcv_msg = false;
    show_received_can_message((const can_receive_buf_t *) &s_can_rx_buf);

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = s_can_rx_buf.dlc;
    tx_buf.id = 0x321;
    uint32_t msg_len = can_get_payload_size_from_dlc((uint8_t) s_can_rx_buf.dlc);
    memcpy(tx_buf.data, (const void *) s_can_rx_buf.data, msg_len);

    status = can_send_message_blocking(ptr, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }

    printf("Sent echo message back\n");
}