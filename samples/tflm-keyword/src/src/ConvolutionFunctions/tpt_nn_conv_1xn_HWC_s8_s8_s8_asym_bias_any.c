/******************************************************************************
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.*
 * Copyright (C) 2018-2023 Andes Technology Corporation. All rights reserved. *
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

#include "internal_nn_math.h"
#include "tpt_nn_support.h"
#include "tpt_nn_convolution.h"

//// Convolution Functions

/*
   * 1xN s8 convolution function.
   *
   * Refer header file for details.
   *
   */

int tpt_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
                            const uint16_t in_tensor_dim_x,
                            const uint16_t in_tensor_ch,
                            const uint16_t in_tensor_batch,
                            const q7_t *ker_weight,
                            const uint16_t out_tensor_ch,
                            const uint16_t ker_dim_x,
                            const uint16_t pad_x,
                            const uint16_t stride_x,
                            const int32_t *bias,
                            q7_t *out_tensor,
                            const int32_t *out_shift,
                            const int32_t *out_scale,
                            const int32_t out_offset,   //value is in the range of [-127, 128]
                            const int32_t in_offset,    //value is in the range of [-128, 127]
                            const int32_t act_min,
                            const int32_t act_max,
                            const uint16_t out_tensor_dim_x,
                            q15_t *in_tmp_buf)
{
    (void)in_tensor_batch;

    int status = 0;
    if (out_tensor_dim_x % 4 != 0)
    {
        status = -1;
        return status;
    }


    (void)in_tmp_buf;
    int32_t i_out_ch, i_out_x, i_input_ch, i_ker_x;
    int32_t conv_out;

    for (i_out_ch = 0; i_out_ch < out_tensor_ch; i_out_ch++)
    {
        for (i_out_x = 0; i_out_x < out_tensor_dim_x; i_out_x++)
        {
            conv_out = bias[i_out_ch];

            const int32_t base_idx_y = 0;
            const int32_t base_idx_x = stride_x * i_out_x - pad_x;

            const int32_t ker_x_start = MAX(0, -base_idx_x);
            const int32_t ker_x_end = MIN(ker_dim_x, in_tensor_dim_x - base_idx_x);

            for (i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
            {
                const int32_t in_row = base_idx_y;
                const int32_t in_col = base_idx_x + i_ker_x;
                for (i_input_ch = 0; i_input_ch < in_tensor_ch; i_input_ch++)
                {
                    conv_out +=
                        (in_tensor[(in_row * in_tensor_dim_x + in_col) * in_tensor_ch + i_input_ch] + in_offset) *
                        ker_weight[i_out_ch * in_tensor_ch * ker_dim_x + (+ i_ker_x) * in_tensor_ch + i_input_ch];
                }
            }

            conv_out = tpt_nn_requantize(conv_out, out_scale[i_out_ch], out_shift[i_out_ch]);
            conv_out += out_offset;
            conv_out = MAX(conv_out, act_min);
            conv_out = MIN(conv_out, act_max);
            out_tensor[i_out_ch + (i_out_x) * out_tensor_ch] = conv_out;
        }
    }

    /* Return to application */
    return status;
}

//Note. the ker_dim_y paramter here is redundant since it's always 1.
int32_t tpt_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(const uint16_t in_tensor_ch,
                                                const uint16_t ker_dim_x,
                                                const uint16_t ker_dim_y)
{
    (void)in_tensor_ch;
    (void)ker_dim_x;
    (void)ker_dim_y;
    return 0;
}