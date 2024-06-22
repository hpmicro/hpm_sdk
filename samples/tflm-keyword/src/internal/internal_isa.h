/***************************************************************************
 * internal_isa.h                                                          *
 *                                                                         *
 * Copyright (C) 2012-2023 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup isa header file
 */

/**
 * This header file create a layer for Andes's instructions
 * excepts DSP ISA. In the other word, this file includes
 * basline, perf ext, perf ext 2, sat ext instructions. The DSP
 * ISA is located at internal_dsp_isa.h.
 */

#ifndef __INTERNAL_ISA_H__
#define __INTERNAL_ISA_H__

#ifdef  __cplusplus
extern "C"
{
#endif

// #include "nds32_math_types.h"
/**
 * Just for testing or debugging Saturation Extension
 * instructions.
 *
 * Define this flag can force to use C functions to simulate
 * Saturation Extension instructions, no matter if it is
 * presented.
 *
 * Note:
 * The performance or code size will be affected if this flag is
 * defined. If SIM_DSP_ISA_WITH_C is defined, the
 * SIM_SAT_ISA_WITH_C will be defined too.
 */
#ifdef SIM_DSP_ISA_WITH_C
#define SIM_SAT_ISA_WITH_C
#else
//#define SIM_SAT_ISA_WITH_C
#endif

/**
 * Just for testing or debugging Performance Extension
 * instructions.
 *
 * Define this flag can force to use C functions to simulate
 * Performance Extension instructions, no matter if it is
 * presented.
 *
 * Note:
 * The performance or code size will be affected if this flag is
 * defined. If SIM_DSP_ISA_WITH_C is defined, the
 * SIM_SAT_ISA_WITH_C will be defined too.
 */
#ifdef SIM_DSP_ISA_WITH_C
#define SIM_PE_ISA_WITH_C
#else
//#define SIM_PE_ISA_WITH_C
#endif

#if defined(SIM_DSP_ISA_WITH_C) || !defined(ENA_NDS_TOOLCHAIN) || defined(NDS_TOOLCHAIN_RISCV)
#define SIM_V3_BASELINE_ISA_WITH_C
#else
//#define SIM_V3_BASELINE_ISA_WITH_C
#endif

#define ISA_FUNC_PRFX           static inline

//IMM is just for labeling the operand/value is immediate.
#define IMM

//#if !defined(__NDS32_EXT_PERF__) || !defined(ENA_EXT_SAT_INST) || defined(SIM_SAT_ISA_WITH_C) || defined(SIM_PE_ISA_WITH_C)
#if !defined(ENA_EXT_SAT_INST) || defined(SIM_SAT_ISA_WITH_C)
#define SIM_SAT_OV_FLAG
#endif

//Un-defined this flag for performance consideration since it is not necessary to check it in libdsp now.
#undef SIM_SAT_OV_FLAG

#ifdef SIM_SAT_OV_FLAG
#ifdef ENA_DSP_ISA
#define nds_sat_ov_flag nds_dsp_ov_flag
extern int nds_sat_ov_flag;
#else
static int nds_sat_ov_flag;
#endif
#endif

//Baseline V1
#ifndef SIM_V3_BASELINE_ISA_WITH_C
#define NDS_ISA_SVA             __nds32__sva
#define NDS_ISA_SVS             __nds32__svs
#else
ISA_FUNC_PRFX uint32_t NDS_ISA_SVA(q31_t x, q31_t y)
{
    uint32_t ret = 0;

    const q31_t max_val = (q31_t)0x7FFFFFFF;
    const q31_t min_val = (q31_t)0x80000000;

    if(((y > 0) && (x > max_val - y)) || ((y < 0) && (x < min_val - y)))
        ret = 1;
    return ret;
}

ISA_FUNC_PRFX uint32_t NDS_ISA_SVS(q31_t x, q31_t y)
{
    uint32_t ret = 0;
    const q31_t max_val = (q31_t)0x7FFFFFFF;
    const q31_t min_val = (q31_t)0x80000000;

    if(((y > 0) && (x < min_val + y)) || ((y < 0) && (x > max_val + y)))
        ret = 1;

    return ret;
}
#endif

//Baseline V2
#define NDS_ISA_MULR64(X, Y)    ((uint64_t)X * Y)
#define NDS_ISA_MULSR64(X, Y)   ((q63_t)X * Y)


ISA_FUNC_PRFX uint32_t NDS_ISA_ABS(q31_t data)
{
    uint32_t ret;

#if defined(__NDS32_EXT_PERF__) && !defined(SIM_PE_ISA_WITH_C)  //V2 and bsp320 V3 has no __nds32__abs() delclaration but bsp400 has (4.4.4 vs. 4.8.2)
#if !(( __GNUC__ == 4) && (__GNUC_MINOR__ == 4))
    ret = __nds32__abs(data);   //for bsp400 and later
#else
#include <stdlib.h>             //for abs() when using bsp320 v3 and v2
    ret = abs(data);
#endif
#elif defined(ENA_DSP_ISA_V5)
    ret = __nds__kabsw(data);   //for bsp500 and later
#else
    ret = (data >= 0) ? data : ((data == INT32_MIN) ? INT32_MAX : -data);
#endif
    return ret;
}

#if defined(__NDS32_EXT_PERF__) && !defined(SIM_PE_ISA_WITH_C)

#define NDS_ISA_CLZ         __nds32__clz
#define NDS_ISA_CLO         __nds32__clo

//CLIP/CLIPS: from bit 0 to bit 31.
#define NDS_ISA_CLIP        __nds32__clip
#define NDS_ISA_CLIPS       __nds32__clips

//Y: from 1 bit to 32 bits.
#define NDS_ISA_SATS(X, Y)      __nds32__clips((X), (Y - 1))
#define NDS_ISA_SAT(X, Y)       __nds32__clip((X), (Y))

#else   //defined(__NDS32_EXT_PERF__) && !defined(SIM_PE_ISA_WITH_C)

//ISA_FUNC_PRFX q31_t clip_q31_to_any(q31_t x, uint32_t y)
//{
//    int32_t posMax, negMin;
//    uint32_t i;
//
//    posMax = 1;
//    for (i = 0; i < (y - 1); i++)
//    {
//        posMax = posMax * 2;
//    }
//
//    if (x > 0)
//    {
//        posMax = (posMax - 1);
//
//        if (x > posMax)
//        {
//#ifdef SIM_SAT_OV_FLAG
//            nds_sat_ov_flag = 1;
//#endif
//
//            x = posMax;
//        }
//    }
//    else
//    {
//        negMin = -posMax;
//
//        if (x < negMin)
//        {
//#ifdef SIM_SAT_OV_FLAG
//            nds_sat_ov_flag = 1;
//#endif
//            x = negMin;
//        }
//    }
//    return (x);
//}

//ISA_FUNC_PRFX q31_t clip_u32_to_any(uint32_t x, uint32_t y)
//{
//    uint32_t posMax;
//    uint32_t ret;
//
//    posMax = (uint32_t)(((uint32_t)1 << (y - 1)) - 1);
//
//    if (x > posMax)
//    {
//#ifdef SIM_SAT_OV_FLAG
//        nds_sat_ov_flag = 1;
//#endif
//        ret = posMax;
//    }
//    else if ((int32_t)x < 0)
//    {
//#ifdef SIM_SAT_OV_FLAG
//        nds_sat_ov_flag = 1;
//#endif
//        ret = 0;
//    }
//    else
//    {
//        ret = x;
//    }
//    return ret;
//}

//bit_size: can be 1 to 32
static inline uint32_t sat_u32_to_any(q31_t src, const uint32_t bit_size)
{
    uint32_t ret;
    const q31_t max_val = (uint32_t)0xFFFFFFFF >> (32 - bit_size);

    ret = (src < 0) ? 0UL : (src > max_val) ? (uint32_t) max_val : (uint32_t) src;

#ifdef SIM_SAT_OV_FLAG
    if (ret != (uint32_t)src)
    {
        nds_sat_ov_flag = 1;
    }
#endif

    return ret;
}


//bit_size: can be 1 to 32
static inline q31_t sat_q31_to_any(q31_t src, const uint32_t bit_size)
{
    q31_t ret;
    const q31_t max_val = (q31_t)0x7FFFFFFF >> (32 - bit_size);
    const q31_t min_val = (q31_t)0x80000000 >> (32 - bit_size);

    ret = (src > max_val) ? max_val : (src < min_val) ? min_val : src;

#ifdef SIM_SAT_OV_FLAG
    if (ret != src)
    {
        nds_sat_ov_flag = 1;
    }
#endif

    return ret;
}


//the value of type should be 16 or 8.
//static inline q31_t sat_q31_to_q15_q8(q31_t src, const uint32_t type)
//{
//    q31_t type_max, type_min, ret;
//
//    if (type == 16U)
//    {
//        type_max = INT16_MAX;
//        type_min = INT16_MIN;
//    }
//    else
//    {
//        type_max = INT8_MAX;
//        type_min = INT8_MIN;
//    }
//
//    ret = (src > type_max) ? type_max : (src < type_min) ? type_min : src;
//
//#ifdef SIM_SAT_OV_FLAG
//    if (ret != src)
//    {
//        nds_sat_ov_flag = 1;
//    }
//#endif
//
//    return ret;
//}

//Calculate the value of log2(v)
static inline int32_t get_log2_32b(uint32_t v)
{
    uint32_t r, shift;

    r = (v > 0xFFFF) << 4;
    v >>= r;
    shift = (v > 0xFF) << 3;
    v >>= shift;
    r |= shift;
    shift = (v > 0xF) << 2;
    v >>= shift;
    r |= shift;
    shift = (v > 0x3) << 1;
    v >>= shift;
    r |= shift;
    r |= (v >> 1);

    return (int32_t)r;
}

//Get the index of Highest Bit for 32-bit value
static inline uint32_t get_highest_bit_32b(q31_t data)
{
    uint32_t idx = (uint32_t)data;
    if (idx != 0)
    {
        idx = get_log2_32b(idx) + 1;
    }
    return idx;
}

ISA_FUNC_PRFX uint32_t NDS_ISA_CLZ(q31_t data)
{
    return (uint32_t)32 - get_highest_bit_32b(data);
}

ISA_FUNC_PRFX uint32_t NDS_ISA_CLO(q31_t data)
{
    return (uint32_t)32 - get_highest_bit_32b(~data);
}

//Unsigned clipping for one 32-bit word at bit_idx, if ov set ov_flag.
//ISA_FUNC_PRFX uint32_t NDS_ISA_CLIP(uint32_t x, IMM uint8_t bit_idx)
//{
//    return sat_u32_to_any(x, bit_idx + 1);
//}
#define NDS_ISA_CLIP(X, Y)          sat_u32_to_any(X, (Y + 1))

//Signed clipping for one 32-bit word at bit_idx, if ov set ov_flag.
//ISA_FUNC_PRFX q31_t NDS_ISA_CLIPS(q31_t x, IMM uint8_t bit_idx)
//{
//    return sat_q31_to_any(x, bit_idx + 1);
//}
#define NDS_ISA_CLIPS(X, Y)         sat_q31_to_any(X, (Y + 1))

//Y: from 1 bit to 32 bits.
#define NDS_ISA_SATS(X, Y)          sat_q31_to_any(X, Y)
#define NDS_ISA_SAT(X, Y)           sat_u32_to_any(X, Y)

#endif  //defined(__NDS32_EXT_PERF__) && !defined(SIM_PE_ISA_WITH_C)

#if defined(ENA_EXT_SAT_INST) && !defined(SIM_SAT_ISA_WITH_C)

#define NDS_ISA_RDOV        (int32_t)__nds32__rdov
#define NDS_ISA_CLOV        __nds32__clrov
#define NDS_ISA_KADDW       __nds32__kaddw
#define NDS_ISA_KSUBW       __nds32__ksubw
#define NDS_ISA_KADDH       __nds32__kaddh
#define NDS_ISA_KSUBH       __nds32__ksubh
#define NDS_ISA_KDMBB       __nds32__kdmbb
#define NDS_ISA_KDMBT       __nds32__kdmbt
#define NDS_ISA_KDMTB       __nds32__kdmtb
#define NDS_ISA_KDMTT       __nds32__kdmtt
#define NDS_ISA_KHMBB       __nds32__khmbb
#define NDS_ISA_KHMBT       __nds32__khmbt
#define NDS_ISA_KHMTB       __nds32__khmtb
#define NDS_ISA_KHMTT       __nds32__khmtt
#define NDS_ISA_KSLRAW      __nds32__kslraw

#else   //defined(ENA_EXT_SAT_INST) && !defined(SIM_SAT_ISA_WITH_C)

ISA_FUNC_PRFX q31_t sat_double_mul(q15_t x, q15_t y)
{
    q31_t ret;
    q31_t x_by_y = (q31_t)x * y;
    ret = x_by_y << 1;
    if (x_by_y != (ret >> 1))
    {
#ifdef SIM_SAT_OV_FLAG
        nds_sat_ov_flag = 1;
#endif
        ret = (q31_t)0x7FFFFFFF;
    }
    return ret;

    //if (((q15_t)0x8000 != x) || ((q15_t)0x8000 != y))
    //{
    //    ret = ((q31_t)x * y) << 1;
    //}
    //else
    //{
    //    ret = (q31_t)0x7FFFFFFF;
    //}
    //return ret;
}

ISA_FUNC_PRFX q15_t sat_half_mul(q15_t aop, q15_t bop)
{
    q31_t ret = ((q31_t)aop * bop) >> 15;
    return NDS_ISA_SATS(ret, 16);

    //q15_t Rt;
    //if (((q15_t)0x8000 != aop) || ((q15_t)0x8000 != bop))
    //{
    //    Rt = (q15_t)(((q31_t)aop * bop) >> 15);
    //}
    //else
    //{
    //    Rt = (q15_t)0x7FFF;
    //}
    //return Rt;
}

//Instructions simulation
ISA_FUNC_PRFX int32_t NDS_ISA_RDOV(void)
{
#ifdef SIM_SAT_OV_FLAG
    return nds_sat_ov_flag;
#else
    return 0;
#endif
}

ISA_FUNC_PRFX void NDS_ISA_CLOV(void)
{
#ifdef SIM_SAT_OV_FLAG
    nds_sat_ov_flag = 0;
#endif
}

ISA_FUNC_PRFX q31_t NDS_ISA_KADDW(q31_t x, q31_t y)
{
    //return clip_q63_to_q31((q63_t)x + y);   //very slow

    q31_t ret = x + y;
    if ((NDS_ISA_SVA(x, y) != 0) )
    {
#ifdef SIM_SAT_OV_FLAG
        nds_sat_ov_flag = 1;
#endif
        ret = (x >> 31) ^ 0x7FFFFFFF;
    }
    return ret;
}

ISA_FUNC_PRFX q31_t NDS_ISA_KSUBW(q31_t x, q31_t y)
{
    //return clip_q63_to_q31((q63_t)x - y);

    q31_t ret = x - y;
    if ((NDS_ISA_SVS(x, y) != 0) )
    {
#ifdef SIM_SAT_OV_FLAG
        nds_sat_ov_flag = 1;
#endif
        ret = (x >> 31) ^ 0x7FFFFFFF;
    }
    return ret;
}

ISA_FUNC_PRFX q15_t NDS_ISA_KADDH(q15_t x, q15_t y)
{
    q31_t ret;

    ret = (q31_t)x + y;
    ret = NDS_ISA_SATS(ret, 16);

    return (q15_t)ret;
}

ISA_FUNC_PRFX q15_t NDS_ISA_KSUBH(q15_t x, q15_t y)
{
    q31_t ret;

    ret = (q31_t)x - y;
    ret = NDS_ISA_SATS(ret, 16);

    return (q15_t)ret;
}


ISA_FUNC_PRFX q31_t NDS_ISA_KDMBB(q31_t x, q31_t y)
{
    return sat_double_mul((q15_t)x, (q15_t)y);
}

ISA_FUNC_PRFX q31_t NDS_ISA_KDMBT(q31_t x, q31_t y)
{
    return sat_double_mul((q15_t)x, (q15_t)(y >> 16));
}

ISA_FUNC_PRFX q31_t NDS_ISA_KDMTB(q31_t x, q31_t y)
{
    return sat_double_mul((q15_t)(x >> 16), (q15_t)y);
}

ISA_FUNC_PRFX q31_t NDS_ISA_KDMTT(q31_t x, q31_t y)
{
    return sat_double_mul((q15_t)(x >> 16), (q15_t)(y >> 16));
}

ISA_FUNC_PRFX q15_t NDS_ISA_KHMBB(q31_t x, q31_t y)
{
    return sat_half_mul((q15_t)x, (q15_t)y);
}
ISA_FUNC_PRFX q15_t NDS_ISA_KHMBT(q31_t x, q31_t y)
{
    return sat_half_mul((q15_t)x, (q15_t)(y >> 16));
}
ISA_FUNC_PRFX q15_t NDS_ISA_KHMTB(q31_t x, q31_t y)
{
    return sat_half_mul((q15_t)(x >> 16), (q15_t)y);
}
ISA_FUNC_PRFX q15_t NDS_ISA_KHMTT(q31_t x, q31_t y)
{
    return sat_half_mul((q15_t)(x >> 16), (q15_t)(y >> 16));
}

ISA_FUNC_PRFX q31_t NDS_ISA_KSLRAW(q31_t x, int sh)
{
    if (sh < 0 )
    {
        return (x >> (-sh));
    }
    else
    {
        //q63_t ret = (q63_t)x << sh;   //64-bit is very slow
        //return clip_q63_to_q31(ret);

        q31_t ret = x << sh;
        q31_t rev_sh_val = ret >> sh;

        if (x != rev_sh_val)
        {
#ifdef SIM_SAT_OV_FLAG
            nds_sat_ov_flag = 1;
#endif
            ret = (x >> 31) ^ 0x7FFFFFFF;
        }

        return ret;
    }
}

#endif  //defined(ENA_EXT_SAT_INST) && !defined(SIM_SAT_ISA_WITH_C)





#ifdef  __cplusplus
}
#endif


#endif      //__INTERNAL_ISA_H__
