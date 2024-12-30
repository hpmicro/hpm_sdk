/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_dmav2_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_interrupt.h"

/* Define */
#define TEST_UART            BOARD_CONSOLE_UART_BASE
#define TEST_UART_CLK_NAME   BOARD_CONSOLE_UART_CLK_NAME
#define TEST_UART_DMA_SRC_RX BOARD_CONSOLE_UART_RX_DMA_REQ
#define TEST_UART_DMA_SRC_TX BOARD_CONSOLE_UART_TX_DMA_REQ

#define TEST_UART_DMA_CONTROLLER    TEST_DMA_CONTROLLER
#define TEST_UART_DMA_IRQ           TEST_DMA_IRQ
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX
#define TEST_UART_RX_DMA_CH         30
#define TEST_UART_TX_DMA_CH         31
#define TEST_UART_RX_DMAMUX_CH      DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH)
#define TEST_UART_TX_DMAMUX_CH      DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH)

#define TEST_BUFFER_SIZE 64

/* dma buffer should be 4-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint8_t uart_tx_buf[TEST_BUFFER_SIZE];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint8_t uart_rx_buf[TEST_BUFFER_SIZE];
static uint32_t rx_front_index;
static uint32_t rx_rear_index;

void init_board_app_uart(void)
{
    hpm_stat_t stat;
    uart_config_t config = { 0 };

    board_init_uart(TEST_UART);

    uart_default_config(TEST_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        while (1) {
        }
    }
}

void init_board_app_dma(void)
{
    dma_handshake_config_t ch_config = { 0 };

    /* 1. config uart circle rx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_RX_DMAMUX_CH, TEST_UART_DMA_SRC_RX, true);
    ch_config.ch_index = TEST_UART_RX_DMA_CH;
    ch_config.dst = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
    ch_config.dst_fixed = false;
    ch_config.src = (uint32_t)&TEST_UART->RBR;
    ch_config.src_fixed = true;
    ch_config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.size_in_byte = TEST_BUFFER_SIZE;
    ch_config.en_infiniteloop = true;
    ch_config.interrupt_mask = DMA_INTERRUPT_MASK_NONE; /* enable irq */
    dma_setup_handshake(TEST_UART_DMA_CONTROLLER, &ch_config, true);

    /* 2. config uart normal tx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_TX_DMAMUX_CH, TEST_UART_DMA_SRC_TX, true);
    ch_config.ch_index = TEST_UART_TX_DMA_CH;
    ch_config.dst = (uint32_t)&TEST_UART->THR;
    ch_config.dst_fixed = true;
    ch_config.src = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_tx_buf);
    ch_config.src_fixed = false;
    ch_config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.size_in_byte = TEST_BUFFER_SIZE;
    ch_config.en_infiniteloop = false;
    ch_config.interrupt_mask = DMA_INTERRUPT_MASK_ALL;
    dma_setup_handshake(TEST_UART_DMA_CONTROLLER, &ch_config, false);

    /* 3. enable irq */
    intc_m_enable_irq_with_priority(TEST_UART_DMA_IRQ, 1);
}

SDK_DECLARE_EXT_ISR_M(TEST_UART_DMA_IRQ, dma_isr)
void dma_isr(void)
{
    uint32_t status;
    uint32_t rx_data_size;
    uint32_t dma_remaining_size;

    status = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH);

    if (((status & DMA_CHANNEL_STATUS_HALF_TC) != 0) || ((status & DMA_CHANNEL_STATUS_TC) != 0)) {
        dma_remaining_size = dma_get_remaining_transfer_size(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH);

        rx_rear_index = TEST_BUFFER_SIZE - dma_remaining_size;
        if (rx_front_index > rx_rear_index) {
            rx_data_size = TEST_BUFFER_SIZE + rx_rear_index - rx_front_index;
        } else {
            rx_data_size = rx_rear_index - rx_front_index;
        }

        for (uint32_t i = 0; i < rx_data_size; i++) {
            if ((rx_front_index + i) < TEST_BUFFER_SIZE) {
                uart_tx_buf[i] = uart_rx_buf[rx_front_index + i];
            } else {
                uart_tx_buf[i] = uart_rx_buf[rx_front_index + i - TEST_BUFFER_SIZE];
            }
        }
        rx_front_index = rx_rear_index;

        if (rx_data_size > 0) {
            if (!dma_channel_is_enable(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH)) {
                dma_set_transfer_size(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH, rx_data_size);
                dma_set_source_address(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_tx_buf));
                dma_enable_channel(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH);
            }
        }
    }
}

int main(void)
{
    board_init();
    init_board_app_uart();
    init_board_app_dma();

    printf("\nDmav2 half transmission complete example");
    printf("\nWhen input a multiple of 32 characters, the serial port will echo. Please input characters:\n");

    while (1) {
        ;
    }
    return 0;
}
