/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_smbus.h"

#define TEST_SMBUS                BOARD_APP_I2C_BASE
#define TEST_SMBUS_CLOCK_NAME     BOARD_APP_I2C_CLK_NAME
#define TEST_SMBUS_SLAVE_ADDRESS  (0x16U)

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
        printf("SMbus transfer all data matched!\n");
    } else {
        printf("Error occurred in SMbus transfer!\n");
    }
}

int main(void)
{
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;

    board_init();
    board_init_i2c_clock(TEST_SMBUS);
    init_i2c_pins(TEST_SMBUS);

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    freq = clock_get_frequency(TEST_SMBUS_CLOCK_NAME);
    stat = i2c_init_master(TEST_SMBUS, freq, &config);
    if (stat != status_success) {
        return stat;
    }

    printf("SMbus master example\n");
    prepare_tx_data();

    if (status_success != hpm_smbus_master_write(TEST_SMBUS, TEST_SMBUS_SLAVE_ADDRESS, tx_buff, sizeof(tx_buff))) {
        printf("SMbus Master write failed\n");
        while (1) {
        }
    }

    /* wait for slave controller to be ready to send data */
    board_delay_ms(100);

    if (status_success != hpm_smbus_master_read(TEST_SMBUS, TEST_SMBUS_SLAVE_ADDRESS, rx_buff, sizeof(rx_buff))) {
        printf("SMbus Master read failed\n");
        while (1) {
        }
    }

    check_transfer_data();

    while (1) {
    }

    return 0;
}

