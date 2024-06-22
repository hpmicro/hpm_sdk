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

int32_t tpt_nn_conv_dw_HWC_u8_u8_s8_sym_any(const u8_t * in_tensor,
                                       const uint16_t in_tensor_dim_x,
                                       const uint16_t in_tensor_dim_y,
                                       const uint16_t in_tensor_ch,
                                       const q7_t * ker_weight,
                                       const uint16_t out_tensor_ch,
                                       const uint16_t ker_dim_x,
                                       const uint16_t ker_dim_y,
                                       const uint16_t pad_x,
                                       const uint16_t pad_y,
                                       const uint16_t stride_x,
                                       const uint16_t stride_y,
                                       const uint16_t pre_rshift,
                                       const uint16_t out_scale,
                                       const uint16_t post_rshift,
                                       u8_t * out_tensor,
                                       const uint16_t out_tensor_dim_x,
                                       const uint16_t out_tensor_dim_y,
                                       q15_t * in_tmp_buf)
{
    /* do some checking here, basically in_tensor_ch == out_tensor_ch */
    if (in_tensor_ch != out_tensor_ch)
    {
        return -1;
    }

    int       i_out_y, i_out_x, i_ch_out;
    int       i_ker_y, i_ker_x;

    for (i_out_y = 0; i_out_y < out_tensor_dim_y; i_out_y++)
    {
        for (i_out_x = 0; i_out_x < out_tensor_dim_x; i_out_x++)
        {
            for (i_ch_out = 0; i_ch_out < out_tensor_ch; i_ch_out++)
            {
                // for each output
                int       conv_out = 0;
                for (i_ker_y = 0; i_ker_y < ker_dim_y; i_ker_y++)
                {
                    for (i_ker_x = 0; i_ker_x < ker_dim_x; i_ker_x++)
                    {
                        int       in_row = stride_y * i_out_y + i_ker_y - pad_y;
                        int       in_col = stride_x * i_out_x + i_ker_x - pad_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < in_tensor_dim_y && in_col < in_tensor_dim_x)
                        {
                            conv_out += in_tensor[(in_row * in_tensor_dim_x + in_col) * in_tensor_ch + i_ch_out] *
                                        ker_weight[(i_ker_y * ker_dim_x + i_ker_x) * out_tensor_ch + i_ch_out];
                        }
                    }
                }
                if (conv_out < 0)
                    out_tensor[(i_out_y * out_tensor_dim_x + i_out_x) * out_tensor_ch + i_ch_out] = 0;
                else
                {
                    conv_out = (conv_out >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
                    out_tensor[(i_out_y * out_tensor_dim_x + i_out_x) * out_tensor_ch + i_ch_out] = NDS_ISA_SAT((conv_out >> post_rshift), 8);
                }
            }
        }
    }
    return 0;
}
