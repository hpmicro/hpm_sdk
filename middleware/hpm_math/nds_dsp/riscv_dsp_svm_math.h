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
#ifndef __RISCV_DSP_SVM_MATH_H__
#define __RISCV_DSP_SVM_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup svm Support Vector Machine (SVM) Estimation Functions
 * @brief SVM Estimation Functions
 *
 * This set of functions implements two-class SVM classification in accordance with the instance
 * structure you provide. Andes DSP library supports four distinct algorithms of SVM classification
 * for floating-point data, described in the following subsections.
 */
/**
 * @addtogroup svm
 * @{
 */
/**
 * @defgroup svm_linear SVM Linear Estimation
 * @brief SVM Linear Estimation
 */
/**
 * @addtogroup svm_linear
 * @{
 */
/**
 * @brief instance structure of the floating-point SVM linear estimator
 */
typedef struct
{
    uint32_t        numofvec;     /**< number of support vectors */
    uint32_t        dimofvec;     /**< dimension of vector space */
    float32_t       intercept;    /**< Intercept */
    const float32_t *dualcoe;     /**< dual coefficients */
    const float32_t *vec;         /**< support vectors */
    const int32_t   *classes;     /**< the 2 classes of SVM */
} riscv_dsp_svm_linear_f32_t;

/**
 * @brief SVM Linear Estimation
 * @param[in]    instance    pointer of the instance structure
 * @param[in]    src         pointer of the input vector
 * @param[out]   result      decision value
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define numofvec 3
 *      \#define dimofvec 4
 *      \#define intercept 0.1
 *      float32_t src[dimofvec] = {1.0, 0.5, 0.4, -0.1};
 *      float32_t dualcoe[numofvec] = {0.40, 0.10, 0.24};
 *      float32_t vec[dimofvec * numofvec] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06, 0.28, -0.04, -0.20, 0.08, 0.40};
 *      int32_t classes[2] = {3, 7};
 *      int32_t *result;
 *      riscv_dsp_svm_linear_f32_t instance = {numofvec, dimofvec, intercept, dualcoe, vec, classes};
 *      riscv_dsp_svm_linear_est_f32(&instance, src, result);
 *     </pre>
 */

void riscv_dsp_svm_linear_est_f32(const riscv_dsp_svm_linear_f32_t *instance, const float32_t *src, int32_t *result);
/** @} svm_linear */

/**
 * @defgroup svm_sigmoid SVM Sigmoid Estimation
 * @brief SVM Sigmoid Estimation
 */
/**
 * @addtogroup svm_sigmoid
 * @{
 */
/**
 * @brief instance structure of the floating-point SVM sigmoid estimator
 */
typedef struct
{
    uint32_t        numofvec;     /**< Number of support vectors */
    uint32_t        dimofvec;     /**< Dimension of vector space */
    float32_t       intercept;    /**< Intercept                 */
    const float32_t *dualcoe;     /**< Dual coefficients         */
    const float32_t *vec;         /**< Support vectors           */
    const int32_t   *classes;     /**< the 2 classes of SVM      */
    float32_t       coef0;        /**< Independent constant      */
    float32_t       gamma;        /**< Gamma factor              */
} riscv_dsp_svm_sigmoid_f32_t;

/**
 * @brief SVM Sigmoid Estimation
 * @param[in]    instance    pointer of the instance structure
 * @param[in]    src         pointer of the input vector
 * @param[out]   result      decision value
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define numofvec 3
 *      \#define dimofvec 4
 *      \#define intercept 0.1
 *      \#define gamma 0.2
 *      \#define coef0 0.3
 *
 *      float32_t src[dimofvec] = {1.0, 0.5, 0.4, -0.1};
 *      float32_t dualcoe[numofvec] = {0.40, 0.10, 0.24};
 *      float32_t vec[dimofvec * numofvec] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06, 0.28, -0.04, -0.20, 0.08, 0.40};
 *      int32_t classes[2] = {-1, 4};
 *      int32_t *result;
 *      riscv_dsp_svm_sigmoid_f32_t instance = {numofvec, dimofvec, intercept, dualcoe, vec, classes, coef0, gamma};
 *      riscv_dsp_svm_sigmoid_est_f32(&instance, src, result);
 *     </pre>
 */
void riscv_dsp_svm_sigmoid_est_f32(const riscv_dsp_svm_sigmoid_f32_t *instance, const float32_t *src, int32_t *result);
/** @} svm_sigmoid */

/**
 * @defgroup svm_rbf SVM Radial Basis Function (RBF) Estimation
 * @brief SVM RBF Estimation
 */
/**
 * @addtogroup svm_rbf
 * @{
 */
/**
 * @brief instance structure of the floating-point SVM RBF estimator
 */
typedef struct
{
    uint32_t        numofvec;     /**< Number of support vectors */
    uint32_t        dimofvec;     /**< Dimension of vector space */
    float32_t       intercept;    /**< Intercept                 */
    const float32_t *dualcoe;     /**< Dual coefficients         */
    const float32_t *vec;         /**< Support vectors           */
    const int32_t   *classes;     /**< The 2 SVM classes         */
    float32_t       gamma;        /**< Gamma factor              */
} riscv_dsp_svm_rbf_f32_t;

/**
 * @brief SVM rbf prediction
 * @param[in]    instance    pointer of the instance structure
 * @param[in]    src         pointer of the input vector
 * @param[out]   result      decision value
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define numofvec 3
 *      \#define dimofvec 4
 *      \#define intercept 0.1
 *      \#define gamma 0.2
 *
 *      float32_t src[dimofvec] = {1.0, 0.5, 0.4, -0.1};
 *      float32_t dualcoe[numofvec] = {0.40, 0.10, 0.24};
 *      float32_t vec[dimofvec * numofvec] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06, 0.28, -0.04, -0.20, 0.08, 0.40};
 *      int32_t classes[2] = {2, 5};
 *      int32_t *result;
 *      riscv_dsp_svm_rbf_f32_t instance = {numofvec, dimofvec, intercept, dualcoe, vec, classes, gamma};
 *      riscv_dsp_svm_rbf_est_f32(&instance, src, result);
 *     </pre>
 */

void riscv_dsp_svm_rbf_est_f32(const riscv_dsp_svm_rbf_f32_t *instance, const float32_t *src, int32_t *result);
/** @} svm_rbfc */

/**
 * @defgroup svm_poly SVM Polynomial Estimation
 * @brief SVM Polynomial Estimation
 */
/**
 * @addtogroup svm_poly
 * @{
 */

/**
 * @brief instance structure of the floating-point SVM polynomial estimator
 */
typedef struct
{
    uint32_t        numofvec;     /**< Number of support vectors */
    uint32_t        dimofvec;     /**< Dimension of vector space */
    float32_t       intercept;    /**< Intercept                 */
    const float32_t *dualcoe;     /**< Dual coefficients         */
    const float32_t *vec;         /**< Support vectors           */
    const int32_t   *classes;     /**< The 2 SVM classes         */
    int32_t         exponent;     /**< Polynomial exponent       */
    float32_t       coef0;        /**< Independent constant      */
    float32_t       gamma;        /**< Gamma factor              */
} riscv_dsp_svm_poly_f32_t;
/**
 * @brief SVM polynomial prediction
 * @param[in]    instance    pointer of the instance structure
 * @param[in]    src         pointer of the input vector
 * @param[out]   result      decision value
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define numofvec 3
 *      \#define dimofvec 4
 *      \#define intercept 0.1
 *      \#define gamma 0.2
 *      \#define exponent 3
 *      \#define coef0 0.3
 *
 *      float32_t src[dimofvec] = {1.0, 0.5, 0.4, -0.1};
 *      float32_t dualcoe[numofvec] = {0.40, 0.10, 0.24};
 *      float32_t vec[dimofvec * numofvec] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06, 0.28, -0.04, -0.20, 0.08, 0.40};
 *      int32_t classes[2] = {-1, 4};
 *      int32_t *result;
 *      riscv_dsp_svm_poly_f32_t instance = {numofvec, dimofvec, intercept, dualcoe, vec, classes, exponent, coef0, gamma};
 *      riscv_dsp_svm_poly_est_f32(&instance, src, result);
 *     </pre>
 */

void riscv_dsp_svm_poly_est_f32(const riscv_dsp_svm_poly_f32_t *instance, const float32_t *src, int32_t *result);
/** @} svm_poly */

/** @} svm */

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_BASIC_MATH_H__
