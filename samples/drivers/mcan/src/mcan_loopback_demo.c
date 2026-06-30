/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file implements two loopback test scenarios:
 *   1. can_loopback_test()              - Blocking loopback for a single CAN instance
 *   2. can_loopback_test_for_all_cans() - Runs the above for every SoC CAN in polling mode
 *   3. board_can_loopback_test_in_interrupt_mode() - Loopback driven by interrupts
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_has_new_rcv_msg;
static volatile bool s_has_sent_out;
static volatile mcan_rx_message_t s_rx_buf;

/* --------------------------------------------------------------------------
 * Internal helpers
 * -------------------------------------------------------------------------- */

/**
 * @brief ISR callback for the loopback interrupt-mode test.
 *
 * Handles RXFIFO0 new-message and TX-complete events; updates the file-private
 * s_has_new_rcv_msg / s_has_sent_out flags consumed by
 * board_can_loopback_test_in_interrupt_mode().
 */
static void loopback_isr(MCAN_Type *ptr)
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

/**
 * @brief Compare the content between a transmission buffer and a receive buffer
 *
 * @param [in] tx_buf Pointer to the transmitted frame
 * @param [in] rx_buf Pointer to the received message
 * @retval true  Content is identical
 * @retval false Content does not match
 */
static bool can_buf_compare(const mcan_tx_frame_t *tx_buf, const mcan_rx_message_t *rx_buf)
{
    bool result = false;

    /* do { ... } while (false) with HPM_BREAK_IF acts as a structured
     * early-exit block: any mismatch breaks out immediately without
     * using goto or nested returns. */
    do {
        HPM_BREAK_IF(tx_buf->dlc != rx_buf->dlc);
        HPM_BREAK_IF(tx_buf->use_ext_id != rx_buf->use_ext_id);
        if (tx_buf->use_ext_id) {
            HPM_BREAK_IF(tx_buf->ext_id != rx_buf->ext_id);
        } else {
            HPM_BREAK_IF(tx_buf->std_id != rx_buf->std_id);
        }

        HPM_BREAK_IF(tx_buf->rtr != rx_buf->rtr);
        bool data_matched = true;

        uint32_t data_bytes = mcan_get_message_size_from_dlc(rx_buf->dlc);
        for (uint32_t i = 0; i < data_bytes; i++) {
            if (tx_buf->data_8[i] != rx_buf->data_8[i]) {
                data_matched = false;
                break;
            }
        }

        result = data_matched;

    } while (false);

    return result;
}

/* --------------------------------------------------------------------------
 * Public functions
 * -------------------------------------------------------------------------- */

bool can_loopback_test(MCAN_Type *base, bool enable_canfd)
{
    uint32_t error_cnt = 0;
    bool result = false;
    mcan_tx_frame_t tx_buf;
    mcan_rx_message_t rx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    memset(&rx_buf, 0, sizeof(rx_buf));

    /* Test Transmission and Reception of Standard Frame */
    tx_buf.std_id = 0x123;
    if (!enable_canfd) {
        tx_buf.dlc = 8;
        for (uint32_t i = 0; i < 8; i++) {
            tx_buf.data_8[i] = (uint8_t) i | (i << 4);
        }
    } else {
        tx_buf.dlc = MCAN_MSG_DLC_64_BYTES;
        tx_buf.canfd_frame = 1;
        tx_buf.bitrate_switch = 1;
        for (uint32_t i = 0; i < mcan_get_message_size_from_dlc(tx_buf.dlc); i++) {
            tx_buf.data_8[i] = i;
        }
    }
    mcan_transmit_blocking(base, &tx_buf);
    mcan_receive_from_fifo_blocking(base, 0, &rx_buf);
    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CAN loopback test for standard frame %s\n", result ? "passed" : "failed");

    /* Test Transmission and Reception of Extended Frame */
    tx_buf.use_ext_id = 1U;
    tx_buf.ext_id = 0x12345678U;

    mcan_transmit_blocking(base, &tx_buf);
    mcan_receive_from_fifo_blocking(base, 0, &rx_buf);

    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CAN loopback test for extend frame %s\n", result ? "passed" : "failed");

    return (error_cnt < 1);
}

void can_loopback_test_for_all_cans(void)
{
    mcan_config_t can_config;
    hpm_stat_t status;
    for (uint32_t i = 0; i < g_can_info_count; i++) {
        if (mcan_setup_msg_buf_by_info(&s_can_info[i]) != status_success) {
            return;
        }
        mcan_get_default_config(s_can_info[i].can_base, &can_config);
        can_config.mode = mcan_mode_loopback_internal;
        board_init_can(s_can_info[i].can_base);
        uint32_t can_src_clk_freq = board_init_can_clock(s_can_info[i].can_base);
        /* IRQ is not used in polling-mode loopback; disable it explicitly so
         * any previously installed callback does not fire unexpectedly. */
        intc_m_disable_irq(s_can_info[i].irq_num);
        status = mcan_init(s_can_info[i].can_base, &can_config, can_src_clk_freq);
        assert(status == status_success);
        (void) status; /* Suppress compiler warning in release builds where assert() is a no-op */
        bool result = can_loopback_test(s_can_info[i].can_base, false);
        mcan_deinit(s_can_info[i].can_base);
        printf("    CAN%d CAN2.0 loopback test %s\n", i, result ? "PASSED" : "FAILED");

        /* Re-use the same config struct for the CANFD round; only enable_canfd
         * and ram_config need updating because mode is already loopback. */
        can_config.enable_canfd = true;
        mcan_get_default_ram_config(s_can_info[i].can_base, &can_config.ram_config, true);
        status = mcan_init(s_can_info[i].can_base, &can_config, can_src_clk_freq);
        assert(status == status_success);
        (void) status; /* Suppress compiler warning in release builds where assert() is a no-op */
        result = can_loopback_test(s_can_info[i].can_base, false);
        mcan_deinit(s_can_info[i].can_base);
        printf("    CAN%d CANFD loopback test %s\n", i, result ? "PASSED" : "FAILED");
    }
}

void board_can_loopback_test_in_interrupt_mode(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
    if (mcan_setup_msg_buf(base) != status_success) {
        return;
    }
    mcan_get_default_config(base, &can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = mcan_mode_loopback_internal;
    board_init_can(base);
    uint32_t can_src_clk_freq = board_init_can_clock(base);
    uint32_t interrupt_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_enable_interrupts(base, interrupt_mask);
    mcan_enable_txbuf_transmission_interrupt(base, ~0UL);

    mcan_install_isr_callback(loopback_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    uint32_t count = 0;
    uint32_t test_rounds = 2048;
    for (uint32_t i = 0; i < test_rounds; i++) {
        tx_buf.std_id = i;
        mcan_transmit_blocking(base, &tx_buf);
        while (!s_has_sent_out) {
        }
        /* Clear s_has_sent_out AFTER the wait to avoid a race where the ISR
         * sets it again before the next loop iteration checks it. */
        while (!s_has_new_rcv_msg) {
        }
        s_has_new_rcv_msg = false;
        s_has_sent_out = false;
        printf("New message received, ID=%08x\n", s_rx_buf.std_id);
        count++;
    }
    mcan_deinit(base);

    printf("%s %s\n", __func__, (count == test_rounds) ? "PASSED" : "FAILED");
}
