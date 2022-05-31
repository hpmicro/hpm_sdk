/**
 * Copyright (C) 2021 Andes Technology Corporation
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __RISCV_DSP_STATISTICS_MATH_H__
#define __RISCV_DSP_STATISTICS_MATH_H__

/**
 * @defgroup statistics Statistics Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

// Maximum
/**
 * @brief Maximum value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
float32_t riscv_dsp_max_f32(const float32_t *src, uint32_t size,
                        uint32_t *index);
float32_t riscv_dsp_max_val_f32(const float32_t *src, uint32_t size);

/**
 * @brief Maximum value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
q15_t riscv_dsp_max_q15(const q15_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Maximum value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
q31_t riscv_dsp_max_q31(const q31_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Maximum value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
q7_t riscv_dsp_max_q7(const q7_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Max value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @param[out]      *index  index of the maximum value.
 * @return max value.
 */
uint8_t riscv_dsp_max_u8(const uint8_t *src, uint32_t size, uint32_t *index);

// Minimum
/**
 * @brief Minimum value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
float32_t riscv_dsp_min_f32(const float32_t *src, uint32_t size,
                        uint32_t *index);

/**
 * @brief Minimum value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
q15_t riscv_dsp_min_q15(const q15_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
q31_t riscv_dsp_min_q31(const q31_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
q7_t riscv_dsp_min_q7(const q7_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @param[out]      *index  index of the minimum value.
 * @return minimum value.
 */
uint8_t riscv_dsp_min_u8(const uint8_t *src, uint32_t size, uint32_t *index);

// Mean
/**
 * @brief Mean value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 */
float32_t riscv_dsp_mean_f32(const float32_t *src, uint32_t size);

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
q15_t riscv_dsp_mean_q15(const q15_t *src, uint32_t size);

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
q31_t riscv_dsp_mean_q31(const q31_t *src, uint32_t size);

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
q7_t riscv_dsp_mean_q7(const q7_t *src, uint32_t size);

/**
 * @brief Mean value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return mean value.
 *
 * The 8-bit format input is accumulated in a 32-bit accumulator
 * and then truncated to yield a result of 8-bit format.
 */
uint8_t riscv_dsp_mean_u8(const uint8_t *src, uint32_t size);

// Sun of the Squares
/**
 * @brief Sum of the squares of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 */
float32_t riscv_dsp_pwr_f32(const float32_t *src, uint32_t size);

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
q63_t riscv_dsp_pwr_q15(const q15_t *src, uint32_t size);

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
q63_t riscv_dsp_pwr_q31(const q31_t *src, uint32_t size);

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
q31_t riscv_dsp_pwr_q7(const q7_t *src, uint32_t size);

// Root Mean Square
/**
 * @brief RMS of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 */
float32_t riscv_dsp_rms_f32(const float32_t *src, uint32_t size);

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
q15_t riscv_dsp_rms_q15(const q15_t *src, uint32_t size);

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
q31_t riscv_dsp_rms_q31(const q31_t *src, uint32_t size);

// Standard deviation
/**
 * @brief Standard deviation of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 */
float32_t riscv_dsp_std_f32(const float32_t *src, uint32_t size);

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
q15_t riscv_dsp_std_q15(const q15_t *src, uint32_t size);

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
q31_t riscv_dsp_std_q31(const q31_t *src, uint32_t size);

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
q15_t riscv_dsp_std_u8(const uint8_t *src, uint32_t size);

// Variance
/**
 * @brief Variance of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 */
float32_t riscv_dsp_var_f32(const float32_t *src, uint32_t size);

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
q31_t riscv_dsp_var_q15(const q15_t *src, uint32_t size);

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
q63_t riscv_dsp_var_q31(const q31_t *src, uint32_t size);

/**
 * @brief Entropy of the floating-potint vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return Entropy value.
 *
 * E = -sum (P .* log2 (P))
 */
float32_t riscv_dsp_entropy_f32(const float32_t * src, uint32_t size);


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
float32_t riscv_dsp_relative_entropy_f32(const float32_t * src1, const float32_t * src2, uint32_t size);

/**
 * @brief Log-Sum-Exp of the floating-potint vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return lse value.
 *
 */
float32_t riscv_dsp_lse_f32(const float32_t * src, uint32_t size);

/**
 * @brief Dot product with Log-Sum-Exp of the floating-potint vector.
 * @param[in]       *src1    points to the first input vector.
 * @param[in]       *src2    points to the second input vector.
 * @param[in]       size    size of the vectors.
 * @param[in]       *buffer  points to temporary buffer.
 * @return the Log-Sum-Exp of dot product value.
 *
 */
float32_t riscv_dsp_lse_dprod_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *buffer);


// Naive Gaussian Bayesian Estimator
typedef struct
{
    uint32_t dimofvec;            //  Dimension of vector space
    uint32_t numofclass;          //  Number of different classes
    const float32_t *mean;        // Mean values for the Gaussians of size (dimofvec * numofclass)
    const float32_t *var;         // Variances for the Gaussians of size (dimofvec * numofclass)
    const float32_t *classprior;  // Class prior probabilities of size (numofclass)
    float32_t additiveofvar;      // Additive value to variances
} riscv_dsp_gaussian_naivebayes_f32_t;

/**
 * @brief Naive Gaussian Bayesian Estimator
 *
 * @param[in]  *instance  points to a naive bayes instance
 * @param[in]  *src       points to the elements of the input vector.
 * @param[in]  *buf       points to a buffer of length numofclass /numberOfClasses
 * @return The predicted class
 *
 */
uint32_t riscv_dsp_gaussian_naive_bayes_est_f32(const riscv_dsp_gaussian_naivebayes_f32_t *instance,
                                              const float32_t * src,
                                              float32_t *buf);

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_STATISTICS_MATH_H__
