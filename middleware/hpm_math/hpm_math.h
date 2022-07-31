/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_MATH_H__
#define __HPM_MATH_H__


#define HPM_DSP_HW_NDS32 nds32_dsp_core//andes hardware dsp

#ifdef CONFIG_HPM_MATH_HAS_EXTRA_CONFIG
#include CONFIG_HPM_MATH_HAS_EXTRA_CONFIG
#else

/*Enable Compute Cell Library*/
// #define HPM_EN_MATH_FFA_LIB
#define HPM_EN_MATH_DSP_LIB

#define HPM_MATH_STATISTICS 1
#define HPM_MATH_BASIC 1
#define HPM_MATH_COMPLEX 1
#define HPM_MATH_CONTROLLER 1
#define HPM_MATH_DISTANCE 1
#define HPM_MATH_FILTERING 1
#define HPM_MATH_MATRIX 1
#define HPM_MATH_SVM 1
#define HPM_MATH_TRANSFORM 1
#define HPM_MATH_UTILS 1

#define HPM_DSP_CORE HPM_DSP_HW_NDS32 //DSP core selection

#define HPM_MATH_PI (3.1415926535898)

/**
 * @brief HPM_MATH_SW_FFT_CHECKLIST Enabled to use table lookup to speed up the software fft,
 *  but will increase the code space,and only support sampling points 2^( 2-10).
 *
 * With this option turned off,
 * the software fft can support as many sample points as necessary with sufficient space
 *
 */
#define HPM_MATH_SW_FFT_CHECKLIST

#endif

#ifdef  __cplusplus
extern "C"
{
#endif

#ifdef HPM_MATH_STATISTICS

/**
 * @defgroup statistics Statistics Functions
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_statistics_math.h"

// Maximum
/**
 * @brief Maximum value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
static inline float32_t hpm_dsp_max_f32(const float32_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_max_f32(src, size, index);
#endif
}
static inline float32_t hpm_dsp_max_val_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_max_val_f32(src, size);
#endif
}
/**
 * @brief Maximum value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
static inline q15_t hpm_dsp_max_q15(const q15_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_max_q15(src, size, index);
#endif
}

/**
 * @brief Maximum value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
static inline q31_t hpm_dsp_max_q31(const q31_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_max_q31(src, size, index);
#endif
}

/**
 * @brief Maximum value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
static inline q7_t hpm_dsp_max_q7(const q7_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_max_q7(src, size, index);
#endif
}

/**
 * @brief Max value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @param[out]      *index  index of the maximum value.
 * @return max value.
 */
static inline uint8_t hpm_dsp_max_u8(const uint8_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_max_u8(src, size, index);
#endif
}

// Minimum
/**
 * @brief Minimum value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
static inline float32_t hpm_dsp_min_f32(const float32_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_min_f32(src, size, index);
#endif
}

/**
 * @brief Minimum value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
static inline q15_t hpm_dsp_min_q15(const q15_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_min_q15(src, size, index);
#endif
}

/**
 * @brief Minimum value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
static inline q31_t hpm_dsp_min_q31(const q31_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_min_q31(src, size, index);
#endif
}

/**
 * @brief Minimum value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
static inline q7_t hpm_dsp_min_q7(const q7_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_min_q7(src, size, index);
#endif
}

/**
 * @brief Minimum value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @param[out]      *index  index of the minimum value.
 * @return minimum value.
 */
static inline uint8_t hpm_dsp_min_u8(const uint8_t *src, uint32_t size, uint32_t *index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_min_u8(src, size, index);
#endif
}

// Mean
/**
 * @brief Mean value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 */
static inline float32_t hpm_dsp_mean_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mean_f32(src, size);
#endif
}

/**
 * @brief Mean value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is accumulated in a 32-bit accumulator in 17.15
 * format and then truncated to yield a result of 1.15 format.
 */
static inline q15_t hpm_dsp_mean_q15(const q15_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mean_q15(src, size);
#endif
}

/**
 * @brief Mean value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is accumulated in a 64-bit accumulator in 33.31
 * format and then truncated to yield a result of 1.31 format.
 */
static inline q31_t hpm_dsp_mean_q31(const q31_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mean_q31(src, size);
#endif
}

/**
 * @brief Mean value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 *
 * <b>Function notes:</b>
 *
 * The 1.7 format input is accumulated in a 32-bit accumulator in 25.7
 * format and then truncated to yield a result of 1.7 format.
 */
static inline q7_t hpm_dsp_mean_q7(const q7_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mean_q7(src, size);
#endif
}

/**
 * @brief Mean value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return mean value.
 *
 * The 8-bit format input is accumulated in a 32-bit accumulator
 * and then truncated to yield a result of 8-bit format.
 */
static inline uint8_t hpm_dsp_mean_u8(const uint8_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mean_u8(src, size);
#endif
}

// Sun of the Squares
/**
 * @brief Sum of the squares of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 */
static inline float32_t hpm_dsp_pwr_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pwr_f32(src, size);
#endif
}

/**
 * @brief Sum of the squares of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the return result is in 34.30 format.
 */
static inline q63_t hpm_dsp_pwr_q15(const q15_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pwr_q15(src, size);
#endif
}

/**
 * @brief Sum of the squares of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format and this result
 * is truncated to 2.48 format by discarding the lower 14 bits. The 2.48
 * result is then added without saturation to a 64-bit accumulator in 16.48
 * format. Finally, the return result is in 16.48 format.
 */
static inline q63_t hpm_dsp_pwr_q31(const q31_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pwr_q31(src, size);
#endif
}

/**
 * @brief Sum of the squares of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 *
 * <b>Function notes:</b>
 *
 * The 1.7 format input is multiplied yields a 2.14 format, and then added
 * without saturation to a 32-bit accumulator in 18.14 format. Finally,
 * the return result is in 18.14 format.
 */
static inline q31_t hpm_dsp_pwr_q7(const q7_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pwr_q7(src, size);
#endif
}

// Root Mean Square
/**
 * @brief RMS of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 */
static inline float32_t hpm_dsp_rms_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rms_f32(src, size);
#endif
}

/**
 * @brief RMS of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
static inline q15_t hpm_dsp_rms_q15(const q15_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rms_q15(src, size);
#endif
}

/**
 * @brief RMS of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(size)</code> bits, Finally, the 2.62 accumulator is right
 * shifted  by 31 bits to yield a 1.31 format value.
 */
static inline q31_t hpm_dsp_rms_q31(const q31_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rms_q31(src, size);
#endif
}

// Standard deviation
/**
 * @brief Standard deviation of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 */
static inline float32_t hpm_dsp_std_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_std_f32(src, size);
#endif
}

/**
 * @brief Standard deviation of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
static inline q15_t hpm_dsp_std_q15(const q15_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_std_q15(src, size);
#endif
}

/**
 * @brief Standard deviation of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(size)</code> bits, Finally, the 2.62 accumulator is right
 * shifted  by 31 bits to yield a 1.31 format value.
 */
static inline q31_t hpm_dsp_std_q31(const q31_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_std_q31(src, size);
#endif
}

/**
 * @brief Standard deviation of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return Standard deviation value.
 *
 * <b>Function notes:</b>
 * The 8-bit format input is multiplied yields a 16-bit format, and then added
 * saturation to a 32-bit accumulator in 16.16 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 1
 * bits, and then saturated to yield a result in 1.15 format.
 */
static inline q15_t hpm_dsp_std_u8(const uint8_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_std_u8(src, size);
#endif
}

// Variance
/**
 * @brief Variance of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 */
static inline float32_t hpm_dsp_var_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_var_f32(src, size);
#endif
}

/**
 * @brief Variance of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
static inline q31_t hpm_dsp_var_q15(const q15_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_var_q15(src, size);
#endif
}

/**
 * @brief Variance of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(size)</code> bits, Finally, the 2.62 accumulator is right
 * shifted  by 31 bits to yield a 1.31 format value.
 */
static inline q63_t hpm_dsp_var_q31(const q31_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_var_q31(src, size);
#endif
}

/**
 * @brief Entropy of the floating-potint vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return Entropy value.
 *
 * E = -sum (P .* log2 (P))
 */
static inline float32_t hpm_dsp_entropy_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_entropy_f32(src, size);
#endif
}


/**
 * @brief Relative Entropy of the floating-potint vector.
 * @param[in]       *src1    points to the first input vector.
 * @param[in]       *src2    points to the second input vector.
 * @param[in]       size    size of the vectors.
 * @return Relative Entropy value.
 *
 * Relative Entropy also called Kullback–Leibler divergence:
 * D(A || B) = A * ln(A / B);
 *
 */
static inline float32_t hpm_dsp_relative_entropy_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_relative_entropy_f32(src1, src2, size);
#endif
}

/**
 * @brief Log-Sum-Exp of the floating-potint vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return lse value.
 *
 */
static inline float32_t hpm_dsp_lse_f32(const float32_t *src, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_lse_f32(src, size);
#endif
}

/**
 * @brief Dot product with Log-Sum-Exp of the floating-potint vector.
 * @param[in]       *src1    points to the first input vector.
 * @param[in]       *src2    points to the second input vector.
 * @param[in]       size    size of the vectors.
 * @param[in]       *buffer  points to temporary buffer.
 * @return the Log-Sum-Exp of dot product value.
 *
 */
static inline float32_t hpm_dsp_lse_dprod_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *buffer)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_lse_dprod_f32(src1, src2, size, buffer);
#endif
}

/**
 * @brief Naive Gaussian Bayesian Estimator
 *
 * @param[in]  *instance  points to a naive bayes instance
 * @param[in]  *src       points to the elements of the input vector.
 * @param[in]  *buf       points to a buffer of length numofclass /numberOfClasses
 * @return The predicted class
 *
 */
static inline uint32_t hpm_dsp_gaussian_naive_bayes_est_f32(const riscv_dsp_gaussian_naivebayes_f32_t *instance, const float32_t * src, float32_t *buf)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_gaussian_naive_bayes_est_f32(instance, src, buf);
#endif
}

#endif
#endif

#ifdef HPM_MATH_BASIC

/**
 * @defgroup basic Basic Functions
 */
#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_basic_math.h"

// Absolute value
/**
 * @brief Absolute value of floating-potint vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_abs_f32(float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_abs_f32(src, dst, size);
#endif
}

/**
 * @brief Absolute value of q31 vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The Q31 value INT32_MIN (0x80000000) will be saturated to the maximum
 * allowable positive value INT32_MAX.
 */
static inline void hpm_dsp_abs_q31(q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_abs_q31(src, dst, size);
#endif
}

/**
 * @brief Absolute value of q15 vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The Q15 value INT16_MIN (0x8000) will be saturated to the maximum
 * allowable positive value INT16_MAX.
 */
static inline void hpm_dsp_abs_q15(q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_abs_q15(src, dst, size);
#endif
}

/**
 * @brief Absolute value of q7 vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The Q7 value INT8_MIN (0x8000) will be saturated to the maximum
 * allowable positive value INT8_MAX.
 */
static inline void hpm_dsp_abs_q7(q7_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_abs_q7(src, dst, size);
#endif
}

// Addition
/**
 * @brief Addition of floating-potint vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_add_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_add_f32(src1, src2, dst, size);
#endif
}

/**
 * @brief Addition of q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_add_q31(q31_t *src1, q31_t *src2, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_add_q31(src1, src2, dst, size);
#endif
}

/**
 * @brief Addition of q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * The output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_add_q15(q15_t *src1, q15_t *src2, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_add_q15(src1, src2, dst, size);
#endif
}

/**
 * @brief Addition of q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q7 range [0x80 0x7F].
 */
static inline void hpm_dsp_add_q7(q7_t *src1, q7_t *src2, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_add_q7(src1, src2, dst, size);
#endif
}

/**
 * @brief Addition of U8 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in U16 range [0x0000 0xFFFF].
 */
static inline void hpm_dsp_add_u8_u16(uint8_t *src1, uint8_t *src2, uint16_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_add_u8_u16(src1, src2, dst, size);
#endif
}

// Subtraction
/**
 * @brief Subtraction of floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_sub_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sub_f32(src1, src2, dst, size);
#endif
}

/**
 * @brief Subtraction of q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_sub_q31(q31_t *src1, q31_t *src2, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sub_q31(src1, src2, dst, size);
#endif
}

/**
 * @brief Subtraction of q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * The output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_sub_q15(q15_t *src1, q15_t *src2, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sub_q15(src1, src2, dst, size);
#endif
}

/**
 * @brief Subtraction of q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q7 range [0x80 0x7F].
 */
static inline void hpm_dsp_sub_q7(q7_t *src1, q7_t *src2, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sub_q7(src1, src2, dst, size);
#endif
}

/**
 * @brief Subtraction of u8 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q7 range [0x80 0x7F].
 */
static inline void hpm_dsp_sub_u8_q7(uint8_t *src1, uint8_t *src2, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sub_u8_q7(src1, src2, dst, size);
#endif
}

// Multiplication
/**
 * @brief Multiplication of floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_mul_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mul_f32(src1, src2, dst, size);
#endif
}

/**
 * @brief Multiplication of q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_mul_q31(q31_t *src1, q31_t *src2, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mul_q31(src1, src2, dst, size);
#endif
}

/**
 * @brief Multiplication of q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_mul_q15(q15_t *src1, q15_t *src2, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mul_q15(src1, src2, dst, size);
#endif
}

/**
 * @brief Multiplication of q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be saturated in Q7 range [0x80 0x7F].
 */
static inline void hpm_dsp_mul_q7(q7_t *src1, q7_t *src2, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mul_q7(src1, src2, dst, size);
#endif
}

/**
 * @brief Multiplication of u8 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 *
 * Ouput results will be in U16 range [0x00 0xFFFF].
 */
static inline void hpm_dsp_mul_u8_u16(uint8_t *src1, uint8_t *src2, uint16_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mul_u8_u16(src1, src2, dst, size);
#endif
}

// Division
/**
 * @brief Division of floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       size  size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_div_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_div_f32(src1, src2, dst, size);
#endif
}

/**
 * @brief Division of q31 inputs.
 * @param[in]       src1 the smaller input value.
 * @param[in]       src2 the larger input value.
 * @return division of two inputs.
 */
static inline q31_t hpm_dsp_div_q31(q31_t src1, q31_t src2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_div_q31(src1, src2);
#endif
}

/**
 * @brief Division of q63 inputs divided by a positive 32 bits.
 * @param[in]       src1 the q63 input value.
 * @param[in]       src2 the positive 32 bits input value.
 * @return division of two inputs.
 */
static inline q31_t hpm_dsp_div_s64_u32(q63_t src1, uint32_t src2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_div_s64_u32(src1, src2);
#endif
}

/**
 * @brief Division of positive 64-bits inputs divided by a positive 32-bits.
 * @param[in]       src1 the positive 64-bits input value.
 * @param[in]       src2 the positive 32-bits input value.
 * @return division of two inputs.
 */
static inline q31_t hpm_dsp_div_u64_u32(uint64_t src1, uint32_t src2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_div_u64_u32(src1, src2);
#endif
}

// Negation
/**
 * @brief Negation of floating-potint vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_neg_f32(float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_neg_f32(src, dst, size);
#endif
}

/**
 * @brief Negation of q31 vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The Q31 value INT32_MIN (0x80000000) will be saturated to the maximum
 * allowable positive value INT32_MAX.
 */
static inline void hpm_dsp_neg_q31(q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_neg_q31(src, dst, size);
#endif
}

/**
 * @brief Negation of q15 vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The Q15 value INT16_MIN (0x8000) will be saturated to the maximum
 * allowable positive value INT16_MAX.
 */
static inline void hpm_dsp_neg_q15(q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_neg_q15(src, dst, size);
#endif
}

/**
 * @brief Negation of q15 vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The Q7 value INT8_MIN (0x80) will be saturated to the maximum allowable
 * positive value INT8_MAX.
 */
static inline void hpm_dsp_neg_q7(q7_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_neg_q7(src, dst, size);
#endif
}

// Dot Production
/**
 * @brief Dot production of floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 */
static inline float32_t hpm_dsp_dprod_f32(float32_t *src1, float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_f32(src1, src2, size);
#endif
}

/**
 * @brief Dot production of q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 *
 * The output of multiplications is truncated from 2.62 to 2.48 format and
 * then added without saturation to a 64-bit accumulator. The return value
 * is in 16.48 format. When the size of the vectors less than 2^16, there is
 * no risk to overflow.
 */
static inline q63_t hpm_dsp_dprod_q31(q31_t *src1, q31_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_q31(src1, src2, size);
#endif
}

/**
 * @brief Dot production of q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 *
 * The output of multiplications is in 2.30 format and then added to a
 * 64-bit accumulator in 34.30 format. The return value is in 34.30 format.
 */
static inline q63_t hpm_dsp_dprod_q15(q15_t *src1, q15_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_q15(src1, src2, size);
#endif
}

/**
 * @brief Dot production of u8 * q15 vectors.
 * @param[in]       *src1 points to the uint8_t format input vector.
 * @param[in]       *src2 points to the q15 format input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 *
 * The output of multiplications is in 1.23 format and then added to an
 * accumulator in 9.23 format. The return result is in 9.23 format.
 */

static inline q31_t hpm_dsp_dprod_u8xq15(uint8_t *src1, q15_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_u8xq15(src1, src2, size);
#endif
}


/**
 * @brief Dot production of q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 *
 * The output of multiplications is in 2.14 format and then added to an
 * accumulator in 18.14 format. The return result is in 18.14 format.
 */
static inline q31_t hpm_dsp_dprod_q7(q7_t *src1, q7_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_q7(src1, src2, size);
#endif
}

/**
 * @brief Dot production of q7 * q15 vectors.
 * @param[in]       *src1 points to the q7_t format input vector.
 * @param[in]       *src2 points to the q15 format input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 *
 * The output of multiplications is in 1.22 format and then added to an
 * accumulator in 10.22 format. The return result is in 10.22 format.
 */
static inline q31_t hpm_dsp_dprod_q7xq15(q7_t *src1, q15_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_q7xq15(src1, src2, size);
#endif
}

/**
 * @brief Dot production of U8 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       size  size of the vectors.
 * @return dot product of two input vectors.
 *
 * The output of multiplications is in 0.16 format and then added to an
 * accumulator in 16.16 format. The return result is in 16.16 format.
 */
static inline uint32_t hpm_dsp_dprod_u8(uint8_t *src1, uint8_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dprod_u8(src1, src2, size);
#endif
}

// Offset
/**
 * @brief The offset of floating-point vectors.
 * @param[in]       *src points to the input vector.
 * @param[in]       offset is the value to be added.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_offset_f32(float32_t *src, float32_t offset, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_offset_f32(src, offset, dst, size);
#endif
}

/**
 * @brief The offset of q31 vectors.
 * @param[in]       *src points to the input vector.
 * @param[in]       offset is the value to be added.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * Output results are saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_offset_q31(q31_t *src, q31_t offset, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_offset_q31(src, offset, dst, size);
#endif
}

/**
 * @brief The offset of q15 vectors.
 * @param[in]       *src points to the input vector.
 * @param[in]       offset is the value to be added.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * Output results are saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_offset_q15(q15_t *src, q15_t offset, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_offset_q15(src, offset, dst, size);
#endif
}

/**
 * @brief The offset of q7 vectors.
 * @param[in]       *src points to the input vector.
 * @param[in]       offset is the value to be added.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * Output results are saturated in Q7 range [0x80 0x7F].
 */
static inline void hpm_dsp_offset_q7(q7_t *src, q7_t offset, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_offset_q7(src, offset, dst, size);
#endif
}

/**
 * @brief The offset of U8 vectors.
 * @param[in]       *src points to the input vector.
 * @param[in]       offset is the value to be added.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * Output results are saturated in U8 range [0x00 0xFF].
 */
static inline void hpm_dsp_offset_u8(uint8_t *src, q7_t offset, uint8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_offset_u8(src, offset, dst, size);
#endif
}

// Scale
/**
 * @brief To multiply a floating-point vectors by a floating-point scale.
 * @param[in]       *src points to the input vector.
 * @param[in]       scale is the value to be multiplied.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_scale_f32(float32_t *src, float32_t scale, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_scale_f32(src, scale, dst, size);
#endif
}

/**
 * @brief To multiply a q31 vectors by a q31 scale.
 * @param[in]       *src points to the input vector.
 * @param[in]       scalefract is the fractional portion value
 *                             to be multiplied.
 * @param[in]       shift      number of bits to shift.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * These are multiplied to yield a 2.62 output and then is shift with
 * saturation to 1.31 format.
 */
static inline void hpm_dsp_scale_q31(q31_t *src, q31_t scalefract, int8_t shift, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_scale_q31(src, scalefract, shift, dst, size);
#endif
}

/**
 * @brief To multiply a q15 vectors by a q15 scale.
 * @param[in]       *src points to the input vector.
 * @param[in]       scalefract is the fractional portion value
 *                             to be multiplied.
 * @param[in]       shift      number of bits to shift.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * These are multiplied to yield a 2.30 output and then is shifted with
 * saturation to 1.15 format.
 */
static inline void hpm_dsp_scale_q15(q15_t *src, q15_t scalefract, int8_t shift, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_scale_q15(src, scalefract, shift, dst, size);
#endif
}

/**
 * @brief To multiply a q7 vectors by a q7 scale.
 * @param[in]       *src points to the input vector.
 * @param[in]       scalefract is the fractional portion value
 *                             to be multiplied.
 * @param[in]       shift      number of bits to shift.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * These are multiplied to yield a 2.14 output and then is shifted with
 * saturation to 1.7 format.
 */
static inline void hpm_dsp_scale_q7(q7_t *src, q7_t scalefract, int8_t shift, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_scale_q7(src, scalefract, shift, dst, size);
#endif
}

/**
 * @brief To multiply a u8 vectors by a q7 scale.
 * @param[in]  *src points to the input vector.
 * @param[in]  scalefract: is the fractional portion value to be multiplied.
 * @param[in]  shift: number of bits to shift.
 * @param[out] *dst points to the output vector.
 * @param[in]  size size of the vectors.
 * @return none.
 *
 * The inputs are multiplied to yield a 1.15 output and then are shift with
 * saturation to 8-bit formats.
 */
static inline void hpm_dsp_scale_u8(uint8_t *src, q7_t scalefract, int8_t shift, uint8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_scale_u8(src, scalefract, shift, dst, size);
#endif
}

// Shift
/**
 * @brief Shifts a q15 vector with a specified shift number.
 * @param[in]       *src the input vector.
 * @param[in]      shift number of shift bits. If (shift > 0) means shifts
 *                       left; (shift < 0) means shifts right.
 * @param[out]      *dst the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The input and output are all saturated to q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_shift_q15(q15_t *src, int8_t shift, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_shift_q15(src, shift, dst, size);
#endif
}

/**
 * @brief Shifts a q31 vector with a specified shift number.
 * @param[in]       *src the input vector.
 * @param[in]      shift number of shift bits. If (shift > 0) means shifts
 *                       left; (shift < 0) means shifts right.
 * @param[out]      *dst the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The input and output are all saturated to q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_shift_q31(q31_t *src, int8_t shift, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_shift_q31(src, shift, dst, size);
#endif
}

/**
 * @brief Shifts a q7 vector with a specified shift number.
 * @param[in]       *src the input vector.
 * @param[in]      shift number of shift bits. If (shift > 0) means shifts
 *                       left; (shift < 0) means shifts right.
 * @param[out]      *dst the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The input and output are all saturated to q7 range [0x80 0x7F].
 */
static inline void hpm_dsp_shift_q7(q7_t *src, int8_t shift, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_shift_q7(src, shift, dst, size);
#endif
}

/**
 * @brief Shifts a u8 vector for a specified shift number.
 * @param[in]       *src the input vector.
 * @param[in]      shift number of shift bits. If (shift > 0) means shifts
 *                       left; (shift < 0) means shifts right.
 * @param[out]      *dst the output vector.
 * @param[in]       size size of the vectors.
 * @return none.
 *
 * The input and output are all saturated to u8 range [0x00 0xFF].
 */
static inline void hpm_dsp_shift_u8(uint8_t *src, int8_t shift, uint8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_shift_u8(src, shift, dst, size);
#endif
}
#endif
#endif

#ifdef HPM_MATH_COMPLEX

/**
 * @defgroup complex Complex Functions
 * This set of functions operates on complex data vectors.
 * The data in the input <code>src</code> vector and output <code>dst</code>
 * are arranged in the array as: [real, imag, real, imag, real, imag, ...).
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_complex_math.h"

// Complex Conjugate
/**
 * @brief Conjugate the floating-potint complex vector.
 * @param[in]		*src the input complex vector.
 * @param[out]		*dst the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_cconj_f32(const float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cconj_f32(src, dst, size);
#endif
}

/**
 * @brief Conjugate the q15 complex vector.
 * @param[in]		*src the input complex vector.
 * @param[out]		*dst the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The Q15 value INT16_MIN (0x8000) will be saturated to the maximum
 * allowable positive value INT16_MAX.
 */
static inline void hpm_dsp_cconj_q15(const q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cconj_q15(src, dst, size);
#endif
}

/**
 * @brief Conjugate the q31 complex vector.
 * @param[in]		*src the input complex vector.
 * @param[out]		*dst the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The Q31 value INT32_MIN (0x80000000) will be saturated to the maximum
 * allowable positive value INT32_MAX.
 */
static inline void hpm_dsp_cconj_q31(const q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cconj_q31(src, dst, size);
#endif
}

// Complex Dot Product
/**
 * @brief Compute the dot product of the floating-potint complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[in]		size  size of the vectors.
 * @param[out]		*dst  the output vector.
 * @return none.
 */
static inline void hpm_dsp_cdprod_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cdprod_f32(src1, src2, size, dst);
#endif
}

/**
 * @brief Compute the dot product type2 of the floating-potint complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[in]		size  size of the vectors.
 * @param[out]		*rout the real sum of the output.
 * @param[out]		*iout the imag sum of the output.
 * @return none.
 */
static inline void hpm_dsp_cdprod_typ2_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *rout, float32_t *iout)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cdprod_typ2_f32(src1, src2, size, rout, iout);
#endif
}

/**
 * @brief Compute the dot product of the q15 complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[in]		size  size of the vectors.
 * @param[out]		*dst  the output vector.
 * @return none.
 *
 * The multiplication outputs are in 1.15 x 1.15 = 2.30 format and
 * finally output is shift into 3.13 format.
 */
static inline void hpm_dsp_cdprod_q15(const q15_t *src1, const q15_t *src2, uint32_t size, q15_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cdprod_q15(src1, src2, size, dst);
#endif
}

/**
 * @brief Compute the dot product type2 of the q15 complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[in]		size  size of the vectors.
 * @param[out]		*rout the real sum of the output.
 * @param[out]		*iout the imag sum of the output.
 * @return none.
 *
 * The multiplication outputs are in 1.15 x 1.15 = 2.30 format and
 * finally output is shift into q24 format.
 */
static inline void hpm_dsp_cdprod_typ2_q15(const q15_t *src1, const q15_t *src2, uint32_t size, q31_t *rout, q31_t *iout)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cdprod_typ2_q15(src1, src2, size, rout, iout);
#endif
}

/**
 * @brief Compute the dot product of the q31 complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[in]		size  size of the vectors.
 * @param[out]		*dst  the output vector.
 * @return none.
 *
 * The multiplication outputs are in 1.31 x 1.31 = 2.62 format and
 * finally output is shift into 3.29 format.
 */
static inline void hpm_dsp_cdprod_q31(const q31_t *src1, const q31_t *src2, uint32_t size, q31_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cdprod_q31(src1, src2, size, dst);
#endif
}


/**
 * @brief Compute the dot product type2 of the q31 complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[in]		size  size of the vectors.
 * @param[out]		*rout the real sum of the output.
 * @param[out]		*iout the imag sum of the output.
 * @return none.
 *
 * The multiplication outputs are in 1.31 x 1.31 = 2.62 format and
 * finally output is shift into q48 format.
 */
static inline void hpm_dsp_cdprod_typ2_q31(const q31_t *src1, const q31_t *src2, uint32_t size, q63_t *rout, q63_t *iout)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cdprod_typ2_q31(src1, src2, size, rout, iout);
#endif
}

// Complex Magnitude
/**
 * @brief Compute the magnitude of the floating-potint complex vector.
 * @param[in]		*src points to the input complex vector.
 * @param[out]		*dst points to the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_cmag_f32(const float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmag_f32(src, dst, size);
#endif
}

/**
 * @brief Compute the magnitude of the q15 complex vector.
 * @param[in]		*src points to the input complex vector.
 * @param[out]		*dst points to the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The multiplication outputs are in 1.15 x 1.15 = 2.30 format and
 * finally output is shift into 2.14 format.
 */
static inline void hpm_dsp_cmag_q15(const q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmag_q15(src, dst, size);
#endif
}

/**
 * @brief Compute the magnitude of the q31 complex vector.
 * @param[in]		*src points to the input complex vector.
 * @param[out]		*dst points to the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The multiplication outputs are in 1.31 x 1.31 = 2.62 format and
 * finally output is shift into 2.30 format.
 */
static inline void hpm_dsp_cmag_q31(const q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmag_q31(src, dst, size);
#endif
}

// Complex Magnitude Squared
/**
 * @brief Compute the magnitude squared of the floating-potint complex
 *        vector.
 * @param[in]		*src points to the input complex vector.
 * @param[out]		*dst points to the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_cmag_sqr_f32(const float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmag_sqr_f32(src, dst, size);
#endif
}

/**
 * @brief Compute the magnitude squared of the q15 complex vector.
 * @param[in]		*src points to the input complex vector.
 * @param[out]		*dst points to the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The multiplication outputs are in 1.15 x 1.15 = 2.30 format and
 * finally output is shift into 3.13 format.
 */
static inline void hpm_dsp_cmag_sqr_q15(const q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmag_sqr_q15(src, dst, size);
#endif
}

/**
 * @brief Compute the magnitude squared of the q31 complex vector.
 * @param[in]		*src points to the input complex vector.
 * @param[out]		*dst points to the output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The multiplication outputs are in 1.31 x 1.31 = 2.62 format and
 * finally output is shift into 3.29 format.
 */
static inline void hpm_dsp_cmag_sqr_q31(const q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmag_sqr_q31(src, dst, size);
#endif
}

// Complex Multiplication
/**
 * @brief Multiply two folating-point complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[out]		*dst  output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_cmul_f32(const float32_t *src1, const float32_t *src2, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmul_f32(src1, src2, dst, size);
#endif
}

/**
 * @brief Multiply two q15 complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[out]		*dst  output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The multiplication outputs are in 1.15 x 1.15 = 2.30 format and
 * finally output is shift into 3.13 format.
 */
static inline void hpm_dsp_cmul_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmul_q15(src1, src2, dst, size);
#endif
}

/**
 * @brief Multiply two q31 complex vector.
 * @param[in]		*src1 the first input complex vector.
 * @param[in]		*src2 the second input complex vector.
 * @param[out]		*dst  output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * The multiplication outputs are in 1.31 x 1.31 = 2.62 format and
 * finally output is shift into 3.29 format.
 */
static inline void hpm_dsp_cmul_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmul_q31(src1, src2, dst, size);
#endif
}

// Complex-by-Real Multiplication
/**
 * @brief Multiply the folating-point complex vector by a real vector.
 * @param[in]		*src  the input complex vector.
 * @param[in]		*real the input real vector.
 * @param[out]		*dst  output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 */
static inline void hpm_dsp_cmul_real_f32(const float32_t *src, const float32_t *real, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmul_real_f32(src, real, dst, size);
#endif
}

/**
 * @brief Multiply the q15 complex vector by a real vector.
 * @param[in]		*src  the input complex vector.
 * @param[in]		*real the input real vector.
 * @param[out]		*dst  output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * Output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_cmul_real_q15(const q15_t *src, const q15_t *real, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmul_real_q15(src, real, dst, size);
#endif
}

/**
 * @brief Multiply the q31 complex vector by a real vector.
 * @param[in]		*src  the input complex vector.
 * @param[in]		*real the input real vector.
 * @param[out]		*dst  output complex vector.
 * @param[in]		size size of the vectors.
 * @return none.
 *
 * Output results will be saturated in Q31 range[0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_cmul_real_q31(const q31_t *src, const q31_t *real, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmul_real_q31(src, real, dst, size);
#endif
}
#endif
#endif

#ifdef HPM_MATH_CONTROLLER

/**
 * @defgroup controller Controller Functions
 */
#ifdef HPM_EN_MATH_DSP_LIB

#include "riscv_dsp_controller_math.h"

// Clarke Transform
/**
 * @brief Clarke transform of floating-point input.
 * @param[in]       a       input three-phase coordinate a.
 * @param[in]       b       input three-phase coordinate b.
 * @param[out]      *alpha  output two-phase orthogonal vector axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vector axis beta.
 * @return none.
 */
static inline void hpm_dsp_clarke_f32(float32_t a, float32_t b, float32_t *alpha, float32_t *beta)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_clarke_f32(a, b, alpha, beta);
#endif
}
/**
 * @brief Clarke transform of q31 input.
 * @param[in]       a       input three-phase coordinate a.
 * @param[in]       b       input three-phase coordinate b.
 * @param[out]      *alpha  output two-phase orthogonal vector axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vector axis beta.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void hpm_dsp_clarke_q31(q31_t a, q31_t b, q31_t *alpha, q31_t *beta)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_clarke_q31(a, b, alpha, beta);
#endif
}

// Inverse Clarke Transform
/**
 * @brief Inverse Clarke transform of floating-point input.
 * @param[in]       alpha   input two-phase orthogonal vector axis alpha.
 * @param[in]       beta    input two-phase orthogonal vector axis beta.
 * @param[out]      *a      output three-phase coordinate a.
 * @param[in]       *b      output three-phase coordinate b.
 * @return none.
 */
static inline void hpm_dsp_inv_clarke_f32(float32_t alpha, float32_t beta, float32_t *a, float32_t *b)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_inv_clarke_f32(alpha, beta, a, b);
#endif
}
/**
 * @brief Inverse Clarke transform of q31 input.
 * @param[in]       alpha   input two-phase orthogonal vector axis alpha.
 * @param[in]       beta    input two-phase orthogonal vector axis beta.
 * @param[out]      *a      output three-phase coordinate a.
 * @param[in]       *b      output three-phase coordinate b.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void hpm_dsp_inv_clarke_q31(q31_t alpha, q31_t beta, q31_t *a, q31_t *b)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_inv_clarke_q31(alpha, beta, a, b);
#endif
}

// Park Transform
/**
 * @brief Park transform of floating-point input.
 * @param[in]       alpha input two-phase coordinate alpha.
 * @param[in]       beta  input two-phase coordinate beta.
 * @param[out]      *a    output rotor frame a.
 * @param[out]      *b    output rotor frame b.
 * @param[in]       sin   sine value of rotation angle θ.
 * @param[in]       cos   cosine value of rotation angle θ.
 * @return none.
 */
static inline void hpm_dsp_park_f32(float32_t alpha, float32_t beta, float32_t *a, float32_t *b, float32_t sin, float32_t cos)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_park_f32(alpha, beta, a, b, sin, cos);
#endif
}

/**
 * @brief Park transform of q31 input.
 * @param[in]       alpha input two-phase coordinate alpha.
 * @param[in]       beta  input two-phase coordinate beta.
 * @param[out]      *a    output rotor frame a.
 * @param[out]      *b    output rotor frame b.
 * @param[in]       sin   sine value of rotation angle θ.
 * @param[in]       cos   cosine value of rotation angle θ.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void hpm_dsp_park_q31(q31_t alpha, q31_t beta, q31_t *a, q31_t *b, q31_t sin, q31_t cos)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_park_q31(alpha, beta, a, b, sin, cos);
#endif
}

// Inverse Park Transform
/**
 * @brief Inverse Park transform of floating-point input.
 * @param[in]       a       input coordinate of rotor frame a.
 * @param[in]       b       input coordinate of rotor frame b.
 * @param[out]      *alpha  output two-phase orthogonal vec axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vec axis beta.
 * @param[in]       sin     sine value of rotation angle θ.
 * @param[in]       cos     cosine value of rotation angle θ.
 * @return none.
 */
static inline void hpm_dsp_inv_park_f32(float32_t a, float32_t b, float32_t *alpha, float32_t *beta, float32_t sin, float32_t cos)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_inv_park_f32(a, b, alpha, beta, sin, cos);
#endif
}
/**
 * @brief Inverse Park transform of q31 input.
 * @param[in]       a       input coordinate of rotor frame a.
 * @param[in]       b       input coordinate of rotor frame b.
 * @param[out]      *alpha  output two-phase orthogonal vec axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vec axis beta.
 * @param[in]       sin     sine value of rotation angle θ.
 * @param[in]       cos     cosine value of rotation angle θ.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void hpm_dsp_inv_park_q31(q31_t a, q31_t b, q31_t *alpha, q31_t *beta, q31_t sin, q31_t cos)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_inv_park_q31(a, b, alpha, beta, sin, cos);
#endif
}
/**
 * @brief PID control of floating-point input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
static inline float32_t hpm_dsp_pid_f32(riscv_dsp_pid_f32_t *instance, float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pid_f32(instance, src);
#endif
}
/**
 * @brief PID initializatopn control function of floating-point formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
static inline void hpm_dsp_init_pid_f32(riscv_dsp_pid_f32_t *instance, int32_t set)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_init_pid_f32(instance, set);
#endif
}

/**
 * @brief PID control of Q31 input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
static inline q31_t hpm_dsp_pid_q31(riscv_dsp_pid_q31_t *instance, q31_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pid_q31(instance, src);
#endif
}

/**
 * @brief PID initializatopn control function of Q31 formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
static inline void hpm_dsp_init_pid_q31(riscv_dsp_pid_q31_t *instance, int32_t set)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_init_pid_q31(instance, set);
#endif
}

static inline q15_t hpm_dsp_pid_q15(riscv_dsp_pid_q15_t *instance, q15_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_pid_q15(instance, src);
#endif
}
/**
 * @brief PID initializatopn control function of Q15 formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
static inline void hpm_dsp_init_pid_q15(riscv_dsp_pid_q15_t *instance, int32_t set)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_init_pid_q15(instance, set);
#endif
}
#endif
#endif

#ifdef HPM_MATH_DISTANCE

/**
 * @defgroup dist Distance Functions
 */
#ifdef HPM_EN_MATH_DSP_LIB

#include "riscv_dsp_distance_math.h"


/**
 * @brief        Bray-Curtis distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_bray_curtis_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_bray_curtis_f32(src1, src2, size);
#endif
}

/**
 * @brief        Canberra distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_canberra_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_canberra_f32(src1, src2, size);
#endif
}

/**
 * @brief        Chebyshev distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_chebyshev_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_chebyshev_f32(src1, src2, size);
#endif
}

/**
 * @brief        Cityblock (Manhattan) distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_city_block_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_city_block_f32(src1, src2, size);
#endif
}

/**
 * @brief        Correlation distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_corr_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_corr_f32(src1, src2, size);
#endif
}

/**
 * @brief        Cosine distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_cos_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_cos_f32(src1, src2, size);
#endif
}

/**
 * @brief        Euclidean distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_euclidean_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_euclidean_f32(src1, src2, size);
#endif
}

/**
 * @brief        Jensen-Shannon distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_jensen_shannon_f32(const float32_t *src1, const float32_t *src2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_jensen_shannon_f32(src1, src2, size);
#endif
}

/**
 * @brief        Minkowski distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    order        Distance order
 * @param[in]    size         vector length
 * @return distance
 */
static inline float32_t hpm_dsp_dist_minkowski_f32(const float32_t *src1, const float32_t *src2, int32_t order, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_dist_minkowski_f32(src1, src2, order, size);
#endif
}

/**
 * @brief        Dice distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_dice_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_dice_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Hamming distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_hamming_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_hamming_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Jaccard distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_jaccard_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_jaccard_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Kulsinski distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_kulsinski_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_kulsinski_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Sokal-Michener distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_sokal_michener_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_sokal_michener_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Sokal-Sneath distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_sokal_sneath_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_sokal_sneath_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Roger Stanimoto distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_rogers_tanimoto_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_rogers_tanimoto_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Yule distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_yule_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_yule_u32_f32(src1, src2, numofbool);
#endif
}

/**
 * @brief        Russell-Rao distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
static inline float32_t hpm_dsp_bdist_russell_rao_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_bdist_russell_rao_u32_f32(src1, src2, numofbool);
#endif
}
#endif
#endif

#ifdef HPM_MATH_FILTERING

/**
 * @defgroup filtering Filtering Functions
 */
#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_filtering_math.h"

/**
 * @brief Function for the floating-point FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
static inline void hpm_dsp_fir_f32(const riscv_dsp_fir_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_fir_f32(instance, src, dst, size);
#endif
}

/**
 * @brief Function for the q31 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.31 format
 * and multiplications yield a 2.62 result. The 2.62 results are accumulated
 * in a 64-bit accumulator and is right shifted by 31 bits and saturated to
 * 1.31 formatthe to yield the final result. In order to avoid overflows
 * completely the input signal must be scaled down by log2(coeff_size) bits.
 */
static inline void hpm_dsp_fir_q31(const riscv_dsp_fir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_fir_q31(instance, src, dst, size);
#endif
}

/**
 * @brief Function for the q31 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.31 format.
 * These intermediate multiplications results are added to a 2.30 accumulator.
 * Finally, the accumulator is saturated and
 * converted to a 1.31 result. In order to avoid overflows
 * completely the input signal must be scaled down by log2(coeff_size) bits.
 */
static inline void hpm_dsp_fir_fast_q31(const riscv_dsp_fir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_fir_fast_q31(instance, src, dst, size);
#endif
}
/**
 * @brief Function for the q15 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.15 format
 * and multiplications yield a 2.30 result. The 2.30 results are accumulated
 * in a 64-bit accumulator in 34.30 format and the results is truncated
 * to 34.15 format by discarding low 15 bits. Lastly, the outputs is
 * saturated to yield a result in 1.15 format.
 */
static inline void hpm_dsp_fir_q15(const riscv_dsp_fir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_fir_q15(instance, src, dst, size);
#endif
}

/**
 * @brief Function for the q15 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in Q15 format and multiplications yield
 * a Q30 result. The results are accumulated in a 32-bit accumulator in Q2.30 format. Lastly, the
 * outputs are saturated to yield a result in Q1.15 format.
 */
static inline void hpm_dsp_fir_fast_q15(const riscv_dsp_fir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_fir_fast_q15(instance, src, dst, size);
#endif
}
/**
 * @brief Function for the q7 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.7 format and multiplications yield a 2.14 result.
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in
 * 18.14 format. The 18.14 result is then converted to 18.7 format by
 * discarding the low 7 bits and then saturated to 1.7 format.
 */
static inline void hpm_dsp_fir_q7(const riscv_dsp_fir_q7_t *instance, q7_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_fir_q7(instance, src, dst, size);
#endif
}

/**
 * @brief Function for the floating-point lattice FIR filter.
 * @param[in]       *instance points to an instance of the lattice
 *                            FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
static inline void hpm_dsp_lfir_f32(const riscv_dsp_lfir_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_lfir_f32(instance, src, dst, size);
#endif
}

/**
 * @brief Function for the q15 lattice FIR filter.
 * @param[in]       *instance points to an instance of the lattice
 *                            FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
static inline void hpm_dsp_lfir_q15(const riscv_dsp_lfir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_lfir_q15(instance, src, dst, size);
#endif
}

/**
 * @brief Function for the q31 lattice FIR filter.
 * @param[in]       *instance points to an instance of the lattice
 *                            FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * In order to avoid overflows the input signal must be scaled down by
 * 2*log2(stage) bits.
 */
static inline void hpm_dsp_lfir_q31(const riscv_dsp_lfir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_lfir_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_dcmfir_f32(const riscv_dsp_dcmfir_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dcmfir_f32(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_dcmfir_q15(const riscv_dsp_dcmfir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dcmfir_q15(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_dcmfir_q31(const riscv_dsp_dcmfir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dcmfir_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_dcmfir_fast_q31(const riscv_dsp_dcmfir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dcmfir_fast_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_dcmfir_fast_q15(const riscv_dsp_dcmfir_q15_t *instance, q15_t *src,  q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dcmfir_fast_q15(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_upsplfir_f32(const riscv_dsp_upsplfir_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_upsplfir_f32(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_upsplfir_q15(const riscv_dsp_upsplfir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_upsplfir_q15(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_upsplfir_q31(const riscv_dsp_upsplfir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_upsplfir_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_spafir_f32(riscv_dsp_spafir_f32_t *instance, float32_t *src, float32_t *dst, float32_t *buf, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_spafir_f32(instance, src, dst, buf, size);
#endif
}
static inline void hpm_dsp_spafir_q15(riscv_dsp_spafir_q15_t *instance, q15_t *src, q15_t *dst, q15_t *buf1, q31_t *buf2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_spafir_q15(instance, src, dst, buf1, buf2, size);
#endif
}
static inline void hpm_dsp_spafir_q31(riscv_dsp_spafir_q31_t *instance, q31_t *src, q31_t *dst, q31_t *buf, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_spafir_q31(instance, src, dst, buf, size);
#endif
}
static inline void hpm_dsp_spafir_q7(riscv_dsp_spafir_q7_t *instance, q7_t *src, q7_t *dst, q7_t *buf1, q31_t *buf2, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_spafir_q7(instance, src, dst, buf1, buf2, size);
#endif
}

// Standard LMS filte
/**
 * @brief Structure for the floatint-point standard LMS Filters.
 */

/**
 * @brief Function for the floating-point LMS filter.
 * @param[in]       *instance points to an instance of the LMS structure.
 * @param[in]       *src      points to the input block data.
 * @param[in]       *ref      points to the reference data.
 * @param[out]      *dst      points to the output data.
 * @param[out]      *err      points to the error data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
static inline void hpm_dsp_lms_f32(const riscv_dsp_lms_f32_t *instance, float32_t *src, float32_t *ref, float32_t *dst, float32_t *err, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_lms_f32(instance, src, ref, dst, err, size);
#endif
}

/**
 * @brief Function for the q31 LMS filter.
 * @param[in]       *instance points to an instance of the LMS structure.
 * @param[in]       *src      points to the input block data.
 * @param[in]       *ref      points to the reference data.
 * @param[out]      *dst      points to the output data.
 * @param[out]      *err      points to the error data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.31 format
 * and multiplications yield a 2.62 result. The 2.62 results are accumulated
 * in a 64-bit accumulator and is right shifted by 31 bits and saturated to
 * 1.31 formatthe to yield the final result. In order to avoid overflows
 * completely the input signal must be scaled down by log2(coeff_size) bits.
 */
static inline void hpm_dsp_lms_q31(const riscv_dsp_lms_q31_t *instance, q31_t *src, q31_t *ref, q31_t *dst, q31_t *err, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_lms_q31(instance, src, ref, dst, err, size);
#endif
}

/**
 * @brief Function for the q15 LMS filter.
 * @param[in]       *instance points to an instance of the LMS structure.
 * @param[in]       *src      points to the input block data.
 * @param[in]       *ref      points to the reference data.
 * @param[out]      *dst      points to the output data.
 * @param[out]      *err      points to the error data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.15 format
 * and multiplications yield a 2.30 result. The 2.30 results are accumulated
 * in a 64-bit accumulator in 34.30 format and the results is truncated
 * to 34.15 format by discarding low 15 bits. Lastly, the outputs is
 * saturated to yield a result in 1.15 format.
 */
static inline void hpm_dsp_lms_q15(const riscv_dsp_lms_q15_t *instance, q15_t *src, q15_t *ref, q15_t *dst, q15_t *err, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_lms_q15(instance, src, ref, dst, err, size);
#endif
}

/**
 * @brief Structure for the f32 normalized LMS filter.
 */

static inline void hpm_dsp_nlms_f32(riscv_dsp_nlms_f32_t *instance, float32_t *src, float32_t *ref, float32_t *dst, float32_t *err, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_nlms_f32(instance, src, ref, dst, err, size);
#endif
}


/**
 * @brief Structure for the q31 normalized LMS filter.
 */

static inline void hpm_dsp_nlms_q31(riscv_dsp_nlms_q31_t *instance, q31_t *src, q31_t *ref, q31_t *dst, q31_t *err, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_nlms_q31(instance, src, ref, dst, err, size);
#endif
}


static inline void hpm_dsp_nlms_q15(riscv_dsp_nlms_q15_t *instance, q15_t *src, q15_t *ref, q15_t *dst, q15_t *err, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_nlms_q15(instance, src, ref, dst, err, size);
#endif
}


// Convolution
/**
 * @brief Convolution of the floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 */
static inline void hpm_dsp_conv_f32(float32_t *src1, uint32_t len1, float32_t *src2, uint32_t len2, float32_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_conv_f32(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Convolution of the q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.15 format and multiplications yield a 2.30 result.
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in
 * 34.30 format. The 34.30 result is then truncated to 34.15 format by
 * discarding the low 15 bits and then saturated to 1.15 format.
 */
static inline void hpm_dsp_conv_q15(q15_t *src1, uint32_t len1, q15_t *src2, uint32_t len2, q15_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_conv_q15(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Convolution of the q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.31 format and the 64-bit accumulator has a 2.62
 * format and maintains full precision of the intermediate multiplication
 * results but provides only a single guard bit. The input signals should be
 * scaled down to avoid intermediate overflows. Scale down the inputs by
 * log2(min(srcALen, srcBLen)), The 2.62 accumulator is right shifted by 31
 * bits and saturated to 1.31 forma t to yield the final result.
 */
static inline void hpm_dsp_conv_q31(q31_t *src1, uint32_t len1, q31_t *src2, uint32_t len2, q31_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_conv_q31(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Convolution of the q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.7 format and multiplications yield a 2.14 result.
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in
 * 18.14 format. The 18.14 result is then truncated to 18.7 format by
 * discarding the low 7 bits and then saturated to 1.7 format.
 */
static inline void hpm_dsp_conv_q7(q7_t *src1, uint32_t len1, q7_t *src2, uint32_t len2, q7_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_conv_q7(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Convolution Partial of the floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
static inline int32_t hpm_dsp_conv_partial_f32(float32_t *src1, uint32_t len1, float32_t *src2, uint32_t len2, float32_t *dst, uint32_t startindex, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_conv_partial_f32(src1, len1, src2, len2, dst, startindex, size);
#endif
}

/**
 * @brief Convolution Partial of the q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
static inline int32_t hpm_dsp_conv_partial_q15(q15_t *src1, uint32_t len1, q15_t *src2, uint32_t len2, q15_t *dst, uint32_t startindex, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_conv_partial_q15(src1, len1, src2, len2, dst, startindex, size);
#endif
}

/**
 * @brief Convolution Partial of the q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
static inline int32_t hpm_dsp_conv_partial_q31(q31_t *src1, uint32_t len1, q31_t *src2, uint32_t len2, q31_t *dst, uint32_t startindex, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_conv_partial_q31(src1, len1, src2, len2, dst, startindex, size);
#endif
}

/**
 * @brief Convolution Partial of the q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
static inline int32_t hpm_dsp_conv_partial_q7(q7_t *src1, uint32_t len1, q7_t *src2, uint32_t len2, q7_t *dst, uint32_t startindex, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_conv_partial_q7(src1, len1, src2, len2, dst, startindex, size);
#endif
}


// Correlation
/**
 * @brief Correlation of the floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        2 * max(len1, len2) - 1.
 * @return none.
 */
static inline void hpm_dsp_corr_f32(float32_t *src1, uint32_t len1, float32_t *src2, uint32_t len2, float32_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_corr_f32(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Correlation of the q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        2 * max(len1, len2) - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.15 format and multiplications yield a 2.30 result.
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in
 * 34.30 format. The 34.30 result is then truncated to 34.15 format by
 * discarding the low 15 bits and then saturated to 1.15 format.
 */
static inline void hpm_dsp_corr_q15(q15_t *src1, uint32_t len1, q15_t *src2, uint32_t len2, q15_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_corr_q15(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Convolution of the q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.31 format and the 64-bit accumulator has a 2.62
 * format and maintains full precision of the intermediate multiplication
 * results but provides only a single guard bit. The input signals should be
 * scaled down to avoid intermediate overflows. Scale down one of the inputs
 * by <code>1/min(srcALen, srcBLen)</code> to avoid overflows since a
 * maximum of <code>min(srcALen, srcBLen)</code> number of additions is
 * carried internally. The 2.62 accumulator is right shifted by 31 bits and
 * saturated to 1.31 forma t to yield the final result.
 */
static inline void hpm_dsp_corr_q31(q31_t *src1, uint32_t len1, q31_t *src2, uint32_t len2, q31_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_corr_q31(src1, len1, src2, len2, dst);
#endif
}

/**
 * @brief Correlation of the q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        2 * max(len1, len2) - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.7 format and multiplications yield a 2.14 result.
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in
 * 18.14 format. The 18.14 result is then truncated to 18.7 format by
 * discarding the low 7 bits and then saturated to 1.7 format.
 */
static inline void hpm_dsp_corr_q7(q7_t *src1, uint32_t len1, q7_t *src2, uint32_t len2, q7_t *dst)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_corr_q7(src1, len1, src2, len2, dst);
#endif
}
static inline void hpm_dsp_bq_df1_f32(const riscv_dsp_bq_df1_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df1_f32(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df1_q15(const riscv_dsp_bq_df1_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df1_q15(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df1_fast_q15(const riscv_dsp_bq_df1_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df1_fast_q15(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df1_q31(const riscv_dsp_bq_df1_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df1_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df1_fast_q31(const riscv_dsp_bq_df1_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df1_fast_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df1_32x64_q31(const riscv_dsp_bq_df1_32x64_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df1_32x64_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df2T_f32(const riscv_dsp_bq_df2T_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df2T_f32(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_df2T_f64(const riscv_dsp_bq_df2T_f64_t *instance, float64_t *src, float64_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_df2T_f64(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_bq_stereo_df2T_f32(const riscv_dsp_bq_stereo_df2T_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_bq_stereo_df2T_f32(instance, src, dst, size);
#endif
}

static inline void hpm_dsp_liir_f32(const riscv_dsp_liir_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_liir_f32(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_liir_q31(const riscv_dsp_liir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_liir_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_liir_fast_q31(const riscv_dsp_liir_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_liir_fast_q31(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_liir_q15(const riscv_dsp_liir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_liir_q15(instance, src, dst, size);
#endif
}
static inline void hpm_dsp_liir_fast_q15(const riscv_dsp_liir_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_liir_fast_q15(instance, src, dst, size);
#endif
}
#endif
#endif

#ifdef HPM_MATH_MATRIX

/**
 * @defgroup matrix Matrix Functions
 *
 * This set of functions provides basic matrix math operations.
 * The funciotn specifies the size of the matrix and then points to an array.
 * For example,
 * the function definition for the floating-point is shown below:
 * <pre>
 *     void riscv_dsp_funcname_f32(const float32_t *src1,
 *                             const float32_t *src2,
 *                                   float32_t *dst,
 *                                   uint32_t row,
 *                                   uint32_t col,
 *                                   uint32_t row2,
 *                                   uint32_t col2)
 * </pre>
 * where it can be transform to the two matrix. For the matrix 1 is a
 * <code>row * col</code> matrix and the matrix 2 is a
 * <code>rol2 * col2</code> and the output matrix woild be different since
 * the math operation. There are similar definitions for Q15 and Q31 data types.
 */
#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_matrix_math.h"

// Matrix Addition
/**
 * @brief Addition of two floating-potint matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_add_f32(const float32_t *src1, const float32_t *src2, float32_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_add_f32(src1, src2, dst, row, col);
#endif
}

/**
 * @brief Addition of two q15 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * The output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_mat_add_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_add_q15(src1, src2, dst, row, col);
#endif
}

/**
 * @brief Addition of two q31 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_mat_add_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_add_q31(src1, src2, dst, row, col);
#endif
}

// Matrix Inverse
/**
 * @brief Compute the inverse matrix of the floating-potint matrix.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       size  number of the matrix row or column.
 * @return the inverse process success or not.
 */
static inline int32_t hpm_dsp_mat_inv_f32(float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mat_inv_f32(src, dst, size);
#endif
}
static inline int32_t hpm_dsp_mat_inv_f64(float64_t *src, float64_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mat_inv_f64(src, dst, size);
#endif
}

// Matrix Multiplication
/**
 * @brief Multiplication of two floating-point matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_mul_f32(const float32_t *src1, const float32_t *src2, float32_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_f32(src1, src2, dst, row, col, col2);
#endif
}

static inline void hpm_dsp_mat_mul_f64(const float64_t *src1, const float64_t *src2, float64_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_f64(src1, src2, dst, row, col, col2);
#endif
}

/**
 * @brief Multiplication of two floating-point complex matrices.
 * @param[in]       *src1 points to the first input complex matrix.
 * @param[in]       *src2 points to the second input complex matrix.
 * @param[out]      *dst  points to the output complex matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 */
static inline void hpm_dsp_cmat_mul_f32(const float32_t *src1, const float32_t *src2, float32_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmat_mul_f32(src1, src2, dst, row, col, col2);
#endif
}

/**
 * @brief Multiplication of two q15 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
static inline void hpm_dsp_mat_mul_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_q15(src1, src2, dst, row, col, col2);
#endif
}
static inline void hpm_dsp_mat_mul_fast_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_fast_q15(src1, src2, dst, row, col, col2);
#endif
}

/**
 * @brief Multiplication of two q15 complex matrices.
 * @param[in]       *src1 points to the first input complex matrix.
 * @param[in]       *src2 points to the second input complex matrix.
 * @param[out]      *dst  points to the output complex matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
static inline void hpm_dsp_cmat_mul_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmat_mul_q15(src1, src2, dst, row, col, col2);
#endif
}

/**
 * @brief Multiplication of two q31 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(col)</code> bits, Finally, the 2.62 accumulator is right
 * shifted by 31 bits to yield a 1.31 format value.
 */
static inline void hpm_dsp_mat_mul_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_q31(src1, src2, dst, row, col, col2);
#endif
}
static inline void hpm_dsp_mat_mul_fast_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_fast_q31(src1, src2, dst, row, col, col2);
#endif
}
/**
 * @brief Outer production of two q31 matrices.
 * @param[in]       *src1 pointer of the first input matrix with a size of size1*1.
 * @param[in]       *src2 pointer of the second input matrix with a size of 1*size2.
 * @param[out]      *dst  pointer of the output matrix with a size of size1 * size2.
 * @param[in]       size1 number of rows in the first input matrix.
 * @param[in]       size2 number of columns in the second input matrix.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 */
static inline void hpm_dsp_mat_oprod_q31(const q31_t * src1, const q31_t * src2, q31_t * dst, uint32_t size1, uint32_t size2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_oprod_q31(src1, src2, dst, size1, size2);
#endif
}

/**
 * @brief Multiplication of two q31 complex matrices.
 * @param[in]       *src1 points to the first input complex matrix.
 * @param[in]       *src2 points to the second input complex matrix.
 * @param[out]      *dst  points to the output complex matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(col)</code> bits, Finally, the 2.62 accumulator is right
 * shifted by 31 bits to yield a 1.31 format value.
 */
static inline void hpm_dsp_cmat_mul_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cmat_mul_q31(src1, src2, dst, row, col, col2);
#endif
}

/**
 * @brief Multiplication of two q7 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.7 format input is multiplied yields a 2.15 format, and then added
 * without saturation to a 32-bit accumulator in 17.15 format. Finally,
 * the added output is truncated to 17.7 format by discarding the lower 7
 * bits, and then saturated to yield a result in 1.7 format.
 */
static inline void hpm_dsp_mat_mul_q7(const q7_t *src1, const q7_t *src2, q7_t *dst, uint32_t row, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_q7(src1, src2, dst, row, col, col2);
#endif
}

/**
 * @brief Multiplication of q7 vetor by matrix.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output vector.
 * @param[in]       col   number of the first input vector columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.7 format input is multiplied yields a 2.15 format, and then added
 * without saturation to a 32-bit accumulator in 17.15 format. Finally,
 * the added output is truncated to 17.7 format by discarding the lower 7
 * bits, and then saturated to yield a result in 1.7 format.
 */
static inline void hpm_dsp_mat_mul_vxm_q7(const q7_t * src1, const q7_t * src2, q7_t * dst, uint32_t col, uint32_t col2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_vxm_q7(src1, src2, dst, col, col2);
#endif
}

// Matrix Power 2 Function
//
// The input is a square matrix for riscv_dsp_mat_pow2_cache_f64.
static inline int32_t hpm_dsp_mat_pwr2_cache_f64(const float64_t *src, float64_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_mat_pwr2_cache_f64(src, dst, size);
#endif
}

// Matrix Scale
/**
 * @brief Multiplt a scale value of floating-potint matrix.
 * @param[in]       *src points to the input matrix.
 * @param[in]       scale is the factor to be multiplied.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_scale_f32(const float32_t *src, float32_t scale, float32_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_scale_f32(src, scale, dst, row, col);
#endif
}

/**
 * @brief Multiplt a scale value of q15 matrix.
 * @param[in]       *src      points to the input matrix.
 * @param[in]       scalefract fractional multiplication.
 * @param[in]       shift      arithmetic shift.
 * @param[out]      *dst       points to the output matrix.
 * @param[in]       row        number of the matrix rows.
 * @param[in]       col        number of the matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format inputs are multiplied to yield a 2.30 intermediate result
 * and this is shifted with saturation to 1.15 format.
 */
static inline void hpm_dsp_mat_scale_q15(const q15_t *src, q15_t scale_fract, int32_t shift, q15_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_scale_q15(src, scale_fract, shift, dst, row, col);
#endif
}

/**
 * @brief Multiplt a scale value of q31 matrix.
 * @param[in]       *src      points to the input matrix.
 * @param[in]       scalefract fractional multiplication.
 * @param[in]       shift      arithmetic shift.
 * @param[out]      *dst       points to the output matrix.
 * @param[in]       row        number of the matrix rows.
 * @param[in]       col        number of the matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input are multiplied to yield a 2.62 intermediate result
 * and this is shifted with saturation to 1.31 format.
 */
static inline void hpm_dsp_mat_scale_q31(const q31_t *src, q31_t scale_fract, int32_t shift, q31_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_scale_q31(src, scale_fract, shift, dst, row, col);
#endif
}

// Matrix Subtraction
/**
 * @brief Substraction of two floating-potint matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_sub_f32(const float32_t *src1, const float32_t *src2, float32_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_sub_f32(src1, src2, dst, row, col);
#endif
}

/**
 * @brief Substraction of two q15 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * The output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
static inline void hpm_dsp_mat_sub_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_sub_q15(src1, src2, dst, row, col);
#endif
}

/**
 * @brief Substraction of two q31 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
static inline void hpm_dsp_mat_sub_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_sub_q31(src1, src2, dst, row, col);
#endif
}

// Matrix Transpose
/**
 * @brief Transpose the floating-potint matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_trans_f32(const float32_t *src, float32_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_f32(src, dst, row, col);
#endif
}

/**
 * @brief Transpose the q15 matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_trans_q15(const q15_t *src, q15_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_q15(src, dst, row, col);
#endif
}

/**
 * @brief Transpose the q31 matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_trans_q31(const q31_t *src, q31_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_q31(src, dst, row, col);
#endif
}

/**
 * @brief Transpose the u8 matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
static inline void hpm_dsp_mat_trans_u8(const uint8_t *src, uint8_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_u8(src, dst, row, col);
#endif
}

#endif
#endif

#ifdef HPM_MATH_SVM

/**
 * @defgroup svm SVM Functions
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_svm_math.h"
/**
 * @brief SVM linear prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

static inline void hpm_dsp_svm_linear_est_f32(const riscv_dsp_svm_linear_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_linear_est_f32(instance, src, result);
#endif
}

/**
 * @brief SVM Sigmoid prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

static inline void hpm_dsp_svm_sigmoid_est_f32(const riscv_dsp_svm_sigmoid_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_sigmoid_est_f32(instance, src, result);
#endif
}

/**
 * @brief SVM rbf prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

static inline void hpm_dsp_svm_rbf_est_f32(const riscv_dsp_svm_rbf_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_rbf_est_f32(instance, src, result);
#endif
}

/**
 * @brief SVM polynomial prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

static inline void hpm_dsp_svm_poly_est_f32(const riscv_dsp_svm_poly_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_poly_est_f32(instance, src, result);
#endif
}

#endif
#endif

#ifdef HPM_MATH_TRANSFORM

/**
 * @defgroup transform Transform Functions
 */
#ifdef HPM_EN_MATH_DSP_LIB

#include "riscv_dsp_transform_math.h"
// CFFT/CIFFT-Radix2
/// Floating-point Radix-2 Complex FFT/IFFT Function
static inline int32_t hpm_dsp_cfft_rd2_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd2_f32(src, m);
#endif
}
static inline int32_t hpm_dsp_cifft_rd2_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd2_f32(src, m);
#endif
}

/// Q15 Radix-2 Complex FFT/IFFT Function
static inline int32_t hpm_dsp_cfft_rd2_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd2_q15(src, m);
#endif
}
static inline int32_t hpm_dsp_cifft_rd2_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd2_q15(src, m);
#endif
}

/// Q31 Radix-2 Complex FFT/IFFT Function
static inline int32_t hpm_dsp_cfft_rd2_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd2_q31(src, m);
#endif
}
static inline int32_t hpm_dsp_cifft_rd2_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd2_q31(src, m);
#endif
}

// CFFT/IFFT-Radix4
/// Floating-point Radix-4 Complex FFT/IFFT Function
static inline int32_t hpm_dsp_cfft_rd4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd4_f32(src, m);
#endif
}
static inline int32_t hpm_dsp_cifft_rd4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd4_f32(src, m);
#endif
}

/// Q15 Radix-4 Complex FFT/IFFT Function
static inline int32_t hpm_dsp_cfft_rd4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd4_q15(src, m);
#endif
}
static inline int32_t hpm_dsp_cifft_rd4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd4_q15(src, m);
#endif
}

/// Q31 Radix-4 Complex FFT/IFFT Function
static inline int32_t hpm_dsp_cfft_rd4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd4_q31(src, m);
#endif
}
static inline int32_t hpm_dsp_cifft_rd4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd4_q31(src, m);
#endif
}

// CFFT/CIFFT
/// Floating-point Complex FFT/IFFT Function
static inline void hpm_dsp_cfft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_f32(src, m);
#endif
}
static inline void hpm_dsp_cifft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_f32(src, m);
#endif
}

/// Q15 Complex FFT/IFFT Function
static inline void hpm_dsp_cfft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_q15(src, m);
#endif
}
static inline void hpm_dsp_cifft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_q15(src, m);
#endif
}

/// Q31 Complex FFT/IFFT Function
static inline void hpm_dsp_cfft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_q31(src, m);
#endif
}
static inline void hpm_dsp_cifft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_q31(src, m);
#endif
}

// Real-FFT
/// Floating-point Real FFT/IFFT Function
static inline int32_t hpm_dsp_rfft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_f32(src, m);
#endif
}
static inline int32_t hpm_dsp_rifft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_f32(src, m);
#endif
}

/// Q15 Real FFT/IFFT Function
static inline int32_t hpm_dsp_rfft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_q15(src, m);
#endif
}
static inline int32_t hpm_dsp_rifft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_q15(src, m);
#endif
}

/// Q31 Real FFT/IFFT Function
static inline int32_t hpm_dsp_rfft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_q31(src, m);
#endif
}
static inline int32_t hpm_dsp_rifft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_q31(src, m);
#endif
}

// DCT/IDCT Type II
/// Floating-point DCT/IDCT Type II Function
static inline void hpm_dsp_dct_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct_f32(src, m);
#endif
}
static inline void hpm_dsp_idct_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct_f32(src, m);
#endif
}

/// Q15 DCT/IDCT Type II Function
static inline void hpm_dsp_dct_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct_q15(src, m);
#endif
}
static inline void hpm_dsp_idct_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct_q15(src, m);
#endif
}

/// Q31 DCT/IDCT Type II Function
static inline void hpm_dsp_dct_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct_q31(src, m);
#endif
}
static inline void hpm_dsp_idct_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct_q31(src, m);
#endif
}

// DCT Type IV
/// Floating-point DCT/IDCT Type IV Function
static inline void hpm_dsp_dct4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct4_f32(src, m);
#endif
}
static inline void hpm_dsp_idct4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct4_f32(src, m);
#endif
}

/// Q15 DCT/IDCT Type IV Function
static inline void hpm_dsp_dct4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct4_q15(src, m);
#endif
}
static inline void hpm_dsp_idct4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct4_q15(src, m);
#endif
}

/// Q31 DCT/IDCT Type IV Function
static inline void hpm_dsp_dct4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct4_q31(src, m);
#endif
}
static inline void hpm_dsp_idct4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct4_q31(src, m);
#endif
}

#endif

#ifdef CONFIG_HAS_HPMSDK_FFA
#include "hpm_ffa_drv.h"
#include "hpm_soc.h"
/**
 * @brief The ffa module requires the user to pay attention to cache operations
 *
 */
/**
 * @brief fft calculation using ffa hardware acceleration unit, q15 format
 *
 * @param[in,out] src pointer of the input vector. After the function is executed,
 *  the output will be stored in the input vector.
 *  The complex data in the input vector are arranged as [real, imaginary,real, imaginary..., real, imaginary].
 * @param[in] m base 2 logarithm value of the sample number and it can be set from 3 to 9
 */
static inline void hpm_ffa_cfft_q15(q15_t *src, uint32_t m)
{
    fft_xfer_t xfer = { 0 };
    xfer.num_points = 1 << m;
    xfer.src = src;
    xfer.dst = src;
    xfer.is_ifft = false;
    xfer.src_data_type = FFA_DATA_TYPE_COMPLEX_Q15;
    xfer.dst_data_type = FFA_DATA_TYPE_COMPLEX_Q15;
    ffa_calculate_fft_blocking(HPM_FFA, &xfer);
}
/**
 * @brief fft calculation using ffa hardware acceleration unit, q31 format
 *
 * @param[in,out] src pointer of the input vector. After the function is executed,
 *  the output will be stored in the input vector.
 * The complex data in the input vector are arranged as [real, imaginary,real, imaginary..., real, imaginary].
 * @param[in] m base 2 logarithm value of the sample number and it can be set from 3 to 9
 */
static inline void hpm_ffa_cfft_q31(q31_t *src, uint32_t m)
{
    fft_xfer_t xfer = { 0 };
    xfer.num_points = 1 << m;
    xfer.src = src;
    xfer.dst = src;
    xfer.is_ifft = false;
    xfer.src_data_type = FFA_DATA_TYPE_COMPLEX_Q31;
    xfer.dst_data_type = FFA_DATA_TYPE_COMPLEX_Q31;
    ffa_calculate_fft_blocking(HPM_FFA, &xfer);
}
/**
 * @brief ifft calculation using ffa hardware acceleration unit, q15 format
 *
 * @param[in,out] src pointer of the input vector. After the function is executed,
 *  the output will be stored in the input vector.
 * The complex data in the input vector are arranged as [real, imaginary,real, imaginary..., real, imaginary].
 * @param[in] m base 2 logarithm value of the sample number and it can be set from 3 to 9
 */
static inline void hpm_ffa_cifft_q15(q15_t *src, uint32_t m)
{
    fft_xfer_t xfer = { 0 };
    xfer.num_points = 1 << m;
    xfer.src = src;
    xfer.dst = src;
    xfer.is_ifft = true;
    xfer.src_data_type = FFA_DATA_TYPE_COMPLEX_Q15;
    xfer.dst_data_type = FFA_DATA_TYPE_COMPLEX_Q15;
    ffa_calculate_fft_blocking(HPM_FFA, &xfer);
}

/**
 * @brief ifft calculation using ffa hardware acceleration unit, q31 format
 *
 * @param[in,out] src pointer of the input vector. After the function is executed,
 *  the output will be stored in the input vector.
 * The complex data in the input vector are arranged as [real, imaginary,real, imaginary..., real, imaginary].
 * @param[in] m base 2 logarithm value of the sample number and it can be set from 3 to 9
 */
static inline void hpm_ffa_cifft_q31(q31_t *src, uint32_t m)
{
    fft_xfer_t xfer = { 0 };
    xfer.num_points = 1 << m;
    xfer.src = src;
    xfer.dst = src;
    xfer.is_ifft = true;
    xfer.src_data_type = FFA_DATA_TYPE_COMPLEX_Q31;
    xfer.dst_data_type = FFA_DATA_TYPE_COMPLEX_Q31;
    ffa_calculate_fft_blocking(HPM_FFA, &xfer);
}


#endif

/**
 * @brief Software implementation does not depend on any hardware
 * 
 */

/**
 * @brief Construct a new hpm software cfft float object
 * 
 * @param src requires double the space than other interfaces, 0-n for input data, n-2n for buffers, 0-n for output data
 * @param m 2^n sampling points, including real and imaginary parts
 */
void hpm_software_cfft_float(float *src, uint32_t m);

#endif

#ifdef HPM_MATH_UTILS

/**
 * @defgroup utils Utils Functions
 * This set of functions implements sine, cosine, arctanm, and square root.
 * There are separate functions for Q15, Q31, and floating-point data.
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_utils_math.h"
// Cosine and Sine
static inline float32_t hpm_dsp_cos_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cos_f32(src);
#endif
}
static inline q31_t hpm_dsp_cos_q31(q31_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cos_q31(src);
#endif
}
static inline q15_t hpm_dsp_cos_q15(q15_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cos_q15(src);
#endif
}

static inline float32_t hpm_dsp_sin_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sin_f32(src);
#endif
}
static inline q31_t hpm_dsp_sin_q31(q31_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sin_q31(src);
#endif
}
static inline q15_t hpm_dsp_sin_q15(q15_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sin_q15(src);
#endif
}

// Arc tangent
static inline float32_t hpm_dsp_atan_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_atan_f32(src);
#endif
}
static inline q31_t hpm_dsp_atan_q31(q31_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_atan_q31(src);
#endif
}
static inline q15_t hpm_dsp_atan_q15(q15_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_atan_q15(src);
#endif
}
static inline float32_t hpm_dsp_atan2_f32(float32_t srcy, float32_t src2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_atan2_f32(srcy, src2);
#endif
}
static inline q15_t hpm_dsp_atan2_q15(q15_t srcy, q15_t src2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_atan2_q15(srcy, src2);
#endif
}
static inline q31_t hpm_dsp_atan2_q31(q31_t srcy, q31_t src2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_atan2_q31(srcy, src2);
#endif
}

// Square Root
/**
 * @brief Square root of the floating-potint input.
 * @param[in]       src the input value.
 * @return the suqare root of input.
 */
static inline float32_t hpm_dsp_sqrt_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sqrt_f32(src);
#endif
}

/**
 * @brief Square root of the q31 input.
 * @param[in]       src the input value.
 * @return the suqare root of input.
 */
static inline q31_t hpm_dsp_sqrt_q31(q31_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sqrt_q31(src);
#endif
}

/**
 * @brief Square root of the q15 input.
 * @param[in]       src the input value.
 * @return the suqare root of input.
 */
static inline q15_t hpm_dsp_sqrt_q15(q15_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sqrt_q15(src);
#endif
}

// Convert function
/**
 * @brief Convert a floating-point vector to Q15.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst yhe output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_f32_q15(float32_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_f32_q15(src, dst, size);
#endif
}

/**
 * @brief Convert a floating-point vector to Q31.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
static inline void hpm_dsp_convert_f32_q31(float32_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_f32_q31(src, dst, size);
#endif
}

/**
 * @brief Convert a floating-point vector to Q7.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
static inline void hpm_dsp_convert_f32_q7(float32_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_f32_q7(src, dst, size);
#endif
}

/**
 * @brief Convert a Q15 vector to floating.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q15_f32(q15_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q15_f32(src, dst, size);
#endif
}

/**
 * @brief Convert a Q15 vector to Q31.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q15_q31(q15_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q15_q31(src, dst, size);
#endif
}

/**
 * @brief Convert a Q15 vector to Q7.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q15_q7(q15_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q15_q7(src, dst, size);
#endif
}

/**
 * @brief Convert a Q31 vector to floating.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q31_f32(q31_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q31_f32(src, dst, size);
#endif
}

/**
 * @brief Convert a Q31 vector to Q15.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q31_q15(q31_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q31_q15(src, dst, size);
#endif
}

/**
 * @brief Convert a Q31 vector to Q7.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q31_q7(q31_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q31_q7(src, dst, size);
#endif
}

/**
 * @brief Convert a Q7 vector to floating.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q7_f32(q7_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q7_f32(src, dst, size);
#endif
}

/**
 * @brief Convert a Q7 vector to Q15.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q7_q15(q7_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q7_q15(src, dst, size);
#endif
}

/**
 * @brief Convert a Q7 vector to Q31.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
static inline void hpm_dsp_convert_q7_q31(q7_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_convert_q7_q31(src, dst, size);
#endif
}

// Duplicate function
/**
 * @brief Duplicate the floating vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
static inline void hpm_dsp_dup_f32(float32_t *src, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dup_f32(src, dst, size);
#endif
}

/**
 * @brief Duplicate the Q15 vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
static inline void hpm_dsp_dup_q15(q15_t *src, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dup_q15(src, dst, size);
#endif
}

/**
 * @brief Duplicate the Q31 vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
static inline void hpm_dsp_dup_q31(q31_t *src, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dup_q31(src, dst, size);
#endif
}

/**
 * @brief Duplicate the Q7 vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
static inline void hpm_dsp_dup_q7(q7_t *src, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dup_q7(src, dst, size);
#endif
}

// Set function
/**
 * @brief Set the floating-point vector.
 * @param[in]       val specify floating-point value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
static inline void hpm_dsp_set_f32(float32_t val, float32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_set_f32(val, dst, size);
#endif
}

/**
 * @brief Set the Q15 vector.
 * @param[in]       val specify Q15 value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
static inline void hpm_dsp_set_q15(q15_t val, q15_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_set_q15(val, dst, size);
#endif
}

/**
 * @brief Set the Q31 vector.
 * @param[in]       val specify Q31 value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
static inline void hpm_dsp_set_q31(q31_t val, q31_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_set_q31(val, dst, size);
#endif
}

/**
 * @brief Set the Q7 vector.
 * @param[in]       val specify Q7 value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
static inline void hpm_dsp_set_q7(q7_t val, q7_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_set_q7(val, dst, size);
#endif
}

/**
 * @brief Weighted Sum of the floating-potint vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       *weight points to the weighted vector.
 * @param[in]       size    size of the vectors.
 * @return Weighted Sumvalue.
 *
 */
static inline float32_t hpm_dsp_weighted_sum_f32(const float32_t *src, const float32_t *weight, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_weighted_sum_f32(src, weight, size);
#endif
}

/**
 * @brief Barycenter of the floating-potint type.
 * @param[in]       *src    points to the input vector.
 * @param[in]       *weight points to the weighted vector.
 * @param[out]      *out    points to the out vector.
 * @param[in]       numofvec    size of the vectors.
 * @param[in]       dimofvec    size of the vectors.
 * @return  None
 *
 */
static inline void hpm_dsp_barycenter_f32(const float32_t *src, const float32_t *weights, float32_t *out, uint32_t numofvec, uint32_t dimofvec)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_barycenter_f32(src, weights, out, numofvec, dimofvec);
#endif
}

#endif
#endif

#ifdef  __cplusplus
}
#endif
#endif // __NDS_STATISTICS_MATH_H__
