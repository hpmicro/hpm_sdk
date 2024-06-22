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

//// FullyConnected Functions
extern void tpt_nn_dup_s8_s16_reordered(const q7_t * src, q15_t * dst, uint32_t size);

int32_t tpt_nn_fc_s8_s8_s8_sft_bias(const q7_t * in_vec,
                     const q7_t * wt_mat,
                     const uint16_t size,
                     const uint16_t wt_row_num,
                     const uint16_t bias_lshift,
                     const uint16_t out_rshift,
                     const q7_t * bias,
                     q7_t * out_vec,
                     q15_t * in_tmp_buf)
{
    int i, j;
    for (i = 0; i < wt_row_num; i++)
    {
        int       ip_out = ((q31_t)(bias[i]) << bias_lshift) + NN_ROUND(out_rshift);
        for (j = 0; j < size; j++)
        {
            ip_out += in_vec[j] * wt_mat[i * size + j];
        }
        out_vec[i] = (q7_t) NDS_ISA_SATS((ip_out >> out_rshift), 8);
    }

    return 0;
}
