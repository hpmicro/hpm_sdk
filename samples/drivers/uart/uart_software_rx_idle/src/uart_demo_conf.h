/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef UART_DEMO_CONF_H
#define UART_DEMO_CONF_H

#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#include "hpm_gptmr_drv.h"
#include "hpm_uart_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"

typedef struct uart_info {
    UART_Type *ptr;
    uint32_t baudrate;
    uint8_t *buff_addr;
} uart_info_t;

typedef struct dma_info {
    DMA_Type *ptr;
    uint8_t ch;
    uint8_t *dst_addr;
    uint32_t buff_size;
} dma_info_t;

typedef struct dmamux_info {
    DMAMUX_Type *ptr;
    uint8_t ch;
    uint8_t src;
} dmamux_info_t;

typedef struct trgm_info {
    TRGM_Type *ptr;
    uint32_t pin_index;
    uint8_t input_src;
    uint8_t output_gptmr_in;
    uint8_t output_gptmr_synci;
} trgm_info_t;

typedef struct gptmr_info {
    GPTMR_Type *ptr;
    clock_name_t clock_name;
    uint8_t cmp_ch;
    uint8_t cap_ch;
    uint8_t irq_index;
} gptmr_info_t;

/*! @brief Initailize structure of uart to reveive indefinite length data */
typedef struct uart_rx_flexiable_data_context {
    volatile bool uart_rx_idle;
    volatile uint32_t uart_receive_data_size;
    uart_info_t uart_info;
    dma_info_t dma_info;
    dmamux_info_t dmamux_info;
    trgm_info_t trgm_info;
    gptmr_info_t gptmr_info;
} uart_rx_flexiable_data_context_t;

#endif /* UART_DEMO_CONF_H */

