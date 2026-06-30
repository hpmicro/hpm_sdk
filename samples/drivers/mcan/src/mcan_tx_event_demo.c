/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates the CAN TX Event FIFO feature.
 *
 * When a frame is transmitted with event_fifo_control = 1 the MCAN IP
 * stores a TX-event element in the TX Event FIFO. The event element
 * carries the message marker and an optional timestamp so that the
 * application can correlate every sent frame with its transmission result.
 *
 * This test submits 256 frames via the TX FIFO (non-blocking) and waits
 * for both the TX-event interrupt and the receive interrupt for each frame
 * before proceeding to the next.
 */

#include "mcan_app_common.h"

/* --------------------------------------------------------------------------
 * File-private ISR state
 * -------------------------------------------------------------------------- */
static volatile bool s_tx_event_occurred;
static volatile bool s_has_new_rcv_msg;
static volatile mcan_tx_event_fifo_elem_t s_tx_evt;
static volatile mcan_rx_message_t s_rx_buf;

/* Number of frames to transmit and confirm via the TX Event FIFO. */
#define TX_EVENT_TEST_ROUNDS  0x100U

/* --------------------------------------------------------------------------
 * ISR callback
 *
 * Handles TX-event-FIFO new-entry and RXFIFO0 new-message events; updates
 * s_tx_event_occurred / s_has_new_rcv_msg and copies the event/frame into the
 * file-private shadow buffers for consumption by board_can_tx_event_test().
 * -------------------------------------------------------------------------- */
static void tx_event_isr(MCAN_Type *ptr)
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

void board_can_tx_event_test(void)
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
    uint32_t interrupt_mask = MCAN_INT_TX_EVT_FIFO_NEW_ENTRY | MCAN_EVENT_RECEIVE;
    status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_enable_interrupts(base, interrupt_mask);
    mcan_enable_txbuf_transmission_interrupt(base, ~0UL);

    mcan_install_isr_callback(tx_event_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    uint32_t test_rounds = TX_EVENT_TEST_ROUNDS;
    uint32_t count = 0;
    for (uint32_t i = 0; i < test_rounds; i++) {
        tx_buf.std_id = i;
        /* event_fifo_control = 1 instructs the MCAN IP to store a TX-event
         * element in the TX Event FIFO after this frame is sent.
         * message_marker_l is an 8-bit application tag echoed verbatim into
         * the TX event element so the application can match the event back to
         * the original submission. */
        tx_buf.event_fifo_control = 1U; /* Store into TX Event FIFO */
        tx_buf.message_marker_l = i & 0xFF;
        /* Transmit via TXFIFO in non-blocking mode; the TX-event ISR will fire
         * once the frame is on the bus, and then the RX ISR will fire when
         * the loopback echo arrives.  Wait for TX event first because it
         * always precedes the RX delivery. */
        mcan_transmit_via_txfifo_nonblocking(base, &tx_buf, NULL);
        while (!s_tx_event_occurred) {
        }
        printf("message with message marker:0x%02x has been sent out successfully\n", s_tx_evt.message_marker);

        while (!s_has_new_rcv_msg) {
        }
        s_has_new_rcv_msg = false;
        s_tx_event_occurred = false;
        ++count;
    }

    printf("%s %s\n", __func__, (count == test_rounds) ? "PASSED" : "FAILED");
}
