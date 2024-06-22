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

int32_t tpt_nn_mat_mult_nt_t_s8(const q7_t *lhs,
                                   const q7_t *rhs,
                                   const q31_t *bias,
                                   q7_t *dst,
                                   const int32_t *dst_multipliers,
                                   const int32_t *dst_shifts,
                                   const int32_t lhs_rows,
                                   const int32_t rhs_rows,
                                   const int32_t rhs_cols,
                                   const int32_t lhs_offset,    //value is in the range of [-127, 128]
                                   const int32_t dst_offset,    //value is in the range of [-128, 127]
                                   const int32_t activation_min,
                                   const int32_t activation_max)
{
    for (int32_t rhs_rows_idx = 0; (rhs_rows_idx + 2) <= rhs_rows; rhs_rows_idx += 2)
    {
        const q7_t *lhs_ptr = &lhs[0];
        q7_t       *dst_ptr = &dst[0];

        q31_t lhs_offset_contribution0 = 0;
        q31_t lhs_offset_contribution1 = 0;

        for (int32_t x = 0; x < rhs_cols; ++x)
        {
            lhs_offset_contribution0 += rhs[x];
            lhs_offset_contribution1 += rhs[x + rhs_cols];
        }

        lhs_offset_contribution0 *= lhs_offset;
        lhs_offset_contribution1 *= lhs_offset;

        lhs_offset_contribution0 += bias[rhs_rows_idx];
        lhs_offset_contribution1 += bias[rhs_rows_idx + 1];

        int32_t lhs_rows_idx = lhs_rows >> 1;

        while (lhs_rows_idx)
        {
            const q7_t *rhs_ptr = &rhs[0];

            q31_t res00 = lhs_offset_contribution0;
            q31_t res01 = lhs_offset_contribution1;
            q31_t res10 = lhs_offset_contribution0;
            q31_t res11 = lhs_offset_contribution1;

            for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
            {
                q7_t rhs_value0 = rhs_ptr[0];
                q7_t rhs_value1 = rhs_ptr[rhs_cols];
                q7_t lhs_value  = lhs_ptr[0];

                res00 += lhs_value * rhs_value0;
                res01 += lhs_value * rhs_value1;

                lhs_value  = lhs_ptr[rhs_cols];
                res10 += lhs_value * rhs_value0;
                res11 += lhs_value * rhs_value1;

                ++rhs_ptr;
                ++lhs_ptr;
            }

            // Quantize down
            res00 = tpt_nn_requantize(res00, dst_multipliers[rhs_rows_idx],     dst_shifts[rhs_rows_idx]);
            res01 = tpt_nn_requantize(res01, dst_multipliers[rhs_rows_idx + 1], dst_shifts[rhs_rows_idx + 1]);
            res10 = tpt_nn_requantize(res10, dst_multipliers[rhs_rows_idx],     dst_shifts[rhs_rows_idx]);
            res11 = tpt_nn_requantize(res11, dst_multipliers[rhs_rows_idx + 1], dst_shifts[rhs_rows_idx + 1]);

            // Add offset
            res00 += dst_offset;
            res01 += dst_offset;
            res10 += dst_offset;
            res11 += dst_offset;

            // Clamp the result
            res00 = MAX(res00, activation_min);
            res00 = MIN(res00, activation_max);
            res01 = MAX(res01, activation_min);
            res01 = MIN(res01, activation_max);
            res10 = MAX(res10, activation_min);
            res10 = MIN(res10, activation_max);
            res11 = MAX(res11, activation_min);
            res11 = MIN(res11, activation_max);

            dst_ptr[0] = (q7_t)res00;
            dst_ptr[1] = (q7_t)res01;
            dst_ptr += rhs_rows;
            dst_ptr[0] = (q7_t)res10;
            dst_ptr[1] = (q7_t)res11;
            dst_ptr += rhs_rows;

            lhs_ptr += rhs_cols;

            lhs_rows_idx--;
        }

        // Left-over rows
        if (lhs_rows & 0x1)
        {
            const q7_t *rhs_ptr = &rhs[0];

            q31_t res00 = lhs_offset_contribution0;
            q31_t res01 = lhs_offset_contribution1;

            for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
            {
                q7_t rhs_value0 = rhs_ptr[0];
                q7_t rhs_value1 = rhs_ptr[rhs_cols];
                q7_t lhs_value  = lhs_ptr[0];

                res00 += lhs_value * rhs_value0;
                res01 += lhs_value * rhs_value1;

                ++rhs_ptr;
                ++lhs_ptr;
            }

            // Quantize down
            res00 = tpt_nn_requantize(res00, dst_multipliers[rhs_rows_idx],     dst_shifts[rhs_rows_idx]);
            res01 = tpt_nn_requantize(res01, dst_multipliers[rhs_rows_idx + 1], dst_shifts[rhs_rows_idx + 1]);

            // Add offset
            res00 += dst_offset;
            res01 += dst_offset;

            // Clamp the result
            res00 = MAX(res00, activation_min);
            res00 = MIN(res00, activation_max);
            res01 = MAX(res01, activation_min);
            res01 = MIN(res01, activation_max);

            dst_ptr[0] = (q7_t)res00;
            dst_ptr[1] = (q7_t)res01;
        }

        rhs += 2 * rhs_cols;
        dst += 2;
    }

    if (rhs_rows & 0x1)
    {
        const q7_t *lhs_ptr = &lhs[0];
        q7_t *dst_ptr = &dst[0];

        for (int32_t lhs_rows_idx = 0; lhs_rows_idx < lhs_rows; ++lhs_rows_idx)
        {
            const q7_t *rhs_ptr = &rhs[0];
            q31_t res00 = bias[rhs_rows - 1];

            for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
            {
                q31_t rhs_value = rhs_ptr[0];
                q31_t lhs_value = lhs_ptr[0] + lhs_offset;

                res00 += lhs_value * rhs_value;

                ++rhs_ptr;
                ++lhs_ptr;
            }

            // Quantize down
            res00 = tpt_nn_requantize(res00, dst_multipliers[rhs_rows - 1], dst_shifts[rhs_rows - 1]);

            // Add offset
            res00 += dst_offset;

            // Clamp the result
            res00 = MAX(res00, activation_min);
            res00 = MIN(res00, activation_max);

            dst_ptr[0] = (q7_t)res00;
            dst_ptr += rhs_rows;
        }
    }
    return 0;
}
