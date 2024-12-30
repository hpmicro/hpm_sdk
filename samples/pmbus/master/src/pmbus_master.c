/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_pmbus.h"

#define TEST_PMBUS                BOARD_APP_I2C_BASE
#define TEST_PMBUS_CLOCK_NAME     BOARD_APP_I2C_CLK_NAME
#define TEST_PMBUS_SLAVE_ADDRESS  (0x16U)

#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
uint8_t rx_buff[TEST_TRANSFER_DATA_IN_BYTE];
uint8_t tx_buff[TEST_TRANSFER_DATA_IN_BYTE];

int main(void)
{
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;
    uint32_t read_len;
    board_init();
    board_init_i2c_clock(TEST_PMBUS);
    init_i2c_pins(TEST_PMBUS);

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    freq = clock_get_frequency(TEST_PMBUS_CLOCK_NAME);
    stat = i2c_init_master(TEST_PMBUS, freq, &config);
    if (stat != status_success) {
        printf("i2c init failed\n");
        while (1) {
        };
    }
    printf("PMbus master test\n");
    while (1) {
        printf("pmbus master write test begin...\n");
        stat = hpm_pmbus_master_write(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_PAGE_PLUS_WRITE, tx_buff, TEST_TRANSFER_DATA_IN_BYTE);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master write failed, err_code:%d\r\n", PMBUS_CODE_PAGE_PLUS_WRITE, stat);
        }
        board_delay_ms(100);
        stat = hpm_pmbus_master_write(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_PAGE, tx_buff, TEST_TRANSFER_DATA_IN_BYTE);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master write failed, err_code:%d\r\n", PMBUS_CODE_PAGE, stat);
        }
        board_delay_ms(100);
        stat = hpm_pmbus_master_write(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_CLEAR_FAULTS, tx_buff, TEST_TRANSFER_DATA_IN_BYTE);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master write failed, err_code:%d\r\n", PMBUS_CODE_CLEAR_FAULTS, stat);
        }
        board_delay_ms(100);
        stat = hpm_pmbus_master_write(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_VOUT_TRIM, tx_buff, TEST_TRANSFER_DATA_IN_BYTE);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master write failed, err_code:%d\r\n", PMBUS_CODE_VOUT_TRIM, stat);
        }
        printf("pmbus master write test end...\n\n");

        printf("pmbus master read test begin...\n");
        board_delay_ms(100);
        read_len = TEST_TRANSFER_DATA_IN_BYTE;
        stat = hpm_pmbus_master_read(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_PAGE, rx_buff, &read_len);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master read failed, err_code:%d\r\n", PMBUS_CODE_PAGE, stat);
        } else {
            printf("pmbus(cmd:0x%02x) master read len:%d\n", PMBUS_CODE_PAGE, read_len);
        }
        board_delay_ms(100);
        read_len = TEST_TRANSFER_DATA_IN_BYTE;
        stat = hpm_pmbus_master_read(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_PAGE_PLUS_READ, rx_buff, &read_len);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master read failed, err_code:%d\r\n", PMBUS_CODE_PAGE_PLUS_READ, stat);
        } else {
            printf("pmbus(cmd:0x%02x) master read len:%d\n", PMBUS_CODE_PAGE_PLUS_READ, read_len);
        }
        board_delay_ms(100);
        read_len = TEST_TRANSFER_DATA_IN_BYTE;
        stat = hpm_pmbus_master_read(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS, PMBUS_CODE_VOUT_TRIM, rx_buff, &read_len);
        if (stat != status_success) {
            printf("pmbus(cmd:0x%02x) master read failed, err_code:%d\r\n", PMBUS_CODE_VOUT_TRIM, stat);
        } else {
            printf("pmbus(cmd:0x%02x) master read len:%d\n", PMBUS_CODE_VOUT_TRIM, read_len);
        }
        board_delay_ms(100);

        printf("pmbus master read test end...\n\n");
    }
    return 0;
}
