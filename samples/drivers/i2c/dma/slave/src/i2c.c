/*
 * Copyright (c) 2022,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_i2c_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"


#define TEST_I2C               BOARD_APP_I2C_BASE
#define TEST_I2C_CLOCK_NAME    BOARD_APP_I2C_CLK_NAME
#define TEST_I2C_DMA           BOARD_APP_I2C_DMA
#define TEST_I2C_DMAMUX        BOARD_APP_I2C_DMAMUX
#define TEST_I2C_DMAMUX_SRC    BOARD_APP_I2C_DMA_SRC
#define TEST_I2C_DMA_CH        0
#define TEST_I2C_DMAMUX_CH     DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_I2C_DMA, TEST_I2C_DMA_CH)
#define TEST_I2C_IRQ           BOARD_APP_I2C_IRQ

#define TEST_I2C_SLAVE_ADDRESS (0x16U)

#ifndef PLACE_BUFF_AT_CACHEABLE
#define PLACE_BUFF_AT_CACHEABLE 1
#endif

#define TEST_TRANSFER_DATA_IN_BYTE (128U)
#if PLACE_BUFF_AT_CACHEABLE
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t data_buff[TEST_TRANSFER_DATA_IN_BYTE];
#else
ATTR_PLACE_AT_NONCACHEABLE uint8_t data_buff[TEST_TRANSFER_DATA_IN_BYTE];
#endif

volatile bool i2c_address_hit;
volatile bool i2c_receive_complete;
volatile bool i2c_transmit_complete;

SDK_DECLARE_EXT_ISR_M(TEST_I2C_IRQ, i2c_isr)
void i2c_isr(void)
{
    volatile uint32_t status;
    status = i2c_get_status(TEST_I2C);

    /* address hit */
    if (status & I2C_EVENT_ADDRESS_HIT) {
        i2c_address_hit = true;
        i2c_clear_status(TEST_I2C, I2C_EVENT_ADDRESS_HIT);
    }

    /* complete */
    if (status & I2C_EVENT_TRANSACTION_COMPLETE) {
        if (I2C_DIR_SLAVE_READ == i2c_get_direction(TEST_I2C)) {
            i2c_receive_complete = true;
        } else {
            i2c_transmit_complete = true;
        }
        i2c_clear_status(TEST_I2C, I2C_EVENT_TRANSACTION_COMPLETE);
    }
}

hpm_stat_t i2c_tx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, I2C_Type *i2c_ptr, uint32_t src, uint32_t size)
{
    dma_handshake_config_t config;

    /* disable i2c dma before next dma transaction */
    i2c_dma_disable(i2c_ptr);
    dma_default_handshake_config(dma_ptr, &config);
    config.ch_index = ch_num;
    config.dst = (uint32_t)&i2c_ptr->DATA;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

hpm_stat_t i2c_rx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, I2C_Type *i2c_ptr, uint32_t dst, uint32_t size)
{
    dma_handshake_config_t config;

    /* disable i2c dma before next dma transaction */
    i2c_dma_disable(i2c_ptr);
    dma_default_handshake_config(dma_ptr, &config);
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&i2c_ptr->DATA;
    config.src_fixed = true;
    config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

static void print_received_data(void)
{
    printf("The received data are:");
    for (uint32_t i = 0; i < TEST_TRANSFER_DATA_IN_BYTE; i++) {
        if ((i & 0x0FU) == 0U) {
            printf("\n");
        }
        printf("0x%02X ", data_buff[i]);
    }
    printf("\n");
}

int main(void)
{
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;

    board_init();
    freq = board_init_i2c_clock(TEST_I2C);
    init_i2c_pins(TEST_I2C);

    printf("I2C DMA slave example, I2C will sent back the received data\n");

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    stat = i2c_init_slave(TEST_I2C, freq, &config, TEST_I2C_SLAVE_ADDRESS);
    if (stat != status_success) {
        return stat;
    }

    /* DMAMUX config */
    dmamux_config(TEST_I2C_DMAMUX, TEST_I2C_DMAMUX_CH, TEST_I2C_DMAMUX_SRC, true);

    /* Enable I2C interrupt */
    i2c_enable_irq(TEST_I2C, I2C_EVENT_ADDRESS_HIT | I2C_EVENT_TRANSACTION_COMPLETE);
    intc_m_enable_irq_with_priority(TEST_I2C_IRQ, 1);

    while (1) {
        if (i2c_address_hit) {
            if (I2C_DIR_SLAVE_READ == i2c_get_direction(TEST_I2C)) {
                /* I2C DMA Receive */
                stat = i2c_rx_trigger_dma(TEST_I2C_DMA,
                                        TEST_I2C_DMA_CH,
                                        TEST_I2C,
                                        core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)data_buff),
                                        TEST_TRANSFER_DATA_IN_BYTE);
                if (stat != status_success) {
                    printf("i2c rx trigger dma failed\n");
                    break;
                }
                if (status_success != i2c_slave_dma_transfer(TEST_I2C, TEST_TRANSFER_DATA_IN_BYTE)) {
                    printf("I2C slave start dma read failed!\n");
                    dma_disable_channel(TEST_I2C_DMA, TEST_I2C_DMA_CH);
                    break;
                }
                #if PLACE_BUFF_AT_CACHEABLE
                if (l1c_dc_is_enabled()) {
                    /* cache invalidate after DMA receive data */
                    l1c_dc_invalidate((uint32_t)data_buff, TEST_TRANSFER_DATA_IN_BYTE);
                }
                #endif
            } else {
                /* I2C DMA Transmit */
                #if PLACE_BUFF_AT_CACHEABLE
                if (l1c_dc_is_enabled()) {
                    /* cache writeback before DMA sent data */
                    l1c_dc_writeback((uint32_t)data_buff, TEST_TRANSFER_DATA_IN_BYTE);
                }
                #endif
                stat = i2c_tx_trigger_dma(TEST_I2C_DMA,
                                        TEST_I2C_DMA_CH,
                                        TEST_I2C,
                                        core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)data_buff),
                                        TEST_TRANSFER_DATA_IN_BYTE);
                if (stat != status_success) {
                    printf("i2c tx trigger dma failed\n");
                    break;
                }
                if (status_success != i2c_slave_dma_transfer(TEST_I2C, TEST_TRANSFER_DATA_IN_BYTE)) {
                    printf("I2C slave start dma write failed!\n");
                    dma_disable_channel(TEST_I2C_DMA, TEST_I2C_DMA_CH);
                    break;
                }
            }

            i2c_address_hit = false;
        }


        /* Complete one reception and one transmission */
        if ((i2c_receive_complete) && (i2c_transmit_complete)) {
            print_received_data();
            break;
        }
    }

    while (1) {
    }

    return 0;
}
