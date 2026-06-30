/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "can_app_common.h"

can_info_t s_can_info[] = {
        { .can_base = HPM_CAN0, .irq_index = IRQn_CAN0 },
        { .can_base = HPM_CAN1, .irq_index = IRQn_CAN1 },
#if defined(HPM_CAN2)
        { .can_base = HPM_CAN2, .irq_index = IRQn_CAN2 },
#endif
#if defined(HPM_CAN3)
        { .can_base = HPM_CAN3, .irq_index = IRQn_CAN3 },
#endif
};

uint32_t g_can_info_count = ARRAY_SIZE(s_can_info);

static can_isr_callback_t g_can_isr_cb;

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, board_can_isr)
void board_can_isr(void)
{
    if (g_can_isr_cb != NULL) {
        g_can_isr_cb(BOARD_APP_CAN_BASE);
    }
}

void can_install_isr_callback(can_isr_callback_t callback)
{
    uint32_t level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
    g_can_isr_cb = callback;
    restore_global_irq(level);
}

bool can_buf_compare(const can_transmit_buf_t *tx_buf, const can_receive_buf_t *rx_buf)
{
    bool result = false;

    do {
        HPM_BREAK_IF(tx_buf->id != rx_buf->id);
        HPM_BREAK_IF(tx_buf->dlc != rx_buf->dlc);
        HPM_BREAK_IF(tx_buf->extend_id != rx_buf->extend_id);
        HPM_BREAK_IF(tx_buf->remote_frame != rx_buf->remote_frame);

        bool data_matched = true;
        uint32_t data_bytes = can_get_payload_size_from_dlc((uint8_t) rx_buf->dlc);
        for (uint32_t i = 0; i < data_bytes; i++) {
            if (tx_buf->data[i] != rx_buf->data[i]) {
                data_matched = false;
                break;
            }
        }

        result = data_matched;
    } while (false);

    return result;
}

void show_received_can_message(const can_receive_buf_t *rx_msg)
{
    uint32_t msg_len = can_get_payload_size_from_dlc((uint8_t) rx_msg->dlc);
    printf("CAN message info:\nID=%08x\nContent=:\n", rx_msg->id);

    for (uint32_t i = 0; i < msg_len; i += 16U) {
        uint32_t print_size = MIN(msg_len - i, 16U);
        for (uint32_t j = 0; j < print_size; j++) {
            printf("%02x ", rx_msg->data[i + j]);
        }
        printf("\n");
    }
}

static void show_help(void)
{
    static const char help_info[] = ""
                                    "*********************************************************************************\n"
                                    "*                                                                               *\n"
                                    "*                         CAN Example Menu                                      *\n"
                                    "*                                                                               *\n"
                                    "* 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)       *\n"
                                    "* 1 - Run loopback test for board supported CAN controller (interrupt mode)     *\n"
                                    "* 2 - Echo test between two boards:initiator                                    *\n"
                                    "* 3 - Echo test between two boards:responder                                    *\n"
                                    "* 4 - Send multiple messages for transmission check                             *\n"
                                    "* 5 - CAN error test (Need to remove current node from CAN BUS for this test)   *\n"
                                    "* 6 - CAN filter test                                                           *\n"
                                    "* 7 - Send multiple CANFD messages for transmission check                       *\n"
                                    "* 8 - CAN error test with re-transmission disabled                              *\n"
                                    "* 9 - CAN timestamp feature demo                                                *\n"
                                    "*                                                                               *\n"
                                    "*********************************************************************************\n";

    printf("%s\n", help_info);
}

static void handle_can_test(void)
{
    show_help();

    while (true) {
        char option = getchar();
        putchar(option);
        putchar('\n');

        switch (option) {
        default:
            show_help();
            break;
        case '0':
            can_loopback_test_for_all_cans();
            break;
        case '1':
            board_can_loopback_test_in_interrupt_mode();
            break;
        case '2':
            board_can_echo_test_initiator();
            break;
        case '3':
            board_can_echo_test_responder();
            break;
        case '4':
            board_can_send_multiple_can_messages();
            break;
        case '5':
            board_can_error_test();
            break;
        case '6':
            board_can_filter_test();
            break;
        case '7':
            board_can_send_multiple_canfd_messages();
            break;
        case '8':
            board_can_error_test_with_no_retransmission();
            break;
        case '9':
            board_can_timestamp_test();
            break;
        }
    }
}

int main(void)
{
    board_init();

    for (uint32_t i = 0; i < g_can_info_count; i++) {
        can_info_t *info = &s_can_info[i];
        board_init_can(info->can_base);
        info->clock_freq = board_init_can_clock(info->can_base);
    }

    handle_can_test();

    return 0;
}