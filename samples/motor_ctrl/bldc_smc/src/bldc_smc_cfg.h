/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef BLDC_SMC_CFG_H
#define BLDC_SMC_CFG_H

#define MOTOR0_ADC_CH_NUM                  (3U)
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*call back func*/
void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name);
void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

