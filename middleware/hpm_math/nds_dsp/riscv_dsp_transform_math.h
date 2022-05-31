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
#ifndef __RISCV_DSP_TRANSFORM_MATH_H__
#define __RISCV_DSP_TRANSFORM_MATH_H__

/**
 * @defgroup transform Transform Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "riscv_dsp_math_types.h"

// CFFT/CIFFT-Radix2
/// Floating-point Radix-2 Complex FFT/IFFT Function
int32_t riscv_dsp_cfft_rd2_f32(float32_t *src, uint32_t m);
int32_t riscv_dsp_cifft_rd2_f32(float32_t *src, uint32_t m);

/// Q15 Radix-2 Complex FFT/IFFT Function
int32_t riscv_dsp_cfft_rd2_q15(q15_t *src, uint32_t m);
int32_t riscv_dsp_cifft_rd2_q15(q15_t *src, uint32_t m);

/// Q31 Radix-2 Complex FFT/IFFT Function
int32_t riscv_dsp_cfft_rd2_q31(q31_t *src, uint32_t m);
int32_t riscv_dsp_cifft_rd2_q31(q31_t *src, uint32_t m);

// CFFT/IFFT-Radix4
/// Floating-point Radix-4 Complex FFT/IFFT Function
int32_t riscv_dsp_cfft_rd4_f32(float32_t *src, uint32_t m);
int32_t riscv_dsp_cifft_rd4_f32(float32_t *src, uint32_t m);

/// Q15 Radix-4 Complex FFT/IFFT Function
int32_t riscv_dsp_cfft_rd4_q15(q15_t *src, uint32_t m);
int32_t riscv_dsp_cifft_rd4_q15(q15_t *src, uint32_t m);

/// Q31 Radix-4 Complex FFT/IFFT Function
int32_t riscv_dsp_cfft_rd4_q31(q31_t *src, uint32_t m);
int32_t riscv_dsp_cifft_rd4_q31(q31_t *src, uint32_t m);

// CFFT/CIFFT
/// Floating-point Complex FFT/IFFT Function
void riscv_dsp_cfft_f32(float32_t *src, uint32_t m);
void riscv_dsp_cifft_f32(float32_t *src, uint32_t m);

/// Q15 Complex FFT/IFFT Function
void riscv_dsp_cfft_q15(q15_t *src, uint32_t m);
void riscv_dsp_cifft_q15(q15_t *src, uint32_t m);

/// Q31 Complex FFT/IFFT Function
void riscv_dsp_cfft_q31(q31_t *src, uint32_t m);
void riscv_dsp_cifft_q31(q31_t *src, uint32_t m);

// Real-FFT
/// Floating-point Real FFT/IFFT Function
int32_t riscv_dsp_rfft_f32(float32_t *src, uint32_t m);
int32_t riscv_dsp_rifft_f32(float32_t *src, uint32_t m);

/// Q15 Real FFT/IFFT Function
int32_t riscv_dsp_rfft_q15(q15_t *src, uint32_t m);
int32_t riscv_dsp_rifft_q15(q15_t *src, uint32_t m);

/// Q31 Real FFT/IFFT Function
int32_t riscv_dsp_rfft_q31(q31_t *src, uint32_t m);
int32_t riscv_dsp_rifft_q31(q31_t *src, uint32_t m);

// DCT/IDCT Type II
/// Floating-point DCT/IDCT Type II Function
void riscv_dsp_dct_f32(float32_t *src, uint32_t m);
void riscv_dsp_idct_f32(float32_t *src, uint32_t m);

/// Q15 DCT/IDCT Type II Function
void riscv_dsp_dct_q15(q15_t *src, uint32_t m);
void riscv_dsp_idct_q15(q15_t *src, uint32_t m);

/// Q31 DCT/IDCT Type II Function
void riscv_dsp_dct_q31(q31_t *src, uint32_t m);
void riscv_dsp_idct_q31(q31_t *src, uint32_t m);

// DCT Type IV
/// Floating-point DCT/IDCT Type IV Function
void riscv_dsp_dct4_f32(float32_t *src, uint32_t m);
void riscv_dsp_idct4_f32(float32_t *src, uint32_t m);

/// Q15 DCT/IDCT Type IV Function
void riscv_dsp_dct4_q15(q15_t *src, uint32_t m);
void riscv_dsp_idct4_q15(q15_t *src, uint32_t m);

/// Q31 DCT/IDCT Type IV Function
void riscv_dsp_dct4_q31(q31_t *src, uint32_t m);
void riscv_dsp_idct4_q31(q31_t *src, uint32_t m);

#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP32_TRANSFORM_MATH_H__
