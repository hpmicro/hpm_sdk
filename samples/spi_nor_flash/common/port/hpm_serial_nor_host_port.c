/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_spi_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_serial_nor_host_port.h"
#include "board.h"
#include "pinmux.h"

#define PORT_SPI_BASE              BOARD_NOR_FLASH_SPI_BASE
#define PORT_SPI_CLK_NAME          BOARD_NOR_FLASH_SPI_CLK_NAME
#define PORT_SPI_CS_PIN            BOARD_NOR_FLASH_SPI_CS_PIN

#if (SERIAL_NOR_USE_DMA_MGR == 0)
#define PORT_SPI_NOR_DMA           BOARD_APP_DMA0
#define PORT_SPI_NOR_DMAMUX        BOARD_APP_DMAMUX
#define PORT_SPI_RX_DMA_CH         0
#define PORT_SPI_TX_DMA_CH         1
#endif

#define PORT_SPI_RX_DMA_REQ        BOARD_NOR_FLASH_SPI_RX_DMA
#define PORT_SPI_TX_DMA_REQ        BOARD_NOR_FLASH_SPI_TX_DMA

#ifndef PORT_SPI_CLK_FREQUENCY
#define PORT_SPI_CLK_FREQUENCY     (80000000u)
#endif

#ifndef PORT_SPI_IO_MODE
#define PORT_SPI_IO_MODE          SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE
#endif

static void set_spi_clk_frequency(void *ops, uint32_t frequency);
static void serial_nor_write_cs(uint32_t cs_pin, uint8_t state);

ATTR_WEAK hpm_stat_t serial_nor_get_board_host(hpm_serial_nor_host_t *host)
{
    host->host_param.flags =  PORT_SPI_IO_MODE |
                              SERIAL_NOR_HOST_SUPPORT_DMA |
                              SERIAL_NOR_HOST_SUPPORT_SPI_INTERFACE;
    host->host_param.param.set_cs = serial_nor_write_cs;
    host->host_param.param.set_frequency = set_spi_clk_frequency;
    host->host_param.param.clock_name = PORT_SPI_CLK_NAME;
    host->host_param.param.pin_or_cs_index = PORT_SPI_CS_PIN;
    host->host_param.param.host_base = PORT_SPI_BASE;
#if (SERIAL_NOR_USE_DMA_MGR == 0)
    host->host_param.param.dma_control.dma_base = PORT_SPI_NOR_DMA;
    host->host_param.param.dma_control.dmamux_base    = PORT_SPI_NOR_DMAMUX;
    host->host_param.param.dma_control.rx_dma_ch  = PORT_SPI_RX_DMA_CH;
    host->host_param.param.dma_control.tx_dma_ch  = PORT_SPI_TX_DMA_CH;
#endif
    host->host_param.param.dma_control.rx_dma_req = PORT_SPI_RX_DMA_REQ;
    host->host_param.param.dma_control.tx_dma_req = PORT_SPI_TX_DMA_REQ;
    host->host_param.param.frequency = PORT_SPI_CLK_FREQUENCY;
    host->host_param.param.transfer_max_size = SPI_SOC_TRANSFER_COUNT_MAX;
    return status_success;
}

static void set_spi_clk_frequency(void *ops, uint32_t frequency)
{
    hpm_serial_nor_host_t *host = (hpm_serial_nor_host_t *)ops;
    SPI_Type *spi_dev = (SPI_Type *)host->host_param.param.host_base;
    spi_timing_config_t timing_config = {0};
    uint32_t div;
    spi_master_get_default_timing_config(&timing_config);
    board_init_spi_clock(spi_dev);
    timing_config.master_config.clk_src_freq_in_hz = clock_get_frequency(host->host_param.param.clock_name);
    timing_config.master_config.sclk_freq_in_hz = frequency;
    if (spi_master_timing_init(spi_dev, &timing_config) != status_success) {
        div = clock_get_frequency(BOARD_NOR_FLASH_SPI_SRC_CLK_NAME) / frequency;
        clock_set_source_divider(host->host_param.param.clock_name, BOARD_NOR_FLASH_SPI_SRC_CLK, div);
        spi_dev->TIMING |= SPI_TIMING_SCLK_DIV_SET(0xFF);
    }
}

static void serial_nor_write_cs(uint32_t cs_pin, uint8_t state)
{
    gpio_write_pin(HPM_GPIO0, GPIO_GET_PORT_INDEX(cs_pin), GPIO_GET_PIN_INDEX(cs_pin), state);
}

void serial_nor_spi_pins_init(SPI_Type *spi)
{
    (void)spi;
    init_spi_nor_flash_pins();
}
