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
#ifndef __RISCV_DSP_BASIC_MATH_H__
#define __RISCV_DSP_BASIC_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup basic Basic Functions
 */
/**
 * @addtogroup basic
 * @{
 */

/**
 * \ingroup basic
 */

// Absolute value
/**
 * @defgroup basic_abs Absolute Functions
 * @brief Absolute Functions
 *
 * Absolute functions get absolute values from elements of a source vector and write them
 * one-by-one into a destination vector. The behavior can be defined as follows:
 *
 * @image html abs.gif ""
 *
 * Andes DSP library supports distinct absolute functions for the following data
 * types: floating-point, Q31, Q15 and Q7. These functions are introduced in the
 * subsections below.
 */

/**
 * @addtogroup basic_abs
 * @{
 */

/**
 * @brief Absolute value of floating-potint vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_abs_f32(float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Absolute value of q31 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector.
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The range of output results is [0, 0x7FFFFFFF].
 * 2. If the input value is INT32_MIN (0x80000000), then the output value
 * is INT32_MAX
 */
void riscv_dsp_abs_q31(q31_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Absolute value of q15 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector.
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The range of output results is [0, 0x7FFF].
 * 2. If the input value is INT16_MIN (0x8000), then the output value is INT16_MAX (0x7FFF).
 */
void riscv_dsp_abs_q15(q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Absolute value of q7 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector.
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The range of output results is [0, 0x7F].
 * 2. If the input value is INT8_MIN (0x80), then the output value is INT8_MAX (0x7F).
 */
void riscv_dsp_abs_q7(q7_t *src, q7_t *dst, uint32_t size);
/** @} basic_abs */

// Addition
/**
 * @defgroup basic_add Addition Functions
 * @brief Addition Functions
 *
 * Addition functions add two elements from separate source vectors and write the results
 * one-by-one into a destination vector. The behavior can be defined as follows:
 *
 * @image html add.gif ""
 *
 * Andes DSP library supports distinct addition functions for floating-point, Q31, Q15, Q7 and
 * other data types. These functions are introduced in the subsections below.
 */

/**
 * @addtogroup basic_add
 * @{
 */

/**
 * @brief Addition of floating-potint vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 */
void riscv_dsp_add_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size);

/**
 * @brief Addition of q31 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_add_q31(q31_t *src1, q31_t *src2, q31_t *dst, uint32_t size);

/**
 * @brief Addition of q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_add_q15(q15_t *src1, q15_t *src2, q15_t *dst, uint32_t size);

/**
 * @brief Addition of q7 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_add_q7(q7_t *src1, q7_t *src2, q7_t *dst, uint32_t size);

/**
 * @brief Addition of U8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition result of two uint8_t data will be saved in uint16_t format.
 */
void riscv_dsp_add_u8_u16(uint8_t *src1, uint8_t *src2, uint16_t *dst, uint32_t size);
/** @} basic_add */

// Subtraction
/**
 * @defgroup basic_sub Subtraction Functions
 * @brief Subtraction Functions
 *
 * Subtraction functions subtract two elements from separate source vectors and write the results
 * one-by-one into a destination vector. The behavior can be defined as follows:
 *
 * @image html sub.gif ""
 *
 * Andes DSP library supports distinct subtraction functions for floating-point, Q31, Q15, Q7 and
 * other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_sub
 * @{
 */

/**
 * @brief Subtraction of floating-point vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 */
void riscv_dsp_sub_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size);

/**
 * @brief Subtraction of q31 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_sub_q31(q31_t *src1, q31_t *src2, q31_t *dst, uint32_t size);

/**
 * @brief Subtraction of q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_sub_q15(q15_t *src1, q15_t *src2, q15_t *dst, uint32_t size);

/**
 * @brief Subtraction of q7 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_sub_q7(q7_t *src1, q7_t *src2, q7_t *dst, uint32_t size);

/**
 * @brief Subtraction of u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_sub_u8_q7(uint8_t *src1, uint8_t *src2, q7_t *dst, uint32_t size);
/** @} basic_sub */

// Multiplication
/**
 * @defgroup basic_mul Multiplication Functions
 * @brief Multiplication Functions
 *
 * Multiplication functions multiply two elements from separate vectors and write the results
 * one-by-one into a destination vector. The behavior can be defined as follows:
 *
 * @image html mul.gif ""
 *
 * Andes DSP library supports distinct multiplication functions for floating-point, Q31, Q15, Q7 and
 * other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_mul
 * @{
 */
/**
 * @brief Multiplication of floating-point vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 */
void riscv_dsp_mul_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size);

/**
 * @brief Multiplication of q31 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_mul_q31(q31_t *src1, q31_t *src2, q31_t *dst, uint32_t size);

/**
 * @brief Multiplication of q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_mul_q15(q15_t *src1, q15_t *src2, q15_t *dst, uint32_t size);

/**
 * @brief Multiplication of q7 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_mul_q7(q7_t *src1, q7_t *src2, q7_t *dst, uint32_t size);

/**
 * @brief Multiplication of u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The multiplication results will be saved to uint16_t *dst.
 */
void riscv_dsp_mul_u8_u16(uint8_t *src1, uint8_t *src2, uint16_t *dst, uint32_t size);
/** @} basic_mul */

// Division
/**
 * @defgroup basic_div Division Functions
 * @brief Subtraction Functions
 *
 * Division functions divide two elements from separate source vectors/scalars and write the
 * results one-by-one into a destination vector/scalar. The behavior can be defined as follows:
 *
 * @image html div.gif ""
 *
 * Andes DSP library supports distinct division functions for floating-point, Q31 and other data
 * types. The functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_div
 * @{
 */
/**
 * @brief Division of floating-point vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst  pointer of the destination vector
 * @param[in]       size  size of elements in a vector
 * @return none.
 */
void riscv_dsp_div_f32(float32_t *src1, float32_t *src2, float32_t *dst, uint32_t size);

/**
 * @brief Division of q31 inputs.
 * @param[in]       src1 dividend value
 * @param[in]       src2 divisor value
 * @return Quotient value
 *
 * @b Note:
 *
 * 1. The absolute value of src1 should be smaller than that of src2.
 * 2. This function deals with scalars instead of vectors.
 */
q31_t riscv_dsp_div_q31(q31_t src1, q31_t src2);

/**
 * @brief Division of q63 inputs divided by a positive 32 bits.
 * @param[in]       src1 dividend value
 * @param[in]       src2 divisor value
 * @return Quotient value
 *
 * @b Note:
 *
 * 1. The value of src1 right-shifted by 31 bits should be smaller than that of src2.
 *    Otherwise, the result is not valid.
 * 2. This function deals with scalars instead of vectors.
 */
q31_t riscv_dsp_div_s64_u32(q63_t src1, uint32_t src2);

/**
 * @brief Division of positive 64-bits inputs divided by a positive 32-bits.
 * @param[in]       src1 dividend value
 * @param[in]       src2 divisor value
 * @return Quotient value
 *
 * @b Note:
 *
 * 1. The absolute value of src1 right-shifted by 31 bits should be smaller than that of src2.
 *    Otherwise, the result is not valid.
 * 2. This function deals with scalars instead of vectors.
 */
q31_t riscv_dsp_div_u64_u32(uint64_t src1, uint32_t src2);
/** @} basic_div */

// Negation
/**
 * @defgroup basic_neg Negate Functions
 * @brief Negate Functions
 *
 * Negate functions negate elements of a source vector and write the results one-by-one into a
 * destination vector. The behavior can be defined as follows:
 *
 * @image html neg.gif ""
 *
 * Andes DSP library supports distinct negate functions for the following data types: floating-point,
 * Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_neg
 * @{
 */
/**
 * @brief Negation of floating-potint vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_neg_f32(float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Negation of q31 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The range of output results is [0x80000000, 0x7FFFFFFF].
 * 2. If the input value is INT32_MIN (0x80000000), then the output value is INT32_MAX
 *    (0x7FFFFFFF).
 */
void riscv_dsp_neg_q31(q31_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Negation of q15 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The range of output results is [0x8000, 0x7FFF].
 * 2. If the input value is INT16_MIN (0x8000), then the output value is INT16_MAX (0x7FFF).
 */
void riscv_dsp_neg_q15(q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Negation of q7 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The range of output results is [0x80, 0x7F].
 * 2. If the input value is INT8_MIN (0x80), then the output value is INT8_MAX (0x7F).
 */
void riscv_dsp_neg_q7(q7_t *src, q7_t *dst, uint32_t size);
/** @} basic_neg */

// Dot Production
/**
 * @defgroup basic_dprod Dot Product Functions
 * @brief Dot Product Functions
 *
 * Dot product functions calculate the dot product of two vectors and return the result values. The
 * definition of dot production is as follows:
 *
 * @image html dprod.gif ""
 *
 * Andes DSP library supports distinct dot product functions for floating-point, Q31, Q15, Q7 and
 * other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_dprod
 * @{
 */
/**
 * @brief Dot production of floating-point vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 */
float32_t riscv_dsp_dprod_f32(float32_t *src1, float32_t *src2,
                             uint32_t size);

/**
 * @brief Dot production of q31 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 *
 * @b Note:
 *
 * The format of the return values is Q48 and its type is q63_t.
 */
q63_t riscv_dsp_dprod_q31(q31_t *src1, q31_t *src2, uint32_t size);

/**
 * @brief Dot production of q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 *
 * @b Note:
 *
 * The format of the return values is Q30 and its type is q63_t.
 */
q63_t riscv_dsp_dprod_q15(q15_t *src1, q15_t *src2, uint32_t size);

/**
 * @brief Dot production of u8 * q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 *
 * @b Note:
 *
 * This function multiplies a uint8_t datum with a q15_t datum and then adds the result to
 * a q31_t accumulator.
 */
q31_t riscv_dsp_dprod_u8xq15(uint8_t *src1, q15_t *src2, uint32_t size);


/**
 * @brief Dot production of q7 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 *
 * @b Note:
 *
 * The format of the return values is Q14 and its type is q31_t.
 */
q31_t riscv_dsp_dprod_q7(q7_t *src1, q7_t *src2, uint32_t size);

/**
 * @brief Dot production of q7 * q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 *
 * @b Note:
 *
 * This function multiplies a q7_t datum with a q15_t datum and then adds the result to a
 * q31_t accumulator.
 */
q31_t riscv_dsp_dprod_q7xq15(q7_t *src1, q15_t *src2, uint32_t size);

/**
 * @brief Dot production of U8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       size  number of elements in a vector
 * @return Dot product of the two input vectors
 *
 * @b Note:
 *
 * This function multiplies a uint8_t datum with another uint8_t datum and then adds the
 * result to a uint32_t accumulator.
 */
uint32_t riscv_dsp_dprod_u8(uint8_t *src1, uint8_t *src2, uint32_t size);
/** @} basic_dprod */

// Offset
/**
 * @defgroup basic_offset Offset Functions
 * @brief Offset Functions
 *
 * Offset functions add each element of a source vector with a constant number and write the
 * results one-by-one into a destination vector. The behavior can be defined as follows:
 *
 * @image html offset.gif ""
 *
 * Andes DSP library supports distinct offset functions for floating-point, Q31, Q15, Q7 and other
 * data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_offset
 * @{
 */
/**
 * @brief The offset of floating-point vectors.
 * @param[in]       *src pointer of the input vector
 * @param[in]       offset constant offset value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_offset_f32(float32_t *src, float32_t offset, float32_t *dst,
                      uint32_t size);

/**
 * @brief The offset of q31 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[in]       offset constant offset value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_offset_q31(q31_t *src, q31_t offset, q31_t *dst, uint32_t size);

/**
 * @brief The offset of q15 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[in]       offset constant offset value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_offset_q15(q15_t *src, q15_t offset, q15_t *dst, uint32_t size);

/**
 * @brief The offset of q7 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[in]       offset constant offset value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The addition results will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_offset_q7(q7_t *src, q7_t offset, q7_t *dst, uint32_t size);

/**
 * @brief The offset of U8 vectors.
 * @param[in]       *src pointer of the input vector
 * @param[in]       offset constant offset value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The result value will be saturated to the uint8_t range [0x00, 0xFF].
 */
void riscv_dsp_offset_u8(uint8_t *src, q7_t offset, uint8_t *dst, uint32_t size);
/** @} basic_offset */

// Scale
/**
 * @defgroup basic_scale Scale Functions
 * @brief Scale Functions
 *
 * Scale functions multiply each element in a source vector by a constant scaling value and write
 * the results one-by-one into a destination vector. In general, its behavior can be defined as
 * follows:
 *
 * @image html scale.gif ""
 *
 * In cases of fractional presentation, a parameter shift is introduced to adjust the range of result
 * values. For details about the parameter, please refer to functions with fractional data types
 *
 * Andes DSP library supports distinct scale functions for floating-point, Q31, Q15, Q7 and other
 * data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_scale
 * @{
 */
/**
 * @brief To multiply a floating-point vectors by a floating-point scale.
 * @param[in]       *src pointer of the input vector
 * @param[in]       scale constant scaling value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define size 2
 *      float32_t src[size] = {0.8167, 0.5};
 *      float32_t dst[size] = {0};
 *      float32_t scale = 0.4;
 *      riscv_dsp_scale_f32(src, scale, dst, size);
 * This example also serves as a reference for examples of Q31, Q15, or Q7 scale functions.
 *     </pre>
 */
void riscv_dsp_scale_f32(float32_t *src, float32_t scale, float32_t *dst,
                     uint32_t size);

/**
 * @brief To multiply a q31 vectors by a q31 scale.
 * @param[in]       *src pointer of the input vector
 * @param[in]       scalefract constant fractional scaling value
 * @param[in]       shift   bits for result adjustment (Please refer to Note 1. and 2. below)
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to fractional presentation, the results should fall within the Q31 range. You can use
 * the input parameter, shift, to adjust the result values. The value of shift can range
 * from 0 to 31 in this function.
 * 2. The behavior of this funcition can be roughly presented as follows:
 *   <pre>
 *   dst[n] = (src[n] * scalefract) >> (31 - shift)
 *   Where 0 <= n < size and 0 <= shift <= 31.
 *   </pre>
 */
void riscv_dsp_scale_q31(q31_t *src, q31_t scalefract, int8_t shift,
                     q31_t *dst, uint32_t size);

/**
 * @brief To multiply a q15 vectors by a q15 scale.
 * @param[in]       *src pointer of the input vector
 * @param[in]       scalefract constant fractional scaling value
 * @param[in]       shift   bits for result adjustment (Please refer to Note 1. and 2. below)
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to fractional presentation, the results should fall within the Q15 range. You can use
 * the input parameter, shift, to adjust the result values. The value of shift can range
 * from 0 to 15 in this function.
 * 2. The behavior of this funcition can be roughly presented as follows:
 *   <pre>
 *   dst[n] = (src[n] * scalefract) >> (15 - shift)
 *   Where 0 <= n < size and 0 <= shift <= 15.
 *   </pre>
 */
void riscv_dsp_scale_q15(q15_t *src, q15_t scalefract, int8_t shift,
                     q15_t *dst, uint32_t size);

/**
 * @brief To multiply a q7 vectors by a q7 scale.
 * @param[in]       *src pointer of the input vector
 * @param[in]       scalefract constant fractional scaling value
 * @param[in]       shift   bits for result adjustment (Please refer to Note 1. and 2. below)
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to fractional presentation, the results should fall within the Q7 range. You can use
 * the input parameter, shift, to adjust the result values. The value of shift can range
 * from 0 to 7 in this function.
 * 2. The behavior of this funcition can be roughly presented as follows:
 *   <pre>
 *   dst[n] = (src[n] * scalefract) >> (7 - shift)
 *   Where 0 <= n < size and 0 <= shift <= 7.
 *   </pre>
 */
void riscv_dsp_scale_q7(q7_t *src, q7_t scalefract, int8_t shift,
                    q7_t *dst, uint32_t size);

/**
 * @brief To multiply a u8 vectors by a q7 scale.
 * @param[in]       *src pointer of the input vector
 * @param[in]       scalefract constant fractional scaling value
 * @param[in]       shift   bits for result adjustment (Please refer to Note 1. and 2. below)
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Since the data type for the destination vector is uint8_t, the scaled results will be
 * saturated to the uint8_t range [0x00, 0xFF]. You can also use the input parameter,
 * shift, to adjust the result values. The value of shift can range from 0 to 7 in this
 * function.
 * 2. The behavior of this funcition can be roughly presented as follows:
 *   <pre>
 *   dst[n] = (src[n] * scalefract) >> (7 - shift)
 *   Where 0 <= n < size and 0 <= shift <= 7.
 *   </pre>
 */
void riscv_dsp_scale_u8(uint8_t *src, q7_t scalefract, int8_t shift, uint8_t *dst, uint32_t size);
/** @} basic_scale */

// Shift
/**
 * @defgroup basic_shift Shift Functions
 * @brief Shift Functions
 *
 * Shift functions shift each element in a source vector by a constant shift value and write the
 * results one-by-one into a destination vector. The positive/negative shift value decides the
 * left/right direction elements will be shifted to. In general, the behavior of shift functions can be
 * defined as follows:
 *
 * @image html shift.gif ""
 *
 * Andes DSP library supports distinct shift functions for Q31, Q15, Q7 and other data types. These
 * functions are introduced in the subsections below.
 */
/**
 * @addtogroup basic_shift
 * @{
 */
/**
 * @brief Shifts a q31 vector with a specified shift number.
 * @param[in]       *src pointer of the input vector
 * @param[in]      shift signed shift value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to performance consideration, the shift value must be no greater than 32;
 * otherwise, the result may be incorrect.
 * 2. The result value will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 *
 * @b Example
 *  <pre>
 *      \#define size 1024
 *      q31_t src[size] = {…};
 *      q31_t dst[size];
 *      q31_t shift = 1;
 *      riscv_dsp_shift_q31(src, shift, dst, size);
 *
 * This example also serves as a reference for examples of Q15 or Q7 shift functions.
 *  </pre>
 */
void riscv_dsp_shift_q31(q31_t *src, int8_t shift, q31_t *dst, uint32_t size);

/**
 * @brief Shifts a q15 vector with a specified shift number.
 * @param[in]       *src pointer of the input vector
 * @param[in]      shift signed shift value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to performance consideration, the shift value must be no greater than 16; otherwise,
 * the result may be incorrect.
 * 2. The result value will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_shift_q15(q15_t *src, int8_t shift, q15_t *dst, uint32_t size);

/**
 * @brief Shifts a q7 vector with a specified shift number.
 * @param[in]       *src pointer of the input vector
 * @param[in]      shift signed shift value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to performance consideration, the shift value must be no greater than 24;
 * otherwise, the result may be incorrect.
 * 2. The result value will be saturated to the Q7 range [0x80, 0x7F].
 */
void riscv_dsp_shift_q7(q7_t *src, int8_t shift, q7_t *dst, uint32_t size);

/**
 * @brief Shifts a u8 vector for a specified shift number.
 * @param[in]       *src pointer of the input vector
 * @param[in]      shift signed shift value
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. Due to performance consideration, the shift value must be no greater than 24;
 * otherwise, the result may be incorrect.
 * 2. The result value will be saturated to the uint8_t range [0x00, 0xFF].
 */
void riscv_dsp_shift_u8(uint8_t *src, int8_t shift, uint8_t *dst, uint32_t size);
/** @} basic_shift */

// Clip
/**
 * @defgroup basic_clip Elementwise Clipping Functions
 * @brief  Elementwise Clipping Functions
 *
 * Elementwise clipping functions constraint the value in two bounds from elements of a source vector and write the results one-by-one into a destination vector.
 *
 * Andes DSP library supports distinct elementwise clipping functions for floating-point, Q31, Q15 and Q7 data types. These functions are introduced in the subsections below.
 */
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
void riscv_dsp_clip_f32(float32_t *src, float32_t *dst, float32_t low, float32_t high, uint32_t size);
/**
 * @brief Elementwise clipping of q31 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_clip_q31(q31_t *src, q31_t *dst, q31_t low, q31_t high, uint32_t size);
/**
 * @brief Elementwise clipping of q15 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_clip_q15(q15_t *src, q15_t *dst, q15_t low, q15_t high, uint32_t size);
/**
 * @brief Elementwise clipping of q7 function.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       low  lower bound.
 * @param[in]       high higher bound.
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_clip_q7(q7_t *src, q7_t *dst, q7_t low, q7_t high, uint32_t size);
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
void riscv_dsp_and_u32(u32_t *src1, u32_t *src2, u32_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise AND of two u16 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_and_u16(u16_t *src1, u16_t *src2, u16_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise AND of two u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_and_u8(u8_t *src1, u8_t *src2, u8_t *dst, uint32_t size);
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
void riscv_dsp_or_u32(u32_t *src1, u32_t *src2, u32_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise OR of two u16 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_or_u16(u16_t *src1, u16_t *src2, u16_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise OR of two u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_or_u8(u8_t *src1, u8_t *src2, u8_t *dst, uint32_t size);
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
void riscv_dsp_xor_u32(u32_t *src1, u32_t *src2, u32_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise XOR of two u16 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_xor_u16(u16_t *src1, u16_t *src2, u16_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise XOR of two u8 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_xor_u8(u8_t *src1, u8_t *src2, u8_t *dst, uint32_t size);
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
void riscv_dsp_not_u32(u32_t *src, u32_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise NOT of u16 vector.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_not_u16(u16_t *src, u16_t *dst, uint32_t size);
/**
 * @brief Compute the logical bitwise NOT of u8 vector.
 * @param[in]       *src pointer of the input vector
 * @param[out]      *dst pointer of the output vector
 * @param[in]       size number of elements in a vector
 * @return none.
 */
void riscv_dsp_not_u8(u8_t *src, u8_t *dst, uint32_t size);
/** @} basic_not */

/** @} basic */

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_BASIC_MATH_H__
