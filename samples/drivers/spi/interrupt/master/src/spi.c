/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_spi_drv.h"
#include "hpm_clock_drv.h"

#define TEST_SPI           BOARD_APP_SPI_BASE
#define TEST_SPI_IRQ       BOARD_APP_SPI_IRQ
#define TEST_SPI_CLK_NAME  BOARD_APP_SPI_CLK_NAME
/* The frequency of SCLK should adapt to SPI interrupt response time, otherwise it may cause overrun or underrun */
#ifndef EXAMPLE_SPI_SCLK_FREQ
#define TEST_SPI_SCLK_FREQ BOARD_APP_SPI_SCLK_FREQ
#else
#define TEST_SPI_SCLK_FREQ EXAMPLE_SPI_SCLK_FREQ
#endif

volatile bool spi_transfer_done;

uint8_t *sent_buff;
uint8_t *receive_buff;
uint32_t sent_count;
uint32_t receive_count;

SDK_DECLARE_EXT_ISR_M(TEST_SPI_IRQ, spi_isr)
void spi_isr(void)
{
    volatile uint32_t irq_status;
    volatile uint8_t data_len_in_bytes;
    hpm_stat_t stat;

    data_len_in_bytes = spi_get_data_length_in_bytes(TEST_SPI);
    irq_status = spi_get_interrupt_status(TEST_SPI); /* get interrupt stat */

    if (irq_status & spi_end_int) {
        spi_transfer_done = true;
        spi_disable_interrupt(TEST_SPI, spi_end_int);
        spi_clear_interrupt_status(TEST_SPI, spi_end_int);
    }
    if (irq_status & spi_rx_fifo_threshold_int) {
        stat = spi_read_data(TEST_SPI, data_len_in_bytes, receive_buff, 1);
        if (stat != status_success) {
            printf("spi read data filed, error: %d\n", stat);
        }
        spi_clear_interrupt_status(TEST_SPI, spi_rx_fifo_threshold_int);

        receive_buff += data_len_in_bytes;
        receive_count--;
        if (receive_count == 0) {
            spi_disable_interrupt(TEST_SPI, spi_rx_fifo_threshold_int);
        }
    }
    if (irq_status & spi_tx_fifo_threshold_int) {
        stat = spi_write_data(TEST_SPI, data_len_in_bytes, sent_buff, 1);
        if (stat != status_success) {
            printf("spi write data filed, error: %d\n", stat);
        }
        spi_clear_interrupt_status(TEST_SPI, spi_tx_fifo_threshold_int);

        sent_buff += data_len_in_bytes;
        sent_count--;
        if (sent_count == 0) {
            spi_disable_interrupt(TEST_SPI, spi_tx_fifo_threshold_int);
        }
    }
}

int main(void)
{
    uint8_t cmd = 0x1a;
    uint32_t addr = 0x10;
    uint8_t wbuff[10] = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9};
    uint8_t rbuff[10] = {0};
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;
    uint32_t spi_clcok;

    /* bsp initialization */
    board_init();
    spi_clcok = board_init_spi_clock(TEST_SPI);
    board_init_spi_pins(TEST_SPI);
    intc_m_enable_irq_with_priority(TEST_SPI_IRQ, 1);
    printf("SPI-Master Interrupt Transfer Example\n");

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clcok;
    timing_config.master_config.sclk_freq_in_hz = TEST_SPI_SCLK_FREQ;
    if (status_success != spi_master_timing_init(TEST_SPI, &timing_config)) {
        printf("SPI master timing init failed\n");
        while (1) {
        }
    }
    printf("SPI-Master transfer timing is configured.\n");
    printf("SPI-Master transfer source clock frequency: %dHz\n", timing_config.master_config.clk_src_freq_in_hz);
    printf("SPI-Master tannsfer sclk frequecny: %dHz\n", timing_config.master_config.sclk_freq_in_hz);

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = 8U;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(TEST_SPI, &format_config);
    printf("SPI-Master transfer format is configured.\n");

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = true;
    control_config.master_config.addr_enable = true;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;
    control_config.common_config.trans_mode = spi_trans_write_dummy_read;

    sent_buff = (uint8_t *)wbuff;
    receive_buff = (uint8_t *)rbuff;
    sent_count = ARRAY_SIZE(wbuff);
    receive_count = ARRAY_SIZE(rbuff);
    stat = spi_control_init(TEST_SPI, &control_config, ARRAY_SIZE(wbuff), ARRAY_SIZE(rbuff));
    if (stat != status_success) {
        printf("SPI control init failed.\n");
        return -1;
    }

    stat = spi_write_address(TEST_SPI, spi_master_mode, &control_config, &addr);
    if (stat != status_success) {
        printf("SPI write address failed.\n");
        return -1;
    }

    /* setup fifo threshold interrupt and transfer complete interrupt */
    spi_set_tx_fifo_threshold(TEST_SPI, SPI_SOC_FIFO_DEPTH - 1U);
    spi_set_rx_fifo_threshold(TEST_SPI, 1U);
    spi_enable_interrupt(TEST_SPI, spi_tx_fifo_threshold_int | spi_rx_fifo_threshold_int | spi_end_int);

    /* start a SPI transfer */
    stat = spi_write_command(TEST_SPI, spi_master_mode, &control_config, &cmd);
    if (stat != status_success) {
        printf("SPI write command failed.\n");
        return -1;
    }

    while (!spi_transfer_done) {
        __asm("nop");
    }

    printf("SPI master sent data:");
    for (uint32_t i = 0; i < ARRAY_SIZE(wbuff); i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", wbuff[i]);
    }
    printf("\n");
    printf("SPI master receive data:");
    for (uint32_t i = 0; i < ARRAY_SIZE(rbuff); i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", rbuff[i]);
    }
    printf("\n");
    printf("SPI master transfer done.\n");

    while (1) {
    }

    return 0;
}
