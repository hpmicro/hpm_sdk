/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PWM_H
#define HPM_PWM_H


#ifdef __cplusplus
extern "C" {
#endif

void pwm_config_output(void);
void pwm_generate_central_aligned_waveform(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_PWM_H */
