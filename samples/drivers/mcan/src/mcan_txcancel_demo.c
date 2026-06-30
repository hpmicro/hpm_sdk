/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates TX buffer / TX FIFO cancellation.
 *
 * Two scenarios are each implemented as a separate static function and called
 * in sequence by board_can_cancel_can_message_send():
 *
 *   can_txfifo_cancellation_test() - Four frames are queued via the TX FIFO;
 *                                    frames 1-3 are immediately cancelled.
 *                                    The test verifies that the cancellation-
 *                                    finished flags are set for all three
 *                                    cancelled entries.
 *
 *   can_txbuf_cancellation_test()  - Four frames are submitted to dedicated
 *                                    TX buffers 0-3; buffers 1-3 are cancelled.
 *                                    The same verification applies.
 *
 * In both cases buffer 0 / FIFO slot 0 is not cancelled so it will complete
 * normally, demonstrating that cancellation is selective.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * Shared init helper
 *
 * Configures the CAN peripheral for 1 Mbps internal loopback and returns
 * status_success on success.
 * Returns 0 (as a clock frequency) on AHB-RAM attribute error (only relevant
 * when MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1).
 * -------------------------------------------------------------------------- */
static hpm_stat_t txcancel_test_init(MCAN_Type *ptr, mcan_config_t *can_config)
{
    hpm_stat_t s = mcan_setup_msg_buf(ptr);
    if (s != status_success) {
        return s;
    }
    mcan_get_default_config(ptr, can_config);
    can_config->baudrate = 1000000; /* 1Mbps */
    can_config->mode     = mcan_mode_loopback_internal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    return mcan_init(ptr, can_config, can_src_clk_freq);
}

/* --------------------------------------------------------------------------
 * Scenario 1: TXFIFO cancellation
 *
 * Four frames (IDs 0-3, DLC=8) are pushed into the TX FIFO via non-blocking
 * mode.  The FIFO indices returned for frames 1-3 are used to request
 * cancellation immediately.  The test counts how many of the three
 * cancellations completed and prints PASSED if all three finished.
 * -------------------------------------------------------------------------- */
static void can_txfifo_cancellation_test(MCAN_Type *ptr, mcan_config_t *can_config)
{
    hpm_stat_t status = txcancel_test_init(ptr, can_config);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }

    uint32_t fifo_idx[4];
    for (uint32_t i = 0; i < 4; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txfifo_nonblocking(ptr, &tx_buf, &fifo_idx[i]);
    }
    for (uint32_t i = 1; i < 4; i++) {
        mcan_cancel_tx_buf_send_request(ptr, fifo_idx[i]);
    }

    uint32_t cancel_cnt = 0;
    for (uint32_t i = 1; i < 4; i++) {
        bool result = mcan_is_tx_buf_cancellation_finished(ptr, fifo_idx[i]);
        cancel_cnt += (result ? 1 : 0);
    }
    printf("TXFIFO cancellation test %s\n", cancel_cnt == 3 ? "PASSED" : "FAILED");
}

/* --------------------------------------------------------------------------
 * Scenario 2: TXBUF cancellation
 *
 * Four frames (IDs 0-3, DLC=8) are submitted to dedicated TX buffers 0-3 via
 * non-blocking mode.  Buffers 1-3 are cancelled.  The test counts how many
 * cancellations completed and prints PASSED if all three finished.
 * -------------------------------------------------------------------------- */
static void can_txbuf_cancellation_test(MCAN_Type *ptr, mcan_config_t *can_config)
{
    hpm_stat_t status = txcancel_test_init(ptr, can_config);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }
    for (uint32_t i = 0; i < 4; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, i, &tx_buf);
    }
    for (uint32_t i = 1; i < 4; i++) {
        mcan_cancel_tx_buf_send_request(ptr, i);
    }

    uint32_t cancel_cnt = 0;
    for (uint32_t i = 1; i < 4; i++) {
        bool result = mcan_is_tx_buf_cancellation_finished(ptr, i);
        cancel_cnt += (result ? 1 : 0);
    }
    printf("TXBUF cancellation test %s\n", cancel_cnt == 3 ? "PASSED" : "FAILED");
}

/* --------------------------------------------------------------------------
 * Public entry point
 * -------------------------------------------------------------------------- */

void board_can_cancel_can_message_send(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;

    can_txfifo_cancellation_test(ptr, &can_config);
    can_txbuf_cancellation_test(ptr, &can_config);
}
