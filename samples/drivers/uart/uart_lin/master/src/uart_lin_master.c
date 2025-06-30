/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_uart_lin.h"

/* UART configuration defines */
#define TEST_UART              BOARD_UART_LIN
#define TEST_UART_IRQ          BOARD_UART_LIN_IRQ
#define TEST_UART_CLK_NAME     BOARD_UART_LIN_CLK_NAME
#define TEST_UART_LIN_BAUDRATE (19200U)

/* LIN frame configuration for master receive */
#define TEST_MASTER_RECEIVE_ID               (0x30)  /* ID for receiving data */
#define TEST_MASTER_RECEIVE_DATA_LENGTH      (8U)    /* Length of data to receive */
#define TEST_MASTER_RECEIVE_ENHANCE_CHECKSUM (1)     /* Use enhanced checksum */

/* LIN frame configuration for master send */
#define TEST_MASTER_SEND_ID                  (0x31)  /* ID for sending data */
#define TEST_MASTER_SEND_DATA_LENGTH         (8U)    /* Length of data to send */
#define TEST_MASTER_SEND_ENHANCE_CHECKSUM    (1)     /* Use enhanced checksum */

/* Status flags for interrupt handling */
volatile bool uart_lin_send_complete;      /* Flag to indicate send completion */
volatile bool uart_lin_receive_complete;   /* Flag to indicate receive completion */

/* UART LIN master configuration structure */
uart_lin_master_config_t uart_lin_config = {
    .ptr = TEST_UART,
    .pin_ctrl = {
        .ptr = HPM_GPIO0,
        .baudrate = TEST_UART_LIN_BAUDRATE,
        .config_uart_pin = init_uart_pins,
        .config_uart_pin_as_gpio = init_uart_pin_as_gpio,
        .delay_us = board_delay_us,
        .tx_port = BOARD_UART_LIN_TX_PORT,
        .tx_pin = BOARD_UART_LIN_TX_PIN,
    },
};

/* UART interrupt handler */
SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
    volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);
    /* Handle TX slot available interrupt */
    if (irq_id == uart_intr_id_tx_slot_avail) {
        uart_lin_send_complete = true;
        uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
    }

    /* Handle RX timeout interrupt */
    if (irq_id == uart_intr_id_rx_timeout) {
        uart_lin_receive_complete = true;
        uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    }
}

/**
 * @brief Test UART LIN master interrupt transfer
 *
 * This function demonstrates LIN master communication using interrupt mode:
 * 1. Sends a LIN frame with test data
 * 2. Receives a LIN frame from slave
 */
void test_uart_lin_master_interrupt_transfer(void)
{
    /* Prepare send data buffer */
    uint8_t send_data[TEST_MASTER_SEND_DATA_LENGTH] = {0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    /* Prepare receive data buffer */
    uint8_t receive_data[TEST_MASTER_RECEIVE_DATA_LENGTH] = {0};

    printf("\nTest uart lin master interrupt transfer\n");
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);

    /* Send data frame */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_head(&uart_lin_config);
    hpm_uart_lin_master_send_data(&uart_lin_config);

    /* Enable TX interrupt and wait for completion */
    uart_enable_irq(TEST_UART, uart_intr_tx_slot_avail);
    while (!uart_lin_send_complete) {
    }

    /* Print send results */
    printf("uart lin master send ID: 0x%x\n", uart_lin_config.id);
    printf("uart send %d data:\n", uart_lin_config.data.length);
    for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
        if (i < uart_lin_config.data.length - 1) {
            printf("0x%x ", send_data[i]);
        } else {
            printf("0x%x\n", send_data[i]);
        }
    }

    /* Receive data frame */
    uart_lin_config.id = TEST_MASTER_RECEIVE_ID;
    uart_lin_config.data.buff = receive_data;
    uart_lin_config.data.length = TEST_MASTER_RECEIVE_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_RECEIVE_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_head(&uart_lin_config);

    /* Enable RX interrupt and wait for completion */
    uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    while (!uart_lin_receive_complete) {
    }
    if (uart_lin_success != hpm_uart_lin_master_receive_data(&uart_lin_config)) {
        printf("uart lin master receive failed\n");
        return;
    }

    /* Print receive results */
    printf("uart lin master send ID: 0x%x\n", uart_lin_config.id);
    printf("uart receive %d data:\n", uart_lin_config.data.length);
    for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
        if (i < uart_lin_config.data.length - 1) {
            printf("0x%x ", receive_data[i]);
        } else {
            printf("0x%x\n", receive_data[i]);
        }
    }
}

/**
 * @brief Test UART LIN master polling transfer
 *
 * This function demonstrates LIN master communication using polling mode:
 * 1. Sends a LIN frame with test data
 * 2. Receives a LIN frame from slave
 * All operations are blocking until completion
 */
void test_uart_lin_master_polling_transfer(void)
{
    /* Prepare send data buffer */
    uint8_t send_data[TEST_MASTER_SEND_DATA_LENGTH] = {0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    /* Prepare receive data buffer */
    uint8_t receive_data[TEST_MASTER_RECEIVE_DATA_LENGTH] = {0};

    printf("Test uart lin master polling transfer\n");

    /* Send data frame */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    if (uart_lin_success != hpm_uart_lin_master_send_frame(&uart_lin_config)) {
        printf("uart lin master send frame failed\n");
        return;
    }

    /* Print send results */
    printf("uart lin master send ID: 0x%x\n", uart_lin_config.id);
    printf("uart send %d data:\n", uart_lin_config.data.length);
    for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
        if (i < uart_lin_config.data.length - 1) {
            printf("0x%x ", send_data[i]);
        } else {
            printf("0x%x\n", send_data[i]);
        }
    }

    /* Receive data frame */
    uart_lin_config.id = TEST_MASTER_RECEIVE_ID;
    uart_lin_config.data.buff = receive_data;
    uart_lin_config.data.length = TEST_MASTER_RECEIVE_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_RECEIVE_ENHANCE_CHECKSUM;
    if (uart_lin_success != hpm_uart_lin_master_receive_frame(&uart_lin_config)) {
        printf("uart lin master receive frame failed\n");
        return;
    }

    /* Print receive results */
    printf("uart lin master send ID: 0x%x\n", uart_lin_config.id);
    printf("uart receive %d data:\n", uart_lin_config.data.length);
    for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
        if (i < uart_lin_config.data.length - 1) {
            printf("0x%x ", receive_data[i]);
        } else {
            printf("0x%x\n", receive_data[i]);
        }
    }
}

/**
 * @brief Main function
 *
 * Program entry point that:
 * 1. Initializes board and UART
 * 2. Configures UART for LIN communication
 * 3. Tests both polling and interrupt transfer modes
 */
int main(void)
{
    /* Initialize board and UART */
    board_init();
    board_init_uart(TEST_UART);
#if defined(BOARD_SUPPORT_LIN_TRANSCEIVER_CONTROL) && (BOARD_SUPPORT_LIN_TRANSCEIVER_CONTROL == 1)
    board_lin_transceiver_control(true);
#endif

    /* Configure UART */
    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.baudrate = TEST_UART_LIN_BAUDRATE;
    /* Configure RX FIFO level to avoid interrupt before receiving all LIN frame data */
    config.rx_fifo_level = uart_rx_fifo_trg_gt_three_quarters;

    if (status_success != uart_init(TEST_UART, &config)) {
        printf("failed to initialize uart\n");
    }

    /* Run test cases */
    test_uart_lin_master_polling_transfer();
    test_uart_lin_master_interrupt_transfer();

    while (1) {
    }

    return 0;
}
