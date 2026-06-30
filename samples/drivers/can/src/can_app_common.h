/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef CAN_APP_COMMON_H
#define CAN_APP_COMMON_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "board.h"
#include "hpm_can_drv.h"

typedef struct {
    CAN_Type *can_base;
    uint32_t clock_freq;
    int32_t irq_index;
} can_info_t;

typedef void (*can_isr_callback_t)(CAN_Type *ptr);

extern can_info_t s_can_info[];
extern uint32_t g_can_info_count;

bool can_buf_compare(const can_transmit_buf_t *tx_buf, const can_receive_buf_t *rx_buf);
void show_received_can_message(const can_receive_buf_t *rx_msg);
void can_install_isr_callback(can_isr_callback_t callback);

bool can_loopback_test(CAN_Type *base);
bool canfd_loopback_test(CAN_Type *base);
void can_loopback_test_for_all_cans(void);
void board_can_loopback_test_in_interrupt_mode(void);

void board_can_echo_test_initiator(void);
void board_can_echo_test_responder(void);

void board_can_send_multiple_can_messages(void);
void board_can_send_multiple_canfd_messages(void);

void board_can_error_test(void);
void board_can_error_test_with_no_retransmission(void);

void board_can_filter_test(void);

void board_can_timestamp_test(void);

#endif /* CAN_APP_COMMON_H */