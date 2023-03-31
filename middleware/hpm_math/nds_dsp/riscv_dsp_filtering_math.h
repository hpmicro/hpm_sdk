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
#ifndef __RISCV_DSP_FILTERING_MATH_H__
#define __RISCV_DSP_FILTERING_MATH_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"
#include "riscv_dsp_utils_math.h"

/**
 * @defgroup filter Filtering Functions
 */
/**
 * @addtogroup filter
 * @{
 */

// Standard FIR filter
/**
 * @defgroup fir Finite Impulse Response (FIR) Filter Functions
 * @brief Finite Impulse Response (FIR) Filter Functions
 *
 * Finite impulse response (FIR) filter functions can be presented by the following equation:
 *
 * @image html fir.gif ""
 *
 * Where b[k] is the filter coefficient and Size is the number of filter coefficients.
 *
 * Andes DSP library supports distinct FIR filter functions and instance structures for the following
 * data types: floating-point, Q31, Q15 and Q7. Before you use FIR filter functions, make sure you
 * initialize the corresponding instance structures first. These functions and their instance
 * structures are introduced in the subsections below.
 */
/**
 * @addtogroup fir
 * @{
 */
/**
 * @brief Instance structure of the floating-point FIR filter
 */
typedef struct
{
    uint32_t coeff_size;   /**< number of coefficients */
    float32_t *state;      /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
    float32_t *coeff;      /**< pointer of the time reversed coefficient vector whose size
                                is <code>coeff_size</code> in time reversed order. */
} riscv_dsp_fir_f32_t;

/**
 * @brief Instance structure of the Q31 FIR filter
 */
typedef struct
{
    uint32_t coeff_size;   /**< number of coefficients */
    q31_t *state;          /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
    q31_t *coeff;          /**< pointer of the time reversed coefficient vector whose size
                                is <code>coeff_size</code> in time reversed order. */
} riscv_dsp_fir_q31_t;

/**
 * @brief Instance structure of the Q15 FIR filter
 */
typedef struct
{
    uint32_t coeff_size;   /**< number of coefficients */
    q15_t *state;          /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
    q15_t *coeff;          /**< pointer of the time reversed coefficient vector whose size
                                is <code>coeff_size</code> in time reversed order. */
} riscv_dsp_fir_q15_t;

/**
 * @brief Instance structure of the Q7 FIR filter
 */
typedef struct
{
    uint32_t coeff_size;   /**< number of coefficients */
    q7_t *state;           /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
    q7_t *coeff;           /**< pointer of the time reversed coefficient vector whose size
                                is <code>coeff_size</code> in time reversed order. */
} riscv_dsp_fir_q7_t;

/**
 * @brief Function for the floating-point FIR filter.
 * @param[in]       *instance pointer of the instance structure
 * @param[in]       *src      pointer of the input vector
 * @param[out]      *dst      pointer of the output vector
 * @param[in]       size      number of samples
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define coeff_size 4
 *      \#define size 6
 *      float32_t state[coeff_size + size - 1];
 *      float32_t coeff[coeff_size] = {0.3, -0.1, 0.4, 0.5};
 *      riscv_dsp_fir_f32_t inst = {coeff_size, state, coeff};
 *      float32_t src[size] = {0.1, -0.2, 0.2, 0.3, 0.4, 0.1};
 *      float32_t dst[size];
 *      riscv_dsp_fir_f32(&inst, src, dst, size);
 *
 * This example also serves as a reference for examples of Q31, fast_Q31, Q15, fast_Q15 or Q7
 * FIR filter functions.
 *     </pre>
 */
void riscv_dsp_fir_f32(const riscv_dsp_fir_f32_t *instance, float32_t *src,
                   float32_t *dst, uint32_t size);

/**
 * @brief Function for the q31 FIR filter.
 * @param[in]       *instance pointer of the instance structure
 * @param[in]       *src      pointer of the input vector
 * @param[out]      *dst      pointer of the output vector
 * @param[in]       size      number of samples
 * @return none.
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in Q31 format and multiplications
 * yield a Q2.62 result. These results are right-shifted by 31 bits, saturated to Q31 format and
 * saved into buffer. Thus, in order to avoid overflow, the input signal must be scaled down by
 * log2(coeff_size) bits before this function is called. When compared with
 * riscv_dsp_fir_fast_q31, this function delivers higher precision and smaller code size.
 */
void riscv_dsp_fir_q31(const riscv_dsp_fir_q31_t *instance, q31_t *src,
                   q31_t *dst, uint32_t size);

/**
 * @brief Function for the q31 FIR filter.
 * @param[in]       *instance pointer of the instance structure
 * @param[in]       *src      pointer of the input vector
 * @param[out]      *dst      pointer of the output vector
 * @param[in]       size      number of samples
 * @return none.
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in Q31 format and multiplications
 * yield a Q1.31 result. These results are left-shifted by 1 bit and saved into buffer. Thus, in
 * order to avoid overflow, the input signal must be scaled down by log2(coeff_size) bits
 * before this function is called. When compared with riscv_dsp_fir_q31, this function
 * delivers higher performance with lower precision and larger code size.
 */
void riscv_dsp_fir_fast_q31(const riscv_dsp_fir_q31_t *instance, q31_t *src,
                   q31_t *dst, uint32_t size);
/**
 * @brief Function for the q15 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in Q15 format and multiplications
 * yield a Q30 result. The results are accumulated in a 64-bit accumulator in Q34.30 format
 * and then truncated to Q34.15 format by discarding the low 15 bits. Lastly, the outputs are
 * saturated to yield a result in Q1.15 format.When compared with
 * riscv_dsp_fir_fast_q15, this function delivers higher precision and smaller code size.
 */
void riscv_dsp_fir_q15(const riscv_dsp_fir_q15_t *instance, q15_t *src,
                   q15_t *dst, uint32_t size);

/**
 * @brief Function for the q15 FIR filter.
 * @param[in]       *instance pointer of the instance structure
 * @param[in]       *src      pointer of the input vector
 * @param[out]      *dst      pointer of the output vector
 * @param[in]       size      number of samples
 * @return none.
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in Q15 format and multiplications
 * yield a Q30 result. The results are accumulated in a 32-bit accumulator in Q2.30 format
 * and then saturated to yield a result in Q1.15 format. When compared with
 * riscv_dsp_fir_q15, this function delivers higher performance with lower precision and
 * larger code size.
 */
void riscv_dsp_fir_fast_q15(const riscv_dsp_fir_q15_t *instance, q15_t *src,
                   q15_t *dst, uint32_t size);
/**
 * @brief Function for the q7 FIR filter.
 * @param[in]       *instance pointer of the instance structure
 * @param[in]       *src      pointer of the input vector
 * @param[out]      *dst      pointer of the output vector
 * @param[in]       size      number of samples
 * @return none.
 *
 * @b Note:
 *
 * Both inputs are in Q7 format and multiplications yield a Q14 result. The intermediate
 * results are accumulated in a 32-bit accumulator in Q18.14 format. The result is then
 * converted to Q18.7 format by discarding the low 7 bits and then saturated to Q1.7 format.
 */
void riscv_dsp_fir_q7(const riscv_dsp_fir_q7_t *instance, q7_t *src, q7_t *dst,
                  uint32_t size);
/** @} fir */

// Lattice FIR filter
/**
 * @defgroup lfir Lattice FIR Filter Functions
 * @brief Lattice FIR Filter Functions
 *
 * Lattice FIR filter functions can be represented by the following equations:
 *
 * @image html lfir.gif ""
 *
 * Where x is the input, y is the output, u is the state of previous inputs, 0 <= z < M and
 * M is the stage number, and Kz is the reflection coefficients.
 *
 * Andes DSP library supports distinct lattice FIR filter functions and instance structures for the
 * following data types: floating-point, Q31 and Q15. Before you use lattice FIR filter functions,
 * make sure you initialize the corresponding instance structure first. These functions and their
 * instance structures are introduced in the subsections below.
 */
/**
 * @addtogroup lfir
 * @{
 */
/**
 * @brief Structure for the f32 Lattice FIR filter.
 */
typedef struct
{
    uint32_t stage;    /**< number of filter stage. */
    float32_t *state;  /**< pointer of the state vector whose size is <code>stage</code>. */
    float32_t *coeff;  /**< pointer of the time reversed coefficient vector whose size is <code>stage</code>. */
} riscv_dsp_lfir_f32_t;

/**
 * @brief Structure for the q15 Lattice FIR filter.
 */
typedef struct
{
    uint32_t stage;    /**< number of filter stage. */
    q15_t *state;      /**< pointer of the state vector whose size is <code>stage</code>. */
    q15_t *coeff;      /**< pointer of the time reversed coefficient vector whose size is <code>stage</code>. */
} riscv_dsp_lfir_q15_t;

/**
 * @brief Structure for the q31 Lattice FIR filter.
 */
typedef struct
{
    uint32_t stage;    /**< number of filter stage. */
    q31_t *state;      /**< pointer of the state vector whose size is <code>stage</code>. */
    q31_t *coeff;      /**< pointer of the time reversed coefficient vector whose size is <code>stage</code>. */
} riscv_dsp_lfir_q31_t;

/**
 * @brief Function for the floating-point lattice FIR filter.
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 * @return none.
 *
 * @b Example
 *     <pre>
 * With the filter length as 6 and the input size as 8, set the instance of the lattice FIR filter as:
 *
 *      \#define M		6
 *      \#define size	8
 *      float32_t state[M];
 *      float32_t coeff[M] = {…};
 *      float32_t src[size] = {…};
 *      float32_t dst[size];
 *      riscv_dsp_lfir_f32_t inst = {M, state, coeff};
 *      riscv_dsp_lfir_f32 (&inst, src, dst, size);
 *
 * This example also serves as a reference for examples of Q31 or Q15 lattice FIR filter
 * functions.
 *     </pre>
 */
void riscv_dsp_lfir_f32(const riscv_dsp_lfir_f32_t *instance, float32_t *src,
                    float32_t *dst, uint32_t size);

/**
 * @brief Function for the q15 lattice FIR filter.
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 * @return none.
 */
void riscv_dsp_lfir_q15(const riscv_dsp_lfir_q15_t *instance, q15_t *src,
                    q15_t *dst, uint32_t size);

/**
 * @brief Function for the q31 lattice FIR filter.
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 * @return none.
 *
 * Function notes:
 * In order to avoid overflows the input signal must be scaled down by
 * 2*log2(stage) bits.
 */
void riscv_dsp_lfir_q31(const riscv_dsp_lfir_q31_t *instance, q31_t *src,
                    q31_t *dst, uint32_t size);
/** @} lfir */

// Decimator FIR Filters
/**
 * @defgroup dcmfir Decimation FIR Filter Function
 * @brief Decimation FIR Filter Function
 *
 * The decimation FIR filter function implements FIR filter with decimation by
 * an integer factor M. They can be presented by the following equation:
 *
 * @image html dcmfir.gif ""
 *
 * Where b[k] is the filter coefficient, src is the input data of the length Size,
 * and dst is the output data of the length Size/M. Size must be a multiple of the
 * decimation factor M so that the number of output samples can be an integer.
 *
 * Andes DSP library supports distinct decimation FIR filter functions and instance structures for
 * the following data types: floating-point, Q31 and Q15. Before you use decimation FIR filter
 * functions, make sure you initialize the corresponding instance structure first. These functions
 * and their instance structures are introduced in the subsections below.
 */
/**
 * @addtogroup dcmfir
 * @{
 */
/**
 * @brief Structure for the f32 Decimator FIR Filters.
 */
typedef struct
{
    uint32_t M;             /**< decimation factor. */
    uint32_t coeff_size;    /**< number of coefficients */
    float32_t *coeff;       /**< pointer of the time reversed coefficient vector whose size is <code>coeff_size</code>. */
    float32_t *state;       /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
} riscv_dsp_dcmfir_f32_t;

/**
 * @brief Structure for the q15 Decimator FIR Filters.
 */
typedef struct
{
    uint32_t M;          /**< decimation factor. */
    uint32_t coeff_size; /**< number of coefficients */
    q15_t *coeff;        /**< pointer of the time reversed coefficient vector whose size is <code>coeff_size</code>. */
    q15_t *state;        /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
} riscv_dsp_dcmfir_q15_t;

/**
 * @brief Structure for the q31 Decimator FIR Filters.
 */
typedef struct
{
    uint32_t M;          /**< decimation factor. */
    uint32_t coeff_size; /**< number of coefficients */
    q31_t *coeff;        /**< pointer of the time reversed coefficient vector whose size is <code>coeff_size</code>. */
    q31_t *state;        /**< pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
} riscv_dsp_dcmfir_q31_t;

/**
 * @brief Floating-point decimator FIR filters function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Example
 *     <pre>
 * With the input size as 24, the decimation factor M as 4 and the filter coefficient
 * size as 6, set the instance of the decimation FIR filter as:
 *
 *     \#define size       24
 *     \#define M           4
 *     \#define coeff_size  6
 *     float32_t coeff[coeff_size] = {…};
 *     float32_t state[coeff_size + size - 1];
 *     riscv_dsp_dcmfir_f32_t inst = {M, coeff_size, coeff, state};
 *     float32_t src[size] = {0.1, -0.2, 0.2, 0.3, 0.4, 0.1, …};
 *     float32_t dst[size / M];
 *     riscv_dsp_dcmfir_f32(&inst, src, dst, size);
 *
 * This example also serves as a reference for examples of Q31 or Q15 decimation FIR filter
 * functions.
 *     </pre>
 */
void riscv_dsp_dcmfir_f32(const riscv_dsp_dcmfir_f32_t *instance, float32_t *src,
                      float32_t *dst, uint32_t size);
/**
 * @brief Q15 decimator FIR filters function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in Q15 format. A 64-bit accumulator is
 * used to do multiply-accumulate operations with saturation to yield a Q33.30 result. After
 * all operations are performed, the results are saturated to Q15 format.
 */
void riscv_dsp_dcmfir_q15(const riscv_dsp_dcmfir_q15_t *instance, q15_t *src,
                      q15_t *dst, uint32_t size);
/**
 * @brief Q15 decimator fast FIR filters function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * 1. Both coefficients and state variables are represented in Q15 format. A 32-bit
 * accumulator is used to do multiply-accumulate operations with saturation to yield a
 * Q2.30 result. If the accumulator result overflows, it wraps around and distorts the
 * result. In order to avoid overflows completely, the input signal must be scaled down by
 * log2(coeff_size) bits. After all operations are performed, the Q2.30 accumulator is
 * then truncated to Q2.15 format and saturated to yield the Q1.15 result.
 *
 * 2. When compared with riscv_dsp_dcmfir_q15, this function delivers higher
 * performance.
 */
void riscv_dsp_dcmfir_fast_q15(const riscv_dsp_dcmfir_q15_t *instance, q15_t *src,
                      q15_t *dst, uint32_t size);
/**
 * @brief Q31 decimator FIR filters function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in Q31 format. A 64-bit accumulator is
 * used to do multiply-accumulate operations with saturation to yield a Q1.62 result. After all
 * operations are performed, the results are saturated to Q31 format.
 */
void riscv_dsp_dcmfir_q31(const riscv_dsp_dcmfir_q31_t *instance, q31_t *src,
                      q31_t *dst, uint32_t size);
/**
 * @brief Q31 decimator fast FIR filters function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * 1. Both coefficients and state variables are represented in Q31 format. A 32-bit
 * accumulator is used to do multiply-accumulate operations and truncated to Q2.30
 * result. To avoid overflows that distort the result, the input signal must be scaled down
 * by log2(coeff_size) bits. After all operations are performed, the results are saturated
 * and converted to Q1.31 format.
 *
 * 2. When compared with riscv_dsp_dcmfir_q31, this function delivers higher
 * performance with lower precision.
 */
void riscv_dsp_dcmfir_fast_q31(const riscv_dsp_dcmfir_q31_t *instance, q31_t *src,
                      q31_t *dst, uint32_t size);

/** @} dcmfir */

// Upsampling FIR Filters
/**
 * @defgroup upspl Upsampling FIR Filter Function
 * @brief Upsampling FIR Filter Function
 *
 * The upsampling FIR filer function upsamples incoming signals to generate multi-rate
 * outputs. It is usually used in audio/sound or images processing like increasing the
 * resolution of images or adjusting the audio output sample rate from 16kbps to 32kbps.
 *
 * Andes DSP library supports distinct upsampling FIR filter functions and instance structures for
 * the following data types: floating-point, Q31 and Q15. Before you use upsampling filter functions,
 * make sure you initialize the corresponding instance structure first. These functions and their
 * instance structures are introduced in the subsections below.
 */
/**
 * @addtogroup upspl
 * @{
 */
/**
 * @brief Structure for the floating-point Upsampling FIR Filters.
 */
typedef struct
{
    uint32_t L;          /**< upsampling factor */
    uint32_t plen;       /**< number of coefficients */
    float32_t *coeff;    /**< pointer of the time reversed coefficient vector whose size is "L * plen" */
    float32_t *state;    /**< pointer of the state vector whose size is "plen + size - 1" */
} riscv_dsp_upsplfir_f32_t;

/**
 * @brief Structure for the Q15 Upsampling FIR Filters.
 */
typedef struct
{
    uint32_t L;      /**< upsampling factor */
    uint32_t plen;   /**< number of coefficients */
    q15_t *coeff;    /**< pointer of the time reversed coefficient vector whose size is "L * plen" */
    q15_t *state;    /**< pointer of the state vector whose size is "plen + size - 1" */
} riscv_dsp_upsplfir_q15_t;

/**
 * @brief Structure for the Q31 Upsampling FIR Filters.
 */
typedef struct
{
    uint32_t L;      /**< upsampling factor */
    uint32_t plen;   /**< number of coefficients */
    q31_t *coeff;    /**< pointer of the time reversed coefficient vector whose size is "L * plen" */
    q31_t *state;    /**< pointer of the state vector whose size is "plen + size - 1" */
} riscv_dsp_upsplfir_q31_t;

/**
 * @brief Floating-point Upsampling FIR Filters
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Example
 *     <pre>
 *     \#define L       2
 *     \#define plen    4
 *     \#define size    6
 *     float32_t state[plen + size - 1] = {0.0};
 *     float32_t coeff[L * plen] = {0.40, 0.10, 0.24, -0.40, 1.0, 0.5, 0.4, -0.1};
 *     float32_t src[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *     float32_t dst[size * L];
 *     riscv_dsp_upsplfir_f32_t instance = {L, plen, state, coeff};
 *     riscv_dsp_upsplfir_f32(&instance, src, dst, size);
 *
 * This example also serves as a reference for examples of Q31 and Q15 Upsampling FIR filter
 * functions.
 *     </pre>
 */
void riscv_dsp_upsplfir_f32(const riscv_dsp_upsplfir_f32_t *instance, float32_t *src,
                        float32_t *dst, uint32_t size);
/**
 * @brief q15 Upsampling FIR Filters
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in 1.15 format and multiplications
 * yield a 2.30 result. The 2.30 results are accumulated in a 64-bit accumulator in 34.30
 * format and the result is truncated to 34.15 format by discarding low 15 bits. Lastly, the
 * outputs are saturated to yield a result in 1.15 format.
 */
void riscv_dsp_upsplfir_q15(const riscv_dsp_upsplfir_q15_t *instance, q15_t *src,
                        q15_t *dst, uint32_t size);
/**
 * @brief q31 Upsampling FIR Filters
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * Both coefficients and state variables are represented in 1.31 format and multiplications
 * yield a 2.62 result. The 2.62 results are accumulated in a 64-bit accumulator. After all
 * operations are performed, the 2.62 accumulator is truncated to 1.32 format and then
 * saturated to 1.31 format.
 */
void riscv_dsp_upsplfir_q31(const riscv_dsp_upsplfir_q31_t *instance, q31_t *src,
                        q31_t *dst, uint32_t size);

/** @} upspl */
// Sparse FIR Filters
/**
 * @defgroup spafir Sparse FIR Filter Function
 * @brief Sparse FIR Filter Function
 *
 * The sparse FIR filter function is the FIR filter with the majority of coefficients
 * being zero. Due to this characteristic, the sparse FIR filter only keeps the non-zero
 * coefficients and ignores the multiplication operation of signal and zero coefficients.
 *
 * Andes DSP library supports distinct sparse FIR filter functions and instance structures for the
 * following data types: floating-point, Q31, Q15 and Q7. Before you use sparse FIR filter functions,
 * make sure you initialize the corresponding instance structures first. These functions and their
 * instance structures are introduced in the subsections below.
 */
/**
 * @addtogroup spafir
 * @{
 */
/**
 * @brief Structure for the flaoting-point Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    /**< number of filter coefficients */
    uint16_t index;         /**< index of the state buffer */
    float32_t *state;       /**< pointer of the state vector whose size is <code>delay + size</code> */
    float32_t *coeff;       /**< pointer of the coefficient vector whose size is <code>coeff_size</code> */
    uint16_t delay;         /**< maximum value in the nezdelay vector */
    int32_t *nezdelay;      /**< vector which stores non-zero coefficients and the size
                                 is <code>coeff_size</code> */
} riscv_dsp_spafir_f32_t;

/**
 * @brief Structure for the Q15 Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    /**< number of filter coefficients */
    uint16_t index;         /**< index of the state buffer */
    q15_t *state;           /**< pointer of the state vector whose size is <code>delay + size</code> */
    q15_t *coeff;           /**< pointer of the coefficient vector whose size is <code>coeff_size</code> */
    uint16_t delay;         /**< maximum value in the nezdelay vector */
    int32_t *nezdelay;      /**< vector which stores non-zero coefficients and the size
                                 is <code>coeff_size</code> */
} riscv_dsp_spafir_q15_t;

/**
 * @brief Structure for the Q31 Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    /**< number of filter coefficients */
    uint16_t index;         /**< index of the state buffer */
    q31_t *state;           /**< pointer of the state vector whose size is <code>delay + size</code> */
    q31_t *coeff;           /**< pointer of the coefficient vector whose size is <code>coeff_size</code> */
    uint16_t delay;         /**< maximum value in the nezdelay vector */
    int32_t *nezdelay;      /**< vector which stores non-zero coefficients and the size
                                 is <code>coeff_size</code> */
} riscv_dsp_spafir_q31_t;

/**
 * @brief Structure for the Q7 Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    /**< number of filter coefficients */
    uint16_t index;         /**< index of the state buffer */
    q7_t *state;            /**< pointer of the state vector whose size is <code>delay + size</code> */
    q7_t *coeff;            /**< pointer of the coefficient vector whose size is <code>coeff_size</code> */
    uint16_t delay;         /**< maximum value in the nezdelay vector */
    int32_t *nezdelay;      /**< vector which stores non-zero coefficients and the size
                                 is <code>coeff_size</code> */
} riscv_dsp_spafir_q7_t;

/**
 * @brief flaoting-point Sparse FIR Filters.
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] buf pointer of the temporary vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 *  The size of temporary buffer should be equal to size, size of the input vector.
 *
 * @b Example
 *     <pre>
 * The example below can be treated as the following formula:
 *
 * <code> y(n)=coeff(0)*x(n - nezdelay(0)) + coeff(1)*x(n - nezdelay(1))+ coeff(2)*x(n - nezdelay(2)) + coeff(3)*x(n - nezdelay(3)) </code>
 *
 *       \#define coeff_size    4
 *       \#define index         1
 *       \#define size          6
 *       int32_t nezdelay[coeff_size] = {3, 0, 2, 1}
 *       \#define delay         3
 *
 *       float32_t state[delay + size] = {0.0};
 *       float32_t coeff[coeff_size] = {0.40, 0.10, 0.24, -0.40};
 *       float32_t src[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *       float32_t buf[size];
 *       float32_t dst[size];
 *
 *       riscv_dsp_spafir_f32_t instance = {coeff_size, index, state, coeff, delay, nezdelay};
 *       riscv_dsp_spafir_f32(&instance, src, dst, buf, size);
 *
 * This example also serves as a reference for examples of Q31, Q15 and Q7 sparse FIR filter
 * functions.
 *     </pre>
 */
void riscv_dsp_spafir_f32(riscv_dsp_spafir_f32_t *instance, float32_t *src,
                      float32_t *dst, float32_t *buf, uint32_t size);
/**
 * @brief q15 Sparse FIR Filters.
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] buf1 pointer of the temporary buffer 1
 * @param[in] buf2 pointer of the temporary buffer 2
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * The temporary buffer 1 and 2 should both have a size of size (size of the input vector).
 */
void riscv_dsp_spafir_q15(riscv_dsp_spafir_q15_t *instance, q15_t *src,
                      q15_t *dst, q15_t *buf1, q31_t *buf2, uint32_t size);
/**
 * @brief q31 Sparse FIR Filters.
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] buf pointer of the temporary vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * The size of temporary buffer should be equal to size, size of the input vector.
 */
void riscv_dsp_spafir_q31(riscv_dsp_spafir_q31_t *instance, q31_t *src,
                      q31_t *dst, q31_t *buf, uint32_t size);
/**
 * @brief q7 Sparse FIR Filters.
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] buf1 pointer of the temporary buffer 1
 * @param[in] buf2 pointer of the temporary buffer 2
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * The temporary buffer 1 and 2 should both have a size of size (size of the input vector).
 */
void riscv_dsp_spafir_q7(riscv_dsp_spafir_q7_t *instance, q7_t *src,
                     q7_t *dst, q7_t *buf1, q31_t *buf2, uint32_t size);

/** @} spafir */

// Standard LMS filte
/**
 * @defgroup lms Least Mean Square (LMS) Filter Functions
 * @brief Least Mean Square (LMS) Filter Functions
 *
 * Least mean square (LMS) filter functions are adaptive filters which perform
 * the following equations to update coefficients and get the minimum mean square error:
 *
 * @image html lms.gif ""
 *
 * Where
 *  - y is the output vector.
 *  - h is the coefficients vector.
 *  - x is the input vector.
 *  - L is the filter order.
 *  - e is the error vector.
 *  - d is the desired vector.
 *  - u is the step size which can adjust coefficients.
 *
 * Andes DSP library supports distinct least mean square filter functions and instance structures
 * for the following data types: floating-point, Q31 and Q15. Before you use least mean square filter
 * functions, make sure you initialize the corresponding instance structure first. These functions
 * and their instance structures are introduced in the subsections below.
 */
/**
 * @addtogroup lms
 * @{
 */
/**
 * @brief Structure for the floatint-point standard LMS Filters.
 */
typedef struct
{
    uint32_t coeff_size;  /**< Number of filter coefficients */
    float32_t *state;     /**< Pointer of the state vector whose size is <code>coeff_size + size - 1</code> */
    float32_t *coeff;     /**< Pointer of the time reversed coefficient vector whose size is <code>coeff_size</code> */
    float32_t mu;         /**< Step size which can adjust coefficients */
} riscv_dsp_lms_f32_t;

/**
 * @brief Structure for the Q31 standard LMS Filters.
 */
typedef struct
{
    uint32_t coeff_size;  /**< Number of filter coefficients */
    q31_t *state;         /**< Pointer of the state vector whose size is <code>coeff_size + size - 1</code> */
    q31_t *coeff;         /**< Pointer of the time reversed coefficient vector whose size
                               is <code>coeff_size</code> */
    q31_t mu;             /**< Step size which can adjust coefficients */
    q31_t shift;          /**< number of coefficient bits to be shifted */
} riscv_dsp_lms_q31_t;

/**
 * @brief Structure for the Q15 standard LMS Filters.
 */
typedef struct
{
    uint32_t coeff_size;  /**< Number of filter coefficients */
    q15_t *state;         /**< Pointer of the state vector whose size is <code>coeff_size + size - 1</code> */
    q15_t *coeff;         /**< Pointer of the time reversed coefficient vector whose size
    q15                        is <code>coeff_size</code> */
    q15_t mu;             /**< Step size which can adjust coefficients */
    q15_t shift;          /**< number of coefficient bits to be shifted */
} riscv_dsp_lms_q15_t;

/**
 * @brief Function for the floating-point LMS filter.
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[in] ref pointer of the desired vector
 * @param[out] dst pointer of the output vector
 * @param[out] err pointer of the error vector
 * @param[in] size number of samples
 * @return none.
 *
 * @b Example
 *     <pre>
 *     \#define coeff_size    5
 *     \#define size          6
 *     \#define mu            0.05
 *     float32_t state[coeff_size + size - 1] = {0.0};
 *     float32_t coeff[coeff_size] = {0.40, 0.10, 0.24, -0.40, -0.34};
 *     float32_t src[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *     float32_t ref[size] = {0.1, 0.2, -0.1, -0.02, -0.1, 0.2};
 *     float32_t err[size];
 *     float32_t dst[size];
 *
 *     riscv_dsp_lms_f32_t instance = {coeff_size, state, coeff, mu};
 *     riscv_dsp_lms_f32(&instance, src, ref, dst, err, size);
 *
 * This example also serves as a reference for examples of Q31 and Q15 LMS filter functions.
 *     </pre>
 */
void riscv_dsp_lms_f32(const riscv_dsp_lms_f32_t *instance, float32_t *src,
                   float32_t *ref, float32_t *dst, float32_t *err, uint32_t size);

/**
 * @brief Function for the q31 LMS filter.
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[in] ref pointer of the desired vector
 * @param[out] dst pointer of the output vector
 * @param[out] err pointer of the error vector
 * @param[in] size number of samples
 * @return none.
 */
void riscv_dsp_lms_q31(const riscv_dsp_lms_q31_t *instance, q31_t *src, q31_t *ref,
                   q31_t *dst, q31_t *err, uint32_t size);

/**
 * @brief Function for the q15 LMS filter.
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[in] ref pointer of the desired vector
 * @param[out] dst pointer of the output vector
 * @param[out] err pointer of the error vector
 * @param[in] size number of samples
 * @return none.
 */
void riscv_dsp_lms_q15(const riscv_dsp_lms_q15_t *instance, q15_t *src, q15_t *ref,
                   q15_t *dst, q15_t *err, uint32_t size);

/** @} lms */
// Normalized LMS filter
/**
 * @defgroup nlms Normalized Least Mean Square (NLMS) Filter Function
 * @brief Normalized Least Mean Square (NLMS) Filter Function
 *
 * The Normalized Least Mean Square (NLMS) filter function is an extension of
 * LMS filter function. It can be represented by the following equations:
 *
 * @image html nlms.gif ""
 *
 * Where
 *  - y is the output vector.
 *  - h is the coefficients vector.
 *  - x is the input vector.
 *  - L is the filter order.
 *  - e is the error vector.
 *  - d is the desired vector.
 *  - u is the step size which can adjust coefficients and is related to the normalized input signals.
 *
 * Andes DSP library supports distinct NLMS filter function and instance structure for the
 * following data types: floating point, Q31 and Q15. Before you use the NLMS filter function, make
 * sure you initialize the corresponding instance structure first. These functions and their instance
 * structures are introduced in the subsections below.
 */
/**
 * @addtogroup nlms
 * @{
 */
/**
 * @brief Structure for the f32 normalized LMS filter.
 */
typedef struct
{
    uint32_t coeff_size; /**< Number of filter coefficients */
    float32_t *state;    /**< Pointer of the state vector whose size is <code>coeff_size + size - 1</code> */
    float32_t *coeff;    /**< Pointer of the time reversed coefficient vector whose size is <code>coeff_size</code> */
    float32_t mu;        /**< Step size which can adjust coefficients */
    float32_t energy;    /**< Energy of the previous frame */
} riscv_dsp_nlms_f32_t;

/**
 * @brief Floating-point NLMS filter function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[in] ref pointer of the desired vector
 * @param[out] dst pointer of the output vector
 * @param[out] err pointer of the error vector
 * @param[in] size number of samples
 *
 * @b Example
 *     <pre>
 *     \#define coeff_size   5
 *     \#define size         6
 *     \#define mu           0.05
 *     \#define energy       0.1
 *     float32_t state[coeff_size + size - 1] = {0.0};
 *     float32_t coeff[coeff_size] = {0.40, 0.10, 0.24, -0.40, -0.34};
 *     float32_t src[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *     float32_t ref[size] = {0.1, 0.2, -0.1, -0.02, -0.1, 0.2};
 *     float32_t err[size];
 *     float32_t dst[size];
 *
 *     riscv_dsp_nlms_f32_t instance = {coeff_size, state, coeff, mu, energy};
 *     riscv_dsp_nlms_f32(&instance, src, ref, dst, err, size);
 *
 * This example also serves as a reference for examples of Q31 and Q15 NLMS filter functions.
 *     </pre>
 */
void riscv_dsp_nlms_f32(riscv_dsp_nlms_f32_t *instance, float32_t *src,
                    float32_t *ref, float32_t *dst, float32_t *err, uint32_t size);


/**
 * @brief Structure for the q31 normalized LMS filter.
 */
typedef struct
{
    uint32_t coeff_size; /**<  number of filter coefficients */
    q31_t *state;        /**<  pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
    q31_t *coeff;        /**<  pointer of the time reversed coefficient vector whose size
                               is <code>coeff_size</code> */
    q31_t mu;            /**<  step size which can adjust coefficients */
    uint8_t postshift;   /**<  number of coefficient bits to be shifted */
    q31_t energy;        /**<  energy of the previous frame */
    q31_t x0;            /**<  previous input sample */
} riscv_dsp_nlms_q31_t;


/**
 * @brief q31 NLMS filter function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[in] ref pointer of the desired vector
 * @param[out] dst pointer of the output vector
 * @param[out] err pointer of the error vector
 * @param[in] size number of samples
 */
void riscv_dsp_nlms_q31(riscv_dsp_nlms_q31_t *instance, q31_t *src,
                    q31_t *ref, q31_t *dst, q31_t *err, uint32_t size);


/**
 * @brief Structure for the q15 normalized LMS filter.
 */
typedef struct
{
    uint32_t coeff_size; /**<  number of filter coefficients */
    q15_t *state;        /**<  pointer of the state vector whose size is <code>coeff_size + size - 1</code>. */
    q15_t *coeff;        /**<  pointer of the time reversed coefficient vector whose size
                               is <code>coeff_size</code> */
    q15_t mu;            /**<  step size which can adjust coefficients */
    uint8_t postshift;   /**<  number of coefficient bits to be shifted */
    q15_t energy;        /**<  energy of the previous frame */
    q15_t x0;            /**<  previous input sample */
} riscv_dsp_nlms_q15_t;


/**
 * @brief q15 NLMS filter function
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[in] ref pointer of the desired vector
 * @param[out] dst pointer of the output vector
 * @param[out] err pointer of the error vector
 * @param[in] size number of samples
 */
void riscv_dsp_nlms_q15(riscv_dsp_nlms_q15_t *instance, q15_t *src,
                    q15_t *ref, q15_t *dst, q15_t *err, uint32_t size);

/** @} nlms */

// Convolution
/**
 * @defgroup conv Convolution Functions
 * @brief Convolution Functions
 *
 * Convolution functions combine two signals into one signal mathematically. The convolution
 * equation can be presented as:
 *
 * @image html conv.gif ""
 *
 * Where the size of src1[n] is len1, the size of src2[n] is len2, and the size of dst[n] is len1 +
 * len2 - 1.
 *
 * Andes DSP library supports distinct convolution functions for the following data types:
 * floating-point, Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup conv
 * @{
 */
/**
 * @brief Convolution of the floating-point vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define len1 6
 *      \#define len2 4
 *      float32_t src1[len1] = {0.3, 0.2, -0.1, 0.2, 0.4, 0.2};
 *      float32_t src2[len2] = {-0.1, 0.3, 0.2, -0.2};
 *      float32_t dst[len1 + len2 - 1];
 *      riscv_dsp_conv_f32(src1, len1, src2, len2, dst);
 *
 * This example also serves as a reference for examples of Q31, Q15 or Q7 convolution
 * functions.
 *     </pre>
 */
void riscv_dsp_conv_f32(float32_t *src1, uint32_t len1,
                    float32_t *src2, uint32_t len2, float32_t *dst);

/**
 * @brief Convolution of the q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @return none.
 */
void riscv_dsp_conv_q15(q15_t *src1, uint32_t len1,
                    q15_t *src2, uint32_t len2, q15_t *dst);

/**
 * @brief Convolution of the q31 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @return none.
 */
void riscv_dsp_conv_q31(q31_t *src1, uint32_t len1,
                    q31_t *src2, uint32_t len2, q31_t *dst);

/**
 * @brief Convolution of the q7 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @return none.
 */
void riscv_dsp_conv_q7(q7_t *src1, uint32_t len1,
                   q7_t *src2, uint32_t len2, q7_t *dst);
/** @} conv */

/**
 * @defgroup conv_partial Partial Convolution Functions
 * @brief Partial Convolution Functions
 *
 * With a starting index and a size specified, partial convolution functions do the convolution
 * partially for two signals. The partial convolution result will be generated to the destination
 * vector in the range [start_index, … , start_index + size - 1 ]. Thus, if the partial
 * convolution result is not within the range [0, … , len1 + len2 - 2], a return value of -1 will be
 * given for the argument error
 *
 * Andes DSP library supports distinct partial convolution functions for the following data types:
 * floating-point, Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup conv_partial
 * @{
 */
/**
 * @brief Convolution Partial of the floating-point vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @param[in]       startindex starting index of the partial convolution
 * @param[in]       size length of the partial convolution
 * @return          Returns (0: success; -1: failure)
 *
 * @b Example
 *     <pre>
 *      \#define len1 6
 *      \#define len2 4
 *      float32_t src1[len1] = {0.3, 0.2, -0.1, 0.2, 0.4, 0.2};
 *      float32_t src2[len2] = {-0.1, 0.3, 0.2, -0.2};
 *      uint32_t dst[len1 + len2 - 1];
 *      riscv_dsp_conv_partial_f32(src1, len1, src2, len2, dst, 3, 4);
 *
 * This example also serves as a reference for examples of Q31, Q15 or Q7 partial convolution
 * functions.
 *     </pre>
 */
int32_t riscv_dsp_conv_partial_f32(float32_t *src1, uint32_t len1,
                               float32_t *src2, uint32_t len2, float32_t *dst, uint32_t startindex, uint32_t size);

/**
 * @brief Convolution Partial of the q15 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @param[in]       startindex starting index of the partial convolution
 * @param[in]       size length of the partial convolution
 * @return          Returns (0: success; -1: failure)
 */
int32_t riscv_dsp_conv_partial_q15(q15_t *src1, uint32_t len1,
                               q15_t *src2, uint32_t len2, q15_t *dst, uint32_t startindex, uint32_t size);

/**
 * @brief Convolution Partial of the q31 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @param[in]       startindex starting index of the partial convolution
 * @param[in]       size length of the partial convolution
 * @return          Returns (0: success; -1: failure)
 */
int32_t riscv_dsp_conv_partial_q31(q31_t *src1, uint32_t len1,
                               q31_t *src2, uint32_t len2, q31_t *dst, uint32_t startindex, uint32_t size);

/**
 * @brief Convolution Partial of the q7 vectors.
 * @param[in]       *src1 pointer of the first input vector
 * @param[in]       len1  length of the first input vector
 * @param[in]       *src2 pointer of the second input vector
 * @param[in]       len2  length of the second input vector
 * @param[out]      *dst  pointer of the output vector
 * @param[in]       startindex starting index of the partial convolution
 * @param[in]       size length of the partial convolution
 * @return          Returns (0: success; -1: failure)
 */
int32_t riscv_dsp_conv_partial_q7(q7_t *src1, uint32_t len1,
                               q7_t *src2, uint32_t len2, q7_t *dst, uint32_t startindex, uint32_t size);

/** @} conv_partial */

// Correlation
/**
 * @defgroup corr Correlation Functions
 *
 * @brief Correlation functions
 *
 * Correlation functions use two signals to form another signal (also called
 * cross-correlation) mathematically. The mathematical operations of correlation
 * functions are similar to those of convolution functions and can be presented as:
 *
 * @image html corr.gif ""
 *
 * Where src1 and src2 are input vectors, and their lengths are len1 and len2
 * separately; dst is the output vector and its size is 2 * max (len1, len2) - 1.
 *
 * Andes DSP library supports distinct correlation functions for the following data types:
 * floating-point, Q31, Q15 and Q7. These functions are introduced in the subsections below.
 */
/**
 * @addtogroup corr
 * @{
 */
/**
 * @brief Correlation of the floating-point vectors.
 * @param[in] src1 pointer of the first input vector
 * @param[in] len1 length of the first input vector
 * @param[in] src2 pointer of the second input vector
 * @param[in] len2 length of the second input vector
 * @param[out] dst pointer of the output vector
 * @return none.
 *
 * @b Example
 *     <pre>
 *     \#define len1	7
 *     \#define len2	5
 *     float32_t src1[len1] = {0.3, 0.2, -0.1, 0.2, 0.4, 0.2, -0.1};
 *     float32_t src2[len2] = {-0.1, 0.3, 0.2, -0.2, -0.2};
 *     float32_t dst[2 * len1 - 1];
 *     riscv_dsp_corr_f32(src1, len1, src2, len2, dst);
 *
 * This example also serves as a reference for examples of Q31, Q15 or Q7 correlation
 * functions.
 *     </pre>
 */
void riscv_dsp_corr_f32(float32_t *src1, uint32_t len1,
                    float32_t *src2, uint32_t len2, float32_t *dst);

/**
 * @brief Correlation of the q15 vectors.
 * @param[in] src1 pointer of the first input vector
 * @param[in] len1 length of the first input vector
 * @param[in] src2 pointer of the second input vector
 * @param[in] len2 length of the second input vector
 * @param[out] dst pointer of the output vector
 * @return none.
 */
void riscv_dsp_corr_q15(q15_t *src1, uint32_t len1,
                    q15_t *src2, uint32_t len2, q15_t *dst);

/**
 * @brief Convolution of the q31 vectors.
 * @param[in] src1 pointer of the first input vector
 * @param[in] len1 length of the first input vector
 * @param[in] src2 pointer of the second input vector
 * @param[in] len2 length of the second input vector
 * @param[out] dst pointer of the output vector
 * @return none.
 */
void riscv_dsp_corr_q31(q31_t *src1, uint32_t len1,
                    q31_t *src2, uint32_t len2, q31_t *dst);

/**
 * @brief Correlation of the q7 vectors.
 * @param[in] src1 pointer of the first input vector
 * @param[in] len1 length of the first input vector
 * @param[in] src2 pointer of the second input vector
 * @param[in] len2 length of the second input vector
 * @param[out] dst pointer of the output vector
 * @return none.
 */
void riscv_dsp_corr_q7(q7_t *src1, uint32_t len1,
                   q7_t *src2, uint32_t len2, q7_t *dst);
/** @} corr */

// Biquad Filter
/**
 * @defgroup bq_df1 Biquadratic Cascade Filter (Direct Form I) Functions
 * @brief Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * Biquadratic cascade filter functions provide biquadratic filters. Andes DSP library supports
 * direct form I biquadratic cascade filters using the following equation:
 *
 * @image html bq_df1.gif ""
 *
 * In the equation, 5 coefficients and 4 state variables are used per stage. The coefficients b0, b1
 * and b2 determine zeros; the coefficients a1 and a2 determine the positions of poles.
 *
 * Andes DSP library supports distinct direct form I biquadratic cascade filter functions and
 * instance structures for floating-point, Q31, Q15 and also 32x64 Q31 to meet high precision.
 * Instance structures are used to store information like stages and coefficients, and save the state
 * in each stage. Before you use biquadratic cascade filter functions, make sure you initialize the
 * corresponding instance structures first. These functions and their instance structures are
 * introduced in the subsections below.
 */

/**
 * @addtogroup bq_df1
 * @{
 */

/**
 * @brief Instance structure of the floating-point biquadratic cascade filter.
 */

typedef struct
{
    uint32_t nstage;    /**< stages in the filter */
    float32_t *state;   /**< pointer of the state vector whose size is 4*nstage */
    float32_t *coeff;   /**< pointer of the coefficient vector whose size is 5*nstage */
} riscv_dsp_bq_df1_f32_t;

/**
 * @brief Structure for the q15 Biquad Filter.
 */
typedef struct
{
    uint32_t nstage;    /**< stages in the filter */
    q15_t *state;       /**< pointer of the state vector whose size is 4*nstage */
    q15_t *coeff;       /**< pointer of the coefficient vector whose size is 5*nstage */
    int8_t shift;       /**< shift bits */
} riscv_dsp_bq_df1_q15_t;

/**
 * @brief Structure for the q31 Biquad Filter.
 */
typedef struct
{
    uint32_t nstage;    /**< stages in the filter */
    q31_t *state;       /**< pointer of the state vector whose size is 4*nstage */
    q31_t *coeff;       /**< pointer of the coefficient vector whose size is 5*nstage */
    int8_t shift;       /**< shift bits */
} riscv_dsp_bq_df1_q31_t;

/**
 * @brief instance structure of the 32x64 Q31 biquadratic cascade filter
 */
typedef struct
{
    uint32_t nstage;    /**< stages in the filter */
    q63_t *state;       /**< pointer of the state vector whose size is 4*nstage */
    q31_t *coeff;       /**< pointer of the coefficient vector whose size is 5*nstage */
    int8_t shift;       /**< shift bits */
} riscv_dsp_bq_df1_32x64_q31_t;

/**
 * @brief Floating-point Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define nstage 3
 *      \#define size 6
 *      float32_t state[4 * nstage] = {0.0};
 *      float32_t coeff[5 * nstage] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06,
 *           0.28, -0.04, -0.20, 0.08, 0.40, 0.60, -1.00, -0.14};
 *      float32_t src[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *      float32_t dst[size];
 *      riscv_dsp_bq_df1_f32_t instance = {nstage, state, coeff};
 *      riscv_dsp_bq_df1_f32(&instance, src, dst, size);
 *     </pre>
 */
void riscv_dsp_bq_df1_f32(const riscv_dsp_bq_df1_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Q31 Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define shift 0
 *      \#define nstage 3
 *      \#define size 6
 *      q31_t state_q31[4 * nstage] = {0} ;
 *      q31_t coeff_q31[5 * nstage], src_q31[size], dst_q31[size];
 *      riscv_dsp_bq_df1_q31_t instance_q31 = {nstage, state_q31, coeff_q31, shift};
 *      float32_t coeff_f32[5 * nstage] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06,
 *      0.28, -0.04, -0.20, 0.08, 0.40, 0.60, -1.00, -0.14};
 *      float32_t src_f32[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *      riscv_dsp_convert_f32_q31(coeff_f32, coeff_q31, 5 * nstage);
 *      riscv_dsp_convert_f32_q31(src_f32, src_q31, size);
 *      riscv_dsp_bq_df1_q31(&instance_q31, src_q31, dst_q31, size);
 *
 * This Q31 biquadratic cascade filter function example uses the
 * riscv_dsp_convert_f32_q31 function to convert the F32 vector to Q31. You may also
 * reference this example for the Q15 biquadratic cascade filter function, but make sure you
 * replace riscv_dsp_convert_f32_q31 with riscv_dsp_convert_f32_q15.
 *     </pre>
 */
void riscv_dsp_bq_df1_q31(const riscv_dsp_bq_df1_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Q31 Fast Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Note
 *
 * When compared with riscv_dsp_bq_df1_q31, this function delivers higher performance
 * with lower precision by using the 32-bit accumulator.
 */
void riscv_dsp_bq_df1_fast_q31(const riscv_dsp_bq_df1_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size);
/**
 * @brief Q15 Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Note
 *
 * The coefficient vector stores coefficients in the order {b0, b1, b2, a1, a2, …}. The first 5
 * elements are for the first stage, the next 5 are for the next stage and so on. If using DSP
 * extension, the order turns to be {b0, 0, b1, b2, a1, a2, …} in which one zero (0) element is
 * inserted between b0 and b1 for SIMD load/store instruction. In this case, the first 6
 * elements are for the first stage, the next 6 are for the next stage and so on.
 */
void riscv_dsp_bq_df1_q15(const riscv_dsp_bq_df1_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Q15 Fast Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Note
 *
 * 1. The coefficient vector stores coefficients in the order {b0, b1, b2, a1, a2, …}. The first 5
 * elements are for the first stage, the next 5 are for the next stage and so on. If using DSP
 * extension, the order turns to be {b0, 0, b1, b2, a1, a2, …} in which one zero (0) element
 * is inserted between b0 and b1 for SIMD load/store instruction. In this case, the first 6
 * elements are for the first stage, the next 6 are for the next stage and so on.
 *
 * 2. When compared with riscv_dsp_bq_df1_q15, this function delivers higher
 * performance.
 */
void riscv_dsp_bq_df1_fast_q15(const riscv_dsp_bq_df1_q15_t *instance, q15_t *src, q15_t *dst, uint32_t size);
/**
 * @brief Q31 Fast Biquadratic Cascade Filter (Direct Form I) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Note
 *
 * This function stores and updates intermediate results with a 64-bit state buffer that
 * calculates accumulated values. As a result, it produces results of higher accuracy than those
 * of the function riscv_dsp_bq_df1_q31.
 */
void riscv_dsp_bq_df1_32x64_q31(const riscv_dsp_bq_df1_32x64_q31_t *instance, q31_t *src, q31_t *dst, uint32_t size);
/** @} bq_df1 */

/**
 * @defgroup bq_df2 Biquadratic Cascade Filter (Direct Form II Transposed) Functions
 * @brief Biquadratic Cascade Filter (Direct Form II Transposed) Functions
 *
 * Biquadratic cascade filter functions provide biquadratic filters. Andes DSP library supports
 * direct form II transposed biquadratic cascade filters using the following equation:
 *
 * @image html bq_df2.gif ""
 *
 * In the equation, 5 coefficients (b0, b1, b2, a1 and a2) and 2 state variables (d0 and d1) are used
 * per stage. After the completion of each stage, the 2 state variables d0 and d1 are saved into a
 * state vector.
 *
 * Andes DSP library also supports a direct form II transposed biquadratic cascade filter for stereo
 * (2 channels) signals. Its equation is as below:
 *
 * @image html bq_df2_2.gif ""
 *
 * In this equation, 5 coefficients (b0, b1, b2, a1 and a2) and 4 state variables (d00, d01, d10 and
 * d11) are used per stage. After the completion of each stage, the 4 state variables are saved into a
 * state vector.
 *
 * Andes DSP library supports distinct direct form II transposed biquadratic cascade filter
 * functions and instance structures only for floating-point data types, F32 and F64. Instance
 * structures are used to store information like stages and coefficients, and save the state in each
 * stage. Before you use biquadratic cascade filter functions, make sure you initialize the
 * corresponding instance structures first. These functions and their instance structures are
 * introduced in the subsections below.
 */

/**
 * @addtogroup bq_df2
 * @{
 */

/**
 * @brief Structure of the F32 biquadratic cascade filter.
 */
typedef struct
{
    uint32_t nstage;    /**< stages in the filter. */
    float32_t *state;   /**< pointer of the state vector whose size is 2*nstage. */
    float32_t *coeff;   /**< pointer of the coefficient vector whose size is 5*nstage. */
} riscv_dsp_bq_df2T_f32_t;

/**
 * @brief structure of the F32 biquadratic cascade filter.
 */
typedef struct
{
    uint32_t nstage;    /**< stages in the filter. */
    float32_t *state;   /**< pointer of the state vector whose size is 4*nstage. */
    float32_t *coeff;   /**< pointer of the coefficient vector whose size is 5*nstage. */
} riscv_dsp_bq_stereo_df2T_f32_t;

/**
 * @brief instance structure of the F64 biquadratic cascade filter.
 */
typedef struct
{
    uint32_t nstage;    /**< stages in the filter */
    float64_t *state;   /**< pointer of the state vector whose size is 2*nstage */
    float64_t *coeff;   /**< pointer of the coefficient vector whose size is 5*nstage */
} riscv_dsp_bq_df2T_f64_t;

/**
 * @brief Floating-point Biquadratic Cascade Filter (Direct Form II) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define nstage 3
 *      \#define size 6
 *      float32_t state[2 * nstage] = {0.0};
 *      float32_t coeff[5 * nstage] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06,
 *      0.28, -0.04, -0.20, 0.08, 0.40, 0.60, -1.00, -0.14};
 *      float32_t src[size] = {1.0, 0.5, 0.4, -0.1, -0.1, 0.3};
 *      float32_t dst[size];
 *      riscv_dsp_bq_df2T_f32_t instance = {nstage, state, coeff};
 *      riscv_dsp_bq_df2T_f32(&instance, src, dst, size);
 *
 * This example also serves as a reference for examples of the F64 direct form II transposed
 * biquadratic cascade filter function.
 *     </pre>
 */
void riscv_dsp_bq_df2T_f32(const riscv_dsp_bq_df2T_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief F64 Biquadratic Cascade Filter (Direct Form II) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 */
void riscv_dsp_bq_df2T_f64(const riscv_dsp_bq_df2T_f64_t *instance, float64_t *src, float64_t *dst, uint32_t size);
/**
 * @brief Floating-point Biquadratic Cascade Filter (Direct Form II) Functions
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples in each stage
 * @return none.
 *
 * @b Example
 *     <pre>
 *      \#define nstage 3
 *      \#define size 1024
 *      float32_t state[4 * nstage] = {0.0};
 *      float32_t coeff[5 * nstage] = {0.40, 0.10, 0.24, -0.40, -0.34, 0.20, 0.06,
 *                                     0.28, -0.04, -0.20, 0.08, 0.40, 0.60, -1.00, -0.14};
 *      float32_t src[size * 2] = {…};
 *      float32_t dst[size * 2];
 *      riscv_dsp_bq_stereo_df2T_f32_t inst = {nstage, state, coeff};
 *      riscv_dsp_bq_stereo_df2T_f32(&inst, src, dst, size);
 *     </pre>
 */
void riscv_dsp_bq_stereo_df2T_f32(const riscv_dsp_bq_stereo_df2T_f32_t *instance, float32_t *src, float32_t *dst, uint32_t size);
/** @} bq_df2 */

// Lattice IIR filter
/**
 * @defgroup liir Lattice Infinite Impulse Response (IIR) Filter Function
 *
 * @brief Lattice IIR Filter Function
 *
 * The lattice infinite impulse response (IIR) filter function can be represented as follows:
 *
 * @image html liir.gif ""
 *
 * Where N is the number of states and z = N, N-1, ... , 1, coefficients k = {rN, rN-1, ....r1},
 * vN is the ladder coefficients and u is the state.
 *
 * Andes DSP library supports distinct lattice IIR filter functions and instance structures for the
 * following data types: floating-point, Q31 and Q15. Before you use lattice IIR filter functions,
 * make sure you initialize the corresponding instance structure first. These functions and their
 * instance structures are introduced in the subsections below.
 */

/**
 * @addtogroup liir
 * @{
 */
/**
 * @brief Structure for the floating-point lattice IIR filter.
 */
typedef struct
{
    uint32_t nstage;    /**< number of filter stage. */
    float32_t *state;   /**< pointer of the state vector whose size is <code>nstage + size</code>. */
    float32_t *rcoeff;  /**< pointer of the time reversed reflection coefficient vector whose size is
                         <code>nstage</code>. */
    float32_t *lcoeff;  /**< pointer of the time reversed ladder coefficient vector whose size is
                         <code>nstage + 1</code>. */
} riscv_dsp_liir_f32_t;

/**
 * @brief Structure for the q31 lattice IIR filter.
 */
typedef struct
{
    uint32_t nstage;    /**< number of filter stage. */
    q31_t *state;       /**< pointer of the state vector whose size is <code>nstage + size</code>. */
    q31_t *rcoeff;      /**< pointer of the time reversed reflection coefficient vector whose size is
                             <code>nstage</code>. */
    q31_t *lcoeff;      /**< pointer of the time reversed ladder coefficient vector whose size is
                             <code>nstage + 1</code>. */
} riscv_dsp_liir_q31_t;

/**
 * @brief Structure for the q15 lattice IIR filter.
 */
typedef struct
{
    uint32_t nstage;    /**< number of filter stage. */
    q15_t *state;       /**< pointer of the state vector whose size is <code>nstage + size</code>. */
    q15_t *rcoeff;      /**< pointer of the time reversed reflection coefficient vector whose size is
                             <code>nstage</code>. */
    q15_t *lcoeff;      /**< pointer of the time reversed ladder coefficient vector whose size is
                             <code>nstage + 1</code>. */
} riscv_dsp_liir_q15_t;

/**
 * @brief Floating-point lattice IIR filter
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Example
 *     <pre>
 *      \#define size    8
 *      \#define nstage  6
 *      float32_t state[nstage + size];
 *      float32_t rcoeff[nstage] = {…};
 *      float32_t lcoeff[nstage + 1] = {…};
 *      riscv_dsp_liir_f32_t inst = {nstage, state, rcoeff, lcoeff}.
 *      float32_t src[size] = {…};
 *      float32_t dst[size];
 *      riscv_dsp_liir_f32(&inst, src, dst, size);
 *
 * This example also serves as a reference for examples of Q31, fast_Q31, Q15 or fast_Q15
 * lattice IIR filter functions.
 *     </pre>
 */
void riscv_dsp_liir_f32(const riscv_dsp_liir_f32_t *instance, float32_t *src,
                    float32_t *dst, uint32_t size);
/**
 * @brief q31 lattice IIR filter
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * When compared with riscv_dsp_liir_fast_q31, this function delivers smaller code size
 * with lower performance and the output has the same accuracy.
 */
void riscv_dsp_liir_q31(const riscv_dsp_liir_q31_t *instance, q31_t *src,
                    q31_t *dst, uint32_t size);
/**
 * @brief q31 fastt lattice IIR filter
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * When compared with riscv_dsp_liir_q31, this function delivers higher performance
 * with larger code size and the output has the same accuracy. In order to avoid overflow, the
 * reflection coefficient must be scaled down by 2.
 */
void riscv_dsp_liir_fast_q31(const riscv_dsp_liir_q31_t *instance, q31_t *src,
                    q31_t *dst, uint32_t size);
/**
 * @brief q15 lattice IIR filter
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * When compared with riscv_dsp_liir_fast_q15, this function delivers smaller code size
 * with lower performance and the output has the same accuracy.
 */
void riscv_dsp_liir_q15(const riscv_dsp_liir_q15_t *instance, q15_t *src,
                    q15_t *dst, uint32_t size);
/**
 * @brief q15 fast lattice IIR filter
 *
 * @param[in] instance pointer of the instance structure
 * @param[in] src pointer of the input vector
 * @param[out] dst pointer of the output vector
 * @param[in] size number of samples
 *
 * @b Note:
 *
 * When compared with riscv_dsp_liir_q15, this function delivers higher performance
 * with larger code size and the output has the same accuracy.
 */
void riscv_dsp_liir_fast_q15(const riscv_dsp_liir_q15_t *instance, q15_t *src,
                    q15_t *dst, uint32_t size);

/** @} liir */

/** @} filter */

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_FILTERING_MATH_H__
