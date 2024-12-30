/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_can_drv.h"


typedef struct {
    CAN_Type *can_base;
    uint32_t clock_freq;
    int32_t irq_index;
} can_info_t;

/**
 * @brief Compare the content between Transmission buffer and Receive buffer
 *
 * @param [in] tx_buf Transmission buffer
 * @param [in] rx_buf Receive buffer
 * @return true Content is identical
 * @return false Content was mismatch
 */
static bool can_buf_compare(const can_transmit_buf_t *tx_buf, const can_receive_buf_t *rx_buf);

/**
 * @brief Decode the CAN messge size from DLC
 *
 * @param [in] dlc Data Length Code
 * @return decoded CAN message size in bytes
 */
static uint8_t can_get_data_bytes_from_dlc(uint32_t dlc);

/**
 * @brief CAN loopback test
 *
 * @param [in] base CAN base address
 * @return true Test passed
 * @return false Test failed
 */
bool can_loopback_test(CAN_Type *base);

/**
 * @brief CANFD loopback test
 *
 * @param [in] base CAN base address
 * @return true Test passed
 * @return false Test failed
 */
bool canfd_loopback_test(CAN_Type *base);

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
 * @brief CAN multiple CAN message transmission test
 */
void board_can_send_multiple_can_messages(void);

/**
 * @brief CAN multiple CANFD message transmission test
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
 * @brief Get the can error kind string
 *
 * @param [in] error_kind Error code from IP
 * @return string for specified CAN error
 */
static const char *get_can_error_kind_str(uint8_t error_kind);

/**
 * @brief Display the CAN message content
 *
 * @param [in] rx_msg CAN message buffer
 */
void show_received_can_message(const can_receive_buf_t *rx_msg);

/**
 * @brief CAN Interrupt Service Routine
 */
void board_can_isr(void);

/**
 * @brief Handler for CAN test options
 */
void handle_can_test(void);

void show_help(void);

static can_info_t s_can_info[] = {
        { .can_base = HPM_CAN0, .irq_index = IRQn_CAN0 },
        { .can_base = HPM_CAN1, .irq_index = IRQn_CAN1 },
#if defined(HPM_CAN2)
        { .can_base = HPM_CAN2, .irq_index = IRQn_CAN2 },
#endif
#if defined (HPM_CAN3)
        { .can_base = HPM_CAN3, .irq_index = IRQn_CAN3 },
#endif
};

static volatile  bool has_new_rcv_msg;
static volatile  bool has_sent_out;
static volatile  bool has_error;
static volatile  can_receive_buf_t s_can_rx_buf;
static volatile  uint8_t error_flags;

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, board_can_isr)
void board_can_isr(void)
{
    uint8_t flags = can_get_tx_rx_flags(BOARD_APP_CAN_BASE);
    if ((flags & CAN_EVENT_RECEIVE) != 0) {
        can_read_received_message(BOARD_APP_CAN_BASE, (can_receive_buf_t *)&s_can_rx_buf);
        has_new_rcv_msg = true;
    }
    if ((flags & (CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF))) {
        has_sent_out = true;
    }
    if ((flags & CAN_EVENT_ERROR) != 0) {
        has_error = true;
    }
    can_clear_tx_rx_flags(BOARD_APP_CAN_BASE, flags);

    error_flags = can_get_error_interrupt_flags(BOARD_APP_CAN_BASE);
    if (error_flags != 0) {
        has_error = true;
    }
    can_clear_error_interrupt_flags(BOARD_APP_CAN_BASE, error_flags);
}

void show_received_can_message(const can_receive_buf_t *rx_msg)
{
    uint32_t msg_len = can_get_data_bytes_from_dlc(rx_msg->dlc);
    printf("CAN message info:\nID=%08x\nContent=:\n", rx_msg->id);
    uint32_t remaining_size = msg_len;
    uint32_t print_size;

    for (uint32_t i = 0; i < msg_len; i += 16) {
        print_size = MIN(remaining_size, 16);
        for (uint32_t j = 0; j < print_size; j++) {
            printf("%02x ", rx_msg->data[i + j]);
        }
        printf("\n");
        remaining_size -= print_size;
    }
}


int main(void)
{
    board_init();

    /* Initialize CAN */
    for (uint32_t i=0; i < ARRAY_SIZE(s_can_info); i++) {
        can_info_t  *info = &s_can_info[i];
        board_init_can(info->can_base);
        info->clock_freq = board_init_can_clock(info->can_base);
    }

    handle_can_test();

    return 0;
}

static bool can_buf_compare(const can_transmit_buf_t *tx_buf, const can_receive_buf_t *rx_buf)
{
    bool result = false;

    do {
        HPM_BREAK_IF(tx_buf->id != rx_buf->id);
        HPM_BREAK_IF(tx_buf->dlc != rx_buf->dlc);
        HPM_BREAK_IF(tx_buf->extend_id != rx_buf->extend_id);
        HPM_BREAK_IF(tx_buf->remote_frame != rx_buf->remote_frame);
        bool data_matched = true;

        uint32_t data_bytes = can_get_data_bytes_from_dlc(rx_buf->dlc);
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

static uint8_t can_get_data_bytes_from_dlc(uint32_t dlc)
{
    uint32_t data_bytes = 0;

    dlc &= 0xFU;
    if (dlc <= 8U) {
        data_bytes = dlc;
    }
    else {
        switch (dlc) {
        case can_payload_size_12:
            data_bytes = 12U;
            break;
        case can_payload_size_16:
            data_bytes = 16U;
            break;
        case can_payload_size_20:
            data_bytes = 20U;
            break;
        case can_payload_size_24:
            data_bytes = 24U;
            break;
        case can_payload_size_32:
            data_bytes = 32U;
            break;
        case can_payload_size_48:
            data_bytes = 48U;
            break;
        case can_payload_size_64:
            data_bytes = 64U;
            break;
        default:
            /* Code should never touch here */
            break;
        }
    }

    return data_bytes;
}

bool can_loopback_test(CAN_Type *base)
{
    uint32_t error_cnt = 0;
    bool result = false;
    can_transmit_buf_t tx_buf;
    can_receive_buf_t rx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    memset(&rx_buf, 0, sizeof(rx_buf));

    tx_buf.id = 0x7f;
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);
    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CAN loopback test for standard frame %s\n", result ? "passed" : "failed");

    tx_buf.extend_id = 1U;
    tx_buf.id = 0x12345678U;

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);

    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CAN loopback test for extend frame %s\n", result ? "passed" : "failed");

    return (error_cnt < 1);
}

bool canfd_loopback_test(CAN_Type *base)
{
    uint32_t error_cnt = 0;
    bool result = false;
    can_transmit_buf_t tx_buf;
    can_receive_buf_t rx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    memset(&rx_buf, 0, sizeof(rx_buf));

    tx_buf.id = 0x7f;
    tx_buf.dlc = 15;
    tx_buf.bitrate_switch = 1;
    tx_buf.canfd_frame = 1;

    uint32_t msg_bytes = can_get_data_bytes_from_dlc(tx_buf.id);
    for (uint32_t i = 0; i < msg_bytes; i++) {
        tx_buf.data[i] = i;
    }

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);
    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CANFD loopback test for standard frame %s\n", result ? "passed" : "failed");

    tx_buf.extend_id = 1U;
    tx_buf.id = 0x12345678U;

    can_send_high_priority_message_blocking(base, &tx_buf);
    can_receive_message_blocking(base, &rx_buf);

    result = can_buf_compare(&tx_buf, &rx_buf);
    if (!result) {
        error_cnt++;
    }
    printf("    CANFD loopback test for extend frame %s\n", result ? "passed" : "failed");

    return (error_cnt < 1);
}

void can_loopback_test_for_all_cans(void)
{
    can_config_t can_config;
    hpm_stat_t status;
    for (uint32_t i = 0; i < ARRAY_SIZE(s_can_info); i++) {
        can_get_default_config(&can_config);
        can_config.baudrate = 1000000; /* 1Mbps */
        can_config.mode = can_mode_loopback_internal;
        status = can_init(s_can_info[i].can_base, &can_config, s_can_info[i].clock_freq);
        assert(status == status_success);
        intc_m_disable_irq(s_can_info[i].irq_index);
        (void)status; /* Suppress compiling warning in release build */
        bool can_result = can_loopback_test(s_can_info[i].can_base);

        can_config.baudrate_fd = 2000000;
        can_config.enable_canfd = true;
        status = can_init(s_can_info[i].can_base, &can_config, s_can_info[i].clock_freq);
        assert(status == status_success);
        bool canfd_result = canfd_loopback_test(s_can_info[i].can_base);
        printf("    CAN%d loopback test %s\n", i, (can_result && canfd_result) ? "PASSED" : "FAILED");

    }
}

void board_can_loopback_test_in_interrupt_mode(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = can_mode_loopback_internal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF;
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        can_send_message_nonblocking(BOARD_APP_CAN_BASE, &tx_buf);
        while (!has_sent_out) {
        }

        while (!has_new_rcv_msg) {

        }
        has_new_rcv_msg = false;
        has_sent_out = false;
        printf("New message received, ID=%08x\n", s_can_rx_buf.id);
    }
}

void board_can_echo_test_initiator(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = can_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    can_enable_tx_rx_irq(ptr, CAN_EVENT_RECEIVE);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    tx_buf.id = 0x123;
    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    printf("Can Echo test: Initiator is sending message out...\n");
    status = can_send_message_blocking(ptr, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }
    printf("Waiting for echo message...\n");
    while (!has_new_rcv_msg) {
    }
    has_new_rcv_msg = false;
    show_received_can_message((const can_receive_buf_t *)&s_can_rx_buf);
}

void board_can_echo_test_responder(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = can_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    can_enable_tx_rx_irq(ptr, CAN_EVENT_RECEIVE);
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);
    printf("CAN echo test: Responder is waiting for echo message...\n");
    while (!has_new_rcv_msg) {
    }
    has_new_rcv_msg = false;
    show_received_can_message((const can_receive_buf_t *)&s_can_rx_buf);

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = s_can_rx_buf.dlc;
    tx_buf.id = 0x321;
    uint32_t msg_len = can_get_data_bytes_from_dlc(s_can_rx_buf.dlc);
    memcpy(&tx_buf.data, (uint8_t *)&s_can_rx_buf.data, msg_len);
    status = can_send_message_blocking(ptr, &tx_buf);
    if (status != status_success) {
        printf("CAN sent message failed, error_code:%d\n", status);
        return;
    }
    printf("Sent echo message back\n");
}


void board_can_send_multiple_can_messages(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = can_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = can_get_data_bytes_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = i | (i << 4);
    }
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        status = can_send_message_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent message failed, error_code:%d\n", status);
            return;
        }
    }
    printf("Sent CAN messages with ID from 0 to 2047 out\n");
}

void board_can_send_multiple_canfd_messages(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    /* Use baudrate setting directly, may not work on CAN-FD if some nodes selects different sampling points */
#if defined(USE_BAUDRATE_FOR_CANFD) && (USE_USE_BAUDRATE_FOR_CANFD == 1)
    can_config.baudrate = 500000; /* 500kbps */
    can_config.baudrate_fd = 5000000; /* 5Mbps */
#else
    /* Assume the CAN clock is 80MHz, configure the nominal baudrate to 500kbit/s, configure the canfd baudrate to 5Mbit/s
     *
     * sample point:  100% * num_seg1 / (num_seg1 + num_seg2)
     * In the below example,
     *      the sample point of arbitration phase is: 100% * 60 / (60 + 20) = 75%
     *      the sample point of data phase is: 100% * 12 / (12 + 4) = 80%
     */
    can_config.use_lowlevel_timing_setting = true;
    /* bitrate = can_src_clk_freq / prescaler / (num_seg1 + num_seg2) */
    can_config.can_timing.num_seg1 = 60;
    can_config.can_timing.num_seg2 = 20;
    can_config.can_timing.num_sjw = 16;
    can_config.can_timing.prescaler = 2;
    can_config.canfd_timing.num_seg1 = 12;
    can_config.canfd_timing.num_seg2  = 4;
    can_config.canfd_timing.num_sjw = 4;
    can_config.canfd_timing.prescaler = 1;
#endif
    can_config.mode = can_mode_normal;
    can_config.enable_canfd = true;
    can_config.enable_tdc = true;

    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = can_payload_size_64;
    tx_buf.bitrate_switch = 1;
    tx_buf.canfd_frame = 1;
    uint32_t msg_len = can_get_data_bytes_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = i;
    }
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        status = can_send_message_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent message failed, error_code:%d\n", status);
            return;
        }
    }
    printf("Sent CANFD messages with ID from 0 to 2047 out\n");
}

void board_can_error_test(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = can_mode_normal;
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF | CAN_EVENT_ERROR;
    can_config.irq_error_enable_mask = CAN_ERROR_ARBITRATION_LOST_INT_ENABLE | CAN_ERROR_PASSIVE_INT_ENABLE | CAN_ERROR_BUS_ERROR_INT_ENABLE;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = can_get_data_bytes_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = i | (i << 4);
    }
    tx_buf.id = 0x123;

    has_error = false;
    has_sent_out = false;
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
}

void board_can_error_test_with_no_retransmission(void)
{
    CAN_Type *ptr = BOARD_APP_CAN_BASE;
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.baudrate = 500000; /* 500kbps */
    can_config.mode = can_mode_normal;
    can_config.disable_stb_retransmission = true;
    /* Due to re-transmission is disabled, user should enable interrupt to confirm either transmission completes or interrupt occurs */
    can_config.irq_txrx_enable_mask = CAN_EVENT_RECEIVE | CAN_EVENT_TX_PRIMARY_BUF | CAN_EVENT_TX_SECONDARY_BUF | CAN_EVENT_ERROR;
    can_config.irq_error_enable_mask = CAN_ERROR_ARBITRATION_LOST_INT_ENABLE | CAN_ERROR_PASSIVE_INT_ENABLE | CAN_ERROR_BUS_ERROR_INT_ENABLE;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    hpm_stat_t status = can_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = can_get_data_bytes_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data[i] = i | (i << 4);
    }
    tx_buf.id = 0x123;

    has_error = false;
    has_sent_out = false;
    can_send_message_nonblocking(ptr, &tx_buf);
    /* Wait transmission interrupt or error interrupt */
    while ((!has_sent_out) && (!has_error)) {
    }
    /* Here software can check has_error flag to confirm whether error happened or not */
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
    can_deinit(ptr);
}

static const char *get_can_error_kind_str(uint8_t error_kind)
{
    const char *error_info_str = NULL;
    switch (error_kind) {
    case CAN_KIND_OF_ERROR_NO_ERROR:
        error_info_str = "No error";
        break;
    case CAN_KIND_OF_ERROR_BIT_ERROR:
        error_info_str = "Bit error";
        break;
    case CAN_KIND_OF_ERROR_FORM_ERROR:
        error_info_str = "Form error";
        break;
    case CAN_KIND_OF_ERROR_STUFF_ERROR:
        error_info_str = "Stuff error";
        break;
    case CAN_KIND_OF_ERROR_ACK_ERROR:
        error_info_str = "ACK error";
        break;
    case CAN_KIND_OF_ERROR_CRC_ERROR:
        error_info_str = "CRC error";
        break;
    case CAN_KIND_OF_ERROR_OTHER_ERROR:
        error_info_str = "Other errors";
        break;
    default:
        error_info_str = "Uknown error";
        break;
    }

    return error_info_str;
}

void board_can_filter_test(void)
{
    /***********************************************************************************************************
     * NOTE
     *
     * 1. The CAN filter can only be configured with the CAN IP is in reset mode, it is recommended to configure
     * the CAN filters in can_init() API instead of separate can_set_filter API call, otherwise, users need to
     * take care of registers bits reset by the can_reset() API.
     *
     * 2. The 'mask' field in can_filter_config_t structure is used for masking corresponding bits
     *    - bit value 1 means the bit will be ignored during ID comparison
     *    - bit value 0 means the bit will participate into the ID comparison
     *    See the following cases for more details.
     ***********************************************************************************************************/
    can_filter_config_t can_filters[16];
    can_config_t can_config;
    CAN_Type *ptr = BOARD_APP_CAN_BASE;

    printf("CAN Filter test case 0: only check bit0 of CAN ID\n");
    can_filters[0].enable = true;
    can_filters[0].index = 0;
    can_filters[0].id_mode = can_filter_id_mode_both_frames;
    can_filters[0].code = 0;
    /* Only filter the bit 0 of CAN ID */
    can_filters[0].mask = 0x3FFFFFFEUL;
    can_get_default_config(&can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
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
    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 1);

    can_transmit_buf_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;

    for (uint32_t i = 0; i < 8; i++) {
        tx_buf.data[i] = (uint8_t) i | (i << 4);
    }

    has_sent_out = false;
    has_new_rcv_msg = false;
    uint32_t rcv_msg_cnt = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.id = i;
        can_send_message_nonblocking(ptr, &tx_buf);
        while (!has_sent_out) {
        }
        has_sent_out = false;
        board_delay_ms(1);
        if (has_new_rcv_msg) {
            rcv_msg_cnt++;
            has_new_rcv_msg = false;
            printf("New message received, ID=%08x\n", s_can_rx_buf.id);
        }
    }
    printf("Received message count: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == 1024) ? "PASSED" : "Failed");
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
    printf("Received message count: %d, %s\n", rcv_msg_cnt, (rcv_msg_cnt == ARRAY_SIZE(can_filters)) ? "PASSED" : "Failed");
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
        }
    }
}

void show_help(void)
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
                                    "*                                                                               *\n"
                                    "*********************************************************************************\n";
    printf("%s\n", help_info);
}
