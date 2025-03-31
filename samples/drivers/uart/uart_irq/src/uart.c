/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"

#define TEST_UART_MAX_BUFFER_SIZE            (20)
#ifndef TEST_UART
#define TEST_UART BOARD_CONSOLE_UART_BASE
#define TEST_UART_IRQ BOARD_CONSOLE_UART_IRQ
#ifdef BOARD_CONSOLE_UART_SRC_FREQ
#define TEST_UART_SRC_FREQ BOARD_CONSOLE_UART_SRC_FREQ
#elif defined(BOARD_CONSOLE_UART_CLK_NAME)
#define TEST_UART_CLK_NAME BOARD_CONSOLE_UART_CLK_NAME
#else
#error unknow UART source frequency
#endif
#endif

uint8_t buff_index;
uint8_t data_count;
ATTR_PLACE_AT_NONCACHEABLE uint8_t data_buff[TEST_UART_MAX_BUFFER_SIZE];

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
    uint8_t irq_id = uart_get_irq_id(TEST_UART);
    if (irq_id == uart_intr_id_rx_data_avail) {
        while (uart_check_status(TEST_UART, uart_stat_data_ready)) {
            data_buff[buff_index++] = uart_read_byte(TEST_UART);
        }
        uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        uart_enable_irq(TEST_UART, uart_intr_tx_slot_avail);
    }
    if (irq_id == uart_intr_id_tx_slot_avail) {
        for (uint8_t i = 0; i < buff_index; i++) {
            uart_write_byte(TEST_UART, data_buff[i]);
        }
        buff_index = 0;
        uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
        uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    }
}

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
    RBR/THR is a RX/TXFIFO in FIFO mode, RBR/THR is just a byte buffer in BUFFER mode
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
        printf("use non-fifo mode\n");
        printf("uart will send back received characters, echo one by one\n");
    } else {
        config.rx_fifo_level = uart_rx_fifo_trg_gt_half;
        data_count = (uart_get_fifo_size(TEST_UART) / 2);
        printf("use fifo mode, the uart RX/TX FIFO depth:%d bytes \n",  uart_get_fifo_size(TEST_UART));
        printf("set fifo threshold to %d bytes; uart will send back received characters on every %d bytes\n", data_count, data_count);
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

