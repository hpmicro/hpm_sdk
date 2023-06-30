/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_FOC_H
#define HPM_FOC_H
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#include "hpm_motor_math.h"

/**
 * @addtogroup mcl_foc_interface HPMicro MCL FOC APIs
 * @ingroup middleware_mcl_interfaces
 * @{
 *
 */

/**
 * @brief Calculation of speed by angular difference of sampling.
 *
 * @param[inout] par Speed parameters @ref BLDC_CONTRL_SPD_PARA
 */
void hpm_mcl_bldc_foc_al_speed(BLDC_CONTRL_SPD_PARA  *par);

/**
 * @brief Update output pwm according to duty cycle, provided by the user
 *
 * @param[inout] par @ref BLDC_CONTROL_PWMOUT_PARA
 */
void hpm_mcl_bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par);

/**
 * @brief Reconfiguring three-phase currents
 *
 * @param[inout] par @ref BLDC_CONTROL_CURRENT_PARA
 */
void hpm_mcl_bldc_foc_current_cal(BLDC_CONTROL_CURRENT_PARA *par);

/**
 * @brief pi control function
 *
 * @param[inout] par @ref BLDC_CONTRL_PID_PARA
 */
void hpm_mcl_bldc_foc_pi_contrl(BLDC_CONTRL_PID_PARA *par);

/**
 * @brief Clark Transformation
 *
 * @param[in] currentu  U-phase current
 * @param[in] currentv  V-phase current
 * @param[in] currentw  W-phase current
 * @param[out] currentalpha alpha-axis current
 * @param[out] currentbeta beta-axis current
 */
void hpm_mcl_bldc_foc_clarke(HPM_MOTOR_MATH_TYPE currentu, HPM_MOTOR_MATH_TYPE currentv, HPM_MOTOR_MATH_TYPE currentw,
             HPM_MOTOR_MATH_TYPE *currentalpha, HPM_MOTOR_MATH_TYPE *currentbeta);

/**
 * @brief transform
 *
 * @brief Park transform
 *
 * @param[in] currentalpha alpha-axis current
 * @param[in] currentbeta beta-axis current
 * @param[out] currentd d-axis current
 * @param[out] currentq q-axis current
 * @param[in] sin_angle sin(theta)
 * @param[in] cos_angle cos(theta)
 */
void hpm_mcl_bldc_foc_park(HPM_MOTOR_MATH_TYPE currentalpha, HPM_MOTOR_MATH_TYPE currentbeta,
                   HPM_MOTOR_MATH_TYPE *currentd, HPM_MOTOR_MATH_TYPE *currentq,
                   HPM_MOTOR_MATH_TYPE sin_angle, HPM_MOTOR_MATH_TYPE cos_angle);

/**
 * @brief Motor rotor angle, electrical angle
 *
 * @param[in] ud d-axis voltage
 * @param[in] uq q-axis voltage
 * @param[out] ualpha u-alpha-axis voltage
 * @param[out] ubeta u-beta-axis voltage
 * @param[in] sin_angle sin(theta)
 * @param[in] cos_angle cos(theta)
 */
void hpm_mcl_bldc_foc_inv_park(HPM_MOTOR_MATH_TYPE ud, HPM_MOTOR_MATH_TYPE uq,
                    HPM_MOTOR_MATH_TYPE *ualpha, HPM_MOTOR_MATH_TYPE *ubeta,
                    HPM_MOTOR_MATH_TYPE sin_angle, HPM_MOTOR_MATH_TYPE cos_angle);

/**
 * @brief svpwm function
 *
 * @param par @ref BLDC_CONTROL_PWM_PARA
 */
void hpm_mcl_bldc_foc_svpwm(BLDC_CONTROL_PWM_PARA *par);

/**
 * @brief Speed Control PI Loop
 *
 * @param[in] memory Internal Data
 * @param[in] targetspeed target speed
 * @param[in] curspeed Current speed
 * @param[in] kp Scale factor
 * @param[in] ki Integral factor
 * @param[in] max Output maximum and integration maximum
 * @param[out] output Output after pi control
 */
void hpm_mcl_bldc_foc_speed_ctrl(float *memory, float targetspeed, float curspeed,
                            float kp, float ki, int16_t max, int16_t *output);

/**
 * @brief Position Control P Loop
 *
 * @param targetpos Target position
 * @param curpos current position
 * @param kp Scale factor
 * @param max Output maximum
 * @param output Output after p control
 */
void hpm_mcl_bldc_foc_position_ctrl(float targetpos, float curpos,
                            float kp, float max, float *output);

/**
 * @brief dq-axis voltage conversion to pwm output
 *
 * @param par @ref BLDC_CONTROL_FOC_PARA
 */
void hpm_mcl_bldc_foc_ctrl_dq_to_pwm(BLDC_CONTROL_FOC_PARA *par);

/**
 * @}
 *
 */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
