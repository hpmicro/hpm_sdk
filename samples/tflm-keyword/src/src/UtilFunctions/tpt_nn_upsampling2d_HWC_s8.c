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
#include "tpt_nn_util.h"  //for tpt_nn_upsample_method enum type
#include "tpt_nn_support.h"

//// Util Functions

int32_t tpt_nn_upsampling2d_HWC_s8(const int8_t* in_tensor,
                                const uint32_t in_tensor_dim_x,
                                const uint32_t in_tensor_dim_y,
                                const uint32_t in_tensor_ch,
                                const uint32_t scale_factor_x,
                                const uint32_t scale_factor_y,
                                const tpt_nn_upsample_method upsample_method,
                                int8_t* out_tensor)
{
    if (upsample_method != NN_UPSAMPLE_NEAREST) {
        //Now we only support nearest upsampling algorithm
        return -1;
    }

    int i, j, k;
    for (j = 0; j < in_tensor_dim_y * scale_factor_y; j++) {
        for (i = 0; i < in_tensor_dim_x * scale_factor_x; i++) {
            for (k = 0; k < in_tensor_ch; k++) {
                int in_index = (j / scale_factor_y) * in_tensor_dim_x *in_tensor_ch + (i / scale_factor_x) * in_tensor_ch + k;
                int out_index = j * in_tensor_dim_x * scale_factor_x *in_tensor_ch + i * in_tensor_ch + k;
                out_tensor[out_index] = in_tensor[in_index];
            }
        }
    }

    return 0;
}