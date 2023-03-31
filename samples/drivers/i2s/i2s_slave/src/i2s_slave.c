/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "i2s_slave.h"
#include "hpm_i2s_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "audio_data.h"

/* data buffer */
ATTR_PLACE_AT_NONCACHEABLE_BSS int16_t buf[DMA_TRANSFER_SIZE / sizeof(int16_t)];

/* DMA transfer flags */
volatile bool dma_transfer_done = true;
volatile bool dma_transfer_error;

void isr_dma(void)
{
    uint32_t stat;

    stat = dma_get_irq_status(BOARD_APP_HDMA);
    dma_clear_irq_status(BOARD_APP_HDMA, stat);

    if (stat & DMA_CHANNEL_IRQ_STATUS_TC(DMA_MUX_CHANNEL)) {
        dma_transfer_done = true;
    } else {
        dma_transfer_error = true;
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_dma)

void dma_transfer_config(uint32_t size, uint32_t *ptr)
{
    dma_channel_config_t ch_config = {0};

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, ISR_PRIORITY_LEVEL);

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.dst_addr = (uint32_t)((uint8_t *)&I2S_SLAVE->TXD[I2S_SLAVE_DATA_LINE] + sizeof(uint16_t));
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = size;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, DMA_MUX_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL, I2S_SLAVE_TX_DMAMUX_SRC, true);
}

void i2s_slave_config(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    i2s_get_default_config(I2S_SLAVE, &i2s_config);
    i2s_config.fifo_threshold = I2S_SLAVE_FIFO_THR;
    i2s_config.use_external_mclk = true;
    i2s_config.use_external_bclk = true;
    i2s_config.use_external_fclk = true;
    i2s_init(I2S_SLAVE, &i2s_config);

    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = I2S_SLAVE_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.master_mode = false;
    transfer.audio_depth = audio_depth;
    transfer.channel_length = i2s_channel_length_32_bits;

    i2s_config_tx_slave(I2S_SLAVE, &transfer);
    i2s_enable_tx_dma_request(I2S_SLAVE);
}

void i2s_slave_transfer(void)
{
    static int idx = 0;
    int cnt = sizeof(audio_data) / sizeof(uint16_t);
    int16_t *p = (int16_t *)audio_data;

    for (size_t cnt = 0; cnt < DMA_TRANSFER_SIZE / sizeof(uint16_t); cnt += sizeof(uint16_t)) {
        buf[cnt] = p[idx++];
        buf[cnt + 1] = p[idx++];
    }

    if (idx >= cnt) {
        idx = 0;
    }

    while (dma_transfer_done == false) {

    }

    dma_transfer_config(DMA_TRANSFER_SIZE, (uint32_t *)buf);
    dma_transfer_done = false;
}

int main(void)
{
    board_init();
    board_init_i2s_pins(I2S_SLAVE);
    i2s_slave_config(I2S_SAMPLE_RATE_HZ, I2S_SAMPLE_BITDEPTH, I2S_SAMPLE_CHANNEL);
    printf("I2S Slave Demo\n");

    while (1) {
        i2s_slave_transfer();
    }

    return 0;
}