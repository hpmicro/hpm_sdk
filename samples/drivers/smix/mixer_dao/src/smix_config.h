/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SMIX_CONFIG_H
#define SMIX_CONFIG_H


#define TEST_I2S            DAO_I2S
#define TEST_I2S_CLK_NAME   clock_i2s1
#define TEST_I2S_DATA_LINE  (0)

/* timer interrupt use to adjust volume */
#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME

/* smix */
#define SMIX HPM_SMIX
#define SMIX_DMA_CH_FOR_SOURCE  (0U)
#define SMIX_DMA_CH_FOR_DST     (2U)
#define SMIX_DMA_IRQ            IRQn_SMIX_DMA
#define SMIX_OUTPUT_SAMPLE_RATE (DAO_SOC_SAMPLE_RATE_IN_HZ)
#define SMIX_SOURCE_CH          (0)
#define SMIX_DST_CH             (0)

#endif /* SMIX_CONFIG_H */