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

volatile bool spi_transfer_done;

uint8_t *sent_buff;
uint8_t *receive_buff;
uint32_t sent_count;
uint32_t receive_count;

void spi_isr(void)
{
    volatile uint32_t irq_status;
    volatile uint8_t data_len_in_bytes;
    hpm_stat_t stat;

    data_len_in_bytes = spi_get_data_length_in_bytes(TEST_SPI);
    irq_status = spi_get_interrupt_status(TEST_SPI); /* get interrupt stat */

    if (irq_status & spi_slave_cmd_int) {
        spi_disable_interrupt(TEST_SPI, spi_slave_cmd_int);
        spi_enable_interrupt(TEST_SPI, spi_tx_fifo_threshold_int | spi_rx_fifo_threshold_int | spi_end_int);
    }
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
SDK_DECLARE_EXT_ISR_M(TEST_SPI_IRQ, spi_isr)

int main(void)
{
    uint8_t cmd = 0;
    uint8_t wbuff[10] = {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9};
    uint8_t rbuff[10] = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;

    /* bsp initialization */
    board_init();
    board_init_spi_clock(TEST_SPI);
    board_init_spi_pins(TEST_SPI);
    intc_m_enable_irq_with_priority(TEST_SPI_IRQ, 1);
    printf("SPI-Slave Interrupt Transfer Example\n");

    /* set SPI format config for slave */
    spi_slave_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = 8U;
    format_config.common_config.mode = spi_slave_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(TEST_SPI, &format_config);
    printf("SPI-Slave transfer format is configured.\n");

    /* set SPI control config for slave */
    spi_slave_get_default_control_config(&control_config);
    control_config.slave_config.slave_data_only = false;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;
    control_config.common_config.trans_mode = spi_trans_read_dummy_write;

    sent_buff = (uint8_t *)wbuff;
    receive_buff = (uint8_t *)rbuff;
    sent_count = ARRAY_SIZE(wbuff);
    receive_count = ARRAY_SIZE(rbuff);
    stat = spi_control_init(TEST_SPI, &control_config, ARRAY_SIZE(wbuff), ARRAY_SIZE(rbuff));
    if (stat != status_success) {
        printf("SPI control init failed.\n");
        return stat;
    }

    /* setup fifo threshold interrupt and transfer complete interrupt */
    spi_set_tx_fifo_threshold(TEST_SPI, SPI_SOC_FIFO_DEPTH - 1U);
    spi_set_rx_fifo_threshold(TEST_SPI, 1U);
    spi_enable_interrupt(TEST_SPI, spi_slave_cmd_int);

    printf("SPI-Slave transfer waits.\n");
    while (!spi_transfer_done) {
        __asm("nop");
    }

    stat = spi_read_command(TEST_SPI, spi_slave_mode, &control_config, &cmd);
    if (stat != status_success) {
        printf("SPI slave mode read command failed.\n");
        return stat;
    }
    printf("SPI slave receive command: 0x%02X\n", cmd);

    printf("SPI slave receive data:");
    for (uint32_t i = 0; i < ARRAY_SIZE(rbuff); i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", rbuff[i]);
    }
    printf("\n");
    printf("SPI slave sent data:");
    for (uint32_t i = 0; i < ARRAY_SIZE(wbuff); i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", wbuff[i]);
    }
    printf("\n");
    printf("SPI slave transfer done.\n");

    while (1) {
    }

    return 0;
}
