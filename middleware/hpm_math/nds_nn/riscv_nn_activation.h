/******************************************************************************
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.*
 * Copyright (C) 2018-2021 Andes Technology Corporation. All rights reserved. *
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

#ifndef __RISCV_NN_ACTIVATION_H__
#define __RISCV_NN_ACTIVATION_H__

#ifdef __cplusplus
extern    "C"
{
#endif

#include "riscv_math_types.h"

/**
 * @brief This is the struct to select an activation function.
 *
 */
typedef enum
{
    NN_SIGMOID = 0,    /**< Use sigmoid activation function */
    NN_TANH = 1,       /**< Use tanh activation function */
} riscv_nn_activation_fun;

/**
 * @defgroup Activation Activation Functions
 * @brief The activation functions are used to filter out some input data. They
 *        include sigmoid, tanh and ReLU (Rectified Linear Unit) functions.
 *
 * @{
 */

/**
 * @brief           This function uses the sigmoid or tanh function to perform
 *                  activation for signed 8-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       int_bits    number of the bits in the integer part, which is
 *                              supposed to be smaller than 4
 * @param[in]       act_fun     selection of activation functions. See the Note
 *                              below for details.
 * @return          None
 *
 * @note
 * The available activation functions for selection include:
 *  - NN_SIGMOID: Use the sigmoid activation function
 *  - NN_TANH: Use the tanh activation function
 *
 * @b Example:
 * @code
 * #define SIZE 32
 * q7_t in_out[SIZE] = {...};
 * riscv_nn_activate_s8(in_out, SIZE, 0, NN_SIGMOID);
 * @endcode
 */
void riscv_nn_activate_s8(q7_t * in_out,
                        uint32_t size,
                        uint16_t int_bits,
                        riscv_nn_activation_fun act_fun);

/**
 * @brief           This function uses sigmoid or tanh function to perform
 *                  activation for signed 16-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       int_bits    number of the bits in the integer part, which is
 *                              supposed to be smaller than 4
 * @param[in]       act_fun     selection of activation functions. See the Note
 *                              below for details.
 * @return          None
 *
 * @note
 * The availbale activation functions for selection include:
 *  - NN_SIGMOID: Use the sigmoid activation function
 *  - NN_TANH: Use the tanh activation function
 */
void riscv_nn_activate_s16(q15_t * in_out,
                        uint32_t size,
                        uint16_t int_bits,
                        riscv_nn_activation_fun act_fun);

/**
 * @brief           This function uses the leaky ReLU function to perform
 *                  activation for signed 8-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       slope       slope value to be multiplied with the negative
 *                              inputs. The result will be right shifted 15 bits
 *                              to scale back to signed 8-bit integer.
 * @return          None
 *
 * @b Example:
 * @code
 * #define SIZE 1024
 * q15_t slope = 16384;
 * q7_t in_out[SIZE] = {...};
 * riscv_nn_leaky_relu_s8(in_out, SIZE, slope);
 * @endcode
 */
void riscv_nn_leaky_relu_s8(q7_t * in_out,
                        uint32_t size,
                        q15_t slope);

/**
 * @brief           This function uses the ReLU function to perform activation
 *                  for signed 8-bit integer input vectors.
 * @param[in,out]   data        pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[in]       max_val     maximum value to limit the output vector
 * @return          None
 */
void riscv_nn_relu_any_s8(q7_t *data, uint16_t size, q7_t max_val);

/**
 * @brief           This function uses the ReLU function to perform activation
 *                  for signed 8-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @return          None
 *
 * @b Example:
 * @code
 * #define H 16
 * #define W 16
 * #define CH 5
 * #define NUM (H * W * CH)
 * q7_t in_out[NUM] = {...};
 * riscv_nn_relu_s8(in_out, NUM);
 * @endcode
 */
void riscv_nn_relu_s8(q7_t * in_out, uint32_t size);

/**
 * @brief           This function uses the ReLU function to perform activation
 *                  for signed 16-bit integer input vectors.
 * @param[in,out]   in_out      pointer of the input/output vector
 * @param[in]       size        number of elements in the input/output vector
 * @return          None
 */
void riscv_nn_relu_s16(q15_t * in_out, uint32_t size);

#ifdef __riscv_zfh
/**
 * @brief           This function uses the sigmoid function to perform
 *                  activation for 16-bit half-precision floating point input
 *                  vectors.
 * @param[in]       in_vec      pointer of the input vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[out]      out_vec     pointer of the output vector
 * @return          This function returns 0.
 */
int32_t riscv_nn_sigmoid_f16(const float16_t * in_vec,
                            uint32_t size,
                            float16_t * out_vec);

/**
 * @brief           This function uses the tanh function to perform activation
 *                  for 16-bit half-precision floating point input vectors.
 * @param[in]       in_vec      pointer of the input vector
 * @param[in]       size        number of elements in the input/output vector
 * @param[out]      out_vec     pointer of the output vector
 * @return          This function returns 0.
 */
int32_t riscv_nn_tanh_f16(const float16_t * in_vec,
                        uint32_t size,
                        float16_t * out_vec);
#endif

/**
 *   * @}
 */

#ifdef __cplusplus
}
#endif

#endif
