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

//// Util Functions

typedef union
{
    float32_t f32;
    int32_t   i32;
} _union32_t;

//--- const values for taylor_poly_f32 ---
// Exponent polynomial coefficients
const float CONST_COE0 = 1.f;
const float CONST_COE1 = 0.0416598916054f;
const float CONST_COE2 = 0.500000596046f;
const float CONST_COE3 = 0.0014122662833f;
const float CONST_COE4 = 1.00000011921f;
const float CONST_COE5 = 0.00833693705499f;
const float CONST_COE6 = 0.166665703058f;
const float CONST_COE7 = 0.000195780929062f;

//--- const values for exp_f32 ---
const float CONST_LN2          = 0.6931471805f;     // ln(2)
const float CONST_INV_LN2      = 1.4426950408f;     // 1/ln(2)
const float CONST_INF          = INFINITY;
const float CONST_MAX_INPUT    = 88.7f;
const float CONST_0            = 0.f;
const int   CONST_NEGATIVE_126 = -126;

static inline float taylor_poly_f32(float x)
{
    float A = CONST_COE0 + CONST_COE4 * x;
    float B = CONST_COE2 + CONST_COE6 * x;
    float C = CONST_COE1 + CONST_COE5 * x;
    float D = CONST_COE3 + CONST_COE7 * x;
    float x2 = x * x;
    float x4 = x2 * x2;
    float res = (A + B * x2) + (C + D * x2) * x4;
    return res;
}

float exp_f32(float x)
{
    // Perform range reduction [-log(2),log(2)]
    int m = x * CONST_INV_LN2;
    float val = x - (float)m * CONST_LN2;

    // Polynomial Approximation
    _union32_t poly;
    poly.f32 = taylor_poly_f32(val);

    // Reconstruct
    int m2 = m << 23;       //shift left and saturate
    if ((m2 >> 23) != m)
        m2 = (m >> 31) ^ 0x7FFFFFFF;

    poly.i32 = NDS_ISA_KADDW(poly.i32, m2);
    // Handle overflow
    poly.f32 = (m < CONST_NEGATIVE_126)? CONST_0 : poly.f32;
    poly.f32 = (x > CONST_MAX_INPUT)? CONST_INF : poly.f32;

    return poly.f32;
}

int32_t tpt_nn_exp_f32(const float32_t * in_vec, uint32_t size, float32_t * out_vec)
{
    while(size-- > 0)
    {
        *out_vec = exp_f32(*in_vec);
        in_vec++;
        out_vec++;
    }

    return 0;
}
