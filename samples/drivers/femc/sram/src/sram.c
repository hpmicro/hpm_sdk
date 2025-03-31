/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_l1c_drv.h"
#include "hpm_femc_drv.h"

#define SRAM_BASE_ADDR 0x48000000U
#define SRAM_SIZE      BOARD_FEMC_ASYNC_SRAM_SIZE

static void rw_comparison(uint32_t start, uint32_t size_in_byte)
{
    hpm_stat_t status;
    volatile uint32_t *u32_data_ptr;
    volatile uint16_t *u16_data_ptr;
    volatile uint8_t *u8_data_ptr;

    printf("comparison test: from 0x%x to 0x%x\n", start, start + size_in_byte);

    u32_data_ptr = (volatile uint32_t *)start;
    for (uint32_t i = 0; i < (size_in_byte>>2); i++) {
        u32_data_ptr[i] = 0x12345678;
    }
    status = status_success;
    for (uint32_t i = 0; i < (size_in_byte>>2); i++) {
        if (u32_data_ptr[i] != 0x12345678) {
            printf("[%x] data u32 mismatch @ 0x%x\n", (uint32_t)&(u32_data_ptr[i]), u32_data_ptr[i]);
            status = status_fail;
            break;
        }
    }
    printf("%s u32 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");

    u16_data_ptr = (volatile uint16_t *)start;
    for (uint32_t i = 0; i < (size_in_byte>>1); i++) {
        u16_data_ptr[i] = 0x5AA5;
    }
    status = status_success;
    for (uint32_t i = 0; i < (size_in_byte>>1); i++) {
        if (u16_data_ptr[i] != 0x5AA5) {
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
    femc_sram_config_t sram_config = {0};
    uint32_t femc_clk_in_hz = board_init_femc_clock();

#ifndef INIT_EXT_RAM_FOR_DATA
    femc_config_t config = {0};
    femc_default_config(HPM_FEMC, &config);
    femc_init(HPM_FEMC, &config);
#endif

    femc_get_typical_sram_config(HPM_FEMC, &sram_config);
    sram_config.base_address = SRAM_BASE_ADDR;
    sram_config.size_in_byte = SRAM_SIZE;
    sram_config.cs_index = BOARD_FEMC_ASYNC_SRAM_CS_INDEX;
    sram_config.port_size = FEMC_SRAM_PORT_SIZE_16_BITS;

    if (BOARD_FEMC_ASYNC_SRAM_AD_MUX_MODE) {
        sram_config.address_mode = FEMC_SRAM_AD_MUX_MODE;
        sram_config.adv_hold_state = FEMC_SRAM_ADV_HOLD_LOW;
        sram_config.adv_polarity = FEMC_SRAM_ADV_ACTIVE_HIGH;
        sram_config.oeh_in_ns = 0;
        sram_config.oel_in_ns = 50;
        sram_config.weh_in_ns = 0;
        sram_config.wel_in_ns = 50;
        sram_config.ah_in_ns = 25;
        sram_config.as_in_ns = 25;
        sram_config.ceh_in_ns = 0;
        sram_config.ces_in_ns = 0;
    } else {
        sram_config.address_mode = FEMC_SRAM_AD_NONMUX_MODE;
        sram_config.oeh_in_ns = 0;
        sram_config.oel_in_ns = 50;
        sram_config.weh_in_ns = 0;
        sram_config.wel_in_ns = 50;
        sram_config.ah_in_ns = 25;
        sram_config.as_in_ns = 25;
        sram_config.ceh_in_ns = 0;
        sram_config.ces_in_ns = 0;
    }

    femc_config_sram(HPM_FEMC, femc_clk_in_hz, &sram_config);
}

int main(void)
{
    l1c_dc_disable();

    board_init();
    init_femc_pins();
    init_sram_config();

    printf("sram example\n");
    rw_comparison(SRAM_BASE_ADDR, SRAM_SIZE);

    while (1) {
        ;
    }
    return 0;
}
