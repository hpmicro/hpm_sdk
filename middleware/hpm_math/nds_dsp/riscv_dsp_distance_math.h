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
#ifndef __RISCV_DSP_DISTANCE_MATH_H__
#define __RISCV_DSP_DISTANCE_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup dist Distance Functions
 * @brief Distance Functions
 *
 * This set of functions computes the distances between two vectors. Andes DSP library supports
 * distinct distance functions for floating-point and Boolean values. These functions are introduced
 * in the subsections below.
 */
/**
 * @addtogroup dist
 * @{
 */

/**
 * @defgroup fdist Float Distance
 * @brief Float Distance
 *
 * This set of functions computes distances between two vectors of float values.
 *
 * The following example applies to all Andes float distance functions except
 * riscv_dsp_dist_minkowski_f32, whose difference is shown in its section.
 *
 * @b Example
 * <pre>
 *      \#define size 4
 *      float32_t src1[size] = {1.0, 0.5, 0.4, -0.1};
 *      float32_t src2[size] = {0.40, -0.2, 0.24, 0.4};
 *      float32_t dist_out;
 *      dist_out = riscv_dsp_dist_bray_curtis_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_canberra_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_chebyshev_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_city_block_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_corr_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_cos_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_euclidean_f32(src1, src2, size);
 *      dist_out = riscv_dsp_dist_jensen_shannon_f32(src1, src2, size);
 * </pre>
 */
/**
 * @addtogroup fdist
 * @{
 */
/**
 * @brief        Bray-Curtis distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_bray_curtis_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Canberra distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_canberra_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Chebyshev distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_chebyshev_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Cityblock (Manhattan) distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_city_block_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Correlation distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_corr_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Cosine distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_cos_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Euclidean distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return distance
 * @return Distance value
 */
float32_t riscv_dsp_dist_euclidean_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Jensen-Shannon distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 */
float32_t riscv_dsp_dist_jensen_shannon_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Minkowski distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    order        distance order
 * @param[in]    size         size of elements in a vector
 * @return Distance value
 *
 * @b Example
 *     <pre>
 *      \#define size 4
 *      \#define order 3
 *      float32_t src1[size] = {1.0, 0.5, 0.4, -0.1};
 *      float32_t src2[size] = {0.40, -0.2, 0.24, 0.4};
 *      float32_t dist_out;
 *      dist_out = riscv_dsp_dist_minkowski_f32(src1, src2, order, size);
 *     </pre>
 */
float32_t riscv_dsp_dist_minkowski_f32(const float32_t *src1, const float32_t *src2, int32_t order, uint32_t size);
/** @} fdist */

/**
 * @defgroup bdist Boolean Distance
 * @brief Boolean Distance
 *
 * This set of functions computes distances between two vectors of Boolean values. Each Boolean
 * value is stored as 1 bit and has to be packed in a 32-bit word from most significant bit (MSB).
 *
 * As shown below, given 120 Boolean numbers, you will need to pack each src input of the desired
 * Boolean distance function with four 32-bit big-endian words. That is, put three 32 bits of
 * Boolean value in three different words and the remaining 24 bits in the MSB of the fourth word
 * in which Bit 0 to 7 are don't-care. This example applies to all Andes Boolean distance functions.
 *
 * @b Example
 * <pre>
 *      \#define numofbool 120
 *      uint32_t src1[4] = {0x12322111, 0x24421111, 0x012ff241, 0x33c31100};
 *      uint32_t src2[4] = {0x22222222, 0x11ea3222, 0x13465648, 0x5b351100};
 *      float32_t dist_out;
 *      dist_out = riscv_dsp_bdist_dice_u32_f32 (src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_hamming_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_jaccard_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_kulsinski_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_rogers_tanimoto_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_russell_rao_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_sokal_michener_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_sokal_sneath_u32_f32(src1, src2, numofbool);
 *      dist_out = riscv_dsp_bdist_yule_u32_f32(src1, src2, numofbool);
 * </pre>
 */
/**
 * @addtogroup bdist
 * @{
 */
/**
 * @brief        Dice distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_dice_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Hamming distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_hamming_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Jaccard distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_jaccard_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Kulsinski distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_kulsinski_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Sokal-Michener distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_sokal_michener_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Sokal-Sneath distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_sokal_sneath_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Roger Stanimoto distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_rogers_tanimoto_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Yule distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_yule_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Russell-Rao distance between two vectors
 * @param[in]    src1         pointer of the first input vector
 * @param[in]    src2         pointer of the second input vector
 * @param[in]    numofbool    number of booleans in a vector
 * @return Distance value
 */
float32_t riscv_dsp_bdist_russell_rao_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/** @} bdist */
/** @} dist */

#ifdef  __cplusplus
}
#endif
#endif
