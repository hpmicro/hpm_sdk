/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"

static volatile bool has_sent_out;
static volatile bool has_error;
static volatile uint8_t error_flags;

static const char *get_can_error_kind_str(uint8_t error_kind)
{
    switch (error_kind) {
    case CAN_KIND_OF_ERROR_NO_ERROR:
        return "No error";
    case CAN_KIND_OF_ERROR_BIT_ERROR:
        return "Bit error";
    case CAN_KIND_OF_ERROR_FORM_ERROR:
        return "Form error";
    case CAN_KIND_OF_ERROR_STUFF_ERROR:
        return "Stuff error";
    case CAN_KIND_OF_ERROR_ACK_ERROR:
        return "ACK error";
    case CAN_KIND_OF_ERROR_CRC_ERROR:
        return "CRC error";
    case CAN_KIND_OF_ERROR_OTHER_ERROR:
        return "Other errors";
    default:
        return "Unknown error";
    }
}

static void can_error_isr(CAN_Type *ptr)
{
    uint8_t flags = can_get_tx_rx_flags(ptr);
    if ((flags & (CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF)) != 0U) {
        has_sent_out = true;
    }
    if ((flags & CAN_EVENT_ERROR) != 0U) {
        has_error = true;
    }
    can_clear_tx_rx_flags(ptr, flags);

    error_flags = can_get_error_interrupt_flags(ptr);
    if (error_flags != 0U) {
        has_error = true;
    }
    can_clear_error_interrupt_flags(ptr, error_flags);
}

static void run_can_error_test(bool disable_retransmission)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000;
    can_config.mode = can_mode_normal;
    can_config.disable_stb_retransmission = disable_retransmission;
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF | CAN_EVENT_ERROR;
    can_config.irq_error_enable_mask = CAN_ERROR_ARBITRATION_LOST_INT_ENABLE | CAN_ERROR_PASSIVE_INT_ENABLE | CAN_ERROR_BUS_ERROR_INT_ENABLE;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    has_error = false;
    has_sent_out = false;
    error_flags = 0;
    can_install_isr_callback(can_error_isr);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf = { 0 };
    tx_buf.dlc = 8;
    tx_buf.id = 0x123;
    uint32_t msg_len = can_get_payload_size_from_dlc((uint8_t) tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = (uint8_t) (i | (i << 4));
    }

    can_send_message_nonblocking(ptr, &tx_buf);
    while ((!has_sent_out) && (!has_error)) {
    }

    if (has_error) {
        uint8_t error_kind = can_get_last_error_kind(ptr);
        uint8_t tcnt = can_get_transmit_error_count(ptr);
        uint8_t rcnt = can_get_receive_error_count(ptr);
        printf("can error exists: last error kind: %s\n", get_can_error_kind_str(error_kind));
        printf("Transmission Error Count: %d\n", tcnt);
        printf("Receiving Error Count: %d\n", rcnt);
    } else {
        printf("Current hardware setup cannot trigger CAN error\n");
    }

    if (disable_retransmission) {
        can_deinit(ptr);
    }
}

void board_can_error_test(void)
{
    run_can_error_test(false);
}

void board_can_error_test_with_no_retransmission(void)
{
    run_can_error_test(true);
}