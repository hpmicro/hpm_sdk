/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_MOTO_H
#define HPM_MOTO_H


#ifdef __cplusplus
extern "C" {
#endif

void pwm_config_output(void);
void pwm_config_central_aligned_waveform(void);
void mmc_open_loop_pred(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_MOTO_H */
