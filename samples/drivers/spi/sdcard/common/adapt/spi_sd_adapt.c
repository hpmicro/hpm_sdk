/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "spi_sd_adapt.h"
#include "hpm_spi_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_clock_drv.h"
#include "board.h"

static hpm_stat_t set_spi_speed(uint32_t freq);
static void cs_select(void);
static void cs_relese(void);
static bool sdcard_is_present(void);
static uint8_t write_read_byte(uint8_t byte);
static hpm_stat_t write_cmd_data(uint8_t cmd, uint8_t *buffer, uint32_t size);
static hpm_stat_t write(uint8_t *buffer, uint32_t size);
static hpm_stat_t read(uint8_t *buffer, uint32_t size);

static spi_control_config_t control_config = {0};
static spi_timing_config_t timing_config   = {0};
static spi_format_config_t format_config   = {0};
static sdcard_spi_interface_t g_spi_io;

hpm_stat_t spi_sd_init(void)
{
    board_init_spi_clock(BOARD_APP_SPI_BASE);
    board_init_spi_pins(BOARD_APP_SPI_BASE);

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = clock_get_frequency(BOARD_APP_SPI_CLK_NAME);;
    timing_config.master_config.sclk_freq_in_hz = 400000;
    if (status_success != spi_master_timing_init(BOARD_APP_SPI_BASE, &timing_config)) {
        printf("SPI master timming init failed\n");
        while (1) {
        }
    }

     /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = BOARD_APP_SPI_DATA_LEN_IN_BITS;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(BOARD_APP_SPI_BASE, &format_config);

    g_spi_io.set_spi_speed     = set_spi_speed;
    g_spi_io.cs_select         = cs_select;
    g_spi_io.cs_relese         = cs_relese;
    g_spi_io.sdcard_is_present = sdcard_is_present;
    g_spi_io.write_read_byte   = write_read_byte;
    g_spi_io.write_cmd_data    = write_cmd_data;
    g_spi_io.write             = write;
    g_spi_io.read              = read;
    return sdcard_spi_init(&g_spi_io);
}

static hpm_stat_t set_spi_speed(uint32_t freq)
{
    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = clock_get_frequency(BOARD_APP_SPI_CLK_NAME);
    timing_config.master_config.sclk_freq_in_hz = freq;
    return spi_master_timing_init(BOARD_APP_SPI_BASE, &timing_config);
}

static void cs_select(void)
{
    /* use hardware cs, so currently there is no need to operate*/
}

static void cs_relese(void)
{
    /* use hardware cs, so currently there is no need to operate*/
}

static bool sdcard_is_present(void)
{
    /* boards is not detect pins, so it always true */

    return true;
}

static uint8_t write_read_byte(uint8_t byte)
{
    uint8_t read_byte = 0x00;
    uint8_t _byte = byte;
    control_config.master_config.cmd_enable  = false; /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode  = spi_trans_write_read_together;
    spi_transfer(BOARD_APP_SPI_BASE, &control_config, NULL, NULL,
                 (uint8_t *)&_byte, sizeof(uint8_t), (uint8_t *)&read_byte,
                 sizeof(uint8_t));
    spi_wait_for_idle_status(BOARD_APP_SPI_BASE);
    return read_byte;
}

static hpm_stat_t write_cmd_data(uint8_t cmd, uint8_t *buffer, uint32_t size)
{
    hpm_stat_t stat;
    control_config.master_config.cmd_enable  = true; /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode  = spi_trans_write_only;
    stat = spi_transfer(BOARD_APP_SPI_BASE, &control_config, &cmd, NULL, buffer, size, NULL, 0);
    stat = spi_wait_for_idle_status(BOARD_APP_SPI_BASE);
    return stat;
}

static hpm_stat_t write(uint8_t *buffer, uint32_t size)
{
    hpm_stat_t stat;
    control_config.master_config.cmd_enable  = false; /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode  = spi_trans_write_only;
    stat = spi_transfer(BOARD_APP_SPI_BASE, &control_config, NULL, NULL, buffer, size, NULL, 0);
    stat = spi_wait_for_idle_status(BOARD_APP_SPI_BASE);
    return stat;
}

static hpm_stat_t read(uint8_t *buffer, uint32_t size)
{
    hpm_stat_t stat;
    control_config.master_config.cmd_enable  = false; /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode  = spi_trans_read_only;
    stat = spi_transfer(BOARD_APP_SPI_BASE, &control_config, NULL, NULL, NULL, 0, buffer, size);
    stat = spi_wait_for_idle_status(BOARD_APP_SPI_BASE);
    return stat;
}
