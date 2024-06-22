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
#include "internal_nn_table.h"
#include "tpt_nn_activation.h"

//// Activations Functions

void tpt_nn_activate_s16_2buf(q15_t* FUNC_RESTRICT in_vec,
                                uint32_t size,
                                uint16_t int_bits,
                                tpt_nn_activation_fun act_fun,
                                q15_t* FUNC_RESTRICT out_vec) {
  uint16_t shift_size = 8 + 3 - int_bits;
  uint32_t bit_mask = 0x7FF >> int_bits;
  uint32_t full_frac = bit_mask + 1;
  const q15_t* lookup_table;

  switch (act_fun) {
    case NN_SIGMOID:
      lookup_table = sigmoidTable_q15;
      break;
    case NN_TANH:
    default:
      lookup_table = tanhTable_q15;
      break;
  }

  uint32_t i = size;
  q15_t* pIn = in_vec;
  q15_t* pOut = out_vec;

  while (i) {
    q15_t out;
    q15_t in = *pIn++;
    q15_t frac = (uint32_t)in & bit_mask;
    q15_t value = lookup_table[(uint8_t)(in >> shift_size)];
    if ((in >> shift_size) != 0x7f) {
      q15_t value2 = lookup_table[(uint8_t)(1 + ((uint8_t)(in >> shift_size)))];
      /* doing the interpolation here for better accuracy */
      out = ((q31_t)(full_frac - frac) * value + (q31_t)value2 * frac) >>
            shift_size;
    } else {
      /* the largest positive value does not have a right side for linear
       * interpolation */
      out = value;
    }

    *pOut++ = out;
    i--;

  }
}
