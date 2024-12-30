/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"

#define TEST_UART BOARD_APP_UART_BASE
#define TEST_UART_IRQ BOARD_APP_UART_IRQ
#define TEST_UART_CLK_NAME BOARD_APP_UART_CLK_NAME

#define TEST_UART_CORRECT_ADDR (0x77U)
#define TEST_UART_WRONG_ADDR   (0x76U)

volatile bool uart_rx_idle;

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
    if (uart_is_rxline_idle(TEST_UART)) {
        uart_rx_idle = true;
        uart_clear_rxline_idle_flag(TEST_UART);
    }
}

void uart_sent_and_receive_data(uint8_t address, uint8_t *data, uint8_t length)
{
    uint8_t count;

    /* uart sent address and data */
    printf("uart send address: 0x%02x and data:\n", address);
    uart_write_byte(TEST_UART, address);
    uart_send_data(TEST_UART, data, length);

    while (!uart_rx_idle) {
    }

    /* uart check received data in fifo */
    count = uart_get_data_count_in_rx_fifo(TEST_UART);
    if (count > 0) {
        printf("uart receive data:\n");
        for (uint8_t i = 0; i < count; i++) {
            printf("0x%02x ", uart_read_byte(TEST_UART));
        }
        printf("\n");
    } else {
        printf("uart not receive any data\n");
    }
    uart_rx_idle = false;
}

int main(void)
{
    hpm_stat_t stat;
    uint8_t data0[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
    uint8_t data1[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

    board_init();
    board_init_uart(TEST_UART);

    printf("uart 9bit address match transmit example\n");

    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    config.baudrate = 115200U;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.fifo_enable = true;
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        /* uart failed to be initialized */
        printf("failed to initialize uart\n");
        return 1;
    }

    /* uart address match function require enable idle dection */
    uart_rxline_idle_config_t idle_config = {0};
    idle_config.detect_enable = true;
    idle_config.threshold = 20U; /* 20 bit */
    uart_init_rxline_idle_detection(TEST_UART, idle_config);
    uart_init_txline_idle_detection(TEST_UART, idle_config);

    /* config 9bit mode and address */
    uart_enable_9bit_transmit_mode(TEST_UART, true);
    uart_enable_address0_match(TEST_UART, TEST_UART_CORRECT_ADDR);

    /* enable irq */
    uart_enable_irq(TEST_UART, uart_intr_rx_line_idle);
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);

    /* First send the address and data, then check if the sent data has been received */
    uart_sent_and_receive_data(TEST_UART_WRONG_ADDR, data0, 8);

    board_delay_ms(100);

    /* First send the address and data, then check if the sent data has been received */
    uart_sent_and_receive_data(TEST_UART_CORRECT_ADDR, data1, 8);

    while (1) {
    }

    return 0;
}
