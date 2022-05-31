/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_BLDC_BLOCK_H
#define HPM_BLDC_BLOCK_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void bldc_block_ctrl(uint8_t motorindex, uint8_t dir ,uint8_t step);
uint8_t bldc_block_step_get(bldc_hall_phase_t phase,uint8_t hall_u,uint8_t hall_v,uint8_t hall_w);
float al_pi_ctrl_func(float *memory,float targetSpeed,float speed,float kp,float ki,float max);
/*call back func*/
void bldc_pwm_enable(uint8_t motor_index,uint8_t pin_name);
void bldc_pwm_disable(uint8_t motor_index,uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
