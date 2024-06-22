/***************************************************************************
 * internal_nds_types.h                                                   *
 *                                                                         *
 * Copyright (C) 2012-2016 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

#ifndef __INTERNAL_NDS_TYPES_H__
#define __INTERNAL_NDS_TYPES_H__

//#include "ndsv_types.h"

#define NDSV_VLEN           512                             //bits of vector regiter
#define NDSV_VLEN_BYTE      (NDSV_VLEN>>3)                  //bytes of vector regiter
#define NDSV_VELEM_NUM_E8   (NDSV_VLEN_BYTE)
#define NDSV_VELEM_NUM_E16  (NDSV_VLEN_BYTE>>1)
#define NDSV_VELEM_NUM_E32  (NDSV_VLEN_BYTE>>2)
#define NDSV_VELEM_NUM_E64  (NDSV_VLEN_BYTE>>3)

//Temp. MACROs will be removed later. Base is 64-bit vector
#define TEMP_BASE_ELEM_SH_E8    3
#define TEMP_BASE_ELEM_SH_E16   2
#define TEMP_BASE_ELEM_SH_E32   1
#define TEMP_BASE_ELEM_SH_E64   0

#if NDSV_VLEN == 64
#define TEMP_SH_ADD             0
#elif NDSV_VLEN == 128
#define TEMP_SH_ADD             1
#elif NDSV_VLEN == 256
#define TEMP_SH_ADD             2
#elif NDSV_VLEN == 512
#define TEMP_SH_ADD             3
#elif NDSV_VLEN == 1024
#define TEMP_SH_ADD             4
#elif NDSV_VLEN == 2048
#define TEMP_SH_ADD             5
#elif NDSV_VLEN == 4096
#define TEMP_SH_ADD             6
#else
#define TEMP_SH_ADD             0   //Default is 64 bits
#endif

#define NDSV_VELEM_SH_E8        (TEMP_BASE_ELEM_SH_E8  + TEMP_SH_ADD)
#define NDSV_VELEM_SH_E16       (TEMP_BASE_ELEM_SH_E16 + TEMP_SH_ADD)
#define NDSV_VELEM_SH_E32       (TEMP_BASE_ELEM_SH_E32 + TEMP_SH_ADD)
#define NDSV_VELEM_SH_E64       (TEMP_BASE_ELEM_SH_E64 + TEMP_SH_ADD)

//Remove temp. MACROs.
//#undef TEMP_BASE_ELEM_SH_E8
//#undef TEMP_BASE_ELEM_SH_E16
//#undef TEMP_BASE_ELEM_SH_E32
//#undef TEMP_BASE_ELEM_SH_E64
//#undef TEMP_SH_ADD

typedef int8_t              s8_t;
typedef uint8_t             u8_t;
typedef int16_t             s16_t;
typedef uint16_t            u16_t;
typedef int32_t             s32_t;
typedef uint32_t            u32_t;
typedef int64_t             s64_t;
typedef uint64_t            u64_t;
typedef float               f32_t;
typedef double              f64_t;
typedef long                long_t;
typedef unsigned long       ulong_t;
typedef long long           llong_t;
typedef unsigned long long  ullong_t;

typedef int8_t   vec_s8_t  __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef uint8_t  vec_u8_t  __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef int16_t  vec_s16_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef uint16_t vec_u16_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef int32_t  vec_s32_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef uint32_t vec_u32_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef int64_t  vec_s64_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef uint64_t vec_u64_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));

typedef float    vec_f32_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));
typedef double   vec_f64_t __attribute__ ((__vector_size__ (NDSV_VLEN_BYTE) ));

//Signed 32-bit union type for SIMD data.
typedef union
{
    s32_t w0;
    u32_t u0;
    f32_t f0;
    long_t l0;
#if (defined(NDS32_EB) || defined(__NDS32_EB__))
    //big endian
    struct
    {
        s16_t h1;
        s16_t h0;
    } b16;
    struct
    {
        s8_t b3;
        s8_t b2;
        s8_t b1;
        s8_t b0;
    } b8;
#else
    //little endian
    struct
    {
        s16_t h0;
        s16_t h1;
    } b16;
    struct
    {
        s8_t b0;
        s8_t b1;
        s8_t b2;
        s8_t b3;
    } b8;
#endif
} union32_t;

//Unsigned 32-bit union type for SIMD data.
typedef union
{
    u32_t w0;
    ulong_t l0;
#if (defined(NDS32_EB) || defined(__NDS32_EB__))
    //big endian
    struct
    {
        u16_t h1;
        u16_t h0;
    } b16;
    struct
    {
        u8_t b3;
        u8_t b2;
        u8_t b1;
        u8_t b0;
    } b8;
#else
    //little endian
    struct
    {
        u16_t h0;
        u16_t h1;
    } b16;
    struct
    {
        u8_t b0;
        u8_t b1;
        u8_t b2;
        u8_t b3;
    } b8;
#endif
} union32u_t;

//Signed 64-bit union type for SIMD data.
typedef union
{
    s64_t d0;
    long_t l0;

#if ENA_DSP_BE
    //big endian
    struct
    {
        s32_t w1;
        union{
            u32_t uw0;
            s32_t w0;
        };
    } b32;
    struct
    {
        s16_t h3;
        s16_t h2;
        s16_t h1;
        s16_t h0;
    } b16;
    struct
    {
        s8_t b7;
        s8_t b6;
        s8_t b5;
        s8_t b4;
        s8_t b3;
        s8_t b2;
        s8_t b1;
        s8_t b0;
    } b8;
#else
    //little endian
    struct
    {
        union{
            u32_t uw0;
            s32_t w0;
        };
        s32_t w1;
    } b32;
    struct
    {
        s16_t h0;
        s16_t h1;
        s16_t h2;
        s16_t h3;
    } b16;
    struct
    {
        s8_t b0;
        s8_t b1;
        s8_t b2;
        s8_t b3;
        s8_t b4;
        s8_t b5;
        s8_t b6;
        s8_t b7;
    } b8;
#endif
} union64_t;

//Unsigned 64-bit union type for SIMD data.
typedef union
{
    u64_t d0;
    ulong_t l0;

#if ENA_DSP_BE
    //big endian
    struct
    {
        u32_t w1;
        u32_t w0;
    } b32;
    struct
    {
        u16_t h3;
        u16_t h2;
        u16_t h1;
        u16_t h0;
    } b16;
    struct
    {
        u8_t b7;
        u8_t b6;
        u8_t b5;
        u8_t b4;
        u8_t b3;
        u8_t b2;
        u8_t b1;
        u8_t b0;
    } b8;
#else
    //little endian
    struct
    {
        u32_t w0;
        u32_t w1;
    } b32;
    struct
    {
        u16_t h0;
        u16_t h1;
        u16_t h2;
        u16_t h3;
    } b16;
    struct
    {
        u8_t b0;
        u8_t b1;
        u8_t b2;
        u8_t b3;
        u8_t b4;
        u8_t b5;
        u8_t b6;
        u8_t b7;
    } b8;
#endif
} union64u_t;

#ifdef NDS_CPU_64
    typedef union64_t suni_t;
    typedef union64u_t uuni_t;
#else
    typedef union32_t suni_t;
    typedef union32u_t uuni_t;
#endif


#define NDSV_CHECK_DstSrc {}
#define NDSV_CHECK_DstSrcCst {}
#define NDSV_CHECK_DstSrc1Src2 {}
#define NDSV_CHECK_AccSrc1Src2 {}
#define NDSV_CHECK_AccSrcCst {}


#define NDSV_CHECK_P1(X)
#define NDSV_CHECK_P2(X, Y)
#define NDSV_CHECK_P3(X, Y, Z)
#define NDSV_CHECK_P4(X, Y, Z, W)

#endif // __INTERNAL_NDS_TYPES_H__
