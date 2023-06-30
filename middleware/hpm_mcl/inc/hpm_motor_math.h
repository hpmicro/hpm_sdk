/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_BLDC_MATH_H
#define HPM_BLDC_MATH_H

/**
 * @addtogroup mcl_math_interface HPMicro MCL MATH APIs
 * @ingroup middleware_mcl_interfaces
 * @{
 *
 */

/**
 * @brief Motor library using mathematical format
 *
 */
#define HPM_MOTOR_MATH_MOD_Q_SW               (1) /**< Qmath software */
#define HPM_MOTOR_MATH_MOD_Q_HW               (2) /**< Qmath hardware */
#define HPM_MOTOR_MATH_MOD_DSP_FP             (3) /**< dsp hardware floating point unit */
#define HPM_MOTOR_MATH_MOD_Q_ALL              (4) /**< Qmath soft ware and hardware */
#define HPM_MOTOR_MATH_MOD_FP                 (5) /**< hardware floating point unit */

/**
 * @brief Q-format
 *
 */
#define HPM_QMATH_MOD_Q15               (15)
#define HPM_QMATH_MOD_Q31               (31)

/**
 * @brief User-defined profiles
 *
 */
#ifdef CONFIG_MATH_HAS_EXTRA_CONFIG
#include CONFIG_MATH_HAS_EXTRA_CONFIG
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HPM_MOTOR_MATH_MOD
#define HPM_MOTOR_MATH_MOD                    HPM_MOTOR_MATH_MOD_FP
#endif

#ifndef HPM_QMATH_N
#define HPM_QMATH_N                     HPM_QMATH_MOD_Q31
#endif

#ifndef HPM_SOFT_Q_N
#define HPM_SOFT_Q_N                                (15)
#endif

#include <stdint.h>
#if HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_Q_SW
    typedef     int32_t     HPM_MOTOR_MATH_TYPE;
#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_Q_HW
    #ifndef HPM_QMATH_N
    #define HPM_QMATH_N                 HPM_QMATH_MOD_Q31
    #endif
    #if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    typedef     int16_t     HPM_MOTOR_MATH_TYPE;/*Q15格式*/
    #elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    typedef     int32_t     HPM_MOTOR_MATH_TYPE;/*Q31格式*/
    #endif
#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_DSP_FP
    typedef    float                                    HPM_MOTOR_MATH_TYPE;
#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_FP
    typedef    float                                    HPM_MOTOR_MATH_TYPE;
#endif

/**
 * @brief Floating point to Q format conversion
 *
 * @param[in] from_f32 Floating point
 * @return @ref HPM_MOTOR_MATH_TYPE
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_f32_q(float from_f32);

/**
 * @brief Q format to floating point conversion
 *
 * @param[in] from_mdf Q format
 * @return float
 */
float hpm_dsp_hw_q_f32(HPM_MOTOR_MATH_TYPE from_mdf);

/**
 * @brief Q-format multiplication
 *
 * @param[in] x data
 * @param[in] y data
 * @return x*y
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_mul_q(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y);

/**
 * @brief Q-format division
 *
 * @param[in] x data
 * @param[in] y data
 * @return x/y
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_div_q(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y);

/**
 * @brief Q-format atan
 *
 * @param[in] x data
 * @return atan(x)
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan_q(HPM_MOTOR_MATH_TYPE x);

/**
 * @brief Q-format atan2
 *
 * @param[in] x data
 * @param[in] y data
 * @return atan(x, y)
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan2_q(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y);

/**
 * @brief Q-format absolute
 *
 * @param[in] x data
 * @return |data|
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_abs_q(HPM_MOTOR_MATH_TYPE x);

/**
 * @brief Floating-point multiplication
 *
 * @param[in] x data
 * @param[in] y data
 * @return x*y
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_mul_fp(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y);

/**
 * @brief Floating-point division
 *
 * @param[in] x data
 * @param[in] y data
 * @return x/y
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_div_fp(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y);

/**
 * @brief Floating-point atan
 *
 * @param[in] x data
 * @return atan(x)
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan_fp(HPM_MOTOR_MATH_TYPE x);

/**
 * @brief Floating-point atan2
 *
 * @param[in] x data
 * @param[in] y data
 * @return atan2(x,y)
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan2_fp(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y);

/**
 * @brief Floating-point absolute
 *
 * @param[in] x data
 * @return |x|
 */
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_abs_fp(HPM_MOTOR_MATH_TYPE x);

static inline HPM_MOTOR_MATH_TYPE soft_conv_float_to_q31(float x)
{
    HPM_MOTOR_MATH_TYPE q31;
    int64_t q64;
    double x1 = x;
    x1 *= (1 << HPM_SOFT_Q_N);
    x1 += (x < 0.0f ? -0.5f : 0.5f);
    q64 = (HPM_MOTOR_MATH_TYPE)x1;
    if (q64 >= INT32_MAX) {
        q64 = INT32_MAX;
    } else if (q64 <= INT32_MIN) {
        q64 = INT32_MIN;
    } else {
        q31 = (HPM_MOTOR_MATH_TYPE)q64;
    }
    return q31;
}
static inline float soft_conv_q31_to_float(HPM_MOTOR_MATH_TYPE x)
{
    float val;
    val = (double)x/(1<<HPM_SOFT_Q_N);
    return val;
}

#if HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_Q_SW
    /*软件*/
#if (HPM_QMATH_N != HPM_QMATH_MOD_Q31)
    #error "SOFT Q_MATH only support q31"
#endif
    #include <math.h>
    #define HPM_MOTOR_MATH_FL_MDF(from_f32)                   soft_conv_float_to_q31(from_f32)
    #define HPM_MOTOR_MATH_MDF_FL(from_mdf)                   soft_conv_q31_to_float(from_mdf)
    #define HPM_MOTOR_MATH_MUL(x, y)                           ((((int64_t)x)*y)>>HPM_SOFT_Q_N)
    #define HPM_MOTOR_MATH_DIV(x, y)                           ((((int64_t)x)<<HPM_SOFT_Q_N)/y)
    #define HPM_MOTOR_MATH_ATAN2(x, y)                         atan2f(x, y)
    #define HPM_MOTOR_MATH_FABS(x)                            abs(x)
#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_Q_HW
    /*Qmath*/
    #define HPM_MOTOR_MATH_FL_MDF(from_f32)                  hpm_dsp_hw_f32_q(from_f32)
    #define HPM_MOTOR_MATH_MDF_FL(from_mdf)                  hpm_dsp_hw_q_f32(from_mdf)
    #define HPM_MOTOR_MATH_MUL(x, y)                          hpm_dsp_hw_mul_q(x, y)
    #define HPM_MOTOR_MATH_DIV(x, y)                          hpm_dsp_hw_div_q(x, y)
    #define HPM_MOTOR_MATH_ATAN2(x, y)                        hpm_dsp_hw_atan2_q(x, y)
    #define HPM_MOTOR_MATH_ATAN(x)                           hpm_dsp_hw_atan_q(x)
    #define HPM_MOTOR_MATH_FABS(x)                           hpm_dsp_hw_abs_q(x)

#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_DSP_FP
    #define HPM_MOTOR_MATH_FL_MDF(from_f32)                   (from_f32)
    #define HPM_MOTOR_MATH_MDF_FL(from_mdf)                   (from_mdf)
    #define HPM_MOTOR_MATH_MUL(x, y)                           hpm_dsp_hw_mul_fp(x, y)
    #define HPM_MOTOR_MATH_DIV(x, y)                           hpm_dsp_hw_div_fp(x, y)
    #define HPM_MOTOR_MATH_ATAN2(x, y)                         hpm_dsp_hw_atan2_fp(x, y)
    #define HPM_MOTOR_MATH_ATAN(x)                            hpm_dsp_hw_atan_fp(x)
    #define HPM_MOTOR_MATH_FABS(x)                            hpm_dsp_hw_abs_fp(x)
#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_FP
    #include <math.h>
    #define HPM_MOTOR_MATH_FL_MDF(from_f32)                   (from_f32)
    #define HPM_MOTOR_MATH_MDF_FL(from_mdf)                   (from_mdf)
    #define HPM_MOTOR_MATH_MUL(x, y)                           (x*y)
    #define HPM_MOTOR_MATH_DIV(x, y)                           (x/y)
    #define HPM_MOTOR_MATH_ATAN2(x, y)                         atan2(x, y)
    #define HPM_MOTOR_MATH_ATAN(x)                            atan(x)
    #define HPM_MOTOR_MATH_FABS(x)                            abs(x)
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 *
 */

#endif
