/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_spi_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"

#define TEST_SPI               BOARD_APP_SPI_BASE
#define TEST_SPI_DMA           BOARD_APP_HDMA
#define TEST_SPI_DMAMUX        BOARD_APP_DMAMUX
#define TEST_SPI_RX_DMA        BOARD_APP_SPI_RX_DMA
#define TEST_SPI_RX_DMAMUX_CH  BOARD_APP_SPI_RX_DMAMUX_CH
#define TEST_SPI_TX_DMA        BOARD_APP_SPI_TX_DMA
#define TEST_SPI_TX_DMAMUX_CH  BOARD_APP_SPI_TX_DMAMUX_CH

#ifndef PLACE_BUFF_AT_CACHEABLE
#define PLACE_BUFF_AT_CACHEABLE 1
#endif

#define TEST_TRANSFER_DATA_IN_BYTE  (128U)
#if PLACE_BUFF_AT_CACHEABLE
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t sent_buff[TEST_TRANSFER_DATA_IN_BYTE];
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t receive_buff[TEST_TRANSFER_DATA_IN_BYTE];
#else
ATTR_PLACE_AT_NONCACHEABLE uint8_t sent_buff[TEST_TRANSFER_DATA_IN_BYTE];
ATTR_PLACE_AT_NONCACHEABLE uint8_t receive_buff[TEST_TRANSFER_DATA_IN_BYTE];
#endif

void prepare_transfer_data(void)
{
    for (uint32_t i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        sent_buff[i] = i % 0xFF;
    }
}

void spi_slave_check_transfer_data(SPI_Type *ptr)
{
    uint32_t i = 0U, error_count = 0U, slv_rcnt, slv_wcnt;

    /* Wait for the spi slave transfer to complete */
    do {
        slv_wcnt = SPI_SLVDATACNT_WCNT_GET(ptr->SLVDATACNT);
        slv_rcnt = SPI_SLVDATACNT_RCNT_GET(ptr->SLVDATACNT);
    } while ((slv_wcnt < TEST_TRANSFER_DATA_IN_BYTE) || (slv_rcnt < TEST_TRANSFER_DATA_IN_BYTE));

    printf("The sent data are:");
    for (i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\r\n");
        }
        printf("0x%02X ", sent_buff[i]);
    }
    printf("\n");
    printf("The received data are:");
    for (i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", receive_buff[i]);
        if (sent_buff[i] != receive_buff[i]) {
            error_count++;
        }
    }
    printf("\n");
    if (error_count == 0) {
        printf("SPI transfer all data matched!\n");
    } else {
        printf("Error occurred in SPI transfer!\n");
    }
}

hpm_stat_t spi_tx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, SPI_Type *spi_ptr, uint32_t src, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = (uint32_t)&spi_ptr->DATA;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config);
}

hpm_stat_t spi_rx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, SPI_Type *spi_ptr, uint32_t dst, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&spi_ptr->DATA;
    config.src_fixed = true;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config);
}

int main(void)
{
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;
    uint8_t cmd = 0x0;
    uint32_t addr = 0x0;

    board_init();
    board_init_spi_clock(TEST_SPI);
    board_init_spi_pins(TEST_SPI);
    printf("SPI Slave DMA Transfer Example\n");

    /* set SPI format config for master */
    spi_slave_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = 8U;
    format_config.common_config.data_merge = false;
    format_config.common_config.mosi_bidir = false;
    format_config.common_config.lsb = false;
    format_config.common_config.mode = spi_slave_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(TEST_SPI, &format_config);

    /* set SPI control config for master */
    spi_slave_get_default_control_config(&control_config);
    control_config.slave_config.slave_data_only = false;
    control_config.common_config.tx_dma_enable = true;
    control_config.common_config.rx_dma_enable = true;
    control_config.common_config.trans_mode = spi_trans_read_dummy_write;
    control_config.common_config.data_phase_fmt = spi_single_io_mode;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;

    stat = spi_setup_dma_transfer(TEST_SPI,
                        &control_config,
                        &cmd, &addr,
                        TEST_TRANSFER_DATA_IN_BYTE, TEST_TRANSFER_DATA_IN_BYTE);
    if (stat != status_success) {
        printf("spi setup dma transfer failed\n");
        while (1) {
        }
    }

    prepare_transfer_data();

    /* setup spi tx trigger dma transfer */
#if PLACE_BUFF_AT_CACHEABLE
    if (l1c_dc_is_enabled()) {
        /* cache writeback for sent buff */
        l1c_dc_writeback((uint32_t)sent_buff, TEST_TRANSFER_DATA_IN_BYTE);
    }
#endif
    dmamux_config(TEST_SPI_DMAMUX, TEST_SPI_TX_DMAMUX_CH, TEST_SPI_TX_DMA, true);
    stat = spi_tx_trigger_dma(TEST_SPI_DMA,
                            TEST_SPI_TX_DMAMUX_CH,
                            TEST_SPI,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)sent_buff),
                            TEST_TRANSFER_DATA_IN_BYTE);
    if (stat != status_success) {
        printf("spi tx trigger dma failed\n");
        while (1) {
        }
    }

    /* setup spi rx trigger dma transfer*/
    dmamux_config(TEST_SPI_DMAMUX, TEST_SPI_RX_DMAMUX_CH, TEST_SPI_RX_DMA, true);
    stat = spi_rx_trigger_dma(TEST_SPI_DMA,
                            TEST_SPI_RX_DMAMUX_CH,
                            TEST_SPI,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)receive_buff),
                            TEST_TRANSFER_DATA_IN_BYTE);
    if (stat != status_success) {
        printf("spi rx trigger dma failed\n");
        while (1) {
        }
    }
#if PLACE_BUFF_AT_CACHEABLE
    if (l1c_dc_is_enabled()) {
        /* cache invalidate for receive buff */
        l1c_dc_invalidate((uint32_t)receive_buff, TEST_TRANSFER_DATA_IN_BYTE);
    }
#endif

    spi_slave_check_transfer_data(TEST_SPI);

    while (1) {
    }

    return 0;
}
