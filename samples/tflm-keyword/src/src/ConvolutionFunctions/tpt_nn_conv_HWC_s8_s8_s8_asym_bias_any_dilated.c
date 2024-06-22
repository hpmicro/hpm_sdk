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

//// Convolution Functions
int32_t tpt_nn_conv_HWC_s8_s8_s8_asym_bias_any_dilated(const q7_t *in_tensor,
    const uint16_t in_tensor_dim_x,
    const uint16_t in_tensor_dim_y,
    const uint16_t in_tensor_ch,
    const uint16_t in_tensor_batch,
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
    const int32_t out_offset,    //value is in the range of [-128, 127]
    const int32_t in_offset,     //value is in the range of [-127, 128]
    const int32_t act_min,
    const int32_t act_max,
    const uint16_t out_tensor_dim_x,
    const uint16_t out_tensor_dim_y,
    const int32_t dilation_x,
    const int32_t dilation_y,
    q15_t *in_tmp_buf)
{
    int i_batch;
    for (i_batch = 0; i_batch < in_tensor_batch; i_batch++)
    {
        in_tensor += i_batch * (in_tensor_dim_x * in_tensor_dim_y * in_tensor_ch);
        out_tensor += i_batch * (out_tensor_dim_x * out_tensor_dim_y * out_tensor_ch);
        (void)in_tmp_buf;
        int32_t i_out_ch, i_out_y, i_out_x, i_input_ch, i_ker_y, i_ker_x;
        int32_t conv_out;

        for (i_out_ch = 0; i_out_ch < out_tensor_ch; i_out_ch++)
        {
            for (i_out_y = 0; i_out_y < out_tensor_dim_y; i_out_y++)
            {
                for (i_out_x = 0; i_out_x < out_tensor_dim_x; i_out_x++)
                {
                    conv_out = 0;
                    if (bias)
                    {
                        conv_out = bias[i_out_ch];
                    }

                    const int32_t base_idx_y = stride_y * i_out_y - pad_y;
                    const int32_t base_idx_x = stride_x * i_out_x - pad_x;
                    const int32_t start_y_max = (-base_idx_y + dilation_y - 1) / dilation_y;
                    const int32_t start_x_max = (-base_idx_x + dilation_x - 1) / dilation_x;
                    const int32_t ker_y_start = MAX(0, start_y_max);
                    const int32_t ker_x_start = MAX(0, start_x_max);
                    const int32_t end_min_y = (in_tensor_dim_y - base_idx_y + dilation_y - 1) / dilation_y;
                    const int32_t end_min_x = (in_tensor_dim_x - base_idx_x + dilation_x - 1) / dilation_x;
                    const int32_t ker_y_end = MIN(ker_dim_y, end_min_y);
                    const int32_t ker_x_end = MIN(ker_dim_x, end_min_x);

                    for (i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        for (i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            const int32_t in_row = base_idx_y + dilation_y * i_ker_y;
                            const int32_t in_col = base_idx_x + dilation_x * i_ker_x;
                            for (i_input_ch = 0; i_input_ch < in_tensor_ch; i_input_ch++)
                            {
                                conv_out +=
                                    (in_tensor[(in_row * in_tensor_dim_x + in_col) * in_tensor_ch + i_input_ch] + in_offset) *
                                    ker_weight[i_out_ch * in_tensor_ch * ker_dim_y * ker_dim_x +
                                           (i_ker_y * ker_dim_x + i_ker_x) * in_tensor_ch + i_input_ch];
                            }
                        }
                    }
                    conv_out = tpt_nn_requantize(conv_out, out_scale[i_out_ch], out_shift[i_out_ch]);
                    conv_out += out_offset;
                    conv_out = MAX(conv_out, act_min);
                    conv_out = MIN(conv_out, act_max);
                    out_tensor[i_out_ch + (i_out_y * out_tensor_dim_x + i_out_x) * out_tensor_ch] = (int8_t)conv_out;
                }
            }
        }
    }

    /* Return to application */
    return 0;
}

int32_t tpt_nn_conv_HWC_s8_s8_s8_asym_bias_any_dilated_get_buffer_size(const uint16_t in_tensor_ch,
    const uint16_t ker_dim_x,
    const uint16_t ker_dim_y)
{
    (void)in_tensor_ch;
    (void)ker_dim_x;
    (void)ker_dim_y;
    return 0;
}
