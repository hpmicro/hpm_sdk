/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates all supported CAN filter types.
 *
 * Each test case is implemented as a separate static function and called in
 * sequence by board_can_filter_test():
 *
 *   can_filter_test_case0() - Classic filter on CAN STD ID (mask: bit0-2)
 *   can_filter_test_case1() - Classic filter with exact STD ID matching (16 individual IDs)
 *   can_filter_test_case2() - Range filter on CAN STD ID (ID 0..15)
 *   can_filter_test_case3() - Dual-ID filter on CAN STD ID (0x22 and 0x33)
 *   can_filter_test_case4() - Dual-ID filter on CAN STD ID (repeat, same IDs)
 *   can_filter_test_case5() - Classic filter on CAN EXT ID (mask: bit0-2)
 *
 * NOTE: The 'mask' field in mcan_filter_elem_t uses bit-value 1 to mean
 *       "this bit participates in the ID comparison" and 0 to mean "ignored".
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_has_new_rcv_msg;
static volatile bool s_has_sent_out;
static volatile mcan_rx_message_t s_rx_buf;

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles RXFIFO0 new-message and TX-complete events; updates the file-private
 * s_has_new_rcv_msg / s_has_sent_out flags used by each filter test case.
 * -------------------------------------------------------------------------- */
static void filter_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0U) {
        mcan_read_rxfifo(ptr, 0, (mcan_rx_message_t *) &s_rx_buf);
        s_has_new_rcv_msg = true;
    }
    if ((flags & MCAN_EVENT_TRANSMIT) != 0U) {
        s_has_sent_out = true;
    }
    mcan_clear_interrupt_flags(ptr, flags);
}

/* --------------------------------------------------------------------------
 * Shared init helper
 *
 * Performs the board-level and peripheral-level initialisation that is
 * identical for every filter test case.  Returns the CAN source clock
 * frequency, or 0 if the AHB-RAM message buffer setup fails.
 * -------------------------------------------------------------------------- */
static uint32_t filter_test_init(MCAN_Type *ptr, mcan_config_t *can_config)
{
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return 0;
    }
    mcan_get_default_config(ptr, can_config);
    board_init_can(ptr);
    return board_init_can_clock(ptr);
}

/* --------------------------------------------------------------------------
 * Case 0: Classic Filter – only check bit0-2 of CAN STD ID
 *
 * One classic filter with filter_id=0 and filter_mask=7 is installed so that
 * only frames whose STD ID has all three LSBs equal to zero are accepted.
 * Out of 2048 IDs (0x000-0x7FF) exactly 256 IDs have bit[2:0]==0, so the
 * expected receive count is 256.
 * -------------------------------------------------------------------------- */
static void can_filter_test_case0(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Filter test case 0: Classic Filter - only check bit0-2 of CAN STD ID\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.filter_id     = 0;
    /* filter_mask: a set bit means that bit position of the ID participates
     * in the comparison.  0x7 = 0b111 means only the three LSBs are checked;
     * all other bits are ignored.  Frames where ID[2:0] == 0 will match. */
    can_filter.filter_mask   = 7;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
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
    can_config.interrupt_mask              = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    can_config.txbuf_trans_interrupt_mask  = ~0UL;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_has_sent_out    = false;
    s_has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!s_has_sent_out) {
        }
        s_has_sent_out = false;
        board_delay_ms(1);
        if (s_has_new_rcv_msg) {
            rcv_msg_cnt++;
            s_has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 256) ? "PASSED" : "Failed");
}

/* --------------------------------------------------------------------------
 * Case 1: Classic Filter – only accept messages with specific STD IDs
 *
 * 16 classic filters with exact-match masks (0x7FF) are installed, one for
 * each of the IDs 0x00, 0x11, 0x22, ..., 0xFF.  Exactly 16 of the 2048
 * transmitted frames should be received.
 * -------------------------------------------------------------------------- */
static void can_filter_test_case1(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Filter test case 1: Classic Filter - only accept message with specified CAN STD ID\n");

    mcan_filter_elem_t can_filters[16];
    for (uint32_t i = 0; i < ARRAY_SIZE(can_filters); i++) {
        can_filters[i].filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
        can_filters[i].filter_config = MCAN_FILTER_ELEM_CFG_SET_PRIORITY_AND_STORE_IN_FIFO0_IF_MATCH;
        can_filters[i].filter_id     = (i << 4) | i;
        can_filters[i].filter_mask   = 0x7FFUL;
        can_filters[i].can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    }

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list    = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = ARRAY_SIZE(can_filters);
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;
    can_config.interrupt_mask             = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    can_config.txbuf_trans_interrupt_mask = ~0UL;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id   = i;
        s_has_sent_out  = false;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!s_has_sent_out) {
        }
        board_delay_ms(1);
        if (s_has_new_rcv_msg) {
            rcv_msg_cnt++;
            s_has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n",
           rcv_msg_cnt,
           (rcv_msg_cnt == ARRAY_SIZE(can_filters)) ? "PASSED" : "Failed");
}

/* --------------------------------------------------------------------------
 * Case 2: Range Filter – CAN STD ID in range [0, 15]
 *
 * A single range filter accepts only frames with STD ID in [0, 15].  Exactly
 * 16 of the 2048 transmitted frames (IDs 0x000-0x7FF) should be received.
 * -------------------------------------------------------------------------- */
static void can_filter_test_case2(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Filter test case 2: Range Filter - CAN STD ID\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_RANGE_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.start_id      = 0;
    can_filter.end_id        = 15;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
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
    can_config.interrupt_mask             = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    can_config.txbuf_trans_interrupt_mask = ~0UL;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_has_sent_out    = false;
    s_has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!s_has_sent_out) {
        }
        s_has_sent_out = false;
        board_delay_ms(1);
        if (s_has_new_rcv_msg) {
            rcv_msg_cnt++;
            s_has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 16) ? "PASSED" : "Failed");
}

/* --------------------------------------------------------------------------
 * Case 3: Dual-ID Filter – CAN STD ID (accept 0x22 and 0x33)
 *
 * A single "specified ID" filter element lists exactly two IDs (0x22 and
 * 0x33).  Exactly 2 of the 2048 transmitted frames should be received.
 * -------------------------------------------------------------------------- */
static void can_filter_test_case3(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Filter test case 3: Dual ID Filter - CAN STD ID\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.id1           = 0x22;
    can_filter.id2           = 0x33;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
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
    can_config.interrupt_mask             = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    can_config.txbuf_trans_interrupt_mask = ~0UL;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_has_sent_out    = false;
    s_has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!s_has_sent_out) {
        }
        s_has_sent_out = false;
        board_delay_ms(1);
        if (s_has_new_rcv_msg) {
            rcv_msg_cnt++;
            s_has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 2) ? "PASSED" : "Failed");
}

/* --------------------------------------------------------------------------
 * Case 4: Dual-ID Filter – CAN STD ID (same IDs, repeated verification)
 *
 * Identical filter setup to case 3 (0x22 and 0x33).  Run again to verify
 * that the MCAN hardware consistently produces the same result across
 * back-to-back re-initializations.
 * -------------------------------------------------------------------------- */
static void can_filter_test_case4(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Filter test case 4: Dual ID Filter - CAN STD ID\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_STANDARD;
    can_filter.id1           = 0x22;
    can_filter.id2           = 0x33;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
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
    can_config.interrupt_mask             = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    can_config.txbuf_trans_interrupt_mask = ~0UL;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_has_sent_out    = false;
    s_has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!s_has_sent_out) {
        }
        s_has_sent_out = false;
        board_delay_ms(1);
        if (s_has_new_rcv_msg) {
            rcv_msg_cnt++;
            s_has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 2) ? "PASSED" : "Failed");
}

/* --------------------------------------------------------------------------
 * Case 5: Classic Filter – only check bit0-2 of CAN EXT ID
 *
 * One classic filter with filter_id=0 and filter_mask=7 is installed for
 * extended IDs so that only frames whose EXT ID has all three LSBs equal to
 * zero are accepted.  Out of 2048 IDs in range [0x800, 0xFFF] exactly 256
 * have bit[2:0]==0, so the expected receive count is 256.
 * -------------------------------------------------------------------------- */
static void can_filter_test_case5(MCAN_Type *ptr, uint32_t can_src_clk_freq)
{
    printf("CAN Filter test case 5: Classic Filter - only check bit0-2 of CAN EXT ID\n");

    mcan_filter_elem_t can_filter;
    can_filter.filter_type   = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filter.filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filter.can_id_type   = MCAN_CAN_ID_TYPE_EXTENDED;
    can_filter.filter_id     = 0;
    can_filter.filter_mask   = 7;

    mcan_config_t can_config;
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode     = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 0;
    can_config.all_filters_config.ext_id_filter_list.filter_elem_list    = &can_filter;
    can_config.all_filters_config.ext_id_filter_list.mcan_filter_elem_count = 1;
    /* ext_id_mask is a secondary 29-bit AND mask applied before the filter
     * comparison.  Setting all 29 bits means the full extended ID is presented
     * to the filter with no masking, so the filter_mask alone controls which
     * bits are checked. */
    can_config.all_filters_config.ext_id_mask = (1UL << 29) - 1UL;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;
    can_config.interrupt_mask             = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    can_config.txbuf_trans_interrupt_mask = ~0UL;

    hpm_stat_t status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc        = 8;
    tx_buf.use_ext_id = 1;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    s_has_sent_out    = false;
    s_has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0x800; i < 0x1000; i++) {
        tx_buf.ext_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!s_has_sent_out) {
        }
        s_has_sent_out = false;
        board_delay_ms(1);
        if (s_has_new_rcv_msg) {
            rcv_msg_cnt++;
            s_has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_rx_buf.ext_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 256) ? "PASSED" : "Failed");
}

/* --------------------------------------------------------------------------
 * Public entry point
 * -------------------------------------------------------------------------- */

void board_can_filter_test(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    uint32_t can_src_clk_freq = filter_test_init(ptr, &can_config);
    if (can_src_clk_freq == 0) {
        return;
    }

    can_filter_test_case0(ptr, can_src_clk_freq);
    can_filter_test_case1(ptr, can_src_clk_freq);
    can_filter_test_case2(ptr, can_src_clk_freq);
    can_filter_test_case3(ptr, can_src_clk_freq);
    can_filter_test_case4(ptr, can_src_clk_freq);
    can_filter_test_case5(ptr, can_src_clk_freq);
}
