/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _AUDIO_V2_SPEAKER_H
#define _AUDIO_V2_SPEAKER_H


/*
 * Function Declaration
 */
void audio_v2_init(uint8_t busid, uint32_t reg_base);
void speaker_init_i2s_dao_codec(void);
void mic_init_i2s_pdm(void);
void i2s_enable_dma_irq_with_priority(int32_t priority);

#endif