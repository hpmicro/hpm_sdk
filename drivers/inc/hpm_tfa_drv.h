/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_TFA_DRV_H
#define HPM_TFA_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__IAR_SYSTEMS_ICC__)
#ifndef M_PI
    #define M_PI 3.1415926f
#endif
const float CONST_2PI      = 6.283185307f;
const float RAD_TO_DEG_DBL = 57.29578f;
const float INT_TO_DEG_DBL = 5965232.0f;
#endif

typedef struct {
    float angle;
    float mod;
} cordic_res_t;

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

#if defined(TFA_SOC_ATANPU2_CORDIC_SUPPORT) && (TFA_SOC_ATANPU2_CORDIC_SUPPORT == 1)
/**
 * @brief ATANPU2 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return ATANPU2 angle_offset value
 *
 */
float hpm_tfa_atanpu2_f32(float x_input, float y_input);

/**
 * @brief CORDIC f32 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return[out] CORDIC angle_offset value
 * @return[out] CORDIC mod value
 *
 */
cordic_res_t hpm_tfa_cordic_f32(int32_t x_input, int32_t y_input);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HPM_TFA_DRV_H */
