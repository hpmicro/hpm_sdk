/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
 
#ifdef __IASMRISCV__

#if defined(CONFIG_TFA_ENABLE)
#ifdef __riscv_flen

     MODULE ?TFA_DRV
     SECTION `.TFA_DRV`:CODE:NOROOT(3)

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
    lui a0, 0x400
loopinv:
    mv a1, ra
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
    lui a0, 0x400
loopsqrt:
    addi a0, a0, 32
    mv a1, ra
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
    lui a0, 0x400
loopsin:
    addi a0, a0, 64
    mv a1, ra
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
    lui a0, 0x400
loopcos:
    addi a0, a0, 96
    mv a1, ra
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
    lui a0, 0x400
looppow:
    addi a0, a0, 128
    mv a1, ra
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
    lui a0, 0x400
looplog:
    addi a0, a0, 160
    mv a1, ra
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
    lui a0, 0x400
loopatan:
    addi a0, a0, 192
    mv a1, ra
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
    lui a0, 0x400
loopinvsqrt:
    addi a0, a0, 224
    mv a1, ra
    call common
    bnez t1, loopinvsqrt
    fmv.s fa0, ft6
    sw x0, 28(a0)
    mv ra, a1
    ret
    end
    
#else

     MODULE ?TFA_DRV
     SECTION `.TFA_DRV`:CODE:NOROOT(3)
     EXTERN __iar_fmul
     EXTERN __iar_fadd

common
    lw a0, 12(sp)
    lw t5, 8(sp)
    sw a0, 0(t5)
    lw a0, 4(t5)
    lw a1, 8(t5)
    mv s1, ra
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
    mv ra, s1
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
    lui t5, 0x400
    sw t5, 8(sp)
loopinv:
    mv s0, ra
    call common
    bnez t3, loopinv
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 32
    sw t5, 8(sp)
loopsqrt:
    mv s0, ra
    call common
    bnez t3, loopsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 64
    sw t5, 8(sp)
loopsin:
    mv s0, ra
    call common
    bnez t3, loopsin
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 96
    sw t5, 8(sp)
loopcos:
    mv s0, ra
    call common
    bnez t3, loopcos
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 128
    sw t5, 8(sp)
looppow:
    mv s0, ra
    call common
    bnez t3, looppow
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 160
    sw t5, 8(sp)
looplog:
    mv s0, ra
    call common
    bnez t3, looplog
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 192
    sw t5, 8(sp)
loopatan:
    mv s0, ra
    call common
    bnez t3, loopatan
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
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
    lui t5, 0x400
    addi t5, t5, 224
    sw t5, 8(sp)
loopinvsqrt:
    mv s0, ra
    call common
    bnez t3, loopinvsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    mv ra, s0
    ret
    end

#endif /* #ifdef __riscv_flen */
#else
    end /* just for iar build */
#endif /* #if defined(CONFIG_TFA_ENABLE) */

#else

/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#if defined(CONFIG_TFA_ENABLE)
#ifdef __riscv_flen

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
    lui a0, 0x400
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
    lui a0, 0x400
loopsqrt:
    addi a0, a0, 32
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
    lui a0, 0x400
loopsin:
    addi a0, a0, 64
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
    lui a0, 0x400
loopcos:
    addi a0, a0, 96
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
    lui a0, 0x400
looppow:
    addi a0, a0, 128
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
    lui a0, 0x400
looplog:
    addi a0, a0, 160
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
    lui a0, 0x400
loopatan:
    addi a0, a0, 192
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
    lui a0, 0x400
loopinvsqrt:
    addi a0, a0, 224
    common
    bnez t1, loopinvsqrt
    fmv.s fa0, ft6
    sw x0, 28(a0)
    RET

#else

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
    lui t5, 0x400
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
    lui t5, 0x400
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
    lui t5, 0x400
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
    lui t5, 0x400
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
    lui t5, 0x400
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
    lui t5, 0x400
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
    lui t5, 0x400
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
    lui t5, 0x400
    addi t5, t5, 224
    sw t5, 8(sp)
loopinvsqrt:
    common
    bnez t3, loopinvsqrt
    sw x0, 28(t5)
    lw ra, 16(sp)
    addi sp, sp, 20
    RET
#endif
#endif

#endif


