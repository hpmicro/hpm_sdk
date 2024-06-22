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

void tpt_nn_avepool_HWC_s8_any(q7_t * in_tensor,
                                const uint16_t in_tensor_dim_x,
                                const uint16_t in_tensor_dim_y,
                                const uint16_t in_tensor_ch,
                                const uint16_t ker_dim_x,
                                const uint16_t ker_dim_y,
                                const uint16_t pad_x,
                                const uint16_t pad_y,
                                const uint16_t stride_x,
                                const uint16_t stride_y,
                                const uint16_t out_tensor_dim_x,
                                const uint16_t out_tensor_dim_y,
                                q7_t * in_tmp_buf,
                                q7_t * out_tensor,
                                const uint16_t out_lshift)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;

    for (i_ch_in = 0; i_ch_in < in_tensor_ch; i_ch_in++)
    {
        for (i_y = 0; i_y < out_tensor_dim_y; i_y++)
        {
            for (i_x = 0; i_x < out_tensor_dim_x; i_x++)
            {
                int sum = 0;
                int count = 0;
                for (k_y = i_y * stride_y - pad_y; k_y < i_y*stride_y - pad_y + ker_dim_y; k_y++)
                {
                    for (k_x = i_x * stride_x - pad_x; k_x < i_x*stride_x - pad_x + ker_dim_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < in_tensor_dim_y && k_x < in_tensor_dim_x)
                        {
                            sum += in_tensor[i_ch_in + in_tensor_ch*(k_x+k_y*in_tensor_dim_x)];
                            count++;
                        }
                    }
                }
                out_tensor[i_ch_in+in_tensor_ch*(i_x+i_y*out_tensor_dim_x)] = (sum << out_lshift) / count;
            }
        }
    }
}
