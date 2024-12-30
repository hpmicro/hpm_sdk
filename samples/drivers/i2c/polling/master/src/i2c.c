/*
 * Copyright (c) 2022 HPMicro
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

#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
uint8_t rx_buff[TEST_TRANSFER_DATA_IN_BYTE];
uint8_t tx_buff[TEST_TRANSFER_DATA_IN_BYTE];

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
            printf("\r\n");
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

    printf("I2C polling master example\n");
    prepare_tx_data();

    if (status_success != i2c_master_write(TEST_I2C, TEST_I2C_SLAVE_ADDRESS, tx_buff, sizeof(tx_buff))) {
        printf("Master write failed");
        while (1) {
        }
    }

    /* wait for slave controller to be ready to send data */
    board_delay_ms(100);

    if (status_success != i2c_master_read(TEST_I2C, TEST_I2C_SLAVE_ADDRESS, rx_buff, sizeof(rx_buff))) {
        printf("Master read failed\n");
        while (1) {
        }
    }

    check_transfer_data();

    while (1) {
    }

    return 0;
}
