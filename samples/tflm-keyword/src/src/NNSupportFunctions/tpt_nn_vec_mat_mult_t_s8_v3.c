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

#include "internal_nn_math.h"

int32_t tpt_nn_vec_mat_mult_t_s8_v3(const q7_t *lhs,
                                const q7_t *rhs,
                                const q31_t *bias,
                                q7_t *dst,
                                const int32_t lhs_offset,   //value is in the range of [-127, 128]
                                const int32_t rhs_offset,   //dummy
                                const int32_t dst_offset,   //value is in the range of [-128, 127]
                                const int32_t dst_multiplier,
                                const int32_t dst_shift,
                                const int32_t rhs_cols,
                                const int32_t rhs_rows,
                                const int32_t activation_min,
                                const int32_t activation_max,
                                const int32_t out_addr_offset)
{
    (void)rhs_offset;
    (void)bias;
    for (int32_t rhs_rows_idx = 0; rhs_rows_idx <= (rhs_rows - 2); rhs_rows_idx += 2)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];
        q31_t res00 = 0;
        q31_t res01 = 0;
        // if(bias != NULL)
        // {
        //     res00 = *bias++;
        //     res01 = *bias++;
        // }

        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = rhs_ptr[0];
            q31_t rhs_value1 = rhs_ptr[rhs_cols];
            q31_t lhs_value  = lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;
            res01 += lhs_value * rhs_value1;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // re-quantize the results
        res00 = tpt_nn_requantize(res00, dst_multiplier, dst_shift);
        res01 = tpt_nn_requantize(res01, dst_multiplier, dst_shift);

        // add offset
        res00 += dst_offset;
        res01 += dst_offset;

        // clip the results
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);
        res01 = MAX(res01, activation_min);
        res01 = MIN(res01, activation_max);

        *dst = res00;
        *(dst + out_addr_offset) = res01;
        dst += 2 * out_addr_offset;

        rhs += 2 * rhs_cols;
    }

    if (rhs_rows & 1)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];
        q31_t res00 = 0;
        // if(bias != NULL)
        // {
        //     res00 = *bias++;
        // }

        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = rhs_ptr[0];
            q31_t lhs_value  = lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // re-quantize the results
        res00 = tpt_nn_requantize(res00, dst_multiplier, dst_shift);

        // add offset
        res00 += dst_offset;

        // clip the results
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);

        *dst = res00;
        dst += out_addr_offset;
    }

    return 0;
}
