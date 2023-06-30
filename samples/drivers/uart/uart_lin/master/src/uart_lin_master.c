/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_uart_lin.h"

#define TEST_UART              BOARD_UART_LIN
#define TEST_UART_IRQ          BOARD_UART_LIN_IRQ
#define TEST_UART_CLK_NAME     BOARD_UART_LIN_CLK_NAME
#define TEST_UART_LIN_BAUDRATE (19200U)

#define TEST_MASTER_RECEIVE_ID               (0x30)
#define TEST_MASTER_RECEIVE_DATA_LENGTH      (8U)
#define TEST_MASTER_RECEIVE_ENHANCE_CHECKSUM (1)
#define TEST_MASTER_SEND_ID                  (0x31)
#define TEST_MASTER_SEND_DATA_LENGTH         (8U)
#define TEST_MASTER_SEND_ENHANCE_CHECKSUM    (1)

volatile bool uart_lin_send_complete;
volatile bool uart_lin_receive_complete;

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

void uart_isr(void)
{
    volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);
    if (irq_id == uart_intr_id_tx_slot_avail) {
        uart_lin_send_complete = true;
        uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
    }

    if (irq_id == uart_intr_id_rx_timeout) {
        uart_lin_receive_complete = true;
        uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    }
}
SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)


void test_uart_lin_master_interrupt_transfer(void)
{
    /* prepare send data  */
    uint8_t send_data[TEST_MASTER_SEND_DATA_LENGTH] = {0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    /* prepare receive data */
    uint8_t receive_data[TEST_MASTER_RECEIVE_DATA_LENGTH] = {0};

    printf("\nTest uart lin master interrupt transfer\n");
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);

    /* send */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_head(&uart_lin_config);
    hpm_uart_lin_master_send_data(&uart_lin_config);

    uart_enable_irq(TEST_UART, uart_intr_tx_slot_avail);
    while (!uart_lin_send_complete) {
    }

    printf("uart lin master send ID: 0x%x\n", uart_lin_config.id);
    printf("uart send %d data:\n", uart_lin_config.data.length);
    for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
        if (i < uart_lin_config.data.length - 1) {
            printf("0x%x ", send_data[i]);
        } else {
            printf("0x%x\n", send_data[i]);
        }
    }

    /* receive */
    uart_lin_config.id = TEST_MASTER_RECEIVE_ID;
    uart_lin_config.data.buff = receive_data;
    uart_lin_config.data.length = TEST_MASTER_RECEIVE_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_RECEIVE_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_head(&uart_lin_config);

    uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    while (!uart_lin_receive_complete) {
    }
    if (uart_lin_success != hpm_uart_lin_master_receive_data(&uart_lin_config)) {
        printf("uart lin master receive failed\n");
        return;
    }

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

void test_uart_lin_master_polling_transfer(void)
{
    /* prepare send data  */
    uint8_t send_data[TEST_MASTER_SEND_DATA_LENGTH] = {0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    /* prepare receive data */
    uint8_t receive_data[TEST_MASTER_RECEIVE_DATA_LENGTH] = {0};

    printf("Test uart lin master polling transfer\n");

    /* send */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    if (uart_lin_success != hpm_uart_lin_master_send_frame(&uart_lin_config)) {
        printf("uart lin master send frame failed\n");
        return;
    }

    printf("uart lin master send ID: 0x%x\n", uart_lin_config.id);
    printf("uart send %d data:\n", uart_lin_config.data.length);
    for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
        if (i < uart_lin_config.data.length - 1) {
            printf("0x%x ", send_data[i]);
        } else {
            printf("0x%x\n", send_data[i]);
        }
    }

    /* receive */
    uart_lin_config.id = TEST_MASTER_RECEIVE_ID;
    uart_lin_config.data.buff = receive_data;
    uart_lin_config.data.length = TEST_MASTER_RECEIVE_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_RECEIVE_ENHANCE_CHECKSUM;
    if (uart_lin_success != hpm_uart_lin_master_receive_frame(&uart_lin_config)) {
        printf("uart lin master receive frame failed\n");
        return;
    }

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

int main(void)
{
    board_init();
    board_init_uart(TEST_UART);

    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.baudrate = TEST_UART_LIN_BAUDRATE;
    /* config rx fifo level to avoid to generate interrupt before receive all lin frame data */
    config.rx_fifo_level = uart_rx_fifo_trg_gt_three_quarters;

    if (status_success != uart_init(TEST_UART, &config)) {
        printf("failed to initialize uart\n");
    }

    test_uart_lin_master_polling_transfer();

    test_uart_lin_master_interrupt_transfer();

    while (1) {
    }

    return 0;
}
