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
uint8_t data_buff[TEST_TRANSFER_DATA_IN_BYTE];

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

    /* address hit */
    if (status & I2C_EVENT_ADDRESS_HIT) {
        if (I2C_DIR_SLAVE_READ == i2c_get_direction(TEST_I2C)) {
            i2c_enable_irq(TEST_I2C, I2C_EVENT_FIFO_FULL);
        } else {
            i2c_enable_irq(TEST_I2C, I2C_EVENT_FIFO_EMPTY);
        }
        i2c_disable_irq(TEST_I2C, I2C_EVENT_ADDRESS_HIT);
        i2c_clear_status(TEST_I2C, I2C_EVENT_ADDRESS_HIT);
    }

    /* receive */
    if (status & I2C_EVENT_FIFO_FULL) {
        while (!i2c_fifo_is_empty(TEST_I2C)) {
            data_buff[received_data_count++] = i2c_read_byte(TEST_I2C);
        }

        if (received_data_count == TEST_TRANSFER_DATA_IN_BYTE) {
            i2c_disable_irq(TEST_I2C, I2C_EVENT_FIFO_FULL);
        }
    }

    /* transmit */
    if ((status & I2C_EVENT_FIFO_EMPTY) && (irq & I2C_EVENT_FIFO_EMPTY)) {
        while (!i2c_fifo_is_full(TEST_I2C)) {
            i2c_write_byte(TEST_I2C, data_buff[sent_data_count++]);
            if (sent_data_count == TEST_TRANSFER_DATA_IN_BYTE) {
                i2c_disable_irq(TEST_I2C, I2C_EVENT_FIFO_EMPTY);
                break;
            }
        }
    }

    /* complete */
    if (status & I2C_EVENT_TRANSACTION_COMPLETE) {
        if (I2C_DIR_SLAVE_READ == i2c_get_direction(TEST_I2C)) {
            while ((!i2c_fifo_is_empty(TEST_I2C)) && (received_data_count < TEST_TRANSFER_DATA_IN_BYTE)) {
                data_buff[received_data_count++] = i2c_read_byte(TEST_I2C);
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


static void print_received_data(void)
{
    printf("The received data are:");
    for (uint32_t i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", data_buff[i]);
    }
    printf("\n");
}

int main(void)
{
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;

    board_init();
    freq = board_init_i2c_clock(TEST_I2C);
    init_i2c_pins(TEST_I2C);

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    stat = i2c_init_slave(TEST_I2C, freq, &config, TEST_I2C_SLAVE_ADDRESS);
    if (stat != status_success) {
        return stat;
    }

    printf("I2C interrupt slave example, I2C will sent back the received data\n");
    intc_m_enable_irq_with_priority(TEST_I2C_IRQ, 1);

    i2c_enable_irq(TEST_I2C, I2C_EVENT_ADDRESS_HIT | I2C_EVENT_TRANSACTION_COMPLETE);
    /*  Wait for receive completed. */
    while (!i2c_receive_complete) {
    }

    i2c_enable_irq(TEST_I2C, I2C_EVENT_ADDRESS_HIT | I2C_EVENT_TRANSACTION_COMPLETE);
    /*  Wait for transfer completed. */
    while (!i2c_transmit_complete) {
    }

    print_received_data();

    while (1) {
    }

    return 0;
}
