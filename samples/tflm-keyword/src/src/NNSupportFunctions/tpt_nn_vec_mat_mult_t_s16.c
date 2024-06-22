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

int32_t tpt_nn_vec_mat_mult_t_s16(const int16_t *lhs,
                                const int8_t *rhs,
                                const int64_t *bias,
                                int16_t *dst,
                                // const int32_t lhs_offset,
                                // const int32_t rhs_offset,
                                // const int32_t dst_offset,
                                const int32_t dst_multiplier,
                                const int32_t dst_shift,
                                const int32_t rhs_cols,
                                const int32_t rhs_rows,
                                const int32_t activation_min,
                                const int32_t activation_max)
{
    for (int rhs_row_idx = 0; rhs_row_idx < rhs_rows; rhs_row_idx++)
    {
        const int16_t *lhs_ptr = lhs;
        const int8_t  *rhs_ptr = &rhs[0];

        q63_t result = 0;

        if (bias)
        {
            result = *bias++;
        }
        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            const int64_t rhs_value = (int8_t)*rhs_ptr;
            const int64_t lhs_value = *lhs_ptr;

            result += lhs_value * rhs_value;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        result = tpt_nn_requantize_s64(result, dst_multiplier, dst_shift);

        result = ((result) > (activation_min) ? (result) : (activation_min));
        result = ((result) < (activation_max) ? (result) : (activation_max));

        *dst++ = result;
        rhs += rhs_cols;
    }

    return 0;
}
