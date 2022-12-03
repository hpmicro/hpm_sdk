/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef UART_DEMO_CONF_H
#define UART_DEMO_CONF_H

#include "hpm_uart_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"

typedef struct uart_info {
    UART_Type *ptr;
    uint32_t baudrate;
    uint32_t *buff_addr;
} uart_info_t;

typedef struct dma_info {
    DMA_Type *ptr;
    uint8_t ch;
    uint32_t *dst_addr;
    uint32_t buff_size;
} dma_info_t;

typedef struct dmamux_info {
    DMAMUX_Type *ptr;
    uint8_t ch;
    uint8_t src;
} dmamux_info_t;

typedef struct ring_buffer_info {
    volatile uint32_t wr;
    volatile uint32_t rd;
    uint8_t* dst;
    uint32_t buffer_ring_size;
    uint32_t one_buf_size;
} ring_buffer_info_t;

/*! @brief Initailize structure of uart to reveive indefinite length data */
typedef struct uart_dma_rx_context {
    uart_info_t uart_info;
    dma_info_t dma_info;
    dmamux_info_t dmamux_info;
    ring_buffer_info_t buf_info;
} uart_dma_rx_context_t;


#endif /* UART_DEMO_CONF_H */

