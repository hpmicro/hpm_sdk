/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef BLDC_FOC_H
#define BLDC_FOC_H

#include "hpm_mcl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

void motor_function_init(void);

void motor_speed_loop_init(void);

void motor_speed_loop_set(int32_t target_speed);

void motor_postion_loop_init(void);

void motor_position_loop_set(int32_t target_position);

int32_t motor_get_actual_speed(void);

int32_t motor_get_actual_position(void);

void motor_enable(void);

void motor_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* BLDC_FOC_H */