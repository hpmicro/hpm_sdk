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
#include "tpt_nn_support.h"      //for tpt_nn_dup_s16_x4_reordered

//// FullyConnected Functions

int32_t tpt_nn_fc_s16_s16_s16_sft_bias_fast(const q15_t * in_vec,
                        const q15_t * wt_mat,
                        const uint16_t size,
                        const uint16_t wt_row_num,
                        const uint16_t bias_lshift,
                        const uint16_t out_rshift,
                        const q15_t * bias,
                        q15_t * out_vec,
                        q15_t * in_tmp_buf)
{
    uint16_t  rowCnt = wt_row_num >> 2;
    const q15_t *pB = wt_mat;
    const q15_t *pA;
    q15_t    *pO = out_vec;
    const q15_t *pBias = bias;

    while (rowCnt)
    {
        q31_t     sum =  ((q31_t)(*pBias++) << bias_lshift) + NN_ROUND(out_rshift);
        q31_t     sum2 = ((q31_t)(*pBias++) << bias_lshift) + NN_ROUND(out_rshift);
        q31_t     sum3 = ((q31_t)(*pBias++) << bias_lshift) + NN_ROUND(out_rshift);
        q31_t     sum4 = ((q31_t)(*pBias++) << bias_lshift) + NN_ROUND(out_rshift);

        uint16_t  colCnt = size >> 1;

        pA = in_vec;
        while (colCnt)
        {
            q15_t     inA1 = *pA++;
            q15_t     inA2 = *pA++;

            q15_t     inB1 = *pB++;
            q15_t     inB2 = *pB++;
            sum += inA1 * inB1 + inA2 * inB2;

            inB1 = *pB++;
            inB2 = *pB++;
            sum2 += inA1 * inB1 + inA2 * inB2;

            inB1 = *pB++;
            inB2 = *pB++;
            sum3 += inA1 * inB1 + inA2 * inB2;

            inB1 = *pB++;
            inB2 = *pB++;
            sum4 += inA1 * inB1 + inA2 * inB2;

            colCnt--;
        }
        colCnt = size & 0x1;
        while (colCnt)
        {
            q15_t     inA = *pA++;
            q15_t     inB = *pB++;
            sum += inA * inB;
            inB = *pB++;
            sum2 += inA * inB;
            inB = *pB++;
            sum3 += inA * inB;
            inB = *pB++;
            sum4 += inA * inB;
            colCnt--;
        }
        *pO++ = (q15_t) NDS_ISA_SATS((sum >> out_rshift), 16);
        *pO++ = (q15_t) NDS_ISA_SATS((sum2 >> out_rshift), 16);
        *pO++ = (q15_t) NDS_ISA_SATS((sum3 >> out_rshift), 16);
        *pO++ = (q15_t) NDS_ISA_SATS((sum4 >> out_rshift), 16);

        rowCnt--;
    }
    rowCnt = wt_row_num & 0x3;

    while (rowCnt)
    {
        int       ip_out = ((q31_t)(*pBias++) << bias_lshift) + NN_ROUND(out_rshift);
        int       j;

        pA = in_vec;
        for (j = 0; j < size; j++)
        {
            q15_t     inA = *pA++;
            q15_t     inB = *pB++;
            ip_out += inA * inB;
        }
        *pO++ = (q15_t) NDS_ISA_SATS((ip_out >> out_rshift), 16);

        rowCnt--;
    }

    return 0;
}
