/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_motor_math.h"
#if (HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_Q_HW) || (HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_DSP_FP)
#include "hpm_math.h"
#endif

#if HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_Q_HW
HPM_MOTOR_MATH_TYPE hpm_dsp_hw_f32_q(float from_f32)
{
     HPM_MOTOR_MATH_TYPE m_val;
     float from_f321 = from_f32;
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
     hpm_dsp_convert_f32_q15(&from_f321, &m_val, 1);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    hpm_dsp_convert_f32_q31(&from_f321, &m_val, 1);
#endif
    return m_val;
}

float hpm_dsp_hw_q_f32(HPM_MOTOR_MATH_TYPE from_mdf)
{
    float m_val;
    HPM_MOTOR_MATH_TYPE from_q1 = from_mdf;
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    hpm_dsp_convert_q15_f32(&from_q1, &m_val, 1);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    hpm_dsp_convert_q31_f32(&from_q1, &m_val, 1);
#endif
    return m_val;
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_mul_q(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y)
{
    HPM_MOTOR_MATH_TYPE m_val;
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    hpm_dsp_mul_q15(&x, &y, &m_val, 1);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    hpm_dsp_mul_q31(&x, &y, &m_val, 1);
#endif
    return m_val;
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_div_q(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y)
{
    HPM_MOTOR_MATH_TYPE m_val;
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    m_val = ((((int)x)<<15)/y);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    hpm_dsp_mul_q31(&x, &y, &m_val, 1);
#endif
    return m_val;
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan_q(HPM_MOTOR_MATH_TYPE x)
{
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    return hpm_dsp_atan_q15(x);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    return hpm_dsp_atan_q31(x);
#endif
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan2_q(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y)
{
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    return hpm_dsp_atan2_q15(x, y);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    return hpm_dsp_atan2_q31(x, y);
#endif
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_abs_q(HPM_MOTOR_MATH_TYPE x)
{
    HPM_MOTOR_MATH_TYPE m_val;
#if (HPM_QMATH_N == HPM_QMATH_MOD_Q15)
    hpm_dsp_abs_q15(&x, &m_val, 1);
#elif (HPM_QMATH_N == HPM_QMATH_MOD_Q31)
    hpm_dsp_abs_q31(&x, &m_val, 1);
#endif
    return m_val;
}
#elif HPM_MOTOR_MATH_MOD == HPM_MOTOR_MATH_MOD_DSP_FP

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_mul_fp(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y)
{
    HPM_MOTOR_MATH_TYPE m_val;
    hpm_dsp_mul_f32(&x, &y, &m_val, 1);
    return m_val;
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_div_fp(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y)
{
    HPM_MOTOR_MATH_TYPE m_val;
    hpm_dsp_div_f32(&x, &y, &m_val, 1);
    return m_val;
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan_fp(HPM_MOTOR_MATH_TYPE x)
{
    return hpm_dsp_atan_f32(x);
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_atan2_fp(HPM_MOTOR_MATH_TYPE x, HPM_MOTOR_MATH_TYPE y)
{
    return hpm_dsp_atan2_f32(x, y);
}

HPM_MOTOR_MATH_TYPE hpm_dsp_hw_abs_fp(HPM_MOTOR_MATH_TYPE x)
{
    HPM_MOTOR_MATH_TYPE m_val;
    hpm_dsp_abs_f32(&x, &m_val, 1);
    return m_val;
}

#endif

