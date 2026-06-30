/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates the CAN Dedicated Receive Buffer (RXBUF) feature.
 *
 * The RXBUF must be paired with a CAN filter that routes matching frames
 * directly into the dedicated buffer rather than the RX FIFOs. In this
 * example only messages with ID 0x123 are stored into RXBUF index 1; all
 * other messages are rejected by the global filter.
 *
 * Transmission is done via TXBUF in non-blocking mode to show how the caller
 * can poll the per-buffer transmission-complete flag.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_has_new_rcv_msg;
static volatile mcan_rx_message_t s_rx_buf;

/* The dedicated RX buffer index that the filter will route matched frames into.
 * The TX buffer index used for the non-blocking transmissions in this test.
 * These two constants must not overlap with any other buffer allocations.    */
#define RXBUF_TEST_RX_BUF_INDEX  1U
#define RXBUF_TEST_TX_BUF_INDEX  2U

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles dedicated RX-buffer store events; scans all RX buffer slots,
 * copies the first unread frame into s_rx_buf, and sets s_has_new_rcv_msg.
 *
 * NOTE: This is a demonstration implementation; for production code scan
 *       only the range of buffer indices that were actually configured.
 * -------------------------------------------------------------------------- */
static void rxbuf_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);
    if ((flags & MCAN_INT_MSG_STORE_TO_RXBUF) != 0U) {
        for (uint32_t idx = 0; idx < MCAN_RXBUF_SIZE_CAN_DEFAULT; idx++) {
            if (mcan_is_rxbuf_data_available(ptr, idx)) {
                mcan_read_rxbuf(ptr, idx, (mcan_rx_message_t *) &s_rx_buf);
                mcan_clear_rxbuf_data_available_flag(ptr, idx);
            }
        }
        s_has_new_rcv_msg = true;
    }
    mcan_clear_interrupt_flags(ptr, flags);
}

void board_can_rxbuf_test(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
    mcan_filter_elem_t can_filters[16];
    if (mcan_setup_msg_buf(base) != status_success) {
        return;
    }
    mcan_get_default_config(base, &can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = mcan_mode_loopback_internal;
    board_init_can(base);
    uint32_t can_src_clk_freq = board_init_can_clock(base);
    uint32_t interrupt_mask = MCAN_EVENT_RECEIVE;

    /* NOTE: CAN RXBUF must be used with the CAN Filter together.
     *
     * In the example, only the message with ID 0x123 is allowed to be stored into RXBUF0, the other messages
     *  get ignored by the CAN controller according to the CAN Filter setting
     *
     */
    uint32_t rxbuf_index = RXBUF_TEST_RX_BUF_INDEX;
    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_INTO_RX_BUFFER_OR_AS_DBG_MSG;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].match_id = 0x123;
    can_filters[0].offset = rxbuf_index;
    can_filters[0].filter_event = 0;
    can_filters[0].store_location = 0;

    can_config.all_filters_config.ext_id_filter_list.mcan_filter_elem_count = 0;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    can_config.txbuf_trans_interrupt_mask = ~0UL;
    can_config.interrupt_mask = interrupt_mask;
    status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_install_isr_callback(rxbuf_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        /* Transmit via a dedicated TX buffer in non-blocking mode; poll the
         * per-buffer transmission-complete flag so each frame is fully sent
         * before the next one is loaded.  This avoids overwriting a buffer
         * that the CAN controller is still transmitting. */
        uint32_t buf_index = RXBUF_TEST_TX_BUF_INDEX;
        mcan_transmit_via_txbuf_nonblocking(base, buf_index, &tx_buf);
        while (!mcan_is_transmit_occurred(base, buf_index)) {
        }
    }

    bool test_pass = false;
    /* Only the frame with ID 0x123 should have been stored into the RXBUF
     * by the filter; all other IDs are rejected by the global filter rule.
     * A single received message with the correct ID means the test passed. */
    if (s_has_new_rcv_msg) {
        s_has_new_rcv_msg = false;
        printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        if (s_rx_buf.std_id == 0x123) {
            test_pass = true;
        }
    }

    printf("%s %s\n", __func__, test_pass ? "PASSED" : "FAILED");
}
