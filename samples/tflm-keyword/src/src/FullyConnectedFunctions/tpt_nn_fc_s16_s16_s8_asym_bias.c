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

int32_t tpt_nn_fc_s16_s16_s8_asym_bias(const int16_t *in_vec,
    const int8_t *wt_mat,
    const int32_t in_vec_col,
    const int32_t wt_mat_row,
    const int32_t in_vec_batch,
    const int32_t in_offset,
    const int32_t wt_offset,
    const int32_t out_scale,
    const int32_t out_shift,
    const int32_t out_offset,
    const int64_t *bias,
    int16_t *out_vec,
    const int32_t act_min,
    const int32_t act_max,
    int16_t *tmp_buf)
{
    (void)in_offset;
    (void)wt_offset;
    (void)out_offset;
    (void)tmp_buf;

    int32_t batch_cnt = in_vec_batch;

    while (batch_cnt)
    {
        tpt_nn_vec_mat_mult_t_s16(in_vec,
                                    wt_mat,
                                    bias,
                                    out_vec,
                                    out_scale,
                                    out_shift,
                                    in_vec_col,
                                    wt_mat_row,
                                    act_min,
                                    act_max);
        in_vec += in_vec_col;
        out_vec += wt_mat_row;
        batch_cnt--;
    }

    return 0;
}

int32_t tpt_nn_fc_s16_s16_s8_asym_bias_get_buffer_size(const uint16_t in_vec_col)
{
    (void)in_vec_col;
    return 0;
}
