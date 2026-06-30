/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file implements the two-board CAN echo tests:
 *
 *   board_can_echo_test_initiator() - Initialises CAN in normal mode, sends a
 *                                     single frame (STD ID 0x123, DLC=8), then
 *                                     waits for the echo message from the responder
 *                                     and prints its content.
 *
 *   board_can_echo_test_responder() - Initialises CAN in normal mode, waits for
 *                                     an incoming frame, prints it, then echoes
 *                                     the payload back using STD ID 0x321.
 *
 * Both nodes must be connected to the same physical CAN bus and must call
 * their respective functions at the same time.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_has_new_rcv_msg;
static volatile mcan_rx_message_t s_rx_buf;

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles RXFIFO0 new-message events; updates s_has_new_rcv_msg and copies the
 * frame into s_rx_buf for consumption by the initiator / responder.
 * -------------------------------------------------------------------------- */
static void echo_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0U) {
        mcan_read_rxfifo(ptr, 0, (mcan_rx_message_t *) &s_rx_buf);
        s_has_new_rcv_msg = true;
    }
    mcan_clear_interrupt_flags(ptr, flags);
}

/* --------------------------------------------------------------------------
 * Shared init helper
 *
 * Initializes CAN at 500 kbps in normal mode and returns status_success on
 * success.  Returns 0 if the AHB-RAM message-buffer attribute setup fails.
 * -------------------------------------------------------------------------- */
static hpm_stat_t echo_test_init(MCAN_Type *ptr, mcan_config_t *can_config)
{
    hpm_stat_t s = mcan_setup_msg_buf(ptr);
    if (s != status_success) {
        return s;
    }
    mcan_get_default_config(ptr, can_config);
    can_config->baudrate = 500000; /* 500 kbps */
    can_config->mode     = mcan_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    return mcan_init(ptr, can_config, can_src_clk_freq);
}

/* --------------------------------------------------------------------------
 * Initiator role
 *
 * Sends a test frame to the responder and waits for the echo reply.
 * -------------------------------------------------------------------------- */
void board_can_echo_test_initiator(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;

    hpm_stat_t status = echo_test_init(ptr, &can_config);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_enable_interrupts(ptr, MCAN_EVENT_RECEIVE);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0U);
    mcan_install_isr_callback(echo_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc    = 8;
    tx_buf.std_id = 0x123;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    printf("Can Echo test: Initiator is sending message out...\n");
    status = mcan_transmit_blocking(BOARD_APP_CAN_BASE, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }
    printf("Waiting for echo message...\n");
    while (!s_has_new_rcv_msg) {
    }
    s_has_new_rcv_msg = false;
    show_received_can_message((const mcan_rx_message_t *) &s_rx_buf);
}

/* --------------------------------------------------------------------------
 * Responder role
 *
 * Waits for a frame from the initiator, prints it, then echoes the payload
 * back using STD ID 0x321.
 * -------------------------------------------------------------------------- */
void board_can_echo_test_responder(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;

    hpm_stat_t status = echo_test_init(base, &can_config);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_enable_interrupts(base, MCAN_EVENT_RECEIVE);
    mcan_install_isr_callback(echo_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    printf("CAN echo test: Responder is waiting for echo message...\n");
    while (!s_has_new_rcv_msg) {
    }
    s_has_new_rcv_msg = false;
    show_received_can_message((const mcan_rx_message_t *) &s_rx_buf);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc    = s_rx_buf.dlc;
    tx_buf.std_id = 0x321;
    uint32_t msg_len = mcan_get_message_size_from_dlc(s_rx_buf.dlc);
    memcpy(&tx_buf.data_8, (uint8_t *) &s_rx_buf.data_8, msg_len);
    status = mcan_transmit_blocking(base, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }
    printf("Sent echo message back\n");
}
