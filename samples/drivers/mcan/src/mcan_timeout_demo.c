/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates the MCAN Timeout Counter feature.
 *
 * The timeout counter counts down from a configured value and fires an
 * interrupt when it reaches zero.  Each trigger mode is implemented as a
 * separate static function and called in sequence by board_can_timeout_counter_test():
 *
 *   can_timeout_test_continuous()       - Case 1: Timeout fires unconditionally
 *                                         after the configured period (continuous mode).
 *
 *   can_timeout_test_tx_evt_fifo()      - Case 2: Timeout counter is reset each
 *                                         time an entry is written into the TX Event
 *                                         FIFO; test verifies a TX-event fires before
 *                                         the timeout.
 *
 *   can_timeout_test_rx_fifo0()         - Case 3: Timeout counter is reset each
 *                                         time a frame arrives in RX FIFO0; test
 *                                         verifies an RX FIFO0 event fires before
 *                                         the timeout.
 *
 *   can_timeout_test_rx_fifo1()         - Case 4: Same as case 3 but for RX FIFO1.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_timeout_event_occurred;
static volatile bool s_rxfifo0_event_occurred;
static volatile bool s_rxfifo1_event_occurred;
static volatile bool s_tx_event_occurred;

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles timeout-occurred, RXFIFO0 new-message, RXFIFO1 new-message and
 * TX-event-FIFO new-entry events; updates the file-private flags consumed
 * by each timeout test case.
 * -------------------------------------------------------------------------- */
static void timeout_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);
    if ((flags & MCAN_INT_TIMEOUT_OCCURRED) != 0U) {
        s_timeout_event_occurred = true;
    }
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0U) {
        s_rxfifo0_event_occurred = true;
    }
    if ((flags & MCAN_INT_RXFIFO1_NEW_MSG) != 0U) {
        s_rxfifo1_event_occurred = true;
    }
    if ((flags & MCAN_INT_TX_EVT_FIFO_NEW_ENTRY) != 0U) {
        s_tx_event_occurred = true;
    }
    mcan_clear_interrupt_flags(ptr, flags);
}

/* --------------------------------------------------------------------------
 * Case 1: Continuous timeout mode
 *
 * The timeout counter runs freely from the configured value (1000 ticks) and
 * fires an interrupt as soon as it reaches zero.  No CAN traffic is required;
 * the test simply waits for the timeout interrupt flag.
 * -------------------------------------------------------------------------- */
static void can_timeout_test_continuous(MCAN_Type *ptr)
{
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
    can_config.interrupt_mask             = MCAN_INT_TIMEOUT_OCCURRED;
    can_config.txbuf_trans_interrupt_mask = ~0UL;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel            = mcan_timeout_continuous_operation;
    can_config.timeout_cfg.timeout_period         = 1000;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(timeout_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    /* Reload the counter with timeout_period and start it running.
     * Without this call the counter stays at the reset value (0) and the
     * timeout interrupt fires immediately (or not at all, depending on the
     * hardware reset state). */
    mcan_reset_timeout_counter_value(ptr);

    while (!s_timeout_event_occurred) {
    }
    mcan_deinit(ptr);
    printf("CAN Timeout test passed in Continuous mode\n");
}

/* --------------------------------------------------------------------------
 * Case 2: TX Event FIFO-triggered timeout
 *
 * The timeout counter resets whenever a new entry is stored in the TX Event
 * FIFO.  One loopback frame (STD ID 0x123, event_fifo_control=1) is sent via
 * the TX FIFO in non-blocking mode.  The test waits for the timeout interrupt
 * and checks that a TX-event interrupt also fired, confirming the sequence:
 * frame transmitted → TX event FIFO entry → counter reset → eventual timeout.
 * -------------------------------------------------------------------------- */
static void can_timeout_test_tx_evt_fifo(MCAN_Type *ptr)
{
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    s_timeout_event_occurred = false;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
    can_config.interrupt_mask             = MCAN_INT_TIMEOUT_OCCURRED;
    can_config.txbuf_trans_interrupt_mask = ~0UL;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel            = mcan_timeout_triggered_by_tx_evt_fifo;
    can_config.timeout_cfg.timeout_period         = 1000;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(timeout_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc              = 8;
    tx_buf.std_id           = 0x123;
    tx_buf.event_fifo_control = 1U; /* Store into TX Event FIFO */
    tx_buf.message_marker_l = 0x123 & 0xFF;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }
    /* Transmit via TXFIFO in non-blocking mode; the TX Event FIFO entry
     * will reset the timeout counter, and the timeout ISR fires after the
     * next countdown completes. */
    mcan_transmit_via_txfifo_nonblocking(ptr, &tx_buf, NULL);
    while (!s_timeout_event_occurred) {
    }
    if (s_tx_event_occurred) {
        printf("CAN Timeout test passed in TX EVENT FIFO triggered mode\n");
    } else {
        printf("CAN Timeout test failed in TX EVENT FIFO triggered mode\n");
    }
    s_tx_event_occurred = false;
}

/* --------------------------------------------------------------------------
 * Case 3: RX FIFO0-triggered timeout
 *
 * The timeout counter resets whenever a frame arrives in RX FIFO0.  A filter
 * is configured to route STD ID 0x123 to RX FIFO0.  One blocking loopback
 * frame is sent.  The test waits for the timeout interrupt and checks that the
 * RX FIFO0 event flag was set.
 * -------------------------------------------------------------------------- */
static void can_timeout_test_rx_fifo0(MCAN_Type *ptr)
{
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.filter_id     = 0x123;
    can_filter.filter_mask   = 0x3ff;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
    can_config.interrupt_mask             = MCAN_INT_TIMEOUT_OCCURRED;
    can_config.txbuf_trans_interrupt_mask = ~0UL;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel            = mcan_timeout_triggered_by_rx_fifo0;
    can_config.timeout_cfg.timeout_period         = 1000;
    can_config.all_filters_config.ext_id_filter_list.mcan_filter_elem_count = 0;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list    = &can_filter;
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
            MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
            MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(timeout_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc    = 8;
    tx_buf.std_id = 0x123;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }
    mcan_transmit_blocking(ptr, &tx_buf);
    /* MCAN_INT_RXFIFO0_NEW_MSG is NOT in interrupt_mask above because the
     * timeout counter ISR sets s_rxfifo0_event_occurred via the RXFIFO0
     * sub-flag inside timeout_isr().  The interrupt_mask controls which
     * interrupt lines are enabled; the sub-flags are still checked inside
     * the single registered callback. */
    while (!s_timeout_event_occurred) {
    }
    mcan_deinit(ptr);
    s_timeout_event_occurred = false;
    if (s_rxfifo0_event_occurred) {
        printf("CAN Timeout test passed in RX FIFO0 triggered mode\n");
    } else {
        printf("CAN Timeout test failed in RX FIFO0 triggered mode\n");
    }
    s_rxfifo0_event_occurred = false;
}

/* --------------------------------------------------------------------------
 * Case 4: RX FIFO1-triggered timeout
 *
 * Same as case 3 but the filter routes STD ID 0x234 to RX FIFO1 and the
 * timeout counter is configured in mcan_timeout_triggered_by_rx_fifo1 mode.
 * -------------------------------------------------------------------------- */
static void can_timeout_test_rx_fifo1(MCAN_Type *ptr)
{
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO1_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.filter_id     = 0x234;
    can_filter.filter_mask   = 0x3ff;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
    can_config.interrupt_mask             = MCAN_INT_TIMEOUT_OCCURRED;
    can_config.txbuf_trans_interrupt_mask = ~0UL;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel            = mcan_timeout_triggered_by_rx_fifo1;
    can_config.timeout_cfg.timeout_period         = 1000;
    can_config.all_filters_config.ext_id_filter_list.mcan_filter_elem_count = 0;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list    = &can_filter;
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
            MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
            MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(timeout_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc    = 8;
    tx_buf.std_id = 0x234;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!s_timeout_event_occurred) {
    }
    mcan_deinit(ptr);
    s_timeout_event_occurred = false;
    if (s_rxfifo1_event_occurred) {
        printf("CAN Timeout test passed in RX FIFO1 triggered mode\n");
    } else {
        printf("CAN Timeout test failed in RX FIFO1 triggered mode\n");
    }
    s_rxfifo1_event_occurred = false;
}

/* --------------------------------------------------------------------------
 * Public entry point
 * -------------------------------------------------------------------------- */

void board_can_timeout_counter_test(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;

    can_timeout_test_continuous(ptr);
    can_timeout_test_tx_evt_fifo(ptr);
    can_timeout_test_rx_fifo0(ptr);
    can_timeout_test_rx_fifo1(ptr);
}
