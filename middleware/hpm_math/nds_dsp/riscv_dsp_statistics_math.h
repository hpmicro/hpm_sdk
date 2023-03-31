/******************************************************************************
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.*
 * Copyright (C) 2018-2021 Andes Technology Corporation. All rights reserved. *
 *                                                                            *
 * SPDX-License-Identifier: Apache-2.0                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the License); you may      *
 * not use this file except in compliance with the License.                   *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 * www.apache.org/licenses/LICENSE-2.0                                        *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT    *
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.           *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/
#ifndef __RISCV_DSP_STATISTICS_MATH_H__
#define __RISCV_DSP_STATISTICS_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup statistics Statistics Functions
 */
/**
 * @addtogroup statistics
 * @{
 */
// Maximum
/**
 * @defgroup max Maximum Functions
 * @brief Maximum Functions
 *
 * Maximum functions compare the values in a vector then return the maximum one and its index.
 *
 * Andes DSP library supports distinct maximum functions for floating-point, Q31, Q15, Q7 and
 * other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup max
 * @{
 */
/**
 * @brief Maximum value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
float32_t riscv_dsp_max_f32(const float32_t *src, uint32_t size, uint32_t *index);
/**
 * @brief Maximum absolute value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
float32_t riscv_dsp_absmax_f32(const float32_t* src, uint32_t size, uint32_t* index);
/**
 * @brief Maximum value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @return Maximum value
 */
float32_t riscv_dsp_max_val_f32(const float32_t *src, uint32_t size);

/**
 * @brief Maximum value of the q15 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
q15_t riscv_dsp_max_q15(const q15_t *src, uint32_t size, uint32_t *index);
/**
 * @brief Maximum absolute value of the q15 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
q15_t riscv_dsp_absmax_q15(const q15_t* src, uint32_t size, uint32_t* index);

/**
 * @brief Maximum value of the q31 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
q31_t riscv_dsp_max_q31(const q31_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Maximum absolute value of the q31 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
q31_t riscv_dsp_absmax_q31(const q31_t* src, uint32_t size, uint32_t* index);
/**
 * @brief Maximum value of the q7 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
q7_t riscv_dsp_max_q7(const q7_t *src, uint32_t size, uint32_t *index);
/**
 * @brief Maximum absolute value of the q7 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
q7_t riscv_dsp_absmax_q7(const q7_t* src, uint32_t size, uint32_t* index);
/**
 * @brief Max value of the u8 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Maximum value
 */
uint8_t riscv_dsp_max_u8(const uint8_t *src, uint32_t size, uint32_t *index);
/** @} maxs */

// Minimum
/**
 * @defgroup min Minimum Functions
 * @brief Minimum Functions
 *
 * Minimum functions compare the values in a vector then return the minimum one and its index.
 *
 * Andes DSP library supports distinct minimum functions for floating-point, Q31, Q15, Q7 and
 * other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup min
 * @{
 */
/**
 * @brief Minimum value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
float32_t riscv_dsp_min_f32(const float32_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum absolute value of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
float32_t riscv_dsp_absmin_f32(const float32_t* src, uint32_t size, uint32_t* index);

/**
 * @brief Minimum value of the q15 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
q15_t riscv_dsp_min_q15(const q15_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum absolute value of the q15 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
q15_t riscv_dsp_absmin_q15(const q15_t* src, uint32_t size, uint32_t* index);

/**
 * @brief Minimum value of the q31 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
q31_t riscv_dsp_min_q31(const q31_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum absolute value of the q31 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
q31_t riscv_dsp_absmin_q31(const q31_t* src, uint32_t size, uint32_t* index);

/**
 * @brief Minimum value of the q7 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
q7_t riscv_dsp_min_q7(const q7_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum absolute value of the q7 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
q7_t riscv_dsp_absmin_q7(const q7_t* src, uint32_t size, uint32_t* index);

/**
 * @brief Minimum value of the u8 vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       size    number of elements in a vector
 * @param[out]      index   index of the maximum value
 * @return Minimum value
 */
uint8_t riscv_dsp_min_u8(const uint8_t *src, uint32_t size, uint32_t *index);

/** @} min */
// Mean
/**
 * @defgroup mean Mean Functions
 * @brief Mean Functions
 *
 * Mean functions calculate the arithmetic mean of a vector.
 *
 * Andes DSP library supports distinct mean functions for floating-point, Q31, Q15, Q7 and other
 * data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup mean
 * @{
 */
/**
 * @brief Mean value of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Mean value.
 */
float32_t riscv_dsp_mean_f32(const float32_t *src, uint32_t size);

/**
 * @brief Mean value of the q15 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Mean value.
 */
q15_t riscv_dsp_mean_q15(const q15_t *src, uint32_t size);

/**
 * @brief Mean value of the q31 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Mean value.
 */
q31_t riscv_dsp_mean_q31(const q31_t *src, uint32_t size);

/**
 * @brief Mean value of the q7 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Mean value.
 */
q7_t riscv_dsp_mean_q7(const q7_t *src, uint32_t size);

/**
 * @brief Mean value of the u8 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Mean value.
 */
uint8_t riscv_dsp_mean_u8(const uint8_t *src, uint32_t size);
/** @} mean */

// Power Functions
/**
 * @defgroup pwr Power Functions
 * @brief ower Functions
 *
 * Power functions compute the sum of squares of a vector. The behavior can be defined as follows:
 *
 * @image html pwr.gif ""
 *
 * Andes DSP library supports distinct sum of squares functions for the following data types:
 * floating-point, Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup pwr
 * @{
 */
/**
 * @brief Sum of the squares of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Sum of squares
 */
float32_t riscv_dsp_pwr_f32(const float32_t *src, uint32_t size);

/**
 * @brief Sum of the squares of the q15 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Sum of squares
 *
 * @b Note:
 *
 * The return value is in Q30 format.
 */
q63_t riscv_dsp_pwr_q15(const q15_t *src, uint32_t size);

/**
 * @brief Sum of the squares of the q31 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Sum of squares
 *
 * @b Note:
 *
 * The return value is in Q48 format
 */
q63_t riscv_dsp_pwr_q31(const q31_t *src, uint32_t size);

/**
 * @brief Sum of the squares of the q7 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Sum of squares
 *
 * @b Note:
 *
 * The return value is in Q14 format
 */
q31_t riscv_dsp_pwr_q7(const q7_t *src, uint32_t size);
/** @} pwr */

// Root Mean Square
/**
 * @defgroup rms Root Mean Square (RMS) Functions
 * @brief RMS Functions
 *
 * Root mean square (RMS) functions compute root mean square of a vector. The behavior can be
 * defined as follows:
 *
 * @image html rms.gif ""
 *
 * For sqrt(), please refer to square root functions.
 *
 * Andes DSP library supports distinct RMS functions for the following data types: floating-point,
 * Q31, and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup rms
 * @{
 */
/**
 * @brief RMS of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return RMS value.
 */
float32_t riscv_dsp_rms_f32(const float32_t *src, uint32_t size);

/**
 * @brief RMS of the q15 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return RMS value.
 */
q15_t riscv_dsp_rms_q15(const q15_t *src, uint32_t size);

/**
 * @brief RMS of the q31 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return RMS value.
 */
q31_t riscv_dsp_rms_q31(const q31_t *src, uint32_t size);
/** @} rms */

// Standard deviation
/**
 * @defgroup std Standard Deviation Functions
 * @brief Standard Deviation Functions
 *
 * Standard deviation functions compute the standard deviation value of a vector. The behavior can
 * be defined as follows:
 *
 * @image html std.gif ""
 *
 * For sqrt(), please refer to square root functions.
 *
 * Andes DSP library supports distinct standard deviation functions for floating-point, Q31, Q15
 * and other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup std
 * @{
 */
/**
 * @brief Standard deviation of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Standard deviation value.
 */
float32_t riscv_dsp_std_f32(const float32_t *src, uint32_t size);

/**
 * @brief Standard deviation of the q15 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Standard deviation value.
 */
q15_t riscv_dsp_std_q15(const q15_t *src, uint32_t size);

/**
 * @brief Standard deviation of the q31 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Standard deviation value.
 */
q31_t riscv_dsp_std_q31(const q31_t *src, uint32_t size);

/**
 * @brief Standard deviation of the u8 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Standard deviation value.
 */
q15_t riscv_dsp_std_u8(const uint8_t *src, uint32_t size);
/** @} std */

// Variance
/**
 * @defgroup var Variance Functions
 * @brief Variance Functions
 *
 * Variance functions compute the variance value of a vector. The behavior can be defined as
 * follows:
 *
 * @image html var.gif ""
 *
 * Andes DSP library supports distinct variance functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup var
 * @{
 */
/**
 * @brief Variance of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Variance value.
 */
float32_t riscv_dsp_var_f32(const float32_t *src, uint32_t size);

/**
 * @brief Variance of the q15 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Variance value.
 */
q31_t riscv_dsp_var_q15(const q15_t *src, uint32_t size);

/**
 * @brief Variance of the q31 vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Variance value.
 */
q63_t riscv_dsp_var_q31(const q31_t *src, uint32_t size);

/** @} var */
/**
 * @defgroup entropy Entropy Function
 * @brief Entropy Function
 *
 * The entropy function computes the entropy of a vector. The behavior can be defined as follows:
 *
 * @image html entropy.gif ""
 *
 * Andes DSP library only supports the entropy function for floating-point data.
 */
/**
 * @addtogroup entropy
 * @{
 */
/**
 * @brief Entropy of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return Entropy value.
 */
float32_t riscv_dsp_entropy_f32(const float32_t * src, uint32_t size);
/** @} entropy */

/**
 * @defgroup rel_entropy Relative Entropy Function
 * @brief Relative Entropy Function
 *
 * The relative entropy function computes the entropy of two vectors and is also called the
 * Kullback–Leibler divergence (DKL). The behavior can be defined as follows:
 *
 * @image html rel_entropy.gif ""
 *
 * Andes DSP library only supports the relative entropy function for floating-point data.
 */
/**
 * @addtogroup rel_entropy
 * @{
 */
/**
 * @brief Relative Entropy of the floating-potint vector.
 * @param[in]       src1    pointer of the first input vector
 * @param[in]       src2    pointer of the second input vector
 * @param[in]       size   size of elements in a vector
 * @return Relative Entropy value.
 */
float32_t riscv_dsp_relative_entropy_f32(const float32_t * src1, const float32_t * src2, uint32_t size);
/** @} rel_entropy */

/**
 * @defgroup lse Log-Sum-Exp (LSE) Function
 * @brief LSE Function
 *
 * The LSE function computes the logarithm of the sum of the exponentials of a vector. The
 * behavior can be defined as follows:
 *
 * @image html lse.gif ""
 *
 * Andes DSP library only supports the LSE function for floating-point data.
 */
/**
 * @addtogroup lse
 * @{
 */
/**
 * @brief Log-Sum-Exp of the floating-potint vector.
 * @param[in]       src    pointer of the input vector
 * @param[in]       size   number of elements in a vector
 * @return LSE value.
 *
 */
float32_t riscv_dsp_lse_f32(const float32_t * src, uint32_t size);
/** @} lse */

/**
 * @defgroup lse_dprod Log-Sum-Exp (LSE) Dot Product Function
 * @brief LSE Dot Product Function
 *
 * @image html lse_dprod.gif ""
 *
 * Andes DSP library only supports the LSE dot product function for floating-point data.
 */
/**
 * @addtogroup lse_dprod
 * @{
 */
/**
 * @brief Dot product with Log-Sum-Exp of the floating-potint vector.
 * @param[in]       src1    pointer of the first input vector
 * @param[in]       src2    pointer of the second input vector
 * @param[in]       size    size of elements in a vector
 * @param[in]       buffer  pointer of the temporary vector
 * @return LSE dot product value
 *
 */
float32_t riscv_dsp_lse_dprod_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *buffer);
/** @} lse_dprod */

// Naive Gaussian Bayesian Estimator
/**
 * @defgroup naivebayes Naive Gaussian Bayesian Estimation Function
 * @brief Naive Gaussian Bayesian Estimation Function
 *
 * The function implements the naive Gaussian Bayes estimator.
 *
 * Andes DSP library only supports the naive GaussianBayesian predictor function for
 * floating-point data.
 */
/**
 * @addtogroup naivebayes
 * @{
 */
/**
 * @brief instance structure of the floating-point Gaussian naïve bayes estimator,
 */

typedef struct
{
    uint32_t dimofvec;            /**< dimension of vector space */
    uint32_t numofclass;          /**< number of classes */
    const float32_t *mean;        /**< mean value for the Gaussian whose size is dimofvec*numofclass */
    const float32_t *var;         /**< variance for the Gaussian whose size is dimofvec*numofclass */
    const float32_t *classprior;  /**< class prior probabilities whose size is numofclass */
    float32_t additiveofvar;      /**< additive value of variance */
} riscv_dsp_gaussian_naivebayes_f32_t;

/**
 * @brief Naive Gaussian Bayesian Estimator
 *
 * @param[in]  instance  pointer of the instance structure
 * @param[in]  src       pointer of the input vector
 * @param[in]  buf       pointer of the buffer vector
 * @return Estimated class
 *
 */
uint32_t riscv_dsp_gaussian_naive_bayes_est_f32(const riscv_dsp_gaussian_naivebayes_f32_t *instance,
                                              const float32_t * src,
                                              float32_t *buf);
/** @} naivebayes */

/** @} statistics */
#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_STATISTICS_MATH_H__
