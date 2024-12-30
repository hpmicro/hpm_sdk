/*
 * Copyright (c) 2023 HPMicro
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

#define TEST_UART                   BOARD_APP_UART_BASE
#define TEST_UART_IRQ               BOARD_APP_UART_IRQ
#define TEST_UART_CLK_NAME          BOARD_APP_UART_CLK_NAME
#define TEST_UART_TX_DMA_REQ        BOARD_APP_UART_TX_DMA_REQ

#define TEST_UART_DMA_CONTROLLER    BOARD_APP_HDMA
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX
#define TEST_UART_TX_DMA_CHN        (0U)
#define TEST_UART_TX_DMAMUX_CHN     DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CHN)
#define TEST_UART_DMA_IRQ           BOARD_APP_HDMA_IRQ

#define TEST_BUFFER_SIZE   (10240U)
ATTR_PLACE_AT_NONCACHEABLE uint8_t uart_buff[TEST_BUFFER_SIZE];

uint16_t buff_index;
volatile bool uart_tx_dma_done;
volatile bool uart_rx_fifo_timeout;
volatile uint16_t rx_interrupt_count;

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

void uart_isr(void);
void dma_isr(void);

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
SDK_DECLARE_EXT_ISR_M(TEST_UART_DMA_IRQ, dma_isr)

void uart_isr(void)
{
    uint8_t count = 0;
    uint8_t irq_id = uart_get_irq_id(TEST_UART);
    if (irq_id == uart_intr_id_rx_data_avail) {
        rx_interrupt_count++;
          while (uart_check_status(TEST_UART, uart_stat_data_ready)) {
            count++;
            uart_buff[buff_index++] = uart_read_byte(TEST_UART);
            /*in order to ensure rx fifo there are remaining bytes*/
            if (count > 12) {
                break;
            }
        }
    }

    if (irq_id == uart_intr_id_rx_timeout) {
        rx_interrupt_count++;
        while ((uart_check_status(TEST_UART, uart_stat_data_ready)) || (uart_check_status(TEST_UART, uart_stat_overrun_error))) {
            uart_buff[buff_index++] = uart_read_byte(TEST_UART);
        }
        uart_rx_fifo_timeout = true;
    }
}

void dma_isr(void)
{
    volatile hpm_stat_t stat_tx_chn;
    stat_tx_chn = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CHN);
    if (stat_tx_chn & DMA_CHANNEL_STATUS_TC) {
        uart_tx_dma_done = true;
    }
}

int main(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};
    buff_index = 0;
    uart_tx_dma_done = false;
    uart_rx_fifo_timeout = 0;
    rx_interrupt_count = 0;
    board_init();
    printf("UART RX FIFO timeout example\n");

    board_init_uart(TEST_UART);
    clock_add_to_group(TEST_UART_CLK_NAME, 0);
    uart_default_config(TEST_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    /* rx fifo level should more than 13 bytes */
#if defined(HPM_IP_FEATURE_UART_FINE_FIFO_THRLD) && (HPM_IP_FEATURE_UART_FINE_FIFO_THRLD == 1)
    config.rx_fifo_level = uart_fifo_14_bytes;
#else
    config.rx_fifo_level = uart_rx_fifo_trg_gt_three_quarters;
#endif
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
    uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 2);
    intc_m_enable_irq_with_priority(TEST_UART_DMA_IRQ, 1);
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_TX_DMAMUX_CHN, TEST_UART_TX_DMA_REQ, true);

    while (1) {

        while (!uart_rx_fifo_timeout) {
            __asm("nop");
        }
        uart_rx_fifo_timeout = false;

        /* config tx dma transfer */
        stat = uart_tx_trigger_dma(TEST_UART_DMA_CONTROLLER,
                                    TEST_UART_TX_DMA_CHN,
                                    TEST_UART,
                                    core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_buff),
                                    buff_index);
        if (stat != status_success) {
            printf("uart tx trigger dma failed\n");
            break;
        }

        while (!uart_tx_dma_done) {
            __asm("nop");
        }
        uart_tx_dma_done = false;
        printf("uart rx data size:%d interrupt count is :%d \r\n", buff_index, rx_interrupt_count);
        rx_interrupt_count = 0;
        buff_index = 0;
    }

    while (1) {
        __asm("nop");
    }

    return 0;
}
