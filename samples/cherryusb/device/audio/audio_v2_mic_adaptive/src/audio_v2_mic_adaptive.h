/*
 * Copyright (c) 2022-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _AUDIO_V2_MIC_H
#define _AUDIO_V2_MIC_H

/*
 * Definition
 */
#if defined(USING_CODEC) && USING_CODEC
    #ifndef BOARD_CODEC_I2C_BASE
    #define CODEC_I2C BOARD_APP_I2C_BASE
    #else
    #define CODEC_I2C BOARD_CODEC_I2C_BASE
    #endif
    #define TARGET_I2S               BOARD_APP_I2S_BASE
    #define TARGET_I2S_CLK_NAME      BOARD_APP_I2S_CLK_NAME
    #define TARGET_I2S_RX_DATA_LINE  BOARD_APP_I2S_RX_DATA_LINE
    #define TARGET_I2S_RX_DMAMUX_SRC BOARD_APP_I2S_RX_DMA_REQ
#else
    #define TARGET_I2S               BOARD_MIC_I2S
    #define TARGET_I2S_CLK_NAME      BOARD_MIC_I2S_CLK_NAME
    #define TARGET_I2S_RX_DATA_LINE  BOARD_MIC_I2S_DATA_LINE
    #define TARGET_I2S_RX_DMAMUX_SRC BOARD_MIC_I2S_RX_DMAMUX_SRC
#endif


/*
 * Function Declaration
 */
void audio_v2_init(uint8_t busid, uint32_t reg_base);
void audio_v2_task(uint8_t busid);
void i2s_enable_dma_irq_with_priority(int32_t priority);
void init_mic_i2s_pdm_codec(void);

#endif