/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This example demonstrates I2S master mode operation with DMA transfer.
 * It shows how to:
 * 1. Configure I2S in master mode for audio data reception
 * 2. Configure DAO for audio playback
 * 3. Set up DMA for data transfer between I2S and DAO
 */

#include "board.h"
#include "i2s_master.h"
#include "hpm_i2s_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_dao_drv.h"

/* DMA transfer status flags */
volatile bool dma_transfer_done = false;    /* Flag indicating DMA transfer completion */
volatile bool dma_transfer_error;           /* Flag indicating DMA transfer error */

/*
 * DMA interrupt handler
 * Handles DMA transfer completion and error events
 */
SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    uint32_t stat;

    /* Check and clear DMA transfer status */
    stat = dma_check_transfer_status(APP_DMA, DMA_CHANNEL);
    dma_clear_transfer_status(APP_DMA, DMA_CHANNEL);

    /* Update transfer status flags */
    if (stat & DMA_CHANNEL_STATUS_TC) {
        dma_transfer_done = true;
    } else {
        dma_transfer_error = true;
    }
}

/*
 * Configure DMA for data transfer between I2S and DAO
 * Parameters:
 * - size: Size of data to transfer in bytes
 */
void dma_transfer_config(uint32_t size)
{
    dma_channel_config_t ch_config = {0};

    /* Enable DMA interrupt */
    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, ISR_PRIORITY_LEVEL);

    /* Initialize DMA channel with default configuration */
    dma_default_channel_config(APP_DMA, &ch_config);

    /* Configure DMA channel parameters */
    ch_config.src_addr = (uint32_t)((uint8_t *)&I2S_MASTER->RXD[I2S_MASTER_RX_DATA_LINE] + sizeof(uint16_t));
    ch_config.dst_addr = (uint32_t)((uint8_t *)&I2S_DAO->TXD[I2S_DAO_DATA_LINE] + sizeof(uint16_t));
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;      /* 16-bit data width */
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;     /* Fixed source address */
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;     /* Fixed destination address */
    ch_config.size_in_byte = size;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;      /* Hardware handshake mode */
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;/* 2 transfers per burst */

    /* Setup DMA channel */
    if (status_success != dma_setup_channel(APP_DMA, DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }

    /* Configure DMAMUX for I2S RX */
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL, I2S_MASTER_RX_DMAMUX_SRC, true);
}

/*
 * Configure I2S master mode
 * Sets up I2S peripheral for audio data reception
 */
void i2s_master_config(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    uint32_t i2s_mclk_hz;

    /* Configure I2S interface */
    i2s_get_default_config(I2S_MASTER, &i2s_config);
    i2s_config.tx_fifo_threshold = I2S_MASTER_FIFO_THR;     /* Set TX FIFO threshold */
    i2s_config.rx_fifo_threshold = I2S_MASTER_FIFO_THR;     /* Set RX FIFO threshold */
    i2s_config.enable_mclk_out = true;                      /* Enable master clock output */
    i2s_init(I2S_MASTER, &i2s_config);

    /* Configure I2S transfer parameters */
    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = I2S_MASTER_RX_DATA_LINE;           /* Select RX data line */
    transfer.sample_rate = I2S_SAMPLE_RATE_HZ;              /* Set sampling rate */
    transfer.audio_depth = I2S_SAMPLE_BITDEPTH;             /* Set bit depth */
    transfer.channel_length = I2S_CHANNEL_LENGTH;           /* Set channel length */
    transfer.master_mode = true;                            /* Configure as master */

    /* Get and verify master clock frequency */
    i2s_mclk_hz = clock_get_frequency(I2S_MASTER_CLOCK_NAME);
    if (status_success != i2s_config_rx(I2S_MASTER, i2s_mclk_hz, &transfer)) {
        printf("I2S config failed!\n");
        while (1) {
            /* Configuration error */
        }
    }

    /* Enable DMA request and start I2S */
    i2s_enable_rx_dma_request(I2S_MASTER);
    i2s_start(I2S_MASTER);
}

/*
 * Configure I2S DAO mode
 * Sets up I2S and DAO for audio playback
 * Parameters:
 * - sample_rate: Audio sampling rate in Hz
 * - audio_depth: Bit depth of audio samples
 * - channel_num: Number of audio channels
 */
void i2s_dao_config(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    uint32_t i2s_mclk_hz;
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;

    /* Configure I2S for DAO */
    i2s_get_default_config(I2S_DAO, &i2s_config);
    i2s_config.tx_fifo_threshold = 2;                       /* Set TX FIFO threshold */
    i2s_init(I2S_DAO, &i2s_config);

    /* Configure transfer parameters for DAO */
    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = sample_rate;                     /* Set sampling rate */
    transfer.audio_depth = audio_depth;                     /* Set bit depth */
    transfer.channel_num_per_frame = channel_num;           /* Set number of channels */

    /* Get and verify master clock frequency */
    i2s_mclk_hz = clock_get_frequency(I2S_DAO_CLK_NAME);
    if (I2S_DAO_MCLK_FREQ_IN_HZ != i2s_mclk_hz) {
        printf("MCLK error for I2S_DAO!\n");
    }

    /* Configure I2S TX for DAO */
    if (status_success != i2s_config_tx(I2S_DAO, i2s_mclk_hz, &transfer)) {
        printf("I2S config failed for I2S_DAO\n");
        while (1) {
            /* Configuration error */
        }
    }

    /* Initialize and start DAO */
    dao_get_default_config(HPM_DAO, &dao_config);
    dao_init(HPM_DAO, &dao_config);

    i2s_start(I2S_DAO);
    dao_start(HPM_DAO);
}

/*
 * Main function
 * Initializes and starts I2S master mode operation
 */
int main(void)
{
    /* Initialize board and clocks */
    board_init();
    board_config_i2s_clock(I2S_MASTER, 48000);
    board_init_dao_clock();

    /* Initialize I2S and DAO pins */
    board_init_i2s_pins(I2S_MASTER);
    board_init_dao_pins();

    printf("I2S Master example\n");

    /* Configure I2S master and DAO */
    i2s_master_config();
    i2s_dao_config(I2S_SAMPLE_RATE_HZ, I2S_SAMPLE_BITDEPTH, I2S_SAMPLE_CHANNEL);

    /* Main loop - continuous DMA transfer */
    while (1) {
        dma_transfer_done = false;
        dma_transfer_config(DMA_TRANSFER_SIZE);
        while (dma_transfer_done == false) {
            /* Wait for DMA transfer completion */
        }
    }

    return 0;
}
