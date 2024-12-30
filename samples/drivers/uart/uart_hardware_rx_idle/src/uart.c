/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"

#define TEST_UART                     BOARD_CONSOLE_UART_BASE
#define TEST_UART_IRQ                 BOARD_CONSOLE_UART_IRQ
#define TEST_UART_BAUDRATE            BOARD_CONSOLE_UART_BAUDRATE
#define TEST_UART_CLK_NAME            BOARD_CONSOLE_UART_CLK_NAME

#define TEST_DMA                      BOARD_APP_HDMA
#define TEST_DMA_CHANNEL              (0U)
#define TEST_DMAMUX                   BOARD_APP_DMAMUX
#define TEST_DMAMUX_CHANNEL           DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_DMA, TEST_DMA_CHANNEL)
#define TEST_DMAMUX_SRC               BOARD_CONSOLE_UART_RX_DMA_REQ

#define BUFF_SIZE                     (1024U)
uint8_t uart_buff[BUFF_SIZE];
ATTR_PLACE_AT_NONCACHEABLE uint8_t dma_buff[BUFF_SIZE];

volatile bool uart_rx_idle;

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
    if (uart_is_rxline_idle(TEST_UART)) {
        uart_rx_idle = true;
        uart_clear_rxline_idle_flag(TEST_UART);
        uart_flush(TEST_UART);
    }
}

hpm_stat_t uart_rx_trigger_dma(DMA_Type *dma_ptr,
                                uint8_t ch_num,
                                UART_Type *uart_ptr,
                                uint32_t dst,
                                uint32_t size)
{
    dma_handshake_config_t config;

    dma_default_handshake_config(dma_ptr, &config);
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&uart_ptr->RBR;
    config.src_fixed = true;
    config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

/* config uart: enable dma and rx idle detect */
void config_uart(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    /* if TEST_UART is same as BOARD_CONSOLE_BASE, it has been initialized in board_init(); */
    board_init_uart(TEST_UART);
    uart_default_config(TEST_UART, &config);
    config.baudrate = TEST_UART_BAUDRATE;
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty; /* this config should not change */
    config.rxidle_config.detect_enable = true;
    config.rxidle_config.detect_irq_enable = true;
    config.rxidle_config.idle_cond = uart_rxline_idle_cond_rxline_logic_one;
    config.rxidle_config.threshold = 20U; /* 20bit */
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
    }

    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);
}

/* config dma to receive uart data */
void configure_uart_dma_transfer(void)
{
    hpm_stat_t stat;

    dmamux_config(TEST_DMAMUX, TEST_DMAMUX_CHANNEL, TEST_DMAMUX_SRC, true);
    stat = uart_rx_trigger_dma(TEST_DMA,
                            TEST_DMA_CHANNEL,
                            TEST_UART,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_buff),
                            BUFF_SIZE);
    if (stat != status_success) {
        printf("uart_rx_trigger_dma function failed\n");
    }
}


int main(void)
{
    uint32_t uart_received_data_count;
    board_init();
    printf("uart hardware rx idle detection.\n");
    config_uart();
    configure_uart_dma_transfer();

    while (1) {
        while (!uart_rx_idle) {
            __asm("nop");
        }

        /* disable dma channel, abort residue transmission */
        dma_disable_channel(TEST_DMA, TEST_DMA_CHANNEL);

        /* print uart received data */
        uart_rx_idle = false;
        uart_received_data_count = BUFF_SIZE - dma_get_remaining_transfer_size(TEST_DMA, TEST_DMA_CHANNEL);
        memcpy(uart_buff, dma_buff, uart_received_data_count);
        printf("uart receive %d bytes, the received data are:", uart_received_data_count);
        for (uint32_t i = 0; i < uart_received_data_count; i++) {
            if ((i % 10) == 0) {
                printf("\n");
            }
            printf("%c ", uart_buff[i]);
        }
        printf("\n");

        /* configure new dma transmission */
        configure_uart_dma_transfer();
    }

    return 0;
}
