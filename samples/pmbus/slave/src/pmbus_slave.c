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
#define TEST_I2C_IRQ            BOARD_APP_I2C_IRQ
#define TEST_PMBUS_SLAVE_ADDRESS  (0x16U)

#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
uint8_t tx_data_buff[TEST_TRANSFER_DATA_IN_BYTE];
uint8_t rx_data_buff[TEST_TRANSFER_DATA_IN_BYTE];

SDK_DECLARE_EXT_ISR_M(TEST_I2C_IRQ, i2c_isr)
void i2c_isr(void)
{
    hpm_pmbus_isr_handler(TEST_PMBUS);
}

void pmbus_callback(I2C_Type *base, hpm_pmbus_complete_cb_cfg_t *cfg)
{
  (void)base;
  printf("cb:cmd: 0x%02x, read:%s, pec_check:%s, len:%d, data_addr:0x%08x\n", cfg->command, cfg->read ? "true" : "false", cfg->pec_check ? "true" : "false", cfg->len, cfg->data);
}

int main(void)
{
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;

    board_init();
    board_init_i2c_clock(TEST_PMBUS);
    init_i2c_pins(TEST_PMBUS);

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    freq = clock_get_frequency(TEST_PMBUS_CLOCK_NAME);
    stat = i2c_init_slave(TEST_PMBUS, freq, &config, TEST_PMBUS_SLAVE_ADDRESS);
    if (stat != status_success) {
        printf("i2c init failed\n");
        while (1) {
        };
    }
    intc_m_enable_irq_with_priority(TEST_I2C_IRQ, 1);
    hpm_pmbus_slave_init(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS);
    printf("PMbus slave test\n");
    if (hpm_pmbus_slave_command_transaction_install(TEST_PMBUS, PMBUS_CODE_PAGE_PLUS_WRITE, NULL, rx_data_buff, 128, pmbus_callback) == status_success) {
        printf("pmbus(cmd:0x%02x) slave command transaction install success\n", PMBUS_CODE_PAGE_PLUS_WRITE);
    } else {
        printf("pmbus(cmd:0x%02x) slave command transaction install fail\n", PMBUS_CODE_PAGE_PLUS_WRITE);
        while (1) {
        };
    }

    if (hpm_pmbus_slave_command_transaction_install(TEST_PMBUS, PMBUS_CODE_PAGE_PLUS_READ, tx_data_buff, NULL, 128, pmbus_callback) == status_success) {
        printf("pmbus(cmd:0x%02x) slave command transaction install success\n", PMBUS_CODE_PAGE_PLUS_READ);
    } else {
        printf("pmbus(cmd:0x%02x) slave command transaction install fail\n", PMBUS_CODE_PAGE_PLUS_READ);
        while (1) {
        };
    }

    if (hpm_pmbus_slave_command_transaction_install(TEST_PMBUS, PMBUS_CODE_PAGE, tx_data_buff, rx_data_buff, 128, pmbus_callback) == status_success) {
        printf("pmbus(cmd:0x%02x) slave command transaction install success\n", PMBUS_CODE_PAGE);
    } else {
        printf("pmbus(cmd:0x%02x) slave command transaction install fail\n", PMBUS_CODE_PAGE);
        while (1) {
        };
    }

    if (hpm_pmbus_slave_command_transaction_install(TEST_PMBUS, PMBUS_CODE_CLEAR_FAULTS, NULL, rx_data_buff, 128, pmbus_callback) == status_success) {
        printf("pmbus(cmd:0x%02x) slave command transaction install success\n", PMBUS_CODE_CLEAR_FAULTS);
    } else {
        printf("pmbus(cmd:0x%02x) slave command transaction install fail\n", PMBUS_CODE_CLEAR_FAULTS);
        while (1) {
        };
    }

    if (hpm_pmbus_slave_command_transaction_install(TEST_PMBUS, PMBUS_CODE_VOUT_TRIM, tx_data_buff, rx_data_buff, 128, pmbus_callback) == status_success) {
        printf("pmbus(cmd:0x%02x) slave command transaction install success\n", PMBUS_CODE_VOUT_TRIM);
    } else {
        printf("pmbus(cmd:0x%02x) slave command transaction install fail\n", PMBUS_CODE_VOUT_TRIM);
        while (1) {
        };
    }
    while (1) {
    }
    return 0;
}
