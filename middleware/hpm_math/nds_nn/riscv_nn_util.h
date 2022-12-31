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

#ifndef __RISCV_NN_UTIL_H__
#define __RISCV_NN_UTIL_H__

#ifdef __cplusplus
extern    "C"
{
#endif

#include "riscv_math_types.h"

/**
 * @defgroup Utils Utils Functions
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
int32_t riscv_nn_exp_f16(const float16_t * in_vec,
                        const uint32_t size,
                        float16_t * out_vec);
#endif

/**
 * @brief           This function turns the input tensor into another tensor
 *                  with the same data but in a different shape.
 * @param[in]       in_tensor       pointer of the input tensor
 * @param[out]      out_tensor      pointer of the output tensor
 * @param[in]       size            size, in bytes, of total input tensor
 * @return          None
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * int8_t in_tensor[SIZE] = {...};
 * int8_t out_tensor[SIZE];
 *
 * riscv_nn_reshape_s8(in_tensor, out_tensor, SIZE);
 * @endcode
 */
void riscv_nn_reshape_s8(const int8_t *in_tensor,
                        int8_t *out_tensor,
                        const uint32_t size);

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
int32_t riscv_nn_top_k_s8(q7_t *in_vec,
                        uint32_t size,
                        uint32_t k,
                        q7_t *val,
                        uint32_t *idx);

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
int32_t riscv_nn_top_k_f16(float16_t *in_vec,
                        uint32_t size,
                        uint32_t k,
                        float16_t *val,
                        uint32_t *idx);
#endif

/**
 *   * @}
 */

#ifdef __cplusplus
}
#endif

#endif
