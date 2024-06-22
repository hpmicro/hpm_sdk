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

void tpt_nn_add_s8_sym_round(const q7_t * in_vec1,
                        const q7_t * in_vec2,
                        const uint32_t scale1,
                        const uint32_t scale2,
                        const uint32_t size,
                        const uint16_t pre_rshift,
                        const uint32_t out_scale,
                        const uint16_t post_rshift,
                        q7_t * out_vec)
{
    long loop_cnt = size;

    int in1, in2, sum;
    while(loop_cnt-- > 0u)
    {
        /* C = A + B */
        in1 = *in_vec1++ * scale1;
        in2 = *in_vec2++ * scale2;

        sum = in1 + in2;
        sum += 1 << (pre_rshift - 1);
        sum = (sum >> pre_rshift) * out_scale;
        sum += 1 << (post_rshift - 1);
        *out_vec++ = NDS_ISA_SATS((sum >> post_rshift), 8);
    }
}
