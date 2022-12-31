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

/** @file*/

#ifndef __RISCV_NN_BASIC_OPERATION_H__
#define __RISCV_NN_BASIC_OPERATION_H__

#ifdef __cplusplus
extern    "C"
{
#endif

#include "riscv_math_types.h"

/**
 * @defgroup Basic Basic Functions
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
 * @return          None
 *
 * @note
 * The calculation of each element can be represented as Figure 1, where
 * rectangles represent inputs or outputs, circles represent arithmetical
 * operations, solid lines are for required steps and  dashed lines, if any, are
 * for optional ones.
 *
 * @image html riscv_nn_add_s8_sym.jpg "Figure 1. riscv_nn_add_s8_sym algorithm flowchart" width=600px
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
 * riscv_nn_add_s8_sym(in_tensor1, in_tensor2, scale1, scale2, SIZE, pre_rshift,
 *     out_scale, post_rshift, out);
 * @endcode
 */
void riscv_nn_add_s8_sym(const q7_t * in_tensor1,
                        const q7_t * in_tensor2,
                        const int16_t * scale1,
                        const int16_t * scale2,
                        const uint32_t size,
                        const uint16_t pre_rshift,
                        const uint16_t out_scale,
                        const uint16_t post_rshift,
                        q7_t * out);

/**
 * @brief           This function performs element-wise addition for signed
 *                  8-bit integer input vectors with two-stage shift with
 *                  rounding.
 * @param[in]       in_tensor1  pointer of the first input vector
 * @param[in]       in_tensor2  pointer of the second input vector
 * @param[in]       scale1      scaling value for the first input vector. It
 *                              should be in the range of  0 to @f{2^23}.
 * @param[in]       scale2      scaling value for the second input vector. It
 *                              should be in the range of 0 to @f{2^23}.
 * @param[in]       size        number of elements in the input vectors
 * @param[in]       pre_rshift  right shift amount for the accumulator before
 *                              the scaling
 * @param[in]       out_scale   scaling value for the accumulator
 * @param[in]       post_rshift right shift amount for the accumulator after the
 *                              scaling
 * @param[out]      out         pointer of element-wise addition results
 * @return          None
 *
 * @note
 * - The calculation of each element could be represented as Figure 2.
 *   @image html riscv_nn_add_s8_sym_round.jpg "Figure 2. riscv_nn_ew_add_s8_asym algorithm flowchart" width=600px
 * - The right shift operations for this function include rounding.
 */
void riscv_nn_add_s8_sym_round(const q7_t * in_tensor1,
                            const q7_t * in_tensor2,
                            const uint32_t scale1,
                            const uint32_t scale2,
                            const uint32_t size,
                            const uint16_t pre_rshift,
                            const uint16_t out_scale,
                            const uint16_t post_rshift,
                            q7_t * out);

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
 * @note
 * - The calculation of each element could be represented as Figure 3.
 *   @image html riscv_nn_ew_add_s8_asym.jpg "Figure 3. riscv_nn_ew_add_s8_asym algorithm flowchart" width=600px
 * - The multiplication for in_scale1, in_scale2 and out_scale could be roughly
 *   expressed as:
 *   @f[32b = ((int64_t)32b * 32b) >> 31@f]
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
 * riscv_nn_ew_add_s8_asym(in_tensor1, in_tensor2, in_offset1, in_scale1,
 *     in_rshift1, in_offset2, in_scale2, in_rshift2, lshift, out, out_offset,
 *     out_scale, out_rshift, act_min, act_max, SIZE);
 * @endcode
 */
int riscv_nn_ew_add_s8_asym(const int8_t *in_tensor1,
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
                            const uint32_t size);

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
 * @note
 * - The calculation of each element could be represented as Figure 4.
 *   @image html riscv_nn_ew_mul_s8_asym.jpg "Figure 4. riscv_nn_ew_mul_s8_asym algorithm flowchart" width=600px
 * - The multiplication for out_scale could be roughly expressed as:
 *   @f$32b = ((int64_t)32b * 32b) >> 31@f$
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
 * riscv_nn_ew_mul_s8_asym(in_tensor1, in_tensor2, in_offset1, in_offset2, out,
 *     out_offset, out_scale, out_shift, act_min, act_max, SIZE);
 * @endcode
 */
int riscv_nn_ew_mul_s8_asym(const int8_t *in_tensor1,
                            const int8_t *in_tensor2,
                            const int32_t in_offset1,
                            const int32_t in_offset2,
                            int8_t *out,
                            const int32_t out_offset,
                            const int32_t out_scale,
                            const int32_t out_shift,
                            const int32_t act_min,
                            const int32_t act_max,
                            const uint32_t size);

/**
 *   * @}
 */

#ifdef __cplusplus
}
#endif

#endif
