/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _AUDIO_V1_MIC_H
#define _AUDIO_V1_MIC_H

/*
 * Function Declaration
 */
void audio_init(void);
void audio_test(void);
void i2s_enable_dma_irq_with_priority(int32_t priority);
void init_mic_i2s_pdm(void);

#endif