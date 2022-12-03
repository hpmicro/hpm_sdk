/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_common.h"
#include "uart_demo_conf.h"
#include "uart.h"

#define TEST_UART BOARD_UART_IDLE
#define TEST_UART_BOUDRATE (1000000UL)
#define TEST_DMA BOARD_APP_HDMA
#define TEST_DMA_CHANNEL (0U) /* Corresponding to DMAMUX CHANNEL*/

#define TEST_DMAMUX BOARD_APP_DMAMUX
#define TEST_DMAMUX_CHANNEL (0U)
#define TEST_DMAMUX_SRC BOARD_UART_IDLE_DMA_SRC

#define BUFF_SIZE (1024U)
#define UART_DMA_ADDR_ALIGNMENT (8U)
#define UART_BUFFER_INDEX_SIZE (4000)

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(UART_DMA_ADDR_ALIGNMENT)
uint8_t uart_rx_buff[UART_BUFFER_INDEX_SIZE][BUFF_SIZE];

uart_dma_rx_context_t uart_context = {
    .uart_info = {
        .ptr = TEST_UART,
        .baudrate = TEST_UART_BOUDRATE,
    },
    .dma_info = {
        .ptr = TEST_DMA,
        .ch = TEST_DMA_CHANNEL,
        .buff_size = BUFF_SIZE,
    },
    .dmamux_info = {
        .ptr = TEST_DMAMUX,
        .ch = TEST_DMAMUX_CHANNEL,
        .src = TEST_DMAMUX_SRC,
    },
    .buf_info = {
        .wr = 0,
        .rd = 0,
        .dst = &uart_rx_buff[0][0],
        .buffer_ring_size = UART_BUFFER_INDEX_SIZE,
        .one_buf_size = BUFF_SIZE,
    },
};

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

    return dma_setup_handshake(dma_ptr, &config, true);
}

void config_uart(uart_info_t *uart_info)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    board_init_uart(uart_info->ptr);
    uart_default_config(uart_info->ptr, &config);
    config.baudrate = uart_info->baudrate;
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = board_init_uart_clock(uart_info->ptr);
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty; /* this config should not change */
    stat = uart_init(uart_info->ptr, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
}

void configure_uart_dma_transfer(uart_info_t *uart_info,
    dma_info_t *dma_info,
    dmamux_info_t *dmamux_info,
    ring_buffer_info_t *buf_info)
{
    hpm_stat_t stat;

    dmamux_config(dmamux_info->ptr, dmamux_info->ch, dmamux_info->src, true);
    stat = uart_rx_trigger_dma(dma_info->ptr,
        dma_info->ch,
        uart_info->ptr,
        core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)buf_info->dst + buf_info->wr * buf_info->one_buf_size),
        dma_info->buff_size);

    if (stat != status_success) {
        printf("uart_rx_trigger_dma function failed\n");
        while (1) {
        }
    }
}

void isr_hdma(void)
{
    ring_buffer_info_t *buf_info = &uart_context.buf_info;
    volatile uint32_t stat = dma_get_irq_status(BOARD_APP_HDMA);
    dma_clear_irq_status(BOARD_APP_HDMA, stat);
    buf_info->wr++;
    buf_info->wr = buf_info->wr % (buf_info->buffer_ring_size - 1);
    if (buf_info->wr == buf_info->rd) {
        printf("over flow!!!!!!!!!!!!!!!!!!!!!\n");
        buf_info->rd++;
        buf_info->rd = buf_info->rd % (buf_info->buffer_ring_size - 1);
    }
    configure_uart_dma_transfer(&uart_context.uart_info, &uart_context.dma_info, &uart_context.dmamux_info, &uart_context.buf_info);
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_hdma)

int uart_dma_rx_init(void)
{
    config_uart(&uart_context.uart_info);

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, 1);
    /* config dma to transfer uart rx data */
    configure_uart_dma_transfer(&uart_context.uart_info, &uart_context.dma_info, &uart_context.dmamux_info, &uart_context.buf_info);

    return 0;
}

/*
    Copy data to dst_buf from uart_rx_buff,
    At most *avilable_len bytes
 */
size_t get_uart_data(uint8_t *dst_buf, size_t *avilable_len)
{
    ring_buffer_info_t *buf_info = &uart_context.buf_info;
    size_t data_size = 0;
    if (buf_info->rd == buf_info->wr) {
        return 0;
    }

    while (buf_info->rd != buf_info->wr) {
        if (*avilable_len >= buf_info->one_buf_size) {
            memcpy(dst_buf, uart_rx_buff[buf_info->rd], buf_info->one_buf_size);
            *avilable_len -= buf_info->one_buf_size;
            dst_buf += buf_info->one_buf_size;
            data_size += buf_info->one_buf_size;
            buf_info->rd++;
            buf_info->rd = buf_info->rd % (buf_info->buffer_ring_size - 1);
        } else {
            break;
        }
    }

    printf("%d / %d left %d\n", buf_info->wr, buf_info->rd,
        (buf_info->buffer_ring_size + buf_info->wr - buf_info->rd) % buf_info->buffer_ring_size);
    return data_size;
}