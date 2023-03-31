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
#ifndef __RISCV_DSP_COMPLEX_MATH_H__
#define __RISCV_DSP_COMPLEX_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup groupclx Complex Functions
 *
 * @brief Complex Functions
 *
 * Complex functions provide calculations for complex vectors. In Andes DSP library, the elements
 * of complex vectors should be arranged as real and imaginary parts interleaved. That is, the
 * memory layout should look like [real, imaginary, real, imaginary, ... , real,
 * imaginary].
 */
/**
 * @addtogroup groupclx
 * @{
 */

// Complex Conjugate
/**
 * @defgroup cconj Complex Conjugate Functions
 * @brief Complex Conjugate Functions
 *
 * Complex conjugate functions calculate conjugation values of complex numbers from a source
 * vector and write the results into a destination vector. The behavior can be defined as follows:
 *
 * @image html cconj.gif ""
 *
 * Andes DSP library supports distinct complex conjugate functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup cconj
 * @{
 */

/**
 * @brief Conjugate the floating-potint complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output complex vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Example
 *     <pre>
 * With a set of complex vector containing three complex elements like {3 + 4i, 4 - 7i, -3
 * + 5i}, the complex conjugation is as follows:
 *
 *      //Complex vector is arranged with real and imaginary parts interleaved.
 *      \#define size 3
 *      float32_t src[2 * size] = {3.0, 4.0, 4.0, -7.0, -3.0, 5.0};
 *      float32_t dst[2 * size];
 *      riscv_dsp_cconj_f32(src, dst, size);
 *
 * This example also serves as a reference for examples of Q31 or Q15 complex conjugate
 * functions.
 *     </pre>
 */
void riscv_dsp_cconj_f32(const float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Conjugate the q15 complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output complex vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The real parts and imaginary parts in complex vectors are both in the Q15 range.
 * 2. When negating the input imaginary part where the value is INT16_MIN (0x8000), the
 * output imaginary part will be INT16_MAX (0x7FFF) after complex conjugation.
 */
void riscv_dsp_cconj_q15(const q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Conjugate the q31 complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output complex vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * 1. The real parts and imaginary parts in complex vectors are both in the Q31 range.
 * 2. When negating the input imaginary part where the value is INT32_MIN (0x80000000),
 * the output imaginary part will be INT32_MAX (0x7FFFFFFF) after complex conjugation.
 * The information
 */
void riscv_dsp_cconj_q31(const q31_t *src, q31_t *dst, uint32_t size);
/** @} cconj */

// Complex Dot Product
/**
 * @defgroup cdprod Complex Dot Product Functions
 * @brief Complex Dot Product Functions
 *
 * Complex dot product functions calculate the dot product of two complex vectors and write the
 * results into a destination vector. There are two kinds of complex dot product functions in Andes
 * DSP library.
 *
 * The first type of complex dot product functions includes riscv_dsp_cdprod_f32,
 * riscv_dsp_cdprod_q31 and riscv_dsp_cdprod_q15. Their behavior can be described as follows:
 *
 * @image html cdprod.gif ""
 *
 * The second type of complex dot product functions, named as complex dot product type 2
 * functions, includes riscv_dsp_cdprod_typ2_f32, riscv_dsp_cdprod_typ2_q31 and
 * riscv_dsp_cdprod_typ2_q15. Their behavior can be described as follows:
 *
 * @image html cdprod_typ2.gif ""
 *
 * Details of each complex dot product functions are introduced in the subsections below.
 */
/**
 * @addtogroup cdprod
 * @{
 */
/**
 * @brief Compute the dot product of the floating-potint complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[in]		size  number of complex elements in a vector
 * @param[out]		*dst  pointer of the output complex vector
 * @return none.
 *
 * @b Example
 *     <pre>
 * With two sets of complex vector each containing three complex numbers like {3 + 4i, 4 -
 * 7i, -3 + 5i} and {1 - 2i, 5 - 1i, -4 + 3i}, the complex dot production is as follows:
 *
 *      \#define size 3
 *      float32_t src1[2*size] = {3, 4, 4, -7, -3, 5};
 *      float32_t src2[2*size] = {1, -2, 5, -1, -4, 3};
 *      float32_t dst[2*size];
 *      riscv_dsp_cdprod_f32(src1, src2, size, dst);
 *
 * This example also serves as a reference for examples of Q31 or Q15 complex dot product
 * functions.
 *     </pre>
 */
void riscv_dsp_cdprod_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *dst);

/**
 * @brief Compute the dot product of the q15 complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[in]		size  number of complex elements in a vector
 * @param[out]		*dst  pointer of the output complex vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q13 format.
 */
void riscv_dsp_cdprod_q15(const q15_t *src1, const q15_t *src2, uint32_t size, q15_t *dst);

/**
 * @brief Compute the dot product of the q31 complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[in]		size  number of complex elements in a vector
 * @param[out]		*dst  pointer of the output complex vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q29 format.
 */
void riscv_dsp_cdprod_q31(const q31_t *src1, const q31_t *src2, uint32_t size, q31_t *dst);

/**
 * @brief Compute the dot product type2 of the floating-potint complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[in]		size  number of complex elements in a vector
 * @param[out]		*rout pointer of the real output
 * @param[out]		*iout pointer of the image output
 * @return none.
 *
 * @b Example
 *     <pre>
 * With two sets of complex vector each containing three complex numbers like {3 + 4i, 4 -
 * 7i, -3 + 5i} and {1 - 2i, 5 - 1i, -4 + 3i}, the complex dot production is as follows:
 *
 *      \#define size 3
 *      float32_t src1[2*size] = {3, 4, 4, -7, -3, 5};
 *      float32_t src2[2*size] = {1, -2, 5, -1, -4, 3};
 *      float32_t real_out, image_out;
 *      riscv_dsp_cdprod_typ2_f32(src1, src2, size, &real_out, &image_out);
 *
 * This example also serves as a reference for examples of Q31 or Q15 complex dot product
 * type 2 functions.
 *     </pre>
 */
void riscv_dsp_cdprod_typ2_f32(const float32_t *src1, const float32_t *src2, uint32_t size, float32_t *rout, float32_t *iout);

/**
 * @brief Compute the dot product type2 of the q15 complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[in]		size  number of complex elements in a vector
 * @param[out]		*rout pointer of the real output
 * @param[out]		*iout pointer of the image output
 * @return none.
 *
 * @b Note:
 *
 * The values written into the two output variables are in Q24 format.
 */
void riscv_dsp_cdprod_typ2_q15(const q15_t *src1, const q15_t *src2, uint32_t size, q31_t *rout, q31_t *iout);

/**
 * @brief Compute the dot product type2 of the q31 complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[in]		size  number of complex elements in a vector
 * @param[out]		*rout pointer of the real output
 * @param[out]		*iout pointer of the image output
 * @return none.
 *
 * @b Note:
 *
 * The values written into the two output variables are in Q48 format.
 */
void riscv_dsp_cdprod_typ2_q31(const q31_t *src1, const q31_t *src2, uint32_t size, q63_t *rout, q63_t *iout);
/** @} cdprod */

// Complex Magnitude
/**
 * @defgroup cmag Complex Magnitude Functions
 * @brief Complex Magnitude Functions
 *
 * Complex magnitude functions compute the magnitude of a complex vector. The behavior can be
 * defined as follows:
 *
 * @image html cmag.gif ""
 *
 * Andes DSP library supports distinct complex magnitude functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup cmag
 * @{
 */
/**
 * @brief Compute the magnitude of the floating-potint complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 */
void riscv_dsp_cmag_f32(const float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Compute the magnitude of the q15 complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q13 format.
 */
void riscv_dsp_cmag_q15(const q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Compute the magnitude of the q31 complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q29 format.
 */
void riscv_dsp_cmag_q31(const q31_t *src, q31_t *dst, uint32_t size);
/** @} cmag */

// Complex Magnitude Squared
/**
 * @defgroup cmag_sqr Complex Magnitude-Squared Function
 * @brief Complex Magnitude-Squared Function
 *
 * Complex magnitude-squared functions compute the magnitude squared of complex numbers
 * from a complex vector. The behavior can be defined as follows:
 *
 * @image html cmag_sqr.gif ""
 *
 * Andes DSP library supports distinct complex magnitude-squared functions for the following
 * data types: floating-point, Q31 and Q15. These functions are introduced in the subsections
 * below.
 */
/**
 * @addtogroup cmag_sqr
 * @{
 */
/**
 * @brief Compute the magnitude squared of the floating-potint complex
 *        vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 */
void riscv_dsp_cmag_sqr_f32(const float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Compute the magnitude squared of the q15 complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q13 format.
 */
void riscv_dsp_cmag_sqr_q15(const q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Compute the magnitude squared of the q31 complex vector.
 * @param[in]		*src pointer of the input complex vector
 * @param[out]		*dst pointer of the output vector
 * @param[in]		size number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q29 format.
 */
void riscv_dsp_cmag_sqr_q31(const q31_t *src, q31_t *dst, uint32_t size);
/** @} cmag_sqr */

    // Complex Multiplication
    /**
 * @defgroup cmul Complex-Multiply-Complex Functions
 * @brief Complex-Multiply-Complex Functions
 *
 * Complex-multiply-complex functions compute the multiplication of two complex vectors and
 * write the results into a destination vector. The behavior can be defined as follows:
 *
 * @image html cmul.gif ""
 *
 * Andes DSP library supports distinct complex-multiply-complex functions for the following data
 * types: floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup cmul
 * @{
 */
/**
 * @brief Multiply two folating-point complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[out]		*dst  pointer of the output complex vector
 * @param[in]		size  number of complex elements in a vector
 * @return none.
 *
 * @b Example
 *     <pre>
 * With two sets of complex vector each containing three complex numbers like [3 + 4i, 4 -
 * 7i, -3 + 5i] and [1 - 2i, 5 - 1i, -4 + 3i], the multiplication of the two complex
 * vectors is as follows:
 *      \#define size 3
 *      float32_t src1[2*size] = {3, 4, 4, -7, -3, 5};
 *      float32_t src2[2*size] = {1, -2, 5, -1, -4, 3};
 *      float32_t dst[2*size];
 *      riscv_dsp_cmul_f32(src1, src2, dst, size);
 * This example also serves as a reference for examples of Q31 or Q15 complex-multiply-complex functions.
 *     </pre>
 */
void riscv_dsp_cmul_f32(const float32_t *src1, const float32_t *src2, float32_t *dst, uint32_t size);

/**
 * @brief Multiply two q15 complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[out]		*dst  pointer of the output complex vector
 * @param[in]		size  number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q13 format.
 */
void riscv_dsp_cmul_q15(const q15_t *src1, const q15_t *src2, q15_t *dst, uint32_t size);

/**
 * @brief Multiply two q31 complex vector.
 * @param[in]		*src1 pointer of the first input complex vector
 * @param[in]		*src2 pointer of the second input complex vector
 * @param[out]		*dst  pointer of the output complex vector
 * @param[in]		size  number of complex elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The values written into the destination vector is in Q29 format.
 */
void riscv_dsp_cmul_q31(const q31_t *src1, const q31_t *src2, q31_t *dst, uint32_t size);
/** @} cmul */

// Complex-by-Real Multiplication
/**
 * @defgroup cmul_real Complex-Multiply-Real Functions
 * @brief Complex-Multiply-Real Functions
 *
 * Complex-multiply-real functions compute the multiplication of a complex vector by a real vector
 * and write the results into a destination complex vector. The behavior can be defined as follows:
 *
 * @image html cmul_real.gif ""
 *
 * Andes DSP library supports distinct complex-multiply-real functions for the following data types:
 * floating-point, Q31 and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup cmul_real
 * @{
 */
/**
 * @brief Multiply the folating-point complex vector by a real vector.
 * @param[in]		*src  pointer of the input complex vector
 * @param[in]		*real pointer of the input real vector
 * @param[out]		*dst  pointer of the output complex vector
 * @param[in]		size  number of elements in a vector
 * @return none.
 *
 * @b Example
 *     <pre>
 * With a complex vector containing three elements like [3 + 4i, 4 - 7i, -3 + 5i] and a
 * real vector containing three elements like [1, -3, 2], the multiplication of the two vectors
 * is as follows:
 *      \#define size 3
 *      float32_t src[2*size] = {3, 4, 4, -7, -3, 5};
 *      float32_t real[size] = {1, -3, 2};
 *      float32_t dst[2*size];
 *      riscv_dsp_cmul_real_f32(src, real, dst, size);
 * This example also serves as a reference for examples of Q31 or Q15 complex-multiply-real
 * functions.
 *     </pre>
 */
void riscv_dsp_cmul_real_f32(const float32_t *src, const float32_t *real, float32_t *dst, uint32_t size);

/**
 * @brief Multiply the q15 complex vector by a real vector.
 * @param[in]		*src  pointer of the input complex vector
 * @param[in]		*real pointer of the input real vector
 * @param[out]		*dst  pointer of the output complex vector
 * @param[in]		size  number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_cmul_real_q15(const q15_t *src, const q15_t *real, q15_t *dst, uint32_t size);

/**
 * @brief Multiply the q31 complex vector by a real vector.
 * @param[in]		*src  pointer of the input complex vector
 * @param[in]		*real pointer of the input real vector
 * @param[out]		*dst  pointer of the output complex vector
 * @param[in]		size  number of elements in a vector
 * @return none.
 *
 * @b Note:
 *
 * The results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_cmul_real_q31(const q31_t *src, const q31_t *real, q31_t *dst, uint32_t size);
/** @} cmul_real */

/** @} groupclx */

#ifdef  __cplusplus
}
#endif

#endif // __RISCV_DSP32_COMPLEX_MATH_H__
