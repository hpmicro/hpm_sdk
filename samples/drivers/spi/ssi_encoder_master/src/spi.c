/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_spi_drv.h"

#define TEST_SPI BOARD_APP_SPI_BASE

static void read_ssi_encoder(void)
{
    spi_control_config_t control_config = { 0 };
    hpm_stat_t stat;
    uint32_t ssi_data;

    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;
    control_config.master_config.addr_enable = false;
    control_config.master_config.addr_phase_fmt = spi_address_phase_format_single_io_mode;
    control_config.common_config.tx_dma_enable = false;
    control_config.common_config.rx_dma_enable = false;
    control_config.common_config.trans_mode = spi_trans_read_only;
    control_config.common_config.data_phase_fmt = spi_single_io_mode;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;

    stat = spi_transfer(BOARD_APP_SPI_BASE, &control_config, NULL, NULL, NULL, 0, (uint8_t *)&ssi_data, 1);

    if (stat != status_success) {
        printf("Error: READ [%d]!\n", stat);
    } else {
        printf("SSI Encoder Data: [0x%x]\n", ssi_data & 0xFFFFFFu);
    }
}

int main(void)
{
    spi_timing_config_t timing_config = { 0 };
    spi_format_config_t format_config = { 0 };
    uint32_t spi_clock;

    board_init();

    spi_clock = board_init_spi_clock(TEST_SPI);
    board_init_spi_pins(TEST_SPI);

    printf("SPI Master Read SSI Encoder Example\n");

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clock;
    timing_config.master_config.sclk_freq_in_hz = 1000000;
    if (status_success != spi_master_timing_init(TEST_SPI, &timing_config)) {
        printf("SPI master timming init failed\n");
        while (1) {
        }
    }

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = 24 + 2;
    format_config.common_config.data_merge = false;
    format_config.common_config.mosi_bidir = false;
    format_config.common_config.lsb = false;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges;
    spi_format_init(TEST_SPI, &format_config);

    board_timer_create(100, read_ssi_encoder);

    while (1) {
        ;
    }

    return 0;
}
