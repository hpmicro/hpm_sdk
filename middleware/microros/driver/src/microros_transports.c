/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <uxr/client/transport.h>
#include "microros_transports.h"
#include "platform_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool hpm_freertos_serial_open(struct uxrCustomTransport *transport)
{
    uart_context_t *uart = (uart_context_t *) transport->args;

    uart_config_init(uart, 115200);
    return true;
}

bool hpm_freertos_serial_close(struct uxrCustomTransport *transport)
{
    printf("use serial close\n");
    return true;
}

size_t hpm_freertos_serial_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *err)
{
    uart_context_t *uart = (uart_context_t *)transport->args;
    
    uart_send_it(uart, buf, len);
    return 0;
}

size_t hpm_freertos_serial_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *err)
{
    uart_context_t *uart = (uart_context_t *)transport->args;
    uint8_t r_len = 0;
    int time = 0;

    while (uart_receive_is_empty(uart) && time < timeout) {
        vTaskDelay(1);
        time++;
    }

    r_len = uart_receive_read(uart, buf, len);
    return r_len;
}

