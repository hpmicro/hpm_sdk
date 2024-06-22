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
int32_t tpt_nn_conv_dw_HWC_3x3_s8_s8_s8_asym_bias_any(const int8_t *in_tensor,
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
                                     const int32_t out_offset,  //value is in the range of [-127, 128]
                                     const int32_t in_offset,   //value is in the range of [-128, 127]
                                     const int32_t act_min,
                                     const int32_t act_max,
                                     const int32_t dilation_x,
                                     const int32_t dilation_y,
                                     int16_t *tmp_buf)
{
    // Check input constraints
    if((in_tensor_ch != out_tensor_ch) || (pad_x > 1))
    {
        return -1;
    }


    (void)dilation_x;
    (void)dilation_y;
    (void)tmp_buf;

    for(int32_t in_y = -pad_y, out_y = 0; out_y < out_tensor_dim_y; in_y += stride_y, out_y++)
    {
        int32_t ker_y_start = MAX(0, -in_y);
        for(int32_t in_x = -pad_x, out_x = 0; out_x < out_tensor_dim_x; in_x += stride_x, out_x++)
        {
            int32_t cur_ch = 0;
            int32_t ker_x_start = MAX(0, -in_x);

            for(; cur_ch <= (in_tensor_ch - 4); cur_ch += 4)
            {
                int32_t out_buff0 = bias[cur_ch + 0];
                int32_t out_buff1 = bias[cur_ch + 1];
                int32_t out_buff2 = bias[cur_ch + 2];
                int32_t out_buff3 = bias[cur_ch + 3];

                const int8_t *input_ptr  = in_tensor + (in_y + ker_y_start) * (in_tensor_ch * in_tensor_dim_x) + in_x * in_tensor_ch + cur_ch;
                const int8_t *kernel_ptr = ker_weight + ker_y_start * (in_tensor_ch * 3) + cur_ch;

                for(int32_t ker_y = ker_y_start; ker_y < MIN(3, in_tensor_dim_y - in_y); ++ker_y)
                {
                    if(ker_x_start == 0)
                    {
                        out_buff0 += (*input_ptr     + in_offset) * *kernel_ptr;
                        out_buff1 += (*(input_ptr+1) + in_offset) * *(kernel_ptr+1);
                        out_buff2 += (*(input_ptr+2) + in_offset) * *(kernel_ptr+2);
                        out_buff3 += (*(input_ptr+3) + in_offset) * *(kernel_ptr+3);
                    }

                    int8_t *input_ptr2 = (int8_t *)(input_ptr + in_tensor_ch);
                    int8_t *kernel_ptr2 = (int8_t *)(kernel_ptr + in_tensor_ch);

                    out_buff0 += (*input_ptr2     + in_offset) * *kernel_ptr2;
                    out_buff1 += (*(input_ptr2+1) + in_offset) * *(kernel_ptr2+1);
                    out_buff2 += (*(input_ptr2+2) + in_offset) * *(kernel_ptr2+2);
                    out_buff3 += (*(input_ptr2+3) + in_offset) * *(kernel_ptr2+3);

                    if((in_tensor_dim_x - in_x) >= 3)
                    {
                        input_ptr2 = (int8_t *)(input_ptr + (in_tensor_ch * 2));
                        kernel_ptr2 = (int8_t *)(kernel_ptr + (in_tensor_ch * 2));

                        out_buff0 += (*input_ptr2     + in_offset) * *kernel_ptr2;
                        out_buff1 += (*(input_ptr2+1) + in_offset) * *(kernel_ptr2+1);
                        out_buff2 += (*(input_ptr2+2) + in_offset) * *(kernel_ptr2+2);
                        out_buff3 += (*(input_ptr2+3) + in_offset) * *(kernel_ptr2+3);
                    }
                    input_ptr  += (in_tensor_ch * in_tensor_dim_x);
                    kernel_ptr += (in_tensor_ch * 3);
                }

                out_buff0 = tpt_nn_requantize(out_buff0, out_scale[cur_ch + 0], out_shift[cur_ch + 0]);
                out_buff1 = tpt_nn_requantize(out_buff1, out_scale[cur_ch + 1], out_shift[cur_ch + 1]);
                out_buff2 = tpt_nn_requantize(out_buff2, out_scale[cur_ch + 2], out_shift[cur_ch + 2]);
                out_buff3 = tpt_nn_requantize(out_buff3, out_scale[cur_ch + 3], out_shift[cur_ch + 3]);

                out_buff0 += out_offset;
                out_buff1 += out_offset;
                out_buff2 += out_offset;
                out_buff3 += out_offset;

                out_buff0 = MIN(MAX(out_buff0, act_min), act_max);
                out_buff1 = MIN(MAX(out_buff1, act_min), act_max);
                out_buff2 = MIN(MAX(out_buff2, act_min), act_max);
                out_buff3 = MIN(MAX(out_buff3, act_min), act_max);

                *out_tensor++ = out_buff0;
                *out_tensor++ = out_buff1;
                *out_tensor++ = out_buff2;
                *out_tensor++ = out_buff3;
            }

            // Leftover
            for(; cur_ch < in_tensor_ch; ++cur_ch)
            {
                int32_t out_buff = bias[cur_ch];

                const int8_t *input_ptr  = in_tensor + (in_y + ker_y_start) * (in_tensor_ch * in_tensor_dim_x) + in_x * in_tensor_ch + cur_ch;
                const int8_t *kernel_ptr = ker_weight + ker_y_start * (in_tensor_ch * 3) + cur_ch;

                for(int32_t ker_y = ker_y_start; ker_y < MIN(3, in_tensor_dim_y - in_y); ++ker_y)
                {
                    if(ker_x_start == 0)
                    {
                        out_buff += (*(input_ptr) + in_offset) * *(kernel_ptr);
                    }

                    out_buff += (*(input_ptr + in_tensor_ch) + in_offset) * *(kernel_ptr + in_tensor_ch);

                    if((in_tensor_dim_x - in_x) >= 3)
                    {
                        out_buff += (*(input_ptr + (in_tensor_ch << 1)) + in_offset) * *(kernel_ptr + (in_tensor_ch << 1));
                    }

                    input_ptr  += (in_tensor_ch * in_tensor_dim_x);
                    kernel_ptr += (in_tensor_ch * 3);
                }

                out_buff = tpt_nn_requantize(out_buff, out_scale[cur_ch], out_shift[cur_ch]);
                out_buff += out_offset;
                out_buff = MIN(MAX(out_buff, act_min), act_max);
                *out_tensor++ = out_buff;
            }
        }
    }

    return 0;
}
