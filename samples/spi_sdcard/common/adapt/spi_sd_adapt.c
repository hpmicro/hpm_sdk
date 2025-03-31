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
#include "hpm_dma_mgr.h"
#include "hpm_spi.h"

#define SD_SPI_BASE                     BOARD_APP_SPI_BASE
#define SD_SPI_CLK_NAME                 BOARD_APP_SPI_CLK_NAME
#define SD_SPI_CS_PIN                   BOARD_SPI_CS_PIN
#define SD_SPI_DATA_LEN_BITS            (8U)

static hpm_stat_t set_spi_speed(uint32_t freq);
static void cs_select(void);
static void cs_relese(void);
static bool sdcard_is_present(void);
static hpm_stat_t write_read_byte(uint8_t *in_byte, uint8_t *out_byte);
static hpm_stat_t write_cmd_data(uint8_t cmd, uint8_t *buffer, uint32_t size);
static hpm_stat_t write(uint8_t *buffer, uint32_t size);
static hpm_stat_t read(uint8_t *buffer, uint32_t size);

static sdcard_spi_interface_t g_spi_io;
ATTR_PLACE_AT_NONCACHEABLE uint8_t tx_dummy[SPI_SD_BLOCK_SIZE] = {0xFF};

#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
static volatile bool rxdma_complete;
static volatile bool txdma_complete;

void spi_rxdma_complete_callback(uint32_t channel)
{
    (void)channel;
    rxdma_complete = true;
}

void spi_txdma_complete_callback(uint32_t channel)
{
    (void)channel;
    txdma_complete = true;
}
#endif

hpm_stat_t spi_sd_init(void)
{
    uint32_t spi_sclk_freq = 400000;
    spi_initialize_config_t init_config;
    board_init_spi_clock(SD_SPI_BASE);
    board_init_spi_pins_with_gpio_as_cs(SD_SPI_BASE);

    hpm_spi_get_default_init_config(&init_config);
    init_config.direction = spi_msb_first;
    init_config.mode = spi_master_mode;
    init_config.clk_phase = spi_sclk_sampling_even_clk_edges;
    init_config.clk_polarity = spi_sclk_high_idle;
    init_config.data_len = SD_SPI_DATA_LEN_BITS;
    /* step.1  initialize spi */
    if (hpm_spi_initialize(SD_SPI_BASE, &init_config) != status_success) {
        printf("hpm_spi_initialize fail\n");
        while (1) {
        }
    }
    if (hpm_spi_set_sclk_frequency(SD_SPI_BASE, spi_sclk_freq) != status_success) {
        printf("hpm_spi_set_sclk_frequency fail\n");
        while (1) {
        }
    }

#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
    if (hpm_spi_dma_mgr_install_callback(SD_SPI_BASE, spi_txdma_complete_callback, spi_rxdma_complete_callback) != status_success) {
        printf("hpm_spi_dma_mgr_install_callback fail\n");
        while (1) {
        }
    }
    rxdma_complete = false;
    txdma_complete = false;
#endif
    memset(tx_dummy, 0xFF, sizeof(tx_dummy));
    g_spi_io.set_spi_speed     = set_spi_speed;
    g_spi_io.cs_select         = cs_select;
    g_spi_io.cs_relese         = cs_relese;
    g_spi_io.sdcard_is_present = sdcard_is_present;
    g_spi_io.write_read_byte   = write_read_byte;
    g_spi_io.write_cmd_data    = write_cmd_data;
    g_spi_io.write             = write;
    g_spi_io.read              = read;
    g_spi_io.delay_ms          = board_delay_ms;
    g_spi_io.delay_us          = board_delay_us;
    return sdcard_spi_init(&g_spi_io);
}

static hpm_stat_t set_spi_speed(uint32_t freq)
{
    /* set SPI sclk frequency for master */
    if (hpm_spi_set_sclk_frequency(SD_SPI_BASE, freq) != status_success) {
        printf("hpm_spi_set_sclk_frequency fail\n");
        while (1) {
        }
    }
    printf("[spi_sdcard] SPI CLK frequency:%d Hz\n", freq);
    return status_success;
}

static void cs_select(void)
{
    board_write_spi_cs(BOARD_SPI_CS_PIN, false);
}

static void cs_relese(void)
{
    board_write_spi_cs(BOARD_SPI_CS_PIN, true);
}

static bool sdcard_is_present(void)
{
    /* boards is not detect pins, so it always true */

    return true;
}

static hpm_stat_t write_read_byte(uint8_t *in_byte, uint8_t *out_byte)
{
    hpm_stat_t stat;
    stat = hpm_spi_transmit_receive_blocking(SD_SPI_BASE, in_byte, out_byte, 1, 0xFFFFFFFF);
    return stat;
}

static hpm_stat_t write_cmd_data(uint8_t cmd, uint8_t *buffer, uint32_t size)
{
    hpm_stat_t stat;
    /* command phase is 48bit,*/
    uint8_t buf[6];
    buf[0] = cmd;
    memcpy(&buf[1], buffer, size);
    stat = hpm_spi_transmit_blocking(SD_SPI_BASE, buf, size + 1, 0xFFFFFFFF);
    return stat;
}

static hpm_stat_t write(uint8_t *buffer, uint32_t size)
{
    hpm_stat_t stat = status_success;
#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
    stat = hpm_spi_transmit_nonblocking(SD_SPI_BASE, buffer, size);
    if (stat != status_success) {
        printf("hpm_spi_transmit_nonblocking fail\n");
        return stat;
    }
    while (txdma_complete == false) {
    };
    txdma_complete = false;
    /* When SPI sends, DMA completion does not mean the transfer is complete. need to wait for the SPI status to complete.*/
    while (spi_is_active(SD_SPI_BASE) == true) {
    };
#else
    stat = hpm_spi_transmit_blocking(SD_SPI_BASE, buffer, size, 0xFFFFFFFF);
#endif
    return stat;
}

static hpm_stat_t read(uint8_t *buffer, uint32_t size)
{
    hpm_stat_t stat = status_success;
#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
    stat = hpm_spi_transmit_receive_nonblocking(SD_SPI_BASE, tx_dummy, buffer, size);
    if (stat != status_success) {
        printf("hpm_spi_transmit_receive_nonblocking fail\n");
        return status_fail;
    }
    while (rxdma_complete == false) {
    };
    while (txdma_complete == false) {
    };
    rxdma_complete = false;
    txdma_complete = false;
#else
    stat = hpm_spi_transmit_receive_blocking(SD_SPI_BASE, tx_dummy, buffer, size, 0xFFFFFFFF);
#endif
    return stat;
}
