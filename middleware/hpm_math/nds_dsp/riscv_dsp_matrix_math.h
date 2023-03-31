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
#ifndef __RISCV_DSP_MATRIX_MATH_H__
#define __RISCV_DSP_MATRIX_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

/**
 * @defgroup mat Matrix Functions
 * @brief Matrix Functions
 *
 * A matrix can be seen as a rectangular array with m rows and n columns and of m * n elements.
 * In Andes DSP library, for programming convenience, matrices are stored into vectors separately
 * in row-major order. That is, the size of a vector equals to the number of a matrix (size = m * n).
 */
/**
 * @addtogroup mat
 * @{
 */

// Matrix Addition
/**
 * @defgroup mat_add Matrix Addition Functions
 * @brief Matrix Addition Functions
 *
 * Matrix addition functions add two elements from two source vectors and write the results
 * one-by-one into a destination vector. The behavior can be defined as follows:
 *
 * @image html mat_add.gif ""
 *
 * Andes DSP library supports distinct matrix addition functions for the following data types:
 * floating-point, Q31, and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup mat_add
 * @{
 */
/**
 * @brief Addition of two single-precision floating-potint matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 *
 * @b Example
 *     <pre>
 * The following equation shows the addition of two matrices and the result.
 *
 * @image html mat_add_2.gif ""
 *
 * The code example with this function is as follows:
 *
 *      \#define row	2
 *      \#define col	2
 *      float32_t src1[row * col] = {0.1, 0.4, -0.2, 0.1};
 *      float32_t src2[row * col] = {-0.2, -0.1, 0.3, 0.5};
 *      float32_t dst[row * col];
 *      riscv_dsp_mat_add_f32(src1, src2, dst, row, col);
 *
 * This example also serves as a reference for examples of Q31 or Q15 matrix addition
 * functions.
 *     </pre>
 */
void riscv_dsp_mat_add_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Addition of two double-precision floating-potint matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_add_f64(const float64_t *src1, const float64_t *src2,
                       float64_t *dst, uint32_t row, uint32_t col);
/**
 * @brief Addition of two q15 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 *
 * @b Note
 *
 * The results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_mat_add_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Addition of two q31 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 *
 * @b Note
 *
 * The results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_mat_add_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col);

/** @} */

// Matrix Inverse
/**
 * @defgroup mat_inv Matrix Inverse Function
 * @brief Matrix Inverse Function
 *
 * The matrix inverse function returns the inverse of a square matrix. The square matrix has the
 * same dimension in the horizontal and vertical direction. Therefore, the matrix inverse function
 * only accepts one parameter (i.e. size) to specify the number of row and that of column. It not
 * only avoids receiving two different numbers for row and column but also slightly improves
 * performance.
 *
 * The matrix inverse function implements Gauss-Jordan elimination to find the inverse of a
 * matrix. First, the output matrix is filled with 0 and 1 to form an identity matrix. Next, elementary
 * row operations are performed on the input and output matrices simultaneously to make the
 * input matrix an identity matrix. If the original input matrix is invertible, the output matrix will
 * eventually be its inverse and 0 will be returned. The behavior can be defined as follows:
 *
 * @image html mat_inv.gif ""
 *
 * Where A is the input matrix and A(inv) is its inverse matrix. I is an identity matrix generated in the
 * output matrix before elementary row operations and found in the input matrix after the
 * operations.
 *
 * If the pivot value is zero during elementary row operations, the function needs to look for a
 * non-zero pivot from following rows. Once it is found, swap the two rows and perform elementary
 * row operations again until the input matrix is transformed into an identity matrix. If a non-zero
 * pivot is not available, return -1, indicating the input matrix is singular.
 *
 * Andes DSP library supports distinct matrix inverse functions for the following data types: singleand
 * double-precision floating-point .
 */
/**
 * @addtogroup mat_inv
 * @{
 */

/**
 * @brief Compute the inverse matrix of the floating-potint matrix.
 * @param[in]       *src pointer of the input matrix
 * @param[out]      *dst  pointer of the output matrix
 * @param[in]       size  number of rows/columns in a matrix (i.e. size = rows = columns)
 * @return (0 success; -1 failure)
 *
 * @b Note
 *
 * 1. If the return value is 0, the input matrix is invertible and transformed into an identity
 * matrix; if the return value is -1, the input matrix is singular.
 *
 * 2. No matter what value is returned, the content of the input and of the output matrix
 * will both be changed after execution of the matrix inverse function.
 *
 * @b Example
 *     <pre>
 * A square matrix (A) and its inverse are listed below:
 *
 * @image html mat_inv_2.gif ""
 *
 * The code example for getting the inverse of matrix A is as follows:
 *      define size 2
 *      float32_t src[size * size] = {0.1, 0.4, -0.2, -0.3};
 *      float32_t dst[size * size];
 *      if (riscv_dsp_mat_inv_f32(src, dst, size) == 0)
 *          Success…
 *      else
 *          Fail…
 *
 * This example also serves as a reference for examples of the F64 matrix inverse function.
 *     </pre>
 */
int32_t riscv_dsp_mat_inv_f32(float32_t *src, float32_t *dst, uint32_t size);
/**
 * @brief Compute the inverse matrix of the floating-potint matrix.
 * @param[in]       *src pointer of the input matrix
 * @param[out]      *dst  pointer of the output matrix
 * @param[in]       size  number of rows/columns in a matrix (i.e. size = rows = columns)
 * @return (0 success; -1 failure)
 */
int32_t riscv_dsp_mat_inv_f64(float64_t *src, float64_t *dst, uint32_t size);
/** @} mat_inv */

// Matrix Multiplication
/**
 * @defgroup mat_mul Matrix Multiplication Functions
 * @brief Matrix Multiplication Functions
 *
 * Matrix multiplication functions compute the multiplication of two source matrices and write the
 * result into a destination matrix. Figure 4 shows the multiplication of matrix A by matrix B, in
 * which both matrices have 2 rows and 2 columns.
 *
 * @image html mat_mul.gif "Figure 4. Matrix A Multiply Matrix B"
 *
 * According to the definition of matrix multiplication, the number of rows in matrix B must be
 * identical with the number of columns in matrix A. That is,
 *
 * @image html mat_mul_2.gif ""
 *
 * where A is a matrix with i rows and j columns, B is a matrix with j rows and k columns, and C is
 * the destination matrix with i rows and k columns.
 *
 * Andes DSP library supports distinct matrix multiplication functions for the following data types:
 * single- and double-precision floating-point, Q31, Q15 and Q7. These functions are introduced in
 * the subsections below.
 */
/**
 * @addtogroup mat_mul
 * @{
 */
/**
 * @brief Multiplication of two floating-point matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 *
 * @b Example
 *     <pre>
 * The following equation shows the multiplication of two matrices:
 *
 * @image html mat_mul_3.gif ""
 *
 * Given each element with a value, the result is like
 *
 * @image html mat_mul_4.gif ""
 *
 * and can be presented in code as follows:
 *
 *      \#define Arow 2
 *      \#define Acol 3
 *      \#define Bcol 2
 *      float32_t src1[Arow * Acol] = {0.1, -0.1, 0.1, 0.2, -0.2, 0.3};
 *      float32_t src2[Acol * Bcol] = {0.2, 0.2, -0.1, 0.3, -0.7, -0.2};
 *      float32_t dst[Arow * Bcol];
 *      riscv_dsp_mat_mul_f32 (src1, src2, dst, Arow, Acol, Bcol);
 *
 * This example also serves as a reference for examples of F64, Q31 or Q15 matrix
 * multiplication functions.
 *     </pre>
 */
void riscv_dsp_mat_mul_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col, uint32_t col2);

/**
 * @brief Multiplication of two f64 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 */
void riscv_dsp_mat_mul_f64(const float64_t *src1, const float64_t *src2,
                       float64_t *dst, uint32_t row, uint32_t col, uint32_t col2);

/**
 * @brief Multiplication of two q15 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 *
 * @b Note
 *
 * For RV64, this function will allocate a temporary buffer of 256 bytes for efficient
 * calculation.
 */
void riscv_dsp_mat_mul_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col, uint32_t col2);
/**
 * @brief Multiplication of two q15 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 *
 * @b Note
 *
 * 1. For RV64, this function will allocate a temporary buffer of 128 bytes for efficient
 * calculation.
 *
 * 2. When compared with riscv_dsp_mat_mul_q15, this function delivers higher
 * performance with lower precision.
 */
void riscv_dsp_mat_mul_fast_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col, uint32_t col2);

/**
 * @brief Multiplication of two q31 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 */
void riscv_dsp_mat_mul_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col, uint32_t col2);
/**
 * @brief Multiplication of two q15 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 *
 * @b Note
 *
 * 1. A 32-bit accumulator is used to do multiply-accumulate operations and truncated to
 * Q2.30 result. To avoid overflows that distort the result, the input matrix must be scaled
 * down by log2(row) bits. After all operations are performed, the results are saturated
 * and converted to Q1.31 format.
 *
 * 2. When compared with riscv_dsp_mat_mul_q31, this function delivers higher
 * performance with lower precision.
 */
void riscv_dsp_mat_mul_fast_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col, uint32_t col2);
/**
 * @brief Multiplication of two q7 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in the first input matrix
 * @param[in] col number of columns in the first input matrix
 * @param[in] col2 number of columns in the second input matrix
 * @return none.
 *
 * @b Note
 *
 * For RV64, this function will allocate a temporary buffer of 512 bytes for efficient calculation;
 * for RV32, a temporary buffer of 128 bytes will be allocated.
 */
void riscv_dsp_mat_mul_q7(const q7_t *src1, const q7_t *src2,
                       q7_t *dst, uint32_t row, uint32_t col, uint32_t col2);

/**
 * @brief Multiplication of q7 vetor by matrix.
 * @param[in] src1 pointer of an input vector
 * @param[in] src2 pointer of an input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] col number of columns in the input vector
 * @param[in] col2 number of columns in the input matrix
 * @return none.
 *
 * @b Note
 *
 * This function multiplies a vector with col columns, src1[1, col], with a matrix with col
 * rows and col2 columns, src2[col, col2], and stores the result into a vector with col2
 * columns, dst[1, col2].
 */
void riscv_dsp_mat_mul_vxm_q7(const q7_t * src1, const q7_t * src2, q7_t * dst,
                          uint32_t col, uint32_t col2);
/** @} mat_mul */

// Matrix Power 2 Function
/**
 * @defgroup mat_pwr2 Matrix Power 2 Function
 * @brief Matrix Power 2 Function
 *
 * The matrix power 2 function powers a square matrix with 2 and then writes the result to a
 * destination matrix. The behavior can be defined as follows:
 *  <pre>
 *       dst = src^2, where src is a square matrix
 *  </pre>
 *
 * Andes DSP library only supports the matrix power 2 function for double precision floating-point
 * data.
 */
/**
 * @addtogroup mat_pwr2
 * @{
 */
/**
 * @param[in] src pointer of the input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] size number of rows or columns in a matrix
 * @return (0 success; -1 failure)
 *
 * @b Note:
 *
 * The input matrix must be a square matrix with the number of rows or columns equal to
 * size. In addition, size must be a multiple of four (such as 28, 32, 64, and 1024).
 * Otherwise, a return value of -1 is given for the error.
 *
 * This function performs a block matrix multiplication. The block size is the size of one cache
 * line for utilizing each cache line fetched from the memory and reducing cache misses.
 *
 * When size is smaller than 40, you are advised to use riscv_dsp_mat_mul_f64 for better
 * performance.
 */
int32_t riscv_dsp_mat_pwr2_cache_f64(const float64_t *src, float64_t *dst, uint32_t size);
/** @} mat_pwr2 */

// Matrix Scale
/**
 * @defgroup mat_scale Matrix Scale Functions
 * @brief Matrix Scale Functions
 *
 * Matrix scale functions multiply a matrix by a constant scaling value and write the result into a
 * destination matrix. Since vectors are used as containers of matrices in Andes DSP library, the
 * behavior of matrix scale functions can be defined as follows:
 *
 * @image html mat_scale.gif ""
 *
 * In cases of fractional presentation, a parameter shift is introduced to adjust the range of result
 * values. For details about the parameter, please refer to functions with fractional data types
 *
 * Andes DSP library supports distinct matrix scale functions for the following data types:
 * floating-point, Q31, and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup mat_scale
 * @{
 */
/**
 * @brief Multiplt a scale value of floating-potint matrix.
 * @param[in]       *src pointer of the input matrix
 * @param[in]       scale constant scaling value
 * @param[out]      *dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 *
 * @b Example
 *     <pre>
 * Given a matrix and a scale value 0.2, the matrix scale equation is as follows:
 *
 * @image html mat_scale_2.gif ""
 *
 * and its code example is like:
 *
 *      \#define row 2
 *      \#define col 2
 *      float32_t src[row * col] = {0.1, 0.4, -0.2, 0.1};
 *      flato32_t scale = 0.2;
 *      float32_t dst[row * col];
 *      riscv_dsp_mat_scale_f32 (src1, scale, dst, row, col);
 *
 * This example also serves as a reference for examples of Q31 or Q15 matrix scale functions.
 *     </pre>
 */
void riscv_dsp_mat_scale_f32(const float32_t *src, float32_t scale,
                         float32_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Multiplt a scale value of q15 matrix.
 * @param[in]       *src      pointer of the input matrix
 * @param[in]       scale_fract constant fractional scaling value
 * @param[in]       shift      shift bits
 * @param[out]      *dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_scale_q15(const q15_t *src, q15_t scale_fract, int32_t shift,
                         q15_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Multiplt a scale value of q31 matrix.
 * @param[in]       *src      pointer of the input matrix
 * @param[in]       scale_fract constant fractional scaling value
 * @param[in]       shift      shift bits
 * @param[out]      *dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_scale_q31(const q31_t *src, q31_t scale_fract, int32_t shift,
                         q31_t *dst, uint32_t row, uint32_t col);
/** @} mat_scale */

// Matrix Subtraction
/**
 * @defgroup mat_sub Matrix Subtraction Functions
 * @brief Matrix Subtraction Functions
 *
 * Matrix subtraction functions subtract two matrices which have same dimensions and write the
 * result into a destination matrix. The behavior can be defined as follows:
 *
 * @image html mat_sub.gif ""
 *
 * Andes DSP library supports distinct matrix subtraction functions for the following data types:
 * floating-point, Q31, and Q15. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup mat_sub
 * @{
 */
/**
 * @brief Substraction of two single-precision floating-potint matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 *
 * @b Example
 *     <pre>
 * Given two matrices and their subtraction like below,
 *
 * @image html mat_sub_2.gif ""
 *
 * the code example of matrix subtraction is as follows:
 *
 *      \#define row 2
 *      \#define col 2
 *      float32_t src1[row * col] = {0.1, 0.4, -0.2, 0.1};
 *      float32_t src2[row * col] = {-0.2, -0.1, 0.3, 0.5};
 *      float32_t dst[row * col];
 *      riscv_dsp_mat_sub_f32 (src1, src2, dst, row, col);
 *
 * This example also serves as a reference for examples of Q31 or Q15 matrix subtraction
 * functions.
 *     </pre>
 */
void riscv_dsp_mat_sub_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Substraction of two double-precision floating-potint matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_sub_f64(const float64_t *src1, const float64_t *src2,
                       float64_t *dst, uint32_t row, uint32_t col);
/**
 * @brief Substraction of two q15 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 *
 * @b Note:
 *
 * The results will be saturated to the Q15 range [0x8000, 0x7FFF].
 */
void riscv_dsp_mat_sub_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Substraction of two q31 matrices.
 * @param[in] src1 pointer of the first input matrix
 * @param[in] src2 pointer of the second input matrix
 * @param[out] dst pointer of the output matrix
 * @param[in] row number of rows in a matrix
 * @param[in] col number of columns in a matrix
 * @return none.
 *
 * @b Note:
 *
 * The results will be saturated to the Q31 range [0x80000000, 0x7FFFFFFF].
 */
void riscv_dsp_mat_sub_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col);
/** @} mat_sub */

// Matrix Transpose
/**
 * @defgroup mat_trans Matrix Transpose Functions
 * @brief Matrix Transpose Functions
 *
 * Matrix transpose functions transpose a matrix then write the result into a destination matrix.
 * The behavior can be defined as follows:
 *
 * @image html mat_trans.gif ""
 *
 * Andes DSP library supports distinct matrix transpose functions for floating-point, Q31, Q15 and
 * other data types. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup mat_trans
 * @{
 */
/**
 * @brief Transpose the single-precision floating-potint matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 *
 * @b Example
 *  <pre>
 * Given an equation of matrix transposing like below,
 *
 * @image html mat_trans_2.gif ""
 *
 * its code example is as follows:
 *
 *      \#define row 2
 *      \#define col 3
 *      float32_t src[row * col] = {0.1, -0.1, 0.1, 0.2, -0.2, 0.3};
 *      float32_t dst[col * row];
 *      riscv_dsp_mat_trans_f32 (src, dst, row, col);
 *
 * This example also serves as a reference for examples of Q31 or Q15 matrix transpose
 * functions.
 *  </pre>
 */
void riscv_dsp_mat_trans_f32(const float32_t *src, float32_t *dst, uint32_t row,
                         uint32_t col);

/**
 * @brief Transpose the double-precision floating-potint matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_trans_f64(const float64_t *src, float64_t *dst, uint32_t row,
                         uint32_t col);
/**
 * @brief Transpose the q15 matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_trans_q15(const q15_t *src, q15_t *dst, uint32_t row,
                         uint32_t col);

/**
 * @brief Transpose the q31 matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_trans_q31(const q31_t *src, q31_t *dst, uint32_t row,
                         uint32_t col);

/**
 * @brief Transpose the u8 matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_trans_u8(const uint8_t *src, uint8_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Transpose the q7 matrices.
 * @param[in]       src  pointer of the input matrix
 * @param[out]      dst  pointer of the output matrix
 * @param[in]       row   number of rows in a matrix
 * @param[in]       col   number of columns in a matrix
 * @return none.
 */
void riscv_dsp_mat_trans_q7(const q7_t *src, q7_t *dst, uint32_t row, uint32_t col);
/** @} mat_trans */

/**
 * @defgroup mat_oprod Matrix Outer Product Function
 * @brief Matrix Outer Product Function
 *
 * The matrix outer product function computes the multiplication of two source vectors and writes
 * the result into a destination matrix. Figure 5 shows the multiplication of vector A by vector B, in
 * which vector A has 3 rows and vector B has 4 columns.
 *
 * @image html mat_oprod.gif "Figure 5. Vector A Multiply Vector B"
 *
 * The outer product can be treated as a matrix multiplication in which source matrices Aij and Bjk
 * have a constraint that the value of j must be 1, as shown below.
 *
 * @image html mat_oprod_2.gif ""
 *
 * Andes DSP library only supports the matrix outer product function for Q31 data.
 */
/**
 * @addtogroup mat_oprod
 * @{
 */

/**
 * @brief Outer production of two q31 matrices.
 * @param[in]       src1 pointer of the first input matrix with a size of size1*1
 * @param[in]       src2 pointer of the second input matrix with a size of 1*size2
 * @param[out]      dst  pointer of the output matrix with a size of size1 * size2
 * @param[in]       size1 number of rows in the first input matrix.
 * @param[in]       size2 number of columns in the second input matrix.
 * @return none.
 *
 * @b Note:
 *
 * This function multiplies a one-column matrix with size1 rows, src1[size1, 1], with a
 * one-row matrix with size2 columns, src2[1, size2], and stores the result into a matrix
 * with size1 rows and size2 columns, dst[size1, size2]. It achieves better efficiency for
 * vector-wise matrix multiplication than for regular matrix multiplication.
 *
 * @b Example
 *     <pre>
 * The following equation shows the outer product of two matrices and its result.
 *
 * @image html mat_oprod_3.gif ""
 *
 * Its code example is as follows:
 *
 *      \#define Arow 3
 *      \#define Bcol 2
 *      q31_t src1[Arow] = {0x200000, 0x100000, 0x50000};
 *      q31_t src2[Bcol] = {0x10000, 0x30000};
 *      q31_t dst[Arow * Bcol];
 *      riscv_dsp_mat_oprod_q31 (src1, src2, dst, Arow, Bcol);
 *     </pre>
 */
void riscv_dsp_mat_oprod_q31(const q31_t * src1, const q31_t * src2,
                       q31_t * dst, uint32_t size1, uint32_t size2);
/** @} mat_oprod */

/**
 * @defgroup cmat_mul Complex Matrix Multiplication Functions
 * @brief Complex Matrix Multiplication Functions
 *
 * Complex matrix multiplication functions compute the multiplication of two source complex
 * matrices and write the result into a destination complex matrix. Figure 4 shows the
 * multiplication of complex matrix A by matrix complex B, in which both matrices have 2 rows
 * and 2 columns
 *
 * @image html mat_cmul.gif ""
 *
 * and each multiplication is treated as a complex multiplication.
 *
 * @image html mat_cmul_2.gif ""
 *
 * According to the definition of matrix multiplication, the number of rows in complex matrix B
 * must be identical with the number of columns in complex matrix A. That is,
 *
 * @image html mat_cmul_3.gif ""
 *
 * where A is a complex matrix with i rows and j columns, B is a complex matrix with j rows and k
 * columns, and C is the destination complex matrix with i rows and k columns.
 *
 * Andes DSP library supports distinct complex matrix multiplication functions for the following
 * data types: floating-point, Q31, and Q15. These functions are introduced in the subsections
 * below.
 */
/**
 * @addtogroup cmat_mul
 * @{
 */
/**
 * @brief Multiplication of two floating-point complex matrices.
 * @param[in]   src1 pointer of the first input complex matrix
 * @param[in]   src2 pointer of the second input complex matrix
 * @param[out]  dst  pointer of the output complex matrix
 * @param[in]   row  number of rows in the first complex matrix
 * @param[in]   col  number of columns in the first complex matrix
 * @param[in]   col2 number of columns in the second complex matrix
 * @return none.
 *
 * @b Example
 *  <pre>
 * The following equation shows the multiplication of two complex matrices:
 *
 * @image html mat_cmul_4.gif ""
 *
 * Given each element with a value, the result is like
 *
 * @image html mat_cmul_5.gif ""
 *
 * and can be presented in code as follows:
 *
 *      \#define Arow 2
 *      \#define Acol 3
 *      \#define Bcol 2
 *      float32_t src1[2 * Arow * Acol] = {0.1, 0.2, -0.1, -0.3, 0.1, 0.4, 0.2, -0.1,
 *                                         -0.2, 0.5, 0.3, -0.4};
 *      float32_t src2[2 * Acol * Bcol] = {0.2, 0.5, 0.2, -0.4, -0.1, 0.2, 0.3, 0.2,
 *                                         -0.7, -0.4, -0.2, -0.1};
 *      float32_t dst[2 * Arow * Bcol];
 *      riscv_dsp_cmat_mul_f32 (src1, src2, dst, Arow, Acol, Bcol);
 *
 * This example also serves as a reference for examples of Q31 or Q15 complex matrix
 * multiplication functions.
 *  </pre>
 */
void riscv_dsp_cmat_mul_f32(const float32_t *src1, const float32_t *src2,
                           float32_t *dst, uint32_t row, uint32_t col, uint32_t col2);
/**
 * @brief Multiplication of two q15 complex matrices.
 * @param[in]   src1 pointer of the first input complex matrix
 * @param[in]   src2 pointer of the second input complex matrix
 * @param[out]  dst  pointer of the output complex matrix
 * @param[in]   row  number of rows in the first complex matrix
 * @param[in]   col  number of columns in the first complex matrix
 * @param[in]   col2 number of columns in the second complex matrix
 * @return none.
 */
void riscv_dsp_cmat_mul_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col, uint32_t col2);
/**
 * @brief Multiplication of two q31 complex matrices.
 * @param[in]   src1 pointer of the first input complex matrix
 * @param[in]   src2 pointer of the second input complex matrix
 * @param[out]  dst  pointer of the output complex matrix
 * @param[in]   row  number of rows in the first complex matrix
 * @param[in]   col  number of columns in the first complex matrix
 * @param[in]   col2 number of columns in the second complex matrix
 * @return none.
 */
void riscv_dsp_cmat_mul_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col, uint32_t col2);

/** @} cmat_mul */

/**
 * @defgroup mat_mul_mxv Matrix Multiplies Vector Functions
 * @brief Matrix Multiplies Vector Functions
 *
 * Matrix multiplies vector functions compute the multiplication of a matrix and a vector and write the result into a destination vector.
 *
 * According to the definition of matrix multiplication, the number of elements of vector B must be identical with the number of columns in matrix A.
 *
 * Andes DSP library supports distinct matrix multiplies vector functions for the following data types: single-precision floating-point, Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup mat_mul_mxv
 * @{
 */
/**
 * @brief Matrix multiply vector for f32 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 *
 * @b Example
 *  <pre>
 * The following equation shows the matrix A multiplied with a vector B:
 *
 * @image html mat_mul_mxv.png ""
 *
 * Given each element with a value, the result is like
 *
 * @image html mat_mul_mxv_2.png ""
 *
 * and can be presented in code as follows:
 *
 *     \#define Arow		2
 *     \#define Acol		3
 *     float32_t src1[Arow * Acol] = {0.1, -0.1, 0.1, 0.2, -0.2, 0.3};
 *     float32_t src2[Acol] = {0.2, -0.1, -0.7};
 *     float32_t dst[Arow];
 *     riscv_dsp_mat_mul_mxv_f32 (src1, src2, dst, Arow, Acol);
 *
 * This example also serves as a reference for examples of Q31, Q15 or Q7 functions.
 *  </pre>
 */
void riscv_dsp_mat_mul_mxv_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col);
/**
 * @brief Matrix multiply vector for q15 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 */
void riscv_dsp_mat_mul_mxv_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col);
/**
 * @brief Matrix multiply vector for q31 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 */
void riscv_dsp_mat_mul_mxv_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col);
/**
 * @brief Matrix multiply vector for q7 formats
 * @param[in]   src1 pointer of the input matrix
 * @param[in]   src2 pointer of the input vector
 * @param[out]  dst  pointer of the output vector
 * @param[in]   row  number of rows in the matrix
 * @param[in]   col  number of columns in the matrix and the elements size of vector
 * @return none.
 */
void riscv_dsp_mat_mul_mxv_q7(const q7_t *src1, const q7_t *src2,
                       q7_t *dst, uint32_t row, uint32_t col);

/** @} mat_mul_mxv */
/** @} mat */
#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_MATRIX_MATH_H__
