/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _PLATFORM_UART_H_
#define _PLATFORM_UART_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "board.h"
typedef struct {
    UART_Type* uart_periph;
    clock_name_t uart_clock;
    uint8_t uart_irq;      
    uint8_t *tx_buf;
    uint16_t tx_buf_size;
    uint16_t tx_rd;
    uint16_t tx_wr;
    uint8_t *rx_buf;
    uint16_t rx_buf_size;
    uint16_t rx_rd;
    uint16_t rx_wr;
} uart_context_t;

extern uart_context_t uart_microros_context;
void uart_config_init(uart_context_t *p, uint32_t baud_rate);
void uart_send_it(uart_context_t *p, const void *_send_buf, const uint16_t send_count);
int uart_receive_read(uart_context_t *p, void *_receive_buf, const int receive_count);
unsigned char uart_receive_is_empty(uart_context_t *p);
unsigned char uart_idle_send(unsigned char *buf, unsigned char len);
unsigned char uart_idle_receive(unsigned char *buf, unsigned char len);

#ifdef __cplusplus
}
#endif

#endif