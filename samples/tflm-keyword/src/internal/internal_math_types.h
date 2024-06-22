#ifndef __INTERNAL_MATH_TYPES_H__
#define __INTERNAL_MATH_TYPES_H__
/***************************************************************************
 * internal_math_types.h                                                   *
 *                                                                         *
 * Copyright (C) 2012-2023 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

//#include "nds32_intrinsic.h"
// #include "nds32_math_types.h"       //for data types (q31_t, q15_t, ...)
#include "internal_nds_types.h"     //for union data types.

//RES_LOGN controls the resolution of cosine tables in transform functions.
//It also decide the value of RES_N is (1 << RES_LOGN)
#ifndef RES_LOGN
#define RES_LOGN    (10)            //default is 10
#endif

//RES_N is decided by RES_LOGN and it should not be defined manully.
#ifdef RES_N
#undef RES_N
#endif
#define RES_N       (1 << RES_LOGN)


//FFT_LOGN is idnetical with RES_LOGN in default configuration.
//
//If user wish to use lower resolution for larger samples transform,
//i.e. the samples in a frame is lager then the coeffients in cosine table.
//then FFT_LOGN should be set to RES_LOGN + 1
//(any value greater than RES_LOGN is OK, since we only check if FFT_LOGN > RES_LOGN).
//This leads lower precision but use smaller cosine table.
#ifndef FFT_LOGN
#define FFT_LOGN    RES_LOGN
#endif

//Only used by STATIC_VERSION
#define FFT_N (1 << FFT_LOGN)

//COS_RES_N controls the resolution of cosine tables in nds_cos_xx and nds_sin_xx functions.
//The value of COS_RES_N is identical with RES_N in default configuration.
//
//There is an exception case when user needs to calculate nds_cos_xx and
//nds32_sin_xx by input degree instead of redian. In this case, COS_RES_N should be set to 3600 manually.
//So both value between RES_N and COS_RES_N are different.
#ifndef COS_RES_N
#define COS_RES_N   RES_N
#endif

// Common Mathematical Symbols
#define TWO_PI      6.28318530717959f
#define PI          3.14159265358979f
#define HALF_PI     1.57079632679490f

// Radians Mapping
/**
 * @brief Radian mapping
 * radian [-2 * PI, 2 * PI) is mapped to [-1, 1)
 * degree [-180, 180) is mapped to [-1, 1)
 */

#ifdef FOUR_PI_RANGE
// radian [-2 * PI, 2 * PI) is mapped to [-1, 1)
// degree [-360, 360) is mapped to [-1, 1)
#define UNIT_Q31    (1.0f/2147483648.0f)
#define TWO_PI_Q31  INT32_MIN   // 0x80000000
#define PI_Q31      1073741824  // 0x40000000
#define HALF_PI_Q31 536870912   // 0x20000000

#define UNIT_Q15    (1.0f/32768.0f)
#define TWO_PI_Q15  INT16_MIN   // 0x8000
#define PI_Q15      16384       // 0x4000
#define HALF_PI_Q15 8192        // 0x2000

#define UNIT_Q7     (1.0f/128.0f)
#define TWO_PI_Q7   INT8_MIN    // 0x80
#define PI_Q7       64          // 0x40
#define HALF_PI_Q7  32          // 0x20

#else
// radian [-PI, PI) is mapped to [-1, 1)
// degree [-180, 180) is mapped to [-1, 1)
#define UNIT_Q31    (1.0f/2147483648.0f)
#define PI_Q31      INT32_MIN   // 0x80000000
#define HALF_PI_Q31 1073741824  // 0x40000000

#define UNIT_Q15    (1.0f/32768.0f)
#define PI_Q15      INT16_MIN   // 0x8000
#define HALF_PI_Q15 16384       // 0x4000

#define UNIT_Q7     (1.0f/128.0f)
#define PI_Q7       INT8_MIN    // 0x80
#define HALF_PI_Q7  64          // 0x40
#endif // FOUR_PI_RANGE

// Type Conversion
/**
 * nds_convert_(source type)_to_(destination type)
 *      Comput the element of the source type to destination type.
 */

/**
 * @brief convert_src_to_dst.
 * @param[in]   *src point to the input vector.
 * @param[out]  *dst point to the output vector.
 * @param[in]   size size of the vectors.
 * @return none.
 */
// floating-point to Q(destination type)
static inline q15_t convert_float_to_q15(float x)
{
    q31_t q31;
    q15_t q15;
    x *= 32768.0f;
    x += (x < 0.0f ? -0.5f : 0.5f);
    q31 = (q31_t)x;
    if (q31 >= 32768)
    {
        q15 = INT16_MAX;
    }
    else if (q31 <= -32768)
    {
        q15 = INT16_MIN;
    }
    else
    {
        q15 = (q15_t)q31;
    }
    return q15;
}

#if 0
#ifdef FOUR_PI_RANGE
// x must be in [-2 * PI, 2 * PI)
#define map_radian_float_to_q15(x)  convert_float_to_q15(x * (0.5f / PI))
#else
// x must be in [-PI, PI)
#define map_radian_float_to_q15(x)  convert_float_to_q15(x * (1.0f / PI))
// x must be in [-180, 180)
#define map_degree_float_to_q15(x)  convert_float_to_q15(x * (1.0f / 180.f))
#endif // FOUR_PI_RANGE
#endif // end of if 0

static inline q31_t convert_float_to_q31(float x)
{
    q63_t q63;
    q31_t q31;
    x *= 2147483648.0f;
    x += (x < 0.0f ? -0.5f : 0.5f);
    q63 = (q63_t)x;
    if (q63 >= 2147483648LL)
    {
        q31 = INT32_MAX;
    }
    else if (q63 <= -2147483648LL)
    {
        q31 = INT32_MIN;
    }
    else
    {
        q31 = (q31_t)q63;
    }

    return q31;
}

#if 0
#ifdef FOUR_PI_RANGE
// x must be in [-2 * PI, 2 * PI)
#define map_radian_float_to_q31(x)  convert_float_to_q31(x * (0.5f / PI))
#else
// x must be in [-PI, PI)
#define map_radian_float_to_q31(x)  convert_float_to_q31(x * (1.0f / PI))
// x must be in [-180, 180)
#define map_degree_float_to_q31(x)  convert_float_to_q31(x * (1.0f / 180.f))
#endif // FOUR_PI_RANGE
#endif // end of if 0

static inline q7_t convert_float_to_q7(float x)
{
    q31_t q31;
    q7_t q7;

    x *= 128.0f;
    x += (x < 0.0f ? -0.5f : 0.5f);
    q31 = (q31_t)x;
    if (q31 >= 128)
    {
        q7 = INT8_MAX;
    }
    else if (q31 <= -128)
    {
        q7 = INT8_MIN;
    }
    else
    {
        q7 = (q7_t)q31;
    }

    return q7;
}

// Q31 to destination type
static inline float convert_q31_to_float(q31_t x)
{
    return x * UNIT_Q31;
}

static inline q15_t convert_q31_to_q15(q31_t x)
{
    return (q15_t)(x >> 16);
}

static inline q7_t convert_q31_to_q7(q31_t x)
{
    return (q7_t)(x >> 24);
}

#if 0
#ifdef FOUR_PI_RANGE
static inline float map_radian_q31_to_float(q31_t x)
{
    return x * (2.0f * PI * UNIT_Q31);
}
#else
static inline float map_radian_q31_to_float(q31_t x)
{
    return x * (PI * UNIT_Q31);
}

static inline float map_degree_q31_to_float(q31_t x)
{
    return x * (180.f * UNIT_Q31);
}
#endif // FOUR_PI_RANGE
#endif // end of if 0

// Q15 to destination type
static inline float convert_q15_to_float(q15_t x)
{
    return x * UNIT_Q15;
}

static inline q31_t convert_q15_to_q31(q15_t x)
{
    return ((q31_t)x) << 16;
}

static inline q7_t convert_q15_to_q7(q15_t x)
{
    return (q7_t)(x >> 8);
}

#if 0
#ifdef FOUR_PI_RANGE
static inline float map_radian_q15_to_float(q15_t x)
{
    return x * (2.0f * PI * UNIT_Q15);
}
#else
static inline float map_radian_q15_to_float(q15_t x)
{
    return x * (PI * UNIT_Q15);
}

static inline float map_degree_q15_to_float(q15_t x)
{
    return x * (180.f * UNIT_Q15);
}
#endif // FOUR_PI_RANGE
#endif // end of if 0

// Q7 to destination type
static inline float convert_q7_to_float(q7_t x)
{
    return x * UNIT_Q7;
}

static inline q31_t convert_q7_to_q31(q7_t x)
{
    return ((q31_t)x) << 24;
}

static inline q15_t convert_q7_to_q15(q7_t x)
{
    return ((q15_t)x) << 8;
}

/**
 * nds_truncate_(source type)_to_(destination type);
 *      Truncates the source type values to destination type.
 */

/**
 * @brief truncate_src_to_dst
 * @param[in]   src input value.
 * @return output value after truncate.
 */

// Q63 to Q(destination type)
static inline q31_t nds_truncate_q63_to_q31(q63_t src)
{
#ifdef NDS_CPU_64
    return src != (q31_t)src ?
           ((INT32_MAX ^ ((q31_t) (src >> 63)))) : (q31_t) src;
#else
    return ((q31_t) (src >> 32) != ((q31_t) src >> 31)) ?
           ((INT32_MAX ^ ((q31_t) (src >> 63)))) : (q31_t) src;
#endif
}

#if 0
static inline q15_t nds_truncate_q63_to_q15(q63_t src)
{
    q31_t a = (src >> 32);
    q31_t b = (src >> 15);

    return (a == b) ? ((q15_t) src) : ((a >= 0) ? INT16_MAX : INT16_MIN);
}

static inline q7_t nds_truncate_q63_to_q7(q63_t src)
{
    q31_t a = (src >> 32);
    q31_t b = (src >> 7);

    return (a == b) ? ((q7_t) src) : ((a >= 0) ? INT8_MAX : INT8_MIN);
}

// Q31 to Q(destination type)
static inline q15_t nds_truncate_q31_to_q15(q31_t src)
{
    q15_t a = (src >> 16);
    q15_t b = (src >> 15);

    return (a == b) ? ((q15_t) src) : ((a >= 0) ? INT16_MAX : INT16_MIN);
}

static inline q7_t nds_truncate_q31_to_q7(q31_t src)
{
    q15_t a = (src >> 16);
    q15_t b = (src >> 7);

    return (a == b) ? ((q7_t) src) : ((a >= 0) ? INT8_MAX : INT8_MIN);
}

// Q15 to Q(destination type)
static inline q7_t nds_truncate_q15_to_q7(q15_t src)
{
    q15_t a = (src >> 8);
    q15_t b = (src >> 7);

    return (a == b) ? ((q7_t) src) : ((a >= 0) ? INT8_MAX : INT8_MIN);
}
#endif // end of if 0

/**
 * nds_saturate(q31_t src, uin32_t (destination_type))
 *      Comput the saturation value to the input data.
 */

/**
 * @brief saturate to dst_type.
 * @param[in]   src input value.
 * @param[in]   type destination type.
 * @return saturate value of the destination type.
 */

//only support 16u and 8u types.
//32u does need to saturate since the incoming type is q31_t.
#ifdef __NDS32_EXT_PERF__

//This function can not be decalred as inline function in -O0.
//The 2nd argument will be treated as variable instead of immediate value since it will not be inlined in -O0.
//So this function should be only as a MACRO for all compiler configuration.
#define nds_saturate(X, Y) (q31_t)__nds32__clips(X, Y - 1)

#else

static inline q31_t nds_saturate(q31_t src, const uint32_t type)
{
    q31_t max, min;

    if (type == 16u)
    {
        max = INT16_MAX;
        min = INT16_MIN;
    }
    else
    {
        max = INT8_MAX;
        min = INT8_MIN;
    }

    return (src > max) ? max : (src < min) ? min : src;
}

#endif // __NDS32_EXT_PERF__

//Follows are patterns for helping compiler to generate correspoding instructions

//Bit-wise rotate right
#define NDS_PTN_ROTR(val,  rsh) \
    ((((uint32_t)(val)) >> (rsh)) | ((uint32_t)(val) << (rsh)))


#endif // __INTERNAL_MATH_TYPES_H__
