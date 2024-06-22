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

static void compare_and_replace_if_larger(int16_t *dst, const int16_t *src, int32_t length)
{
    while (length-- > 0)
    {
        if (*src > *dst)
        {
            *dst = *src;
        }
        dst++;
        src++;
    }
}

static void clamp_output(int16_t *source, int32_t length, const int16_t act_min, const int16_t act_max)
{
    while (length-- > 0)
    {
        int16_t in_val = *source;
        in_val = MAX(in_val, act_min);
        in_val = MIN(in_val, act_max);
        *source = in_val;
    }
}

int32_t tpt_nn_maxpool_HWC_s16_any_act(const int32_t in_tensor_dim_y,
    const int32_t in_tensor_dim_x,
    const int32_t out_tensor_dim_y,
    const int32_t out_tensor_dim_x,
    const int32_t stride_y,
    const int32_t stride_x,
    const int32_t ker_dim_y,
    const int32_t ker_dim_x,
    const int32_t pad_y,
    const int32_t pad_x,
    const int32_t act_min,
    const int32_t act_max,
    const int32_t in_tensor_ch,
    int16_t *in_tensor,
    int16_t *tmp_buffer,
    int16_t *out_tensor)
{
    for (int i_y = 0, base_idx_y = -pad_y; i_y < out_tensor_dim_y; base_idx_y += stride_y, i_y++)
    {
        for (int i_x = 0, base_idx_x = -pad_x; i_x < out_tensor_dim_x; base_idx_x += stride_x, i_x++)
        {
            const int32_t ker_y_start = MAX(0, -base_idx_y);
            const int32_t ker_x_start = MAX(0, -base_idx_x);
            const int32_t kernel_y_end = MIN(ker_dim_y, in_tensor_dim_y - base_idx_y);
            const int32_t kernel_x_end = MIN(ker_dim_x, in_tensor_dim_x - base_idx_x);

            int count = 0;
            for (int k_y = ker_y_start; k_y < kernel_y_end; k_y++)
            {
                for (int k_x = ker_x_start; k_x < kernel_x_end; k_x++)
                {
                    const int16_t *start = in_tensor + in_tensor_ch * (k_x + base_idx_x + (k_y + base_idx_y) * in_tensor_dim_x);

                    if (count == 0)
                    {
                        memcpy(out_tensor, start, in_tensor_ch * sizeof(int16_t));
                        count++;
                    }
                    else
                    {
                        compare_and_replace_if_larger(out_tensor, start, in_tensor_ch);
                    }
                }
            }

            out_tensor += in_tensor_ch;
        }
    }

    clamp_output(out_tensor, out_tensor_dim_x * out_tensor_dim_y * in_tensor_ch, act_min, act_max);

    return 0;
}
