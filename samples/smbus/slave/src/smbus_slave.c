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
uint8_t data_buff[TEST_TRANSFER_DATA_IN_BYTE];

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
    board_init_i2c_clock(TEST_SMBUS);
    init_i2c_pins(TEST_SMBUS);

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    freq = clock_get_frequency(TEST_SMBUS_CLOCK_NAME);
    stat = i2c_init_slave(TEST_SMBUS, freq, &config, TEST_SMBUS_SLAVE_ADDRESS);
    if (stat != status_success) {
        return stat;
    }

    printf("SMbus slave example, SMbus will sent back the received data\n");

    /* wait for address hit */
    do {
        stat = hpm_smbus_slave_read(TEST_SMBUS, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
    } while (stat == status_fail);

    if (stat != status_success) {
        printf("TEST_SMBUS Slave read failed\n");
        while (1) {
        }
    }

    /* wait for address hit */
    do {
        stat = hpm_smbus_slave_write(TEST_SMBUS, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
    } while (stat == status_fail);

    if (stat != status_success) {
        printf("TEST_SMBUS Slave write failed\n");
        while (1) {
        }
    }

    print_received_data();

    while (1) {
    }

    return 0;
}
