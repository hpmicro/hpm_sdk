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

hpm_stat_t qeo_gen_abz_signal(void);
void qeo_gen_pwm_output(void);

void pwm_config_output(void);
void pwm_config_central_aligned_waveform(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_MOTO_H */
