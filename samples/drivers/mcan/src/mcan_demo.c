/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_mcan_drv.h"


/***********************************************************************************************************************
 *
 *  Definitions
 *
 **********************************************************************************************************************/

typedef struct {
    MCAN_Type *can_base;
    uint32_t clock_freq;
    uint32_t irq_num;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    uint32_t ram_base;
    uint32_t ram_size;
#endif
} can_info_t;

/***********************************************************************************************************************
 *
 *  Prototypes
 *
 **********************************************************************************************************************/

/**
 * @brief Compare the content between Transmission buffer and Receive buffer
 *
 * @param [in] tx_buf Transmission buffer
 * @param [in] rx_buf Receive buffer
 * @retval true Content is identical
 * @retval false Content was mismatch
 */
static bool can_buf_compare(const mcan_tx_frame_t *tx_buf, const mcan_rx_message_t *rx_buf);

/**
 * @brief CAN loopback test
 *
 * @param [in] base CAN base address
 * @retval true Test passed
 * @retval false Test failed
 */
bool can_loopback_test(MCAN_Type *base, bool enable_canfd);

/**
 * @brief Run loopback test for all SoC supported CANs
 */
void can_loopback_test_for_all_cans(void);

/**
 * @brief Test CAN interrupt mode in loopback mode
 */
void board_can_loopback_test_in_interrupt_mode(void);

/**
 * @brief CAN echo test, initiator
 */
void board_can_echo_test_initiator(void);

/**
 * @brief CAN echo test, responder
 */
void board_can_echo_test_responder(void);

/**
 * @brief CAN multiple classic CAN message transmission test
 */
void board_can_send_multiple_classic_can_messages(void);

/**
 * @brief Sent out multiple CANFD messages
 */
void board_can_send_multiple_canfd_messages(void);

/**
 * @brief CAN error interrupt test
 */
void board_can_error_test(void);

/**
 * @brief CAN filter test
 */
void board_can_filter_test(void);

/**
 * @brief CAN RXBUF test
 *
 * This case demonstrates the usage of the CAN RXBUF
 */
void board_can_rxbuf_test(void);

/**
 * @brief CAN TX Event Test
 */
void board_can_tx_event_test(void);

/**
 * @brief CAN Timestamp test
 */
void board_can_timestamp_test(void);

/**
 * @brief CAN timeout counter test
 *  This sample demonstrate the timeout feature supported by MCAN IP, the following scenarios are supported:
 *    1. Continuous mode. Timeout occurs if the TOCV register counts down to zero
 *    2. Triggered by TX Event FIFO. Timeout occurs if the data in TX Event FIFO is not handled in timeout period
 *    3. Triggered by RXFIFO0. Timeout occurs if the data in RXFIFO0 is not handed in timeout period
 *    4. Triggered by RXFIFO1. Timeout occurs if the data in RXFIFO1 is not handled in timeout period
 */
void board_can_timeout_counter_test(void);

/**
 * @brief Display the CAN message content
 *
 * @param [in] rx_msg CAN message buffer
 */
void show_received_can_message(const mcan_rx_message_t *rx_msg);

/**
 * @brief CAN Interrupt Service Routine
 */
void board_can_isr(void);

/**
 * @brief Handler for CAN test options
 */
void handle_can_test(void);

/**
 * @brief Handle CAN errors
 */
void handle_can_error(MCAN_Type *ptr);

void show_help(void);

char *get_timestamp_hex_string(const mcan_timestamp_value_t *ts_val);

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
static can_info_t *find_board_can_info(void);
#endif

/***********************************************************************************************************************
 *
 *  Variables
 *
 *  NOTE:
 *  1. The MCAN Message Buffer Must be placed at AHB RAM, otherwise, it won't work
 *  2. The `MCAN_MSG_BUF_SIZE_IN_WORDS` is the default Message Buffer Size, the size can be smaller or larger depends on
 *     the users' application model, once changing the default buffer size, the software must take care of the
 *     ram_config and adjust the default allocation of the filter, etc.
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
#endif

static can_info_t s_can_info[] = {
#if defined(HPM_MCAN0)
    {
        .can_base = HPM_MCAN0,
        .irq_num = IRQn_MCAN0,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan0_msg_buf,
        .ram_size = sizeof(mcan0_msg_buf),
#endif
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
        },
#endif
#if defined (HPM_MCAN3)
    {
        .can_base = HPM_MCAN3,
        .irq_num = IRQn_MCAN3,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan3_msg_buf,
        .ram_size = sizeof(mcan3_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN4)
    {
        .can_base = HPM_MCAN4,
        .irq_num = IRQn_MCAN4,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan4_msg_buf,
        .ram_size = sizeof(mcan4_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN5)
    {
        .can_base = HPM_MCAN5,
        .irq_num = IRQn_MCAN5,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan5_msg_buf,
        .ram_size = sizeof(mcan5_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN6)
    {
        .can_base = HPM_MCAN6,
        .irq_num = IRQn_MCAN6,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan6_msg_buf,
        .ram_size = sizeof(mcan6_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN7)
    {
        .can_base = HPM_MCAN7,
        .irq_num = IRQn_MCAN7,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan7_msg_buf,
        .ram_size = sizeof(mcan7_msg_buf),
#endif
        },
#endif
};

static volatile bool has_new_rcv_msg;

static volatile bool has_sent_out;

static volatile bool has_error;

static volatile bool tx_event_occurred;

static volatile bool timeout_event_occurred;

static volatile bool rxfifo0_event_occurred;

static volatile bool rxfifo1_event_occurred;

static volatile mcan_rx_message_t s_can_rx_buf;

static volatile mcan_tx_event_fifo_elem_t s_can_tx_evt;

/***********************************************************************************************************************
 *
 *  Codes
 *
 **********************************************************************************************************************/

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, board_can_isr)
void board_can_isr(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    uint32_t flags = mcan_get_interrupt_flags(base);
    /* New message is available in RXFIFO0 */
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0) {
        mcan_read_rxfifo(base, 0, (mcan_rx_message_t *) &s_can_rx_buf);
        has_new_rcv_msg = true;
        rxfifo0_event_occurred = true;
    }
    /* New message is available in RXFIFO1 */
    if ((flags & MCAN_INT_RXFIFO1_NEW_MSG) != 0U) {
        mcan_read_rxfifo(base, 1, (mcan_rx_message_t *) &s_can_rx_buf);
        has_new_rcv_msg = true;
        rxfifo1_event_occurred = true;
    }
    /* New message is available in RXBUF */
    if ((flags & MCAN_INT_MSG_STORE_TO_RXBUF) != 0U) {
        has_new_rcv_msg = true;
        /* NOTE: Below code is for demonstration purpose, the performance is not optimized
         *       Users should optimize the performance according to real use case.
         */
        for (uint32_t buf_index = 0; buf_index < MCAN_RXBUF_SIZE_CAN_DEFAULT; buf_index++) {
            if (mcan_is_rxbuf_data_available(base, buf_index)) {
                mcan_read_rxbuf(base, buf_index, (mcan_rx_message_t *) &s_can_rx_buf);
                mcan_clear_rxbuf_data_available_flag(base, buf_index);
            }
        }
    }
    /* New TX Event occurred */
    if ((flags & MCAN_INT_TX_EVT_FIFO_NEW_ENTRY) != 0) {
        mcan_read_tx_evt_fifo(BOARD_APP_CAN_BASE, (mcan_tx_event_fifo_elem_t *) &s_can_tx_evt);
        tx_event_occurred = true;
    }
    /* Transmit completed */
    if ((flags & MCAN_EVENT_TRANSMIT) != 0U) {
        has_sent_out = true;
    }
    /* Error happened */
    if ((flags & MCAN_EVENT_ERROR) != 0) {
        has_error = true;
    }

    if ((flags & MCAN_INT_TIMEOUT_OCCURRED) != 0) {
        timeout_event_occurred = true;
    }
    mcan_clear_interrupt_flags(BOARD_APP_CAN_BASE, flags);
}

void show_received_can_message(const mcan_rx_message_t *rx_msg)
{
    uint32_t msg_len = mcan_get_message_size_from_dlc(rx_msg->dlc);
    if (rx_msg->use_ext_id) {
        printf("CAN message info:\nID=%08x\nContent=:\n", rx_msg->ext_id);
    } else {
        printf("CAN message info:\nID=%08x\nContent=:\n", rx_msg->std_id);
    }
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

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
static can_info_t *find_board_can_info(void)
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

int main(void)
{
    board_init();

    /* Initialize CAN */
    for (uint32_t i = 0; i < ARRAY_SIZE(s_can_info); i++) {
        can_info_t *info = &s_can_info[i];
        board_init_can(info->can_base);
        info->clock_freq = board_init_can_clock(info->can_base);
    }

    /* For the device with the MCAN message buffer located in the AHB RAM, the software must:
     *      1. place the message buffer at the AHB RAM
     *      2. set the message buffer attribute before initializing the MCAN peripheral
     */
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    can_info_t *can_info = find_board_can_info();
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    hpm_stat_t status = mcan_set_msg_buf_attr(base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return status;
    }
#endif

    handle_can_test();

    return 0;
}

static bool can_buf_compare(const mcan_tx_frame_t *tx_buf, const mcan_rx_message_t *rx_buf)
{
    bool result = false;

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
        tx_buf.dlc = MCAN_DATA_FIELD_SIZE_64BYTES;
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
    for (uint32_t i = 0; i < ARRAY_SIZE(s_can_info); i++) {
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        mcan_msg_buf_attr_t attr = { s_can_info[i].ram_base, s_can_info[i].ram_size };
        status = mcan_set_msg_buf_attr(s_can_info[i].can_base, &attr);
        if (status != status_success) {
            printf("Error was detected during setting message buffer attribute, please check the arguments\n");
            return;
        }
#endif
        mcan_get_default_config(s_can_info[i].can_base, &can_config);
        can_config.mode = mcan_mode_loopback_internal;
        board_init_can(s_can_info[i].can_base);
        uint32_t can_src_clk_freq = board_init_can_clock(s_can_info[i].can_base);
        intc_m_disable_irq(s_can_info[i].irq_num);
        status = mcan_init(s_can_info[i].can_base, &can_config, can_src_clk_freq);
        assert(status == status_success);
        (void) status; /* Suppress compiling warning in release build */
        bool result = can_loopback_test(s_can_info[i].can_base, false);
        mcan_deinit(s_can_info[i].can_base);
        printf("    CAN%d CAN2.0 loopback test %s\n", i, result ? "PASSED" : "FAILED");

        can_config.enable_canfd = true;
        mcan_get_default_ram_config(s_can_info[i].can_base, &can_config.ram_config, true);
        status = mcan_init(s_can_info[i].can_base, &can_config, can_src_clk_freq);
        assert(status == status_success);
        (void) status; /* Suppress compiling warning in release build */
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
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
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
        while (!has_sent_out) {
        }

        while (!has_new_rcv_msg) {
        }
        has_new_rcv_msg = false;
        has_sent_out = false;
        printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        count++;
    }
    mcan_deinit(base);

    printf("%s %s\n", __func__, (count == test_rounds) ? "PASSED" : "FAILED");
}

void board_can_rxbuf_test(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
    mcan_filter_elem_t can_filters[16];
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
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
    uint32_t rxbuf_index = 1;
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

    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        /* Demonstrate Transmit via TXBUF in non-blocking way here
         */
        uint32_t buf_index = 2;
        mcan_transmit_via_txbuf_nonblocking(base, buf_index, &tx_buf);
        while (!mcan_is_transmit_occurred(base, buf_index)) {
        }
    }

    bool test_pass = false;
    /* If below condition was met, it means this test case passed */
    if (has_new_rcv_msg) {
        has_new_rcv_msg = false;
        has_sent_out = false;
        printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        if (s_can_rx_buf.std_id == 0x123) {
            test_pass = true;
        }
    }

    printf("%s %s\n", __func__, test_pass ? "PASSED" : "FAILED");
}

void board_can_tx_event_test(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
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

    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    uint32_t test_rounds = 0x100;
    uint32_t count = 0;
    for (uint32_t i = 0; i < test_rounds; i++) {
        tx_buf.std_id = i;
        tx_buf.event_fifo_control = 1U; /* Store into TX Event FIFO */
        tx_buf.message_marker_l = i & 0xFF;
        /* Demonstrate the Transmit via TXFIFO in nonblocking mode here */
        mcan_transmit_via_txfifo_nonblocking(base, &tx_buf, NULL);
        while (!tx_event_occurred) {
        }
        printf("message with message marker:0x%02x has been sent out successfully\n", s_can_tx_evt.message_marker);

        while (!has_new_rcv_msg) {
        }
        has_new_rcv_msg = false;
        tx_event_occurred = false;
        ++count;
    }

    printf("%s %s\n", __func__, (count == test_rounds) ? "PASSED" : "FAILED");
}

void board_can_echo_test_initiator(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = mcan_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    mcan_enable_interrupts(ptr, MCAN_EVENT_RECEIVE);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0U);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    tx_buf.std_id = 0x123;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    printf("Can Echo test: Initiator is sending message out...\n");
    status = mcan_transmit_blocking(BOARD_APP_CAN_BASE, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }
    printf("Waiting for echo message...\n");
    while (!has_new_rcv_msg) {
    }
    has_new_rcv_msg = false;
    show_received_can_message((const mcan_rx_message_t *) &s_can_rx_buf);
}

void board_can_echo_test_responder(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(base, &can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = mcan_mode_normal;
    board_init_can(base);
    uint32_t can_src_clk_freq = board_init_can_clock(base);
    status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_enable_interrupts(base, MCAN_EVENT_RECEIVE);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    printf("CAN echo test: Responder is waiting for echo message...\n");
    while (!has_new_rcv_msg) {
    }
    has_new_rcv_msg = false;
    show_received_can_message((const mcan_rx_message_t *) &s_can_rx_buf);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = s_can_rx_buf.dlc;
    tx_buf.std_id = 0x321;
    uint32_t msg_len = mcan_get_message_size_from_dlc(s_can_rx_buf.dlc);
    memcpy(&tx_buf.data_8, (uint8_t *) &s_can_rx_buf.data_8, msg_len);
    status = mcan_transmit_blocking(base, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }
    printf("Sent echo message back\n");
}

void board_can_send_multiple_classic_can_messages(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
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
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        status = mcan_transmit_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent CAN2.0 message failed, error_code:%d\n", status);
            return;
        }
    }
    printf("Sent messages with ID from 0 to 2047 out\n");
}

void board_can_send_multiple_canfd_messages(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);
    can_config.use_lowlevel_timing_setting = true;
    uint16_t prescaler;
    uint16_t prescaler_fd;
    uint16_t seg1;
    uint16_t seg1_fd;
    uint16_t seg2;
    uint16_t seg2_fd;
    uint16_t sjw;
    uint16_t sjw_fd;
    if (can_src_clk_freq == 40000000UL) {
        prescaler = 1;
        prescaler_fd = 1;

    } else if (can_src_clk_freq == 80000000UL) {
        prescaler = 2;
        prescaler_fd = 2;
    } else {
        printf("Unsupported clock frequency for the CAN-FD test");
        return;
    }

    /*
     * Baudrate Calculation formula:
     *  CAN baudrate = clock_freq / prescaler/ (1 + num_seg1 + num_seg2)
     *
     *  Sample Point Calculation formula:
     *  Sample point = (1 + SEG1) / (1 + SEG1 + SGE2) * 100%
     *  Recommended Sample point setting:
     *   75% if baudrate >= 800Kbps
     *   80% if baudrate > 500kbps
     *   87.5% if baudrate <= 500kbps
     *  In the below example,
     *      the sample point of arbitration phase is: (1 + 29) / (1 + 29 + 10) * 100% = 75%
     *      the sample point of data phase is: (1 + 14) / (1 + 14 + 5) * 100% = 75%
     */
    /* CAN Baudrate = 1Mbps */
    seg1 = 29;
    seg2 = 10;
    sjw = 2;
    /* CAN baudrate = 2Mbps */
    seg1_fd = 14;
    seg2_fd = 5;
    sjw_fd = 2;

    can_config.can_timing.prescaler = prescaler;
    can_config.can_timing.num_seg1 = seg1;
    can_config.can_timing.num_seg2 = seg2;
    can_config.can_timing.num_sjw = sjw;

    can_config.canfd_timing.prescaler = prescaler_fd;
    can_config.canfd_timing.num_seg1 = seg1_fd;
    can_config.canfd_timing.num_seg2 = seg2_fd;
    can_config.canfd_timing.num_sjw = sjw_fd;
    /* Note: Check the CAN Transceiver datasheet carefully, if the loop delay exceeds 1 data bit time
     * Please enable the Transmitter Delay Compensation feature and configure the `ssp_offset` properly
     * Below is an example setting.
     * */
#ifdef CANFD_ENABLE_TRANSMITTER_DELAY_COMPENSATION
    can_config.canfd_timing.enable_tdc = true;
    /* If the ssp_offset is set to 0, then the following configuration is used by default */
    can_config.tdc_config.ssp_offset = seg1_fd + 1;
    can_config.tdc_config.filter_window_length = can_config.tdc_config.ssp_offset;
#else
    can_config.canfd_timing.enable_tdc = false;
#endif
    can_config.enable_canfd = true;
    can_config.enable_tdc = can_config.canfd_timing.enable_tdc;
    can_config.mode = mcan_mode_normal;
    mcan_get_default_ram_config(ptr, &can_config.ram_config, true);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 15;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i;
    }
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        tx_buf.canfd_frame = 1;
        tx_buf.bitrate_switch = 1;
        status = mcan_transmit_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent CANFD message failed, error_code:%d\n", status);
            return;
        }
    }
    printf("Sent CANFD messages with ID from 0 to 2047 out\n");
}

void board_can_error_test(void)
{
    /**
     * Case 1: Test CAN2.0B errors
     */
    MCAN_Type *base = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(base, &can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = mcan_mode_normal;
    uint32_t interrupt_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT | MCAN_EVENT_ERROR;
    board_init_can(base);
    uint32_t can_src_clk_freq = board_init_can_clock(base);
    can_config.interrupt_mask = interrupt_mask;
    can_config.txbuf_trans_interrupt_mask = ~0UL;
    status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    has_sent_out = false;
    has_error = false;

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }
    tx_buf.std_id = 0x123;
    mcan_transmit_via_txbuf_nonblocking(base, 0, &tx_buf);
    while ((!has_sent_out) && (!has_error)) {
    }
    if (has_error) {
        printf("can error happened\n");
    } else {
        printf("no can error happened\n");
    }
    handle_can_error(base);

    /**
     * Case 2: Test CANFD errors
     */
    mcan_get_default_config(base, &can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.baudrate_fd = 2000000; /* 2Mbps*/
    can_config.enable_canfd = true;
    can_config.mode = mcan_mode_normal;
    board_init_can(base);
    can_config.interrupt_mask = interrupt_mask;
    can_config.txbuf_trans_interrupt_mask = ~0UL;
    status = mcan_init(base, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    has_sent_out = false;
    has_error = false;

    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 15;
    tx_buf.canfd_frame = true;
    tx_buf.bitrate_switch = true;
    msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }
    tx_buf.std_id = 0x123;
    mcan_transmit_via_txbuf_nonblocking(base, 0, &tx_buf);
    while ((!has_sent_out) && (!has_error)) {
    }
    if (has_error) {
        printf("can error happened\n");
    } else {
        printf("no can error happened\n");
    }
    has_error = false;
    handle_can_error(base);
}

void board_can_filter_test(void)
{
    /***********************************************************************************************************
     * NOTE
     * 1. The 'mask' field in can_filter_config_t structure is used for masking corresponding bits
     *    - bit value 1 means the bit will participate into the ID comparison
     *    - bit value 0 means the bit will be ignored during the ID comparison
     *    See the following cases for more details.
     ***********************************************************************************************************/
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_filter_elem_t can_filters[16];
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    printf("CAN Filter test case 0: Classic Filter - only check bit0-2 of CAN STD ID\n");
    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].filter_id = 0;
    can_filters[0].filter_mask = 7;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;
    uint32_t int_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    uint32_t txbuf_int_mask = ~0UL;
    can_config.interrupt_mask = int_mask;
    can_config.txbuf_trans_interrupt_mask = txbuf_int_mask;
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!has_sent_out) {
        }
        has_sent_out = false;
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 256) ? "PASSED" : "Failed");


    printf("CAN Filter test case 1: Classic Filter - only accept message with specified CAN STD ID\n");
    for (uint32_t i = 0; i < ARRAY_SIZE(can_filters); i++) {
        can_filters[i].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
        can_filters[i].filter_config = MCAN_FILTER_ELEM_CFG_SET_PRIORITY_AND_STORE_IN_FIFO0_IF_MATCH;
        can_filters[i].filter_id = (i << 4) | i;
        can_filters[i].filter_mask = 0x7FFUL;
        can_filters[i].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    }

    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = ARRAY_SIZE(can_filters);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        has_sent_out = false;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!has_sent_out) {
        }
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n",
           rcv_msg_cnt,
           (rcv_msg_cnt == ARRAY_SIZE(can_filters)) ? "PASSED" : "Failed");


    printf("CAN Filter test case 2: Range Filter - CAN STD ID\n");
    can_filters[0].filter_type = MCAN_FILTER_TYPE_RANGE_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].start_id = 0;
    can_filters[0].end_id = 15;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    has_new_rcv_msg = false;
    rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!has_sent_out) {
        }
        has_sent_out = false;
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 16) ? "PASSED" : "Failed");

    printf("CAN Filter test case 3: Dual ID Filter - CAN STD ID\n");
    can_filters[0].filter_type = MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].id1 = 0x22;
    can_filters[0].id2 = 0x33;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    has_new_rcv_msg = false;
    rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!has_sent_out) {
        }
        has_sent_out = false;
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        }

    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 2) ? "PASSED" : "Failed");


    printf("CAN Filter test case 4: Dual ID Filter - CAN STD ID\n");
    can_filters[0].filter_type = MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].id1 = 0x22;
    can_filters[0].id2 = 0x33;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    has_new_rcv_msg = false;
    rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!has_sent_out) {
        }
        has_sent_out = false;
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 2) ? "PASSED" : "Failed");


    printf("CAN Filter test case 5: Classic Filter - only check bit0-2 of CAN EXT ID\n");
    mcan_get_default_config(ptr, &can_config);
    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_EXTENDED;
    can_filters[0].filter_id = 0;
    can_filters[0].filter_mask = 7;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 0;
    can_config.all_filters_config.ext_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.ext_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.ext_id_mask = (1UL << 30) - 1UL;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    int_mask = MCAN_EVENT_RECEIVE | MCAN_EVENT_TRANSMIT;
    txbuf_int_mask = ~0UL;
    can_config.interrupt_mask = int_mask;
    can_config.txbuf_trans_interrupt_mask = txbuf_int_mask;

    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    tx_buf.use_ext_id = 1;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    has_new_rcv_msg = false;
    rcv_msg_cnt = 0;
    for (uint32_t i = 0x800; i < 0x1000; i++) {
        tx_buf.ext_id = i;
        mcan_transmit_via_txbuf_nonblocking(ptr, 0, &tx_buf);
        while (!has_sent_out) {
        }
        has_sent_out = false;
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.ext_id);
        }
    }
    printf("Count of received message: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 256) ? "PASSED" : "Failed");
}

void board_can_timestamp_test(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_filter_elem_t can_filters[16];
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    printf("CAN Timestamp Test Case 0: Use 32-bit TSU Timestamp\n");
    /* Timestamp Configuration: Use TSU */
    can_config.use_timestamping_unit = true;
    can_config.tsu_config.prescaler = 256U;
    can_config.tsu_config.enable_tsu = true;
    can_config.tsu_config.capture_on_sof = false;
    can_config.tsu_config.use_ext_timebase = false;

    can_config.timestamp_cfg.counter_prescaler = 16U;
    can_config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED;

    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].sync_message = 1U;
    can_filters[0].filter_id = 0x123;
    can_filters[0].filter_mask = 0x3FF;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    uint32_t int_mask = MCAN_INT_TX_EVT_FIFO_NEW_ENTRY | MCAN_EVENT_RECEIVE;
    mcan_enable_interrupts(ptr, int_mask);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0UL);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    mcan_timestamp_value_t ts_val;
    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    tx_buf.message_marker_l = 0x00;
    tx_buf.timestamp_capture_enable = true;
    tx_buf.event_fifo_control = 1U;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    tx_event_occurred = false;
    has_new_rcv_msg = false;
    tx_buf.std_id = 0x123;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!tx_event_occurred) {
    }
    tx_event_occurred = false;
    while (!has_new_rcv_msg) {
    }
    if (s_can_tx_evt.tx_timestamp_captured) {
        mcan_get_timestamp_from_tx_event(ptr, (const mcan_tx_event_fifo_elem_t *) &s_can_tx_evt, &ts_val);
        printf("TX Timestamp for ID:0x%03x is %s\n", tx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    if (s_can_rx_buf.rx_timestamp_captured != 0U) {
        mcan_get_timestamp_from_received_message(ptr, (const mcan_rx_message_t *) &s_can_rx_buf, &ts_val);
        printf("RX Timestamp for ID:0x%03x is %s\n", s_can_rx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    has_new_rcv_msg = false;
    printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);


    printf("CAN Timestamp Test Case 1: Use 64-bit TSU Timestamp\n");
    /* Timestamp Configuration: Use TSU */
    can_config.use_timestamping_unit = true;
    can_config.tsu_config.prescaler = 1U;
    can_config.tsu_config.enable_tsu = true;
    can_config.tsu_config.capture_on_sof = false;
    can_config.tsu_config.use_ext_timebase = false;
    can_config.tsu_config.enable_64bit_timestamp = true;

    can_config.timestamp_cfg.counter_prescaler = 1;
    can_config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED;

    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].sync_message = 1U;
    can_filters[0].filter_id = 0x456;
    can_filters[0].filter_mask = 0x3FF;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    int_mask = MCAN_INT_TX_EVT_FIFO_NEW_ENTRY | MCAN_EVENT_RECEIVE;
    mcan_enable_interrupts(ptr, int_mask);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0UL);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    tx_buf.message_marker_l = 0x01;
    tx_buf.timestamp_capture_enable = true;
    tx_buf.event_fifo_control = 1U;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    tx_event_occurred = false;
    has_new_rcv_msg = false;
    tx_buf.std_id = 0x456;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!tx_event_occurred) {
    }
    tx_event_occurred = false;
    while (!has_new_rcv_msg) {
    }

    if (s_can_tx_evt.tx_timestamp_captured) {
        mcan_get_timestamp_from_tx_event(ptr, (const mcan_tx_event_fifo_elem_t *) &s_can_tx_evt, &ts_val);
        printf("TX Timestamp for ID:0x%03x is %s\n", tx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    if (s_can_rx_buf.rx_timestamp_captured != 0U) {
        mcan_get_timestamp_from_received_message(ptr, (const mcan_rx_message_t *) &s_can_rx_buf, &ts_val);
        printf("RX Timestamp for ID:0x%03x is %s\n", s_can_rx_buf.std_id, get_timestamp_hex_string(&ts_val));
    }
    has_new_rcv_msg = false;
    printf("New message received, ID=%08x\n", s_can_rx_buf.std_id);


    printf("CAN Timestamp Test Case 2: Use Internal 16-bit Timestamp\n");
    /* Timestamp Configuration: Do not Use TSU */
    can_config.use_timestamping_unit = false;
    can_config.tsu_config.prescaler = 1U;
    can_config.tsu_config.enable_tsu = false;
    can_config.tsu_config.capture_on_sof = false;
    can_config.tsu_config.use_ext_timebase = false;

    can_config.timestamp_cfg.counter_prescaler = 16U;
    can_config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_VALUE_INCREMENT;

    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].sync_message = 1U;
    can_filters[0].filter_id = 0x321;
    can_filters[0].filter_mask = 0x3FF;

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    can_config.all_filters_config.std_id_filter_list.filter_elem_list = &can_filters[0];
    can_config.all_filters_config.std_id_filter_list.mcan_filter_elem_count = 1;
    can_config.all_filters_config.global_filter_config.accept_non_matching_std_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.accept_non_matching_ext_frame_option =
        MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT;
    can_config.all_filters_config.global_filter_config.reject_remote_std_frame = true;
    can_config.all_filters_config.global_filter_config.reject_remote_ext_frame = true;

    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    int_mask = MCAN_EVENT_RECEIVE | MCAN_INT_TX_EVT_FIFO_NEW_ENTRY;
    mcan_enable_interrupts(ptr, int_mask);
    mcan_enable_txbuf_transmission_interrupt(ptr, ~0UL);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    tx_buf.message_marker_l = 0x02;
    tx_buf.event_fifo_control = 1U;
    tx_buf.timestamp_capture_enable = true;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }

    tx_event_occurred = false;
    has_new_rcv_msg = false;
    tx_buf.std_id = 0x321;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!tx_event_occurred) {
    }
    tx_event_occurred = false;
    while (!has_new_rcv_msg) {
    }

    mcan_get_timestamp_from_tx_event(ptr, (const mcan_tx_event_fifo_elem_t *) &s_can_tx_evt, &ts_val);
    printf("TX Timestamp for ID:0x%03x is %s\n", tx_buf.std_id, get_timestamp_hex_string(&ts_val));

    mcan_get_timestamp_from_received_message(ptr, (const mcan_rx_message_t *) &s_can_rx_buf, &ts_val);
    printf("RX Timestamp for ID:0x%03x is %s\n", s_can_rx_buf.std_id, get_timestamp_hex_string(&ts_val));

    mcan_deinit(ptr);
}

void handle_can_error(MCAN_Type *ptr)
{
    mcan_protocol_status_t protocol_status;
    mcan_error_count_t error_count;
    mcan_parse_protocol_status(ptr->PSR, &protocol_status);
    mcan_get_error_counter(ptr, &error_count);
    const char *error_msg = NULL;
    switch (protocol_status.last_error_code) {
    case mcan_last_error_code_no_error:
        error_msg = "No Error";
        break;
    case mcan_last_error_code_stuff_error:
        error_msg = "Stuff Error";
        break;
    case mcan_last_error_code_format_error:
        error_msg = "Format Error";
        break;
    case mcan_last_error_code_ack_error:
        error_msg = "Acknowledge Error";
        break;
    case mcan_last_error_code_bit1_error:
        error_msg = "Bit1 Error: Sent 1 but monitored as 0";
        break;
    case mcan_last_error_code_bit0_error:
        error_msg = "Bit0 Error: Sent 0 but monitored as 1";
        break;
    case mcan_last_error_code_crc_error:
        error_msg = "CRC Error";
        break;
    case mcan_last_error_code_no_change:
        error_msg = "Last Error was not changed";
        break;
    default:
        /* Suppress compiling warning */
        break;
    }
    printf("Last Error: %s\n", error_msg);
    printf("Error Count:\n");
    printf("Transmit Errors: %d\n", error_count.transmit_error_count);
    printf("Receive Errors: %d\n", error_count.receive_error_count);
    if (protocol_status.in_bus_off_state) {
        printf("CAN is in Bus-off mode\n");
    } else if (protocol_status.in_error_passive_state) {
        printf("CAN is in Error Passive Mode\n");
    } else if (protocol_status.in_warning_state) {
        printf("CAN is in Error Warning mode\n");
    } else {
        /* Suppress warnings*/
    }
}

void board_can_timeout_counter_test(void)
{
    /***********************************************************************************************************
     * Case 1: Timeout counter works in continuous mode
     ***********************************************************************************************************/
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_filter_elem_t can_filters[16];
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    uint32_t int_mask = MCAN_INT_TIMEOUT_OCCURRED;
    uint32_t txbuf_int_mask = ~0UL;
    can_config.interrupt_mask = int_mask;
    can_config.txbuf_trans_interrupt_mask = txbuf_int_mask;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel = mcan_timeout_continuous_operation;
    can_config.timeout_cfg.timeout_period = 1000;
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    mcan_reset_timeout_counter_value(ptr);

    while (!timeout_event_occurred) {
    }
    mcan_deinit(ptr);
    printf("CAN Timeout test passed in Continuous mode\n");

    /***********************************************************************************************************
     * Case 2: Timeout counter is triggered by TX Event FIFO
     ***********************************************************************************************************/
    timeout_event_occurred = false;
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    int_mask = MCAN_INT_TIMEOUT_OCCURRED;
    txbuf_int_mask = ~0UL;
    can_config.interrupt_mask = int_mask;
    can_config.txbuf_trans_interrupt_mask = txbuf_int_mask;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel = mcan_timeout_triggered_by_tx_evt_fifo;
    can_config.timeout_cfg.timeout_period = 1000;
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }
    tx_buf.std_id = 0x123;
    tx_buf.event_fifo_control = 1U; /* Store into TX Event FIFO */
    tx_buf.message_marker_l = 0x123 & 0xFF;
    /* Demonstrate the Transmit via TXFIFO in nonblocking mode here */
    mcan_transmit_via_txfifo_nonblocking(ptr, &tx_buf, NULL);
    while (!timeout_event_occurred) {
    }
    if (tx_event_occurred) {
        printf("CAN Timeout test passed in TX EVENT FIFO triggered mode\n");
    } else {
        printf("CAN Timeout test failed in TX EVENT FIFO triggered mode\n");
    }
    tx_event_occurred = false;

    /***********************************************************************************************************
     * Case 3: Timeout counter is triggered by RX FIFO0
     ***********************************************************************************************************/
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    int_mask = MCAN_INT_TIMEOUT_OCCURRED;
    txbuf_int_mask = ~0UL;
    can_config.interrupt_mask = int_mask;
    can_config.txbuf_trans_interrupt_mask = txbuf_int_mask;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel = mcan_timeout_triggered_by_rx_fifo0;
    can_config.timeout_cfg.timeout_period = 1000;
    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].filter_id = 0x123;
    can_filters[0].filter_mask = 0xfff;
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
    can_config.interrupt_mask = int_mask;
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }
    tx_buf.std_id = 0x123;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!timeout_event_occurred) {
    }
    mcan_deinit(ptr);
    timeout_event_occurred = false;
    if (rxfifo0_event_occurred) {
        printf("CAN Timeout test passed in RX FIFO0 triggered mode\n");
    } else {
        printf("CAN Timeout test failed in RX FIFO0 triggered mode\n");
    }
    rxfifo0_event_occurred = false;

    /***********************************************************************************************************
     * Case 4: Timeout counter is triggered by RX FIFO1
     ***********************************************************************************************************/
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    int_mask = MCAN_INT_TIMEOUT_OCCURRED;
    txbuf_int_mask = ~0UL;
    can_config.interrupt_mask = int_mask;
    can_config.txbuf_trans_interrupt_mask = txbuf_int_mask;
    can_config.timeout_cfg.enable_timeout_counter = true;
    can_config.timeout_cfg.timeout_sel = mcan_timeout_triggered_by_rx_fifo1;
    can_config.timeout_cfg.timeout_period = 1000;
    can_filters[0].filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER;
    can_filters[0].filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO1_IF_MATCH;
    can_filters[0].can_id_type = MCAN_CAN_ID_TYPE_STANDARD;
    can_filters[0].filter_id = 0x234;
    can_filters[0].filter_mask = 0xfff;
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
    can_config.interrupt_mask = int_mask;
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data_8[i] = (uint8_t) i | (i << 4);
    }
    tx_buf.std_id = 0x234;
    mcan_transmit_blocking(ptr, &tx_buf);
    while (!timeout_event_occurred) {
    }
    mcan_deinit(ptr);
    timeout_event_occurred = false;
    if (rxfifo1_event_occurred) {
        printf("CAN Timeout test passed in RX FIFO1 triggered mode\n");
    } else {
        printf("CAN Timeout test failed in RX FIFO1 triggered mode\n");
    }
    rxfifo1_event_occurred = false;
}

void board_can_cancel_can_message_send(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    can_info_t *can_info = find_board_can_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
        return;
    }
#endif
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_loopback_internal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);

    /* Test TXBUF cancellation for TXFIFO */
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
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

    /* Test TXBUF cancellation for TXBUF */
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
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
    cancel_cnt = 0;
    for (uint32_t i = 1; i < 4; i++) {
        bool result = mcan_is_tx_buf_cancellation_finished(ptr, i);
        cancel_cnt += (result ? 1 : 0);
    }
    printf("TXBUF cancellation test %s\n", cancel_cnt == 3 ? "PASSED" : "FAILED");
}

void handle_can_test(void)
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
        }
    }
}

void show_help(void)
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
                                    "*                                                                             *\n"
                                    "*******************************************************************************\n";
    printf("%s\n", help_info);
}

char *get_timestamp_hex_string(const mcan_timestamp_value_t *ts_val)
{
    static char hex_str[100];
    memset(hex_str, 0, sizeof(hex_str));
    if (ts_val->is_16bit) {
        sprintf(hex_str, "0x%04x", ts_val->ts_16bit);
    }
    if (ts_val->is_32bit) {
        sprintf(hex_str, "0x%08x", ts_val->ts_32bit);
    }
    if (ts_val->is_64bit) {
        sprintf(hex_str, "0x%08x%08x", ts_val->words[1], ts_val->words[0]);
    }
    if (ts_val->is_empty) {
        sprintf(hex_str, "not exist");
    }
    return hex_str;
}