/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_uart_lin.h"

#define TEST_UART              BOARD_UART_LIN
#define TEST_UART_IRQ          BOARD_UART_LIN_IRQ
#define TEST_UART_CLK_NAME     BOARD_UART_LIN_CLK_NAME
#define TEST_UART_LIN_BAUDRATE (19200U)

#define TEST_SLAVE_RECEIVE_ID               (0x31)
#define TEST_SLAVE_RECEIVE_DATA_LENGTH      (8U)
#define TEST_SLAVE_RECEIVE_ENHANCE_CHECKSUM (1)
#define TEST_SLAVE_SEND_ID                  (0x30)
#define TEST_SLAVE_SEND_DATA_LENGTH         (8U)
#define TEST_SLAVE_SEND_ENHANCE_CHECKSUM    (1)
#define UART_LIN_FRANME_DATA_COUNT_MAX      (8U)


volatile bool uart_lin_receive_head_error; /* receive 0x55 and pid error: 0x55 not match or pid parity error  */
volatile bool uart_lin_receive_data_error; /* receive data: data count error or checksum error */
volatile bool uart_lin_receive_complete;   /* receive data complete */
volatile bool uart_lin_send_complete;      /* send data complete */
volatile uint8_t head_index;
uint8_t head_buff[2];                       /* store received 0x55 and pid */
uint8_t receive_data[UART_LIN_FRANME_DATA_COUNT_MAX]; /* store the data to be sent */
uint8_t send_data[UART_LIN_FRANME_DATA_COUNT_MAX];    /* store the received data */

uart_lin_slave_config_t uart_lin_config = {
    .ptr = TEST_UART,
};

/* uart fifo control */
uart_fifo_ctrl_t fifo_ctrl_1 = {
    .tx_fifo_level = uart_tx_fifo_trg_not_full,
    .rx_fifo_level = uart_rx_fifo_trg_not_empty,
    .dma_enable = false,
    .fifo_enable = true,
    .reset_tx_fifo = false,
    .reset_rx_fifo = false,
};

/* uart fifo control */
uart_fifo_ctrl_t fifo_ctrl_2 = {
    .tx_fifo_level = uart_tx_fifo_trg_not_full,
    .rx_fifo_level = uart_rx_fifo_trg_gt_three_quarters,
    .dma_enable = false,
    .fifo_enable = true,
    .reset_tx_fifo = false,
    .reset_rx_fifo = false,
};

void uart_lin_check_head_and_respond(UART_Type *ptr, uint8_t *head_buff);

void uart_isr(void)
{
    volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);

    /* detect lin break signal */
    if (irq_id == uart_intr_id_rx_line_stat) {
        if (uart_check_status(TEST_UART, uart_stat_line_break)) {
            /* clear data in rx fifo */
            uart_clear_rx_fifo(TEST_UART);
            head_index = 0; /* clear head index */
            /* set rx fifo trigger level and enable interrupt */
            uart_config_fifo_ctrl(TEST_UART, &fifo_ctrl_1);
            uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        }
    }

    /* receive first two bytes and check them  */
    if (irq_id == uart_intr_id_rx_data_avail) {
        if (uart_check_status(TEST_UART, uart_stat_data_ready)) {
            head_buff[head_index++] = uart_read_byte(TEST_UART);
        }

        if (head_index == 2) {
            uart_lin_check_head_and_respond(TEST_UART, head_buff);
        }
    }

    /* sent data complete */
    if (irq_id == uart_intr_id_tx_slot_avail) {
        uart_lin_send_complete = true;
        uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
    }

    /* receive data complete */
    if (irq_id == uart_intr_id_rx_timeout) {
        uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        if (uart_lin_success == hpm_uart_lin_slave_receive_data(&uart_lin_config)) {
            uart_lin_receive_complete = true;
        } else {
            uart_lin_receive_data_error = true;
        }
    }

}
SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)

void uart_lin_check_head_and_respond(UART_Type *ptr, uint8_t *head_buff)
{
    uint8_t pid = head_buff[1];
    uint8_t id = pid & 0x3f;

    if (0x55 != head_buff[0]) {
        uart_disable_irq(ptr, uart_intr_rx_data_avail_or_timeout); /* No longer receiving data */
        uart_lin_receive_head_error = true;
        return;
    }

    if (pid != hpm_uart_lin_calculate_protected_id(id)) {
        uart_disable_irq(ptr, uart_intr_rx_data_avail_or_timeout); /* No longer receiving data */
        uart_lin_receive_head_error = true;
        return;
    }

    /* change rx fifo trigger level, using rx time out interrupt to receive data */
    uart_config_fifo_ctrl(ptr, &fifo_ctrl_2);

    switch (id) {
    case TEST_SLAVE_RECEIVE_ID:
        uart_lin_config.pid = pid;
        uart_lin_config.data.buff = receive_data;
        uart_lin_config.data.length = TEST_SLAVE_RECEIVE_DATA_LENGTH;
        uart_lin_config.data.enhance_checksum = TEST_SLAVE_RECEIVE_ENHANCE_CHECKSUM;
        break;
    case TEST_SLAVE_SEND_ID:
        uart_disable_irq(ptr, uart_intr_rx_data_avail_or_timeout); /* No longer receiving data */
        uint8_t data[TEST_SLAVE_SEND_DATA_LENGTH] = {0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
        memcpy(send_data, data, TEST_SLAVE_SEND_DATA_LENGTH);

        uart_lin_config.pid = pid;
        uart_lin_config.data.buff = send_data;
        uart_lin_config.data.length = TEST_SLAVE_SEND_DATA_LENGTH;
        uart_lin_config.data.enhance_checksum = TEST_SLAVE_SEND_ENHANCE_CHECKSUM;
        hpm_uart_lin_slave_send_data(&uart_lin_config);
        uart_enable_irq(ptr, uart_intr_tx_slot_avail);
        break;
    default:
        uart_disable_irq(ptr, uart_intr_rx_data_avail_or_timeout); /* No longer receiving data */
        return;
    }
}

int main(void)
{
    board_init();
    board_init_uart(TEST_UART);

    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.baudrate = TEST_UART_LIN_BAUDRATE; /* specify baudrate, not support baudrate adaptation */
    if (status_success != uart_init(TEST_UART, &config)) {
        printf("failed to initialize uart\n");
    }

    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);
    uart_enable_irq(TEST_UART, uart_intr_rx_line_stat);

    printf("Test uart lin salve example\n");

    while (1) {
        if (uart_lin_receive_complete) {
            uart_lin_receive_complete = false;
            printf("uart lin receive ID: 0x%x\n", uart_lin_config.pid & 0x3f);
            printf("uart receive %d data:\n", uart_lin_config.data.length);
            for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
                if (i < uart_lin_config.data.length - 1) {
                    printf("0x%x ", receive_data[i]);
                } else {
                    printf("0x%x\n", receive_data[i]);
                }
            }
        }

        if (uart_lin_send_complete) {
            uart_lin_send_complete = false;
            printf("uart lin receive ID: 0x%x\n", uart_lin_config.pid & 0x3f);
            printf("uart send %d data:\n", uart_lin_config.data.length);
            for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
                if (i < uart_lin_config.data.length - 1) {
                    printf("0x%x ", send_data[i]);
                } else {
                    printf("0x%x\n", send_data[i]);
                }
            }
            uart_clear_rx_fifo(TEST_UART); /* clear all data in rx fifo */
        }

        if (uart_lin_receive_head_error) {
            uart_lin_receive_head_error = false;
            printf("uart lin receive header error\n");
        }

        if (uart_lin_receive_data_error) {
            uart_lin_receive_data_error = false;
            printf("uart lin receive data error\n");
        }
    }

    return 0;
}
