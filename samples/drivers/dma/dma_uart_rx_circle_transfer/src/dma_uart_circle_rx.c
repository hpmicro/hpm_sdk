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
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_l1c_drv.h"

/* Define */
#define TEST_UART BOARD_APP_UART_BASE
#define TEST_UART_CLK_NAME BOARD_APP_UART_CLK_NAME
#define TEST_UART_DMA_CONTROLLER BOARD_APP_HDMA
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX

#define TEST_BUFFER_SIZE HPM_L1C_CACHELINE_SIZE

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

    uart_default_config(TEST_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    board_init_uart(TEST_UART);
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        while (1) {
        }
    }
}

void init_board_app_dma(void)
{
    hpm_stat_t stat;
    dma_channel_config_t ch0_config = { 0 };
    dma_handshake_config_t ch1_config = { 0 };

/* 1. config uart circle rx dma */
    /* 1.1 set chain descriptors */
    for (uint32_t i = 0; i < ARRAY_SIZE(descriptors); i++) {
        descriptors[i].trans_size = TEST_BUFFER_SIZE;
        descriptors[i].src_addr = (uint32_t)&TEST_UART->RBR;
        descriptors[i].dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
        descriptors[i].ctrl = DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_HANDSHAKE)
                            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
                            | DMA_CHCTRL_CTRL_SRCREQSEL_SET(DMAMUX_MUXCFG_HDMA_MUX0)
                            | DMA_CHCTRL_CTRL_DSTREQSEL_SET(DMAMUX_MUXCFG_HDMA_MUX0);
    }
    descriptors[0].linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[1]);
    descriptors[1].linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);

    /* 1.2 config rx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX0, HPM_DMA_SRC_UART0_RX, true);
    ch0_config.src_addr = (uint32_t)&TEST_UART->RBR;
    ch0_config.src_width = DMA_TRANSFER_WIDTH_BYTE;  /*  In DMA handshake case, source width and destination width must be BYTE. */
    ch0_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch0_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch0_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
    ch0_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;  /*  In DMA handshake case, source width and destination width must be BYTE. */
    ch0_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch0_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch0_config.size_in_byte = TEST_BUFFER_SIZE;
    ch0_config.src_burst_size = 0;  /*  In DMA handshake case, source burst size must be 1 transfer, that is 0. */
    ch0_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_setup_channel(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX0, &ch0_config, true);
    if (stat != status_success) {
        while (1) {
        };
    }

/* 2. config uart tx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1, HPM_DMA_SRC_UART0_TX, true);
    ch1_config.ch_index = DMAMUX_MUXCFG_HDMA_MUX1;
    ch1_config.dst = (uint32_t)&TEST_UART->THR;
    ch1_config.dst_fixed = true;
    ch1_config.src = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_tx_buf);
    ch1_config.src_fixed = false;
    ch1_config.size_in_byte = TEST_BUFFER_SIZE;
    dma_setup_handshake(TEST_UART_DMA_CONTROLLER, &ch1_config, false);
}

void task_entry_5ms(void)
{
    uint32_t rx_data_size;
    uint32_t dma_residue_size;

    dma_residue_size = dma_get_residue_transfer_size(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX0);

    rx_rear_index = TEST_BUFFER_SIZE - dma_residue_size;
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
        if (!dma_channel_is_transferring(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1)) {
            dma_set_transfer_size(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1, rx_data_size);
            dma_set_source_address(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_tx_buf));
            dma_enable_channel(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1);
        }
    }
}

int main(void)
{
    board_init();
    init_board_app_uart();
    init_board_app_dma();

    board_timer_create(5, task_entry_5ms);

    while (1) {
        ;
    }
    return 0;
}
