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
#include "tpt_nn_convolution.h"

//// Convolution Functions

int32_t tpt_nn_conv_HWC_wrapper_s16_s16_s8_asym(const q15_t *in_tensor,
    const int32_t in_tensor_dim_x,
    const int32_t in_tensor_dim_y,
    const int32_t in_tensor_ch,
    const int32_t in_tensor_batch,
    const q7_t *ker_weight,
    const int32_t ker_dim_x,
    const int32_t ker_dim_y,
    const int32_t pad_x,
    const int32_t pad_y,
    const int32_t stride_x,
    const int32_t stride_y,
    const int64_t *bias,
    q15_t *out_tensor,
    const int32_t *out_shift,
    const int32_t *out_scale,
    const int32_t out_offset,
    const int32_t in_offset,
    const int32_t act_min,
    const int32_t act_max,
    const int32_t out_tensor_ch,
    const int32_t out_tensor_dim_x,
    const int32_t out_tensor_dim_y,
    const int32_t dilation_x,
    const int32_t dilation_y,
    q15_t *in_tmp_buf)
{
    if ((dilation_x == 1)
        && (dilation_y == 1)
        && ((in_tensor_ch * ker_dim_x * ker_dim_y) <= 512))
    {
        return  tpt_nn_conv_HWC_s16_s16_s8_asym_bias_fast_any( in_tensor,
                    in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch,
                    in_tensor_batch, ker_weight, ker_dim_x, ker_dim_y,
                    pad_x, pad_y, stride_x, stride_y, bias, out_tensor,
                    out_shift, out_scale, out_offset, in_offset, act_min,
                    act_max, out_tensor_ch, out_tensor_dim_x,
                    out_tensor_dim_y, in_tmp_buf);
    }
    else
    {
        return tpt_nn_conv_HWC_s16_s16_s8_asym_bias_any(in_tensor,
                    in_tensor_dim_x, in_tensor_dim_y, in_tensor_ch,
                    in_tensor_batch, ker_weight, ker_dim_x, ker_dim_y,
                    pad_x, pad_y, stride_x, stride_y, bias, out_tensor,
                    out_shift, out_scale, out_offset, in_offset, act_min,
                    act_max, out_tensor_ch, out_tensor_dim_x, out_tensor_dim_y,
                    dilation_x, dilation_y, in_tmp_buf);
    }
}

int32_t tpt_nn_conv_HWC_wrapper_s16_s16_s8_asym_get_buffer_size(const int32_t in_tensor_ch,
    const int32_t ker_dim_x,
    const int32_t ker_dim_y,
    const int32_t dilation_x,
    const int32_t dilation_y)
{
    if ((dilation_x == 1)
        && (dilation_y == 1)
        && ((in_tensor_ch * ker_dim_x * ker_dim_y) <= 512))
    {
        return tpt_nn_conv_HWC_s16_s16_s8_asym_bias_any_get_buffer_size(in_tensor_ch,
                ker_dim_x, ker_dim_y);
    }
    else
    {
        return tpt_nn_conv_HWC_s16_s16_s8_asym_bias_fast_any_get_buffer_size(in_tensor_ch,
                ker_dim_x, ker_dim_y);
    }
}
