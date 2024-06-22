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

//// Pooling Functions

int32_t tpt_nn_avepool_HWC_s8_any_act(const int in_tensor_dim_y,
                                        const int in_tensor_dim_x,
                                        const int out_tensor_dim_y,
                                        const int out_tensor_dim_x,
                                        const int stride_y,
                                        const int stride_x,
                                        const int ker_dim_y,
                                        const int ker_dim_x,
                                        const int pad_y,
                                        const int pad_x,
                                        const int act_min,
                                        const int act_max,
                                        const int in_tensor_ch,
                                        int8_t *in_tensor,
                                        int16_t *in_tmp_buf,
                                        int8_t *out_tensor)
{
    (void)in_tmp_buf;
    int16_t   i_ch_in, i_x, i_y;
    int16_t   k_x, k_y;

    for (i_y = 0; i_y < out_tensor_dim_y; i_y++)
    {
        for (i_x = 0; i_x < out_tensor_dim_x; i_x++)
        {
            for (i_ch_in = 0; i_ch_in < in_tensor_ch; i_ch_in++)
            {
                int       sum = 0;
                int       count = 0;
                for (k_y = i_y * stride_y - pad_y; k_y < i_y * stride_y - pad_y + ker_dim_y; k_y++)
                {
                    for (k_x = i_x * stride_x - pad_x; k_x < i_x * stride_x - pad_x + ker_dim_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < in_tensor_dim_y && k_x < in_tensor_dim_x)
                        {
                            sum += in_tensor[i_ch_in + in_tensor_ch * (k_x + k_y * in_tensor_dim_x)];
                            count++;
                        }
                    }
                }
                sum = sum > 0 ? (sum + count / 2) / count : (sum - count / 2) / count;
                sum = MAX(sum, act_min);
                sum = MIN(sum, act_max);

                out_tensor[i_ch_in + in_tensor_ch * (i_x + i_y * out_tensor_dim_x)] = sum;
            }
        }
    }

    return 0;
}

int32_t tpt_nn_avepool_HWC_s8_any_act_get_buffer_size(const int out_tensor_dim_x,
                                                const int in_tensor_ch)
{
    (void)out_tensor_dim_x;
    (void)in_tensor_ch;
    return 0;
}
