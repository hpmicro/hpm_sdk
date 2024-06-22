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

//// Convolution Functions

int32_t tpt_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(const q7_t *in_tensor,
                                    const uint16_t in_tensor_dim_x,
                                    const uint16_t in_tensor_dim_y,
                                    const uint16_t in_tensor_ch,
                                    const uint16_t in_tensor_batch,
                                    const q7_t *ker_weight,
                                    const uint16_t out_tensor_ch,
                                    const uint16_t pad_x,
                                    const uint16_t pad_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const int32_t *bias,
                                    q7_t *out_tensor,
                                    const int32_t *out_shift,
                                    const int32_t *out_scale,
                                    const int32_t out_offset,   //value is in the range of [-128, 127]
                                    const int32_t in_offset,    //value is in the range of [-127, 128]
                                    const int32_t act_min,
                                    const int32_t act_max,
                                    const uint16_t out_tensor_dim_x,
                                    const uint16_t out_tensor_dim_y,
                                    q15_t *tmp_buf)
{
    if (in_tensor_ch % 4 != 0 ||
        pad_x != 0 || pad_y != 0 ||
        stride_x != 1 || stride_y != 1)
    {
        return -1;
    }

    (void)in_tensor_dim_x;
    (void)in_tensor_dim_y;
    (void)out_tensor_dim_x;
    (void)out_tensor_dim_y;
    (void)tmp_buf;

    const int32_t lhs_rows = in_tensor_dim_x * in_tensor_dim_y * in_tensor_batch;
    const int32_t rhs_rows = out_tensor_ch;
    const int32_t rhs_cols = in_tensor_ch;

    tpt_nn_mat_mult_nt_t_s8(in_tensor,
                            ker_weight,
                            bias,
                            out_tensor,
                            out_scale,
                            out_shift,
                            lhs_rows,
                            rhs_rows,
                            rhs_cols,
                            in_offset,
                            out_offset,
                            act_min,
                            act_max);


    /* Return to application */
    return 0;
}

int32_t tpt_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(const uint16_t in_tensor_ch)
{
    (void)in_tensor_ch;
    return 0;
}
