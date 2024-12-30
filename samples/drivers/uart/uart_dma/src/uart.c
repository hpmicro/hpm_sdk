/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_common.h"

#define TEST_UART                   BOARD_CONSOLE_UART_BASE
#define TEST_UART_CLK_NAME          BOARD_CONSOLE_UART_CLK_NAME
#define TEST_UART_TX_DMA_REQ        BOARD_CONSOLE_UART_TX_DMA_REQ
#define TEST_UART_RX_DMA_REQ        BOARD_CONSOLE_UART_RX_DMA_REQ

#define TEST_UART_DMA_CONTROLLER    BOARD_APP_HDMA
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX
#define TEST_UART_TX_DMA_CHN        (0U)
#define TEST_UART_RX_DMA_CHN        (1U)
#define TEST_UART_TX_DMAMUX_CHN     DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CHN)
#define TEST_UART_RX_DMAMUX_CHN     DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CHN)
#define TEST_UART_DMA_IRQ           BOARD_APP_HDMA_IRQ

#define TEST_BUFFER_SIZE   (16U)
ATTR_PLACE_AT_NONCACHEABLE uint8_t uart_buff[TEST_BUFFER_SIZE];

volatile bool uart_tx_dma_done;
volatile bool uart_rx_dma_done;

hpm_stat_t uart_tx_trigger_dma(DMA_Type *dma_ptr,
                    uint8_t ch_num,
                    UART_Type *uart_ptr,
                    uint32_t src,
                    uint32_t size)
{
    dma_handshake_config_t config;

    dma_default_handshake_config(dma_ptr, &config);
    config.ch_index = ch_num;
    config.dst = (uint32_t)&uart_ptr->THR;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
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

SDK_DECLARE_EXT_ISR_M(TEST_UART_DMA_IRQ, dma_isr)
void dma_isr(void)
{
    volatile hpm_stat_t stat_rx_chn, stat_tx_chn;
    stat_rx_chn = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CHN);
    if (stat_rx_chn & DMA_CHANNEL_STATUS_TC) {
        uart_rx_dma_done = true;
    }
    stat_tx_chn = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CHN);
    if (stat_tx_chn & DMA_CHANNEL_STATUS_TC) {
        uart_tx_dma_done = true;
    }
}

int main(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    board_init();
    printf("UART DMA example\n");
    printf("UART will send back received characters, echo every %d bytes\n", TEST_BUFFER_SIZE);

    /* if TEST_UART is same as BOARD_CONSOLE_BASE, it has been initialized in board_init(); */
    board_init_uart(TEST_UART);
    uart_default_config(TEST_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }

    intc_m_enable_irq_with_priority(TEST_UART_DMA_IRQ, 1);
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_RX_DMAMUX_CHN, TEST_UART_RX_DMA_REQ, true);
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_TX_DMAMUX_CHN, TEST_UART_TX_DMA_REQ, true);

    while (1) {
        /* config rx dma transfer */
        stat = uart_rx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                                    TEST_UART_RX_DMA_CHN,
                                    TEST_UART,
                                    core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_buff),
                                    TEST_BUFFER_SIZE);
        if (stat != status_success) {
            printf("uart rx trigger dma failed\n");
            break;
        }

        while (!uart_rx_dma_done) {
            __asm("nop");
        }
        uart_rx_dma_done = false;

        /* config tx dma transfer */
        stat = uart_tx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                                    TEST_UART_TX_DMA_CHN,
                                    TEST_UART,
                                    core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_buff),
                                    TEST_BUFFER_SIZE);
        if (stat != status_success) {
            printf("uart tx trigger dma failed\n");
            break;
        }

        while (!uart_tx_dma_done) {
            __asm("nop");
        }
        uart_tx_dma_done = false;
    }

    while (1) {
        __asm("nop");
    }

    return 0;
}
