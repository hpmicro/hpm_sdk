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

static void conv_dw_HWC_s8_any_mult_4(const int8_t *in_tensor,
                                     const int32_t in_tensor_dim_x,
                                     const int32_t in_tensor_dim_y,
                                     const int32_t in_tensor_ch,
                                     const int8_t *ker_weight,
                                     const int32_t out_tensor_ch,
                                     const int32_t ch_mult,
                                     const int32_t ker_dim_x,
                                     const int32_t ker_dim_y,
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
                                     const int32_t act_max)
{
    for(int32_t in_h = -pad_y, out_h = 0, out_idx = 0; out_h < out_tensor_dim_y; in_h += stride_y, ++out_h)
    {
        for(int32_t in_w = -pad_x, out_w = 0, ker_h_start = MAX(0, -in_h); out_w < out_tensor_dim_x; in_w += stride_x, ++out_w)
        {
            for(int32_t in_ch = 0, out_ch = 0, ker_w_start = MAX(0, -in_w); out_ch < out_tensor_ch; ++in_ch, out_ch += ch_mult)
            {
                for(int mult_tile = 0; mult_tile < ch_mult; mult_tile += 4)
                {
                    int32_t out_buff0 = 0;
                    int32_t out_buff1 = 0;
                    int32_t out_buff2 = 0;
                    int32_t out_buff3 = 0;
                    if (bias)
                    {
                        out_buff0 = bias[out_ch + 0 + mult_tile];
                        out_buff1 = bias[out_ch + 1 + mult_tile];
                        out_buff2 = bias[out_ch + 2 + mult_tile];
                        out_buff3 = bias[out_ch + 3 + mult_tile];
                    }

                    for(int32_t ker_h = ker_h_start; ker_h < MIN(ker_dim_y, in_tensor_dim_y - in_h); ++ker_h)
                    {
                        int32_t ker_idx = ker_h * (out_tensor_ch * ker_dim_x) + ker_w_start * out_tensor_ch + out_ch;
                        int32_t in_idx  = (in_h + ker_h) * (in_tensor_ch * in_tensor_dim_x) + in_w * in_tensor_ch + in_ch;

                        for(int32_t ker_w = ker_w_start; ker_w < MIN(ker_dim_x, in_tensor_dim_x - in_w); ++ker_w, ker_idx += out_tensor_ch)
                        {
                            int32_t in_val = in_tensor[in_idx + ker_w * in_tensor_ch] + in_offset;

                            out_buff0 += in_val * ker_weight[ker_idx + 0 + mult_tile];
                            out_buff1 += in_val * ker_weight[ker_idx + 1 + mult_tile];
                            out_buff2 += in_val * ker_weight[ker_idx + 2 + mult_tile];
                            out_buff3 += in_val * ker_weight[ker_idx + 3 + mult_tile];
                        }
                    }

                    out_buff0 = tpt_nn_requantize(out_buff0, out_scale[out_ch + 0 + mult_tile], out_shift[out_ch + 0 + mult_tile]);
                    out_buff1 = tpt_nn_requantize(out_buff1, out_scale[out_ch + 1 + mult_tile], out_shift[out_ch + 1 + mult_tile]);
                    out_buff2 = tpt_nn_requantize(out_buff2, out_scale[out_ch + 2 + mult_tile], out_shift[out_ch + 2 + mult_tile]);
                    out_buff3 = tpt_nn_requantize(out_buff3, out_scale[out_ch + 3 + mult_tile], out_shift[out_ch + 3 + mult_tile]);

                    out_buff0 += out_offset;
                    out_buff1 += out_offset;
                    out_buff2 += out_offset;
                    out_buff3 += out_offset;

                    out_buff0 = MIN(MAX(out_buff0, act_min), act_max);
                    out_buff1 = MIN(MAX(out_buff1, act_min), act_max);
                    out_buff2 = MIN(MAX(out_buff2, act_min), act_max);
                    out_buff3 = MIN(MAX(out_buff3, act_min), act_max);

                    out_tensor[out_idx++] = (int8_t)out_buff0;
                    out_tensor[out_idx++] = (int8_t)out_buff1;
                    out_tensor[out_idx++] = (int8_t)out_buff2;
                    out_tensor[out_idx++] = (int8_t)out_buff3;
                }
            }
        }
    }
}

static void conv_dw_HWC_s8_any_generic(const q7_t *in_tensor,
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
                                      const uint16_t dilation_y)
{
    (void)out_tensor_ch;

    int i_out = 0;
    for (int i_out_y = 0; i_out_y < out_tensor_dim_y; i_out_y++)
    {
        const int16_t base_idx_y = (i_out_y * stride_y) - pad_y;
        for (int i_out_x = 0; i_out_x < out_tensor_dim_x; i_out_x++)
        {
            const int16_t base_idx_x = (i_out_x * stride_x) - pad_x;
            for (int i_input_ch = 0; i_input_ch < in_tensor_ch; i_input_ch++)
            {
                int i_ch_mult = 0;
                for (; i_ch_mult < ch_mult; i_ch_mult++)
                {
                    const int idx_out_ch = i_ch_mult + i_input_ch * ch_mult;
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

                    int32_t acc_0 = 0;
                    if (bias)
                    {
                        acc_0 = bias[idx_out_ch];
                    }

                    for (int i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        const int32_t idx_y = base_idx_y + i_ker_y;
                        for (int i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            const int32_t idx_x = base_idx_x + i_ker_x;
                            int32_t idx_0 = (idx_y * in_tensor_dim_x + idx_x) * in_tensor_ch + i_input_ch;
                            int32_t ker_idx_0 = (i_ker_y * ker_dim_x + i_ker_x) * (in_tensor_ch * ch_mult) + idx_out_ch;

                            acc_0 += (in_tensor[idx_0] + in_offset) * ker_weight[ker_idx_0];
                        }
                    }

                    /* Requantize and clamp out_tensor to provided range */
                    acc_0 = tpt_nn_requantize(acc_0, out_scale[idx_out_ch], out_shift[idx_out_ch]);
                    acc_0 += out_offset;
                    acc_0 = MAX(acc_0, act_min);
                    acc_0 = MIN(acc_0, act_max);

                    out_tensor[i_out++] = acc_0;
                }
            }
        }
    }
}

  /*
   *  Basic s8 depthwise convolution function.
   *
   *  Refer header file for details.
   *  Optimization using DSP extension is not available for the generic case where channel multiplier is > 1.
   *
   */
int32_t tpt_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(const q7_t *in_tensor,
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
                                    q15_t *tmp_buf)
{
    (void)tmp_buf;

    if(ch_mult % 4 == 0 &&
       dilation_x == 1 &&
       dilation_y == 1)
    {
        conv_dw_HWC_s8_any_mult_4(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ch_mult, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias,
                                 out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max);
    }
    else
    {
        conv_dw_HWC_s8_any_generic(in_tensor, in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch, ker_weight, out_tensor_ch, ch_mult, ker_dim_x, ker_dim_y, pad_x, pad_y, stride_x, stride_y, bias,
                                  out_tensor, out_shift, out_scale, out_tensor_dim_x, out_tensor_dim_y, out_offset, in_offset, act_min, act_max, dilation_x, dilation_y);
    }

    /* Return to application */
    return 0;
}
