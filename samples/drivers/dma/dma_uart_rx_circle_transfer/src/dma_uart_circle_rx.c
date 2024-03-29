/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_uart_drv.h"

/* Define */
#define TEST_UART            BOARD_CONSOLE_UART_BASE
#define TEST_UART_CLK_NAME   BOARD_CONSOLE_UART_CLK_NAME
#define TEST_UART_DMA_SRC_RX BOARD_CONSOLE_UART_RX_DMA_REQ
#define TEST_UART_DMA_SRC_TX BOARD_CONSOLE_UART_TX_DMA_REQ

#define TEST_UART_DMA_CONTROLLER    BOARD_APP_HDMA
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX
#define TEST_UART_RX_DMA_CH         0
#define TEST_UART_TX_DMA_CH         1
#define TEST_UART_RX_DMAMUX_CH      DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH)
#define TEST_UART_TX_DMAMUX_CH      DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH)

#define TEST_BUFFER_SIZE 64

/* dma buffer should be 4-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint8_t uart_tx_buf[TEST_BUFFER_SIZE];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint8_t uart_rx_buf[TEST_BUFFER_SIZE];
/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(8) dma_linked_descriptor_t descriptors[2];
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
    hpm_stat_t stat;
    dma_channel_config_t rx_ch_config = { 0 };
    dma_handshake_config_t tx_ch_config = { 0 };

    /* 1. config uart circle rx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_RX_DMAMUX_CH, TEST_UART_DMA_SRC_RX, true);

    /* 1.1 config chain descriptors */
    dma_default_channel_config(TEST_UART_DMA_CONTROLLER, &rx_ch_config);
    rx_ch_config.src_addr = (uint32_t)&TEST_UART->RBR;
    rx_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE; /*  In DMA handshake case, source width and destination width must be BYTE. */
    rx_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    rx_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    rx_ch_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
    rx_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE; /*  In DMA handshake case, source width and destination width must be BYTE. */
    rx_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    rx_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    rx_ch_config.size_in_byte = TEST_BUFFER_SIZE;
    rx_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T; /*  In DMA handshake case, source burst size must be 1 transfer, that is 0. */
    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[1]);
    stat = dma_config_linked_descriptor(TEST_UART_DMA_CONTROLLER, &descriptors[0], TEST_UART_RX_DMA_CH, &rx_ch_config);
    if (stat != status_success) {
        while (1) {
        };
    }

    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_config_linked_descriptor(TEST_UART_DMA_CONTROLLER, &descriptors[1], TEST_UART_RX_DMA_CH, &rx_ch_config);
    if (stat != status_success) {
        while (1) {
        };
    }

    /* 1.2 config rx dma */
    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_setup_channel(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH, &rx_ch_config, true);
    if (stat != status_success) {
        while (1) {
        };
    }

    /* 2. config uart tx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_TX_DMAMUX_CH, TEST_UART_DMA_SRC_TX, true);
    dma_default_handshake_config(TEST_UART_DMA_CONTROLLER, &tx_ch_config);
    tx_ch_config.ch_index = TEST_UART_TX_DMA_CH;
    tx_ch_config.dst = (uint32_t)&TEST_UART->THR;
    tx_ch_config.dst_fixed = true;
    tx_ch_config.src = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_tx_buf);
    tx_ch_config.src_fixed = false;
    tx_ch_config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    tx_ch_config.size_in_byte = TEST_BUFFER_SIZE;
    dma_setup_handshake(TEST_UART_DMA_CONTROLLER, &tx_ch_config, false);
}

void task_entry_5ms(void)
{
    uint32_t rx_data_size;
    uint32_t dma_remaining_size;

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

int main(void)
{
    board_init();
    init_board_app_uart();
    init_board_app_dma();

    printf("\nDma used link chain to implement circle transmission example");
    printf("\nPlease input some characters, the serial port will echo. Please input characters:\n");

    board_timer_create(5, task_entry_5ms);

    while (1) {
        ;
    }
    return 0;
}
