/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_TFA_DRV_H
#define HPM_TFA_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inversion value calculation.
 * @param[in]       src   input value
 *
 * @return Inversion value
 *
 * if src =0 , dst = inf
 */
float hpm_tfa_inv_f32(float src);

/**
 * @brief Square Root Calculations.
 * @param[in]       src   input value
 *
 * @return Sqrt value
 *
 * if src < 0, dst = nan
 */
float hpm_tfa_sqrt_f32(float src);

/**
 * @brief Sine value calculation.
 * @param[in]       src   input value
 *
 * @return Sin value
 *
 */
float hpm_tfa_sin_f32(float src);

/**
 * @brief Cosine value calculation.
 * @param[in]       src   input value
 *
 * @return cos value
 *
 */
float hpm_tfa_cos_f32(float src);

/**
 * @brief Power 2 value calculation.
 * @param[in]       src   input value
 *
 * @return  power value
 *
 */
float hpm_tfa_power2_f32(float src);

/**
 * @brief Logarithmic function value calculation.
 * @param[in]       src   input value
 *
 * @return log value
 *
 * if src < 0, dst = nan
 * if src = 0, dst = -inf
 */
float hpm_tfa_log2_f32(float src);

/**
 * @brief ATAN value calculation.
 * @param[in]       src   input value
 *
 * @return Atan value
 *
 * if src < -1, dst = -0.125
 * if src > 1, dst = 0.125
 */
float hpm_tfa_atan_f32(float src);

/**
 * @brief INVSQRT value calculation.
 * @param[in]       src   input value
 *
 * @return Invsqrt value
 *
 * if src < 0, dst = nan
 * if src = 0, dst = inf
 */
float hpm_tfa_invsqrt_f32(float src);

#ifdef __cplusplus
}
#endif

#endif /* HPM_TFA_DRV_H */
