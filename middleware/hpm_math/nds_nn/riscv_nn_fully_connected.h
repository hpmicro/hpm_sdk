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

#ifndef __RISCV_NN_FULLY_CONNECTED_H__
#define __RISCV_NN_FULLY_CONNECTED_H__

#ifdef __cplusplus
extern    "C"
{
#endif

#include "riscv_math_types.h"

/**
 * @defgroup FullyConnect Fully Connected Functions
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
 * riscv_nn_fc_s8_s8_s8_sft_bias(in_vec, wt_mat, IN_SIZE, OUT_SIZE, BIAS_LSHIFT,
 *     OUT_RSHIFT, bias, out_vec, NULL);
 * @endcode
 */
int32_t riscv_nn_fc_s8_s8_s8_sft_bias(const q7_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t bias_lshift,
                                    const uint16_t out_rshift,
                                    const q7_t * bias,
                                    q7_t * out_vec,
                                    q15_t * in_tmp_buf);

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
int32_t riscv_nn_fc_s8_s8_s8_sft_bias_fast(const q7_t * in_vec,
                                        const q7_t * wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t bias_lshift,
                                        const uint16_t out_rshift,
                                        const q7_t * bias,
                                        q7_t * out_vec,
                                        q15_t * in_tmp_buf);

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
int32_t riscv_nn_fc_s16_s16_s16_sft_bias(const q15_t * in_vec,
                                        const q15_t * wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t bias_lshift,
                                        const uint16_t out_rshift,
                                        const q15_t * bias,
                                        q15_t * out_vec,
                                        q15_t * tmp_buf);

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
int32_t riscv_nn_fc_s16_s16_s16_sft_bias_fast(const q15_t * in_vec,
                                            const q15_t * wt_mat,
                                            const uint16_t size,
                                            const uint16_t wt_row_num,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            const q15_t * bias,
                                            q15_t * out_vec,
                                            q15_t * in_tmp_buf);

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
int32_t riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias(const q15_t * in_vec,
                                                const q7_t * wt_mat,
                                                const uint16_t size,
                                                const uint16_t wt_row_num,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                const q7_t * bias,
                                                q15_t * out_vec,
                                                q15_t * tmp_buf);

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
 * riscv_nn_fc_mat_vec_s8_wt_converter.
 */
int32_t riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias_fast(const q15_t * in_vec,
                                                    const q7_t * wt_mat,
                                                    const uint16_t size,
                                                    const uint16_t wt_row_num,
                                                    const uint16_t bias_lshift,
                                                    const uint16_t out_rshift,
                                                    const q7_t * bias,
                                                    q15_t * out_vec,
                                                    q15_t * tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s8_s8_sym_bias(const q7_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t * bias,
                                    q7_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s16_s8_sym_bias(const q7_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t * bias,
                                    q15_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_u8_s8_sym_bias(const u8_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t * bias,
                                    u8_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s8_s8_sym_bias(const u8_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t * bias,
                                    q7_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s16_s8_sym_bias(const u8_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    const q31_t * bias,
                                    q15_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s8_s8_sym(const q7_t * in_vec,
                                const q7_t * wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t * out_vec,
                                q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s16_s8_sym(const q7_t * in_vec,
                                const q7_t * wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q15_t * out_vec,
                                q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_u8_s8_sym(const u8_t * in_vec,
                                const q7_t * wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                u8_t * out_vec,
                                q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s8_s8_sym(const u8_t * in_vec,
                                const q7_t * wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t * out_vec,
                                q15_t * in_tmp_buf);

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
 * out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s16_s8_sym(const u8_t * in_vec,
                                const q7_t * wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q15_t * out_vec,
                                q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s8_s8_sym_bias_fast(const q7_t * in_vec,
                                        const q7_t * wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t * bias,
                                        q7_t * out_vec,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s16_s8_sym_bias_fast(const q7_t * in_vec,
                                            const q7_t * wt_mat,
                                            const uint16_t size,
                                            const uint16_t wt_row_num,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            const q31_t * bias,
                                            q15_t * out_vec,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_u8_s8_sym_bias_fast(const u8_t * in_vec,
                                        const q7_t * wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t * bias,
                                        u8_t * out_vec,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s8_s8_sym_bias_fast(const u8_t * in_vec,
                                        const q7_t * wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t * bias,
                                        q7_t * out_vec,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s16_s8_sym_bias_fast(const u8_t * in_vec,
                                        const q7_t * wt_mat,
                                        const uint16_t size,
                                        const uint16_t wt_row_num,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        const q31_t * bias,
                                        q15_t * out_vec,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s8_s8_sym_fast(const q7_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_s8_s16_s8_sym_fast(const q7_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_u8_s8_sym_fast(const u8_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    u8_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s8_s8_sym_fast(const u8_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t * out_vec,
                                    q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_fc_u8_s16_s8_sym_fast(const u8_t * in_vec,
                                    const q7_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t * out_vec,
                                    q15_t * in_tmp_buf);

/**
 * @brief           This is a weight converter for those fully-connected
 *                  functions with signed 8-bit weight data and named with
 *                  "fast".
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[out]      wt_mat_out      pointer of the weight matrix stored in
 *                                  specific ordering
 * @return          None
 */
void riscv_nn_fc_s8_wt_converter(const q7_t *wt_mat,
                                const uint32_t size,
                                const uint32_t wt_row_num,
                                q7_t *wt_mat_out);

/**
 * @brief           This is a weight converter for those fully-connected
 *                  functions with signed 16-bit weight data and named with
 *                  "fast".
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[out]      wt_mat_out      pointer of the weight matrix stored in
 *                                  specific ordering
 * @return          None
 */
void riscv_nn_fc_s16_wt_converter(const q15_t *wt_mat,
                                const uint32_t size,
                                const uint32_t wt_row_num,
                                q15_t *wt_mat_out);

/**
 * @brief           This is a weight converter for
 *                  riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias_fast.
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[out]      wt_mat_out      pointer of the weight matrix stored in
 *                                  specific ordering
 * @return          None
 */
void riscv_nn_fc_mat_vec_s8_wt_converter(const q7_t *wt_mat,
                                        const uint32_t size,
                                        const uint32_t wt_row_num,
                                        q7_t *wt_mat_out);

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
int32_t riscv_nn_fc_s8_s8_s8_asym_bias(const int8_t *in_vec,
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
                                    q15_t *tmp_buf);

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the temporary buffer of riscv_nn_fc_s8_s8_s8_asym_bias.
 * @param[in]       in_vec_col      number of columns in the input vector (or
 *                                  transposed weight matrix)
 * @return          This function returns the needed size by the temporary buffer.
 */
int32_t riscv_nn_fc_s8_s8_s8_asym_bias_get_buffer_size(const uint16_t in_vec_col);

#ifdef __riscv_zfh
/**
 * @brief           This is a fully connected layer function for 16-bit
 *                  half-precision floating point inputs/outputs.
 * @param[in]       in_vec          pointer of the input vector
 * @param[in]       wt_mat          pointer of the weight matrix
 * @param[in]       size            number of elements in the input vector
 * @param[in]       wt_row_num      number of rows in the weight matrix
 * @param[in]       bias            pointer of the bias vector
 * @param[out]      out_vec         pointer of the output vector
 * @param[in]       tmp_buf         dummy
 * @return          This function only returns 0.
 */
int32_t riscv_nn_fc_f16_f16_f16_bias(const float16_t * in_vec,
                                    const float16_t * wt_mat,
                                    const uint16_t size,
                                    const uint16_t wt_row_num,
                                    const float16_t * bias,
                                    float16_t * out_vec,
                                    float16_t * tmp_buf);
#endif

/**
 *   * @}
 */

#ifdef __cplusplus
}
#endif

#endif
