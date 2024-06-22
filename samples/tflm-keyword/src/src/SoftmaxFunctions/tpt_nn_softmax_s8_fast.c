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

void tpt_nn_softmax_s8_fast(const q7_t * in_vec, const uint16_t size, q7_t * out_vec)
{
    q31_t sum;
    q15_t base;
    int16_t i;
    uint8_t shift;
    base = -257;

    /* We first search for the maximum */
    for (i = 0; i < size; i++)
    {
        if (in_vec[i] > base)
        {
            base = in_vec[i];
        }
    }

    /*
     * So the base is set to max-8, meaning
     * that we ignore really small values.
     * anyway, they will be 0 after shrinking to q7_t.
     */
    base = base - 8;
    sum = 0;

    for (i = 0; i < size; i++)
    {
        if (in_vec[i] > base)
        {
            shift = (uint8_t)NDS_ISA_CLIP(in_vec[i] - base, 5);
            sum += 0x1 << shift;
        }
    }

    /* This is effectively (0x1 << 20) / sum */
    int output_base = 0x100000 / sum;

    /*
     * Final confidence will be output_base >> ( 13 - (in_vec[i] - base) )
     * so 128 (0x1<<7) -> 100% confidence when sum = 0x1 << 8, output_base = 0x1 << 12
     * and in_vec[i]-base = 8
     */
    for (i = 0; i < size; i++)
    {
        if (in_vec[i] > base)
        {
            /* Here minimum value of 13+base-in_vec[i] will be 5 */
            shift = (uint8_t)NDS_ISA_CLIP(13 + base - in_vec[i], 5);
            out_vec[i] = (q7_t) NDS_ISA_SATS((output_base >> shift), 8);
        }
        else
        {
            out_vec[i] = 0;
        }
    }
}
