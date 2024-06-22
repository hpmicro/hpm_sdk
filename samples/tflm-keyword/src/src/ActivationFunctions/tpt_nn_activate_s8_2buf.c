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

#include "tpt_nn_activation.h"
#include "internal_nn_table.h"
#include "internal_nn_math.h"

//// Activations Functions

void tpt_nn_activate_s8_2buf(q7_t * FUNC_RESTRICT in_vec, uint32_t size, uint16_t int_bits, tpt_nn_activation_fun act_fun, q7_t * FUNC_RESTRICT out_vec)
{
    uint16_t  shift_size = 3 - int_bits;
    const q7_t *lookup_table;
    switch (act_fun)
    {
    case NN_SIGMOID:
        lookup_table = sigmoidTable_q7;
        break;
    case NN_TANH:
    default:
        lookup_table = tanhTable_q7;
        break;
    }

    uint32_t i = size;
    q7_t *pIn = in_vec;
    q7_t *pOut = out_vec;
    q7_t in;
    q7_t out;

    while (i)
    {
        in = *pIn++;
        out = lookup_table[(uint8_t)(in >> shift_size)];
        *pOut++ = out;
        i--;
    }
}
