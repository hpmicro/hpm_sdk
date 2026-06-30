/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_ppi.h"

#define ADC_DATA_BASE_ADDR 0xF9000000U    /* Base address should be aligned by 1MB */
#define ADC_DATA_SIZE 2

static void init_parallel_adc_config(void)
{
    ppi_async_sram_config_t config;
    ppi_clk_pin_config_t clk_config;

    ppi_get_async_sram_defconfig(HPM_PPI, &config);
    config.base_address = ADC_DATA_BASE_ADDR;
    config.size_in_byte = ADC_DATA_SIZE;
    config.port_size = ppi_port_size_16bits;
    config.ad_mux_mode = false;
    config.cs_valid_polarity = false;
    config.dm_valid_polarity = false;
    config.addr_valid_polarity = false;
    config.cs_sync_clk_en = true;           /* need to enable cs sync clk */
    config.cs_sync_clk_sel = 0;
    config.adv_ctrl_pin = 7;
    config.rel_ctrl_pin = 1;
    config.wel_ctrl_pin = 0;
    config.as_in_ns = 0;
    config.ah_in_ns = 20;
    config.rel_in_ns = 20;
    config.reh_in_ns = 0;
    config.wel_in_ns = 20;
    config.weh_in_ns = 0;
    config.dq_sig_sel[0] = BOARD_PPI_ASYNC_SRAM_SIG_DQ0_7;
    config.dq_sig_sel[1] = BOARD_PPI_ASYNC_SRAM_SIG_DQ8_15;
    config.dq_sig_sel[2] = BOARD_PPI_ASYNC_SRAM_SIG_DQ16_23;
    config.dq_sig_sel[3] = BOARD_PPI_ASYNC_SRAM_SIG_DQ24_31;
    ppi_config_async_sram(HPM_PPI, BOARD_PPI_ADC_CS_INDEX, 8, &config);

    clk_config.cycle_num = 10;    /* 50ns */
    clk_config.high_num = 5;
    clk_config.low_num = 0;
    clk_config.mode = ppi_clk_always_output;
    clk_config.revert = true;
    ppi_config_clk_pin(HPM_PPI, &clk_config);
}

int main(void)
{
    uint16_t *data_ptr = (uint16_t *)ADC_DATA_BASE_ADDR;
    volatile uint16_t adc_data;

    board_init();
    clock_add_to_group(clock_ppi0, 0);
    init_ppi_pins();
    init_parallel_adc_config();

    printf("ppi parallel adc example\n");

    adc_data = *data_ptr;    /* Dummy Read, trigger adc output */

    while (1) {
        board_delay_ms(300);
        adc_data = *data_ptr;
        printf("adc data: %#x\n", adc_data & 0x03FF);
    }
    return 0;
}
