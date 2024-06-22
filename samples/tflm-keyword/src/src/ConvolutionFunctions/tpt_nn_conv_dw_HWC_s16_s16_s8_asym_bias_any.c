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

//// Convolution Functions

int32_t tpt_nn_conv_dw_HWC_s16_s16_s8_asym_bias_any(const int16_t *in_tensor,
    const uint16_t in_tensor_batch,
    const uint16_t in_tensor_dim_x,
    const uint16_t in_tensor_dim_y,
    const uint16_t in_tensor_ch,
    const int8_t *ker_weight,
    const uint16_t ch_mult,
    const uint16_t ker_dim_x,
    const uint16_t ker_dim_y,
    const uint16_t pad_x,
    const uint16_t pad_y,
    const uint16_t stride_x,
    const uint16_t stride_y,
    const int32_t *bias,
    int16_t *out_tensor,
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
    int16_t *tmp_buf)
{
    (void)tmp_buf;
    (void)in_offset;
    (void)out_offset;

    for (int cur_batch = 0; cur_batch < in_tensor_batch; cur_batch++)
    {
        for (int cur_out_y = 0; cur_out_y < out_tensor_dim_y; cur_out_y++)
        {
            const int16_t base_idx_y = (cur_out_y * stride_y) - pad_y;
            for (int cur_out_x = 0; cur_out_x < out_tensor_dim_x; cur_out_x++)
            {
                const int16_t base_idx_x = (cur_out_x * stride_x) - pad_x;
                for (int cur_input_ch = 0; cur_input_ch < in_tensor_ch; cur_input_ch++)
                {
                    for (int cur_ch_mult = 0; cur_ch_mult < ch_mult; cur_ch_mult++)
                    {
                        const int idx_out_ch = cur_ch_mult + cur_input_ch * ch_mult;

                        const q31_t reduced_sacle = REDUCE_MULTIPLIER(out_scale[idx_out_ch]);
                        int64_t conv_out = 0;

                        int ker_y_start;
                        int ker_x_start;
                        int ker_y_end;
                        int ker_x_end;

                        if (dilation_x > 1)
                        {
                            const int32_t start_x_max = (-base_idx_x + dilation_x - 1) / dilation_x;
                            ker_x_start = MAX(0, start_x_max);
                            const int32_t end_min_x = (in_tensor_dim_x - base_idx_x + dilation_x - 1) / dilation_x;
                            ker_x_end = MIN(ker_dim_x, end_min_x);
                        }
                        else
                        {
                            ker_x_start = MAX(0, -base_idx_x);
                            ker_x_end = MIN(ker_dim_x, in_tensor_dim_x - base_idx_x);
                        }

                        if (dilation_y > 1)
                        {
                            const int32_t start_y_max = (-base_idx_y + dilation_y - 1) / dilation_y;
                            ker_y_start = MAX(0, start_y_max);
                            const int32_t end_min_y = (in_tensor_dim_y - base_idx_y + dilation_y - 1) / dilation_y;
                            ker_y_end = MIN(ker_dim_y, end_min_y);
                        }
                        else
                        {
                            ker_y_start = MAX(0, -base_idx_y);
                            ker_y_end = MIN(ker_dim_y, in_tensor_dim_y - base_idx_y);
                        }

                        if (bias)
                        {
                            conv_out = bias[idx_out_ch];
                        }

                        for (int i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                        {
                            const int32_t idx_y = base_idx_y + dilation_y * i_ker_y;
                            for (int i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                            {
                                const int32_t idx_x = base_idx_x + dilation_x * i_ker_x;
                                int32_t idx_0 = (idx_y * in_tensor_dim_x + idx_x) * in_tensor_ch + cur_input_ch;
                                int32_t ker_idx_0 = (i_ker_y * ker_dim_x + i_ker_x) * (in_tensor_ch * ch_mult) + idx_out_ch;

                                conv_out += in_tensor[idx_0] * ker_weight[ker_idx_0];
                            }
                        }

                        int32_t result = tpt_nn_requantize_s64(conv_out, reduced_sacle, out_shift[idx_out_ch]);
                        result = MAX(result, act_min);
                        result = MIN(result, act_max);
                        *out_tensor++ = (int16_t)result;
                    }
                }
            }
        }

        in_tensor += (in_tensor_dim_x * in_tensor_dim_y * in_tensor_ch);
    }
    return 0;
}
