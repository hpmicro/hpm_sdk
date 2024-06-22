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

//// Util Functions

int tpt_nn_svdf_s8_state_s8(q31_t *tmp_buf,    //temp buffer for input
                              q31_t *tmp_buf2,   //temp buffer for output
                              const int32_t rank,
                              const int32_t in_offset,
                              const int32_t out_offset,
                              const int32_t in_act_min,
                              const int32_t in_act_max,
                              const int32_t out_act_min,
                              const int32_t out_act_max,
                              const int32_t in_scale,
                              const int32_t in_shift,
                              const int32_t out_scale,
                              const int32_t out_shift,
                              const int32_t in_batch,
                              const int32_t in_height,
                              const q7_t *in_tensor,
                              q7_t *state_tensor,
                              const int32_t wt_feature_batch,
                              const q7_t *wt_feature_tensor,
                              const int32_t wt_time_height,
                              const q7_t *wt_time_tensor,
                              const q31_t *bias,
                              q7_t *out_tensor)
{
    if (in_height < 0 || (Q31_MAX - in_height) < 16 || wt_time_height < 0)
    {
        return -1;
    }

    const int32_t unit_count = wt_feature_batch / rank;
    q31_t *buffer_a = (q31_t *)tmp_buf;
    q31_t *buffer_b = (q31_t *)tmp_buf2;

    memmove((int8_t *)state_tensor,
            (int8_t *)state_tensor + 1,
            (size_t)((in_batch * wt_feature_batch * wt_time_height - 1) * (int32_t)sizeof(int8_t)));

    for (int i_batch = 0; i_batch < in_batch; i_batch++)
    {
        q7_t *res_ptr = state_tensor + (wt_time_height * i_batch * wt_feature_batch) + (wt_time_height - 1);
        const q7_t *weight = wt_feature_tensor;
        const q7_t *input = in_tensor + i_batch * in_height;

        tpt_nn_vec_mat_mult_t_s8_v3(input,
                                      weight,
                                      NULL,
                                      res_ptr,
                                      -in_offset,
                                      0,
                                      0,
                                      in_scale,
                                      in_shift,
                                      in_height,
                                      wt_feature_batch,
                                      in_act_min,
                                      in_act_max,
                                      wt_time_height);
    }

    {
        q31_t *ptr_a = buffer_a;
        const q7_t *v2 = state_tensor;
        for (int i_batch = 0; i_batch < in_batch; i_batch++)
        {
            const q7_t *v1 = wt_time_tensor;

            for (int i_feature_batch = 0; i_feature_batch < wt_feature_batch; i_feature_batch++)
            {
                *ptr_a = 0;
                long sum = 0;
                int j = 0;
                for (; j < wt_time_height; j++)
                {
                    sum += *v1 * *v2;
                    v1++;
                    v2++;
                }

                *ptr_a = sum;
                ptr_a++;
            }
        }
    }

    if (bias)
    {
        if (unit_count == wt_feature_batch)
        {
            for (int i = 0; i < in_batch; i++)
            {
                q31_t *output_temp = buffer_b + i * wt_feature_batch;
                const q31_t *ptr_a = buffer_a + i * wt_feature_batch;

                const int32_t *bi = bias;
                int j = 0;
                for (; j < wt_feature_batch; j++)
                {
                    output_temp[j] = ptr_a[j] + bi[j];
                }
            }
        }
        else
        {
            for (int i_batch = 0; i_batch < in_batch; i_batch++)
            {
                q31_t *out_tensor_temp = buffer_b + i_batch * unit_count;
                q31_t *ptr_a = buffer_a + i_batch * wt_feature_batch;

                for (int i = 0; i < unit_count; i++)
                {
                    int32_t sum = bias[i];
                    int j = 0;
                    for (; j < rank; j++)
                    {
                        sum += *ptr_a;
                        ptr_a++;
                    }
                    out_tensor_temp[i] = sum;
                }
            }
        }
    }
    else
    {
        for (int i_batch = 0; i_batch < in_batch; i_batch++)
        {
            q31_t *out_tensor_temp = buffer_b + i_batch * unit_count;
            q31_t *ptr_a = buffer_a + i_batch * wt_feature_batch;

            for (int i = 0; i < unit_count; i++)
            {
                int32_t sum = 0;
                int j = 0;
                for (; j < rank; j++)
                {
                    sum += *ptr_a;
                    ptr_a++;
                }
                out_tensor_temp[i] = sum;
            }
        }
    }

    for (int i = 0; i < in_batch * unit_count; i++)
    {
        q31_t out = tpt_nn_requantize(buffer_b[i], out_scale, out_shift) + out_offset;
        out = MAX(out, out_act_min);
        out = MIN(out, out_act_max);
        out_tensor[i] = out;
    }

    return 0;
}
