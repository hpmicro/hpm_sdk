/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "platform_uart.h"
#include "hpm_uart_drv.h"

static ATTR_PLACE_AT_NONCACHEABLE uint8_t uart_tx_buf[1024];
static ATTR_PLACE_AT_NONCACHEABLE uint8_t uart_rx_buf[1024];

uart_context_t uart_microros_context = {
    BOARD_MICROROS_UART_BASE,
    BOARD_MICROROS_UART_CLK_NAME,
    BOARD_MICROROS_UART_IRQ,
    uart_tx_buf,
    sizeof(uart_tx_buf),
    0,
    0,
    uart_rx_buf,
    sizeof(uart_rx_buf),
    0,
    0
};

void uart_config_init(uart_context_t *p, uint32_t baud_rate)
{
    hpm_stat_t stat;

    uart_config_t config = {0};
    uart_default_config(p->uart_periph, &config);
    config.baudrate = baud_rate;
    config.fifo_enable = false;
    config.src_freq_in_hz = clock_get_frequency(p->uart_clock);

    stat = uart_init(p->uart_periph, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }

    uart_enable_irq(p->uart_periph, uart_intr_rx_data_avail_or_timeout);
    uart_disable_irq(p->uart_periph, uart_intr_tx_slot_avail);
    intc_m_enable_irq_with_priority(p->uart_irq, 7);
    printf("hpm_freertos serial config ok\n");
}

void uart_send_it(uart_context_t *p, const void *_send_buf, const uint16_t send_count)
{
    const uint8_t *send_buf = (const uint8_t *)_send_buf;
    uint16_t i;

    for (i = 0; i < send_count; i++) {
        while ((p->tx_wr + 1) % p->tx_buf_size == p->tx_rd);
        p->tx_buf[p->tx_wr++] = send_buf[i];
        p->tx_wr %= p->tx_buf_size;
    }

    uart_enable_irq(p->uart_periph, uart_intr_tx_slot_avail);
}

int uart_receive_read(uart_context_t *p, void *_receive_buf, const int receive_count)
{
    uint8_t *receive_buf = (uint8_t *)_receive_buf;
    int i, receive_count_real;

    for (i = 0, receive_count_real = 0; i < receive_count; i++) {
        if (p->rx_rd == p->rx_wr) {
            return receive_count_real;
        } else {
            receive_buf[i] = p->rx_buf[p->rx_rd++];
            p->rx_rd %= p->rx_buf_size;
            receive_count_real++;
        }
    }
    return receive_count_real;
}

unsigned char uart_receive_is_empty(uart_context_t *p)
{
    if (p->rx_rd == p->rx_wr)
        return 1;

    return 0;
}

static void uart_rx_avail_it(uart_context_t *p)
{
    uart_receive_byte(p->uart_periph, &p->rx_buf[p->rx_wr]);
    p->rx_wr++;
    p->rx_wr %= p->rx_buf_size;

    if (p->rx_wr == p->rx_rd) {
        p->rx_rd++;
        p->rx_rd %= p->rx_buf_size;
    }
}

static void uart_tx_avail_it(uart_context_t *p)
{
    if (p->tx_rd != p->tx_wr) {
        uart_send_byte(p->uart_periph, p->tx_buf[p->tx_rd++]);
        p->tx_rd %= p->tx_buf_size;
    if (p->tx_rd == p->tx_wr)
        uart_disable_irq(p->uart_periph, uart_intr_tx_slot_avail);
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_MICROROS_UART_IRQ, uart_isr)
void uart_isr(void)
{
    uart_context_t *p = &uart_microros_context;
    uint8_t irq_id = uart_get_irq_id(p->uart_periph);
    if (irq_id == uart_intr_id_rx_data_avail) {
        uart_rx_avail_it(p);
    }
    if (irq_id == uart_intr_id_tx_slot_avail) {
        uart_tx_avail_it(p);
    }
}

unsigned char uart_idle_send(unsigned char *buf, unsigned char len)
{
    uart_send_it(&uart_microros_context, buf, len);
    return 0;
}

unsigned char uart_idle_receive(unsigned char *buf, unsigned char len)
{
    return uart_receive_read(&uart_microros_context, buf, len);
}
