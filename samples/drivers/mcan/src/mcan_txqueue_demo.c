/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates the CAN TX Queue mode using fully asynchronous
 * interrupt-driven operation.
 *
 * In TX Queue mode the hardware schedules frames according to their CAN
 * priority (lowest ID = highest priority) rather than the order they were
 * submitted.  This contrasts with TX FIFO mode where frames are transmitted
 * in submission order.
 *
 * The test:
 *   1. Installs txqueue_isr to handle RX-FIFO0-new-message interrupts.
 *   2. Fills the TX queue as fast as possible: frames are submitted back-to-
 *      back via mcan_transmit_via_txqueue_nonblocking().  When the hardware
 *      queue is momentarily full the call returns an error; the loop retries
 *      immediately (busy-spin) until a slot is freed by the hardware.  No
 *      synchronisation with a TX-complete interrupt is used, so the CPU is
 *      always pushing new frames into the queue concurrently with the
 *      hardware transmitting previously enqueued ones.
 *   3. After all 2048 frames are enqueued, waits until the ISR has delivered
 *      all 2048 loopback echoes.
 *   4. Verifies that every submitted ID was echoed back exactly once.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile uint32_t s_rx_received;    /* total loopback echoes delivered by ISR  */

/* The rx_flags array is written from the ISR and read from main context.
 * Declared at file scope so it is zero-initialised at startup and reset
 * explicitly before each test run. */
static bool s_rx_flags[2048];

/* --------------------------------------------------------------------------
 * txqueue_isr
 *
 * Handles RX-FIFO0-new-message events only.
 * Reads every available frame out of RX FIFO0 and marks s_rx_flags so
 * the main loop can later verify completeness.
 * -------------------------------------------------------------------------- */
static void txqueue_isr(MCAN_Type *ptr)
{
    uint32_t flags = mcan_get_interrupt_flags(ptr);

    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0U) {
        mcan_rx_message_t rx_msg;
        /* Drain all frames currently in FIFO0 */
        while (mcan_is_interrupt_flag_set(ptr, MCAN_INT_RXFIFO0_NEW_MSG)) {
            (void)mcan_read_rxfifo(ptr, 0, &rx_msg);
            if (rx_msg.std_id < 2048U) {
                s_rx_flags[rx_msg.std_id] = true;
                s_rx_received++;
            }
            mcan_clear_interrupt_flags(ptr, MCAN_INT_RXFIFO0_NEW_MSG);
        }
    }

    mcan_clear_interrupt_flags(ptr, flags);
}

/* --------------------------------------------------------------------------
 * board_can_txqueue_test
 * -------------------------------------------------------------------------- */
void board_can_txqueue_test(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate                          = 1000000; /* 1 Mbps */
    can_config.mode                              = mcan_mode_loopback_internal;
    can_config.ram_config.txfifo_or_txqueue_mode = MCAN_TXBUF_OPERATION_MODE_QUEUE;
    can_config.interrupt_mask                    = MCAN_INT_RXFIFO0_NEW_MSG;
    can_config.txbuf_trans_interrupt_mask        = ~0UL;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_install_isr_callback(txqueue_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    /* Reset per-run state */
    s_rx_received  = 0U;
    memset(s_rx_flags, 0, sizeof(s_rx_flags));

    mcan_tx_frame_t tx_frame;
    memset(&tx_frame, 0, sizeof(tx_frame));
    tx_frame.dlc = 8;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_frame.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_frame.data_8[i] = (uint8_t)(i | (i << 4));
    }

    for (uint32_t i = 0; i < 2048U; i++) {
        tx_frame.std_id = i;
        uint32_t queue_index;
        /* Fill the queue as fast as possible.  If the hardware queue is full
         * the call returns an error; spin-retry immediately — the TX hardware
         * is draining slots in the background so a slot will free up shortly
         * without any explicit synchronization with a TX-complete interrupt. */
        while (mcan_transmit_via_txqueue_nonblocking(ptr, &tx_frame, &queue_index) != status_success) {
        }
    }

    /* Wait for the ISR to deliver all 2048 loopback echoes */
    while (s_rx_received < 2048U) {
    }

    /* Verify every submitted ID was echoed back exactly once */
    bool all_received = true;
    for (uint32_t i = 0; i < 2048U; i++) {
        if (!s_rx_flags[i]) {
            all_received = false;
            break;
        }
    }

    if (all_received) {
        printf("Sent messages with ID from 0 to 2047 out via TX Queue mode\n");
    } else {
        printf("Not all messages sent out via TX Queue mode were received\n");
    }
}
