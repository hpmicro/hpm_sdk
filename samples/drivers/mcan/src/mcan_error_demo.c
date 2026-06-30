/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file implements CAN error-related functionality.
 *
 * handle_can_error()       - Decodes and prints the current protocol error state
 *                            from the PSR register.
 *
 * board_can_error_test()   - Triggers both CAN 2.0 and CANFD error scenarios by
 *                            attempting transmission with no other node present on
 *                            the bus (no-ACK condition).  Implemented as two
 *                            separate static sub-functions:
 *
 *   can_error_test_can20()   - CAN 2.0B, 500 kbps, DLC=8
 *   can_error_test_canfd()   - CANFD, 500 kbps nominal / 2 Mbps data, DLC=15
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_has_sent_out;
static volatile bool s_has_error;

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles TX-complete and CAN error events; updates the file-private
 * s_has_sent_out / s_has_error flags consumed by the error test cases.
 * -------------------------------------------------------------------------- */
static void error_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);
    if ((flags & MCAN_EVENT_TRANSMIT) != 0U) {
        s_has_sent_out = true;
    }
    if ((flags & MCAN_EVENT_ERROR) != 0U) {
        s_has_error = true;
    }
    mcan_clear_interrupt_flags(ptr, flags);
}

/* --------------------------------------------------------------------------
 * handle_can_error – decode and print PSR/ECR state
 * -------------------------------------------------------------------------- */

void handle_can_error(MCAN_Type *ptr)
{
    mcan_protocol_status_t protocol_status;
    mcan_error_count_t error_count;
    mcan_parse_protocol_status(ptr->PSR, &protocol_status);
    mcan_get_error_counter(ptr, &error_count);
    const char *error_msg = NULL;
    switch (protocol_status.last_error_code) {
    case mcan_last_error_code_no_error:
        error_msg = "No Error";
        break;
    case mcan_last_error_code_stuff_error:
        error_msg = "Stuff Error";
        break;
    case mcan_last_error_code_format_error:
        error_msg = "Format Error";
        break;
    case mcan_last_error_code_ack_error:
        error_msg = "Acknowledge Error";
        break;
    case mcan_last_error_code_bit1_error:
        error_msg = "Bit1 Error: Sent 1 but monitored as 0";
        break;
    case mcan_last_error_code_bit0_error:
        error_msg = "Bit0 Error: Sent 0 but monitored as 1";
        break;
    case mcan_last_error_code_crc_error:
        error_msg = "CRC Error";
        break;
    case mcan_last_error_code_no_change:
        error_msg = "Last Error was not changed";
        break;
    default:
        /* Suppress compiling warning */
        break;
    }
    printf("Last Error: %s\n", error_msg);
    printf("Error Count:\n");
    printf("Transmit Errors: %d\n", error_count.transmit_error_count);
    printf("Receive Errors: %d\n", error_count.receive_error_count);
    if (protocol_status.in_bus_off_state) {
        printf("CAN is in Bus-off mode\n");
    } else if (protocol_status.in_error_passive_state) {
        printf("CAN is in Error Passive Mode\n");
    } else if (protocol_status.in_warning_state) {
        printf("CAN is in Error Warning mode\n");
    } else {
        /* Suppress warnings*/
    }
}

/* --------------------------------------------------------------------------
 * Shared init helper
 *
 * Configures the board and returns the CAN source clock frequency.
 * Returns 0 when the AHB-RAM message-buffer attribute setup fails.
 * -------------------------------------------------------------------------- */
static uint32_t error_test_init(MCAN_Type *base, mcan_config_t *can_config, uint32_t interrupt_mask)
{
    if (mcan_setup_msg_buf(base) != status_success) {
        return 0;
    }
    mcan_get_default_config(base, can_config);
    board_init_can(base);
    uint32_t can_src_clk_freq = board_init_can_clock(base);
    can_config->interrupt_mask             = interrupt_mask;
    can_config->txbuf_trans_interrupt_mask = ~0UL;
    return can_src_clk_freq;
}

/* --------------------------------------------------------------------------
 * Case 1: CAN 2.0B error test
 *
 * Transmits a standard 8-byte CAN 2.0B frame in normal mode with no peer
 * node on the bus.  The missing ACK causes an acknowledge error.  The PSR
 * and ECR registers are printed via handle_can_error() so the developer can
 * observe the error counters and protocol state.
 * -------------------------------------------------------------------------- */
static void can_error_test_can20(MCAN_Type *base)
{
    mcan_config_t can_config;
    uint32_t interrupt_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT | MCAN_EVENT_ERROR;

    uint32_t can_src_clk_freq = error_test_init(base, &can_config, interrupt_mask);
    if (can_src_clk_freq == 0) {
        return;
    }

    can_config.baudrate = 500000; /* 500 kbps */
    can_config.mode     = mcan_mode_normal;

    hpm_stat_t status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(error_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    s_has_sent_out = false;
    s_has_error    = false;

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }
    tx_buf.std_id = 0x123;
    mcan_transmit_via_txbuf_nonblocking(base, 0, &tx_buf);
    while ((!s_has_sent_out) && (!s_has_error)) {
    }
    if (s_has_error) {
        printf("can error happened\n");
    } else {
        printf("no can error happened\n");
    }
    handle_can_error(base);
}

/* --------------------------------------------------------------------------
 * Case 2: CANFD error test
 *
 * Same scenario as case 1 but uses a CANFD frame with BRS enabled (DLC=15,
 * 64 bytes).  Nominal bitrate 500 kbps / data bitrate 2 Mbps.
 * -------------------------------------------------------------------------- */
static void can_error_test_canfd(MCAN_Type *base)
{
    mcan_config_t can_config;
    uint32_t interrupt_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT | MCAN_EVENT_ERROR;

    uint32_t can_src_clk_freq = error_test_init(base, &can_config, interrupt_mask);
    if (can_src_clk_freq == 0) {
        return;
    }

    can_config.baudrate    = 500000;  /* 500 kbps nominal */
    can_config.baudrate_fd = 2000000; /* 2 Mbps data */
    can_config.enable_canfd = true;
    mcan_get_default_ram_config(base, &can_config.ram_config, true);
    can_config.mode = mcan_mode_normal;

    hpm_stat_t status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(error_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    s_has_sent_out = false;
    s_has_error    = false;

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc           = 15;
    tx_buf.canfd_frame   = true;
    tx_buf.bitrate_switch = true;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }
    tx_buf.std_id = 0x123;
    mcan_transmit_via_txbuf_nonblocking(base, 0, &tx_buf);
    while ((!s_has_sent_out) && (!s_has_error)) {
    }
    if (s_has_error) {
        printf("can error happened\n");
    } else {
        printf("no can error happened\n");
    }
    s_has_error = false;
    handle_can_error(base);
}

/* --------------------------------------------------------------------------
 * Public entry point
 * -------------------------------------------------------------------------- */

void board_can_error_test(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;

    can_error_test_can20(base);
    can_error_test_canfd(base);
}
