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

int tpt_nn_vec_mat_mult_t_svdf_s8(const q7_t *lhs,
                                    const q7_t *rhs,
                                    q15_t *dst,
                                    const int32_t lhs_offset,
                                    const int32_t rhs_offset,
                                    const int32_t dst_offset,
                                    const int32_t dst_multiplier,
                                    const int32_t dst_shift,
                                    const int32_t rhs_cols,
                                    const int32_t rhs_rows,
                                    const int32_t activation_min,
                                    const int32_t activation_max)
{
    (void)rhs_offset;

    int32_t row_loop_cnt = rhs_rows / 3;
    for (int i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
    {
        const q7_t *lhs_ptr = lhs;
        const q7_t *rhs_ptr_0 = &rhs[0];
        const q7_t *rhs_ptr_1 = &rhs[rhs_cols];
        const q7_t *rhs_ptr_2 = &rhs[rhs_cols * 2];

        q31_t res00 = 0;
        q31_t res01 = 0;
        q31_t res02 = 0;
        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            const q31_t rhs_value0 = (int8_t)*rhs_ptr_0;
            const q31_t rhs_value1 = (int8_t)*rhs_ptr_1;
            const q31_t rhs_value2 = (int8_t)*rhs_ptr_2;
            const q31_t lhs_value = (int8_t)*lhs_ptr + lhs_offset;

            res00 += lhs_value * rhs_value0;
            res01 += lhs_value * rhs_value1;
            res02 += lhs_value * rhs_value2;

            ++rhs_ptr_0;
            ++rhs_ptr_1;
            ++rhs_ptr_2;
            ++lhs_ptr;
        }
        // Quantize down
        res00 = tpt_nn_requantize(res00, dst_multiplier, dst_shift);
        res01 = tpt_nn_requantize(res01, dst_multiplier, dst_shift);
        res02 = tpt_nn_requantize(res02, dst_multiplier, dst_shift);

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);
        res01 = MAX(res01, activation_min);
        res01 = MIN(res01, activation_max);
        res02 = MAX(res02, activation_min);
        res02 = MIN(res02, activation_max);

        *dst = (q15_t)res00;
        *(dst + dst_offset) = (q15_t)res01;
        *(dst + 2 * dst_offset) = (q15_t)res02;
        dst += 3 * dst_offset;
        rhs += 3 * rhs_cols;
    }

    const int loop_cnt = rhs_rows % 3;
    for (int i_loop_cnt = 0; i_loop_cnt < loop_cnt; i_loop_cnt++)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];

        q31_t res00 = 0;

        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = (int8_t)rhs_ptr[0];
            q31_t lhs_value  = (int8_t)lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // Quantize down
        res00 = tpt_nn_requantize(res00, dst_multiplier, dst_shift);

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);

        *dst = (q15_t)res00;
        dst += dst_offset;
        rhs += rhs_cols;
    }
    return 0;
}