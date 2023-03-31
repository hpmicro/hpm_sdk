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
#define DMA_TRANSFER_SIZE     (96U)
#define DMA_MUX_CHANNEL       (2U)
#define DMA_MUX_SRC           HPM_DMA_SRC_I2S0_RX

/* I2S Master */
#define I2S_MASTER            HPM_I2S0
#define I2S_MASTER_CLOCK_NAME clock_i2s0
#define I2S_MASTER_RX_LINE    I2S_DATA_LINE_1
#define I2S_MASTER_FIFO_THR   (2U)

/* I2S Master Transfer Format */
#define I2S_SAMPLE_RATE_HZ    (24000)
#define I2S_SAMPLE_CHANNEL    (2U)
#define I2S_SAMPLE_BITDEPTH   i2s_audio_depth_16_bits
#define I2S_CHANNEL_LENGTH    i2s_channel_length_32_bits

/* DAO */
#define I2S_DAO                 DAO_I2S
#define I2S_DAO_CLK_NAME        clock_i2s1
#define I2S_DAO_DATA_LINE       (0U)
#define I2S_DAO_MCLK_FREQ_IN_HZ (24576000UL)

#endif /* I2S_MASTER_H */