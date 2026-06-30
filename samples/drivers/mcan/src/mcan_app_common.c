/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * mcan_app_common.c – application entry point and shared infrastructure.
 *
 * Responsibilities of this file:
 *   - AHB RAM message-buffer placement (SoC-specific)
 *   - s_can_info[] table describing every CAN instance on the board
 *   - Shared volatile flags and receive/event shadow buffers
 *   - Unified ISR (board_can_isr) that serves all test modules
 *   - Utility functions: show_received_can_message(), get_timestamp_hex_string(),
 *                        find_board_can_info()
 *   - main() and the interactive menu (handle_can_test / show_help)
 *
 * handle_can_error() is defined in mcan_error.c (co-located with the error test).
 * Each individual test is implemented in its own .c file.  See mcan_app_common.h for
 * the public interface shared across all translation units.
 */

#include "mcan_app_common.h"

/***********************************************************************************************************************
 *
 *  AHB RAM message buffers
 *
 *  NOTE:
 *  1. The MCAN Message Buffer MUST be placed at AHB RAM, otherwise, it won't work.
 *  2. MCAN_MSG_BUF_SIZE_IN_WORDS is the default size; adjust and remap the filter /
 *     FIFO allocation accordingly if the size is changed.
 *
 **********************************************************************************************************************/
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
#if defined(HPM_MCAN0)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan0_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN1)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan1_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN2)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan2_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN3)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan3_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN4)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan4_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN5)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan5_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN6)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan6_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN7)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan7_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#endif /* MCAN_SOC_MSG_BUF_IN_AHB_RAM */

/***********************************************************************************************************************
 *
 *  CAN instance table
 *
 **********************************************************************************************************************/

can_info_t s_can_info[] = {
#if defined(HPM_MCAN0)
    {
        .can_base = HPM_MCAN0,
        .irq_num = IRQn_MCAN0,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan0_msg_buf,
        .ram_size = sizeof(mcan0_msg_buf),
#endif
        .clock_name = clock_can0,
    },
#endif
#if defined(HPM_MCAN1)
    {
        .can_base = HPM_MCAN1,
        .irq_num = IRQn_MCAN1,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan1_msg_buf,
        .ram_size = sizeof(mcan1_msg_buf),
#endif
        .clock_name = clock_can1,
    },
#endif
#if defined(HPM_MCAN2)
    {
        .can_base = HPM_MCAN2,
        .irq_num = IRQn_MCAN2,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan2_msg_buf,
        .ram_size = sizeof(mcan2_msg_buf),
#endif
        .clock_name = clock_can2,
    },
#endif
#if defined(HPM_MCAN3)
    {
        .can_base = HPM_MCAN3,
        .irq_num = IRQn_MCAN3,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan3_msg_buf,
        .ram_size = sizeof(mcan3_msg_buf),
#endif
        .clock_name = clock_can3,
    },
#endif
#if defined(HPM_MCAN4)
    {
        .can_base = HPM_MCAN4,
        .irq_num = IRQn_MCAN4,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan4_msg_buf,
        .ram_size = sizeof(mcan4_msg_buf),
#endif
        .clock_name = clock_can4,
    },
#endif
#if defined(HPM_MCAN5)
    {
        .can_base = HPM_MCAN5,
        .irq_num = IRQn_MCAN5,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan5_msg_buf,
        .ram_size = sizeof(mcan5_msg_buf),
#endif
        .clock_name = clock_can5,
    },
#endif
#if defined(HPM_MCAN6)
    {
        .can_base = HPM_MCAN6,
        .irq_num = IRQn_MCAN6,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan6_msg_buf,
        .ram_size = sizeof(mcan6_msg_buf),
#endif
        .clock_name = clock_can6,
    },
#endif
#if defined(HPM_MCAN7)
    {
        .can_base = HPM_MCAN7,
        .irq_num = IRQn_MCAN7,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan7_msg_buf,
        .ram_size = sizeof(mcan7_msg_buf),
#endif
        .clock_name = clock_can7,
    },
#endif
};

uint32_t g_can_info_count = ARRAY_SIZE(s_can_info);

/***********************************************************************************************************************
 *
 *  ISR callback slot
 *  (volatile flags and shadow buffers are now owned by each demo's own
 *   static variables; nothing shared here)
 *
 **********************************************************************************************************************/

mcan_isr_callback_t g_mcan_isr_cb;

/***********************************************************************************************************************
 *
 *  ISR
 *
 **********************************************************************************************************************/

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, board_can_isr)
void board_can_isr(void)
{
    if (g_mcan_isr_cb != NULL) {
        g_mcan_isr_cb(BOARD_APP_CAN_BASE);
    }
}

/***********************************************************************************************************************
 *
 *  Utility functions
 *
 **********************************************************************************************************************/

void mcan_install_isr_callback(mcan_isr_callback_t callback)
{
    /* Disable MIE so the ISR cannot fire between the load and store of the
     * function pointer, preventing board_can_isr() from seeing a half-written
     * pointer value. */
    uint32_t level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
    g_mcan_isr_cb = callback;
    restore_global_irq(level);
}

void show_received_can_message(const mcan_rx_message_t *rx_msg)
{
    uint32_t msg_len = mcan_get_message_size_from_dlc(rx_msg->dlc);
    if (rx_msg->use_ext_id) {
        printf("CAN message info:\nID=%08x\nContent=:\n", rx_msg->ext_id);
    } else {
        printf("CAN message info:\nID=%08x\nContent=:\n", rx_msg->std_id);
    }
    /* Print the payload in rows of up to 16 bytes, using the same
     * hex-dump style as a standard memory viewer. */
    uint32_t remaining_size = msg_len;
    uint32_t print_size;

    for (uint32_t i = 0; i < msg_len; i += 16) {
        print_size = MIN(remaining_size, 16);
        for (uint32_t j = 0; j < print_size; j++) {
            printf("%02x ", rx_msg->data_8[i + j]);
        }
        printf("\n");
        remaining_size -= print_size;
    }
}

char *get_timestamp_hex_string(const mcan_timestamp_value_t *ts_val)
{
    /* Static buffer – valid until the next call to this function. */
    static char hex_str[100];
    memset(hex_str, 0, sizeof(hex_str));
    /* mcan_timestamp_value_t carries a discriminated union; exactly one of the
     * is_* flags is true.  The if-chain (not if-else) is intentional because
     * only one branch will match. */
    if (ts_val->is_16bit) {
        sprintf(hex_str, "0x%04x", ts_val->ts_16bit);
    }
    if (ts_val->is_32bit) {
        sprintf(hex_str, "0x%08x", ts_val->ts_32bit);
    }
    if (ts_val->is_64bit) {
        /* 64-bit value stored as two 32-bit words; words[1] is the high half. */
        sprintf(hex_str, "0x%08x%08x", ts_val->words[1], ts_val->words[0]);
    }
    if (ts_val->is_empty) {
        sprintf(hex_str, "not exist");
    }
    return hex_str;
}

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
/* Scan s_can_info[] for the entry whose can_base matches BOARD_APP_CAN_BASE.
 * Used by mcan_setup_msg_buf() so that single-peripheral demos do not need to
 * keep track of their own can_info_t pointer. */
can_info_t *find_board_can_info(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_can_info); i++) {
        if (s_can_info[i].can_base == base) {
            return &s_can_info[i];
        }
    }
    return NULL;
}
#endif

hpm_stat_t mcan_setup_msg_buf_by_info(can_info_t *info)
{
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    assert(info != NULL);
    mcan_msg_buf_attr_t attr = { info->ram_base, info->ram_size };
    hpm_stat_t status = mcan_set_msg_buf_attr(info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return status;
    }
#else
    (void)info;
#endif
    return status_success;
}

hpm_stat_t mcan_setup_msg_buf(MCAN_Type *ptr)
{
    (void)ptr;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    return mcan_setup_msg_buf_by_info(find_board_can_info());
#else
    return status_success;
#endif
}

/***********************************************************************************************************************
 *
 *  Interactive menu
 *
 **********************************************************************************************************************/

static void show_help(void)
{
    static const char help_info[] = ""
                                    "*******************************************************************************\n"
                                    "*                                                                             *\n"
                                    "*                         CAN Example Menu                                    *\n"
                                    "*                                                                             *\n"
                                    "* 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)     *\n"
                                    "* 1 - Run loopback test for board supported CAN controller (interrupt mode)   *\n"
                                    "* 2 - Echo test between two board:initiator                                   *\n"
                                    "* 3 - Echo test between two board:responder                                   *\n"
                                    "* 4 - Send multiple classic CAN messages for transmission check               *\n"
                                    "* 5 - Send multiple CANFD messages for transmission check                     *\n"
                                    "* 6 - CAN error test (Need to remove current node from CAN BUS for this test) *\n"
                                    "* 7 - CAN filter test                                                         *\n"
                                    "* 8 - CAN RXBUF test                                                          *\n"
                                    "* 9 - CAN TX EVENT FIFO test                                                  *\n"
                                    "* a - CAN Timestamp Test                                                      *\n"
                                    "* b - CAN timeout counter Test                                                *\n"
                                    "* c - CAN TXBUF cancellation Test                                             *\n"
                                    "* d - CAN TX Queue test                                                       *\n"
                                    "*                                                                             *\n"
                                    "*******************************************************************************\n";
    printf("%s\n", help_info);
}

static void handle_can_test(void)
{
    show_help();

    /* Echo the typed character back to the terminal (no implicit local echo
     * on the SoC UART) and dispatch to the appropriate test function.
     * Unrecognised keys re-print the help menu. */
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
            board_can_send_multiple_classic_can_messages();
            break;
        case '5':
            board_can_send_multiple_canfd_messages();
            break;
        case '6':
            board_can_error_test();
            break;
        case '7':
            board_can_filter_test();
            break;
        case '8':
            board_can_rxbuf_test();
            break;
        case '9':
            board_can_tx_event_test();
            break;
        case 'a':
            board_can_timestamp_test();
            break;
        case 'b':
            board_can_timeout_counter_test();
            break;
        case 'c':
            board_can_cancel_can_message_send();
            break;
        case 'd':
            board_can_txqueue_test();
            break;
        }
    }
}

/***********************************************************************************************************************
 *
 *  Application entry point
 *
 **********************************************************************************************************************/

int main(void)
{
    board_init();

    /* Initialize clock and GPIO for every CAN instance on the board.
     * clock_freq is stored so individual demos can use it without calling
     * board_init_can_clock() again. */
    for (uint32_t i = 0; i < ARRAY_SIZE(s_can_info); i++) {
        can_info_t *info = &s_can_info[i];
        board_init_can(info->can_base);
        info->clock_freq = board_init_can_clock(info->can_base);
    }

    /* Configure AHB RAM message buffer for the board CAN instance */
    hpm_stat_t status = mcan_setup_msg_buf(BOARD_APP_CAN_BASE);
    if (status != status_success) {
        return status;
    }

    handle_can_test();

    return 0;
}
