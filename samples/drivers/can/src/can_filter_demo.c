/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"

static volatile bool has_new_rcv_msg;
static volatile bool has_sent_out;
static volatile can_receive_buf_t s_can_rx_buf;

static void can_filter_isr(CAN_Type *ptr)
{
    uint8_t flags = can_get_tx_rx_flags(ptr);

    if ((flags & CAN_EVENT_RECEIVE) != 0U) {
        can_read_received_message(ptr, (can_receive_buf_t *) &s_can_rx_buf);
        has_new_rcv_msg = true;
    }

    if ((flags & (CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF)) != 0U) {
        has_sent_out = true;
    }

    can_clear_tx_rx_flags(ptr, flags);
}

void board_can_filter_test(void)
{
    can_filter_config_t can_filters[16] = { 0 };
    can_config_t can_config;
    CAN_Type *ptr = BOARD_APP_CAN_BASE;

    printf("CAN Filter test case 0: only check bit0 of CAN ID\n");
    can_filters[0].enable = true;
    can_filters[0].index = 0;
    can_filters[0].id_mode = can_filter_id_mode_both_frames;
    can_filters[0].code = 0;
    can_filters[0].mask = 0x3FFFFFFEUL;

    can_get_default_config(&can_config);
    can_config.baudrate = 1000000;
    can_config.mode = can_mode_loopback_internal;
    can_config.filter_list_num = 1;
    can_config.filter_list = &can_filters[0];
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    has_new_rcv_msg = false;
    has_sent_out = false;
    can_install_isr_callback(can_filter_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = 8;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        has_sent_out = false;
        has_new_rcv_msg = false;
        can_send_message_nonblocking(ptr, &tx_buf);
        while (!has_sent_out) {
        }
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.id);
        }
    }
    printf("Received message count: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 1024U) ? "PASSED" : "Failed");

    printf("CAN Filter test case 1: only accept message with specified CAN ID\n");
    for (uint32_t i = 0; i < ARRAY_SIZE(can_filters); i++) {
        can_filters[i].enable = true;
        can_filters[i].index = i;
        can_filters[i].id_mode = can_filter_id_mode_both_frames;
        can_filters[i].code = (i << 4) | i;
        can_filters[i].mask = 0;
    }
    can_config.filter_list_num = ARRAY_SIZE(can_filters);
    status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        has_sent_out = false;
        has_new_rcv_msg = false;
        can_send_message_nonblocking(ptr, &tx_buf);
        while (!has_sent_out) {
        }
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.id);
        }
    }
    printf("Received message count: %d, %s\n", rcv_msg_cnt,
           (rcv_msg_cnt == ARRAY_SIZE(can_filters)) ? "PASSED" : "Failed");
}