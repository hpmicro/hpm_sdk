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
#define SRAM_SIZE 2

static void init_sram_config(void)
{
    ppi_async_sram_config_t config;
    ppi_clk_pin_config_t clk_config;

    config.base_address = SRAM_BASE_ADDR;
    config.size_in_byte = SRAM_SIZE;
    config.port_size = ppi_port_size_16bits;
    config.ad_mux_mode = false;
    config.cs_valid_polarity = false;
    config.dm_valid_polarity = false;
    config.addr_valid_polarity = false;
    config.adv_ctrl_pin = 7;
    config.rel_ctrl_pin = 1;
    config.wel_ctrl_pin = 0;
    config.as_in_ns = 0;
    config.ah_in_ns = 20;
    config.rel_in_ns = 20;
    config.reh_in_ns = 0;
    config.wel_in_ns = 20;
    config.weh_in_ns = 0;
    ppi_config_async_sram(HPM_PPI, 0, 0, &config);

    clk_config.cycle_num = 6;
    clk_config.high_num = 3;
    clk_config.low_num = 0;
    clk_config.mode = ppi_clk_always_output;
    clk_config.revert = false;
    ppi_config_clk_pin(HPM_PPI, &clk_config);
}

static void read_adc_data(void)
{
    uint16_t *data_ptr = (uint16_t *)SRAM_BASE_ADDR;

    printf("adc data: %#x\n", (*data_ptr) & 0x03FF);
}

int main(void)
{
    board_init();
    init_ppi_pins();
    init_sram_config();

    printf("ppi parallel adc example\n");

    while (1) {
        read_adc_data();
        board_delay_ms(300);
    }
    return 0;
}
