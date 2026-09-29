/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _AUDIO_V2_HEADSET_H
#define _AUDIO_V2_HEADSET_H

/*
 * Definition
 */
#ifndef BOARD_CODEC_I2C_BASE
#define CODEC_I2C BOARD_APP_I2C_BASE
#else
#define CODEC_I2C BOARD_CODEC_I2C_BASE
#endif

#define CODEC_I2S               BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME      BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_TX_DATA_LINE  BOARD_APP_I2S_TX_DATA_LINE
#define CODEC_I2S_RX_DATA_LINE  BOARD_APP_I2S_RX_DATA_LINE
#define CODEC_I2S_TX_DMAMUX_SRC BOARD_APP_I2S_TX_DMA_REQ
#define CODEC_I2S_RX_DMAMUX_SRC BOARD_APP_I2S_RX_DMA_REQ


/*
 * Function Declaration
 */
void audio_v2_init(uint8_t busid, uint32_t reg_base);
void audio_v2_task(uint8_t busid);
void audio_v2_init_i2s_codec(void);

#endif
