/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_spi.h"
#include "hpm_l1c_drv.h"

#define TEST_SPI               BOARD_APP_SPI_BASE
#define TEST_SPI_SCLK_FREQ     BOARD_APP_SPI_SCLK_FREQ
#define TEST_SPI_DMA           BOARD_APP_HDMA
#define TEST_SPI_RX_DMA_CH     BOARD_APP_SPI_RX_DMAMUX_CH
#define TEST_SPI_TX_DMA_CH     BOARD_APP_SPI_TX_DMAMUX_CH
#define TEST_SPI_DMAMUX        BOARD_APP_DMAMUX
#define TEST_SPI_RX_DMA_REQ    BOARD_APP_SPI_RX_DMA
#define TEST_SPI_RX_DMAMUX_CH  BOARD_APP_SPI_RX_DMAMUX_CH
#define TEST_SPI_TX_DMA_REQ    BOARD_APP_SPI_TX_DMA
#define TEST_SPI_TX_DMAMUX_CH  BOARD_APP_SPI_TX_DMAMUX_CH
#define TEST_SPI_DMA_IRQ       BOARD_APP_HDMA_IRQ
#define TEST_SPI_GPIO_CS_PIN   BOARD_SPI_CS_PIN

#ifndef PLACE_BUFF_AT_CACHEABLE
#define PLACE_BUFF_AT_CACHEABLE 1
#endif

#define SPI_TRANS_DATA_BYTE  (512U + 64U)
#define SPI_TRANS_COUNT      ((SPI_TRANS_DATA_BYTE + SPI_SOC_TRANSFER_COUNT_MAX - 1) / SPI_SOC_TRANSFER_COUNT_MAX)

/* data buff */
#if PLACE_BUFF_AT_CACHEABLE
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t sent_buff[SPI_TRANS_DATA_BYTE];
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t receive_buff[SPI_TRANS_DATA_BYTE];
#else
ATTR_PLACE_AT_NONCACHEABLE uint8_t sent_buff[SPI_TRANS_DATA_BYTE];
ATTR_PLACE_AT_NONCACHEABLE uint8_t receive_buff[SPI_TRANS_DATA_BYTE];
#endif
/* dma descriptors buff */
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_linked_descriptor_t dma_linked_descriptor[SPI_TRANS_COUNT * SPI_DMA_DESC_COUNT_PER_TRANS];
ATTR_PLACE_AT_NONCACHEABLE uint32_t spi_transctrl[SPI_TRANS_COUNT];

volatile bool spi_rx_dma_trans_done;
volatile bool spi_tx_dma_trans_done;

spi_context_t spi_context = {
    .ptr = TEST_SPI,
    .write_cs = board_write_spi_cs,
    .tx_buff = sent_buff,
    .tx_size = SPI_TRANS_DATA_BYTE,
    .rx_buff = receive_buff,
    .rx_size = SPI_TRANS_DATA_BYTE,
    .per_trans_max = SPI_SOC_TRANSFER_COUNT_MAX,
    .dma_context = {
            .dma_ptr = TEST_SPI_DMA,
            .rx_dma_ch = TEST_SPI_RX_DMA_CH,
            .tx_dma_ch = TEST_SPI_TX_DMA_CH,
            .dmamux_ptr = TEST_SPI_DMAMUX,
            .rx_dmamux_ch = TEST_SPI_RX_DMAMUX_CH,
            .tx_dmamux_ch = TEST_SPI_TX_DMAMUX_CH,
            .rx_req = TEST_SPI_RX_DMA_REQ,
            .tx_req = TEST_SPI_TX_DMA_REQ,
    },
    .running_core = BOARD_RUNNING_CORE,
    .dma_linked_descriptor = dma_linked_descriptor,
    .spi_transctrl = spi_transctrl,
};

void prepare_spi_sent_data(void)
{
    for (uint32_t i = 0; i < SPI_TRANS_DATA_BYTE; i++) {
        sent_buff[i] = i % 0x100;
    }
}

void spi_check_transfer_data(void)
{
    uint32_t i = 0U, error_count = 0U;

    printf("The sent data are:");
    for (i = 0; i < SPI_TRANS_DATA_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\r\n");
        }
        printf("0x%02X ", sent_buff[i]);
    }
    printf("\n");
    printf("The received data are:");
    for (i = 0; i < SPI_TRANS_DATA_BYTE; i++) {
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

void isr_dma(void)
{
    volatile hpm_stat_t rx_stat, tx_stat;
    rx_stat = dma_check_transfer_status(TEST_SPI_DMA, TEST_SPI_RX_DMAMUX_CH);
    tx_stat = dma_check_transfer_status(TEST_SPI_DMA, TEST_SPI_TX_DMAMUX_CH);
    if (rx_stat & (DMA_CHANNEL_STATUS_TC | DMA_CHANNEL_STATUS_ERROR | DMA_CHANNEL_STATUS_ABORT)) {
        spi_rx_dma_trans_done = true;
    }
    if (tx_stat & (DMA_CHANNEL_STATUS_TC | DMA_CHANNEL_STATUS_ERROR | DMA_CHANNEL_STATUS_ABORT)) {
        spi_tx_dma_trans_done = true;
    }
}
SDK_DECLARE_EXT_ISR_M(TEST_SPI_DMA_IRQ, isr_dma)

int main(void)
{
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;
    uint8_t cmd = 0x1a;
    uint32_t addr = 0x10;

    board_init();
    board_init_spi_clock(TEST_SPI);
    board_init_spi_pins_with_gpio_as_cs(TEST_SPI);
    printf("SPI Master DMA Transfer Example\n");

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = board_init_spi_clock(TEST_SPI);
    timing_config.master_config.sclk_freq_in_hz = TEST_SPI_SCLK_FREQ;
    if (status_success != spi_master_timing_init(TEST_SPI, &timing_config)) {
        printf("SPI master timming init failed\n");
    }

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.master_config.addr_len_in_bytes = 1U;
    format_config.common_config.data_len_in_bits = 8U;
    format_config.common_config.data_merge = false;
    format_config.common_config.mosi_bidir = false;
    format_config.common_config.lsb = false;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(TEST_SPI, &format_config);

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = true;
    control_config.master_config.addr_enable = true;
    control_config.master_config.addr_phase_fmt = spi_address_phase_format_single_io_mode;
    control_config.common_config.tx_dma_enable = true;
    control_config.common_config.rx_dma_enable = true;
    control_config.common_config.trans_mode = spi_trans_write_read_together;
    control_config.common_config.data_phase_fmt = spi_single_io_mode;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;

    prepare_spi_sent_data();
    intc_m_enable_irq_with_priority(TEST_SPI_DMA_IRQ, 1);

    spi_context.cmd = cmd;
    spi_context.addr = addr;
    spi_context.cs_pin = TEST_SPI_GPIO_CS_PIN;
    stat = hpm_spi_setup_dma_transfer(&spi_context, &control_config);
    if (stat != status_success) {
        printf("spi setup dma transfer failed\n");
    }

    while (!spi_tx_dma_trans_done) {
        __asm("nop");
    }
    /* release gpio pin which used as spi cs function when SPI trans completed */
    hpm_spi_release_gpio_cs(&spi_context);

    spi_check_transfer_data();
    printf("SPI Master DMA Transfer Completed\n");

    while (1) {
    }

    return 0;
}
