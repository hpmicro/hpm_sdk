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

//// Softmax Functions

#define ACCUM_BITS 12

void tpt_nn_softmax_s8_s16_hp(const int8_t *in_tensor,
                                const int32_t in_tensor_row,
                                const int32_t in_tensor_col,
                                const int32_t scale,
                                const int32_t lshift,
                                const int32_t diff_min,
                                int16_t *out_tensor)
{
    const int32_t mask = (1 << lshift);

    long col = 0;
    long row_idx;

    for (row_idx = 0; row_idx < in_tensor_row; ++row_idx)
    {
        // Find the maximum value in order to ensure numerical stability
        int8_t max;

        max = *in_tensor;
        for (col = 1; col < in_tensor_col; ++col)
        {
            max = MAX(max, in_tensor[col]);
        }

        long diff = 0;
        int32_t sum = 0;
        for (col = 0; col < in_tensor_col; ++col)
        {
            diff = in_tensor[col] - max;
            if (diff >= diff_min)
            {
                sum += DIV_POW2_V2(EXP_ON_NEG(MUL_SAT(diff * mask, scale)), ACCUM_BITS);
            }
        }

        const int32_t headroom = NDS_ISA_CLZ(sum);
        const int32_t bits_over_unit = ACCUM_BITS - headroom + 15;
        const int32_t shifted_scale = ONE_OVER1((sum > 0 ? sum << headroom : 0) - (1 << 31));

        for (col = 0; col < in_tensor_col; ++col)
        {
            diff = in_tensor[col] - max;
            if (diff >= diff_min)
            {
                const int32_t res = DIV_POW2_V2(MUL_SAT(shifted_scale, EXP_ON_NEG(MUL_SAT(diff * mask, scale))), bits_over_unit) + Q15_MIN;
                out_tensor[col] = tpt_nn_clip_any(res, (int32_t)Q15_MIN, (int32_t)Q15_MAX);
            }
            else
            {
                out_tensor[col] = Q15_MIN;
            }
        }
        in_tensor += in_tensor_col;
        out_tensor += in_tensor_col;
    }
}
