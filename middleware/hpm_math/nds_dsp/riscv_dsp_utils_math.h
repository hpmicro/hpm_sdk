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
#ifndef __RISCV_DSP_UTILS_MATH_H__
#define __RISCV_DSP_UTILS_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup utils Utils Functions
 */
/**
 * @addtogroup utils
 * @{
 */
// Cosine and Sine
/**
 * @defgroup cos_sine Cosine and Sine Functions
 * @brief Cosine and Sine Functions
 *
 * Cosine and sine functions compute the cosine and sine values.
 * The input values of these functions are in radians and their ranges are different between
 * floating-point and Q number functions.
 *
 * For floating-point functions, there is no limitation for the input range since the inputs, whatever
 * values they are, will be pre-processed by the following snippet into the range [-2pi, 2pi]:
 *
 * @image html cos.gif ""
 *
 * where src is the input value.
 *
 * For Q number (Q31 and Q15) functions, they only support the input range [-1, 1) to represent
 * the Q format range [-pi, pi]. Thus, the input values may need to be converted into the Q format
 * range before these functions are called.
 *
 * Andes DSP library supports distinct cosine and sine functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup cos_sine
 * @{
 */
/**
 * @param[in] src input value (radian)
 * @return Cosine value of the input
 *
 * @b Example
 *     <pre>
 *      float32_t src = 0.5;
 *      float32_t dst;
 *      dst = riscv_dsp_cos_f32 (src);
 *
 * This example also serves as a reference for examples of Q31 or Q15 cosine/sine functions.
 *     </pre>
 */
float32_t riscv_dsp_cos_f32(float32_t src);

#if defined (__riscv_zfh)
/**
 * @param[in] src input value (radian)
 * @return Cosine value of the input
 */
float16_t riscv_dsp_cos_f16(float16_t src);
#endif
/**
 * @param[in] src input value (radian)
 * @return Cosine value of the input
 *
 * @b Note:
 *
 * The input range is [0x80000000, 0x7FFFFFFF] and mapped to the range [-pi, pi]
 */
q31_t riscv_dsp_cos_q31(q31_t src);
/**
 * @param[in] src input value (radian)
 * @return Cosine value of the input
 *
 * @b Note:
 *
 * The input range is [0x8000, 0x7FFF] and mapped to the range [-pi, pi]
 */
q15_t riscv_dsp_cos_q15(q15_t src);

/**
 * @param[in] src input value (radian)
 * @return Sine value of the input
 */
float32_t riscv_dsp_sin_f32(float32_t src);

#if defined (__riscv_zfh)
/**
 * @param[in] src input value (radian)
 * @return Cosine value of the input
 */
float16_t riscv_dsp_sin_f16(float16_t src);
#endif

/**
 * @param[in] src input value (radian)
 * @return Sine value of the input
 *
 * @b Note:
 *
 * The input range is [0x80000000, 0x7FFFFFFF] and mapped to the range [-pi, pi]
 */
q31_t riscv_dsp_sin_q31(q31_t src);
/**
 * @param[in] src input value (radian)
 * @return Sine value of the input
 *
 * @b Note:
 *
 * The input range is [0x8000, 0x7FFF] and mapped to the range [-pi, pi]
 */
q15_t riscv_dsp_sin_q15(q15_t src);

/** @} cos_sine */

// Arc tangent
/**
 * @defgroup atan Arc Tangent Functions
 * @brief Arc Tangent Functions
 *
 * Arc tangent functions are the inverse 0f trigonometric functions which return the inverse of
 * tangent given an input value. The input values and output results of arc tangent functions are in
 * radians and the output results are always in the range [-pi/2, pi/2]
 *
 * Andes DSP library supports distinct arc tangent functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup atan
 * @{
 */
/**
 * @param[in] src input value (radian)
 * @return Radian value
 *
 * @b Example
 *   <pre>
 *      float32_t src = 0.5;
 *      float32_t dst;
 *      dst = riscv_dsp_atan_f32 (src);
 *
 * This example also serves as a reference for examples of Q31 or Q15 arc tangent functions.
 *   </pre>
 */
float32_t riscv_dsp_atan_f32(float32_t src);
/**
 * @param[in] src input value (radian)
 * @return Radian value
 */
q31_t riscv_dsp_atan_q31(q31_t src);
/**
 * @param[in] src input value (radian)
 * @return Radian value
 */
q15_t riscv_dsp_atan_q15(q15_t src);
/** @} atan */

/**
 * @defgroup atan2 Arc Tangent 2 Functions
 * @brief Arc Tangent 2 Functions
 *
 * Arc tangent 2 functions are similar to arc tangent functions but accept two inputs y and x to
 * compute the inverse tangent value. Normally, the output results are all in radians and always in
 * the range [-pi, pi]. For convenient calculation, they are mapped to [-1, 1) in Q number
 * functions, but you can use conversion formulas to convert them to their floating-point
 * counterparts.
 *
 * Andes DSP library supports distinct arc tangent 2 functions for floating-point, Q31 and Q15 data
 * types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup atan2
 * @{
 */
/**
 * @param[in]    y  input value y
 * @param[in]    x  input value x
 * @return Radian value
 *
 * @b Note:
 *
 * The return value is in the floating-point format and always in the range [-pi, pi]
 */
float32_t riscv_dsp_atan2_f32(float32_t y, float32_t x);
/**
 * @param[in]    y  input value y
 * @param[in]    x  input value x
 * @return Radian value
 *
 * @b Note:
 *
 * The return value is in the Q15 format and always in the range [0x8000, 0x7FFF] (namely,
 * [-1, 1)). To convert the value to the floating-point format, apply the following formula:
 *   <pre>
 *     floating point value = (return value * pi) / (2 ^ 15)
 *   </pre>
 */
q15_t riscv_dsp_atan2_q15(q15_t y, q15_t x);
/**
 * @param[in]    y  input value y
 * @param[in]    x  input value x
 * @return Radian value
 *
 * @b Note:
 *
 * The return value is in the Q31 format and always in the range [0x80000000, 0x7FFFFFFF] (namely,
 * [-1, 1)). To convert the value to the floating-point format, apply the following formula:
 *   <pre>
 *     floating point value = (return value * pi) / (2 ^ 31)
 *   </pre>
 */
q31_t riscv_dsp_atan2_q31(q31_t y, q31_t x);
/** @} atan2 */

// Square Root
/**
 * @defgroup sqrt Square Root Functions
 * @brief Square Root Functions
 *
 * Square root functions return the square root value of the input.
 *
 * Andes DSP library supports distinct square root functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup sqrt
 * @{
 */
/**
 * @brief Square root of the floating-potint input.
 * @param[in]       src input value
 * @return Square root of the input.
 */
float32_t riscv_dsp_sqrt_f32(float32_t src);

/**
 * @brief Square root of the q31 input.
 * @param[in]       src input value
 * @return Square root of the input.
 */
q31_t riscv_dsp_sqrt_q31(q31_t src);

/**
 * @brief Square root of the q15 input.
 * @param[in]       src input value
 * @return Square root of the input.
 */
q15_t riscv_dsp_sqrt_q15(q15_t src);
/** @} sqrt */

// Convert function
/**
 * @defgroup convert Conversion Functions
 * @brief Conversion Functions
 *
 * Conversion functions convert element values in a source vector from one data type to another
 * and write the results into a destination vector. There are following data type conversions:
 *
 * @image html convert.gif ""
 *
 * These conversion functions are introduced in the subsections below.
 */
/**
 * @addtogroup convert
 * @{
 */
/**
 * @brief Convert a floating-point vector to Q15.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. This function converts values from float32_t to q15_t.
 * 2. The results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_convert_f32_q15(float32_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Convert a floating-point vector to Q31.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. This function converts values from float32_t to q31_t.
 * 2. The results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_convert_f32_q31(float32_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Convert a floating-point vector to Q7.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. This function converts values from float32_t to q7_t.
 * 2. The results will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_convert_f32_q7(float32_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Convert a Q15 vector to floating.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q15_t to float32_t.
 */
void riscv_dsp_convert_q15_f32(q15_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Convert a Q15 vector to Q31.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q15_t to q31_t (left-shifted with 16 bits).
 */
void riscv_dsp_convert_q15_q31(q15_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Convert a Q15 vector to Q7.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q15_t to q7_t (right-shifted with 8 bits).
 */
void riscv_dsp_convert_q15_q7(q15_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Convert a Q31 vector to floating.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q31_t to float32_t.
 */
void riscv_dsp_convert_q31_f32(q31_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Convert a Q31 vector to Q15.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. This function converts values from q31_t to q15_t.
 * 2. There is no overflow situation since the values are simply right shifted with 16 bits.
 */
void riscv_dsp_convert_q31_q15(q31_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Convert a Q31 vector to Q7.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. This function converts values from q31_t to q7_t.
 * 2. There is no overflow situation since the values are simply right shifted with 24 bits.
 */
void riscv_dsp_convert_q31_q7(q31_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Convert a Q7 vector to floating.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q7_t to float32_t.
 */
void riscv_dsp_convert_q7_f32(q7_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Convert a Q7 vector to Q15.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q7_t to q15_t (left-shifted with 8 bits).
 */
void riscv_dsp_convert_q7_q15(q7_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Convert a Q7 vector to Q31.
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * This function converts values from q7_t to q31_t (left-shifted with 24 bits).
 */
void riscv_dsp_convert_q7_q31(q7_t *src, q31_t *dst, uint32_t size);

/** @} convert */

// Duplicate function
/**
 * @defgroup dup Duplicate Functions
 * @brief Duplicate Functions
 *
 * Duplicate functions copy element values in a source vector and write the values one-by-one into
 * a destination vector. The behavior can be defined as follows:
 *
 * @image html dup.gif ""
 *
 * Andes DSP library supports distinct duplicate functions for the following data types:
 * floating-point, Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup dup
 * @{
 */
/**
 * @brief Duplicate the floating vector
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_dup_f32(float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Duplicate the Q15 vector
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_dup_q15(q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Duplicate the Q31 vector
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_dup_q31(q31_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Duplicate the Q7 vector
 * @param[in]       src  pointer of the input vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_dup_q7(q7_t *src, q7_t *dst, uint32_t size);
/** @} dup */

// Set function
/**
 * @defgroup set Set Functions
 * @brief Set Functions
 *
 * Set functions set all elements in a destination vector to a constant value. The behavior can be
 * defined as follows:
 *
 * @image html set.gif ""
 *
 * Andes DSP library supports distinct set functions for the following data types: floating-point,
 * Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup set
 * @{
 */
/**
 * @brief Set the floating-point vector.
 * @param[in]       val  value to be written to the output vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_set_f32(float32_t val, float32_t *dst, uint32_t size);

/**
 * @brief Set the Q15 vector.
 * @param[in]       val  value to be written to the output vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_set_q15(q15_t val, q15_t *dst, uint32_t size);

/**
 * @brief Set the Q31 vector.
 * @param[in]       val  value to be written to the output vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_set_q31(q31_t val, q31_t *dst, uint32_t size);

/**
 * @brief Set the Q7 vector.
 * @param[in]       val  value to be written to the output vector
 * @param[out]      dst  pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_set_q7(q7_t val, q7_t *dst, uint32_t size);
/** @} set */

/**
 * @defgroup weighted Weighted Sum Function
 * @brief Weighted Sum Function
 *
 * This function calculates the weighted sum of the input vector.
 * Andes DSP library only supports the weighted sum function for floating-point data.
 */
/**
 * @addtogroup weighted
 * @{
 */
/**
 * @brief Weighted Sum of the floating-potint vector.
 * @param[in]       src     pointer of the input vector
 * @param[in]       weight  pointer of the weighted vector
 * @param[in]       size    number of elements in a vector
 * @return Weighted sum value
 */
float32_t riscv_dsp_weighted_sum_f32(const float32_t *src, const float32_t *weight, uint32_t size);
/** @} weighted */

/**
 * @defgroup bary Barycenter Function
 * @brief Barycenter Function
 *
 * The function calculates the barycenter of the input vector.
 * Andes DSP library only supports the barycenter function for floating-point data.
 */
/**
 * @addtogroup bary
 * @{
 */
/**
 * @brief Barycenter of the floating-potint type.
 * @param[in]       src         pointer of the input vector
 * @param[in]       weights     pointer of the weighted vector
 * @param[out]      out         pointer of the output vector
 * @param[in]       numofvec    number of input vectors
 * @param[in]       dimofvec    dimension of vector space
 * @return  None
 *
 */
void riscv_dsp_barycenter_f32(const float32_t *src, const float32_t *weights,
                              float32_t *out, uint32_t numofvec, uint32_t dimofvec);
/** @} bary */

/**
 * @defgroup exp Exponential Functions
 * @brief Exponential Functions
 *
 * This function calculates the exponential value of the input value.
 *
 * Andes DSP library supports distinct exponential value functions for single- and half-precision floating-point data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup exp
 * @{
 */
/**
 * @brief Calculate exponential value of f32 vector.
 * @param[in] src input value
 * @return exponential value of the input
 */
float32_t riscv_dsp_exp_f32(float32_t src);

#if defined (__riscv_zfh)
/**
 * @brief Calculate exponential value of f16 vector.
 * @param[in] src input value
 * @return exponential value of the input
 */
float16_t riscv_dsp_exp_f16(float16_t src);
#endif

/** @} exp */

/**
 * @defgroup sigmoid Sigmoid Functions
 * @brief Sigmoid Functions
 *
 * This function calculates the sigmoid value of the input value.
 *
 * Andes DSP library supports distinct natural log value functions for single- and half-precision floating-point data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup sigmoid
 * @{
 */
/**
 * @brief Calculate sigmoid value of f32 vector.
 * @param[in] src input value
 * @return sigmoid value of the input
 */
float32_t riscv_dsp_sigmoid_f32(float32_t src);

#if defined (__riscv_zfh)
/**
 * @brief Calculate sigmoid value of f16 vector.
 * @param[in] src input value
 * @return sigmoid value of the input
 */
float16_t riscv_dsp_sigmoid_f16(float16_t src);
#endif
/** @} sigmoid */

/**
 * @defgroup log Natural Logarithmic Functions
 * @brief Natural Logarithmic Functions
 *
 * This function calculates the natural log value of the input value. The input value must large than zero.
 *
 * Andes DSP library supports distinct natural log value functions for single- and half-precision floating-point data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup log
 * @{
 */
/**
 * @brief Calculate the natural logarithm value of f32 vector.
 * @param[in] src input value
 * @return natural logarithm value of the input
 */
float32_t riscv_dsp_log_f32(float32_t src);

#if defined (__riscv_zfh)
/**
 * @brief Calculate the natural logarithm value of f16 vector.
 * @param[in] src input value
 * @return natural logarithm value of the input
 */
float16_t riscv_dsp_log_f16(float16_t src);
#endif
/** @} log */

/** @} utils */

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_UTILS_MATH_H__
