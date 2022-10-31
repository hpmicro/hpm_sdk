/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_SMC_H
#define HPM_SMC_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup mcl_smc_interface HPMicro MCL SMC APIs
 * @ingroup middleware_mcl_interfaces
 * @{
 *
 */

/**
 * @brief Sliding mode control function
 *
 * @param par @ref BLDC_CONTROL_SMC_PARA
 */
void bldc_smc_pos_cal(BLDC_CONTROL_SMC_PARA *par);

/**
 * @brief Calculation of sliding mode control static parameters
 *
 * @param par @ref BLDC_MOTOR_PARA
 */
void bldc_smc_const_cal(BLDC_MOTOR_PARA *par);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/**
 * @}
 *
 */

#endif