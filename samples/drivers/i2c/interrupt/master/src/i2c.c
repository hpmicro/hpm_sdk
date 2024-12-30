/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_i2c_drv.h"

#define TEST_I2C                BOARD_APP_I2C_BASE
#define TEST_I2C_CLOCK_NAME     BOARD_APP_I2C_CLK_NAME
#define TEST_I2C_SLAVE_ADDRESS  (0x16U)
#define TEST_I2C_IRQ            BOARD_APP_I2C_IRQ

#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
uint8_t rx_buff[TEST_TRANSFER_DATA_IN_BYTE];
uint8_t tx_buff[TEST_TRANSFER_DATA_IN_BYTE];

uint32_t sent_data_count;
uint32_t received_data_count;
volatile bool i2c_receive_complete;
volatile bool i2c_transmit_complete;

SDK_DECLARE_EXT_ISR_M(TEST_I2C_IRQ, i2c_isr)
void i2c_isr(void)
{
    volatile uint32_t status, irq;
    status = i2c_get_status(TEST_I2C);
    irq = i2c_get_irq_setting(TEST_I2C);

    /* transmit */
    if ((status & I2C_EVENT_FIFO_EMPTY) && (irq & I2C_EVENT_FIFO_EMPTY)) {
        while (!i2c_fifo_is_full(TEST_I2C)) {
            i2c_write_byte(TEST_I2C, tx_buff[sent_data_count++]);
            if (sent_data_count == TEST_TRANSFER_DATA_IN_BYTE) {
                i2c_disable_irq(TEST_I2C, I2C_EVENT_FIFO_EMPTY);
                break;
            }
        }
    }

    /* receive */
    if (status & I2C_EVENT_FIFO_FULL) {
        while (!i2c_fifo_is_empty(TEST_I2C)) {
            rx_buff[received_data_count++] = i2c_read_byte(TEST_I2C);
        }

        if (received_data_count == TEST_TRANSFER_DATA_IN_BYTE) {
            i2c_disable_irq(TEST_I2C, I2C_EVENT_FIFO_FULL);
        }
    }

    /* complete */
    if (status & I2C_EVENT_TRANSACTION_COMPLETE) {
        if (I2C_DIR_MASTER_READ == i2c_get_direction(TEST_I2C)) {
            while ((!i2c_fifo_is_empty(TEST_I2C)) && (received_data_count < TEST_TRANSFER_DATA_IN_BYTE)) {
                rx_buff[received_data_count++] = i2c_read_byte(TEST_I2C);
                if (received_data_count == TEST_TRANSFER_DATA_IN_BYTE) {
                    i2c_disable_irq(TEST_I2C, I2C_EVENT_FIFO_FULL);
                    break;
                }
            }
            i2c_receive_complete = true;
        } else {
            i2c_transmit_complete = true;
        }
        i2c_disable_irq(TEST_I2C, I2C_EVENT_TRANSACTION_COMPLETE);
        i2c_clear_status(TEST_I2C, I2C_EVENT_TRANSACTION_COMPLETE);
    }
}

static void prepare_tx_data(void)
{
    for (uint8_t i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        tx_buff[i] = i % 0xFF;
    }
}

static void check_transfer_data(void)
{
    uint32_t i = 0U, error_count = 0U;

    printf("The sent data are:");
    for (i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", tx_buff[i]);
    }
    printf("\n");
    printf("The received data are:");
    for (i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", rx_buff[i]);
        if (tx_buff[i] != rx_buff[i]) {
            error_count++;
        }
    }
    printf("\n");
    if (error_count == 0) {
        printf("I2C transfer all data matched!\n");
    } else {
        printf("Error occurred in I2C transfer!\n");
    }
}

int main(void)
{
    board_init();
    board_init_i2c(TEST_I2C);

    printf("I2C interrupt master example\n");
    prepare_tx_data();
    intc_m_enable_irq_with_priority(TEST_I2C_IRQ, 1);

    /* i2c configure sent data */
    i2c_master_configure_transfer(TEST_I2C, TEST_I2C_SLAVE_ADDRESS, sizeof(tx_buff), false);
    i2c_enable_irq(TEST_I2C, I2C_EVENT_TRANSACTION_COMPLETE | I2C_EVENT_FIFO_EMPTY);

    /*  Wait for transfer completed. */
    while (!i2c_transmit_complete) {
    }

    /* wait for slave controller to be ready to send data */
    board_delay_ms(100);

    /* i2c configure receive data */
    i2c_master_configure_transfer(TEST_I2C, TEST_I2C_SLAVE_ADDRESS, sizeof(tx_buff), true);
    i2c_enable_irq(TEST_I2C, I2C_EVENT_TRANSACTION_COMPLETE | I2C_EVENT_FIFO_FULL);
    /*  Wait for receive completed. */
    while (!i2c_receive_complete) {
    }

    check_transfer_data();

    while (1) {
    }

    return 0;
}
