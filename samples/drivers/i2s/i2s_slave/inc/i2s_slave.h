/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef I2S_SLAVE_H
#define I2S_SLAVE_H

#include "hpm_i2s_common.h"

/* ISR Priority */
#define ISR_PRIORITY_LEVEL (1U)

/* DMA */
#define APP_DMA           BOARD_APP_XDMA
#define DMA_TRANSFER_SIZE (96U)
#define DMA_CHANNEL       (2U)
#define DMA_MUX_CHANNEL   DMA_SOC_CHN_TO_DMAMUX_CHN(APP_DMA, DMA_CHANNEL)

/* I2S Slave */
#define I2S_SLAVE               BOARD_SLAVE_I2S
#define I2S_SLAVE_CLOCK_NAME    BOARD_SLAVE_I2S_CLK_NAME
#define I2S_SLAVE_TX_DATA_LINE  BOARD_SLAVE_I2S_TX_DATA_LINE
#define I2S_SLAVE_TX_DMAMUX_SRC BOARD_SLAVE_I2S_TX_DMAMUX_SRC
#define I2S_SLAVE_FIFO_THR      (6U)

/* I2S Slave Transfer Format */
#define I2S_SAMPLE_RATE_HZ  (24000U)
#define I2S_SAMPLE_CHANNEL  (2U)
#define I2S_SAMPLE_BITDEPTH i2s_audio_depth_16_bits

#endif /* I2S_SLAVE_H */
