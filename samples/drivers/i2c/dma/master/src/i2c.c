/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_i2c_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"

#define TEST_I2C               BOARD_APP_I2C_BASE
#define TEST_I2C_CLOCK_NAME    BOARD_APP_I2C_CLK_NAME
#define TEST_I2C_DMA           BOARD_APP_I2C_DMA
#define TEST_I2C_DMAMUX        BOARD_APP_I2C_DMAMUX
#define TEST_I2C_DMAMUX_SRC    BOARD_APP_I2C_DMA_SRC
#define TEST_I2C_DMAMUX_CH     BOARD_APP_I2C_DMAMUX_CH

#define TEST_I2C_SLAVE_ADDRESS (0x16U)

#ifndef PLACE_BUFF_AT_CACHEABLE
#define PLACE_BUFF_AT_CACHEABLE 1
#endif

#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
#if PLACE_BUFF_AT_CACHEABLE
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t rx_buff[TEST_TRANSFER_DATA_IN_BYTE];
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t tx_buff[TEST_TRANSFER_DATA_IN_BYTE];
#else
ATTR_PLACE_AT_NONCACHEABLE uint8_t rx_buff[TEST_TRANSFER_DATA_IN_BYTE];
ATTR_PLACE_AT_NONCACHEABLE uint8_t tx_buff[TEST_TRANSFER_DATA_IN_BYTE];
#endif

hpm_stat_t i2c_tx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, I2C_Type *i2c_ptr, uint32_t src, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = (uint32_t)&i2c_ptr->DATA;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config);
}

hpm_stat_t i2c_rx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, I2C_Type *i2c_ptr, uint32_t dst, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&i2c_ptr->DATA;
    config.src_fixed = true;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config);
}

static void i2c_handle_dma_transfer_complete(I2C_Type *ptr)
{
    volatile uint32_t status;

    /* wait for i2c transaction complete */
    do {
        status = i2c_get_status(TEST_I2C);
    } while (!(status & I2C_STATUS_CMPL_MASK));
    /* clear status */
    i2c_clear_status(TEST_I2C, status);
    /* disable i2c dma before next dma transaction */
    i2c_dma_disable(TEST_I2C);
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
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;

    board_init();
    init_i2c_pins(TEST_I2C);

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    freq = clock_get_frequency(TEST_I2C_CLOCK_NAME);
    stat = i2c_init_master(TEST_I2C, freq, &config);
    if (stat != status_success) {
        return stat;
    }

    printf("I2C DMA master example\n");
    prepare_tx_data();

    dmamux_config(TEST_I2C_DMAMUX, TEST_I2C_DMAMUX_CH, TEST_I2C_DMAMUX_SRC, true);

    /* setup i2c dma tx */
#if PLACE_BUFF_AT_CACHEABLE
    if (l1c_dc_is_enabled()) {
        /* cache writeback before DMA sent data */
        l1c_dc_writeback((uint32_t)tx_buff, TEST_TRANSFER_DATA_IN_BYTE);
    }
#endif
    stat = i2c_tx_trigger_dma(TEST_I2C_DMA,
                            TEST_I2C_DMAMUX_CH,
                            TEST_I2C,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tx_buff),
                            TEST_TRANSFER_DATA_IN_BYTE);
    if (stat != status_success) {
        printf("i2c tx trigger dma failed\n");
        while (1) {
        }
    }
    i2c_master_start_dma_write(TEST_I2C, TEST_I2C_SLAVE_ADDRESS, TEST_TRANSFER_DATA_IN_BYTE);
    i2c_handle_dma_transfer_complete(TEST_I2C);

    /* setup i2c dma rx */
    stat = i2c_rx_trigger_dma(TEST_I2C_DMA,
                            TEST_I2C_DMAMUX_CH,
                            TEST_I2C,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rx_buff),
                            TEST_TRANSFER_DATA_IN_BYTE);
    i2c_master_start_dma_read(TEST_I2C, TEST_I2C_SLAVE_ADDRESS, TEST_TRANSFER_DATA_IN_BYTE);
    i2c_handle_dma_transfer_complete(TEST_I2C);
#if PLACE_BUFF_AT_CACHEABLE
    if (l1c_dc_is_enabled()) {
        /* cache invalidate after DMA receive data */
        l1c_dc_invalidate((uint32_t)rx_buff, TEST_TRANSFER_DATA_IN_BYTE);
    }
#endif

    check_transfer_data();

    while (1) {
    }

    return 0;
}
