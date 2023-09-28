/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"

#define TEST_UART_MAX_BUFFER_SIZE            (20U)
#ifndef TEST_UART
#define TEST_UART BOARD_APP_UART_BASE
#define TEST_UART_IRQ BOARD_APP_UART_IRQ
#ifdef BOARD_APP_UART_SRC_FREQ
#define TEST_UART_SRC_FREQ BOARD_APP_UART_SRC_FREQ
#elif defined(BOARD_APP_UART_CLK_NAME)
#define TEST_UART_CLK_NAME BOARD_APP_UART_CLK_NAME
#else
#error unknow UART source frequency
#endif
#endif

static void send_data(uint8_t *buffer, uint8_t *remain_size, uint8_t max_len);

uint8_t buff_index;
uint8_t data_count;
ATTR_PLACE_AT_NONCACHEABLE uint8_t data_buff[TEST_UART_MAX_BUFFER_SIZE];

void uart_isr(void)
{
    uint8_t c;
    uint8_t irq_id = uart_get_irq_id(TEST_UART);
    if (irq_id == uart_intr_id_rx_data_avail) {
        if (status_success != uart_receive_byte(TEST_UART, &c)) {
            while (1) {
            }
        }
        data_buff[buff_index] = c;
        buff_index++;
        if (buff_index >= data_count) {
            uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
            send_data(data_buff, &buff_index, data_count);
            if (buff_index == 0) {
                uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
            }
        }
    }
    if (irq_id == uart_intr_id_tx_slot_avail) {
#if defined(CONFIG_UART_FIFO_MODE) && (CONFIG_UART_FIFO_MODE == 1)
        send_data(data_buff, &buff_index, data_count);
        if (buff_index == 0) {
            uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        }
#else
        uart_write_byte(TEST_UART, data_buff[data_count - buff_index]);
        buff_index--;
        if (buff_index == 0) {
            uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
            uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        }
#endif
    }
}

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)

int main(void)
{
    hpm_stat_t stat;
    buff_index = 0;

    board_init();
    /* if TEST_UART is same as BOARD_CONSOLE_BASE, it has been initialized in board_init(); */
    board_init_uart(TEST_UART);

    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);

    /* The RBR and THR reg has two modes,the FIFO mode and the BUFFER mode
    RBR/THR is a RX/TXFIFO in FIFO mode, RBR/THR is jusr a byte buffer in BUFFER mode
    CONFIG_UART_FIFO_MODE = 1 is means FIFO mode
    CONFIG_UART_FIFO_MODE = 0 or not define CONFIG_UART_FIFO_MODE is means BUFFER mode. */
#if defined(CONFIG_UART_FIFO_MODE) && (CONFIG_UART_FIFO_MODE == 1)
    config.fifo_enable = true;
#else
    config.fifo_enable = false;
#endif
#ifdef TEST_UART_SRC_FREQ
    config.src_freq_in_hz = TEST_UART_SRC_FREQ;
#else
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
#endif

    printf("uart driver example\n");
    if (!config.fifo_enable) {
        data_count = 1;
        printf("non-fifo mode\n");
        printf("uart will send back received characters, echo one by one\n");
    } else {
        data_count = TEST_UART_MAX_BUFFER_SIZE;
        printf("fifo mode\n");
        printf("uart will send back received characters, echo every %d bytes\n", TEST_UART_MAX_BUFFER_SIZE);
    }

    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        /* uart failed to be initialized */
        printf("failed to initialize uart\n");
        while(1);
    }

    uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);

    while(1)
    {
        __asm("wfi;\n");
    }

    return 0;
}

static void send_data(uint8_t *buffer, uint8_t *remain_size, uint8_t max_len)
{
    /* in fifo mode, write data to fifo before enable tx irq best, it can maximun guaranteed fifo usage */
    uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
    uint8_t fifo_size = uart_get_fifo_size(TEST_UART);
    uint8_t tx_size = ((*remain_size) > fifo_size) ? fifo_size : (*remain_size);
    for (uint8_t i = 0; i < tx_size; i++) {
        uart_write_byte(TEST_UART, buffer[max_len - (*remain_size)]);
        (*remain_size)--;
    }
    if ((*remain_size)) {
        uart_enable_irq(TEST_UART, uart_intr_tx_slot_avail);
    }
}
