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

#ifndef __RISCV_NN_CONVOLUTION_H__
#define __RISCV_NN_CONVOLUTION_H__

#ifdef __cplusplus
extern    "C"
{
#endif

#include "riscv_math_types.h"

/**
 * @defgroup Convolution Convolution Functions
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
 *  //160x120x8 output tensor. Let both dimensions’ padding be 0 and their
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

int32_t riscv_nn_conv_1x1_HWC_s8_s8_s8_sft_bias_fast_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf,
                                                q7_t * tmp_buf);

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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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

int32_t riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q7_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t * in_tmp_buf,
                                                q7_t * tmp_buf);

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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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
int32_t riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias_fast(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q7_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t * in_tmp_buf,
                                                q15_t * wt_tmp_buf);


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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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

int32_t riscv_nn_conv_HWC_s8_s8_s8_sft_bias(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q7_t * bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q7_t * tmp_buf);

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
 * @return          None
 *
 * @b Example:
 * @code
 *  //Convolve a 160x120x3 input tensor with a 3x5 kernel and generate a 80x59x5
 *  //output tensor. Let both dimensions’ padding be 1 and their stride be 2.
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

void riscv_nn_conv_HWC_s8_s8_s8_sft_bias_any(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q7_t * bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf,
                                            q7_t * tmp_buf);

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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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

int32_t riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast(const q7_t * in_tensor,
                                const uint16_t in_tensor_dim,
                                const uint16_t in_tensor_ch,
                                const q7_t * ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ker_dim,
                                const uint16_t pad,
                                const uint16_t stride,
                                const q7_t * bias,
                                const uint16_t bias_lshift,
                                const uint16_t out_rshift,
                                q7_t * out_tensor,
                                const uint16_t out_tensor_dim,
                                q15_t * in_tmp_buf,
                                q7_t * tmp_buf);

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
 *  //80x59x8 output tensor. Let both dimensions’ padding be 1 and their stride
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

int32_t riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf,
                                                q7_t * tmp_buf);


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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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

int32_t riscv_nn_conv_HWC_s16_s16_s16_sft_bias(const q15_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q15_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q15_t * bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q7_t * tmp_buf);

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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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

int32_t riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast(const q15_t * in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const uint16_t in_tensor_ch,
                                                const q15_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q15_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t * in_tmp_buf,
                                                q7_t * tmp_buf);

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
 *  //80x59x8 output tensor. Let both dimensions’ padding be 1 and their stride
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

int32_t riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast_any(const q15_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q15_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q15_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf,
                                                q7_t * tmp_buf);

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
 *  //output tensor. Let both dimensions’ padding be 0 and their stride be 1.
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

int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q7_t * bias,
                                            const uint16_t bias_lshift,
                                            const uint16_t out_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q7_t * tmp_buf);

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
 *  //kernel and generate a 77x57x12 output tensor. Let both dimensions’ padding
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
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t * bias,
                                                const uint16_t bias_lshift,
                                                const uint16_t out_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf,
                                                q7_t * tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_s8_s8_s8_sym_bias_fast_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_s8_s16_s8_sym_bias_fast_any(const q7_t * in_tensor,
                                                    const uint16_t in_tensor_dim_x,
                                                    const uint16_t in_tensor_dim_y,
                                                    const uint16_t in_tensor_ch,
                                                    const q7_t * ker_weight,
                                                    const uint16_t out_tensor_ch,
                                                    const uint16_t ker_dim_x,
                                                    const uint16_t ker_dim_y,
                                                    const uint16_t pad_x,
                                                    const uint16_t pad_y,
                                                    const uint16_t stride_x,
                                                    const uint16_t stride_y,
                                                    const q31_t * bias,
                                                    const uint16_t pre_rshift,
                                                    const uint16_t out_scale,
                                                    const uint16_t post_rshift,
                                                    q15_t * out_tensor,
                                                    const uint16_t out_tensor_dim_x,
                                                    const uint16_t out_tensor_dim_y,
                                                    q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_u8_u8_s8_sym_bias_fast_any(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                u8_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_u8_s8_s8_sym_bias_fast_any(const u8_t * in_tensor,
                                                    const uint16_t in_tensor_dim_x,
                                                    const uint16_t in_tensor_dim_y,
                                                    const uint16_t in_tensor_ch,
                                                    const q7_t * ker_weight,
                                                    const uint16_t out_tensor_ch,
                                                    const uint16_t ker_dim_x,
                                                    const uint16_t ker_dim_y,
                                                    const uint16_t pad_x,
                                                    const uint16_t pad_y,
                                                    const uint16_t stride_x,
                                                    const uint16_t stride_y,
                                                    const q31_t * bias,
                                                    const uint16_t pre_rshift,
                                                    const uint16_t out_scale,
                                                    const uint16_t post_rshift,
                                                    q7_t * out_tensor,
                                                    const uint16_t out_tensor_dim_x,
                                                    const uint16_t out_tensor_dim_y,
                                                    q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_u8_s16_s8_sym_bias_fast_any(const u8_t * in_tensor,
                                                    const uint16_t in_tensor_dim_x,
                                                    const uint16_t in_tensor_dim_y,
                                                    const uint16_t in_tensor_ch,
                                                    const q7_t * ker_weight,
                                                    const uint16_t out_tensor_ch,
                                                    const uint16_t ker_dim_x,
                                                    const uint16_t ker_dim_y,
                                                    const uint16_t pad_x,
                                                    const uint16_t pad_y,
                                                    const uint16_t stride_x,
                                                    const uint16_t stride_y,
                                                    const q31_t * bias,
                                                    const uint16_t pre_rshift,
                                                    const uint16_t out_scale,
                                                    const uint16_t post_rshift,
                                                    q15_t * out_tensor,
                                                    const uint16_t out_tensor_dim_x,
                                                    const uint16_t out_tensor_dim_y,
                                                    q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_s8_s8_s8_sym_fast_any(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
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
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_s8_s16_s8_sym_fast_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
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
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_u8_u8_s8_sym_fast_any(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
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
                                            u8_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_u8_s8_s8_sym_fast_any(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
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
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *   out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_1x1_HWC_u8_s16_s8_sym_fast_any(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
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
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_RGB_sym_bias_fast(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s16_s8_RGB_sym_bias_fast(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t * in_tmp_buf,
                                                q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_u8_s8_RGB_sym_bias_fast(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s8_s8_RGB_sym_bias_fast(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t * in_tmp_buf,
                                                q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s16_s8_RGB_sym_bias_fast(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim,
                                                const uint16_t pad,
                                                const uint16_t stride,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim,
                                                q15_t * in_tmp_buf,
                                                q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_RGB_sym_fast(const q7_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf,
                                        q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s16_s8_RGB_sym_fast(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_u8_s8_RGB_sym_fast(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf,
                                        q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s8_s8_RGB_sym_fast(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q15_t * wt_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s16_s8_RGB_sym_fast(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf,
                                            q15_t * wt_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_sym_bias_fast(const q7_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t * bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s16_s8_sym_bias_fast(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_u8_s8_sym_bias_fast(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t * bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s8_s8_sym_bias_fast(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s16_s8_sym_bias_fast(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_sym_fast(const q7_t * in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t * out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s16_s8_sym_fast(const q7_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q15_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_u8_s8_sym_fast(const u8_t * in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    u8_t * out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s8_s8_sym_fast(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s16_s8_sym_fast(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q15_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_sym_bias_fast_any(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s16_s8_sym_bias_fast_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_u8_s8_sym_bias_fast_any(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s8_s8_sym_bias_fast_any(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q7_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s16_s8_sym_bias_fast_any(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_sym_fast_any(const q7_t * in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
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
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_s8_s16_s8_sym_fast_any(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
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
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_u8_s8_sym_fast_any(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
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
                                        u8_t * out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s8_s8_sym_fast_any(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
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
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *    out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_HWC_u8_s16_s8_sym_fast_any(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
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
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);


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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_sym_bias(const q7_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t * bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s16_s8_sym_bias(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_u8_s8_sym_bias(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t * bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        u8_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s8_s8_sym_bias(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const q31_t * bias,
                                        const uint16_t pre_rshift,
                                        const uint16_t out_scale,
                                        const uint16_t post_rshift,
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s16_s8_sym_bias(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q15_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_sym(const q7_t * in_tensor,
                                const uint16_t in_tensor_dim,
                                const uint16_t in_tensor_ch,
                                const q7_t * ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ker_dim,
                                const uint16_t pad,
                                const uint16_t stride,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t * out_tensor,
                                const uint16_t out_tensor_dim,
                                q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s16_s8_sym(const q7_t * in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t * out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_u8_s8_sym(const u8_t * in_tensor,
                                const uint16_t in_tensor_dim,
                                const uint16_t in_tensor_ch,
                                const q7_t * ker_weight,
                                const uint16_t out_tensor_ch,
                                const uint16_t ker_dim,
                                const uint16_t pad,
                                const uint16_t stride,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                u8_t * out_tensor,
                                const uint16_t out_tensor_dim,
                                q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s8_s8_sym(const u8_t * in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q7_t * out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s16_s8_sym(const u8_t * in_tensor,
                                    const uint16_t in_tensor_dim,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t ker_dim,
                                    const uint16_t pad,
                                    const uint16_t stride,
                                    const uint16_t pre_rshift,
                                    const uint16_t out_scale,
                                    const uint16_t post_rshift,
                                    q15_t * out_tensor,
                                    const uint16_t out_tensor_dim,
                                    q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_sym_bias_any(const q7_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s16_s8_sym_bias_any(const q7_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_u8_s8_sym_bias_any(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            u8_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s8_s8_sym_bias_any(const u8_t * in_tensor,
                                            const uint16_t in_tensor_dim_x,
                                            const uint16_t in_tensor_dim_y,
                                            const uint16_t in_tensor_ch,
                                            const q7_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim_x,
                                            const uint16_t ker_dim_y,
                                            const uint16_t pad_x,
                                            const uint16_t pad_y,
                                            const uint16_t stride_x,
                                            const uint16_t stride_y,
                                            const q31_t * bias,
                                            const uint16_t pre_rshift,
                                            const uint16_t out_scale,
                                            const uint16_t post_rshift,
                                            q7_t * out_tensor,
                                            const uint16_t out_tensor_dim_x,
                                            const uint16_t out_tensor_dim_y,
                                            q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s16_s8_sym_bias_any(const u8_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const q7_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q31_t * bias,
                                                const uint16_t pre_rshift,
                                                const uint16_t out_scale,
                                                const uint16_t post_rshift,
                                                q15_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_sym_any(const q7_t * in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
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
                                    q7_t * out_tensor,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s16_s8_sym_any(const q7_t * in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
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
                                        q15_t * out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_u8_s8_sym_any(const u8_t * in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const q7_t * ker_weight,
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
                                    u8_t * out_tensor,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s8_s8_sym_any(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
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
                                        q7_t * out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t * in_tmp_buf);

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
 *  out = ((out >> pre_rshift) * out_scale) >> post_rshift.
 */
int32_t riscv_nn_conv_dw_HWC_u8_s16_s8_sym_any(const u8_t * in_tensor,
                                        const uint16_t in_tensor_dim_x,
                                        const uint16_t in_tensor_dim_y,
                                        const uint16_t in_tensor_ch,
                                        const q7_t * ker_weight,
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
                                        q15_t * out_tensor,
                                        const uint16_t out_tensor_dim_x,
                                        const uint16_t out_tensor_dim_y,
                                        q15_t * in_tmp_buf);

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
int32_t riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
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
                                    q15_t *tmp_buf);

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @return          This function returns the needed size by the temporary buffer.
 */
int32_t riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch);

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
int riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
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
                                                q15_t *in_tmp_buf);

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel. It is
 *                                      always 1 here.
 * @return          This function returns the needed size by the temporary buffer.
 */
int32_t riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y);

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
int32_t riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
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
                                                q15_t *in_tmp_buf);

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @return          This function returns the needed size by the temporary buffer.
 */
int32_t riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                        const uint16_t ker_dim_x,
                                        const uint16_t ker_dim_y);

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
int32_t riscv_nn_conv_dw_HWC_3x3_s8_s8_s8_asym_bias_any(const int8_t *in_tensor,
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
                                                int16_t *tmp_buf);

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
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
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
                                q15_t *tmp_buf);

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
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
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
                                     q15_t *in_tmp_buf);

/**
 * @brief           This function is used to get the needed size, in bytes, by
 *                  the input temporary buffer of riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any.
 * @param[in]       in_tensor_ch        number of input tensor channels
 * @param[in]       ker_dim_x           x dimension of the filter kernel
 * @param[in]       ker_dim_y           y dimension of the filter kernel
 * @return          This function returns the needed size by the temporary buffer.
 */
int32_t riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                  const uint16_t ker_dim_x,
                                                  const uint16_t ker_dim_y);

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
int32_t riscv_nn_conv_dw_HWC_u8_u8_u8_asym_bias_any(const uint8_t *in_tensor,
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
                                    const int32_t out_scale);

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
int32_t riscv_nn_conv_1x1_HWC_f16_f16_f16_bias_any(const float16_t * in_tensor,
                                                const uint16_t in_tensor_dim_x,
                                                const uint16_t in_tensor_dim_y,
                                                const uint16_t in_tensor_ch,
                                                const float16_t * ker_weight,
                                                const uint16_t out_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y,
                                                const uint16_t pad_x,
                                                const uint16_t pad_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const float16_t * bias,
                                                float16_t * out_tensor,
                                                const uint16_t out_tensor_dim_x,
                                                const uint16_t out_tensor_dim_y,
                                                float16_t * in_tmp_buf,
                                                float16_t * tmp_buf);

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
int32_t riscv_nn_conv_HWC_f16_f16_f16_bias(const float16_t * in_tensor,
                                        const uint16_t in_tensor_dim,
                                        const uint16_t in_tensor_ch,
                                        const float16_t * ker_weight,
                                        const uint16_t out_tensor_ch,
                                        const uint16_t ker_dim,
                                        const uint16_t pad,
                                        const uint16_t stride,
                                        const float16_t * bias,
                                        float16_t * out_tensor,
                                        const uint16_t out_tensor_dim,
                                        float16_t * in_tmp_buf,
                                        float16_t * tmp_buf);

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
int32_t riscv_nn_conv_dw_HWC_f16_f16_f16_bias(const float16_t * in_tensor,
                                            const uint16_t in_tensor_dim,
                                            const uint16_t in_tensor_ch,
                                            const float16_t * ker_weight,
                                            const uint16_t out_tensor_ch,
                                            const uint16_t ker_dim,
                                            const uint16_t pad,
                                            const uint16_t stride,
                                            const float16_t * bias,
                                            float16_t * out_tensor,
                                            const uint16_t out_tensor_dim,
                                            float16_t * in_tmp_buf,
                                            float16_t * tmp_buf);
#endif

/**
 *   * @}
 */

#ifdef __cplusplus
}
#endif

#endif
