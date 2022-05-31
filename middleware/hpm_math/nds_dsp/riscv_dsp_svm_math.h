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
#ifndef __RISCV_DSP_SVM_MATH_H__
#define __RISCV_DSP_SVM_MATH_H__

/**
 * @defgroup svm SVM Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"
typedef struct
{
    uint32_t        numofvec;     // Number of support vectors
    uint32_t        dimofvec;     // Dimension of vector space
    float32_t       intercept;    // Intercept
    const float32_t *dualcoe;     // Dual coefficients
    const float32_t *vec;         // Support vectors
    const int32_t   *class;       // The two SVM classes
} riscv_dsp_svm_linear_f32_t;

/**
 * @brief SVM linear prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

void riscv_dsp_svm_linear_est_f32(const riscv_dsp_svm_linear_f32_t *instance, const float32_t *src, int32_t *result);

typedef struct
{
    uint32_t        numofvec;     // Number of support vectors
    uint32_t        dimofvec;     // Dimension of vector space
    float32_t       intercept;    // Intercept
    const float32_t *dualcoe;     // Dual coefficients
    const float32_t *vec;         // Support vectors
    const int32_t   *class;       // The two SVM classes
    float32_t       coef0;        // Independant constant
    float32_t       gamma;        //  Gamma factor */
} riscv_dsp_svm_sigmoid_f32_t;
/**
 * @brief SVM Sigmoid prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

void riscv_dsp_svm_sigmoid_est_f32(const riscv_dsp_svm_sigmoid_f32_t *instance, const float32_t *src, int32_t *result);

typedef struct
{
    uint32_t        numofvec;     // Number of support vectors
    uint32_t        dimofvec;     // Dimension of vector space
    float32_t       intercept;    // Intercept
    const float32_t *dualcoe;     // Dual coefficients
    const float32_t *vec;         // Support vectors
    const int32_t   *class;       // The two SVM classes
    float32_t       gamma;        //  Gamma factor */
} riscv_dsp_svm_rbf_f32_t;
/**
 * @brief SVM rbf prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

void riscv_dsp_svm_rbf_est_f32(const riscv_dsp_svm_rbf_f32_t *instance, const float32_t *src, int32_t *result);


typedef struct
{
    uint32_t        numofvec;     // Number of support vectors
    uint32_t        dimofvec;     // Dimension of vector space
    float32_t       intercept;    // Intercept
    const float32_t *dualcoe;     // Dual coefficients
    const float32_t *vec;         // Support vectors
    const int32_t   *class;       // The two SVM classes
    int32_t         exponent;     // Polynomial exponent
    float32_t       coef0;        // Independant constant
    float32_t       gamma;        //  Gamma factor */
} riscv_dsp_svm_poly_f32_t;
/**
 * @brief SVM polynomial prediction
 * @param[in]    S          Pointer to an instance of the linear SVM structure.
 * @param[in]    src         Pointer to input vector
 * @param[out]   result    Decision value
 * @return none.
 */

void riscv_dsp_svm_poly_est_f32(const riscv_dsp_svm_poly_f32_t *instance, const float32_t *src, int32_t *result);


#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_BASIC_MATH_H__
