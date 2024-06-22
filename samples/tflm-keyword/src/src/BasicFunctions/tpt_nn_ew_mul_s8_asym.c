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

int tpt_nn_ew_mul_s8_asym(const int8_t *in_vec1,
                    const int8_t *in_vec2,
                    const int32_t in_offset1,
                    const int32_t in_offset2,
                    int8_t *out_vec,
                    const int32_t out_offset,
                    const int32_t out_scale,
                    const int32_t out_shift,
                    const int32_t act_min,
                    const int32_t act_max,
                    const uint32_t size)
{
    uint32_t loop;
    int32_t in1, in2, output;

    loop = size;

    while (loop > 0)
    {
        in1 = *in_vec1++ + in_offset1;
        in2 = *in_vec2++ + in_offset2;

        output = in1 * in2;
        output = tpt_nn_requantize(output, out_scale, out_shift) + out_offset;
        output = tpt_nn_clip_any(output, act_min, act_max);

        *out_vec++ = output;
        loop--;
    }

  return 0;
}
