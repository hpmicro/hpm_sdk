/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef BLDC_FOC_CFG_H
#define BLDC_FOC_CFG_H

#define MOTOR0_BLDCPWM              BOARD_BLDCPWM

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name);
void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

