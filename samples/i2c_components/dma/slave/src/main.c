/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_dma_mgr.h"
#include "hpm_i2c.h"
#include "board.h"
#define TEST_I2C                    BOARD_APP_I2C_BASE
#define TEST_I2C_SLAVE_ADDRESS      (0x16U)
#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
#define TEST_REG_ADDR_LEN_IN_BITS   (16)
#define TEST_REG_ADDR_LEN_IN_BYTES  ((TEST_REG_ADDR_LEN_IN_BITS + 7) / 8)

#if ((TEST_REG_ADDR_LEN_IN_BYTES * 8) == 8)
#define TEST_REG_ADDR_TYPE_SIZE     8
#elif ((TEST_REG_ADDR_LEN_IN_BYTES * 8) == 16)
#define TEST_REG_ADDR_TYPE_SIZE     16
#else
#define TEST_REG_ADDR_TYPE_SIZE     32
#endif

#define _BUFFER_CONCAT3(x, y, z)     x ## y ## z
#define BUFFER_CONCAT3(x, y, z)     _BUFFER_CONCAT3(x, y, z)

typedef BUFFER_CONCAT3(uint, TEST_REG_ADDR_TYPE_SIZE, _t)  buffer_type;

ATTR_PLACE_AT_NONCACHEABLE uint8_t rx_buff[TEST_TRANSFER_DATA_IN_BYTE + sizeof(buffer_type)];
ATTR_PLACE_AT_NONCACHEABLE uint8_t tx_buff[TEST_TRANSFER_DATA_IN_BYTE + sizeof(buffer_type)];

static volatile bool dma_complete;

static void prepare_transfer_data(void);
static void i2c_check_transfer_data(uint8_t *tx_buff, uint8_t *rx_buff, uint32_t size);


void i2c_dma_complete_callback(uint32_t channel)
{
    (void)channel;
    dma_complete = true;
}


int main(void)
{
    hpm_i2c_context_t i2c_context;
    buffer_type reg0_addr, reg1_addr;
    board_init();
    board_init_i2c_clock(TEST_I2C);
    init_i2c_pins(TEST_I2C);
    dma_mgr_init();
    printf("i2c components slave use dma\n");
    hpm_i2c_get_default_init_context(&i2c_context);
    i2c_context.init_config.communication_mode = i2c_slave;
    i2c_context.init_config.slave_address = TEST_I2C_SLAVE_ADDRESS;
    i2c_context.init_config.speed = i2c_speed_400khz;
    i2c_context.base = TEST_I2C;
    if (hpm_i2c_initialize(&i2c_context) != status_success) {
        printf("i2c slave initialize failed\n");
        while (1) {
        };
    }
    if (hpm_i2c_dma_mgr_install_callback(&i2c_context, i2c_dma_complete_callback) != status_success) {
        printf("i2c dma install callback fail\n");
        while (1) {
        }
    }
    dma_complete = false;

    printf("i2c slave read and write data of 16bit register address.....\n");
    prepare_transfer_data();
    /* frist: waiting i2c slave address hit*/
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_ADDRHIT_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_ADDRHIT_MASK);
    /* second: Waiting for the transmission direction to be read */
    while (i2c_get_direction(TEST_I2C) != I2C_DIR_SLAVE_READ) {
    };
    /* third: configure DMA */
    if (hpm_i2c_slave_read_nonblocking(&i2c_context, (uint8_t *)rx_buff, TEST_TRANSFER_DATA_IN_BYTE + sizeof(buffer_type)) != status_success) {
        printf("i2c slave dma read failed.....\n");
        while (1) {
        };
    }
    /* fourth: wait for the transfer complete */
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_CMPL_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_CMPL_MASK);
    /* fifth: wait for the DMA complete */
    while (dma_complete == false) {
    };
    dma_complete = false;
    /* record the address of the first register */
    reg0_addr = *(buffer_type *)&rx_buff[0];


    /* frist: waiting i2c slave address hit*/
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_ADDRHIT_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_ADDRHIT_MASK);
    /* second: Waiting for the transmission direction to be read */
    while (i2c_get_direction(TEST_I2C) != I2C_DIR_SLAVE_READ) {
    };
    /* third: configure DMA, Read the address of the register to be written */
    if (hpm_i2c_slave_read_nonblocking(&i2c_context, (uint8_t *)rx_buff, sizeof(buffer_type)) != status_success) {
        printf("i2c slave dma read failed.....\n");
        while (1) {
        };
    }
    /* fourth: wait for the transfer complete */
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_CMPL_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_CMPL_MASK);
    /* fifth: wait for the DMA complete */
    while (dma_complete == false) {
    };
    dma_complete = false;
    /* record the address of the second register */
    reg1_addr = *(buffer_type *)&rx_buff[0];

    /* sixth: write content, waiting i2c slave address hit*/
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_ADDRHIT_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_ADDRHIT_MASK);
    /* seventh: wait for the direction is write */
    while (i2c_get_direction(TEST_I2C) != I2C_DIR_SLAVE_WRITE) {
    };
    /* eighth: configure DMA, write content */
    if (hpm_i2c_slave_write_nonblocking(&i2c_context, (uint8_t *)tx_buff, TEST_TRANSFER_DATA_IN_BYTE) != status_success) {
        printf("i2c slave dma write failed.....\n");
        while (1) {
        };
    }
    /* ninth: waiting for the transfer complete */
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_CMPL_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_CMPL_MASK);
    /* tenth: waiting for the DMA complete */
    while (dma_complete == false) {
    };
    dma_complete = false;
    printf("read %dbit reg0: 0x%x\n", TEST_REG_ADDR_TYPE_SIZE, reg0_addr);
    printf("write %dbit reg0: 0x%x\n", TEST_REG_ADDR_TYPE_SIZE, reg1_addr);
    i2c_check_transfer_data(tx_buff, &rx_buff[sizeof(buffer_type)], TEST_TRANSFER_DATA_IN_BYTE);

    printf("\ni2c slave read and write data .....\n");
    prepare_transfer_data();
    /* frist: waiting i2c slave address hit*/
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_ADDRHIT_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_ADDRHIT_MASK);
    /* second: Waiting for the transmission direction to be write */
    while (i2c_get_direction(TEST_I2C) != I2C_DIR_SLAVE_READ) {
    };
    /* third: configure DMA */
    if (hpm_i2c_slave_read_nonblocking(&i2c_context, (uint8_t *)rx_buff, TEST_TRANSFER_DATA_IN_BYTE) != status_success) {
        printf("i2c slave dma mem read failed.....\n");
        while (1) {
        };
    }
    /* fourth: wait for the transfer complete */
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_CMPL_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_CMPL_MASK);
    /* fifth: wait for the DMA complete */
    while (dma_complete == false) {
    };
    dma_complete = false;
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_ADDRHIT_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_ADDRHIT_MASK);
    while (i2c_get_direction(TEST_I2C) != I2C_DIR_SLAVE_WRITE) {
    };
    if (hpm_i2c_slave_write_nonblocking(&i2c_context, (uint8_t *)tx_buff, TEST_TRANSFER_DATA_IN_BYTE) != status_success) {
        printf("i2c slave dma mem write failed.....\n");
        while (1) {
        };
    }
    while (!(i2c_get_status(TEST_I2C) & I2C_STATUS_CMPL_MASK)) {
    };
    i2c_clear_status(TEST_I2C, I2C_STATUS_CMPL_MASK);
    while (dma_complete == false) {
    };
    dma_complete = false;
    i2c_check_transfer_data(tx_buff, rx_buff, TEST_TRANSFER_DATA_IN_BYTE);
}

static void prepare_transfer_data(void)
{
    for (uint32_t i = 0; i < sizeof(tx_buff); i++) {
        tx_buff[i] = i % 0xFF;
    }
    memset(rx_buff, 0, sizeof(rx_buff));
}

static void i2c_check_transfer_data(uint8_t *tx_buff, uint8_t *rx_buff, uint32_t size)
{
    uint32_t i = 0U, error_count = 0U;
    printf("The sent data are:");
    for (i = 0; i < size; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%x ", tx_buff[i]);
    }
    printf("\n");
    printf("The received data are:");
    for (i = 0; i < size; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%x ", rx_buff[i]);
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
