/**
 * Copyright (C) 2021 Andes Technology Corporation
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __RISCV_DSP_DISTANCE_MATH_H__
#define __RISCV_DSP_DISTANCE_MATH_H__

/**
 * @defgroup dist Distance Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @brief        Bray-Curtis distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_bray_curtis_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Canberra distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_canberra_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Chebyshev distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_chebyshev_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Cityblock (Manhattan) distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_city_block_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Correlation distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_corr_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Cosine distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_cos_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Euclidean distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_euclidean_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Jensen-Shannon distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_jensen_shannon_f32(const float32_t *src1, const float32_t *src2, uint32_t size);

/**
 * @brief        Minkowski distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    order        Distance order
 * @param[in]    size         vector length
 * @return distance
 */
float32_t riscv_dsp_dist_minkowski_f32(const float32_t *src1, const float32_t *src2, int32_t order, uint32_t size);

/**
 * @brief        Dice distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_dice_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Hamming distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_hamming_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Jaccard distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_jaccard_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Kulsinski distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_kulsinski_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Sokal-Michener distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_sokal_michener_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Sokal-Sneath distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_sokal_sneath_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Roger Stanimoto distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_rogers_tanimoto_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Yule distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_yule_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

/**
 * @brief        Russell-Rao distance between two vectors
 * @param[in]    src1         First vector
 * @param[in]    src2         Second vector
 * @param[in]    numofbool    Number of booleans
 * @return distance
 */
float32_t riscv_dsp_bdist_russell_rao_u32_f32(const uint32_t *src1, const uint32_t *src2, uint32_t numofbool);

#ifdef  __cplusplus
}
#endif
#endif
