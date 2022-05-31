/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_BLDC_FOC_FUNC_H
#define HPM_BLDC_FOC_FUNC_H
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#include "hpm_motor_math.h"

void bldc_foc_al_speed(BLDC_CONTRL_SPD_PARA  *par);
void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par);

void bldc_foc_current_cal(BLDC_CONTROL_CURRENT_PARA *par);

void bldc_foc_pi_contrl(BLDC_CONTRL_PID_PARA *par);

void bldc_foc_clarke(HPM_MOTOR_MATH_TYPE currentU,HPM_MOTOR_MATH_TYPE currentV,HPM_MOTOR_MATH_TYPE currentW,
             HPM_MOTOR_MATH_TYPE * currentAlpha, HPM_MOTOR_MATH_TYPE * currentBeta);

void bldc_foc_park( HPM_MOTOR_MATH_TYPE currentAlpha, HPM_MOTOR_MATH_TYPE currentBeta, HPM_MOTOR_MATH_TYPE angle,
                   HPM_MOTOR_MATH_TYPE * currentd, HPM_MOTOR_MATH_TYPE * currentq);
void bldc_foc_inv_park(HPM_MOTOR_MATH_TYPE ud,HPM_MOTOR_MATH_TYPE uq,HPM_MOTOR_MATH_TYPE *ualpha, HPM_MOTOR_MATH_TYPE *ubeta, HPM_MOTOR_MATH_TYPE angle);

void bldc_foc_svpwm(BLDC_CONTROL_PWM_PARA *par);

void bldc_foc_speed_ctrl(float *memory, float targetspeed, float curspeed, 
                            float kp, float ki, int16_t max, int16_t *output);

void bldc_foc_position_ctrl(float targetpos, float curpos, 
                            float kp, float max, float *output);

void bldc_foc_ctrl_dq_to_pwm(BLDC_CONTROL_FOC_PARA *par);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
