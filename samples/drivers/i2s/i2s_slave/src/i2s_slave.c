/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file implements an I2S slave device that transmits audio data to an I2S master.
 * The implementation uses DMA for efficient data transfer from a buffer to the I2S TX FIFO.
 */

#include "board.h"
#include "i2s_slave.h"
#include "hpm_i2s_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "audio_data.h"

/* DMA transfer buffer - placed in non-cacheable memory to ensure DMA coherency */
ATTR_PLACE_AT_NONCACHEABLE_BSS int16_t buf[DMA_TRANSFER_SIZE / sizeof(int16_t)];

/* DMA transfer status flags */
volatile bool dma_transfer_done = true;  /* Indicates if current DMA transfer is complete */
volatile bool dma_transfer_error;        /* Indicates if DMA transfer encountered an error */

/*
 * DMA Interrupt Service Routine
 * Handles DMA transfer completion and error events
 */
SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    uint32_t stat;

    /* Check and clear DMA transfer status */
    stat = dma_check_transfer_status(APP_DMA, DMA_CHANNEL);
    dma_clear_transfer_status(APP_DMA, DMA_CHANNEL);

    /* Update transfer status flags based on DMA status */
    if (stat & DMA_CHANNEL_STATUS_TC) {
        dma_transfer_done = true;  /* Transfer complete */
    } else {
        dma_transfer_error = true; /* Transfer error occurred */
    }
}

/*
 * Configure DMA channel for I2S data transfer
 * @param size: Size of data to transfer in bytes
 * @param ptr: Source data pointer
 */
void dma_transfer_config(uint32_t size, uint32_t *ptr)
{
    dma_channel_config_t ch_config = {0};

    /* Enable DMA interrupt with specified priority */
    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, ISR_PRIORITY_LEVEL);

    /* Configure DMA channel parameters */
    dma_default_channel_config(APP_DMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.dst_addr = (uint32_t)((uint8_t *)&I2S_SLAVE->TXD[I2S_SLAVE_TX_DATA_LINE] + sizeof(uint16_t));
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;      /* 16-bit data width */
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;  /* Increment source address */
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;      /* Fixed destination address (I2S FIFO) */
    ch_config.size_in_byte = size;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;       /* Enable handshaking with I2S */
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;/* 2 transfers per burst */

    /* Setup DMA channel with configuration */
    if (status_success != dma_setup_channel(APP_DMA, DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }

    /* Configure DMAMUX to connect DMA channel with I2S TX request */
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL, I2S_SLAVE_TX_DMAMUX_SRC, true);
}

/*
 * Configure I2S controller in slave mode
 * @param sample_rate: Audio sampling rate in Hz
 * @param audio_depth: Bit depth of audio samples
 * @param channel_num: Number of audio channels
 */
void i2s_slave_config(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    /* Configure basic I2S parameters */
    i2s_get_default_config(I2S_SLAVE, &i2s_config);
    i2s_config.tx_fifo_threshold = I2S_SLAVE_FIFO_THR;
    i2s_config.rx_fifo_threshold = I2S_SLAVE_FIFO_THR;
    i2s_config.use_external_mclk = true;  /* Use external master clock */
    i2s_config.use_external_bclk = true;  /* Use external bit clock */
    i2s_config.use_external_fclk = true;  /* Use external frame clock */
    i2s_init(I2S_SLAVE, &i2s_config);

    /* Configure I2S transfer parameters */
    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = I2S_SLAVE_TX_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.master_mode = false;                          /* Configure as slave */
    transfer.channel_length = i2s_channel_length_32_bits;
    transfer.channel_num_per_frame = channel_num;

    /* Initialize I2S in slave mode */
    i2s_config_tx_slave(I2S_SLAVE, &transfer);
    i2s_enable_tx_dma_request(I2S_SLAVE);  /* Enable DMA requests */
    i2s_start(I2S_SLAVE);                  /* Start I2S operation */
}

/*
 * Handle I2S data transfer
 * Copies audio data to DMA buffer and initiates DMA transfer
 */
void i2s_slave_transfer(void)
{
    static int idx = 0;
    int cnt = sizeof(audio_data) / sizeof(uint16_t);
    int16_t *p = (int16_t *)audio_data;

    /* Copy audio data to DMA buffer */
    for (size_t cnt = 0; cnt < DMA_TRANSFER_SIZE / sizeof(uint16_t); cnt += sizeof(uint16_t)) {
        buf[cnt] = p[idx++];
        buf[cnt + 1] = p[idx++];
    }

    /* Reset index if reached end of audio data */
    if (idx >= cnt) {
        idx = 0;
    }

    /* Wait for previous DMA transfer to complete */
    while (dma_transfer_done == false) {
    }

    /* Start new DMA transfer */
    dma_transfer_config(DMA_TRANSFER_SIZE, (uint32_t *)buf);
    dma_transfer_done = false;
}

/*
 * Main application entry point
 */
int main(void)
{
    /* Initialize board and I2S hardware */
    board_init();
    board_init_i2s_pins(I2S_SLAVE);    /* Configure I2S pins */
    clock_add_to_group(I2S_SLAVE_CLOCK_NAME, (BOARD_RUNNING_CORE & 0x1)); /* Enable I2S clock */

    /* Configure I2S slave with specified audio parameters */
    i2s_slave_config(I2S_SAMPLE_RATE_HZ, I2S_SAMPLE_BITDEPTH, I2S_SAMPLE_CHANNEL);
    printf("I2S Slave Demo\n");

    /* Main processing loop */
    while (1) {
        i2s_slave_transfer();
    }

    return 0;
}
