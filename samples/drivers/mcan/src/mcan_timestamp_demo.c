/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates three CAN timestamp capture modes.
 *
 * Each mode is implemented as a separate static function and called in
 * sequence by board_can_timestamp_test():
 *
 *   can_timestamp_test_case0() - 32-bit TSU timestamp using the Timestamp Unit
 *                                (TSU) with an internal time-base, prescaler=256.
 *                                Sync-message filter on STD ID 0x123.
 *
 *   can_timestamp_test_case1() - 64-bit TSU timestamp (extended precision,
 *                                prescaler=1).  Sync-message filter on STD ID 0x456.
 *
 *   can_timestamp_test_case2() - Internal 16-bit timestamp counter (no TSU,
 *                                MCAN_TIMESTAMP_SEL_VALUE_INCREMENT, prescaler=16).
 *                                Sync-message filter on STD ID 0x321.
 *
 * For every case, both the TX-event timestamp and the RX-message timestamp
 * are printed so the user can verify that the two match.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_tx_event_occurred;
static volatile bool s_has_new_rcv_msg;
static volatile mcan_tx_event_fifo_elem_t s_tx_evt;
static volatile mcan_rx_message_t s_rx_buf;

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles TX-event-FIFO new-entry and RXFIFO0 new-message events; updates
 * s_tx_event_occurred / s_has_new_rcv_msg and copies the event/frame into the
 * file-private shadow buffers for consumption by the timestamp test cases.
 * -------------------------------------------------------------------------- */
static void timestamp_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);
    if ((flags & MCAN_INT_TX_EVT_FIFO_NEW_ENTRY) != 0U) {
        mcan_read_tx_evt_fifo(ptr, (mcan_tx_event_fifo_elem_t *) &s_tx_evt);
        s_tx_event_occurred = true;
    }
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0U) {
        mcan_read_rxfifo(ptr, 0, (mcan_rx_message_t *) &s_rx_buf);
        s_has_new_rcv_msg = true;
    }
    mcan_clear_interrupt_flags(ptr, flags);
}

/* --------------------------------------------------------------------------
 * Shared init helper
 *
 * Performs board-level initialisation and returns the CAN source clock
 * frequency.  Returns 0 if the AHB-RAM message-buffer attribute setup fails.
 * -------------------------------------------------------------------------- */
static uint32_t timestamp_test_init(MCAN_Type *ptr)
{
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return 0;
    }
    board_init_can(ptr);
    return board_init_can_clock(ptr);
}

/* --------------------------------------------------------------------------
 * Case 0: 32-bit TSU Timestamp
 *
 * Configures the TSU with prescaler=256 using the internal time-base.
 * The MCAN timestamp register is set to MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED
 * so that each captured timestamp comes from the TSU's 32-bit counter.
 * A single loopback frame (STD ID 0x123, DLC=8) is sent; the TX-event FIFO
 * and RX-FIFO0 timestamps are printed for comparison.
 * -------------------------------------------------------------------------- */
static void can_timestamp_test_case0(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Timestamp Test Case 0: Use 32-bit TSU Timestamp\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.sync_message  = 1U;
    can_filter.filter_id     = 0x123;
    can_filter.filter_mask   = 0x3FF;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);

    /* TSU configuration: 32-bit mode, internal time-base, prescaler=256 */
    can_config.use_timestamping_unit           = true;
    can_config.tsu_config.prescaler            = 256U;
    can_config.tsu_config.enable_tsu           = true;
    can_config.tsu_config.capture_on_sof       = false;
    can_config.tsu_config.use_ext_timebase     = false;

    can_config.timestamp_cfg.counter_prescaler = 16U;
    /* MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED routes the TSU counter value into
     * the timestamp field of every captured frame instead of the MCAN's own
     * 16-bit internal counter. */
    can_config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
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
    uint32_t int_mask = MCAN_INT_TX_EVT_FIFO_NEW_ENTRY | MCAN_EVENT_RECEIVE;
    mcan_enable_interrupts(ptr, int_mask);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0UL);
    mcan_install_isr_callback(timestamp_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc                    = 8;
    tx_buf.message_marker_l       = 0x00;
    /* timestamp_capture_enable = true causes the hardware to latch the
     * current timestamp into this frame's TX-event FIFO entry and into
     * the corresponding RX message element when it is received. */
    tx_buf.timestamp_capture_enable = true;
    tx_buf.event_fifo_control     = 1U;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_tx_event_occurred = false;
    s_has_new_rcv_msg   = false;
    /* sync_message = 1 in the filter element triggers a TSU capture when this
     * frame's ID matches.  The captured value is read back via
     * mcan_get_timestamp_from_tx_event() / mcan_get_timestamp_from_received_message(). */
    tx_buf.std_id     = 0x123;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!s_tx_event_occurred) {
    }
    s_tx_event_occurred = false;
    while (!s_has_new_rcv_msg) {
    }

    mcan_timestamp_value_t ts_val;
    if (s_tx_evt.tx_timestamp_captured) {
        mcan_get_timestamp_from_tx_event(ptr, (const mcan_tx_event_fifo_elem_t *) &s_tx_evt, &ts_val);
        printf("TX Timestamp for ID:0x%03x is %s\n", tx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    if (s_rx_buf.rx_timestamp_captured != 0U) {
        mcan_get_timestamp_from_received_message(ptr, (const mcan_rx_message_t *) &s_rx_buf, &ts_val);
        printf("RX Timestamp for ID:0x%03x is %s\n", s_rx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    s_has_new_rcv_msg = false;
    printf("New message received, ID=%08x\n", s_rx_buf.std_id);
}

/* --------------------------------------------------------------------------
 * Case 1: 64-bit TSU Timestamp
 *
 * Configures the TSU with prescaler=1 and 64-bit mode enabled.  Extended
 * precision allows sub-nanosecond timestamping at high CAN clock rates.
 * Sync-message filter on STD ID 0x456.  Same TX/RX print as case 0.
 * -------------------------------------------------------------------------- */
static void can_timestamp_test_case1(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Timestamp Test Case 1: Use 64-bit TSU Timestamp\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.sync_message  = 1U;
    can_filter.filter_id     = 0x456;
    can_filter.filter_mask   = 0x3FF;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);

    /* TSU configuration: 64-bit mode, internal time-base, prescaler=1 */
    can_config.use_timestamping_unit                = true;
    can_config.tsu_config.prescaler                 = 1U;
    can_config.tsu_config.enable_tsu                = true;
    can_config.tsu_config.capture_on_sof            = false;
    can_config.tsu_config.use_ext_timebase          = false;
    can_config.tsu_config.enable_64bit_timestamp    = true;

    can_config.timestamp_cfg.counter_prescaler     = 1;
    can_config.timestamp_cfg.timestamp_selection   = MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
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
    uint32_t int_mask = MCAN_INT_TX_EVT_FIFO_NEW_ENTRY | MCAN_EVENT_RECEIVE;
    mcan_enable_interrupts(ptr, int_mask);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0UL);
    mcan_install_isr_callback(timestamp_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc                    = 8;
    tx_buf.message_marker_l       = 0x01;
    tx_buf.timestamp_capture_enable = true;
    tx_buf.event_fifo_control     = 1U;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_tx_event_occurred = false;
    s_has_new_rcv_msg   = false;
    tx_buf.std_id     = 0x456;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!s_tx_event_occurred) {
    }
    s_tx_event_occurred = false;
    while (!s_has_new_rcv_msg) {
    }

    mcan_timestamp_value_t ts_val;
    if (s_tx_evt.tx_timestamp_captured) {
        mcan_get_timestamp_from_tx_event(ptr, (const mcan_tx_event_fifo_elem_t *) &s_tx_evt, &ts_val);
        printf("TX Timestamp for ID:0x%03x is %s\n", tx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    if (s_rx_buf.rx_timestamp_captured != 0U) {
        mcan_get_timestamp_from_received_message(ptr, (const mcan_rx_message_t *) &s_rx_buf, &ts_val);
        printf("RX Timestamp for ID:0x%03x is %s\n", s_rx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    s_has_new_rcv_msg = false;
    printf("New message received, ID=%08x\n", s_rx_buf.std_id);
}

/* --------------------------------------------------------------------------
 * Case 2: Internal 16-bit Timestamp Counter
 *
 * Uses the MCAN's built-in 16-bit timestamp counter instead of the TSU.
 * The counter increments on each CAN bit period scaled by counter_prescaler.
 * Sync-message filter on STD ID 0x321.  mcan_deinit() is called at the end
 * to release the peripheral.
 * -------------------------------------------------------------------------- */
static void can_timestamp_test_case2(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Timestamp Test Case 2: Use Internal 16-bit Timestamp\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.sync_message  = 1U;
    can_filter.filter_id     = 0x321;
    can_filter.filter_mask   = 0x3FF;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);

    /* No TSU – use the internal 16-bit counter (prescaler=16) */
    can_config.use_timestamping_unit                = false;
    can_config.tsu_config.prescaler                 = 1U;
    can_config.tsu_config.enable_tsu                = false;
    can_config.tsu_config.capture_on_sof            = false;
    can_config.tsu_config.use_ext_timebase          = false;

    can_config.timestamp_cfg.counter_prescaler     = 16U;
    can_config.timestamp_cfg.timestamp_selection   = MCAN_TIMESTAMP_SEL_VALUE_INCREMENT;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
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
    uint32_t int_mask = MCAN_EVENT_RECEIVE | MCAN_INT_TX_EVT_FIFO_NEW_ENTRY;
    mcan_enable_interrupts(ptr, int_mask);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0UL);
    mcan_install_isr_callback(timestamp_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc                    = 8;
    tx_buf.message_marker_l       = 0x02;
    tx_buf.event_fifo_control     = 1U;
    tx_buf.timestamp_capture_enable = true;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_tx_event_occurred = false;
    s_has_new_rcv_msg   = false;
    tx_buf.std_id     = 0x321;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!s_tx_event_occurred) {
    }
    s_tx_event_occurred = false;
    while (!s_has_new_rcv_msg) {
    }

    mcan_timestamp_value_t ts_val;
    /* No guard on mcan_get_timestamp_from_tx_event: the internal 16-bit
     * counter always attaches a timestamp, so tx_timestamp_captured is
     * always true for case 2.  Same reasoning applies to the RX side. */
    mcan_get_timestamp_from_tx_event(ptr, (const mcan_tx_event_fifo_elem_t *) &s_tx_evt, &ts_val);
    printf("TX Timestamp for ID:0x%03x is %s\n", tx_buf.std_id, get_timestamp_hex_string(&ts_val));

    mcan_get_timestamp_from_received_message(ptr, (const mcan_rx_message_t *) &s_rx_buf, &ts_val);
    printf("RX Timestamp for ID:0x%03x is %s\n", s_rx_buf.std_id, get_timestamp_hex_string(&ts_val));

    mcan_deinit(ptr);
}

/* --------------------------------------------------------------------------
 * Public entry point
 * -------------------------------------------------------------------------- */

void board_can_timestamp_test(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    uint32_t can_src_clk_freq = timestamp_test_init(ptr);
    if (can_src_clk_freq == 0) {
        return;
    }

    can_timestamp_test_case0(ptr, can_src_clk_freq);
    can_timestamp_test_case1(ptr, can_src_clk_freq);
    can_timestamp_test_case2(ptr, can_src_clk_freq);
}
