/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_MATH_H__
#define __HPM_MATH_H__

#include <stddef.h>
/**
 * @defgroup hpmmath HPMicro Math Functions
 * @ingroup middleware_interfaces
 */

#define HPM_DSP_HW_NDS32 1 /* andes hardware dsp */

#ifdef CONFIG_HPM_MATH_HAS_EXTRA_CONFIG
#include CONFIG_HPM_MATH_HAS_EXTRA_CONFIG
#else

/* Enable Compute Cell Library*/
/* #define HPM_EN_MATH_FFA_LIB */
/* #define HPM_EN_MATH_DSP_LIB */
/* #define HPM_EN_MATH_NN_LIB */

#define HPM_MATH_DSP_STATISTICS 1
#define HPM_MATH_DSP_BASIC 1
#define HPM_MATH_DSP_COMPLEX 1
#define HPM_MATH_DSP_CONTROLLER 1
#define HPM_MATH_DSP_DISTANCE 1
#define HPM_MATH_DSP_FILTERING 1
#define HPM_MATH_DSP_MATRIX 1
#define HPM_MATH_DSP_SVM 1
#define HPM_MATH_DSP_TRANSFORM 1
#define HPM_MATH_DSP_UTILS 1
#define HPM_MATH_DSP_SORT 1

#define HPM_MATH_NN_ACTIVATION 1
#define HPM_MATH_NN_BASIC  1
#define HPM_MATH_NN_CONCATENATION 1
#define HPM_MATH_NN_CONVOLUTION 1
#define HPM_MATH_NN_CONNECTED 1
#define HPM_MATH_NN_POOLING 1
#define HPM_MATH_NN_SOFTMAX 1
#define HPM_MATH_NN_UTIL 1

#define HPM_DSP_CORE HPM_DSP_HW_NDS32 /* DSP core selection */

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

#ifdef HPM_MATH_DSP_STATISTICS

/**
 * @defgroup statistics DSP Statistics Functions
 * @ingroup hpmmath
 * @{
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
 * Relative Entropy also called KullbackLeibler divergence:
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

/**
 * @brief Maximum absolute value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
static inline float32_t hpm_dsp_absmax_f32(const float32_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmax_f32(src, size, index);
#endif
}

/**
 * @brief Maximum absolute value of the q15 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
static inline q15_t hpm_dsp_absmax_q15(const q15_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmax_q15(src, size, index);
#endif
}

/**
 * @brief Maximum absolute value of the q31 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
static inline q31_t hpm_dsp_absmax_q31(const q31_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmax_q31(src, size, index);
#endif
}

/**
 * @brief Maximum absolute value of the q7 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
static inline q7_t hpm_dsp_absmax_q7(const q7_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmax_q7(src, size, index);
#endif
}

/**
 * @brief Minimum absolute value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
static inline float32_t hpm_dsp_absmin_f32(const float32_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmin_f32(src, size, index);
#endif
}

/**
 * @brief Minimum absolute value of the q31 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
static inline q31_t hpm_dsp_absmin_q31(const q31_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmin_q31(src, size, index);
#endif
}

/**
 * @brief Minimum absolute value of the q15 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
static inline q15_t hpm_dsp_absmin_q15(const q15_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmin_q15(src, size, index);
#endif
}

/**
 * @brief Minimum absolute value of the q7 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
static inline q7_t hpm_dsp_absmin_q7(const q7_t* src, uint32_t size, uint32_t* index)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_absmin_q7(src, size, index);
#endif
}

#endif
#endif

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_BASIC

/**
 * @defgroup basic DSP Basic Functions
 * @ingroup hpmmath
 * @{
 */
#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_basic_math.h"

// Absolute value
/**
 * @brief Absolute value of floating-potint vectors.
 * @param[in]       *src points to the input vector.
 * @param[out]      *dst points to the output vector.
 * @param[in]       size size of the vectors.
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
 *
 * The input and output are all saturated to u8 range [0x00 0xFF].
 */
static inline void hpm_dsp_shift_u8(uint8_t *src, int8_t shift, uint8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_shift_u8(src, shift, dst, size);
#endif
}

/**
 * @addtogroup basic_clip
 * @{
 */
/**
 * @brief Elementwise clipping of f32 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_clip_f32(float32_t *src, float32_t *dst, float32_t low, float32_t high, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_clip_f32(src, dst, low, high, size);
#endif
}
/**
 * @brief Elementwise clipping of q31 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_clip_q31(q31_t *src, q31_t *dst, q31_t low, q31_t high, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_clip_q31(src, dst, low, high, size);
#endif
}
/**
 * @brief Elementwise clipping of q15 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_clip_q15(q15_t *src, q15_t *dst, q15_t low, q15_t high, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_clip_q15(src, dst, low, high, size);
#endif
}
/**
 * @brief Elementwise clipping of q7 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_clip_q7(q7_t *src, q7_t *dst, q7_t low, q7_t high, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_clip_q7(src, dst, low, high, size);
#endif
}
/** @} basic_clip */

// AND
/**
 * @defgroup basic_and Bitwise AND Functions
 * @brief  Bitwise AND Functions
 *
 * Bitwise AND functions calculate logical bitwise AND value from separate source vectors and write the results one-by-one into a destination vector.
 *
 * Andes DSP library supports distinct bitwise AND functions for U32, U15 and U8 data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_and
 * @{
 */
/**
 * @brief Compute the logical bitwise AND of two u32 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_and_u32(u32_t *src1, u32_t *src2, u32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_and_u32(src1, src2, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise AND of two u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_and_u8(u8_t *src1, u8_t *src2, u8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_and_u8(src1, src2, dst, size);
#endif
}
/** @} basic_and */

// OR
/**
 * @defgroup basic_or Bitwise Inclusive OR Functions
 * @brief  Bitwise Inclusive OR Functions
 *
 * Bitwise inclusive OR functions calculate logical bitwise OR value from separate source vectors and write the results one-by-one into a destination vector.
 *
 * Andes DSP library supports distinct bitwise inclusive OR functions for U32, U15 and U8 data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_or
 * @{
 */
/**
 * @brief Compute the logical bitwise OR of two u32 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_or_u32(u32_t *src1, u32_t *src2, u32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_or_u32(src1, src2, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise OR of two u16 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_or_u16(u16_t *src1, u16_t *src2, u16_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_or_u16(src1, src2, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise OR of two u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_or_u8(u8_t *src1, u8_t *src2, u8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_or_u8(src1, src2, dst, size);
#endif
}
/** @} basic_or */

// XOR
/**
 * @defgroup basic_xor Bitwise exclusive OR Functions
 * @brief  Bitwise exclusive OR Functions
 *
 * Bitwise exclusive OR (XOR) functions calculate logical bitwise XOR value from separate source vectors and write the results one-by-one into a destination vector.
 *
 * Andes DSP library supports distinct bitwise XOR functions for U32, U15 and U8 data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_xor
 * @{
 */
/**
 * @brief Compute the logical bitwise XOR of two u32 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_xor_u32(u32_t *src1, u32_t *src2, u32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_xor_u32(src1, src2, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise XOR of two u16 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_xor_u16(u16_t *src1, u16_t *src2, u16_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_xor_u16(src1, src2, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise XOR of two u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_xor_u8(u8_t *src1, u8_t *src2, u8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_xor_u8(src1, src2, dst, size);
#endif
}
/** @} basic_xor */

// Not
/**
 * @defgroup basic_not Bitwise NOT Functions
 * @brief  Bitwise NOT Functions
 *
 * Bitwise NOT functions calculate logical bitwise NOT value from elements of a source vector and write them one-by-one into a destination vector.
 *
 * Andes DSP library supports distinct bitwise NOT functions for U32, U15 and U8 data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_not
 * @{
 */
/**
 * @brief Compute the logical bitwise NOT of u32 vector.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_not_u32(u32_t *src, u32_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_not_u32(src, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise NOT of u16 vector.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_not_u16(u16_t *src, u16_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_not_u16(src, dst, size);
#endif
}
/**
 * @brief Compute the logical bitwise NOT of u8 vector.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
static inline void hpm_dsp_not_u8(u8_t *src, u8_t *dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_not_u8(src, dst, size);
#endif
}
/** @} basic_not */

/** @} basic */

#endif

#include <stdint.h>

/**
 * @brief Reserve 8bit data lsb to msb
 *
 * @param[in] lsb lsb data
 * @return uint8_t msb
 */
uint8_t hpm_math_sw_reverse_bit8_lsb_to_msb(uint8_t lsb);

/**
 * @brief Reserve 8bit data msb to lsb
 *
 * @param[in] msb msb data
 * @return uint8_t lsb
 */
uint8_t hpm_math_sw_reverse_bit8_msb_to_lsb(uint8_t msb);

/**
 * @brief Reserve 32bit data lsb to msb
 *
 * @param[in] lsb lsb data
 * @return uint32_t msb
 */
uint32_t hpm_math_sw_reverse_bit32_lsb_to_msb(uint32_t lsb);

/**
 * @brief Reserve 32bit data msb to lsb
 *
 * @param[in] msb msb data
 * @return uint32_t lsb
 */
uint32_t hpm_math_sw_reverse_bit32_msb_to_lsb(uint32_t msb);


#endif

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_COMPLEX

/**
 * @defgroup complex DSP Complex Functions
 * This set of functions operates on complex data vectors.
 * The data in the input <code>src</code> vector and output <code>dst</code>
 * are arranged in the array as: [real, imag, real, imag, real, imag, ...).
 * @ingroup hpmmath
 * @{
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_complex_math.h"

// Complex Conjugate
/**
 * @brief Conjugate the floating-potint complex vector.
 * @param[in]		*src the input complex vector.
 * @param[out]		*dst the output complex vector.
 * @param[in]		size size of the vectors.
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

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_CONTROLLER

/**
 * @defgroup controller DSP Controller Functions
 * @ingroup hpmmath
 * @{
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
 * @param[in]       sin   sine value of rotation angle .
 * @param[in]       cos   cosine value of rotation angle .
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
 * @param[in]       sin   sine value of rotation angle .
 * @param[in]       cos   cosine value of rotation angle .
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
 * @param[in]       sin     sine value of rotation angle .
 * @param[in]       cos     cosine value of rotation angle .
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
 * @param[in]       sin     sine value of rotation angle .
 * @param[in]       cos     cosine value of rotation angle .
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

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_DISTANCE

/**
 * @defgroup dist DSP Distance Functions
 * @ingroup hpmmath
 * @{
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

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_FILTERING

/**
 * @defgroup filtering DSP Filtering Functions
 * @ingroup hpmmath
 * @{
 */
#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_filtering_math.h"

/**
 * @brief Function for the floating-point FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
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

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_MATRIX

/**
 * @defgroup matrix DSP Matrix Functions
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
 * @ingroup hpmmath
 * @{
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
 * @param[in]       *src points to the input matrix.
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
 * @brief Multiplication of two q31 complex matrices.
 * @param[in]       *src1 points to the first input complex matrix.
 * @param[in]       *src2 points to the second input complex matrix.
 * @param[out]      *dst  points to the output complex matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
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
 * @param[in]       scale_fract fractional multiplication.
 * @param[in]       shift      arithmetic shift.
 * @param[out]      *dst       points to the output matrix.
 * @param[in]       row        number of the matrix rows.
 * @param[in]       col        number of the matrix columns.
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
 * @param[in]       scale_fract fractional multiplication.
 * @param[in]       shift      arithmetic shift.
 * @param[out]      *dst       points to the output matrix.
 * @param[in]       row        number of the matrix rows.
 * @param[in]       col        number of the matrix columns.
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
 * @brief Substraction of two double-precision floating-potint matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 */
static inline void hpm_dsp_mat_sub_f64(const float64_t *src1, const float64_t *src2,
                       float64_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_sub_f64(src1, src2, dst, row, col);
#endif
}

/**
 * @brief Substraction of two floating-potint matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
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
 * @brief Transpose the double-precision floating-potint matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
static inline void hpm_dsp_mat_trans_f64(const float64_t *src, float64_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_f64(src, dst, row, col);
#endif
}

/**
 * @brief Transpose the floating-potint matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
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
 */
static inline void hpm_dsp_mat_trans_u8(const uint8_t *src, uint8_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_u8(src, dst, row, col);
#endif
}

/**
 * @brief Transpose the q7 matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
static inline void hpm_dsp_mat_trans_q7(const q7_t *src, q7_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_trans_q7(src, dst, row, col);
#endif
}

/**
 * @brief Outer production of two q31 matrices.
 * @param[in]       src1 pointer of the first input matrix with a size of size1*1
 * @param[in]       src2 pointer of the second input matrix with a size of 1*size2
 * @param[out]      dst  pointer of the output matrix with a size of size1 * size2
 * @param[in]       size1 number of rows in the first input matrix.
 * @param[in]       size2 number of columns in the second input matrix.
 * @return none.
 *
 * @b Note:
 *
 * This function multiplies a one-column matrix with size1 rows, src1[size1, 1], with a
 * one-row matrix with size2 columns, src2[1, size2], and stores the result into a matrix
 * with size1 rows and size2 columns, dst[size1, size2]. It achieves better efficiency for
 * vector-wise matrix multiplication than for regular matrix multiplication.
 *
 * @b Example
 *     <pre>
 * The following equation shows the outer product of two matrices and its result.
 *
 *
 * Its code example is as follows:
 *
 *      \#define Arow 3
 *      \#define Bcol 2
 *      q31_t src1[Arow] = {0x200000, 0x100000, 0x50000};
 *      q31_t src2[Bcol] = {0x10000, 0x30000};
 *      q31_t dst[Arow * Bcol];
 *      hpm_dsp_mat_oprod_q31 (src1, src2, dst, Arow, Bcol);
 *     </pre>
 */
static inline void hpm_dsp_mat_oprod_q31(const q31_t * src1, const q31_t * src2,
                       q31_t * dst, uint32_t size1, uint32_t size2)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_oprod_q31(src1, src2, dst, size1, size2);
#endif
}

/**
 * @brief Matrix multiply vector for f32 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 *
 * @b Example
 *  <pre>
 *
 *     \#define Arow		2
 *     \#define Acol		3
 *     float32_t src1[Arow * Acol] = {0.1, -0.1, 0.1, 0.2, -0.2, 0.3};
 *     float32_t src2[Acol] = {0.2, -0.1, -0.7};
 *     float32_t dst[Arow];
 *     hpm_dsp_mat_mul_mxv_f32 (src1, src2, dst, Arow, Acol);
 *
 * This example also serves as a reference for examples of Q31, Q15 or Q7 functions.
 *  </pre>
 */
static inline void hpm_dsp_mat_mul_mxv_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_mxv_f32(src1, src2, dst, row, col);
#endif
}
/**
 * @brief Matrix multiply vector for q15 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 */
static inline void hpm_dsp_mat_mul_mxv_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_mxv_q15(src1, src2, dst, row, col);
#endif
}
/**
 * @brief Matrix multiply vector for q31 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 */
static inline void hpm_dsp_mat_mul_mxv_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_mxv_q31(src1, src2, dst, row, col);
#endif
}
/**
 * @brief Matrix multiply vector for q7 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 */
static inline void hpm_dsp_mat_mul_mxv_q7(const q7_t *src1, const q7_t *src2,
                       q7_t *dst, uint32_t row, uint32_t col)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_mat_mul_mxv_q7(src1, src2, dst, row, col);
#endif
}

#endif
#endif

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_SVM

/**
 * @defgroup svm DSP SVM Functions
 * @ingroup hpmmath
 * @{
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_svm_math.h"
/**
 * @brief SVM linear prediction
 * @param[in]    instance          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 */

static inline void hpm_dsp_svm_linear_est_f32(const riscv_dsp_svm_linear_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_linear_est_f32(instance, src, result);
#endif
}

/**
 * @brief SVM Sigmoid prediction
 * @param[in]    instance          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 */

static inline void hpm_dsp_svm_sigmoid_est_f32(const riscv_dsp_svm_sigmoid_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_sigmoid_est_f32(instance, src, result);
#endif
}

/**
 * @brief SVM rbf prediction
 * @param[in]    instance          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 */

static inline void hpm_dsp_svm_rbf_est_f32(const riscv_dsp_svm_rbf_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_rbf_est_f32(instance, src, result);
#endif
}

/**
 * @brief SVM polynomial prediction
 * @param[in]    instance          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 */

static inline void hpm_dsp_svm_poly_est_f32(const riscv_dsp_svm_poly_f32_t *instance, const float32_t *src, int32_t *result)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_svm_poly_est_f32(instance, src, result);
#endif
}

#endif
#endif

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_TRANSFORM

/**
 * @defgroup transform DSP Transform Functions
 * @ingroup hpmmath
 * @{
 */
#ifdef HPM_EN_MATH_DSP_LIB

#include "riscv_dsp_transform_math.h"
/**
 * @brief cfft_rd2 of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 * @return 0 success; -1 failure
 *
 * @b Example
 *     <pre>
 * Given 128 samples (that is, FFT_LOGN = 7), the example of floating-point Radix-2 CFFT and
 * CIFFT is as follows:
 *      \#define FFT_LOGN 7
 *      float32_t src[2* (1 << FFT_LOGN)] = {â€?};
 *      int32_t ret;
 *      ret = hpm_dsp_cfft_rd2_f32(src, FFT_LOGN);
 *      if (ret == 0)
 *          Success
 *      Else
 *          Fail
 *      ret = hpm_dsp_cifft_rd2_f32(src, FFT_LOGN);
 *      if (ret == 0)
 *           Success
 *      Else
 *           Fail
 *
 * This example also serves as a reference for examples of Q31 and Q15 Radix-2 CFFT and
 * CIFFT functions.
 *     </pre>
 */
static inline int32_t hpm_dsp_cfft_rd2_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd2_f32(src, m);
#endif
}

/**
 * @brief cifft_rd2 of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 * @return 0 success; -1 failure
 */
static inline int32_t hpm_dsp_cifft_rd2_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd2_f32(src, m);
#endif
}

/**
 * @brief cfft_rd2 of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cfft_rd2_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd2_q15(src, m);
#endif
}

/**
 * @brief cifft_rd2 of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cifft_rd2_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd2_q15(src, m);
#endif
}

/**
 * @brief cfft_rd2 of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cfft_rd2_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd2_q31(src, m);
#endif
}

/**
 * @brief cfft_rd2 of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cifft_rd2_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd2_q31(src, m);
#endif
}

/**
 * @brief cfft_rd4 of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set as 4, 6, 8 or 10
 * @return 0 success; -1 failure
 *
 * @b Example
 *     <pre>
 * Given 256 samples (that is, FFT_LOGN = 8), the example of floating-point Radix-4 CFFT and
 * CIFFT is as follows:
 *      \#define FFT_LOGN 8
 *      float32_t src[2* (1 << FFT_LOGN)] = {â€?};
 *      int32_t ret;
 *      ret = hpm_dsp_cfft_rd4_f32(src, FFT_LOGN);
 *      if (ret == 0)
 *          Success
 *      Else
 *          Fail
 *      ret = riscv_dsp_cifft_rd4_f32(src, FFT_LOGN);
 *      if (ret == 0)
 *          Success
 *      Else
 *          Fail
 *
 * This example also serves as a reference for examples of Q31 or Q15 Radix-4 CFFT and
 * CIFFT functions.
 *     </pre>
 */
static inline int32_t hpm_dsp_cfft_rd4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd4_f32(src, m);
#endif
}

/**
 * @brief cifft_rd4 of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set as 4, 6, 8 or 10
 * @return 0 success; -1 failure
 */
static inline int32_t hpm_dsp_cifft_rd4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd4_f32(src, m);
#endif
}

/**
 * @brief cfft_rd4 of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set as 4, 6, 8 or 10
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cfft_rd4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd4_q15(src, m);
#endif
}

/**
 * @brief cifft_rd4 of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set as 4, 6, 8 or 10
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cifft_rd4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd4_q15(src, m);
#endif
}

/**
 * @brief cfft_rd4 of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set as 4, 6, 8 or 10
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cfft_rd4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cfft_rd4_q31(src, m);
#endif
}

/**
 * @brief cifft_rd4 of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set as 4, 6, 8 or 10
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_cifft_rd4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_cifft_rd4_q31(src, m);
#endif
}

/**
 * @brief cfft of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 *
 * @b Example
 *     <pre>
 * Given 128 samples (that is, FFT_LOGN = 7), the example of floating-point CFFT and
 * CIFFT is as follows:
 *      \#define FFT_LOGN 7
 *      float32_t src[2* (1 << FFT_LOGN)] = {â€?};
 *      int32_t ret;
 *      hpm_dsp_cfft_f32(src, FFT_LOGN);
 *      hpm_dsp_cifft_f32(src, FFT_LOGN);
 *
 * This example also serves as a reference for examples of F16, F64, Q31 and Q15 CFFT and
 * CIFFT functions.
 *     </pre>
 */
static inline void hpm_dsp_cfft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_f32(src, m);
#endif
}

/**
 * @brief cfft of f64 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 */
static inline void hpm_dsp_cfft_f64(float64_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_f64(src, m);
#endif
}

/**
 * @brief cifft of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 */
static inline void hpm_dsp_cifft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_f32(src, m);
#endif
}

/**
 * @brief cifft of f64 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 */
static inline void hpm_dsp_cifft_f64(float64_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_f64(src, m);
#endif
}


/**
 * @brief cfft of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_cfft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_q15(src, m);
#endif
}

/**
 * @brief cifft of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_cifft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_q15(src, m);
#endif
}

/**
 * @brief cfft of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_cfft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cfft_q31(src, m);
#endif
}

/**
 * @brief cifft of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 13
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_cifft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_cifft_q31(src, m);
#endif
}

/**
 * @brief rfft of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 *
 * @b Example
 *     <pre>
 * Given 128 samples (that is, FFT_LOGN = 7), the example of floating-point RFFT and RIFFT
 * is as follows:
 *      \#define FFT_LOGN 7
 *      float32_t src[(1 << FFT_LOGN)] = {â€?};
 *      int32_t ret;
 *      ret = hpm_dsp_rfft_f32(src, FFT_LOGN);
 *      if (ret == 0)
 *          Success
 *      else
 *          Fail
 *      ret = riscv_dsp_rifft_f32(src, FFT_LOGN);
 *      if (ret == 0)
 *          Success
 *      else
 *          Fail
 *
 * This example also serves as a reference for examples of Q31 or Q15 RFFT and RIFFT
 * functions.
 *     </pre>
 */
static inline int32_t hpm_dsp_rfft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_f32(src, m);
#endif
}

/**
 * @brief rfft of f64 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 */
static inline int32_t hpm_dsp_rfft_f64(float64_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_f64(src, m);
#endif
}

/**
 * @brief rifft of f32 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 */
static inline int32_t hpm_dsp_rifft_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_f32(src, m);
#endif
}

/**
 * @brief rifft of f64 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 */
static inline int32_t hpm_dsp_rifft_f64(float64_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_f64(src, m);
#endif
}

/**
 * @brief rfft of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_rfft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_q15(src, m);
#endif
}

/**
 * @brief rifft of q15 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_rifft_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_q15(src, m);
#endif
}

/**
 * @brief rfft of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_rfft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rfft_q31(src, m);
#endif
}

/**
 * @brief rifft of q31 vectors.
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 4 to 14
 * @return 0 success; -1 failure
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline int32_t hpm_dsp_rifft_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_rifft_q31(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 8
 * @return none
 *
 * @b Example
 *  <pre>
 * Given 256 samples (that is, FFT_LOGN = 8), the example of floating-point (DCT) type II and
 * IDCT is as follows:
 *      \#define FFT_LOGN 8
 *      float32_t src[(1 << FFT_LOGN)] = {â€?};
 *      riscv_dsp_dct_f32(src, FFT_LOGN);
 *      riscv_dsp_idct_f32(src, FFT_LOGN);
 * This example also serves as a reference for examples of Q31 or Q15 DCT type II and IDCT
 * functions.
 *  </pre>
 */
static inline void hpm_dsp_dct_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct_f32(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 8
 * @return none
 */
static inline void hpm_dsp_idct_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct_f32(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 8
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_dct_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct_q15(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 8
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_idct_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct_q15(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 8
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_dct_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct_q31(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 8
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_idct_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct_q31(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 7
 * @return none
 *
 * @b Example
 *  <pre>
 * Given 128 samples (that is, FFT_LOGN = 7), the example of floating-point DCT or IDCT type
 * IV transform is as follows:
 *      \#define FFT_LOGN 7
 *      float32_t src[(1 << FFT_LOGN)] = {â€?};
 *      riscv_dsp_dct4_f32(src, FFT_LOGN);
 *      riscv_dsp_idct4_f32(src, FFT_LOGN);
 * This example also serves as a reference for examples of Q31 or Q15 DCT type IV and IDCT
 * functions.
 *  </pre>
 */
static inline void hpm_dsp_dct4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct4_f32(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 7
 * @return none
 */
static inline void hpm_dsp_idct4_f32(float32_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct4_f32(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 7
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_dct4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct4_q15(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 7
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_idct4_q15(q15_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct4_q15(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 7
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_dct4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_dct4_q31(src, m);
#endif
}

/**
 * @param[in, out] src   pointer of the input vector. After the function is executed, the
 *                       output will be stored in the input vector.
 * @param[in]        m   base 2 logarithm value of the sample number and it can be set from 3 to 7
 * @return none
 *
 * @b Note:
 *
 * The input and output formats are listed below. To satisfy the input format corresponding to
 * your input size, you may need to perform an arithmetic shift operation before calling this
 * function.
 */
static inline void hpm_dsp_idct4_q31(q31_t *src, uint32_t m)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_idct4_q31(src, m);
#endif
}

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

#endif

/**
 * @}
 *
 */

#ifdef HPM_MATH_DSP_UTILS

/**
 * @defgroup utils DSP Utils Functions
 * This set of functions implements sine, cosine, arctanm, and square root.
 * There are separate functions for Q15, Q31, and floating-point data.
 * @ingroup hpmmath
 * @{
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

#if defined (__riscv_zfh)
/**
 * @param[in] src input value (radian)
 * @return Sine value of the input
 */
static inline float16_t hpm_dsp_sin_f16(float16_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sin_f16(src);
#endif
}
#endif

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
 * @param[in]       *weights points to the weighted vector.
 * @param[out]      *out    points to the out vector.
 * @param[in]       numofvec    size of the vectors.
 * @param[in]       dimofvec    size of the vectors.
 *
 */
static inline void hpm_dsp_barycenter_f32(const float32_t *src, const float32_t *weights, float32_t *out, uint32_t numofvec, uint32_t dimofvec)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_barycenter_f32(src, weights, out, numofvec, dimofvec);
#endif
}

/**
 * @brief Calculate exponential value of f32 vector.
 * @param[in] src input value
 * @return exponential value of the input
 */
static inline float32_t hpm_dsp_exp_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_exp_f32(src);
#endif
}

#if defined (__riscv_zfh)
/**
 * @brief Calculate exponential value of f16 vector.
 * @param[in] src input value
 * @return exponential value of the input
 */
static inline float16_t hpm_dsp_exp_f16(float16_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_exp_f16(src);
#endif
}
#endif

/**
 * @brief Calculate sigmoid value of f32 vector.
 * @param[in] src input value
 * @return sigmoid value of the input
 */
static inline float32_t hpm_dsp_sigmoid_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sigmoid_f32(src);
#endif
}

#if defined (__riscv_zfh)
/**
 * @brief Calculate sigmoid value of f16 vector.
 * @param[in] src input value
 * @return sigmoid value of the input
 */
static inline float16_t hpm_dsp_sigmoid_f16(float16_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_sigmoid_f16(src);
#endif
}
#endif

/**
 * @brief Calculate the natural logarithm value of f32 vector.
 * @param[in] src input value
 * @return natural logarithm value of the input
 */
static inline float32_t hpm_dsp_log_f32(float32_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_log_f32(src);
#endif
}

#if defined (__riscv_zfh)
/**
 * @brief Calculate the natural logarithm value of f16 vector.
 * @param[in] src input value
 * @return natural logarithm value of the input
 */
static inline float16_t hpm_dsp_log_f16(float16_t src)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    return riscv_dsp_log_f16(src);
#endif
}
#endif

/**
 * @}
 *
 */


#endif
#endif

#ifdef HPM_MATH_DSP_SORT

/**
 * @defgroup sort DSP Sort Functions
 * The generic sort function sorts elements of a vector by the algorithm and sorting order specified
 * in its instance structure. The algorithms to be chosen from to perform the generic sorting
 * include bitonic sort, bubble sort, heap sort, insertion sort, quick sort and selection sort.
 * Andes DSP library only supports the generic sort function for floating-point data.
 * @ingroup hpmmath
 * @{
 */

#ifdef HPM_EN_MATH_DSP_LIB
#include "riscv_dsp_sort_math.h"
/**
 * @param[in,out]  instance     pointer of the instance structure
 * @param[in]      alg          desired sorting algorithm
 * @param[in]      order        desired sorting order
 *
 * @b Note:
 *
 * 1. This function has to be called to initialize the instance structure before the function
 *    riscv_dsp_sort_f32 is executed. Please refer to code examples.
 *
 * 2. The possible sorting algorithms for the generic sorting (i.e., options for alg) include
 *    - RISCV_DSP_SORT_BITONIC      bitonic sort
 *    - RISCV_DSP_SORT_BUBBLE       bubble sort
 *    - RISCV_DSP_SORT_HEAP         heap sort
 *    - RISCV_DSP_SORT_INSERTION    insertion sort
 *    - RISCV_DSP_SORT_QUICK        quick sort
 *    - RISCV_DSP_SORT_SELECTION    selection sort
 *
 * 3. The possible sorting orders for the generic sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 */
static inline void hpm_dsp_sort_init_f32(riscv_dsp_sort_f32_t * instance, riscv_dsp_sort_alg alg, riscv_dsp_sort_order order)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sort_init_f32(instance, alg, order);
#endif
}

/**
 * @brief Generic sorting function
 *
 * @param[in]  instance  pointer of the instance structure
 * @param[in]  src       pointer of the input vector
 * @param[out] dst       pointer of the output vector
 * @param[in]  size      number of elements in a vector
 *
 * @b Note:
 *
 * 1. The possible sorting algorithms for the generic sorting (i.e., options for alg) include
 *    - RISCV_DSP_SORT_BITONIC      bitonic sort
 *    - RISCV_DSP_SORT_BUBBLE       bubble sort
 *    - RISCV_DSP_SORT_HEAP         heap sort
 *    - RISCV_DSP_SORT_INSERTION    insertion sort
 *    - RISCV_DSP_SORT_QUICK        quick sort
 *    - RISCV_DSP_SORT_SELECTION    selection sort
 *
 * 2. The possible sorting orders for the generic sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 *
 * 3. To ensure correct results, you must initialize the instance structure with the function
 *    riscv_dsp_sort_init_f32 before using this function riscv_dsp_sort_f32. For
 *    how to use the two functions, please refer to the code examples below.
 *
 * @b Example
 *     <pre>
 *       With the input size as 100, sorting order as ascending and sorting algorithm as quick
 *       sort, the code example of generic sorting is as follows:
 *
 *          \#define size 100
 *          riscv_dsp_sort_f32_t *instance;
 *          float32_t src[size] = {â€?};
 *          float32_t dst[size];
 *          riscv_dsp_sort_init_f32(instance, RISCV_DSP_SORT_QUICK,
 *          RISCV_DSP_SORT_ASCENDING);
 *          riscv_dsp_sort_f32(instance, src, dst, size);
 *     </pre>
 */
static inline void hpm_dsp_sort_f32(const riscv_dsp_sort_f32_t * instance,float32_t * src, float32_t * dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sort_f32(instance, src, dst, size);
#endif
}

/**
 * @param[in, out]  instance  pointer of the instance structure.
 * @param[in]       order     desired sorting order
 * @param[in]       buf       pointer of the working buffer
 *
 * @b Note:
 *
 * 1. This function has to be called to initialize the instance structure before the function
 *    riscv_dsp_sort_merge_f32 is executed. Please refer to Section 2.11.2.2 for a code
 *    example.
 *
 * 2. The possible sorting orders for the merge sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 */
static inline void hpm_dsp_sort_merge_init_f32(riscv_dsp_sort_merge_f32_t * instance, riscv_dsp_sort_order order, float32_t * buf)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sort_merge_init_f32(instance, order, buf);
#endif
}

/**
 * @brief Merge sort
 *
 * @param[in]  instance  pointer of the instance structure.
 * @param[in]  src       pointer of the input vector
 * @param[out] dst       pointer of the output vector
 * @param[in]  size      number of elements in a vector
 *
 * @b Note:
 *
 * 1. The possible sorting orders for the merge sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 *
 * 2. To ensure correct results, you must initialize the instance structure with the function
 *    riscv_dsp_sort_merge_init_f32 before using this function
 *    riscv_dsp_sort_merge_f32. For how to use the two functions, please refer to the
 *    code example below.
 *
 * @b Example
 *     <pre>
 *     With the input size as 100 and sorting order as descending, the code example of merge
 *     sorting is as follows:
 *
 *     \#define size 100
 *     riscv_dsp_sort_merge_f32_t *instance;
 *     float32_t src[size] = {â€?};
 *     float32_t buf[size];
 *     float32_t dst[size];
 *     riscv_dsp_sort_merge_init_f32(instance, RISCV_DSP_SORT_DESCENDING, buf);
 *     riscv_dsp_sort_merge_f32(instance, src, dst, size);
 *     </pre>
 */
static inline void hpm_dsp_sort_merge_f32(const riscv_dsp_sort_merge_f32_t * instance, float32_t * src, float32_t * dst, uint32_t size)
{
#if HPM_DSP_CORE == HPM_DSP_HW_NDS32
    riscv_dsp_sort_merge_f32(instance, src, dst, size);
#endif
}

#endif
#endif

#ifdef HPM_MATH_NN_ACTIVATION
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_activation.h"
/**
 * @defgroup nnactivation NN Activation Functions
 * @ingroup hpmmath
 * @brief The activation functions are used to filter out some input data. They
 *        include sigmoid, tanh and ReLU (Rectified Linear Unit) functions.
 *
 * @{
 */

/**
 * @brief           This function uses the sigmoid or tanh function to perform
 *                  activation for signed 8-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       int_bits    number of the bits in the integer part, which is
 *                              supposed to be smaller than 4
 * @param[in]       act_fun     selection of activation functions. See the Note
 *                              below for details.
 *
 * @note
 * The available activation functions for selection include:
 *  - NN_SIGMOID: Use the sigmoid activation function
 *  - NN_TANH: Use the tanh activation function
 *
 * @b Example:
 * @code
 * #define SIZE 32
 * q7_t in_out[SIZE] = {...};
 * hpm_nn_activate_s8(in_out, SIZE, 0, NN_SIGMOID);
 * @endcode
 */
static inline void hpm_nn_activate_s8(q7_t *in_out,
                        uint32_t size,
                        uint16_t int_bits,
                        riscv_nn_activation_fun act_fun)
{
    riscv_nn_activate_s8(in_out, size, int_bits, act_fun);
}

/**
 * @brief           This function uses sigmoid or tanh function to perform
 *                  activation for signed 16-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       int_bits    number of the bits in the integer part, which is
 *                              supposed to be smaller than 4
 * @param[in]       act_fun     selection of activation functions. See the Note
 *                              below for details.
 *
 * @note
 * The availbale activation functions for selection include:
 *  - NN_SIGMOID: Use the sigmoid activation function
 *  - NN_TANH: Use the tanh activation function
 */
static inline void hpm_nn_activate_s16(q15_t *in_out,
                        uint32_t size,
                        uint16_t int_bits,
                        riscv_nn_activation_fun act_fun)
{
    riscv_nn_activate_s16(in_out, size, int_bits, act_fun);
}

/**
 * @brief           This function uses the leaky ReLU function to perform
 *                  activation for signed 8-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       slope       slope value to be multiplied with the negative
 *                              inputs. The result will be right shifted 15 bits
 *                              to scale back to signed 8-bit integer.
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * q15_t slope = 16384;
 * q7_t in_out[SIZE] = {...};
 * hpm_nn_leaky_relu_s8(in_out, SIZE, slope);
 * @endcode
 */
static inline void hpm_nn_leaky_relu_s8(q7_t *in_out,
                        uint32_t size,
                        q15_t slope)
{
    riscv_nn_leaky_relu_s8(in_out, size, slope);
}

/**
 * @brief           This function uses the ReLU function to perform activation
 *                  for signed 8-bit integer input vectors.
 * @param[in,out]   data        pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       max_val     maximum value to limit the output vector
 */
static inline void hpm_nn_relu_any_s8(q7_t *data, uint16_t size, q7_t max_val)
{
    riscv_nn_relu_any_s8(data, size, max_val);
}

/**
 * @brief           This function uses the ReLU function to perform activation
 *                  for signed 8-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 *
 * @b Example:
 * @code
 * #define H 16
 * #define W 16
 * #define CH 5
 * #define NUM (H * W *CH)
 * q7_t in_out[NUM] = {...};
 * hpm_nn_relu_s8(in_out, NUM);
 * @endcode
 */
static inline void hpm_nn_relu_s8(q7_t *in_out, uint32_t size)
{
    riscv_nn_relu_s8(in_out, size);
}

/**
 * @brief           This function uses the ReLU function to perform activation
 *                  for signed 16-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 */
static inline void hpm_nn_relu_s16(q15_t *in_out, uint32_t size)
{
    riscv_nn_relu_s16(in_out, size);
}

#ifdef __riscv_zfh
/**
 * @brief           This function uses the sigmoid function to perform
 *                  activation for 16-bit half-precision floating point input
 *                  vectors.
 * @param[in]       in_vec      pointer of the input vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[out]      out_vec     pointer of the output vector
 * @return          This function returns 0.
 */
static inline int32_t hpm_nn_sigmoid_f16(const float16_t *in_vec,
                            uint32_t size,
                            float16_t *out_vec)
{
    return riscv_nn_sigmoid_f16(in_vec, size, out_vec);
}

/**
 * @brief           This function uses the tanh function to perform activation
 *                  for 16-bit half-precision floating point input vectors.
 * @param[in]       in_vec      pointer of the input vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[out]      out_vec     pointer of the output vector
 * @return          This function returns 0.
 */
static inline int32_t hpm_nn_tanh_f16(const float16_t *in_vec,
                        uint32_t size,
                        float16_t *out_vec)
{
    return riscv_nn_tanh_f16(in_vec, size, out_vec);
}
#endif

/**
 *   * @}
 */
#endif
#endif

#ifdef HPM_MATH_NN_BASIC
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_basic.h"

/**
 * @defgroup nnbasic NN Basic Functions
 * @ingroup hpmmath
 * @brief The basic functions are used to perform element-wise basic arithmetic
 *        operations.
 *
 * @{
 */

/**
 * @brief           This function performs element-wise addition for signed
 *                  8-bit integer input vectors with two-stage shift.
 * @param[in]       in_tensor1  pointer of the first input vector
 * @param[in]       in_tensor2  pointer of the second input vector
 * @param[in]       scale1      pointer of the first scaling vector
 * @param[in]       scale2      pointer of the second scaling vector
 * @param[in]       size        number of elements in the input vectors
 * @param[in]       pre_rshift  right shift amount for the accumulator before
 *                              the scaling
 * @param[in]       out_scale   scaling value for the accumulator
 * @param[in]       post_rshift right shift amount for the accumulator after the
 *                              scaling
 * @param[out]      out         pointer of the element-wise addition results
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * uint16_t pre_rshift = 8;        // The addition results of both scaled input
 *                                 // tensors are in the range of 24-bit; thus, the
 *                                 // pre_rshift should be in the range of [0, 24].
 *                                 // Here we scale down the results into 16-bit
 *                                 // range.
 * uint16_t out_scale = 3;         // Scale up the result into 18-bit range.
 * uint16_t post_rshift = 11;      // Scale down the result into 7-bit range.
 *
 * q7_t in_tensor1[SIZE] = {...};
 * q7_t in_tensor2[SIZE] = {...};
 * q15_t scale1[SIZE] = {...};
 * q15_t scale2[SIZE] = {...};
 * q7_t out[SIZE];
 *
 * hpm_nn_add_s8_sym(in_tensor1, in_tensor2, scale1, scale2, SIZE, pre_rshift,
 *     out_scale, post_rshift, out);
 * @endcode
 */
static inline void hpm_nn_add_s8_sym(const q7_t *in_tensor1,
                        const q7_t *in_tensor2,
                        const int16_t *scale1,
                        const int16_t *scale2,
                        const uint32_t size,
                        const uint16_t pre_rshift,
                        const uint16_t out_scale,
                        const uint16_t post_rshift,
                        q7_t *out)
{
        riscv_nn_add_s8_sym(in_tensor1, in_tensor2, scale1, scale2, size, pre_rshift, out_scale, post_rshift, out);
}

/**
 * @brief           This function performs element-wise addition for signed
 *                  8-bit integer input vectors with two-stage shift with
 *                  rounding.
 * @param[in]       in_tensor1  pointer of the first input vector
 * @param[in]       in_tensor2  pointer of the second input vector
 * @param[in]       scale1      scaling value for the first input vector. It
 *                              should be in the range of  0 to {2^23}.
 * @param[in]       scale2      scaling value for the second input vector. It
 *                              should be in the range of 0 to {2^23}.
 * @param[in]       size        number of elements in the input vectors
 * @param[in]       pre_rshift  right shift amount for the accumulator before
 *                              the scaling
 * @param[in]       out_scale   scaling value for the accumulator
 * @param[in]       post_rshift right shift amount for the accumulator after the
 *                              scaling
 * @param[out]      out         pointer of element-wise addition results
 *
 */
static inline void hpm_nn_add_s8_sym_round(const q7_t *in_tensor1,
                            const q7_t *in_tensor2,
                            const uint32_t scale1,
                            const uint32_t scale2,
                            const uint32_t size,
                            const uint16_t pre_rshift,
                            const uint16_t out_scale,
                            const uint16_t post_rshift,
                            q7_t *out)
{
        riscv_nn_add_s8_sym_round(in_tensor1, in_tensor2, scale1, scale2, size, pre_rshift, out_scale, post_rshift, out);
}

/**
 * @brief           This function performs element-wise addition for signed
 *                  8-bit integer input vectors.
 * @param[in]       in_tensor1  pointer of the first input vector
 * @param[in]       in_tensor2  pointer of the second input vector
 * @param[in]       in_offset1  offset value for first input vector. It should
 *                              be in the range of -127 to 128.
 * @param[in]       in_scale1   scaling value for first input vector
 * @param[in]       in_rshift1  right shift amount for the first input vector
 * @param[in]       in_offset2  offset value for the second input vector. It
 *                              should be in the range of -127 to 128.
 * @param[in]       in_scale2   scaling value for the second input vector
 * @param[in]       in_rshift2  right shift amount for the second input vector
 * @param[in]       lshift      left shift amount for the first and second input
 *                              vectors
 * @param[out]      out         pointer of the element-wise addition results
 * @param[in]       out_offset  offset value for the output
 * @param[in]       out_scale   scaling value for the output
 * @param[in]       out_rshift  right shift amount for the output
 * @param[in]       act_min     minimum value that the output is limited to
 * @param[in]       act_max     maximum value that the output is limited to
 * @param[in]       size        number of elements in the input vectors
 * @return          This function returns 0.
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * int32_t in_offset1 = 16;        // Offset for in_tensor1
 * int32_t in_scale1 = (1<<28);    // Scale down in_tensor1 by 1/23
 * int32_t in_rshift1 = 3;         // Scale down in_tensor1 by 1/23
 * int32_t in_offset2 = 17;        // Offset for in_tensor2
 * int32_t in_scale2 = (1<<28);    // Scale down in_tensor2 by 1/23
 * int32_t in_rshift2 = 3;         // Scale down in_tensor2 by 1/23
 * int32_t lshift = 10;            // Scale up the input tensor by 210 times
 * int32_t out_offset = 18;        // Offset for the output tensor
 * int32_t out_scale = (1<<30);    // Scale down in_tensor2 by 1/2
 * int32_t out_rshift = 4;         // Scale down in_tensor2 by 1/24
 * int32_t act_min = 0xffffffa3;   // Limit the outputs in the range of
 *                                 // [0xffffffa3, 0x0000005d]
 * int32_t act_max = 0x0000005d;   // Limit the outputs in the range of
 *                                 // [0xffffffa3, 0x0000005d]
 *
 * int8_t in_tensor1[SIZE] = {...};
 * int8_t in_tensor2[SIZE] = {...};
 * int8_t out[SIZE];
 *
 * hpm_nn_ew_add_s8_asym(in_tensor1, in_tensor2, in_offset1, in_scale1,
 *     in_rshift1, in_offset2, in_scale2, in_rshift2, lshift, out, out_offset,
 *     out_scale, out_rshift, act_min, act_max, SIZE);
 * @endcode
 */
static inline int hpm_nn_ew_add_s8_asym(const int8_t *in_tensor1,
                            const int8_t *in_tensor2,
                            const int32_t in_offset1,
                            const int32_t in_scale1,
                            const int32_t in_rshift1,
                            const int32_t in_offset2,
                            const int32_t in_scale2,
                            const int32_t in_rshift2,
                            const int32_t lshift,
                            int8_t *out,
                            const int32_t out_offset,
                            const int32_t out_scale,
                            const int32_t out_rshift,
                            const int32_t act_min,
                            const int32_t act_max,
                            const uint32_t size)
{
        return riscv_nn_ew_add_s8_asym(in_tensor1, in_tensor2, in_offset1, in_scale1, in_rshift1, in_offset2, in_scale2, in_rshift2, lshift, out, out_offset, out_scale, out_rshift, act_min, act_max, size);
}

/**
 * @brief           This function performs element-wise multiplication for
 *                  signed 8-bit integer input vectors.
 * @param[in]       in_tensor1  pointer of the first input vector
 * @param[in]       in_tensor2  pointer of the second input vector
 * @param[in]       in_offset1  offset value for the first input vector. It
 *                              should be in the range of -127 to 128.
 * @param[in]       in_offset2  offset value for the second input vector. It
 *                              should be in the range of -127 to 128.
 * @param[out]      out         pointer of element-wise multiplication results
 * @param[in]       out_offset  offset value for the output
 * @param[in]       out_scale   scaling value for the output
 * @param[in]       out_shift   shift amount for the output
 * @param[in]       act_min     minimum value that the output is limited to
 * @param[in]       act_max     maximum value that the output is limited to
 * @param[in]       size        number of elements in the input vectors
 * @return          This function returns 0.
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * int32_t in_offset1 = 16;            // Offset for in_tensor1
 * int32_t in_offset2 = 17;            // Offset for in_tensor2
 * int32_t out_offset = 18;            // Offset for the output tensor
 * int32_t out_scale = (1<<30);        // Scale down the output tensor by 1/2
 * int32_t out_shift = -4;             // Scale down the output tensor by 1/24
 * int32_t act_min = 0xffffffa3;       // Limit the outputs in the range of
 *                                     // [0xffffffa3, 0x0000005d]
 * int32_t act_max = 0x0000005d;       // Limit the outputs in the range of
 *                                     // [0xffffffa3, 0x0000005d]
 *
 * in_tensor1[SIZE] = {...};
 * in_tensor2[SIZE] = {...};
 * out[SIZE];
 *
 * hpm_nn_ew_mul_s8_asym(in_tensor1, in_tensor2, in_offset1, in_offset2, out,
 *     out_offset, out_scale, out_shift, act_min, act_max, SIZE);
 * @endcode
 */
static inline int hpm_nn_ew_mul_s8_asym(const int8_t *in_tensor1,
                            const int8_t *in_tensor2,
                            const int32_t in_offset1,
                            const int32_t in_offset2,
                            int8_t *out,
                            const int32_t out_offset,
                            const int32_t out_scale,
                            const int32_t out_shift,
                            const int32_t act_min,
                            const int32_t act_max,
                            const uint32_t size)
{
        return riscv_nn_ew_mul_s8_asym(in_tensor1, in_tensor2, in_offset1, in_offset2, out, out_offset, out_scale, out_shift, act_min, act_max, size);
}

/**
 *   * @}
 */

#endif

#ifdef HPM_EN_MATH_NN_RVP32_LIB
#include "riscv_nn_basic.h"

/**
 * @brief           This function performs element-wise addition for signed
 *                  8-bit integer input vectors.
 * @param[in]       in_tensor1  pointer of the first input vector
 * @param[in]       in_tensor2  pointer of the second input vector
 * @param[in]       in_offset1  offset value for first input vector. It should
 *                              be in the range of -127 to 128.
 * @param[in]       in_scale1   scaling value for first input vector
 * @param[in]       in_rshift1  right shift amount for the first input vector
 * @param[in]       in_offset2  offset value for the second input vector. It
 *                              should be in the range of -127 to 128.
 * @param[in]       in_scale2   scaling value for the second input vector
 * @param[in]       in_rshift2  right shift amount for the second input vector
 * @param[in]       lshift      left shift amount for the first and second input
 *                              vectors
 * @param[out]      out         pointer of the element-wise addition results
 * @param[in]       out_offset  offset value for the output
 * @param[in]       out_scale   scaling value for the output
 * @param[in]       out_rshift  right shift amount for the output
 * @param[in]       act_min     minimum value that the output is limited to
 * @param[in]       act_max     maximum value that the output is limited to
 * @param[in]       size        number of elements in the input vectors
 * @return          This function returns 0.
 *
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * int32_t in_offset1 = 16;        // Offset for in_tensor1
 * int32_t in_scale1 = (1<<28);    // Scale down in_tensor1 by 1/23
 * int32_t in_rshift1 = 3;         // Scale down in_tensor1 by 1/23
 * int32_t in_offset2 = 17;        // Offset for in_tensor2
 * int32_t in_scale2 = (1<<28);    // Scale down in_tensor2 by 1/23
 * int32_t in_rshift2 = 3;         // Scale down in_tensor2 by 1/23
 * int32_t lshift = 10;            // Scale up the input tensor by 210 times
 * int32_t out_offset = 18;        // Offset for the output tensor
 * int32_t out_scale = (1<<30);    // Scale down in_tensor2 by 1/2
 * int32_t out_rshift = 4;         // Scale down in_tensor2 by 1/24
 * int32_t act_min = 0xffffffa3;   // Limit the outputs in the range of
 *                                 // [0xffffffa3, 0x0000005d]
 * int32_t act_max = 0x0000005d;   // Limit the outputs in the range of
 *                                 // [0xffffffa3, 0x0000005d]
 *
 * int8_t in_tensor1[SIZE] = {...};
 * int8_t in_tensor2[SIZE] = {...};
 * int8_t out[SIZE];
 *
 * hpm_nn_ew_add_s8_asym(in_tensor1, in_tensor2, in_offset1, in_scale1,
 *     in_rshift1, in_offset2, in_scale2, in_rshift2, lshift, out, out_offset,
 *     out_scale, out_rshift, act_min, act_max, SIZE);
 * @endcode
 */
static inline int hpm_nn_ew_add_s8_asym(const int8_t *in_tensor1,
                            const int8_t *in_tensor2,
                            const int32_t in_offset1,
                            const int32_t in_scale1,
                            const int32_t in_rshift1,
                            const int32_t in_offset2,
                            const int32_t in_scale2,
                            const int32_t in_rshift2,
                            const int32_t lshift,
                            int8_t *out,
                            const int32_t out_offset,
                            const int32_t out_scale,
                            const int32_t out_rshift,
                            const int32_t act_min,
                            const int32_t act_max,
                            const uint32_t size)
{
        return riscv_nn_ew_add_s8_asym(in_tensor1, in_tensor2, in_offset1, in_scale1, in_rshift1, in_offset2, in_scale2, in_rshift2, lshift, out, out_offset, out_scale, out_rshift, act_min, act_max, size);
}

#endif

#endif

#ifdef HPM_MATH_NN_CONCATENATION
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_concatenation.h"

/**
 * @defgroup nnconcatenation NN Concatenation Functions
 * @ingroup hpmmath
 * @brief The concatenation functions are used to concatenate the tensor along
 *        the specified axis.
 *
 * @{
 */

/**
 * @brief       This function concatenates the int8_t/uint8_t input tensor along
 *              the w-axis with the output tensor.
 * @param[in]   in_tensor       pointer of the input tensor
 * @param[in]   in_tensor_x     x dimension of the input tensor
 * @param[in]   in_tensor_y     y dimension of the input tensor
 * @param[in]   in_tensor_z     z dimension of the input tensor
 * @param[in]   in_tensor_w     w dimension of the input tensor
 * @param[in]   out_tensor      pointer of the output tensor
 * @param[in]   out_offset_w    offset value to be added to the w axis of the
 *                              output tensor before the concatenation
 *
 * @note
 * The x, y and z dimension of the output tensor will be the same as those of
 * the input tensor.
 */
static inline void hpm_nn_concate_s8_w(const int8_t *in_tensor,
                        const uint16_t in_tensor_x,
                        const uint16_t in_tensor_y,
                        const uint16_t in_tensor_z,
                        const uint16_t in_tensor_w,
                        int8_t *out_tensor,
                        const uint32_t out_offset_w)
{
    riscv_nn_concate_s8_w(in_tensor, in_tensor_x, in_tensor_y, in_tensor_z, in_tensor_w, out_tensor, out_offset_w);
}

/**
 * @brief       This function concatenates the int8_t/uint8_t input tensor along
 *              the x-axis with the output tensor.
 * @param[in]   in_tensor       pointer of the input tensor
 * @param[in]   in_tensor_x     x dimension of the input tensor
 * @param[in]   in_tensor_y     y dimension of the input tensor
 * @param[in]   in_tensor_z     z dimension of the input tensor
 * @param[in]   in_tensor_w     w dimension of the input tensor
 * @param[in]   out_tensor      pointer of the output tensor
 * @param[in]   out_tensor_x    x dimension of the output tensor
 * @param[in]   out_offset_x    offset value to be added to the x axis of the
 *                              output tensor before the concatenation

 *
 * @note
 * The y, z and w dimensions of the output tensor will be the same as those of
 * the input tensor.
 */
static inline void hpm_nn_concate_s8_x(const int8_t *in_tensor,
                        const uint16_t in_tensor_x,
                        const uint16_t in_tensor_y,
                        const uint16_t in_tensor_z,
                        const uint16_t in_tensor_w,
                        int8_t *out_tensor,
                        const uint16_t out_tensor_x,
                        const uint32_t out_offset_x)
{
    riscv_nn_concate_s8_x(in_tensor, in_tensor_x, in_tensor_y, in_tensor_z, in_tensor_w, out_tensor, out_tensor_x, out_offset_x);
}

/**
 * @brief       This function concatenates the int8_t/uint8_t input tensor along
 *              the y-axis with the output tensor.
 * @param[in]   in_tensor       pointer of the input tensor
 * @param[in]   in_tensor_x     x dimension of the input tensor
 * @param[in]   in_tensor_y     y dimension of the input tensor
 * @param[in]   in_tensor_z     z dimension of the input tensor
 * @param[in]   in_tensor_w     w dimension of the input tensor
 * @param[in]   out_tensor      pointer of the output tensor
 * @param[in]   out_tensor_y    y dimension of the output tensor
 * @param[in]   out_offset_y    offset value to be added to the y axis of the
 *                              output tensor before the concatenation
 *
 * @note
 * The x, z and w dimensions of the output tensor will be the same as those of
 * the input tensor.
 */
static inline void hpm_nn_concate_s8_y(const int8_t *in_tensor,
                        const uint16_t in_tensor_x,
                        const uint16_t in_tensor_y,
                        const uint16_t in_tensor_z,
                        const uint16_t in_tensor_w,
                        int8_t *out_tensor,
                        const uint16_t out_tensor_y,
                        const uint32_t out_offset_y)
{
    riscv_nn_concate_s8_y(in_tensor, in_tensor_x, in_tensor_y, in_tensor_z, in_tensor_w, out_tensor, out_tensor_y, out_offset_y);
}

/**
 * @brief       This function concatenates the int8_t/uint8_t input tensor along
 *              the z-axis with the output tensor.
 * @param[in]   in_tensor       pointer of the input tensor
 * @param[in]   in_tensor_x     x dimension of the input tensor
 * @param[in]   in_tensor_y     y dimension of the input tensor
 * @param[in]   in_tensor_z     z dimension of the input tensor
 * @param[in]   in_tensor_w     w dimension of the input tensor
 * @param[in]   out_tensor      pointer of the output tensor
 * @param[in]   out_tensor_z    z dimension of the output tensor
 * @param[in]   out_offset_z    offset value to be added to the z axis of the
 *                              output tensor before the concatenation
 *
 * @note
 * The x, y and w dimensions of the output tensor will be the same as those of
 * the input tensor.
 */
static inline void hpm_nn_concate_s8_z(const int8_t *in_tensor,
                        const uint16_t in_tensor_x,
                        const uint16_t in_tensor_y,
                        const uint16_t in_tensor_z,
                        const uint16_t in_tensor_w,
                        int8_t *out_tensor,
                        const uint16_t out_tensor_z,
                        const uint32_t out_offset_z)
{
    riscv_nn_concate_s8_z(in_tensor, in_tensor_x, in_tensor_y, in_tensor_z, in_tensor_w, out_tensor, out_tensor_z, out_offset_z);
}

/**
 *   * @}
 */

#endif
#endif

#ifdef HPM_MATH_NN_CONVOLUTION
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_convolution.h"

/**
 * @defgroup nnconvolution NN Convolution Functions
 * @ingroup hpmmath
 * @brief The convolution functions transform the input matrix into a column
 * vector with im2col, and then use matrix-matrix multiplication to get the
 * convolution result.
 *
 * @{
 */

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  shift-based quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       bias_lshift         left shift amount for the bias
 * @param[in]       out_rshift          right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 *
 * @b Example:
 * @code
 *  //Convolve a 160x120x20 input tensor with a 1x1 kernel and generate a
 *  //160x120x8 output tensor. Let both dimensions padding be 0 and their
 *  //stride be 1.
 *
 *  #define IN_X 160
 *  #define IN_Y 120
 *  #define IN_CH 20
 *  #define OUT_CH 8
 *  #define KER_DIM_X 1
 *  #define KER_DIM_Y 1
 *  #define PAD_X 0
 *  #define PAD_Y 0
 *  #define STRIDE_X 1
 *  #define STRIDE_Y 1
 *  #define BIAS_LSHIFT 6       //Scale up the bias by 2^6
 *  #define OUT_RSHIFT 9        //Scale down the output tensor by 1/2^9
 *  #define OUT_X 160
 *  #define OUT_Y 120
 *
 *  q7_t in_data[IN_CH * IN_X * IN_Y] = {...};
 *  q7_t weight[IN_CH * KER_DIM_X * KER_DIM_Y * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * IN_CH * KER_DIM_X * KER_DIM_Y] = {0};
 *  q7_t out_data[OUT_CH * OUT_X * OUT_Y];
 *
 *  riscv_nn_conv_1x1_HWC_s8_s8_s8_sft_bias_fast_any(in_data, IN_X, IN_Y ,
 *      IN_CH, weight, OUT_CH, KER_DIM_X, KER_DIM_Y, PAD_X, PAD_Y, STRIDE_X,
 *      STRIDE_Y, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data, OUT_X, OUT_Y,
 *      in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_sft_bias_fast_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf,
                                                q7_t *tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_sft_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs signed 8-bit integer convolution for
 *                  RGB images with shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @param[in]       tmp_buf         temporary buffer for kernel weights. It is
 *                                  required when -mext-vector enabled and its
 *                                  size must be "out_tensor_ch * (3 * ker_dim *
 *                                  ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 *  //Convolve a 28x28x3 input tensor with a 5x5 kernel and generate a 24x24x20
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 28
 *  #define KER_DIM 5
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 10
 *  #define OUT_CH 20
 *  #define OUT_DIM 24
 *
 *  q7_t in_data[3 * IN_DIM * IN_DIM] = {...};
 *  q7_t weight[3 * KER_DIM * KER_DIM * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * 3 * KER_DIM * KER_DIM] = {0};
 *  q7_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias(in_data, IN_DIM, weight, OUT_CH,
 *      KER_DIM, PAD, STRIDE, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data, OUT_DIM,
 *      in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_RGB_sft_bias(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q7_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t *in_tmp_buf,
                                                q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs fast signed 8-bit integer convolution
 *                  for RGB images with shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 *  //Convolve a 28x28x3 input tensor with a 5x5 kernel and generate a 24x24x20
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 28
 *  #define KER_DIM 5
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 10
 *  #define OUT_CH 20
 *  #define OUT_DIM 24
 *
 *  q7_t in_data[3 * IN_DIM * IN_DIM] = {...};
 *  q7_t weight[3 * KER_DIM * KER_DIM * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * (3 * KER_DIM * KER_DIM + 1)] = {0};
 *  q15_t wt_tmp_buf[OUT_CH * (3 * KER_DIM * KER_DIM + 1)];
 *  q7_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias_fast(in_data, IN_DIM, weight,
 *      OUT_CH, KER_DIM, PAD, STRIDE, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data,
 *      OUT_DIM, in_tmp_buf, wt_tmp_buf);
 * @endcode
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_RGB_sft_bias_fast(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q7_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t *in_tmp_buf,
                                                q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}


/**
 * @brief           This function performs signed 8-bit integer convolution with
 *                  shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 *  //Convolve a 28x28x1 input tensor with a 5x5 kernel and generate a 24x24x20
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 28
 *  #define IN_CH 1
 *  #define KER_DIM 5
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 10
 *  #define OUT_CH 20
 *  #define OUT_DIM 24
 *
 *  q7_t in_data[IN_CH * IN_DIM * IN_DIM] = {...};
 *  q7_t weight[IN_CH * KER_DIM * KER_DIM * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * IN_CH * KER_DIM * KER_DIM] = {0};
 *  q7_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_HWC_s8_s8_s8_sft_bias(in_data, IN_DIM, IN_CH, weight, OUT_CH,
 *      KER_DIM, PAD, STRIDE, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data, OUT_DIM,
 *      in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sft_bias(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q7_t *bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sft_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs signed 8-bit integer convolution in
 *                  any x and y dimensions with shift-based quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       bias_lshift         left shift amount for the bias
 * @param[in]       out_rshift          right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @param[in]       tmp_buf             dummy
 *
 * @b Example:
 * @code
 *  //Convolve a 160x120x3 input tensor with a 3x5 kernel and generate a 80x59x5
 *  //output tensor. Let both dimensions padding be 1 and their stride be 2.
 *
 *  #define IN_X 160
 *  #define IN_Y 120
 *  #define IN_CH 3
 *  #define OUT_CH 5
 *  #define KER_DIM_X 3
 *  #define KER_DIM_Y 5
 *  #define PAD_X 1
 *  #define PAD_Y 1
 *  #define STRIDE_X 2
 *  #define STRIDE_Y 2
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 9
 *  #define OUT_X 40
 *  #define OUT_Y 30
 *
 *  q7_t in_data[IN_CH * IN_X * IN_Y] = {...};
 *  q7_t weight[IN_CH * KER_DIM_X * KER_DIM_Y * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * IN_CH * KER_DIM_X * KER_DIM_Y] = {0};
 *  q7_t out_data[OUT_CH * OUT_X * OUT_Y];
 *
 *  riscv_nn_conv_HWC_s8_s8_s8_sft_bias_any(in_data, IN_X, IN_Y , IN_CH, weight,
 *      OUT_CH, KER_DIM_X, KER_DIM_Y, PAD_X, PAD_Y, STRIDE_X, STRIDE_Y, bias,
 *      BIAS_LSHIFT, OUT_RSHIFT, out_data, OUT_X, OUT_Y, in_tmp_buf, NULL);
 * @endcode
 */

static inline void hpm_nn_conv_HWC_s8_s8_s8_sft_bias_any(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q7_t *bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf,
                                            q7_t *tmp_buf)
{
	riscv_nn_conv_HWC_s8_s8_s8_sft_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs fast signed 8-bit integer convolution
 *                  with shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @param[in]       tmp_buf         dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @b Example:
 * @code
 *  //Convolve a 12x12x20 input tensor with a 5x5 kernel and generate a 8x8x50
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 12
 *  #define IN_CH 20
 *  #define KER_DIM 5
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 10
 *  #define OUT_CH 50
 *  #define OUT_DIM 8
 *
 *  q7_t in_data[IN_CH * IN_DIM * IN_DIM] = {...};
 *  q7_t weight[IN_CH * KER_DIM * KER_DIM * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * IN_CH * KER_DIM * KER_DIM] = {0};
 *  q7_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast(in_data, IN_DIM, IN_CH, weight,
 *      OUT_CH, KER_DIM, PAD, STRIDE, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data,
 *      OUT_DIM, in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sft_bias_fast(const q7_t *in_tensor,
                                const uint16_t in_tensor_dim,
                                const uint16_t in_tensor_ch,
                                const q7_t *ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ker_dim,
                                const uint16_t pad,
                                const uint16_t stride,
                                const q7_t *bias,
                                const uint16_t bias_lshift,
                                const uint16_t out_rshift,
                                q7_t *out_tensor,
                                const uint16_t out_tensor_dim,
                                q15_t *in_tmp_buf,
                                q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs fast signed 8-bit integer convolution
 *                  in any x and y dimensions with shift-based quantization on
 *                  the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       bias_lshift         left shift amount for the bias
 * @param[in]       out_rshift          right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @b Example:
 * @code
 *  //Convolve a 160x120x20 input tensor with a 3x5 kernel and generate a
 *  //80x59x8 output tensor. Let both dimensions padding be 1 and their stride
 *  //be 2.
 *
 *  #define IN_X 160
 *  #define IN_Y 120
 *  #define IN_CH 20
 *  #define OUT_CH 8
 *  #define KER_DIM_X 3
 *  #define KER_DIM_Y 5
 *  #define PAD_X 1
 *  #define PAD_Y 1
 *  #define STRIDE_X 2
 *  #define STRIDE_Y 2
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 9
 *  #define OUT_X 80
 *  #define OUT_Y 59
 *
 *  q7_t in_data[IN_CH * IN_X * IN_Y] = {...};
 *  q7_t weight[IN_CH * KER_DIM_X * KER_DIM_Y * OUT_CH] = {...};
 *  q7_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * IN_CH * KER_DIM_X * KER_DIM_Y] = {0};
 *  q7_t out_data[OUT_CH * OUT_Y * OUT_X];
 *
 *  riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast_any(in_data, IN_W, IN_Y , IN_CH,
 *      weight, OUT_CH, KER_DIM_X, KER_DIM_Y, PAD_X, PAD_Y, STRIDE_X, STRIDE_Y,
 *      bias, BIAS_LSHIFT, OUT_RSHIFT, out_data, OUT_X, OUT_Y, in_tmp_buf,
 *      NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sft_bias_fast_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf,
                                                q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, tmp_buf);
}


/**
 * @brief           This function performs signed 16-bit integer convolution
 *                  with shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "in_tensor_ch * ker_dim * ker_dim".
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 *  //Convolve a 28x28x1 input tensor with a 5x5 kernel and generate a 24x24x20
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 28
 *  #define IN_CH 1
 *  #define KER_DIM 5
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 10
 *  #define OUT_CH 20
 *  #define OUT_DIM 24
 *
 *  q15_t input_data[IN_CH * IN_DIM * IN_DIM] = {...};
 *  q15_t weight[IN_CH * KER_DIM * KER_DIM * OUT_CH] = {...};
 *  q15_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[IN_CH * KER_DIM * KER_DIM] = {0};
 *  q15_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_HWC_s16_s16_s16_sft_bias(input_data, IN_DIM, IN_CH, weight,
 *      OUT_CH, KER_DIM, PAD, STRIDE, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data,
 *      OUT_DIM, in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s16_s16_s16_sft_bias(const q15_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q15_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q15_t *bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s16_s16_s16_sft_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs fast signed 16-bit integer
 *                  convolution with shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @param[in]       tmp_buf         dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that both
 *                  in_tensor_ch and out_tensor_ch are multiple of 2.
 *
 * @b Example:
 * @code
 *  //Convolve a 28x28x4 input tensor with a 5x5 kernel and generate a 24x24x8
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 28
 *  #define IN_CH 4
 *  #define KER_DIM 5
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 10
 *  #define OUT_CH 8
 *  #define OUT_DIM 24
 *
 *  q15_t in_data[IN_CH * IN_DIM * IN_DIM] = {...};
 *  q15_t weight[IN_CH * KER_DIM * KER_DIM * OUT_CH] = {...};
 *  q15_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[IN_CH * KER_DIM * KER_DIM] = {0};
 *  q15_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast(in_data, IN_DIM, IN_CH, weight,
 *      OUT_CH, KER_DIM, PAD, STRIDE, bias, BIAS_LSHIFT, OUT_RSHIFT, out_data,
 *      OUT_DIM, in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s16_s16_s16_sft_bias_fast(const q15_t *in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const uint16_t in_tensor_ch,
                                                const q15_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q15_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t *in_tmp_buf,
                                                q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs fast signed 16-bit integer
 *                  convolution in any x and y dimensions with shift-based
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       bias_lshift         left shift amount for the bias
 * @param[in]       out_rshift          right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that both
 *                  in_tensor_ch and out_tensor_ch are multiple of 2.
 *
 * @b Example:
 * @code
 *  //Convolve a 160x120x20 input tensor with a 3x5 kernel and generate a
 *  //80x59x8 output tensor. Let both dimensions padding be 1 and their stride
 *  //be 2.
 *
 *  #define IN_X 160
 *  #define IN_Y 120
 *  #define IN_CH 20
 *  #define OUT_CH 8
 *  #define KER_DIM_X 3
 *  #define KER_DIM_Y 5
 *  #define PAD_X 1
 *  #define PAD_Y 1
 *  #define STRIDE_X 2
 *  #define STRIDE_Y 2
 *  #define BIAS_LSHIFT 6
 *  #define OUT_RSHIFT 9
 *  #define OUT_X 80
 *  #define OUT_Y 59
 *
 *  q15_t in_data[IN_CH * IN_X * IN_Y] = {...};
 *  q15_t weight[IN_CH * KER_DIM_X * KER_DIM_Y * OUT_CH] = {...};
 *  q15_t bias[OUT_CH] = {...};
 *  q15_t in_tmp_buf[2 * IN_CH * KER_DIM_X  * KER_DIM_Y] = {0};
 *  q15_t out_data[OUT_CH * OUT_X * OUT_Y];
 *
 *  riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast_any(in_data, IN_X, IN_Y , IN_CH,
 *      weight, OUT_CH, KER_DIM_X, KER_DIM_Y, PAD_X, PAD_Y, STRIDE_X, STRIDE_Y,
 *      bias, BIAS_LSHIFT, OUT_RSHIFT, out_data, OUT_X, OUT_Y, in_tmp_buf,
 *      NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_HWC_s16_s16_s16_sft_bias_fast_any(const q15_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q15_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q15_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf,
                                                q7_t *tmp_buf)
{
    return riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs signed 8-bit integer depthwise
 *                  convolution with shift-based quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @param[in]       tmp_buf         dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @b Example:
 * @code
 *  //Convolve a 11x11x28 input tensor with a 3x3 kernel and generate a 9x9x48
 *  //output tensor. Let both dimensions padding be 0 and their stride be 1.
 *
 *  #define IN_DIM 11
 *  #define IN_CH 28
 *  #define OUT_CH 48
 *  #define KER_DIM 3
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define OUT_RSHIFT 7
 *  #define OUT_DIM 9
 *
 *  q7_t in_data[IN_CH * IN_DIM * IN_DIM] = {...};
 *  q7_t weight[IN_CH * KER_DIM * KER_DIM * IN_CH] = {...};
 *  q7_t bias[IN_CH] = {...};
 *  q15_t in_tmp_buf[2 * OUT_CH * KER_DIM * KER_DIM] = {0};
 *  q7_t out_data[OUT_CH * OUT_DIM * OUT_DIM];
 *
 *  riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias(in_data, IN_DIM, IN_CH, weight,
 *      OUT_CH, KER_DIM, PAD, STRIDE, bias, 0, OUT_RSHIFT, out_data, OUT_DIM,
 *      in_tmp_buf, NULL);
 * @endcode
 */

static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_sft_bias(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q7_t *bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q7_t *tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs signed 8-bit integer depthwise
 *                  convolution in any x and y dimensions with shift-based
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       bias_lshift         left shift amount for the bias
 * @param[in]       out_rshift          right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @b Example:
 * @code
 *  //Perform a depth-wise convolution for a 79x59x12 input tensor with a 3x3
 *  //kernel and generate a 77x57x12 output tensor. Let both dimensions padding
 *  //be 0 and their stride be 1.
 *
 *  #define IN_DIM_X 79
 *  #define IN_DIM_Y 59
 *  #define IN_CH 12
 *  #define OUT_CH 12
 *  #define KER_DIM 3
 *  #define PAD 0
 *  #define STRIDE 1
 *  #define BIAS_SHIFT 0
 *  #define OUT_RSHIFT 7
 *  #define OUT_DIM_X 77
 *  #define OUT_DIM_Y 57
 *
 *  q7_t in_data[IN_CH * IN_DIM_X * IN_DIM_Y] = {...};
 *  q7_t weight[IN_CH * KER_DIM * KER_DIM * IN_CH] = {...};
 *  q7_t bias[IN_CH] = {...};
 *  q15_t in_tmp_buf[2 * OUT_CH * KER_DIM * KER_DIM] = {0};
 *  q7_t out_data[OUT_CH * OUT_DIM_X * OUT_DIM_Y];
 *
 *  riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias_any(in_data, IN_DIM_X, IN_DIM_Y,
 *      IN_CH, weight, OUT_CH, KER_DIM, KER_DIM, PAD, PAD, STRIDE, STRIDE, bias,
 *      BIAS_SHIFT, OUT_RSHIFT, out_data, OUT_DIM_X, OUT_DIM_Y, in_tmp_buf,
 *      NULL);
 *  @endcode
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_sft_bias_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t *bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf,
                                                q7_t *tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, bias_lshift, out_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  bias inputs and symmetric quantization on the outputs..
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_sym_bias_fast_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s16_s8_sym_bias_fast_any(const q7_t *in_tensor,
                                                    const uint16_t in_tensor_dim_x,
                                                    const uint16_t in_tensor_dim_y,
                                                    const uint16_t in_tensor_ch,
                                                    const q7_t *ker_weight,
                                                    const uint16_t out_tensor_ch,
                                                    const uint16_t ker_dim_x,
                                                    const uint16_t ker_dim_y,
                                                    const uint16_t pad_x,
                                                    const uint16_t pad_y,
                                                    const uint16_t stride_x,
                                                    const uint16_t stride_y,
                                                    const q31_t *bias,
                                                    const uint16_t pre_rshift,
                                                    const uint16_t out_scale,
                                                    const uint16_t post_rshift,
                                                    q15_t *out_tensor,
                                                    const uint16_t out_tensor_dim_x,
                                                    const uint16_t out_tensor_dim_y,
                                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s16_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for unsigned
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_u8_u8_s8_sym_bias_fast_any(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                u8_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_u8_u8_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for unsigned
 *                  8-bit integer inputs and signed 8-bit integer outputs in any
 *                  x and y dimensions with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_u8_s8_s8_sym_bias_fast_any(const u8_t *in_tensor,
                                                    const uint16_t in_tensor_dim_x,
                                                    const uint16_t in_tensor_dim_y,
                                                    const uint16_t in_tensor_ch,
                                                    const q7_t *ker_weight,
                                                    const uint16_t out_tensor_ch,
                                                    const uint16_t ker_dim_x,
                                                    const uint16_t ker_dim_y,
                                                    const uint16_t pad_x,
                                                    const uint16_t pad_y,
                                                    const uint16_t stride_x,
                                                    const uint16_t stride_y,
                                                    const q31_t *bias,
                                                    const uint16_t pre_rshift,
                                                    const uint16_t out_scale,
                                                    const uint16_t post_rshift,
                                                    q7_t *out_tensor,
                                                    const uint16_t out_tensor_dim_x,
                                                    const uint16_t out_tensor_dim_y,
                                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_u8_s8_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for unsigned
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_u8_s16_s8_sym_bias_fast_any(const u8_t *in_tensor,
                                                    const uint16_t in_tensor_dim_x,
                                                    const uint16_t in_tensor_dim_y,
                                                    const uint16_t in_tensor_ch,
                                                    const q7_t *ker_weight,
                                                    const uint16_t out_tensor_ch,
                                                    const uint16_t ker_dim_x,
                                                    const uint16_t ker_dim_y,
                                                    const uint16_t pad_x,
                                                    const uint16_t pad_y,
                                                    const uint16_t stride_x,
                                                    const uint16_t stride_y,
                                                    const q31_t *bias,
                                                    const uint16_t pre_rshift,
                                                    const uint16_t out_scale,
                                                    const uint16_t post_rshift,
                                                    q15_t *out_tensor,
                                                    const uint16_t out_tensor_dim_x,
                                                    const uint16_t out_tensor_dim_y,
                                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_u8_s16_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to 2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_sym_fast_any(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to 2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s16_s8_sym_fast_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s16_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for unsigned
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_u8_u8_s8_sym_fast_any(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_u8_u8_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for unsigned
 *                  8-bit integer inputs and signed 8-bit integer outputs in any
 *                  x and y dimensions with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to 2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_u8_s8_s8_sym_fast_any(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_u8_s8_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for unsigned
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "2 * in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 * - The outputs will be 2-stage shifted before being stored, i.e.,
 *   out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_u8_s16_s8_sym_fast_any(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_u8_s16_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  signed 8-bit integer inputs/outputs with bias inputs and
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_RGB_sym_bias_fast(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_RGB_sym_bias_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  signed 8-bit integer inputs and signed 16-bit integer
 *                  outputs with bias inputs and symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s16_s8_RGB_sym_bias_fast(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t *in_tmp_buf,
                                                q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s16_s8_RGB_sym_bias_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  unsigned 8-bit integer inputs/outputs with symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_u8_s8_RGB_sym_bias_fast(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_u8_s8_RGB_sym_bias_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  signed 8-bit integer inputs/outputs with bias inputs and
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s8_s8_RGB_sym_bias_fast(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t *in_tmp_buf,
                                                q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s8_s8_RGB_sym_bias_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  unsigned 8-bit integer inputs and signed 16-bit integer
 *                  outputs with bias inputs and symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s16_s8_RGB_sym_bias_fast(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t *in_tmp_buf,
                                                q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s16_s8_RGB_sym_bias_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  signed 8-bit integer inputs/outputs with symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_RGB_sym_fast(const q7_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf,
                                        q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_RGB_sym_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  signed 8-bit integer inputs and signed 16-bit integer
 *                  outputs with symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s16_s8_RGB_sym_fast(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s16_s8_RGB_sym_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  unsigned 8-bit integer inputs/outputs with symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_u8_s8_RGB_sym_fast(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf,
                                        q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_u8_s8_RGB_sym_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  unsigned 8-bit integer inputs and signed 8-bit integer
 *                  outputs with symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s8_s8_RGB_sym_fast(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s8_s8_RGB_sym_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution on RGB images for
 *                  unsigned 8-bit integer inputs and signed 16-bit integer
 *                  outputs with symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   input tensor dimension
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "2 * (3 *
 *                                  ker_dim * ker_dim + 1)".
 * @param[in]       wt_tmp_buf      temporary buffer for kernel weights. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be "out_tensor_ch *
 *                                  (3 * ker_dim * ker_dim + 1)".
 * @return          This function only returns 0.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s16_s8_RGB_sym_fast(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf,
                                            q15_t *wt_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s16_s8_RGB_sym_fast(in_tensor, in_tensor_dim, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf, wt_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs/outputs with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sym_bias_fast(const q7_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t *bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sym_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs and signed 16-bit integer outputs with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s16_s8_sym_bias_fast(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s16_s8_sym_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs/outputs with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_u8_s8_sym_bias_fast(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t *bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_u8_s8_sym_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s8_s8_sym_bias_fast(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s8_s8_sym_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s16_s8_sym_bias_fast(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s16_s8_sym_bias_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs/outputs with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sym_fast(const q7_t *in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t *out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sym_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs and signed 16-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s16_s8_sym_fast(const q7_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q15_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s16_s8_sym_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs/outputs with symmetric quantization on
 *                  the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_u8_s8_sym_fast(const u8_t *in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    u8_t *out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_u8_s8_sym_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s8_s8_sym_fast(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s8_s8_sym_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input vector
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector
 *                                  enabled and its size must be equal to "2 *
 *                                  in_tensor_ch * ker_dim * ker_dim".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s16_s8_sym_fast(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q15_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s16_s8_sym_fast(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs/outputs in any x and y dimensions with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sym_bias_fast_any(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs and signed 16-bit integer outputs in any x
 *                  and y dimensions with bias inputs and symmetric quantization
 *                  on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s16_s8_sym_bias_fast_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s16_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs/outputs in any x and y dimensions with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_u8_s8_sym_bias_fast_any(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_u8_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs in any x and
 *                  y dimensions with bias inputs and symmetric quantization on
 *                  the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s8_s8_sym_bias_fast_any(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s8_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs in any x
 *                  and y dimensions with bias inputs and symmetric quantization
 *                  on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s16_s8_sym_bias_fast_any(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s16_s8_sym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs/outputs in any x and y dimensions with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_sym_fast_any(const q7_t *in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y,
                                        const uint16_t pad_x,
                                        const uint16_t pad_y,
                                        const uint16_t stride_x,
                                        const uint16_t stride_y,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for signed 8-bit
 *                  integer inputs and signed 16-bit integer outputs in any x
 *                  and y dimensions with symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s16_s8_sym_fast_any(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s16_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs/outputs in any x and y dimensions with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_u8_s8_sym_fast_any(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y,
                                        const uint16_t pad_x,
                                        const uint16_t pad_y,
                                        const uint16_t stride_x,
                                        const uint16_t stride_y,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t *out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_u8_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs in any x and
 *                  y dimensions with symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s8_s8_sym_fast_any(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s8_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs fast convolution for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs in any x
 *                  and y dimensions with symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input vector
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector enabled and its size must
 *                                      be equal to "2 * in_tensor_ch * ker_dim_x
 *                                      * ker_dim_y".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  is a multiple of 4 and out_tensor_ch is a multiple of 2.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_HWC_u8_s16_s8_sym_fast_any(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_u8_s16_s8_sym_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}


/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs/outputs with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_sym_bias(const q7_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t *bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_sym_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs and signed 16-bit integer outputs with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s16_s8_sym_bias(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s16_s8_sym_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs/outputs with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_u8_s8_sym_bias(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t *bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_u8_s8_sym_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 8-bit integer outputs with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s8_s8_sym_bias(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t *bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s8_s8_sym_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 16-bit integer outputs with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s16_s8_sym_bias(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s16_s8_sym_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs/outputs with symmetric quantization on
 *                  the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_sym(const q7_t *in_tensor,
                                const uint16_t in_tensor_dim,
                                const uint16_t in_tensor_ch,
                                const q7_t *ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ker_dim,
                                const uint16_t pad,
                                const uint16_t stride,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t *out_tensor,
                                const uint16_t out_tensor_dim,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_sym(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs and signed 16-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s16_s8_sym(const q7_t *in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t *out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s16_s8_sym(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs/outputs with symmetric quantization on
 *                  the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  (in_tensor_ch * ker_dim * ker_dim + 1) / 2.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_u8_s8_sym(const u8_t *in_tensor,
                                const uint16_t in_tensor_dim,
                                const uint16_t in_tensor_ch,
                                const q7_t *ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ker_dim,
                                const uint16_t pad,
                                const uint16_t stride,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                u8_t *out_tensor,
                                const uint16_t out_tensor_dim,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_u8_s8_sym(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 8-bit integer outputs, and
 *                  with symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s8_s8_sym(const u8_t *in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t *out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s8_s8_sym(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 16-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_weight      pointer of kernel weights
 * @param[in]       out_tensor_ch   number of output tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor. It is
 *                                  required when -mext-dsp or -mext-vector is
 *                                  enabled and its size must be equal to
 *                                  "(in_tensor_ch * ker_dim * ker_dim + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s16_s8_sym(const u8_t *in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t *out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s16_s8_sym(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_sym_bias_any(const q7_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_sym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s16_s8_sym_bias_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s16_s8_sym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_u8_s8_sym_bias_any(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_u8_s8_sym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 8-bit integer outputs in any
 *                  x and y dimensions with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s8_s8_sym_bias_any(const u8_t *in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t *bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t *out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s8_s8_sym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s16_s8_sym_bias_any(const u8_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t *bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s16_s8_sym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  bias inputs and symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_sym_any(const q7_t *in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim_x,
                                    const uint16_t ker_dim_y,
                                    const uint16_t pad_x,
                                    const uint16_t pad_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t *out_tensor,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_sym_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s16_s8_sym_any(const q7_t *in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y,
                                        const uint16_t pad_x,
                                        const uint16_t pad_y,
                                        const uint16_t stride_x,
                                        const uint16_t stride_y,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q15_t *out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s16_s8_sym_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_u8_s8_sym_any(const u8_t *in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim_x,
                                    const uint16_t ker_dim_y,
                                    const uint16_t pad_x,
                                    const uint16_t pad_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    u8_t *out_tensor,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_u8_s8_sym_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 8-bit integer outputs in any
 *                  x and y dimensions with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s8_s8_sym_any(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y,
                                        const uint16_t pad_x,
                                        const uint16_t pad_y,
                                        const uint16_t stride_x,
                                        const uint16_t stride_y,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t *out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s8_s8_sym_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs and signed 16-bit integer outputs in
 *                  any x and y dimensions with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       pre_rshift          right shift amount for the output
 * @param[in]       out_scale           value of scaling for the output
 * @param[in]       post_rshift         right shift amount for the output
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its size
 *                                      must be equal to "(in_tensor_ch *
 *                                      ker_dim_x * ker_dim_y + 1) / 2".
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  must be equal to out_tensor_ch.
 *
 * @note
 *  The outputs will be 2-stage shifted before being stored, i.e.,
 *  out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_s16_s8_sym_any(const u8_t *in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y,
                                        const uint16_t pad_x,
                                        const uint16_t pad_y,
                                        const uint16_t stride_x,
                                        const uint16_t stride_y,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q15_t *out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_u8_s16_s8_sym_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, pre_rshift, out_scale, post_rshift, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit interger inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor_group     number of input tensor groups
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const uint16_t in_tensor_group,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t pad_x,
                                    const uint16_t pad_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const int32_t *bias,
                                    q7_t *out_tensor,
                                    const int32_t *out_shift,
                                    const int32_t *out_scale,
                                    const int32_t out_offset,
                                    const int32_t in_offset,
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t *tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, in_tensor_group, ker_weight, out_tensor_ch, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_offset, in_offset, act_min, act_max, out_tensor_dim_x, out_tensor_dim_y, tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(in_tensor_ch);
}

/**
 * @brief           This function performs 1xn kernels convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor_group     dummy
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its needed
 *                                      size could be get by calling riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraint that
 *                  out_tensor_dim_x is a multiple of 4.
 */
static inline int hpm_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_ch,
                                                const uint16_t in_tensor_group,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t pad_x,
                                                const uint16_t stride_x,
                                                const int32_t *bias,
                                                q7_t *out_tensor,
                                                const int32_t *out_shift,
                                                const int32_t *out_scale,
                                                const int32_t out_offset,
                                                const int32_t in_offset,
                                                const int32_t act_min,
                                                const int32_t act_max,
                                                const uint16_t out_tensor_dim_x,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_ch, in_tensor_group, ker_weight, out_tensor_ch, ker_dim_x, pad_x, stride_x, bias, out_tensor, out_shift, out_scale, out_offset, in_offset, act_min, act_max, out_tensor_dim_x, in_tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel. It is
 *                                      always 1 here.
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y)
{
    return riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(in_tensor_ch, ker_dim_x, ker_dim_y);
}

/**
 * @brief           This function performs convolution for signed 8-bit integer
 *                  inputs/outputs in any x and y dimensions with asymmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor_group     number of input tensor groups
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its needed
 *                                      size could be get by calling riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size.
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const uint16_t in_tensor_group,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const int32_t *bias,
                                                q7_t *out_tensor,
                                                const int32_t *out_shift,
                                                const int32_t *out_scale,
                                                const int32_t out_offset,
                                                const int32_t in_offset,
                                                const int32_t act_min,
                                                const int32_t act_max,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, in_tensor_group, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_offset, in_offset, act_min, act_max, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y)
{
    return riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(in_tensor_ch, ker_dim_x, ker_dim_y);
}

/**
 * @brief           This function performs depthwise 3x3 kernels convolution for
 *                  signed 8-bit integer inputs/outputs in any x and y
 *                  dimensions with asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       dilation_x          dummy
 * @param[in]       dilation_y          dummy
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints that in_tensor_ch
 *                  has to be equal to out_tensor_ch and pad_x is less than 1.
 */
static inline int32_t hpm_nn_conv_dw_HWC_3x3_s8_s8_s8_asym_bias_any(const int8_t *in_tensor,
                                                const int32_t in_tensor_dim_x,
                                                const int32_t in_tensor_dim_y,
                                                const int32_t in_tensor_ch,
                                                const int8_t *ker_weight,
                                                const int32_t out_tensor_ch,
                                                const int32_t pad_x,
                                                const int32_t pad_y,
                                                const int32_t stride_x,
                                                const int32_t stride_y,
                                                const int32_t *bias,
                                                int8_t *out_tensor,
                                                const int32_t *out_shift,
                                                const int32_t *out_scale,
                                                const int32_t out_tensor_dim_x,
                                                const int32_t out_tensor_dim_y,
                                                const int32_t out_offset,
                                                const int32_t in_offset,
                                                const int32_t act_min,
                                                const int32_t act_max,
                                                const int32_t dilation_x,
                                                const int32_t dilation_y,
                                                int16_t *tmp_buf)
{
    return riscv_nn_conv_dw_HWC_3x3_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max, dilation_x, dilation_y, tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit interger inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels.
 *                                      out_tensor_ch is equal to ch_mult *
 *                                      in_tensor_ch.
 * @param[in]       ch_mult             multiplier of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       dilation_x          dummy
 * @param[in]       dilation_y          dummy
 * @param[in]       tmp_buf             dummy
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 *  to be modified...
 * @endcode
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                                const uint16_t in_tensor_dim_x,
                                const uint16_t in_tensor_dim_y,
                                const uint16_t in_tensor_ch,
                                const q7_t *ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ch_mult,
                                const uint16_t ker_dim_x,
                                const uint16_t ker_dim_y,
                                const uint16_t pad_x,
                                const uint16_t pad_y,
                                const uint16_t stride_x,
                                const uint16_t stride_y,
                                const int32_t *bias,
                                q7_t *out_tensor,
                                const int32_t *out_shift,
                                const int32_t *out_scale,
                                const uint16_t out_tensor_dim_x,
                                const uint16_t out_tensor_dim_y,
                                const int32_t out_offset,
                                const int32_t in_offset,
                                const int32_t act_min,
                                const int32_t act_max,
                                const uint16_t dilation_x,
                                const uint16_t dilation_y,
                                q15_t *tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ch_mult, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max, dilation_x, dilation_y, tmp_buf);
}

/**
 * @brief           This function performs fast depthwise convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       dilation_x          dummy
 * @param[in]       dilation_y          dummy
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its needed
 *                                      size could be get by calling riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraint that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
                                     const uint16_t in_tensor_dim_x,
                                     const uint16_t in_tensor_dim_y,
                                     const uint16_t in_tensor_ch,
                                     const q7_t *ker_weight,
                                     const uint16_t out_tensor_ch,
                                     const uint16_t ker_dim_x,
                                     const uint16_t ker_dim_y,
                                     const uint16_t pad_x,
                                     const uint16_t pad_y,
                                     const uint16_t stride_x,
                                     const uint16_t stride_y,
                                     const int32_t *bias,
                                     q7_t *out_tensor,
                                     const int32_t *out_shift,
                                     const int32_t *out_scale,
                                     const uint16_t out_tensor_dim_x,
                                     const uint16_t out_tensor_dim_y,
                                     const int32_t out_offset,
                                     const int32_t in_offset,
                                     const int32_t act_min,
                                     const int32_t act_max,
                                     const uint16_t dilation_x,
                                     const uint16_t dilation_y,
                                     q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max, dilation_x, dilation_y, in_tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                  const uint16_t ker_dim_x,
                                                  const uint16_t ker_dim_y)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(in_tensor_ch, ker_dim_x, ker_dim_y);
}

/**
 * @brief           This function performs depthwise convolution for unsigned
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       ch_mult             multiplier of input tensor channels
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       dilation_x          dummy
 * @param[in]       dilation_y          dummy
 * @param[in]       bias                pointer of the bias vector
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -255 to 0.
 * @param[in]       ker_offset          value of offset for the filter kernel
 *                                      It should be in the range of -255 to 0.
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of 0 to 255.
 * @param[in]       out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      0 to 255.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      0 to 255.
 * @param[in]       out_shift           shift amount for the output tensor
 * @param[in]       out_scale           value of sacling for the output tensor
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraint that both ch_mult
 *                  and ker_dim_x are multiple of 2.
 */
static inline int32_t hpm_nn_conv_dw_HWC_u8_u8_u8_asym_bias_any(const uint8_t *in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const uint8_t *ker_weight,
                                    const uint16_t ker_dim_x,
                                    const uint16_t ker_dim_y,
                                    const int16_t ch_mult,
                                    const int16_t pad_x,
                                    const int16_t pad_y,
                                    const int16_t stride_x,
                                    const int16_t stride_y,
                                    const int16_t dilation_x,
                                    const int16_t dilation_y,
                                    const int32_t *bias,
                                    const int32_t in_offset,
                                    const int32_t ker_offset,
                                    const int32_t out_offset,
                                    uint8_t *out_tensor,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    const int32_t out_shift,
                                    const int32_t out_scale)
{
    return riscv_nn_conv_dw_HWC_u8_u8_u8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, ker_dim_x, ker_dim_y, ch_mult, pad_x, pad_y, stride_x, stride_y, dilation_x, dilation_y, bias, in_offset, ker_offset, out_offset, out_tensor, out_tensor_dim_x, out_tensor_dim_y, act_min, act_max, out_shift, out_scale);
}

#ifdef __riscv_zfh
/**
 * @brief           This function performs 1x1 kernels convolution for 16-bit
 *                  half-precision floating point inputs/outputs in any x and y
 *                  dimensions.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          dummy
 * @param[in]       tmp_buf             dummy
 * @return          This function only returns 0.
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - out_tensor_ch is a multiple of 2
 *     - ker_dim_x is 1
 *     - ker_dim_y is 1
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 */
static inline int32_t hpm_nn_conv_1x1_HWC_f16_f16_f16_bias_any(const float16_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const float16_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const float16_t *bias,
                                                float16_t *out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                float16_t *in_tmp_buf,
                                                float16_t *tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_f16_f16_f16_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs convolution for 16-bit half-precision
 *                  floating point inputs/outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim       dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim             dimension of the filter kernel
 * @param[in]       pad                 padding size
 * @param[in]       stride              convolution stride
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim      dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-vector is
 *                                      enabled and its size must be equal to
 *                                      "2 * in_tensor_ch * ker_dim * ker_dim".
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0.
 */
static inline int32_t hpm_nn_conv_HWC_f16_f16_f16_bias(const float16_t *in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const float16_t *ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const float16_t *bias,
                                        float16_t *out_tensor,
                                        const uint16_t out_tensor_dim,
                                        float16_t *in_tmp_buf,
                                        float16_t *tmp_buf)
{
    return riscv_nn_conv_HWC_f16_f16_f16_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for 16-bit
 *                  half-precision floating point inputs/outputs
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim       dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim             dimension of the filter kernel
 * @param[in]       pad                 padding size
 * @param[in]       stride              convolution stride
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_dim      dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-vector is
 *                                      enabled and its size must be equal to
 *                                      "in_tensor_ch * ker_dim * ker_dim".
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0.
 */
static inline int32_t hpm_nn_conv_dw_HWC_f16_f16_f16_bias(const float16_t *in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const float16_t *ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const float16_t *bias,
                                            float16_t *out_tensor,
                                            const uint16_t out_tensor_dim,
                                            float16_t *in_tmp_buf,
                                            float16_t *tmp_buf)
{
    return riscv_nn_conv_dw_HWC_f16_f16_f16_bias(in_tensor, in_tensor_dim, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim, pad, stride, bias, out_tensor, out_tensor_dim, in_tmp_buf, tmp_buf);
}
#endif

/**
 *   * @}
 */

#endif

#ifdef HPM_EN_MATH_NN_RVP32_LIB

#include "riscv_nn_convolution.h"
/**
 * @brief           This function performs convolution for signed 8-bit integer
 *                  inputs/outputs in any x and y dimensions with asymmetric
 *                  quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor_group     number of input tensor groups
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its needed
 *                                      size could be get by calling riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size.
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const uint16_t in_tensor_group,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const int32_t *bias,
                                                q7_t *out_tensor,
                                                const int32_t *out_shift,
                                                const int32_t *out_scale,
                                                const int32_t out_offset,
                                                const int32_t in_offset,
                                                const int32_t act_min,
                                                const int32_t act_max,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, in_tensor_group, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_offset, in_offset, act_min, act_max, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf);
}

/**
 * @brief           This function performs 1x1 kernels convolution for signed
 *                  8-bit interger inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor_group     number of input tensor groups
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       tmp_buf             dummy
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraints (see the Note
 *                  below for details).
 *
 * @note
 * - The input constraints of this function are:
 *     - in_tensor_ch is a multiple of 4
 *     - pad_x is 0
 *     - pad_y is 0
 *     - stride_x is 1
 *     - stride_y is 1
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const uint16_t in_tensor_group,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t pad_x,
                                    const uint16_t pad_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const int32_t *bias,
                                    q7_t *out_tensor,
                                    const int32_t *out_shift,
                                    const int32_t *out_scale,
                                    const int32_t out_offset,
                                    const int32_t in_offset,
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t *tmp_buf)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, in_tensor_group, ker_weight, out_tensor_ch, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_offset, in_offset, act_min, act_max, out_tensor_dim_x, out_tensor_dim_y, tmp_buf);
}

/**
 * @brief           This function performs depthwise convolution for signed
 *                  8-bit interger inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels.
 *                                      out_tensor_ch is equal to ch_mult *
 *                                      in_tensor_ch.
 * @param[in]       ch_mult             multiplier of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       dilation_x          dummy
 * @param[in]       dilation_y          dummy
 * @param[in]       tmp_buf             dummy
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 *  to be modified...
 * @endcode
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                                const uint16_t in_tensor_dim_x,
                                const uint16_t in_tensor_dim_y,
                                const uint16_t in_tensor_ch,
                                const q7_t *ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ch_mult,
                                const uint16_t ker_dim_x,
                                const uint16_t ker_dim_y,
                                const uint16_t pad_x,
                                const uint16_t pad_y,
                                const uint16_t stride_x,
                                const uint16_t stride_y,
                                const int32_t *bias,
                                q7_t *out_tensor,
                                const int32_t *out_shift,
                                const int32_t *out_scale,
                                const uint16_t out_tensor_dim_x,
                                const uint16_t out_tensor_dim_y,
                                const int32_t out_offset,
                                const int32_t in_offset,
                                const int32_t act_min,
                                const int32_t act_max,
                                const uint16_t dilation_x,
                                const uint16_t dilation_y,
                                q15_t *tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ch_mult, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max, dilation_x, dilation_y, tmp_buf);
}

/**
 * @brief           This function performs 1xn kernels convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor_group     dummy
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its needed
 *                                      size could be get by calling riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraint that
 *                  out_tensor_dim_x is a multiple of 4.
 */
static inline int hpm_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_ch,
                                                const uint16_t in_tensor_group,
                                                const q7_t *ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t pad_x,
                                                const uint16_t stride_x,
                                                const int32_t *bias,
                                                q7_t *out_tensor,
                                                const int32_t *out_shift,
                                                const int32_t *out_scale,
                                                const int32_t out_offset,
                                                const int32_t in_offset,
                                                const int32_t act_min,
                                                const int32_t act_max,
                                                const uint16_t out_tensor_dim_x,
                                                q15_t *in_tmp_buf)
{
    return riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(in_tensor, in_tensor_dim_x, in_tensor_ch, in_tensor_group, ker_weight, out_tensor_ch, ker_dim_x, pad_x, stride_x, bias, out_tensor, out_shift, out_scale, out_offset, in_offset, act_min, act_max, out_tensor_dim_x, in_tmp_buf);
}

/**
 * @brief           This function performs fast depthwise convolution for signed
 *                  8-bit integer inputs/outputs in any x and y dimensions with
 *                  asymmetric quantization on the outputs.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_weight          pointer of kernel weights
 * @param[in]       out_tensor_ch       number of output tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       bias                pointer of the bias vector
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_shift           pointer of the shift vector for output
 *                                      tensor
 * @param[in]       out_scale           pointer of the scaling vector for output
 *                                      tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_offset          value of offset for the output tensor.
 *                                      It should be in the range of -128 to 127.
 * @param[in]       in_offset           value of offset for the input tensor
 *                                      It should be in the range of -127 to 128.
 * @param[in]       act_min             minimum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value to clip out the ouput
 *                                      tensor. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       dilation_x          dummy
 * @param[in]       dilation_y          dummy
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp or
 *                                      -mext-vector is enabled and its needed
 *                                      size could be get by calling riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size.
 * @return          This function returns 0 on success; otherwise, it returns -1
 *                  if its inputs do not meet the constraint that in_tensor_ch
 *                  has to be equal to out_tensor_ch.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
                                     const uint16_t in_tensor_dim_x,
                                     const uint16_t in_tensor_dim_y,
                                     const uint16_t in_tensor_ch,
                                     const q7_t *ker_weight,
                                     const uint16_t out_tensor_ch,
                                     const uint16_t ker_dim_x,
                                     const uint16_t ker_dim_y,
                                     const uint16_t pad_x,
                                     const uint16_t pad_y,
                                     const uint16_t stride_x,
                                     const uint16_t stride_y,
                                     const int32_t *bias,
                                     q7_t *out_tensor,
                                     const int32_t *out_shift,
                                     const int32_t *out_scale,
                                     const uint16_t out_tensor_dim_x,
                                     const uint16_t out_tensor_dim_y,
                                     const int32_t out_offset,
                                     const int32_t in_offset,
                                     const int32_t act_min,
                                     const int32_t act_max,
                                     const uint16_t dilation_x,
                                     const uint16_t dilation_y,
                                     q15_t *in_tmp_buf)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias, out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max, dilation_x, dilation_y, in_tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch)
{
    return riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(in_tensor_ch);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                  const uint16_t ker_dim_x,
                                                  const uint16_t ker_dim_y)
{
    return riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(in_tensor_ch, ker_dim_x, ker_dim_y);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel. It is
 *                                      always 1 here.
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y)
{
    return riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(in_tensor_ch, ker_dim_x, ker_dim_y);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y)
{
    return riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(in_tensor_ch, ker_dim_x, ker_dim_y);
}

#endif

#endif

#ifdef HPM_MATH_NN_CONNECTED
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_fully_connected.h"

/**
 * @defgroup nnfullyconnect NN Fully Connected Functions
 * @ingroup hpmmath
 * @brief The fully connected functions multiply the input vector by a weight
 * matrix and add a bias, if any, to the result. The supported combinations of
 * input vector and weight matrix are (signed 8-bit integer, signed 8-bit integer),
 * (unsigned 8-bit integer, signed 8-bit integer), (signed 16-bit integer,
 * signed 8-bit integer), (signed 16-bit integer, signed 16-bit integer) and
 * (16-bit half-precision floating point, 16-bit half-precision floating point).
 *
 * @{
 */

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs with shift-based quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      dummy
 * @return          This function only returns 0.
 *
 * @b Example:
 * @code
 * #define IN_SIZE 2048
 * #define OUT_SIZE 256
 * #define BIAS_LSHIFT 9
 * #define OUT_RSHIFT 9
 *
 * q7_t in_vec[IN_SIZE] = {...};;
 * q7_t wt_mat[IN_SIZE * OUT_SIZE] {...};
 * q7_t bias[OUT_SIZE] = {...};
 * q7_t out_vec[OUT_SIZE];
 *
 * hpm_nn_fc_s8_s8_s8_sft_bias(in_vec, wt_mat, IN_SIZE, OUT_SIZE, BIAS_LSHIFT,
 *     OUT_RSHIFT, bias, out_vec, NULL);
 * @endcode
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_sft_bias(const q7_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t bias_lshift,
                                    const uint16_t out_rshift,
                                    const q7_t *bias,
                                    q7_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_sft_bias(in_vec, wt_mat, size, wt_row_num, bias_lshift, out_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs with interleaved multiplication and
 *                  shift-based quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be "2 * size".
 * @return          This function only returns 0.
 *
 * @note
 * In this function, the input vector is multiplied by the weight matrix in
 * interleaved formats which could be obtained by riscv_nn_fc_s8_wt_converter.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_sft_bias_fast(const q7_t *in_vec,
                                        const q7_t *wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t bias_lshift,
                                        const uint16_t out_rshift,
                                        const q7_t *bias,
                                        q7_t *out_vec,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_sft_bias_fast(in_vec, wt_mat, size, wt_row_num, bias_lshift, out_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 16-bit
 *                  integer inputs with shift-based quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[in]       bias            pointer of the bias
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_fc_s16_s16_s16_sft_bias(const q15_t *in_vec,
                                        const q15_t *wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t bias_lshift,
                                        const uint16_t out_rshift,
                                        const q15_t *bias,
                                        q15_t *out_vec,
                                        q15_t *tmp_buf)
{
    return riscv_nn_fc_s16_s16_s16_sft_bias(in_vec, wt_mat, size, wt_row_num, bias_lshift, out_rshift, bias, out_vec, tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 16-bit
 *                  integer inputs with interleaved multiplication and
 *                  shift-based quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[in]       bias            pointer of the bias
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 4 * size.
 * @return          This function only returns 0.
 *
 *
 * @note
 * In this function, the input vector is multiplied by a weight matrix in
 * interleaved formats which could be obtained by riscv_nn_fc_s16_wt_converter.
 */
static inline int32_t hpm_nn_fc_s16_s16_s16_sft_bias_fast(const q15_t *in_vec,
                                            const q15_t *wt_mat,
                                            const uint16_t size,
                                            const uint16_t wt_row_num,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            const q15_t *bias,
                                            q15_t *out_vec,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s16_s16_s16_sft_bias_fast(in_vec, wt_mat, size, wt_row_num, bias_lshift, out_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This function multiplies a signed 16-bit integer input
 *                  vector by a signed 8-bit integer weight matrix with
 *                  shift-based quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[in]       bias            pointer of the bias
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_fc_mat_vec_s16_s16_s8_sft_bias(const q15_t *in_vec,
                                                const q7_t *wt_mat,
                                                const uint16_t size,
                                                const uint16_t wt_row_num,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                const q7_t *bias,
                                                q15_t *out_vec,
                                                q15_t *tmp_buf)
{
    return riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias(in_vec, wt_mat, size, wt_row_num, bias_lshift, out_rshift, bias, out_vec, tmp_buf);
}

/**
 * @brief           This function multiplies a signed 16-bit integer input
 *                  vector by a signed 8-bit integer weight matrix with
 *                  interleaved multiplication and shift-based quantization on
 *                  the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias_lshift     left shift amount for the bias
 * @param[in]       out_rshift      right shift amount for the output
 * @param[in]       bias            pointer of the bias
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 *
 * @note
 * In this function, the input vector is multiplied by a weight matrix in
 * interleaved formats which could be obtained by
 * hpm_nn_fc_mat_vec_s8_wt_converter.
 */
static inline int32_t hpm_nn_fc_mat_vec_s16_s16_s8_sft_bias_fast(const q15_t *in_vec,
                                                    const q7_t *wt_mat,
                                                    const uint16_t size,
                                                    const uint16_t wt_row_num,
                                                    const uint16_t bias_lshift,
                                                    const uint16_t out_rshift,
                                                    const q7_t *bias,
                                                    q15_t *out_vec,
                                                    q15_t *tmp_buf)
{
    return riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias_fast(in_vec, wt_mat, size, wt_row_num, bias_lshift, out_rshift, bias, out_vec, tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs/outputs with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_sym_bias(const q7_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t *bias,
                                    q7_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_sym_bias(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs and signed 16-bit integer outputs with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s16_s8_sym_bias(const q7_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t *bias,
                                    q15_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s16_s8_sym_bias(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs/outputs with bias inputs and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_u8_s8_sym_bias(const u8_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t *bias,
                                    u8_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_u8_s8_sym_bias(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s8_s8_sym_bias(const u8_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t *bias,
                                    q7_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s8_s8_sym_bias(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs with bias
 *                  inputs and symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s16_s8_sym_bias(const u8_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t *bias,
                                    q15_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s16_s8_sym_bias(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs/outputs with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_sym(const q7_t *in_vec,
                                const q7_t *wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t *out_vec,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_sym(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs and signed 16-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s16_s8_sym(const q7_t *in_vec,
                                const q7_t *wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q15_t *out_vec,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s16_s8_sym(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs/outputs with symmetric quantization on the
 *                  outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_u8_s8_sym(const u8_t *in_vec,
                                const q7_t *wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                u8_t *out_vec,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_u8_s8_sym(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s8_s8_sym(const u8_t *in_vec,
                                const q7_t *wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t *out_vec,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s8_s8_sym(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs with
 *                  symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output before the
 *                                  scaling
 * @param[in]       out_scale       scaling value for the output
 * @param[in]       post_rshift     right shift amount for the output after the
 *                                  scaling
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-dsp is enabled and its
 *                                  size must be "size".
 * @return          This function only returns 0.
 *
 * @note
 * The outputs will be two-stage shifted before being stored, i.e.,
 * out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s16_s8_sym(const u8_t *in_vec,
                                const q7_t *wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q15_t *out_vec,
                                q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s16_s8_sym(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs/outputs with bias inputs, interleaved
 *                  multiplication and symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_sym_bias_fast(const q7_t *in_vec,
                                        const q7_t *wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t *bias,
                                        q7_t *out_vec,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_sym_bias_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs and signed 16-bit integer  outputs with bias
 *                  inputs, interleaved multiplication and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s16_s8_sym_bias_fast(const q7_t *in_vec,
                                            const q7_t *wt_mat,
                                            const uint16_t size,
                                            const uint16_t wt_row_num,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            const q31_t *bias,
                                            q15_t *out_vec,
                                            q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s16_s8_sym_bias_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs/outputs with bias inputs, interleaved
 *                  multiplication and symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_u8_s8_sym_bias_fast(const u8_t *in_vec,
                                        const q7_t *wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t *bias,
                                        u8_t *out_vec,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_u8_s8_sym_bias_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs with bias
 *                  inputs, interleaved multiplication and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s8_s8_sym_bias_fast(const u8_t *in_vec,
                                        const q7_t *wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t *bias,
                                        q7_t *out_vec,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s8_s8_sym_bias_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs with bias
 *                  inputs, interleaved multiplication and symmetric
 *                  quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s16_s8_sym_bias_fast(const u8_t *in_vec,
                                        const q7_t *wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t *bias,
                                        q15_t *out_vec,
                                        q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s16_s8_sym_bias_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, bias, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs/outputs with interleaved multiplication and
 *                  symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_sym_fast(const q7_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_sym_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs and signed 16-bit integer  outputs with
 *                  interleaved multiplication and symmetric quantization on the
 *                  outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_s8_s16_s8_sym_fast(const q7_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_s8_s16_s8_sym_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs/outputs with interleaved multiplication and
 *                  symmetric quantization on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_u8_s8_sym_fast(const u8_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    u8_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_u8_s8_sym_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 8-bit integer outputs with
 *                  interleaved multiplication and symmetric quantization on the
 *                  outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s8_s8_sym_fast(const u8_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s8_s8_sym_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a fully connected layer function for unsigned 8-bit
 *                  integer inputs and signed 16-bit integer outputs with
 *                  interleaved multiplication and symmetric quantization on the
 *                  outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       pre_rshift      right shift amount for the output
 * @param[in]       out_scale       value of scaling for the output
 * @param[in]       post_rshift     right shift amount for the output
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       in_tmp_buf      temporary buffer for input vector. It is
 *                                  required when -mext-vector is enabled and
 *                                  its size must be 2 * size.
 * @return          This function only returns 0.
 *
 * @note
 *  - In this function, the input vector is multiplied by the weight matrix in
 *    interleaved format which could be obtained by riscv_nn_fc_s8_wt_converter.
 *  - The outputs will be two-stage shifted before being stored, i.e.,
 *    out = ((out >> pre_rshift) *out_scale) >> post_rshift.
 */
static inline int32_t hpm_nn_fc_u8_s16_s8_sym_fast(const u8_t *in_vec,
                                    const q7_t *wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t *out_vec,
                                    q15_t *in_tmp_buf)
{
    return riscv_nn_fc_u8_s16_s8_sym_fast(in_vec, wt_mat, size, wt_row_num, pre_rshift, out_scale, post_rshift, out_vec, in_tmp_buf);
}

/**
 * @brief           This is a weight converter for those fully-connected
 *                  functions with signed 8-bit weight data and named with
 *                  "fast".
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[out]      wt_mat_out      pointer of the weight matrix stored in
 *                                  specific ordering
 */
static inline void hpm_nn_fc_s8_wt_converter(const q7_t *wt_mat,
                                const uint32_t size,
                                const uint32_t wt_row_num,
                                q7_t *wt_mat_out)
{
    riscv_nn_fc_s8_wt_converter(wt_mat, size, wt_row_num, wt_mat_out);
}

/**
 * @brief           This is a weight converter for those fully-connected
 *                  functions with signed 16-bit weight data and named with
 *                  "fast".
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[out]      wt_mat_out      pointer of the weight matrix stored in
 *                                  specific ordering
 */
static inline void hpm_nn_fc_s16_wt_converter(const q15_t *wt_mat,
                                const uint32_t size,
                                const uint32_t wt_row_num,
                                q15_t *wt_mat_out)
{
    riscv_nn_fc_s16_wt_converter(wt_mat, size, wt_row_num, wt_mat_out);
}

/**
 * @brief           This is a weight converter for
 *                  riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias_fast.
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[out]      wt_mat_out      pointer of the weight matrix stored in
 *                                  specific ordering
 */
static inline void hpm_nn_fc_mat_vec_s8_wt_converter(const q7_t *wt_mat,
                                        const uint32_t size,
                                        const uint32_t wt_row_num,
                                        q7_t *wt_mat_out)
{
    riscv_nn_fc_mat_vec_s8_wt_converter(wt_mat, size, wt_row_num, wt_mat_out);
}

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs with bias inputs and asymmetric quantization
 *                  on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the transposed weight matrix
 * @param[in]       in_vec_col      number of columns in the input vector (or
 *                                  transposed weight matrix)
 * @param[in]       wt_mat_row      number of rows in the transposed weight
 *                                  matrix
 * @param[in]       in_vec_group    number of input vector groups
 * @param[in]       in_offset       value of offset to be added to the input
 *                                  tensor. It should be in the range of -127 to
 *                                  128.
 * @param[in]       wt_offset       value of offset to be added to the weight.
 *                                  It should be in the range of -127 to 128.
 * @param[in]       out_scale       value of sacling for the output tensor
 * @param[in]       out_shift       shift amount for the output tensor
 * @param[in]       out_offset      value of offset to be added to the output
 *                                  tensor. It should be in the range of -128 to
 *                                  127.
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       out_vec         pointer of the output vector
 * @param[in]       act_min         minimum value to clip out the ouput tensor.
 *                                  It should be in the range of -128 to 127.
 * @param[in]       act_max         maximum value to clip out the ouput tensor.
 *                                  It should be in the range of -128 to 127.
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_asym_bias(const int8_t *in_vec,
                                    const int8_t *wt_mat,
                                    const uint16_t in_vec_col,
                                    const uint16_t wt_mat_row,
                                    const uint16_t in_vec_group,
                                    const int32_t in_offset,
                                    const int32_t wt_offset,
                                    const int32_t out_scale,
                                    const int32_t out_shift,
                                    const int32_t out_offset,
                                    const int32_t *bias,
                                    int8_t *out_vec,
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    q15_t *tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_asym_bias(in_vec, wt_mat, in_vec_col, wt_mat_row, in_vec_group, in_offset, wt_offset, out_scale, out_shift, out_offset, bias, out_vec, act_min, act_max, tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the temporary buffer of riscv_nn_fc_s8_s8_s8_asym_bias.
 * @param[in]       in_vec_col      number of columns in the input vector (or
 *                                  transposed weight matrix)
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_asym_bias_get_buffer_size(const uint16_t in_vec_col)
{
    return riscv_nn_fc_s8_s8_s8_asym_bias_get_buffer_size(in_vec_col);
}

/**
 *   * @}
 */

#endif

#ifdef HPM_EN_MATH_NN_RVP32_LIB
#include "riscv_nn_fully_connected.h"

/**
 * @brief           This is a fully connected layer function for signed 8-bit
 *                  integer inputs with bias inputs and asymmetric quantization
 *                  on the outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the transposed weight matrix
 * @param[in]       in_vec_col      number of columns in the input vector (or
 *                                  transposed weight matrix)
 * @param[in]       wt_mat_row      number of rows in the transposed weight
 *                                  matrix
 * @param[in]       in_vec_group    number of input vector groups
 * @param[in]       in_offset       value of offset to be added to the input
 *                                  tensor. It should be in the range of -127 to
 *                                  128.
 * @param[in]       wt_offset       value of offset to be added to the weight.
 *                                  It should be in the range of -127 to 128.
 * @param[in]       out_scale       value of sacling for the output tensor
 * @param[in]       out_shift       shift amount for the output tensor
 * @param[in]       out_offset      value of offset to be added to the output
 *                                  tensor. It should be in the range of -128 to
 *                                  127.
 * @param[in]       bias            pointer of the bias vector
 * @param[in]       out_vec         pointer of the output vector
 * @param[in]       act_min         minimum value to clip out the ouput tensor.
 *                                  It should be in the range of -128 to 127.
 * @param[in]       act_max         maximum value to clip out the ouput tensor.
 *                                  It should be in the range of -128 to 127.
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_asym_bias(const int8_t *in_vec,
                                    const int8_t *wt_mat,
                                    const uint16_t in_vec_col,
                                    const uint16_t wt_mat_row,
                                    const uint16_t in_vec_group,
                                    const int32_t in_offset,
                                    const int32_t wt_offset,
                                    const int32_t out_scale,
                                    const int32_t out_shift,
                                    const int32_t out_offset,
                                    const int32_t *bias,
                                    int8_t *out_vec,
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    q15_t *tmp_buf)
{
    return riscv_nn_fc_s8_s8_s8_asym_bias(in_vec, wt_mat, in_vec_col, wt_mat_row, in_vec_group, in_offset, wt_offset, out_scale, out_shift, out_offset, bias, out_vec, act_min, act_max, tmp_buf);
}

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the temporary buffer of riscv_nn_fc_s8_s8_s8_asym_bias.
 * @param[in]       in_vec_col      number of columns in the input vector (or
 *                                  transposed weight matrix)
 * @return          This function returns the needed size by the temporary buffer.
 */
static inline int32_t hpm_nn_fc_s8_s8_s8_asym_bias_get_buffer_size(const uint16_t in_vec_col)
{
    return riscv_nn_fc_s8_s8_s8_asym_bias_get_buffer_size(in_vec_col);
}

#endif /* HPM_EN_MATH_NN_RVP32_LIB */

#endif

#ifdef HPM_MATH_NN_POOLING
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_pooling.h"

/**
 * @defgroup nnpooling NN Pooling Functions
 * @ingroup hpmmath
 * @brief The pooling functions are used to downsample input data. They include
 * max and average pooling functions.
 *
 * @{
 */

/**
 * @brief           This is an average pooling function for signed 8-bit integer
 *                  inputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      temporary buffer for the input tensor
 * @param[out]      out_tensor      pointer of the output tensor. It is required
 *                                  when -mext-dsp is enabled and its size must
 *                                  be equal to "2 * out_tensor_dim *
 *                                  in_tensor_ch".
 *
 * @b Example:
 * @code
 * #define IN_DIM 32
 * #define IN_CH 32
 * #define KER_DIM 3
 * #define PAD 0
 * #define STRIDE 2
 * #define OUT_DIM 15
 *
 * q7_t in_data[IN_CH * IN_DIM * IN_DIM] = {...};
 * q7_t out_data[IN_CH * OUT_DIM * OUT_DIM] = {...};
 * q7_t in_tmp_buf[2 * OUT_DIM * IN_CH];
 *
 * hpm_nn_avepool_HWC_s8(in_data, IN_DIM, IN_CH, KER_DIM, PAD, STRIDE,
 *      OUT_DIM, in_tmp_buf, out_data);
 * @endcode
 */
static inline void hpm_nn_avepool_HWC_s8(q7_t *in_tensor,
                           const uint16_t in_tensor_dim,
                           const uint16_t in_tensor_ch,
                           const uint16_t ker_dim,
                           const uint16_t pad,
                           const uint16_t stride,
                           const uint16_t out_tensor_dim,
                           q7_t *in_tmp_buf,
                           q7_t *out_tensor)
{
    riscv_nn_avepool_HWC_s8(in_tensor, in_tensor_dim, in_tensor_ch, ker_dim, pad, stride, out_tensor_dim, in_tmp_buf, out_tensor);
}

/**
 * @brief           This is an average pooling function for signed 8-bit integer
 *                  inputs in any x and y dimensions.
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      It is required when -mext-dsp is enabled
 *                                      and its size must be equal to "2 *
 *                                      out_tensor_dim_x * in_tensor_ch".
 * @param[out]      out_tensor          pointer of the output tensor
 * @param[in]       out_lshift          left shift amount for the output
 *
 * @b Example:
 * @code
 *  #define IN_X 160
 *  #define IN_Y 120
 *  #define IN_CH 3
 *  #define KER_DIM_X 3
 *  #define KER_DIM_Y 5
 *  #define PAD_X 1
 *  #define PAD_Y 1
 *  #define STRIDE_X 2
 *  #define STRIDE_Y 2
 *  #define OUT_LSHIFT 3
 *  #define OUT_X 80
 *  #define OUT_Y 59
 *
 * q7_t in_data[IN_CH * IN_X * IN_Y] = {...};
 * q7_t out_data[IN_CH * OUT_X * OUT_Y] = {...};
 * q7_t in_tmp_buf[2 * IN_CH * OUT_X * OUT_Y];
 *
 * hpm_nn_avepool_HWC_s8_any(in_data, IN_X, IN_Y, IN_CH, KER_DIM_X, KER_DIM_Y,
 *      PAD_X, PAD_Y, STRIDE_X, STRIDE_Y, OUT_X, OUT_Y, in_tmp_buf, out_data,
 *      OUT_LSHIFT);
 * @endcode
 */
static inline void hpm_nn_avepool_HWC_s8_any(q7_t *in_tensor,
                               const uint16_t in_tensor_dim_x,
                               const uint16_t in_tensor_dim_y,
                               const uint16_t in_tensor_ch,
                               const uint16_t ker_dim_x,
                               const uint16_t ker_dim_y,
                               const uint16_t pad_x,
                               const uint16_t pad_y,
                               const uint16_t stride_x,
                               const uint16_t stride_y,
                               const uint16_t out_tensor_dim_x,
                               const uint16_t out_tensor_dim_y,
                               q7_t *in_tmp_buf,
                               q7_t *out_tensor,
                               const uint16_t out_lshift)
{
    riscv_nn_avepool_HWC_s8_any(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, out_tensor_dim_x, out_tensor_dim_y, in_tmp_buf, out_tensor, out_lshift);
}

/**
 * @brief           This is an average pooling function for S8 inputs with any x
 *                  and y dimension with the actvating parameters to limit the
 *                  outputs.
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       act_min             minimum value that the output tensor is
 *                                      limited to. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value that the output tensor is
 *                                      limited to. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      Its needed size could be obtained by
 *                                      calling riscv_nn_avepool_s8_HWC_any_get_buffer_size.
 * @param[out]      out_tensor          pointer of the output tensor
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_avepool_HWC_s8_any_act(const int in_tensor_dim_y,
                                const int in_tensor_dim_x,
                                const int out_tensor_dim_y,
                                const int out_tensor_dim_x,
                                const int stride_y,
                                const int stride_x,
                                const int ker_dim_y,
                                const int ker_dim_x,
                                const int pad_y,
                                const int pad_x,
                                const int act_min,
                                const int act_max,
                                const int in_tensor_ch,
                                int8_t *in_tensor,
                                int16_t *in_tmp_buf,
                                int8_t *out_tensor)
{
    return riscv_nn_avepool_HWC_s8_any_act(in_tensor_dim_y, in_tensor_dim_x, out_tensor_dim_y, out_tensor_dim_x, stride_y, stride_x, ker_dim_y, ker_dim_x, pad_y, pad_x, act_min, act_max, in_tensor_ch, in_tensor, in_tmp_buf, out_tensor);
}

/**
 * @brief           This function is used to obtain the required size, in bytes,
 *                  for the input temporary buffer of riscv_nn_avepool_HWC_s8_any_act.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @return          This function returns the size required by the temporary
 *                  buffer.
 */
static inline int32_t hpm_nn_avepool_HWC_s8_any_act_get_buffer_size(const int out_tensor_dim_x, const int in_tensor_ch)
{
    return riscv_nn_avepool_HWC_s8_any_act_get_buffer_size(out_tensor_dim_x, in_tensor_ch);
}

/**
 * @brief           This is a max pooling function for signed 8-bit integer
 *                  inputs.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_dim   dimension of the input tensor
 * @param[in]       in_tensor_ch    number of input tensor channels
 * @param[in]       ker_dim         dimension of the filter kernel
 * @param[in]       pad             padding size
 * @param[in]       stride          convolution stride
 * @param[in]       out_tensor_dim  dimension of the output tensor
 * @param[in]       in_tmp_buf      dummy
 * @param[out]      out_tensor      pointer of the output tensor
 *
 * @b Example:
 * @code
 * #define IN_DIM 32
 * #define IN_CH 32
 * #define KER_DIM 3
 * #define PAD 0
 * #define STRIDE 2
 * #define OUT_DIM 15
 *
 * q7_t in_data[IN_CH * IN_DIM * IN_DIM] = {...};
 * q7_t out_data[IN_CH * OUT_DIM * OUT_DIM] = {...};
 *
 * hpm_nn_maxpool_HWC_s8(in_data, IN_DIM, IN_CH, KER_DIM, PAD, STRIDE,
 *      OUT_DIM, NULL, out_data);
 * @endcode
 */
 static inline void hpm_nn_maxpool_HWC_s8(q7_t *in_tensor,
                            const uint16_t in_tensor_dim,
                            const uint16_t in_tensor_ch,
                            const uint16_t ker_dim,
                            const uint16_t pad,
                            const uint16_t stride,
                            const uint16_t out_tensor_dim,
                            q7_t *in_tmp_buf,
                            q7_t *out_tensor)
{
    riscv_nn_maxpool_HWC_s8(in_tensor, in_tensor_dim, in_tensor_ch, ker_dim, pad, stride, out_tensor_dim, in_tmp_buf, out_tensor);
}

/**
 * @brief           This is a max pooling function for signed 8-bit integer
 *                  inputs in any x and y dimensions with the actvating
 *                  parameters to limit the outputs.
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       act_min             minimum value that the output tensor is
 *                                      limited to. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value that the output tensor is
 *                                      limited to. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       tmp_buffer          dummy
 * @param[in]       out_tensor          pointer of the output tensor
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_maxpool_HWC_s8_any_act(const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t stride_y,
                                        const uint16_t stride_x,
                                        const uint16_t ker_dim_y,
                                        const uint16_t ker_dim_x,
                                        const uint16_t pad_y,
                                        const uint16_t pad_x,
                                        const int8_t act_min,
                                        const int8_t act_max,
                                        const uint16_t in_tensor_ch,
                                        int8_t *in_tensor,
                                        int16_t *tmp_buffer,
                                        int8_t *out_tensor)
{
    return riscv_nn_maxpool_HWC_s8_any_act(in_tensor_dim_y, in_tensor_dim_x, out_tensor_dim_y, out_tensor_dim_x, stride_y, stride_x, ker_dim_y, ker_dim_x, pad_y, pad_x, act_min, act_max, in_tensor_ch, in_tensor, tmp_buffer, out_tensor);
}

/**
 *   * @}
 */

#endif

#ifdef HPM_EN_MATH_NN_RVP32_LIB
#include "riscv_nn_pooling.h"

/**
 * @brief           This is an average pooling function for S8 inputs with any x
 *                  and y dimension with the actvating parameters to limit the
 *                  outputs.
 * @param[in]       in_tensor_dim_y     y dimension of the input tensor
 * @param[in]       in_tensor_dim_x     x dimension of the input tensor
 * @param[in]       out_tensor_dim_y    y dimension of the output tensor
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       stride_y            convolution stride in the y dimension
 * @param[in]       stride_x            convolution stride in the x dimension
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       pad_y               padding size in the y dimension
 * @param[in]       pad_x               padding size in the x dimension
 * @param[in]       act_min             minimum value that the output tensor is
 *                                      limited to. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       act_max             maximum value that the output tensor is
 *                                      limited to. It should be in the range of
 *                                      -128 to 127.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       in_tensor           pointer of the input tensor
 * @param[in]       in_tmp_buf          temporary buffer for the input tensor.
 *                                      Its needed size could be obtained by
 *                                      calling riscv_nn_avepool_s8_HWC_any_get_buffer_size.
 * @param[out]      out_tensor          pointer of the output tensor
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_avepool_HWC_s8_any_act(const int in_tensor_dim_y,
                                const int in_tensor_dim_x,
                                const int out_tensor_dim_y,
                                const int out_tensor_dim_x,
                                const int stride_y,
                                const int stride_x,
                                const int ker_dim_y,
                                const int ker_dim_x,
                                const int pad_y,
                                const int pad_x,
                                const int act_min,
                                const int act_max,
                                const int in_tensor_ch,
                                int8_t *in_tensor,
                                int16_t *in_tmp_buf,
                                int8_t *out_tensor)
{
    return riscv_nn_avepool_HWC_s8_any_act(in_tensor_dim_y, in_tensor_dim_x, out_tensor_dim_y, out_tensor_dim_x, stride_y, stride_x, ker_dim_y, ker_dim_x, pad_y, pad_x, act_min, act_max, in_tensor_ch, in_tensor, in_tmp_buf, out_tensor);
}

/**
 * @brief           This function is used to obtain the required size, in bytes,
 *                  for the input temporary buffer of riscv_nn_avepool_HWC_s8_any_act.
 * @param[in]       out_tensor_dim_x    x dimension of the output tensor
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @return          This function returns the size required by the temporary
 *                  buffer.
 */
static inline int32_t hpm_nn_avepool_HWC_s8_any_act_get_buffer_size(const int out_tensor_dim_x, const int in_tensor_ch)
{
    return riscv_nn_avepool_HWC_s8_any_act_get_buffer_size(out_tensor_dim_x, in_tensor_ch);
}

#endif
#endif

#ifdef HPM_MATH_NN_SOFTMAX
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_softmax.h"

/**
 * @defgroup nnsoftmax NN Softmax Functions
 * @ingroup hpmmath
 * @brief The softmax functions are exponential functions with base 2.
 *
 * @{
 */

/**
 * @brief           This is a softmax function for signed 8-bit integer input
 *                  vectors.
 * @param[in]       in_vec      pointer of the input vector
 * @param[in]       size        number of elements in the input vector
 * @param[out]      out_vec     pointer of the output vector
 *
 * @b Example:
 * @code
 * #define LENGTH 10
 * q7_t in_data[LENGTH] = {...};
 * q7_t out_data[LENGTH];
 *
 * hpm_nn_softmax_s8_fast(in_data, LENGTH, out_data);
 * @endcode
 */
static inline void hpm_nn_softmax_s8_fast(const q7_t *in_vec,
                            const uint16_t size,
                            q7_t *out_vec)
{
    riscv_nn_softmax_s8_fast(in_vec, size, out_vec);
}

/**
 * @brief           This is a softmax function for signed 16-bit integer input
 *                  vectors.
 * @param[in]       in_vec      pointer of the input vector
 * @param[in]       size        number of elements in the input vector
 * @param[out]      out_vec     pointer of the output vector
 */
static inline void hpm_nn_softmax_s16_fast(const q15_t *in_vec,
                            const uint16_t size,
                            q15_t *out_vec)
{
    riscv_nn_softmax_s16_fast(in_vec, size, out_vec);
}

/**
 * @brief           This is a softmax function for signed 8-bit integer input
 *                  tensor with high precision algorithm.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_row   number of rows in the input tensor
 * @param[in]       in_tensor_col   number of columns in the input tensor
 * @param[in]       scale           scaling value for input quantization
 * @param[in]       lshift          left shift amount for input quantization
 * @param[in]       diff_min        minimum threshold to perform the quantized
 *                                  exponential operation. The difference can be
 *                                  obtained by subtracting the input from the
 *                                  maximum in row.
 * @param[out]      out_tensor      pointer of the output tensor
 */
static inline void hpm_nn_softmax_s8_hp(const int8_t *in_tensor,
                            const int32_t in_tensor_row,
                            const int32_t in_tensor_col,
                            const int32_t scale,
                            const int32_t lshift,
                            const int32_t diff_min,
                            int8_t *out_tensor)
{
    riscv_nn_softmax_s8_hp(in_tensor, in_tensor_row, in_tensor_col, scale, lshift, diff_min, out_tensor);
}

/**
 * @brief           This is a softmax function for unsigned 8-bit integer input
 *                  tensor with high precision algorithm.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_row   number of rows in the input tensor
 * @param[in]       in_tensor_col   number of columns in the input tensor
 * @param[in]       scale           scaling value for input quantization
 * @param[in]       lshift          left shift amount for input quantization
 * @param[in]       diff_min        minimum threshold to perform the quantized
 *                                  exponential operation. The difference can be
 *                                  obtained by subtracting the input from the
 *                                  maximum in row.
 * @param[out]      out_tensor      pointer of the output tensor
 */
static inline void hpm_nn_softmax_u8_hp(const uint8_t *in_tensor,
                            const int32_t in_tensor_row,
                            const int32_t in_tensor_col,
                            const int32_t scale,
                            const int32_t lshift,
                            const int32_t diff_min,
                            uint8_t *out_tensor)
{
    riscv_nn_softmax_u8_hp(in_tensor, in_tensor_row, in_tensor_col, scale, lshift, diff_min, out_tensor);
}

/**
 *   * @}
 */

#endif

#ifdef HPM_EN_MATH_NN_RVP32_LIB
#include "riscv_nn_softmax.h"
/**
 * @brief           This is a softmax function for signed 8-bit integer input
 *                  tensor with high precision algorithm.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[in]       in_tensor_row   number of rows in the input tensor
 * @param[in]       in_tensor_col   number of columns in the input tensor
 * @param[in]       scale           scaling value for input quantization
 * @param[in]       lshift          left shift amount for input quantization
 * @param[in]       diff_min        minimum threshold to perform the quantized
 *                                  exponential operation. The difference can be
 *                                  obtained by subtracting the input from the
 *                                  maximum in row.
 * @param[out]      out_tensor      pointer of the output tensor
 */
static inline void hpm_nn_softmax_s8_hp(const int8_t *in_tensor,
                            const int32_t in_tensor_row,
                            const int32_t in_tensor_col,
                            const int32_t scale,
                            const int32_t lshift,
                            const int32_t diff_min,
                            int8_t *out_tensor)
{
    riscv_nn_softmax_s8_hp(in_tensor, in_tensor_row, in_tensor_col, scale, lshift, diff_min, out_tensor);
}
#endif

#endif

#ifdef HPM_MATH_NN_UTIL
#ifdef HPM_EN_MATH_NN_LIB
#include "riscv_nn_util.h"

/**
 * @defgroup nnutils NN Utils Functions
 * @ingroup hpmmath
 * @brief Utils functions are miscellaneous auxiliary tools.
 *
 * @{
 */

#ifdef __riscv_zfh
/**
 * @brief           This function calculates the base-e exponential values of
 *                  16-bit half-precision floating point inputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       size            number of elements in the input vector
 * @param[out]      out_vec         pointer of the output vector
 * @return          This function only returns 0.
 */
static inline int32_t hpm_nn_exp_f16(const float16_t *in_vec,
                        const uint32_t size,
                        float16_t *out_vec)
{
    return riscv_nn_exp_f16(in_vec, size, out_vec);
}
#endif

/**
 * @brief           This function turns the input tensor into another tensor
 *                  with the same data but in a different shape.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       size            size, in bytes, of total input tensor
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * int8_t in_tensor[SIZE] = {...};
 * int8_t out_tensor[SIZE];
 *
 * hpm_nn_reshape_s8(in_tensor, out_tensor, SIZE);
 * @endcode
 */
static inline void hpm_nn_reshape_s8(const int8_t *in_tensor,
                        int8_t *out_tensor,
                        const uint32_t size)
{
    riscv_nn_reshape_s8(in_tensor, out_tensor, size);
}

/**
 * @brief           This function finds the k largest values and their indices
 *                  from the signed 8-bit integer input vector.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       size            number of elements in the input vector
 * @param[in]       k               the number of the largest values to be
 *                                  searched
 * @param[out]      val             the k largest values in the input vector
 * @param[out]      idx             the indices of the k largest values in the
 *                                  input vector
 * @return          This function only returns 0.
 *
 * @note
 * - If there is a number of elements with the same value, the element with
 *   smaller index will be selected with high priority.
 * - The k largest values will be sorted from largest to smallest and stored in
 *   "val" output vector. If there is a number of elements with the same value,
 *   the elements will be sorted from smallest index to largest index.
 */
static inline int32_t hpm_nn_top_k_s8(q7_t *in_vec,
                        uint32_t size,
                        uint32_t k,
                        q7_t *val,
                        uint32_t *idx)
{
    return riscv_nn_top_k_s8(in_vec, size, k, val, idx);
}

#ifdef __riscv_zfh
/**
 * @brief           This function finds the k largest values and their indices
 *                  from the 16-bit half-precision floating point input vector.
 * @param[in]       in_vec          pointer of the input tensor
 * @param[in]       size            number of elements in the input vector
 * @param[in]       k               the number of the largest values to be
 *                                  searched
 * @param[out]      val             the k largest values in the input vector
 * @param[out]      idx             the indices of the k largest values in the
 *                                  input vector
 * @return          This function only returns 0.
 *
 * @note
 * - If there is a number of elements with the same value, the element with
 *   smaller index will be selected with high priority.
 * - The k largest values will be sorted from largest to smallest and stored in
 *   "val" output vector. If there is a number of elements with the same value,
 *   the elements will be sorted from smallest index to largest index.
 */
static inline int32_t hpm_nn_top_k_f16(float16_t *in_vec,
                        uint32_t size,
                        uint32_t k,
                        float16_t *val,
                        uint32_t *idx)
{
    return riscv_nn_top_k_f16(in_vec, size, k, val, idx);
}
#endif

/**
 *   * @}
 */

#endif

#ifdef HPM_EN_MATH_NN_RVP32_LIB

#include "riscv_nn_util.h"
/**
 * @brief           This function turns the input tensor into another tensor
 *                  with the same data but in a different shape.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       size            size, in bytes, of total input tensor
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * int8_t in_tensor[SIZE] = {...};
 * int8_t out_tensor[SIZE];
 *
 * hpm_nn_reshape_s8(in_tensor, out_tensor, SIZE);
 * @endcode
 */
static inline void hpm_nn_reshape_s8(const int8_t *in_tensor,
                        int8_t *out_tensor,
                        const uint32_t size)
{
    riscv_nn_reshape_s8(in_tensor, out_tensor, size);
}

#endif

#endif

#ifdef  __cplusplus
}
#endif
#endif
