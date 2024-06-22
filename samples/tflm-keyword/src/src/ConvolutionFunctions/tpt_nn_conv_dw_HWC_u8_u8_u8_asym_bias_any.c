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

int32_t tpt_nn_conv_dw_HWC_u8_u8_u8_asym_bias_any(const uint8_t *in_tensor,
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
                                    const int32_t in_offset,    //assume (in + offset) is in the range of int16
                                    const int32_t ker_offset,   //assume (kernel + offset) is in the range of int16
                                    const int32_t out_offset,   //assume (output + offset) is in the range of int16
                                    uint8_t *out_tensor,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    const int32_t out_shift,
                                    const int32_t out_scale)
{
    int32_t status = 0;

    //check input constraints
    if((ch_mult & 1) || (ker_dim_x & 1))
    {
        return -1;
    }
    //to be checked: it seems pad_x must be even
    // if(pad_x & 1)
    // {
    //     return 1;
    // }
    for (int32_t i_out_y = 0; i_out_y < out_tensor_dim_y; i_out_y++)
    {
        int32_t base_idx_y = (i_out_y * stride_y) - pad_y;
        for (int32_t i_out_x = 0; i_out_x < out_tensor_dim_x; i_out_x++)
        {
            int32_t base_idx_x = (i_out_x * stride_x) - pad_x;
            for (int32_t i_input_ch = 0; i_input_ch < in_tensor_ch; i_input_ch++)
            {
                int32_t i_ch_mult = 0;
                for (; i_ch_mult < ch_mult; i_ch_mult++)
                {
                    int32_t idx_out_ch = i_ch_mult + i_input_ch * ch_mult;
                    // calculate ker_weight start position
                    // (base_idx_x + ker_x_start) >= 0
                    // (base_idx_y + ker_y_start) >= 0
                    int32_t ker_y_start = MAX(0, -base_idx_y);
                    int32_t ker_x_start = MAX(0, -base_idx_x);
                    // calculate ker_weight end position
                    // (base_idx_x + ker_x_end) < in_tensor_dim_x
                    // (base_idx_y + ker_y_end) < in_tensor_dim_y
                    int32_t ker_y_end = MIN(ker_dim_y, in_tensor_dim_y - base_idx_y);
                    int32_t ker_x_end = MIN(ker_dim_x, in_tensor_dim_x - base_idx_x);
                    int32_t acc_0 = bias[idx_out_ch];

                    for (int32_t i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        int32_t idx_y = base_idx_y + i_ker_y;
                        for (int32_t i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            int32_t idx_x = base_idx_x + i_ker_x;
                            int32_t idx_0 = (idx_y * in_tensor_dim_x + idx_x) * in_tensor_ch + i_input_ch;
                            int32_t ker_idx_0 = (i_ker_y * ker_dim_x + i_ker_x) * (in_tensor_ch * ch_mult) + idx_out_ch;

                            acc_0 += (in_tensor[idx_0] + in_offset) * (ker_weight[ker_idx_0] + ker_offset);
                        }
                    }

                    // Requantize and clip output into specified range
                    acc_0 = tpt_nn_requantize(acc_0, out_scale, out_shift);
                    acc_0 += out_offset;
                    acc_0 = MAX(acc_0, act_min);
                    acc_0 = MIN(acc_0, act_max);
                    *out_tensor++ = acc_0;
                }
            }
        }
    }
    return status;
}
