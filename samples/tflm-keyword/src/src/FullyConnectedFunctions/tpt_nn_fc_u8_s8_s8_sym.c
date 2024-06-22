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

//// FullyConnected Functions
int32_t tpt_nn_fc_u8_s8_s8_sym(const u8_t * in_vec,
                                const q7_t * wt_mat,
                                const uint16_t size,
                                const uint16_t wt_row_num,
                                const uint16_t pre_rshift,
                                const uint16_t out_scale,
                                const uint16_t post_rshift,
                                q7_t * out_vec,
                                q15_t * in_tmp_buf)
{
    int i, j;
    for (i = 0; i < wt_row_num; i++)
    {
        int       ip_out = 0;
        for (j = 0; j < size; j++)
        {
            ip_out += in_vec[j] * wt_mat[i * size + j];
        }

        ip_out = (ip_out >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
        out_vec[i] = NDS_ISA_SATS((ip_out >> post_rshift), 8);
    }

    return 0;
}
