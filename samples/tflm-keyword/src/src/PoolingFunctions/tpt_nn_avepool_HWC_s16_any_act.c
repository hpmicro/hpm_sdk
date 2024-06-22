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

int32_t tpt_nn_avepool_HWC_s16_any_act(const int in_tensor_dim_y,
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
    int16_t *in_tensor,
    int16_t *in_tmp_buf,
    int16_t *out_tensor)
{
    (void)in_tmp_buf;

    for (int i_y = 0, base_idx_y = -pad_y; i_y < out_tensor_dim_y; base_idx_y += stride_y, i_y++)
    {
        for (int i_x = 0, base_idx_x = -pad_x; i_x < out_tensor_dim_x; base_idx_x += stride_x, i_x++)
        {
            const int32_t ker_y_start = MAX(0, -base_idx_y);
            const int32_t ker_x_start = MAX(0, -base_idx_x);

            const int32_t ker_dim_y_end = MIN(ker_dim_y, in_tensor_dim_y - base_idx_y);
            const int32_t ker_dim_x_end = MIN(ker_dim_x, in_tensor_dim_x - base_idx_x);

            for (int i_ch_in = 0; i_ch_in < in_tensor_ch; i_ch_in++)
            {
                int sum = 0;
                int count = 0;

                for (int k_y = ker_y_start; k_y < ker_dim_y_end; k_y++)
                {
                    for (int k_x = ker_x_start; k_x < ker_dim_x_end; k_x++)
                    {
                        sum += in_tensor[i_ch_in + in_tensor_ch * (k_x + base_idx_x + (k_y + base_idx_y) * in_tensor_dim_x)];
                        count++;
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

int32_t tpt_nn_avepool_HWC_s16_any_act_get_buffer_size(const int out_tensor_dim_x,
    const int in_tensor_ch)
{
    (void)out_tensor_dim_x;
    (void)in_tensor_ch;
    return 0;
}
