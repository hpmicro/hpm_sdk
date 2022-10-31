/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef FFA_TEST_DATA_H
#define FFA_TEST_DATA_H

#include "hpm_ffa_drv.h"
#include "hpm_common.h"

ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_16_point_src[16*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_16_point_result[16*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_32_point_src[32*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_32_point_result[32*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_64_point_src[64*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_64_point_result[64*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_256_point_src[256*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_256_point_result[256*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_512_point_src[512*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fft_complex_q31_512_point_result[512*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_ifft_complex_q31_8_point_src[8*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_ifft_complex_q31_8_point_result[8*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_ifft_complex_q31_16_point_src[16*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_ifft_complex_q31_16_point_result[16*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_ifft_complex_q31_32_point_src[32*2];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_ifft_complex_q31_32_point_result[32*2];

ATTR_ALIGN(64) extern const ffa_q31_t ffa_fir_q31_46_taps_src[46];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fir_q31_17_taps_coeff[17];
ATTR_ALIGN(64) extern const ffa_q31_t ffa_fir_q31_coeff_17taps_src48taps_result[30];



#endif /* FFA_TEST_DATA_H */
