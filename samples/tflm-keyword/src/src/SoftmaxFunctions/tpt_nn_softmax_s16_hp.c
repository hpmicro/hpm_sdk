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

int tpt_nn_softmax_s16_hp(const int16_t *in_tensor,
                             const int32_t in_tensor_row,
                             const int32_t in_tensor_col,
                             const int32_t scale,
                             const int32_t shift,
                             const int16_t *exp_lut,
                             const int16_t *one_by_one_lut,
                             int16_t *out_tensor)
{
    int32_t col = 0;
    int32_t row_idx;

    for (row_idx = 0; row_idx < in_tensor_row; ++row_idx)
    {
        // Find the maximum value in order to ensure numerical stability
        int16_t max = *in_tensor;
        for (col = 1; col < in_tensor_col; ++col)
        {
            max = MAX(max, in_tensor[col]);
        }

        int32_t diff = 0;
        int32_t sum = 0;
        int16_t *cached_exp_results = out_tensor;

        for (col = 0; col < in_tensor_col; ++col)
        {
            diff = in_tensor[col] - max;
            const int32_t scaled_diff = tpt_nn_requantize_ps(diff, scale, shift);
            const int32_t symmetric_scaled_diff = scaled_diff + Q15_MAX;
            const int16_t saturated_symmetric_scaled_diff = MIN(MAX(symmetric_scaled_diff, Q15_MIN), Q15_MAX);

            // Lookup from exp table and cache result for next step
            const int16_t index = (256 + (saturated_symmetric_scaled_diff >> 7));
            const int16_t offset = saturated_symmetric_scaled_diff & 0x7f;
            const int16_t base = exp_lut[index];
            const int16_t slope = exp_lut[index + 1] - exp_lut[index];
            const int16_t delta = (slope * offset + 64) >> 7;
            const int16_t result = (base + delta);
            cached_exp_results[col] = result;

            sum += cached_exp_results[col];
        }

        const int32_t headroom = NDS_ISA_CLZ(sum);

        // Compute the reciprocal 1/sum
        const int32_t shifted_sum = (((sum) << (headroom - 1)) + (1 << 13)) >> 14;

        // Since LUT computes 1/(1 + x), compute x = (sum - 1) => -65536
        // Since LUT expects a symmetrical in_tensor, recenter from [UINT16_MIN, UINT16_MAX] to [INT16_MIN, INT16_MAX] =>
        // -32768 ==> So in total -65536 -32768 => -98304
        const int16_t symmetric_shifted_sum = shifted_sum - 98304;

        // Lookup from one by one table
        const int16_t index = (256 + (symmetric_shifted_sum >> 7));
        const int16_t offset = symmetric_shifted_sum & 0x7f;
        const int16_t base = one_by_one_lut[index];
        const int16_t slope = one_by_one_lut[index + 1] - one_by_one_lut[index];
        const int16_t delta = (slope * offset + 64) >> 7;
        const int16_t one_by_one_result = (base + delta);

        for (col = 0; col < in_tensor_col; ++col)
        {
            const int16_t right_shift = 30 - headroom;
            int32_t result = (cached_exp_results[col] * one_by_one_result) >> right_shift;
            result = (result + 1) >> 1; // Last shift position and insert round
            out_tensor[col] = (int16_t)result;
        }
        out_tensor += in_tensor_col;
        in_tensor += in_tensor_col;
    }

    return 0;
}
