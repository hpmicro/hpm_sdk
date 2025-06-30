/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef I2S_MASTER_H
#define I2S_MASTER_H

#include "hpm_i2s_common.h"

/* ISR Priority */
#define ISR_PRIORITY_LEVEL (1U)

/* DMA */
#define APP_DMA           BOARD_APP_XDMA
#define DMA_TRANSFER_SIZE (96U)
#define DMA_CHANNEL       (2U)
#define DMA_MUX_CHANNEL   DMA_SOC_CHN_TO_DMAMUX_CHN(APP_DMA, DMA_CHANNEL)

/* I2S Master */
#define I2S_MASTER               BOARD_MASTER_I2S
#define I2S_MASTER_CLOCK_NAME    BOARD_MASTER_I2S_CLK_NAME
#define I2S_MASTER_RX_DATA_LINE  BOARD_MASTER_I2S_RX_DATA_LINE
#define I2S_MASTER_RX_DMAMUX_SRC BOARD_MASTER_I2S_RX_DMAMUX_SRC
#define I2S_MASTER_FIFO_THR      (2U)

/* I2S Master Transfer Format */
#define I2S_SAMPLE_RATE_HZ  (24000)
#define I2S_SAMPLE_CHANNEL  (2U)
#define I2S_SAMPLE_BITDEPTH i2s_audio_depth_16_bits
#define I2S_CHANNEL_LENGTH  i2s_channel_length_32_bits

/* DAO */
#define I2S_DAO                 DAO_I2S
#define I2S_DAO_CLK_NAME        clock_i2s1
#define I2S_DAO_DATA_LINE       (0U)
#define I2S_DAO_MCLK_FREQ_IN_HZ (24576000UL)

#endif /* I2S_MASTER_H */
