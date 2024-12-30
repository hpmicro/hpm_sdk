/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _AUDIO_CODEC_COMMON_H
#define _AUDIO_CODEC_COMMON_H

#if defined(USING_CODEC) && USING_CODEC
    #ifndef BOARD_CODEC_I2C_BASE
    #define CODEC_I2C BOARD_APP_I2C_BASE
    #else
    #define CODEC_I2C BOARD_CODEC_I2C_BASE
    #endif
    #define TARGET_I2S BOARD_APP_I2S_BASE
    #define TARGET_I2S_CLK_NAME BOARD_APP_I2S_CLK_NAME
    #define TARGET_I2S_TX_DATA_LINE BOARD_APP_I2S_TX_DATA_LINE
    #define TARGET_I2S_TX_DMAMUX_SRC BOARD_APP_I2S_TX_DMA_REQ
#elif defined(USING_DAO) && USING_DAO
    #define TARGET_I2S BOARD_SPEAKER_I2S
    #define TARGET_I2S_CLK_NAME BOARD_SPEAKER_I2S_CLK_NAME
    #define TARGET_I2S_TX_DATA_LINE BOARD_SPEAKER_I2S_DATA_LINE
    #define TARGET_I2S_TX_DMAMUX_SRC BOARD_SPEAKER_I2S_TX_DMAMUX_SRC
#else
    #error define USING_CODEC or USING_DAO
#endif

#ifndef TARGET_I2S_TX_DMA_CH
#define TARGET_I2S_TX_DMA_CH 2
#endif

#define TARGET_I2S_TX_DMAMUX_CH DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH)

#ifndef TARGET_I2S_TX_DMA_IRQ_PRIORITY
#define TARGET_I2S_TX_DMA_IRQ_PRIORITY 7
#endif

/*
 * Function Declaration
 */
void init_codec(void);
void init_dao(void);
hpm_stat_t init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
void i2s_dma_start_transfer(uint32_t addr, uint32_t size);

#endif
