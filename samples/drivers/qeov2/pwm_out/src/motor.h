/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_MOTOR_H
#define HPM_MOTOR_H


#ifdef __cplusplus
extern "C" {
#endif

void pwm_config_output(void);
void pwm_generate_central_aligned_waveform(void);

/* MTG generates trajectories of uniform motion */
void mtg_generate_trajectory(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_MOTOR_H */
