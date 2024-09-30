/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RISCV_SIMD_CONVERT_H__
#define __RISCV_SIMD_CONVERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hpm_common.h"
#if defined(__iar_riscv_andes_dsp) && (__iar_riscv_andes_dsp)
#include "nds_intrinsic.h"
#else
#include "nds32_intrinsic.h"
#endif

#ifndef __STATIC_FORCEINLINE
    #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif

#define __PKHBT(_a, _b, _shift)     __nds__pkbb16(_b, _a) /**< Only available if shift is 16 */
#define __PKHTB(_a, _b, _shift)     __nds__pktt16(_a, _b) /**< Only available if shift is 16 */
#define __ROR(value, bits) ((value >> bits) | (value << (32 - bits)))
#define __SXTB16_ROR(_a, _b)  __nds__sunpkd831(_a)
#define __SXTB16(_a)  __nds__sunpkd820(_a)
#define __SADD16(_a, _b) __nds__kadd16(_a, _b)
#define __SMLAD(_a, _b, _c)    __nds__kmada(_c, _a, _b)

__STATIC_FORCEINLINE uint32_t __UXTB16(uint32_t x)
{
    return (x >> 8) | (x << 8);
}

__STATIC_FORCEINLINE uint32_t __SXTAB16(uint32_t x, uint32_t y)
{
    return ((uint32_t)((((((int32_t)y << 24) >> 24) + (((int32_t)x << 16) >> 16)) & (int32_t)0x0000FFFF) |
                       (((((int32_t)y <<  8) >>  8)  + (((int32_t)x >> 16) << 16)) & (int32_t)0xFFFF0000)));
}

#ifdef __cplusplus
}
#endif

#endif
