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

#ifndef __RISCV_DSP_SORT_MATH_H__
#define __RISCV_DSP_SORT_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"
/**
 * @defgroup sort Sort Functions
 */
/**
 * @addtogroup sort
 * @{
 */

/**
 * @brief Struct for specifying sorting algorithm
 */
typedef enum
{
  RISCV_DSP_SORT_DESCENDING = 0,      /**< Descending order */
  RISCV_DSP_SORT_ASCENDING = 1        /**< Ascending order  */
} riscv_dsp_sort_order;

/**
 * @defgroup gen_sort Generic Sort Functions
 * @brief Generic Sort Functions
 *
 * The generic sort function sorts elements of a vector by the algorithm and sorting order specified
 * in its instance structure. The algorithms to be chosen from to perform the generic sorting
 * include bitonic sort, bubble sort, heap sort, insertion sort, quick sort and selection sort.
 * Andes DSP library only supports the generic sort function for floating-point data. The function
 * and its instance structures are introduced in the subsections below.
 */
/**
 * @addtogroup gen_sort
 * @{
 */

/**
 * @brief Struct for specifying sorting algorithm
 */
typedef enum
{
    RISCV_DSP_SORT_BITONIC   = 0,         /**< Bitonic sort   */
    RISCV_DSP_SORT_BUBBLE    = 1,         /**< Bubble sort    */
    RISCV_DSP_SORT_HEAP      = 2,         /**< Heap sort      */
    RISCV_DSP_SORT_INSERTION = 3,         /**< Insertion sort */
    RISCV_DSP_SORT_QUICK     = 4,         /**< Quick sort     */
    RISCV_DSP_SORT_SELECTION = 5          /**< Selection sort */
} riscv_dsp_sort_alg;

/**
 * @brief Instance structure for the sorting algorithms.
 */
typedef struct
{
  riscv_dsp_sort_alg   alg;        /**< desired sorting algorithm */
  riscv_dsp_sort_order order;      /**< desired sorting order */
} riscv_dsp_sort_f32_t;

/**
 * @param[in,out]  instance     pointer of the instance structure
 * @param[in]      alg          desired sorting algorithm
 * @param[in]      order        desired sorting order
 *
 * @b Note:
 *
 * 1. This function has to be called to initialize the instance structure before the function
 *    riscv_dsp_sort_f32 is executed. Please refer to code examples.
 *
 * 2. The possible sorting algorithms for the generic sorting (i.e., options for alg) include
 *    - RISCV_DSP_SORT_BITONIC      bitonic sort
 *    - RISCV_DSP_SORT_BUBBLE       bubble sort
 *    - RISCV_DSP_SORT_HEAP         heap sort
 *    - RISCV_DSP_SORT_INSERTION    insertion sort
 *    - RISCV_DSP_SORT_QUICK        quick sort
 *    - RISCV_DSP_SORT_SELECTION    selection sort
 *
 * 3. The possible sorting orders for the generic sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 */
void riscv_dsp_sort_init_f32(riscv_dsp_sort_f32_t * instance, riscv_dsp_sort_alg alg, riscv_dsp_sort_order order);

/**
 * @brief Generic sorting function
 *
 * @param[in]  instance  pointer of the instance structure
 * @param[in]  src       pointer of the input vector
 * @param[out] dst       pointer of the output vector
 * @param[in]  size      number of elements in a vector
 *
 * @b Note:
 *
 * 1. The possible sorting algorithms for the generic sorting (i.e., options for alg) include
 *    - RISCV_DSP_SORT_BITONIC      bitonic sort
 *    - RISCV_DSP_SORT_BUBBLE       bubble sort
 *    - RISCV_DSP_SORT_HEAP         heap sort
 *    - RISCV_DSP_SORT_INSERTION    insertion sort
 *    - RISCV_DSP_SORT_QUICK        quick sort
 *    - RISCV_DSP_SORT_SELECTION    selection sort
 *
 * 2. The possible sorting orders for the generic sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 *
 * 3. To ensure correct results, you must initialize the instance structure with the function
 *    riscv_dsp_sort_init_f32 before using this function riscv_dsp_sort_f32. For
 *    how to use the two functions, please refer to the code examples below.
 *
 * @b Example
 *     <pre>
 *       With the input size as 100, sorting order as ascending and sorting algorithm as quick
 *       sort, the code example of generic sorting is as follows:
 *
 *          \#define size 100
 *          riscv_dsp_sort_f32_t *instance;
 *          float32_t src[size] = {…};
 *          float32_t dst[size];
 *          riscv_dsp_sort_init_f32(instance, RISCV_DSP_SORT_QUICK,
 *          RISCV_DSP_SORT_ASCENDING);
 *          riscv_dsp_sort_f32(instance, src, dst, size);
 *     </pre>
 */
void riscv_dsp_sort_f32(const riscv_dsp_sort_f32_t * instance,float32_t * src, float32_t * dst, uint32_t size);

/** @} gen_sort */

/**
 * @defgroup merge_sort Merge Sort Functions
 * @brief Merge Sort Functions
 *
 * The merge sort function sorts elements of a vector by the sorting order specified in its instance
 * structure and a divide and conquer algorithm, which divides the input array in sublists and
 * conquer them to produce longer sorted sublists until there is only one list remaining. A buffer is
 * required by the algorithm to store the conquered values.
 * Andes DSP library only supports the merge sort function for floating-point data. The function
 * and its instance structures are introduced in the subsections below.
 */

/**
 * @addtogroup merge_sort
 * @{
 */

/**
 * @brief Instance structure for the merge sorting algorithms.
 */
typedef struct
{
  riscv_dsp_sort_order order;   /**< desired sorting order */
  float32_t * buf;              /**< pointer of the working buffer */
} riscv_dsp_sort_merge_f32_t;
/**
 * @param[in, out]  instance  pointer of the instance structure.
 * @param[in]       order     desired sorting order
 * @param[in]       buf       pointer of the working buffer
 *
 * @b Note:
 *
 * 1. This function has to be called to initialize the instance structure before the function
 *    riscv_dsp_sort_merge_f32 is executed. Please refer to Section 2.11.2.2 for a code
 *    example.
 *
 * 2. The possible sorting orders for the merge sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 */
void riscv_dsp_sort_merge_init_f32(riscv_dsp_sort_merge_f32_t * instance, riscv_dsp_sort_order order, float32_t * buf);

/**
 * @brief Merge sort
 *
 * @param[in]  instance  pointer of the instance structure.
 * @param[in]  src       pointer of the input vector
 * @param[out] dst       pointer of the output vector
 * @param[in]  size      number of elements in a vector
 *
 * @b Note:
 *
 * 1. The possible sorting orders for the merge sorting (i.e., options for order) include
 *    - RISCV_DSP_SORT_DESCENDING   descending order
 *    - RISCV_DSP_SORT_ASCENDING    ascending order
 *
 * 2. To ensure correct results, you must initialize the instance structure with the function
 *    riscv_dsp_sort_merge_init_f32 before using this function
 *    riscv_dsp_sort_merge_f32. For how to use the two functions, please refer to the
 *    code example below.
 *
 * @b Example
 *     <pre>
 *     With the input size as 100 and sorting order as descending, the code example of merge
 *     sorting is as follows:
 *
 *     \#define size 100
 *     riscv_dsp_sort_merge_f32_t *instance;
 *     float32_t src[size] = {…};
 *     float32_t buf[size];
 *     float32_t dst[size];
 *     riscv_dsp_sort_merge_init_f32(instance, RISCV_DSP_SORT_DESCENDING, buf);
 *     riscv_dsp_sort_merge_f32(instance, src, dst, size);
 *     </pre>
 */
void riscv_dsp_sort_merge_f32(const riscv_dsp_sort_merge_f32_t * instance, float32_t * src, float32_t * dst, uint32_t size);

/** @} merge_sort */

/** @} sort */

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP_SORT_MATH_H__
