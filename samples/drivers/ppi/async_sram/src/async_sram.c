/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_ppi.h"

#define SRAM_BASE_ADDR 0xF8000000U    /* Base address should be aligned by 1MB */
#define SRAM_SIZE      BOARD_PPI_ASYNC_SRAM_SIZE

static void rw_comparison(uint32_t start, uint32_t size_in_byte)
{
    hpm_stat_t status;
    volatile uint32_t *u32_data_ptr;
    volatile uint16_t *u16_data_ptr;
    volatile uint8_t *u8_data_ptr;

    printf("comparison test: from 0x%x to 0x%x\n", start, start + size_in_byte);

    u32_data_ptr = (volatile uint32_t *)start;
    for (uint32_t i = 0; i < (size_in_byte>>2); i++) {
        u32_data_ptr[i] = 0x12345678 + i;
    }
    status = status_success;
    for (uint32_t i = 0; i < (size_in_byte>>2); i++) {
        if (u32_data_ptr[i] != 0x12345678 + i) {
            printf("[%x] data u32 mismatch @ 0x%x\n", (uint32_t)&(u32_data_ptr[i]), u32_data_ptr[i]);
            status = status_fail;
            break;
        }
    }
    printf("%s u32 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");

    u16_data_ptr = (volatile uint16_t *)start;
    for (uint32_t i = 0; i < (size_in_byte>>1); i++) {
        u16_data_ptr[i] = (uint16_t)(0x5AA5 + i);
    }
    status = status_success;
    for (uint32_t i = 0; i < (size_in_byte>>1); i++) {
        if (u16_data_ptr[i] != (uint16_t)(0x5AA5 + i)) {
            printf("[%x] data u16 mismatch @ 0x%x\n", (uint32_t)&(u16_data_ptr[i]), u16_data_ptr[i]);
            status = status_fail;
            break;
        }
    }
    printf("%s u16 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");

    u8_data_ptr = (volatile uint8_t *)start;
    for (uint32_t i = 0; i < (size_in_byte); i++) {
        u8_data_ptr[i] = (i % 256);
    }
    status = status_success;
    for (uint32_t i = 0; i < (size_in_byte); i++) {
        if (u8_data_ptr[i] != (i % 256)) {
            printf("[%x] data u8 mismatch @ 0x%x\n", (uint32_t)&(u8_data_ptr[i]), u8_data_ptr[i]);
            status = status_fail;
            break;
        }
    }
    printf("%s u8 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");
}

static void init_sram_config(void)
{
    ppi_async_sram_config_t config;

    config.base_address = SRAM_BASE_ADDR;
    config.size_in_byte = SRAM_SIZE;
    config.port_size = ppi_port_size_16bits;
    config.ad_mux_mode = BOARD_PPI_ASYNC_SRAM_AD_MUX_MODE;
    config.cs_valid_polarity = false;
    config.dm_valid_polarity = false;
    config.addr_valid_polarity = true;
    config.adv_ctrl_pin = BOARD_PPI_ASYNC_SRAM_ADV_CTRL_PIN;
    config.rel_ctrl_pin = BOARD_PPI_ASYNC_SRAM_OE_CTRL_PIN;
    config.wel_ctrl_pin = BOARD_PPI_ASYNC_SRAM_WE_CTRL_PIN;
    config.as_in_ns = 0;
    config.ah_in_ns = 48;
    config.rel_in_ns = 48;
    config.reh_in_ns = 0;
    config.wel_in_ns = 48;
    config.weh_in_ns = 0;
    ppi_config_async_sram(HPM_PPI, BOARD_PPI_ASYNC_SRAM_CS_INDEX, 0, &config);
}

int main(void)
{
    board_init();
    init_ppi_pins();
    init_sram_config();

    printf("ppi sram example\n");
    rw_comparison(SRAM_BASE_ADDR, SRAM_SIZE);

    while (1) {
        ;
    }
    return 0;
}
