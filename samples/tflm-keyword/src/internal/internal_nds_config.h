/******************************************************************************
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.*
 * Copyright (C) 2018-2023 Andes Technology Corporation. All rights reserved. *
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


/**
 * @defgroup Andes toolchain configuration header file
 */

/**
 * This header file checks the configuration from toolchain and
 * defines following MACROs:
 *        ENA_NDS_TOOLCHAIN
 *        NDS_TOOLCHAIN_V3
 *        NDS_TOOLCHAIN_RISCV
 *        ENA_DSP_ISA
 *        ENA_DSP_ISA_V3
 *        ENA_DSP_ISA_V5
 *        ENA_DSP_ISA_32
 *        ENA_DSP_ISA_64
 *        ENA_DSP_BE
 *        ENA_DSP_ZOL
 *        SIM_DSP_ISA_WITH_C
 *        ENA_DBG_MSG
 *        ENA_DSP_MSG
 *        DBGMSG
 *
 * This file is included by internal_config.h.
 *
 */

#ifndef __INTERNAL_NDS_CONFIG_H__
#define __INTERNAL_NDS_CONFIG_H__

#ifdef  __cplusplus
extern "C"
{
#endif

//-----------------------------------------------//
// Toolchain and related definitions checking ...//
//-----------------------------------------------//

//TOOLCHAIN
#if defined(__NDS32__) || defined(__riscv)
    #define ENA_NDS_TOOLCHAIN

    #ifdef __NDS32__
        #define NDS_TOOLCHAIN_V3
        #define NDS_CPU_32
        #define NDS_REG_LEN             32
    #endif

    #ifdef __riscv
        #define NDS_TOOLCHAIN_RISCV
        #if __riscv_xlen == 64
            #define NDS_CPU_64
            #define NDS_REG_LEN         64
        #elif __riscv_xlen == 32
            #define NDS_CPU_32
            #define NDS_REG_LEN         32
        #else
            #error "[Error]The value of __riscv_xlen is not 32 nor 64!"
        #endif
    #endif
#else
    #define NDS_REG_LEN             32
#endif  //TOOLCHAIN

//DSP ISA
#if defined(ENA_DSP_ISA) || defined(__NDS32_EXT_DSP__) || defined(__riscv_dsp)
    #if defined(__NDS32_EXT_DSP__) && defined(__riscv_dsp)
        #error "[Error]Cannot support V3 and V5 DSP extension together!"
    #endif

    #ifndef ENA_DSP_ISA
        #define ENA_DSP_ISA
    #endif

    #ifdef __NDS32_EXT_DSP__    //DSP v3
        #define ENA_DSP_ISA_V3
        #define ENA_DSP_ISA_32
    #endif

    #ifdef __riscv_dsp          //DSP v5
        #define ENA_DSP_ISA_V5
        #define ENA_DSP_ISA2    //v5 always supports DSP_ISA2

        #if __riscv_xlen == 64
            #define ENA_DSP_ISA_64
        #elif __riscv_xlen == 32
            #define ENA_DSP_ISA_32
        #else
            #error "[Error]The value of __riscv_xlen is not 32 nor 64!"
        #endif
    #endif

    #define ENA_BIT_MANIP_EXT   //B extension is MUST to have in DSP
#endif  //DSP ISA

//RVV ISA
#ifdef __riscv_vector
    #ifndef ENA_VEC_ISA
        #define ENA_VEC_ISA
    #endif
#endif

//Little/Big endian
#if (defined(NDS32_EB) || defined(__NDS32_EB__))
    #define ENA_DSP_BE          1
#else
    #define ENA_DSP_BE          0
#endif

//ZOL
#ifdef __NDS32_EXT_ZOL__
    #define ENA_DSP_ZOL
#endif  //ZOL

//Andes V5 extension
//For toolchain with P ext but without Andes V5 extension (e.g., FFB).
#ifdef __nds_v5
    #define ENA_NDS_V5_EXT
#endif

#ifdef ENA_NDS_TOOLCHAIN
    //Sim with C ?

// #if defined(ENA_DSP_ISA_V5) && defined(ENA_DSP_ISA_64)
//     #include <nds_intrinsic.h>
// #else
//     #include <nds32_intrinsic.h>     //Baseline still needs intrinsic functions (e.g. abs).
// #endif

#endif

#ifdef ENA_DSP_ZOL
    #define FUNC_ATTR_NO_HWLOOP         __attribute__ ((no_ext_zol))
    #define LOOP_HINT_NO_HWLOOP()       __nds32__no_ext_zol()
    // Use for dup function for force generate zol code in -O3.
    #define FUNC_ATTR_FORCE_MEMCPY_ZOL  __attribute__ ((force_memcpy_zol))
#else
    #define FUNC_ATTR_NO_HWLOOP
    #define LOOP_HINT_NO_HWLOOP()
    #define FUNC_ATTR_FORCE_MEMCPY_ZOL
#endif

//FPU: SP
#if defined(__NDS32_EXT_FPU_SP__) || defined(__riscv_float_abi_single)
    #define ENA_NDS_FPU
    #define ENA_NDS_FPU_SP
#endif
//FPU: DP
#if defined(__NDS32_EXT_FPU_DP__) || defined(__riscv_float_abi_double)
    #define ENA_NDS_FPU
    #define ENA_NDS_FPU_DP
#endif
//FPU: HP (Not supported for Andes v3 now)
#if defined(ENA_NDS_FPU) && defined(__riscv_zfh)
    #define ENA_NDS_FPU_HP
#endif


//If using other toolchains w/ DSP (e.g. x86, linux gcc,..) instead of Andes toolchain:
#if defined(ENA_DSP_ISA) && !defined(ENA_NDS_TOOLCHAIN)
    #define ENA_DSP_ISA_V3                  //ENA_DSP_ISA_V3 or ENA_DSP_ISA_V5
    #define ENA_DSP_ISA_32                  //ENA_DSP_ISA_32 or ENA_DSP_ISA_64
    #define ENA_DSP_BE          0           //0 for little or 1 for big
    #define SIM_DSP_ISA_WITH_C              //Need to simulate DSP instructions

    //Enable FPU by default in x86/x86_64
    #define ENA_NDS_FPU
    #define ENA_NDS_FPU_SP
    #define ENA_NDS_FPU_DP
#endif

//-----------------------------------------------//
// Detection done.                               //
//-----------------------------------------------//

//Include header files
//#ifdef ENA_DSP_ISA
//#if defined(ENA_DSP_ISA_V5) && defined(ENA_DSP_ISA_64)
//    #include "internal_dsp_isa_rv64.h"
//#else
//    #include "internal_dsp_isa.h"
//#endif
//#else
//#ifdef NDS_TOOLCHAIN_V3
//    #include "internal_isa.h"
//#endif
//#endif

//-----------------------------------------------//
// Check MACROs:                                 //
//-----------------------------------------------//

#if defined(NDS_CPU_64) && defined(NDS_CPU_32)
    #error "[Error]Only one of MACRO, NDS_CPU_64 and NDS_CPU_32, can be defined!"
#endif

#ifdef NDS_CPU_64
    //Numbers of SIMD elements:
    #define SIMD_NUM_Q7         ((int)8)
    #define SIMD_NUM_Q15        ((int)4)
    #define SIMD_NUM_Q31        ((int)2)

    //Masks for rest elements:
    #define SIMD_NUM_MASK_Q7    ((int)(SIMD_NUM_Q7  - (int)1))
    #define SIMD_NUM_MASK_Q15   ((int)(SIMD_NUM_Q15 - (int)1))
    #define SIMD_NUM_MASK_Q31   ((int)(SIMD_NUM_Q31 - (int)1))

    //Shift amount for SIMD elements:
    #define SIMD_SA_Q7          ((int)3)
    #define SIMD_SA_Q15         ((int)2)
    #define SIMD_SA_Q31         ((int)1)
#elif defined(NDS_CPU_32)
    //Number of SIMD elements:
    #define SIMD_NUM_Q7         ((int)4)
    #define SIMD_NUM_Q15        ((int)2)

    //Masks for rest elements:
    #define SIMD_NUM_MASK_Q7    ((int)(SIMD_NUM_Q7  - (int)1))
    #define SIMD_NUM_MASK_Q15   ((int)(SIMD_NUM_Q15 - (int)1))

    //Shift amount for SIMD elements:
    #define SIMD_SA_Q7          ((int)2)
    #define SIMD_SA_Q15         ((int)1)
    #define SIMD_SA_Q31         ((int)0)
    //#else //Disable this error for x86 compilation.
    //#error "[Error]One of MACRO, NDS_CPU_64 and NDS_CPU_32, should be defined!"
#endif

#ifdef ENA_DBG_MSG
    #include "stdio.h"
    #define DBGMSG printf
    void dump_data_q15(q15_t *src, int size, char *disp_string);
#else //ENA_DBG_MSG
    #define DBGMSG(...)
    #define dump_data_q15(...)
#endif  //ENA_DBG_MSG

#ifdef ENA_DSP_MSG
    #include "stdio.h"
    #define DSPMSG printf
    void print16_2(q31_t x, q31_t y);
    #define P16_2(X,Y) DSPMSG("[%s] :", __FUNCTION__);print16_2(  X,Y)
#else   //ENA_DSP_MSG
    #define DSPMSG(...)
    #define print16_2(...)
    #define P16_2(...)
#endif  //ENA_DSP_MSG

//-----------------------------------------------//
//Show MACROs
//-----------------------------------------------//
//#define SHOW_NDS_CONFIG_MACRO

#ifdef SHOW_NDS_CONFIG_MACRO

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var ": "  VALUE(var)

#pragma message (VAR_NAME_VALUE(ENA_NDS_TOOLCHAIN))
#pragma message (VAR_NAME_VALUE(NDS_TOOLCHAIN_V3))
#pragma message (VAR_NAME_VALUE(NDS_TOOLCHAIN_RISCV))
#pragma message (VAR_NAME_VALUE(NDS_CPU_32))
#pragma message (VAR_NAME_VALUE(NDS_CPU_64))
#pragma message (VAR_NAME_VALUE(ENA_DSP_ISA))
#pragma message (VAR_NAME_VALUE(ENA_DSP_ISA_V3))
#pragma message (VAR_NAME_VALUE(ENA_DSP_ISA_V5))
#pragma message (VAR_NAME_VALUE(ENA_DSP_ISA_32))
#pragma message (VAR_NAME_VALUE(ENA_DSP_ISA_64))
#pragma message (VAR_NAME_VALUE(ENA_DSP_BE))
#pragma message (VAR_NAME_VALUE(ENA_DSP_ZOL))
#pragma message (VAR_NAME_VALUE(SIM_DSP_ISA_WITH_C))
#pragma message (VAR_NAME_VALUE(ENA_DBG_MSG))
#pragma message (VAR_NAME_VALUE(ENA_DSP_MSG))
#pragma message (VAR_NAME_VALUE(DBGMSG))
#pragma message (VAR_NAME_VALUE(SIMD_NUM_Q7))
#pragma message (VAR_NAME_VALUE(SIMD_NUM_Q15))
#pragma message (VAR_NAME_VALUE(SIMD_NUM_Q31))
#pragma message (VAR_NAME_VALUE(SIMD_NUM_MASK_Q7))
#pragma message (VAR_NAME_VALUE(SIMD_NUM_MASK_Q15))
#pragma message (VAR_NAME_VALUE(SIMD_NUM_MASK_Q31))
#pragma message (VAR_NAME_VALUE(SIMD_SA_Q7))
#pragma message (VAR_NAME_VALUE(SIMD_SA_Q15))
#pragma message (VAR_NAME_VALUE(SIMD_SA_Q31))
#pragma message (VAR_NAME_VALUE(ENA_NDS_FPU))
#pragma message (VAR_NAME_VALUE(ENA_NDS_FPU_SP))
#pragma message (VAR_NAME_VALUE(ENA_NDS_FPU_DP))

#endif

#ifdef  __cplusplus
}
#endif


#endif      //__INTERNAL_NDS_CONFIG_H__
