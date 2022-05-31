/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_common.h"




#define TEST_UART BOARD_APP_UART_BASE
#define TEST_UART_IRQ BOARD_APP_UART_IRQ
#define TEST_UART_CLK_NAME BOARD_APP_UART_CLK_NAME
#define TEST_UART_DMA_CONTROLLER BOARD_APP_HDMA
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX


#define ALL_LETTERS_NUMBER (26U)
#define TEST_BUFFER_SIZE (HPM_L1C_CACHELINE_SIZE*2)
#define UART_DMA_ADDR_ALIGNMENT  (4U)
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(UART_DMA_ADDR_ALIGNMENT) uint8_t uart_tx_buf[TEST_BUFFER_SIZE];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(UART_DMA_ADDR_ALIGNMENT) uint8_t uart_rx_buf[TEST_BUFFER_SIZE];
/* The DMA interrupt is generated only when the serial port receives UART_DMA_TRIGGER_LEVEL bytes. */
#define UART_DMA_TRIGGER_LEVEL (1U)

hpm_stat_t uart_tx_trigger_dma(DMA_Type *dma_ptr,
                    uint8_t ch_num,
                    UART_Type *uart_ptr,
                    uint32_t src,
                    uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = (uint32_t)&uart_ptr->THR;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config);
}

hpm_stat_t uart_rx_trigger_dma(DMA_Type *dma_ptr,
                    uint8_t ch_num,
                    UART_Type *uart_ptr,
                    uint32_t dst,
                    uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&uart_ptr->RBR;
    config.src_fixed = true;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config);
}


void dma_isr(void)
{
    volatile hpm_stat_t stat_channel0, stat_channel1;

    stat_channel0 = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX0);
    if (stat_channel0 & DMA_CHANNEL_STATUS_TC) {
        printf("\r\n Channel 0 transfers done! \r\n");
    }

    stat_channel1 = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1);
    if (stat_channel1 & DMA_CHANNEL_STATUS_TC) {
        printf("\r\n Channel 1 transfers done! \r\n");

        /* The received data is used here. */
        for (int i = 0; i < UART_DMA_TRIGGER_LEVEL; i++) {
            printf("%c\r\n", uart_rx_buf[i]);
        }
        uart_rx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                            DMAMUX_MUXCFG_HDMA_MUX1,
                            TEST_UART,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE,
                            (uint32_t)uart_rx_buf),
                            UART_DMA_TRIGGER_LEVEL);
        if (l1c_dc_is_enabled()) {
            l1c_dc_invalidate((uint32_t)uart_rx_buf, TEST_BUFFER_SIZE);
        }
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, dma_isr)



int main(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    board_init();
    uart_default_config(TEST_UART, &config);

    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    board_init_uart(TEST_UART);
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, 1);

    for (int i = 0; i < TEST_BUFFER_SIZE; i++) {
        /* Set the data to be sent. */
        uart_tx_buf[i] = 'A' + i%ALL_LETTERS_NUMBER;
        if (l1c_dc_is_enabled()) {
            l1c_dc_flush((uint32_t) uart_tx_buf, TEST_BUFFER_SIZE);
        }
    }

    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX0, HPM_DMA_SRC_UART0_TX, true);
    /* Sending data for the first time */
    stat = uart_tx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                                                 DMAMUX_MUXCFG_HDMA_MUX0,
                                                 TEST_UART,
                                                 core_local_mem_to_sys_address(BOARD_RUNNING_CORE,
                                                 (uint32_t)uart_tx_buf),
                                                 TEST_BUFFER_SIZE);
    if (stat != status_success) {
        printf("uart_tx_trigger_dma function failed\n");
        while (1) {
        }
    }

    /* Some delay times is needed here, otherwise the device is busy. */
    board_delay_ms(500);

    /* Sending data for the second time */
    stat = uart_tx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                                                 DMAMUX_MUXCFG_HDMA_MUX0,
                                                 TEST_UART,
                                                 core_local_mem_to_sys_address(BOARD_RUNNING_CORE,
                                                 (uint32_t)uart_tx_buf),
                                                 TEST_BUFFER_SIZE);
    if (stat != status_success) {
        printf("uart_tx_trigger_dma function failed\n");
        while (1) {
        }
    }

    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, DMAMUX_MUXCFG_HDMA_MUX1, HPM_DMA_SRC_UART0_RX, true);
    stat = uart_rx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                                                 DMAMUX_MUXCFG_HDMA_MUX1,
                                                 TEST_UART,
                                                 core_local_mem_to_sys_address(BOARD_RUNNING_CORE,
                                                 (uint32_t)uart_rx_buf),
                                                 UART_DMA_TRIGGER_LEVEL);
    if (stat != status_success) {
        printf("uart_rx_trigger_dma function failed\n");
        while (1) {
        }
    }

    while (1) {
        __asm("wfi;\n");
    }

    return 0;
}
