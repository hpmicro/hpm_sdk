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

//// Activation Functions

#ifdef NEA_VEC_ISA
// following macro is for v-ext algorithm (if following macro is not defined,
// frec7 will be replaced by fdiv)
// #define ENA_FREC7
#endif

int32_t tpt_nn_tanh_f32(const float32_t * in_vec, uint32_t size, float32_t * out_vec)
{
    const float CONST_1 = 1.f;
    const float CONST_2 = 2.f;
    const float CONST_MIN_TANH = -10.f;
    const float CONST_MAX_TANH = 10.f;
    const float CONST_THR = 5.e-3;
    const float CONST_1_3 = 0.3333333f;

    long i;
    for(i=0; i<size; i++)
    {
        float x = in_vec[i];
        x = (x > CONST_MAX_TANH)? CONST_MAX_TANH : x;
        x = (x < CONST_MIN_TANH)? CONST_MIN_TANH : x;

        // x * (1 - x^2/3) if |x| < 5.e-3 or (exp2x - 1) / (exp2x + 1) otherwise
        if(fabs(x) < CONST_THR)
            out_vec[i] = x * (1 - x*x*CONST_1_3);
        else
            out_vec[i] = (exp_f32(CONST_2 * x) - CONST_1) / (exp_f32(CONST_2 * x) + CONST_1);
    }

    return 0;
}
