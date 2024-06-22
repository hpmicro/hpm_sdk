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

//// Basic Operation Functions

/************************************************************************
* Note. NDS' shift amount (in_rshift1, in_rshift2 and out_rshift)       *
* are expected to be >=0; however, CMSIS' are expected to be <=0.       *
*************************************************************************/

int tpt_nn_ew_add_s16_asym(const int16_t *in_vec1,
                             const int16_t *in_vec2,
                             const int32_t in_offset1,
                             const int32_t in_scale1,
                             const int32_t in_rshift1,
                             const int32_t in_offset2,
                             const int32_t in_scale2,
                             const int32_t in_rshift2,
                             const int32_t lshift,
                             int16_t *out_vec,
                             const int32_t out_offset,
                             const int32_t out_scale,
                             const int32_t out_rshift,
                             const int32_t act_min,
                             const int32_t act_max,
                             const int32_t size)
{
    (void)in_offset1;
    (void)in_offset2;
    (void)out_offset;
    int32_t loop;

    loop = size;
    while (loop > 0)
    {
        int32_t input_1 = *in_vec1++ << lshift;
        int32_t input_2 = *in_vec2++ << lshift;

        input_1 = tpt_nn_requantize_ns(input_1, in_scale1, -in_rshift1);
        input_2 = tpt_nn_requantize_ns(input_2, in_scale2, -in_rshift2);

        int32_t sum = input_1 + input_2;
        sum = tpt_nn_requantize_ns(sum, out_scale, -out_rshift);

        sum = MAX(sum, act_min);
        sum = MIN(sum, act_max);

        *out_vec++ = sum;
        loop--;
    }

    return 0;
}
