/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_tfa_soc_drv.h"
#ifdef __IASMRISCV__

    MODULE ?TFA_DRV
    SECTION `.TFA_DRV`:CODE:NOROOT(3)

    EXTERN CONST_2PI
    EXTERN RAD_TO_DEG_DBL
    EXTERN INT_TO_DEG_DBL

#ifdef __riscv_flen

common:
    fsw fa0, 0(a0)
    flw	ft1, 4(a0)
    flw	ft2, 8(a0)
    flw	ft3, 12(a0)
    flw	ft4, 16(a0)
    flw	ft5, 20(a0)
    fmul.s	ft1,ft1,ft2
    fmul.s	ft3,ft3,ft4
    fadd.s	ft1,ft1,ft3
    fadd.s	ft6,ft1,ft5
    lw t1, 24(a0)
    srli t1, t1, 31
    ret

/**
 * @brief Inversion value calculation.
 * @param[in]       src   input value
 *
 * @return Inversion value
 *
 */
    PUBLIC hpm_tfa_inv_f32

hpm_tfa_inv_f32:
    la a0, TFA_BASE_ADDR
    mv a1, ra
loopinv:
    call common
    bnez t1, loopinv
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief Square Root Calculations.
 * @param[in]       src   input value
 *
 * @return Sqrt value
 *
 */
    PUBLIC hpm_tfa_sqrt_f32

hpm_tfa_sqrt_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 32
    mv a1, ra
loopsqrt:
    call common
    bnez t1, loopsqrt
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief Sine value calculation.
 * @param[in]       src   input value
 *
 * @return Sin value
 *
 */
    PUBLIC hpm_tfa_sin_f32

hpm_tfa_sin_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 64
    mv a1, ra
loopsin:
    call common
    bnez t1, loopsin
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief Cosine value calculation.
 * @param[in]       src   input value
 *
 * @return cos value
 *
 */
    PUBLIC hpm_tfa_cos_f32

hpm_tfa_cos_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 96
    mv a1, ra
loopcos:
    call common
    bnez t1, loopcos
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief Power 2 value calculation.
 * @param[in]       src   input value
 *
 * @return  power value
 *
 */
    PUBLIC hpm_tfa_power2_f32

hpm_tfa_power2_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 128
    mv a1, ra
looppow:
    call common
    bnez t1, looppow
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief Logarithmic function value calculation.
 * @param[in]       src   input value
 *
 * @return log value
 *
 */
    PUBLIC hpm_tfa_log2_f32

hpm_tfa_log2_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 160
    mv a1, ra
looplog:
    call common
    bnez t1, looplog
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief ATAN value calculation.
 * @param[in]       src   input value
 *
 * @return Atan value
 *
 */
    PUBLIC hpm_tfa_atan_f32

hpm_tfa_atan_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 192
    mv a1, ra
loopatan:
    call common
    bnez t1, loopatan
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief INVSQRT value calculation.
 * @param[in]       src   input value
 *
 * @return Invsqrt value
 *
 */
    PUBLIC hpm_tfa_invsqrt_f32

hpm_tfa_invsqrt_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 224
    mv a1, ra
loopinvsqrt:
    call common
    bnez t1, loopinvsqrt
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret

/**
 * @brief ATANPU2 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return ATANPU2 angle_offset value
 *
 */
    PUBLIC hpm_tfa_atanpu2_f32

hpm_tfa_atanpu2_f32:
    la t5, TFA_BASE_ADDR
    addi t5, t5, 256
    la t0, CONST_2PI
    flw ft10, 0(t0)
    mv a2, ra
loopatanpu2:
    fsw fa1, 0(t5)
    fsw fa0, 4(t5)
    flw ft7, 8(t5)
    flw ft8, 12(t5)
    flw ft9, 16(t5)
    fdiv.s fa0, ft9, ft8
    call hpm_tfa_atan_f32
    fmul.s fa0, fa0, ft10
    fmul.s ft7, ft7, ft10
    fadd.s fa0, fa0, ft7
    lw t1, 24(t5)
    srli t1, t1, 31
    bnez t1, loopatanpu2
    sw x0, 28(t5)
    mv ra, a2
    ret

/**
 * @brief CORDIC f32 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    PUBLIC hpm_tfa_cordic_f32

hpm_tfa_cordic_f32:
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    la t6, INT_TO_DEG_DBL
    flw ft0, 0(t6)
    mv a5, ra
loopcordicf32:
    lw t1, 0x18(t0)
    li t2, 0x1F008000
    not t2, t2
    and t1, t1, t2
    li t3, 0x14000000
    or t1, t1, t3
    sw t1, 0x18(t0)
    sw a1, 0(t0)
    sw a2, 4(t0)
    lw a6, 8(t0)
    lw a7, 12(t0)
    fcvt.s.w fa0, a6
    fcvt.s.w fa1, a7
    fdiv.s fa0, fa0, ft0
    fsw fa0, 0(a0)
    fsw fa1, 4(a0)
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf32
    sw x0, 28(t0)
    mv ra, a5
    ret

/**
 * @brief CORDIC f16 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    PUBLIC hpm_tfa_cordic_f16

hpm_tfa_cordic_f16:
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    la t6, INT_TO_DEG_DBL
    flw ft0, 0(t6)
    mv a5, ra
loopcordicf16:
    lw    t1, 0x18(t0)
    li    t2, 0x0000F800
    not   t2, t2
    and   t1, t1, t2
    li    t3, 0x8000
    or    t1, t1, t3
    sw    t1, 0x18(t0)
    li t2, 0xFFFF
    and a1, a1, t2
    slli t1, a2, 16
    or t1, t1, a1
    sw t1, 4(t0)
    lw a6, 8(t0)
    lw a7, 12(t0)
    srli a7, a7, 16
    fcvt.s.w fa0, a6
    fcvt.s.w fa1, a7
    fdiv.s fa0, fa0, ft0
    fsw fa0, 0(a0)
    fsw fa1, 4(a0)
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf16
    sw x0, 28(t0)
    mv ra, a5
    ret
    ret
    end

#else

    EXTERN __iar_fmul
    EXTERN __iar_fadd
    EXTERN __iar_fdiv
    EXTERN __iar_fdiv_dbl
    EXTERN __iar_i2f

common
    lw a0, 12(sp)
    lw t5, 8(sp)
    sw a0, 0(t5)
    lw a0, 4(t5)
    lw a1, 8(t5)
    mv a6, ra
    jal ra, __iar_fmul
    sw a0, 4(sp)
    lw t5, 8(sp)
    lw a0, 12(t5)
    lw a1, 16(t5)
    jal ra, __iar_fmul
    lw t5, 8(sp)
    lw a1, 4(sp)
    jal ra, __iar_fadd
    lw t5, 8(sp)
    lw a1, 20(t5)
    jal ra, __iar_fadd
    lw t5, 8(sp)
    lw t3, 24(t5)
    srli t3, t3, 31
    mv ra, a6
    ret

/**
 * @brief Inversion value calculation.
 * @param[in]       src   input value
 *
 * @return Inversion value
 *
 */
    PUBLIC hpm_tfa_inv_f32
hpm_tfa_inv_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    sw t5, 8(sp)
loopinv:
    mv a7, ra
    call common
    bnez t3, loopinv
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief Square Root Calculations.
 * @param[in]       src   input value
 *
 * @return Sqrt value
 *
 */
    PUBLIC hpm_tfa_sqrt_f32
hpm_tfa_sqrt_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 32
    sw t5, 8(sp)
loopsqrt:
    mv a7, ra
    call common
    bnez t3, loopsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief Sine value calculation.
 * @param[in]       src   input value
 *
 * @return Sin value
 *
 */
    PUBLIC hpm_tfa_sin_f32
hpm_tfa_sin_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 64
    sw t5, 8(sp)
loopsin:
    mv a7, ra
    call common
    bnez t3, loopsin
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief Cosine value calculation.
 * @param[in]       src   input value
 *
 * @return cos value
 *
 */
    PUBLIC hpm_tfa_cos_f32
hpm_tfa_cos_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 96
    sw t5, 8(sp)
loopcos:
    mv a7, ra
    call common
    bnez t3, loopcos
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief Power 2 value calculation.
 * @param[in]       src   input value
 *
 * @return  power value
 *
 */
    PUBLIC hpm_tfa_power2_f32
hpm_tfa_power2_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 128
    sw t5, 8(sp)
looppow:
    mv a7, ra
    call common
    bnez t3, looppow
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief Logarithmic function value calculation.
 * @param[in]       src   input value
 *
 * @return log value
 *
 */
    PUBLIC hpm_tfa_log2_f32
hpm_tfa_log2_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 160
    sw t5, 8(sp)
looplog:
    mv a7, ra
    call common
    bnez t3, looplog
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief ATAN value calculation.
 * @param[in]       src   input value
 *
 * @return Atan value
 *
 */
    PUBLIC hpm_tfa_atan_f32
hpm_tfa_atan_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 192
    sw t5, 8(sp)
loopatan:
    mv a7, ra
    call common
    bnez t3, loopatan
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief INVSQRT value calculation.
 * @param[in]       src   input value
 *
 * @return Invsqrt value
 *
 */
    PUBLIC hpm_tfa_invsqrt_f32
hpm_tfa_invsqrt_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 224
    sw t5, 8(sp)
loopinvsqrt:
    mv a7, ra
    call common
    bnez t3, loopinvsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, a7
    ret

/**
 * @brief ATANPU2 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return ATANPU2 angle_offset value
 *
 */
    PUBLIC hpm_tfa_atanpu2_f32
hpm_tfa_atanpu2_f32:
    addi sp, sp, -32
    sw ra, 28(sp)
    sw a0, 16(sp)
    sw a1, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 256
    sw t5, 20(sp)
    la t0, CONST_2PI
    lw t1, 0(t0)
    sw t1, 24(sp)
loopatanpu2:
    lw t5, 20(sp)
    lw a1, 12(sp)
    lw a0, 16(sp)
    sw a1, 0(t5)
    sw a0, 4(t5)
    lw t2, 8(t5)
    lw t3, 12(t5)
    lw t4, 16(t5)
    mv a0, t4
    mv a1, t3
    jal ra, __iar_fdiv
    sw a0, 8(sp)
    jal ra, hpm_tfa_atan_f32
    mv a1, a0
    lw a0, 24(sp)
    jal ra, __iar_fmul
    sw a0, 8(sp)
    mv a0, t2
    lw a1, 24(sp)
    jal ra, __iar_fmul
    sw a0, 4(sp)
    lw a0, 8(sp)
    lw a1, 4(sp)
    jal ra, __iar_fadd
    sw a0, 0(sp)
    lw t5, 20(sp)
    lw t1, 24(t5)
    srli t1, t1, 31
    bnez t1, loopatanpu2
    sw x0, 28(t5)
    lw a0, 0(sp)
    lw ra, 28(sp)
    addi sp, sp, 32
    ret

/**
 * @brief CORDIC f32 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    PUBLIC hpm_tfa_cordic_f32

hpm_tfa_cordic_f32:
    addi sp, sp, -24
    sw ra, 12(sp)
    sw a0, 8(sp)
    sw a1, 4(sp)
    sw a2, 20(sp)
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    sw t0, 16(sp)
    la t1, INT_TO_DEG_DBL
    lw t2, 0(t1)
    sw t2, 0(sp)
loopcordicf32:
    lw t1, 24(t0)
    li t3, 0x1F008000
    not t3, t3
    and t1, t1, t3
    li t3, 0x14000000
    or t1, t1, t3
    sw t1, 24(t0)
    lw a1, 4(sp)
    lw a2, 20(sp)
    sw a1, 0(t0)
    sw a2, 4(t0)
    lw t0, 16(sp)
    lw a0, 8(t0)
    jal ra, __iar_i2f
    mv t3, a0
    lw a1, 0(sp)
    mv a0, t3
    jal ra, __iar_fdiv
    mv t3, a0
    lw t0, 16(sp)
    lw a0, 12(t0)
    jal ra, __iar_i2f
    mv a1, a0
    mv a0, t3
    lw t0, 8(sp)
    sw a0, 0(t0)
    sw a1, 4(t0)
    lw t0, 16(sp)
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf32
    sw x0, 28(t0)
    lw ra, 12(sp)
    addi sp, sp, 24
    ret

/**
 * @brief CORDIC f16 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    PUBLIC hpm_tfa_cordic_f16
hpm_tfa_cordic_f16:
    addi sp, sp, -24
    sw ra, 12(sp)
    sw a0, 8(sp)
    sw a1, 4(sp)
    sw a2, 20(sp)
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    sw t0, 16(sp)
    la t1, INT_TO_DEG_DBL
    lw t2, 0(t1)
    sw t2, 0(sp)
loopcordicf16:
    lw    t1, 0x18(t0)
    li    t2, 0x0000F800
    not   t2, t2
    and   t1, t1, t2
    li    t3, 0x8000
    or    t1, t1, t3
    sw    t1, 0x18(t0)
    li t2, 0xFFFF
    and a1, a1, t2
    slli t1, a2, 16
    or t1, t1, a1
    sw t1, 4(t0)
    lw t0, 16(sp)
    lw a0, 8(t0)
    jal ra, __iar_i2f
    mv t3, a0
    lw a1, 0(sp)
    mv a0, t3
    jal ra, __iar_fdiv
    mv t3, a0
    lw t0, 16(sp)
    lw a0, 12(t0)
    srli a0, a0, 16
    jal ra, __iar_i2f
    mv a1, a0
    mv a0, t3
    lw t0, 8(sp)
    sw a0, 0(t0)
    sw a1, 4(t0)
    lw t0, 16(sp)
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf16
    sw x0, 28(t0)
    lw ra, 12(sp)
    addi sp, sp, 24
    ret
    end

#endif /* #ifdef __riscv_flen */

#else

#ifdef __riscv_flen

.section .rodata
.align 2
CONST_2PI: .float 6.283185307
RAD_TO_DEG_DBL: .float 57.29578
INT_TO_DEG_DBL: .float 5965232.0

.section .TFA_DRV, "ax"
.extern TFA_BASE_ADDR

.macro common
        fsw fa0, 0(a0)
        flw	ft1, 4(a0)
        flw	ft2, 8(a0)
        flw	ft3, 12(a0)
        flw	ft4, 16(a0)
        flw	ft5, 20(a0)
        fmul.s	ft1,ft1,ft2
        fmul.s	ft3,ft3,ft4
        fadd.s	ft1,ft1,ft3
        fadd.s	ft6,ft1,ft5
        lw t1, 24(a0)
        srli t1, t1, 31
.endm

/**
 * @brief Inversion value calculation.
 * @param[in]       src   input value
 *
 * @return Inversion value
 *
 */
    .global hpm_tfa_inv_f32
hpm_tfa_inv_f32:
    la a0, TFA_BASE_ADDR
loopinv:
    common
    bnez t1, loopinv
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief Square Root Calculations.
 * @param[in]       src   input value
 *
 * @return Sqrt value
 *
 */
    .global hpm_tfa_sqrt_f32
hpm_tfa_sqrt_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 32
loopsqrt:
    common
    bnez t1, loopsqrt
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief Sine value calculation.
 * @param[in]       src   input value
 *
 * @return Sin value
 *
 */
    .global hpm_tfa_sin_f32
hpm_tfa_sin_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 64
loopsin:
    common
    bnez t1, loopsin
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief Cosine value calculation.
 * @param[in]       src   input value
 *
 * @return cos value
 *
 */
    .global hpm_tfa_cos_f32
hpm_tfa_cos_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 96
loopcos:
    common
    bnez t1, loopcos
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief Power 2 value calculation.
 * @param[in]       src   input value
 *
 * @return  power value
 *
 */
    .global hpm_tfa_power2_f32
hpm_tfa_power2_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 128
looppow:
    common
    bnez t1, looppow
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief Logarithmic function value calculation.
 * @param[in]       src   input value
 *
 * @return log value
 *
 */
    .global hpm_tfa_log2_f32
hpm_tfa_log2_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 160
looplog:
    common
    bnez t1, looplog
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief ATAN value calculation.
 * @param[in]       src   input value
 *
 * @return Atan value
 *
 */
    .global hpm_tfa_atan_f32
hpm_tfa_atan_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 192
loopatan:
    common
    bnez t1, loopatan
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief INVSQRT value calculation.
 * @param[in]       src   input value
 *
 * @return Invsqrt value
 *
 */
    .global hpm_tfa_invsqrt_f32
hpm_tfa_invsqrt_f32:
    la a0, TFA_BASE_ADDR
    addi a0, a0, 224
loopinvsqrt:
    common
    bnez t1, loopinvsqrt
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

/**
 * @brief ATANPU2 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return ATANPU2 angle_offset value
 *
 */
    .global hpm_tfa_atanpu2_f32
hpm_tfa_atanpu2_f32:
    la t5, TFA_BASE_ADDR
    addi t5, t5, 256
    la t0, CONST_2PI
    mv a2, ra
loopatanpu2:
    flw ft10, 0(t0)
    fsw fa1, 0(t5)
    fsw fa0, 4(t5)
    flw ft7, 8(t5)
    flw ft8, 12(t5)
    flw ft9, 16(t5)
    fdiv.s fa0, ft9, ft8
    call hpm_tfa_atan_f32
    fmul.s fa0, fa0, ft10
    fmul.s ft7, ft7, ft10
    fadd.s fa0, fa0, ft7
    lw t1, 24(t5)
    srli t1, t1, 31
    bnez t1, loopatanpu2
    sw x0, 28(t5)
    mv ra, a2
    ret

/**
 * @brief CORDIC f32 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    .global hpm_tfa_cordic_f32
hpm_tfa_cordic_f32:
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    la t6, INT_TO_DEG_DBL
    flw ft0, 0(t6)
    mv a5, ra
loopcordicf32:
    lw t1, 0x18(t0)
    li t2, 0x1F008000
    not t2, t2
    and t1, t1, t2
    li t3, 0x14000000
    or t1, t1, t3
    sw t1, 0x18(t0)
    sw a0, 0(t0)
    sw a1, 4(t0)
    lw a6, 8(t0)
    lw a7, 12(t0)
    fcvt.s.w fa0, a6
    fcvt.s.w fa1, a7
    fdiv.s fa0, fa0, ft0
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf32
    sw x0, 28(t0)
    mv ra, a5
    ret

/**
 * @brief CORDIC f16 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    .global hpm_tfa_cordic_f16
hpm_tfa_cordic_f16:
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    la t6, INT_TO_DEG_DBL
    flw ft0, 0(t6)
    mv a5, ra
loopcordicf16:
    lw    t1, 0x18(t0)
    li    t2, 0x0000F800
    not   t2, t2
    and   t1, t1, t2
    li    t3, 0x8000
    or    t1, t1, t3
    sw    t1, 0x18(t0)
    li t2, 0xFFFF
    and a0, a0, t2
    slli t1, a1, 16
    or t1, t1, a0
    sw t1, 4(t0)
    lw a6, 8(t0)
    lw a7, 12(t0)
    srli a7, a7, 16
    fcvt.s.w fa0, a6
    fcvt.s.w fa1, a7
    fdiv.s fa0, fa0, ft0
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf16
    sw x0, 28(t0)
    mv ra, a5
    ret

#else

.section .rodata
.align 2
CONST_2PI: .float 6.283185307
RAD_TO_DEG_DBL: .float 57.29578
INT_TO_DEG_DBL: .float 5965232.0

.section .TFA_DRV, "ax"
.extern TFA_BASE_ADDR

.macro common
    lw a0, 12(sp)
    lw t5, 8(sp)
    sw a0, 0(t5)
    lw a0, 4(t5)
    lw a1, 8(t5)
    JAL __mulsf3
    sw a0, 4(sp)
    lw t5, 8(sp)
    lw a0, 12(t5)
    lw a1, 16(t5)
    JAL __mulsf3
    lw t5, 8(sp)
    lw a1, 4(sp)
    JAL __addsf3
    lw t5, 8(sp)
    lw a1, 20(t5)
    JAL __addsf3
    lw t5, 8(sp)
    lw t3, 24(t5)
    srli t3, t3, 31
.endm

/**
 * @brief Inversion value calculation.
 * @param[in]       src   input value
 *
 * @return Inversion value
 *
 */
    .global hpm_tfa_inv_f32
hpm_tfa_inv_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    sw t5, 8(sp)
loopinv:
    common
    bnez t3, loopinv
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief Square Root Calculations.
 * @param[in]       src   input value
 *
 * @return Sqrt value
 *
 */
    .global hpm_tfa_sqrt_f32
hpm_tfa_sqrt_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 32
    sw t5, 8(sp)
loopsqrt:
    common
    bnez t3, loopsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief Sine value calculation.
 * @param[in]       src   input value
 *
 * @return Sin value
 *
 */
    .global hpm_tfa_sin_f32
hpm_tfa_sin_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 64
    sw t5, 8(sp)
loopsin:
    common
    bnez t3, loopsin
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief Cosine value calculation.
 * @param[in]       src   input value
 *
 * @return cos value
 *
 */
    .global hpm_tfa_cos_f32
hpm_tfa_cos_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 96
    sw t5, 8(sp)
loopcos:
    common
    bnez t3, loopcos
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief Power 2 value calculation.
 * @param[in]       src   input value
 *
 * @return  power value
 *
 */
    .global hpm_tfa_power2_f32
hpm_tfa_power2_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 128
    sw t5, 8(sp)
looppow:
    common
    bnez t3, looppow
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief Logarithmic function value calculation.
 * @param[in]       src   input value
 *
 * @return log value
 *
 */
    .global hpm_tfa_log2_f32
hpm_tfa_log2_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 160
    sw t5, 8(sp)
looplog:
    common
    bnez t3, looplog
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief ATAN value calculation.
 * @param[in]       src   input value
 *
 * @return Atan value
 *
 */
    .global hpm_tfa_atan_f32
hpm_tfa_atan_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 192
    sw t5, 8(sp)
loopatan:
    common
    bnez t3, loopatan
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief INVSQRT value calculation.
 * @param[in]       src   input value
 *
 * @return Invsqrt value
 *
 */
    .global hpm_tfa_invsqrt_f32
hpm_tfa_invsqrt_f32:
    addi sp, sp, -20
    sw ra, 16(sp)
    sw a0, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 224
    sw t5, 8(sp)
loopinvsqrt:
    common
    bnez t3, loopinvsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET

/**
 * @brief ATANPU2 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return ATANPU2 angle_offset value
 *
 */
    .global hpm_tfa_atanpu2_f32
hpm_tfa_atanpu2_f32:
    addi sp, sp, -32
    sw ra, 28(sp)
    sw a0, 16(sp)
    sw a1, 12(sp)
    la t5, TFA_BASE_ADDR
    addi t5, t5, 256
    sw t5, 20(sp)
    la t0, CONST_2PI
    lw t1, 0(t0)
    sw t1, 24(sp)
loopatanpu2:
    lw t5, 20(sp)
    lw a1, 12(sp)
    lw a0, 16(sp)
    sw a1, 0(t5)
    sw a0, 4(t5)
    lw t2, 8(t5)
    lw t3, 12(t5)
    lw t4, 16(t5)
    mv a0, t4
    mv a1, t3
    jal ra, __divsf3
    sw a0, 8(sp)
    jal ra, hpm_tfa_atan_f32
    mv a1, a0
    lw a0, 24(sp)
    jal ra, __mulsf3
    sw a0, 8(sp)
    mv a0, t2
    lw a1, 24(sp)
    jal ra, __mulsf3
    sw a0, 4(sp)
    lw a0, 8(sp)
    lw a1, 4(sp)
    jal ra, __addsf3
    sw a0, 0(sp)
    lw t5, 20(sp)
    lw t1, 24(t5)
    srli t1, t1, 31
    bnez t1, loopatanpu2
    sw x0, 28(t5)
    lw a0, 0(sp)
    lw ra, 28(sp)
    addi sp, sp, 32
    ret

/**
 * @brief CORDIC f32 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    .global hpm_tfa_cordic_f32
hpm_tfa_cordic_f32:
    addi sp, sp, -20
    sw ra, 12(sp)
    sw a0, 8(sp)
    sw a1, 4(sp)
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    sw t0, 16(sp)
    la t1, INT_TO_DEG_DBL
    lw t2, 0(t1)
    sw t2, 0(sp)
loopcordicf32:
    lw t1, 24(t0)
    li t3, 0x1F008000
    not t3, t3
    and t1, t1, t3
    li t3, 0x14000000
    or t1, t1, t3
    sw t1, 24(t0)
    lw a0, 8(sp)
    lw a1, 4(sp)
    sw a0, 0(t0)
    sw a1, 4(t0)
    lw t0, 16(sp)
    lw a0, 8(t0)
    jal ra, __floatsisf
    mv t3, a0
    lw a1, 0(sp)
    mv a0, t3
    jal ra, __divsf3
    mv t3, a0
    lw t0, 16(sp)
    lw a0, 12(t0)
    jal ra, __floatsisf
    mv a1, a0
    mv a0, t3
    lw t0, 16(sp)
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf32
    sw x0, 28(t0)
    lw ra, 12(sp)
    addi sp, sp, 20
    ret

/**
 * @brief CORDIC f16 value calculation.
 * @param[in]       x_input   input value
 * @param[in]       y_input   input value
 *
 * @return CORDIC angle_offset value
 * @return CORDIC mode value
 *
 */
    .global hpm_tfa_cordic_f16
hpm_tfa_cordic_f16:
    addi sp, sp, -20
    sw ra, 12(sp)
    sw a0, 8(sp)
    sw a1, 4(sp)
    la t0, TFA_BASE_ADDR
    addi t0, t0, 288
    sw t0, 16(sp)
    la t1, INT_TO_DEG_DBL
    lw t2, 0(t1)
    sw t2, 0(sp)
loopcordicf16:
    lw    t1, 0x18(t0)
    li    t2, 0x0000F800
    not   t2, t2
    and   t1, t1, t2
    li    t3, 0x8000
    or    t1, t1, t3
    sw    t1, 0x18(t0)
    li t2, 0xFFFF
    and a0, a0, t2
    slli t1, a1, 16
    or t1, t1, a0
    sw t1, 4(t0)
    lw t0, 16(sp)
    lw a0, 8(t0)
    jal ra, __floatsisf
    mv t3, a0
    lw a1, 0(sp)
    mv a0, t3
    jal ra, __divsf3
    mv t3, a0
    lw t0, 16(sp)
    lw a0, 12(t0)
    srli a0, a0, 16
    jal ra, __floatsisf
    mv a1, a0
    mv a0, t3
    lw t0, 16(sp)
    lw t1, 24(t0)
    srli t1, t1, 31
    bnez t1, loopcordicf16
    sw x0, 28(t0)
    lw ra, 12(sp)
    addi sp, sp, 20
    ret
#endif  /* #ifdef __riscv_flen */
#endif  /* #else - IAR vs GCC */


