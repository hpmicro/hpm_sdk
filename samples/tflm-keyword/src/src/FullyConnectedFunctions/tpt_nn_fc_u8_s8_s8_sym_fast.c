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
#include "tpt_nn_support.h"

//// FullyConnected Functions

int32_t tpt_nn_fc_u8_s8_s8_sym_fast(const u8_t * in_vec,
                                  const q7_t * wt_mat,
                                  const uint16_t size,
                                  const uint16_t wt_row_num,
                                  const uint16_t pre_rshift,
                                  const uint16_t out_scale,
                                  const uint16_t post_rshift,
                                  q7_t * out_vec,
                                  q15_t * in_tmp_buf)
{
    uint16_t  rowCnt = wt_row_num >> 2;
    const q7_t *pB = wt_mat;
    const u8_t *pA;
    q7_t     *pO = out_vec;

    while (rowCnt)
    {
        q31_t     sum =  0;
        q31_t     sum2 = 0;
        q31_t     sum3 = 0;
        q31_t     sum4 = 0;

        uint16_t  colCnt = size >> 2;

        pA = in_vec;

        while (colCnt)
        {
            u8_t      inA1 = *pA++;
            u8_t      inA3 = *pA++;
            u8_t      inA2 = *pA++;
            u8_t      inA4 = *pA++;

            q7_t      inB1 = *pB++;
            q7_t      inB3 = *pB++;
            q7_t      inB2 = *pB++;
            q7_t      inB4 = *pB++;

            sum += inA1 * inB1 + inA2 * inB2;
            sum2 += inA1 * inB3 + inA2 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum3 += inA1 * inB1 + inA2 * inB2;
            sum4 += inA1 * inB3 + inA2 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum += inA3 * inB1 + inA4 * inB2;
            sum2 += inA3 * inB3 + inA4 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum3 += inA3 * inB1 + inA4 * inB2;
            sum4 += inA3 * inB3 + inA4 * inB4;

            colCnt--;
        }
        colCnt = size & 0x3;
        while (colCnt)
        {
            u8_t      inA = *pA++;
            q7_t      inB = *pB++;
            sum += inA * inB;
            inB = *pB++;
            sum2 += inA * inB;
            inB = *pB++;
            sum3 += inA * inB;
            inB = *pB++;
            sum4 += inA * inB;

            colCnt--;
        }
        sum = (sum >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
        sum2 = (sum2 >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
        sum3 = (sum3 >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
        sum4 = (sum4 >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
        *pO++ = NDS_ISA_SATS((sum >> post_rshift), 8);
        *pO++ = NDS_ISA_SATS((sum2 >> post_rshift), 8);
        *pO++ = NDS_ISA_SATS((sum3 >> post_rshift), 8);
        *pO++ = NDS_ISA_SATS((sum4 >> post_rshift), 8);
        rowCnt--;
    }

    rowCnt = wt_row_num & 0x3;

    while (rowCnt)
    {
        int ip_out = 0;
        int j;

        pA = in_vec;
        for (j = 0; j < size; j++)
        {
            u8_t      inA = *pA++;
            q7_t      inB = *pB++;
            ip_out += inA * inB;
        }
        ip_out = (ip_out >> pre_rshift) * out_scale + NN_ROUND(post_rshift);
        *pO++ = NDS_ISA_SATS((ip_out >> post_rshift), 8);
        rowCnt--;
    }
    return 0;
}
