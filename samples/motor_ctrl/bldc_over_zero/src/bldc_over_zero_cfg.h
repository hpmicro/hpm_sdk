/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef BLDC_OVER_ZERO_CFG_H
#define BLDC_OVER_ZERO_CFG_H

#define PWM_FREQUENCY               (20000) /*PWM HZ*/

/**
 * @brief PI Control define
 *
 */

#define PI_PWM_RANGE           1000 /* PI output will from PI_PWM_RANGE to -PI_PWM_RANGE */
#define PI_PWM_OUT_MAX        (PWM_RELOAD * 0.9)
#define PI_PWM_OUT_MIN        (PWM_RELOAD * 0.1)
#define PI_P_VAL                (124) /* PI p Val */
#define PI_I_VAL                (0.15) /* PI i Val */

#define MOTOR0_BLDCPWM              BOARD_BLDCPWM
#define MOTOR0_POLE_PAIR            (2)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Turn on the pwm output of the pin
 *
 * @param[in] motor_index Motor number, user defined
 * @param[in] pin_name Pwm pins, user defined
 */
void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name);

/**
 * @brief Turn off the pwm output of the pin
 *
 * @param[in] motor_index Motor number, user defined
 * @param[in] pin_name Pwm pins, user defined
 */
void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

