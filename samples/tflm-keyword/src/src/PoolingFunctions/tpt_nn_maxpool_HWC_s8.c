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

void tpt_nn_maxpool_HWC_s8(q7_t * in_tensor,
                           const uint16_t in_tensor_dim,
                           const uint16_t in_tensor_ch,
                           const uint16_t ker_dim,
                           const uint16_t pad,
                           const uint16_t stride,
                           const uint16_t out_tensor_dim,
                           q7_t * in_tmp_buf,
                           q7_t * out_tensor)
{
    int16_t   i_ch_in, i_x, i_y;
    int16_t   k_x, k_y;

    for (i_ch_in = 0; i_ch_in < in_tensor_ch; i_ch_in++)
    {
        for (i_y = 0; i_y < out_tensor_dim; i_y++)
        {
            for (i_x = 0; i_x < out_tensor_dim; i_x++)
            {
                int       max = -129;
                for (k_y = i_y * stride - pad; k_y < i_y * stride - pad + ker_dim; k_y++)
                {
                    for (k_x = i_x * stride - pad; k_x < i_x * stride - pad + ker_dim; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < in_tensor_dim && k_x < in_tensor_dim)
                        {
                            if (in_tensor[i_ch_in + in_tensor_ch * (k_x + k_y * in_tensor_dim)] > max)
                            {
                                max = in_tensor[i_ch_in + in_tensor_ch * (k_x + k_y * in_tensor_dim)];
                            }
                        }
                    }
                }
                out_tensor[i_ch_in + in_tensor_ch * (i_x + i_y * out_tensor_dim)] = max;
            }
        }
    }

}
