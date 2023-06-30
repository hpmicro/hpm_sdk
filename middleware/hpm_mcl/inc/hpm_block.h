/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_BLOCK_H
#define HPM_BLOCK_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup mcl_block_interface HPMicro MCL BLOCK APIs
 * @ingroup middleware_mcl_interfaces
 * @{
 *
 */

/**
 * @brief Output pwm according to the number of motor steps
 *
 * @param[in] motorindex number of motor @ref BLDC_MOTOR0_INDEX ... BLDC_MOTOR3_INDEX
 * @param[in] dir Motor operation direction @ref BLDC_MOTOR_DIR_FORWARD or @ref BLDC_MOTOR_DIR_REVERSE
 * @param[in] step Motor steps, 60 degrees per step
 */
void hpm_mcl_bldc_block_ctrl(uint8_t motorindex, uint8_t dir, uint8_t step);

/**
 * @brief Get step count based on Hall sensor status
 *
 * @param[in] phase Hall Displacement @ref bldc_hall_phase_t
 * @param[in] hall_u u-phase Hall level state, 0-low 1-high
 * @param[in] hall_v v-phase Hall level state, 0-low 1-high
 * @param[in] hall_w w-phase Hall level state, 0-low 1-high
 * @return uint8_t step
 */
uint8_t hpm_mcl_bldc_block_step_get(bldc_hall_phase_t phase, uint8_t hall_u, uint8_t hall_v, uint8_t hall_w);

/**
 * @brief pi loop function
 *
 * @param[in] memory Store points data
 * @param[in] targetspeed Target speed
 * @param[in] speed Real speed
 * @param[in] kp Scale factor
 * @param[in] ki Integral factor
 * @param[in] max Output maximum and integration maximum
 * @return float Data after pi adjustment
 */
float hpm_mcl_al_pi_ctrl_func(float *memory, float targetspeed, float speed, float kp, float ki, float max);

/**
 * @brief Enables the pwm output of the given pin
 *
 * @param[in] motor_index number of motor @ref BLDC_MOTOR0_INDEX ... BLDC_MOTOR3_INDEX
 * @param[in] pin_name pin names @ref BLDC_PWM_PIN_UH ... BLDC_PWM_PIN_WL
 */
void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name);

/**
 * @brief Disable the pwm output of the given pin
 *
 * @param[in] motor_index number of motor @ref BLDC_MOTOR0_INDEX ... BLDC_MOTOR3_INDEX
 * @param[in] pin_name pin names @ref BLDC_PWM_PIN_UH ... BLDC_PWM_PIN_WL
 */
void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name);

/**
 * @}
 *
 */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
