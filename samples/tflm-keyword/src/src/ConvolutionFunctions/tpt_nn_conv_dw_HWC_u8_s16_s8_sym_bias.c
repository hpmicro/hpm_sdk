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

int32_t tpt_nn_conv_dw_HWC_u8_s16_s8_sym_bias(const u8_t * in_tensor,
        const uint16_t in_tensor_dim,
        const uint16_t in_tensor_ch,
        const q7_t * ker_weight,
        const uint16_t out_tensor_ch,
        const uint16_t ker_dim,
        const uint16_t pad,
        const uint16_t stride,
        const q31_t * bias,
        const uint16_t pre_rshift,
        const uint16_t out_scale,
        const uint16_t post_rshift,
        q15_t * out_tensor,
        const uint16_t out_tensor_dim,
        q15_t * in_tmp_buf)
{
    /* do some checking here, basically in_tensor_ch == out_tensor_ch */
    if (in_tensor_ch != out_tensor_ch)
    {
        return -1;
    }

    int       i_out_y, i_out_x, i_ch_out, i_ker_x, i_ker_y;
    int       conv_out;

    for (i_out_y = 0; i_out_y < out_tensor_dim; i_out_y++)
    {
        for (i_out_x = 0; i_out_x < out_tensor_dim; i_out_x++)
        {
            for (i_ch_out = 0; i_ch_out < out_tensor_ch; i_ch_out++)
            {
                // for each output
                conv_out = bias[i_ch_out];
                for (i_ker_y = 0; i_ker_y < ker_dim; i_ker_y++)
                {
                    for (i_ker_x = 0; i_ker_x < ker_dim; i_ker_x++)
                    {
                        int       in_row = stride * i_out_y + i_ker_y - pad;
                        int       in_col = stride * i_out_x + i_ker_x - pad;
                        if (in_row >= 0 && in_col >= 0 && in_row < in_tensor_dim && in_col < in_tensor_dim)
                        {
                            conv_out +=
                                in_tensor[(in_row * in_tensor_dim + in_col) * in_tensor_ch + i_ch_out]
                                * ker_weight[(i_ker_y * ker_dim + i_ker_x) * out_tensor_ch + i_ch_out];
                        }
                    }
                }
                conv_out = (conv_out >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
                out_tensor[(i_out_y * out_tensor_dim + i_out_x) * out_tensor_ch + i_ch_out] = NDS_ISA_SATS((conv_out >> post_rshift), 16);
            }
        }
    }
    return 0;
}
