/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#if __riscv_xlen == 64
    #define portWORD_SIZE 8
    #define store_x sd
    #define load_x ld
#elif __riscv_xlen == 32
    #define store_x sw
    #define load_x lw
    #define portWORD_SIZE 4
#else
    #error Assembler did not define __riscv_xlen
#endif

#include "ucos_risc_v_chip_specific_extensions.h"

#ifndef portasmHANDLE_INTERRUPT
    #error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.
#endif

#ifndef portasmHAS_SIFIVE_CLINT
    #define portasmHAS_SIFIVE_CLINT 0
#endif

#define portCONTEXT_SIZE ( 32 * portWORD_SIZE )
;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OSRunning                               ; External references
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCurPtr
    EXTERN  OSTCBHighRdyPtr
    EXTERN  OSIntExit
    EXTERN  OSTaskSwHook
    EXTERN  CPU_TS_Setup
    EXTERN  Software_IRQHandler

#ifdef __riscv_flen
    PUBLIC  portasmSAVE_FPU_REGISTERS
    PUBLIC  portasmRESTORE_FPU_REGISTERS
#endif
;********************************************************************************************************
;                                               EQUATES
;********************************************************************************************************

RISCV_MSTATUS_MIE     DEFINE          0x08

RISCV_MIE_MSIE        DEFINE          0x08

RISCV_MEIE_MTIE       DEFINE          0x880

RISCV_PRCI_BASE_ADDR  DEFINE          0xE6401000

#ifdef __riscv_flen

#define MSTATUS_FS           0x00006000 /* Floating-point Status */
    #define MSTATUS_FS_OFF       0x00000000
    #define MSTATUS_FS_INITIAL   0x00002000
    #define MSTATUS_FS_CLEAN     0x00004000
    #define MSTATUS_FS_DIRTY     0x00006000

#if __riscv_flen == 32
    #define store_fpu           fsw
    #define load_fpu            flw
    #define FREGBYTES           4
#endif
#if __riscv_flen == 64
    #define store_fpu           fsd
    #define load_fpu            fld
    #define FREGBYTES           8
#endif

#define portFPU_CONTEXT_WORDS   34
#define portFPU_CONTEXT_SIZE (portFPU_CONTEXT_WORDS * FREGBYTES)

    SECTION `.isr_vector`:CODE(2)
    CODE
portasmSAVE_FPU_REGISTERS:
    addi    sp, sp, -portFPU_CONTEXT_SIZE
    /* get FS field from mstatus */
    li t0, MSTATUS_FS
    csrr    t1, mstatus
    store_x t1, 33 * FREGBYTES(sp)
    and t0, t1, t0
    li t2, MSTATUS_FS_DIRTY
    bne t2, t0, skip_store_fpu
    /* FS == dirty */
    /* Make room for the additional FPU registers. */
    store_fpu  f0, 1 * FREGBYTES(sp)
    store_fpu  f1, 2 * FREGBYTES(sp)
    store_fpu  f2, 3 * FREGBYTES(sp)
    store_fpu  f3, 4 * FREGBYTES(sp)
    store_fpu  f4, 5 * FREGBYTES(sp)
    store_fpu  f5, 6 * FREGBYTES(sp)
    store_fpu  f6, 7 * FREGBYTES(sp)
    store_fpu  f7, 8 * FREGBYTES(sp)
    store_fpu  f8, 9 * FREGBYTES(sp)
    store_fpu  f9, 10 * FREGBYTES(sp)
    store_fpu  f10, 11 * FREGBYTES(sp)
    store_fpu  f11, 12 * FREGBYTES(sp)
    store_fpu  f12, 13 * FREGBYTES(sp)
    store_fpu  f13, 14 * FREGBYTES(sp)
    store_fpu  f14, 15 * FREGBYTES(sp)
    store_fpu  f15, 16 * FREGBYTES(sp)
    store_fpu  f16, 17 * FREGBYTES(sp)
    store_fpu  f17, 18 * FREGBYTES(sp)
    store_fpu  f18, 19 * FREGBYTES(sp)
    store_fpu  f19, 20 * FREGBYTES(sp)
    store_fpu  f20, 21 * FREGBYTES(sp)
    store_fpu  f21, 22 * FREGBYTES(sp)
    store_fpu  f22, 23 * FREGBYTES(sp)
    store_fpu  f23, 24 * FREGBYTES(sp)
    store_fpu  f24, 25 * FREGBYTES(sp)
    store_fpu  f25, 26 * FREGBYTES(sp)
    store_fpu  f26, 27 * FREGBYTES(sp)
    store_fpu  f27, 28 * FREGBYTES(sp)
    store_fpu  f28, 29 * FREGBYTES(sp)
    store_fpu  f29, 30 * FREGBYTES(sp)
    store_fpu  f30, 31 * FREGBYTES(sp)
    store_fpu  f31, 32 * FREGBYTES(sp)

    /* must set FS to clean */
    csrc    mstatus, t0
    li  t1, MSTATUS_FS_CLEAN
    csrs    mstatus, t1
skip_store_fpu:
    ret

    SECTION `.isr_vector`:CODE(2)
    CODE
portasmRESTORE_FPU_REGISTERS
    load_x  t1, 33 * FREGBYTES(sp)
    /* get FS field from mstatus */
    li t0, MSTATUS_FS
    and t0, t1, t0
    li t2, MSTATUS_FS_DIRTY
    bne t2, t0, skip_load_fpu
     /* FS == dirty */
    csrs    mstatus, t0
    load_fpu   f0, 1 * FREGBYTES(sp)
    load_fpu   f1, 2 * FREGBYTES(sp)
    load_fpu   f2, 3 * FREGBYTES(sp)
    load_fpu   f3, 4 * FREGBYTES(sp)
    load_fpu   f4, 5 * FREGBYTES(sp)
    load_fpu   f5, 6 * FREGBYTES(sp)
    load_fpu   f6, 7 * FREGBYTES(sp)
    load_fpu   f7, 8 * FREGBYTES(sp)
    load_fpu   f8, 9 * FREGBYTES(sp)
    load_fpu   f9, 10 * FREGBYTES(sp)
    load_fpu   f10, 11 * FREGBYTES(sp)
    load_fpu   f11, 12 * FREGBYTES(sp)
    load_fpu   f12, 13 * FREGBYTES(sp)
    load_fpu   f13, 14 * FREGBYTES(sp)
    load_fpu   f14, 15 * FREGBYTES(sp)
    load_fpu   f15, 16 * FREGBYTES(sp)
    load_fpu   f16, 17 * FREGBYTES(sp)
    load_fpu   f17, 18 * FREGBYTES(sp)
    load_fpu   f18, 19 * FREGBYTES(sp)
    load_fpu   f19, 20 * FREGBYTES(sp)
    load_fpu   f20, 21 * FREGBYTES(sp)
    load_fpu   f21, 22 * FREGBYTES(sp)
    load_fpu   f22, 23 * FREGBYTES(sp)
    load_fpu   f23, 24 * FREGBYTES(sp)
    load_fpu   f24, 25 * FREGBYTES(sp)
    load_fpu   f25, 26 * FREGBYTES(sp)
    load_fpu   f26, 27 * FREGBYTES(sp)
    load_fpu   f27, 28 * FREGBYTES(sp)
    load_fpu   f28, 29 * FREGBYTES(sp)
    load_fpu   f29, 30 * FREGBYTES(sp)
    load_fpu   f30, 31 * FREGBYTES(sp)
    load_fpu   f31, 32 * FREGBYTES(sp)
skip_load_fpu:
    csrw    mstatus, t1
    addi    sp, sp, portFPU_CONTEXT_SIZE
    ret

#endif

    PUBLIC ucos_risc_v_trap_handler
    EXTERN OSTCBCurPtr
    EXTERN ulPortTrapHandler
    EXTERN vTaskSwitchContext
    EXTERN OSTimeTick
    EXTERN OSIntEnter
    EXTERN OSIntExit
    EXTERN Timer_IRQHandler
    EXTERN p_mchtmr_cmp_reg
    EXTERN p_os_next_tic
    EXTERN timer_increment_per_tick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
    EXTERN isr_stack_top
    EXTERN portasmHANDLE_INTERRUPT

    SECTION `.isr_vector`:CODE(2)
ucos_risc_v_trap_handler:
    addi sp, sp, -4 * 32
    store_x     ra,   0 * 4(sp)
    store_x     t0,   4 * 4(sp)
    store_x     t1,   5 * 4(sp)
    store_x     t2,   6 * 4(sp)
    store_x     s0,   7 * 4(sp)
    store_x     s1,   8 * 4(sp)
    store_x     a0,   9 * 4(sp)
    store_x     a1,  10 * 4(sp)
    store_x     a2,  11 * 4(sp)
    store_x     a3,  12 * 4(sp)
    store_x     a4,  13 * 4(sp)
    store_x     a5,  14 * 4(sp)
    store_x     a6,  15 * 4(sp)
    store_x     a7,  16 * 4(sp)
    store_x     s2,  17 * 4(sp)
    store_x     s3,  18 * 4(sp)
    store_x     s4,  19 * 4(sp)
    store_x     s5,  20 * 4(sp)
    store_x     s6,  21 * 4(sp)
    store_x     s7,  22 * 4(sp)
    store_x     s8,  23 * 4(sp)
    store_x     s9,  24 * 4(sp)
    store_x     s10, 25 * 4(sp)
    store_x     s11, 26 * 4(sp)
    store_x     t3,  27 * 4(sp)
    store_x     t4,  28 * 4(sp)
    store_x     t5,  29 * 4(sp)
    store_x     t6,  30 * 4(sp)
    csrr        t0,  mepc
    store_x     t0,  31 * 4(sp)

    portasmSAVE_ADDITIONAL_REGISTERS    /* Defined in ucos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */
#ifdef __riscv_flen
    call portasmSAVE_FPU_REGISTERS
#endif

    csrr t0, mscratch
    bne t0, x0, skip0
    /* not in isr context, set to tcb stack */
    load_x  t0, OSTCBCurPtr            /* Load OSTCBCurPtr. */
    store_x  sp, 0( t0 )                /* Write sp to first TCB member. */
skip0:

    csrr a0, mcause

test_if_asynchronous:
    srli a2, a0, __riscv_xlen - 1        /* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
    beq a2, x0, handle_synchronous        /* Branch past interrupt handing if not asynchronous. */

handle_asynchronous:
test_if_sw_isr:
        addi t0, x0, 1
        slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
        addi t1, t0, 3                    /* 0x8000[]0003 == software interrupt. */
#if( portasmHAS_MTIME != 0 )
        bne  a0, t1, test_if_mtimer
#else
        bne  a0, t1, other_interrupts
#endif /* portasmHAS_MTIME */
software_isr:
        csrr t0, mscratch
        bne t0, x0, _skip2
        /* not in isr context, load use isr_stack_top as isr stack top, otherwise use current sp */
        load_x sp, isr_stack_top               /* Switch to ISR stack before function call. */
_skip2:
        addi t0, t0, 1
        csrw mscratch, t0

        call Software_IRQHandler

#if( portasmHAS_MTIME != 0 )

test_if_mtimer:                        /* If there is a CLINT then the mtimer is used to generate the tick interrupt. */
        addi t0, x0, 1
        slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
        addi t1, t0, 7                    /* 0x8000[]0007 == machine timer interrupt. */
        bne a0, t1, other_interrupts

        load_x t0, p_mchtmr_cmp_reg  /* Load address of compare register into t0. */
        load_x t1, p_os_next_tic          /* Load the address of ullNextTime into t1. */

        #if( __riscv_xlen == 32 )

            /* Update the 64-bit mtimer compare match value in two 32-bit writes. */
            li t4, -1
            lw t2, 0(t1)                /* Load the low word of ullNextTime into t2. */
            lw t3, 4(t1)                /* Load the high word of ullNextTime into t3. */
            sw t4, 0(t0)                /* Low word no smaller than old value to start with - will be overwritten below. */
            sw t3, 4(t0)                /* Store high word of ullNextTime into compare register.  No smaller than new value. */
            sw t2, 0(t0)                /* Store low word of ullNextTime into compare register. */
            lw t0, timer_increment_per_tick    /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to p_os_next_tic?). */
            add t4, t0, t2                /* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits). */
            sltu t5, t4, t2                /* See if the sum of low words overflowed (what about the zero case?). */
            add t6, t3, t5                /* Add overflow to high word of ullNextTime. */
            sw t4, 0(t1)                /* Store new low word of ullNextTime. */
            sw t6, 4(t1)                /* Store new high word of ullNextTime. */

        #endif /* __riscv_xlen == 32 */

        #if( __riscv_xlen == 64 )

            /* Update the 64-bit mtimer compare match value. */
            ld t2, 0(t1)                 /* Load ullNextTime into t2. */
            sd t2, 0(t0)                /* Store ullNextTime into compare register. */
            ld t0, timer_increment_per_tick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to p_os_next_tic?). */
            add t4, t0, t2                /* Add ullNextTime to the timer increments for one tick. */
            sd t4, 0(t1)                /* Store ullNextTime. */

        #endif /* __riscv_xlen == 64 */

        csrr t0, mscratch
        bne t0, x0, _skip1
        /* not in isr context, load use isr_stack_top as isr stack top, otherwise use current sp */
        load_x sp, isr_stack_top            /* Switch to ISR stack before function call. */
_skip1:
        addi t0, t0, 1
        csrw mscratch, t0
        call OSIntEnter
        call OSTimeTick
        call OSIntExit
        j processed_source

#endif /* portasmHAS_MTIME */

other_interrupts:
    csrr t0, mscratch
    bne t0, x0, skip2
    /* not in isr context, load use isr_stack_top as isr stack top, otherwise use current sp */
    load_x sp, isr_stack_top               /* Switch to ISR stack before function call. */
skip2:
    addi t0, t0, 1
    csrw mscratch, t0
    call OSIntEnter
    call portasmHANDLE_INTERRUPT          /* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
    call OSIntExit
    j processed_source

handle_synchronous:
    addi a1, a1, 4                        /* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
    store_x a1, 0( sp )                   /* Save updated exception return address. */

test_if_environment_call:
    li t0, 11                             /* 11 == environment call. */
    bne a0, t0, is_exception              /* Not an M environment call, so some other exception. */

    csrr t0, mscratch
    bne t0, x0, skip3
    /* not in isr context, load use isr_stack_top as isr stack top, otherwise use current sp */
    load_x sp, isr_stack_top              /* Switch to ISR stack before function call. */
skip3:
    addi t0, t0, 1
    csrw mscratch, t0

    /* Disable task scheduler, if it's already in ISR */
    addi t1, t0, -1
    bgtz t1, processed_source

    j processed_source

is_exception:
    csrr t0, mcause                        /* For viewing in the debugger only. */
    csrr t1, mepc                        /* For viewing in the debugger only */
    csrr t2, mstatus
    j is_exception                        /* No other exceptions handled yet. */

processed_source:
    csrr t0, mscratch
    beq t0, x0, skip4
    /* in isr context, mscrach needs to be updated */
    addi t0, t0, -1
    csrw mscratch, t0
skip4:
    bne t0, x0, skip5
    /* not in isr context any more, sp needs to be restored from tcb */
    load_x  t1, OSTCBCurPtr            /* Load OSTCBCurPtr. */
    load_x  sp, 0( t1 )                     /* Read sp from first TCB member. */
skip5:
#ifdef __riscv_flen
    call portasmRESTORE_FPU_REGISTERS
#endif

    portasmRESTORE_ADDITIONAL_REGISTERS    /* Defined in ucos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

; Retrieve the address at which exception happened
    lw     t0, 31 * 4(sp)
    csrw   mepc, t0

    li     t0, 0x08
    csrrc  zero, mstatus, t0

; Restore x1 to x31 registers
    lw     ra,   0 * 4(sp)
    lw     t0,   4 * 4(sp)
    lw     t1,   5 * 4(sp)
    lw     t2,   6 * 4(sp)
    lw     s0,   7 * 4(sp)
    lw     s1,   8 * 4(sp)
    lw     a0,   9 * 4(sp)
    lw     a1,  10 * 4(sp)
    lw     a2,  11 * 4(sp)
    lw     a3,  12 * 4(sp)
    lw     a4,  13 * 4(sp)
    lw     a5,  14 * 4(sp)
    lw     a6,  15 * 4(sp)
    lw     a7,  16 * 4(sp)
    lw     s2,  17 * 4(sp)
    lw     s3,  18 * 4(sp)
    lw     s4,  19 * 4(sp)
    lw     s5,  20 * 4(sp)
    lw     s6,  21 * 4(sp)
    lw     s7,  22 * 4(sp)
    lw     s8,  23 * 4(sp)
    lw     s9,  24 * 4(sp)
    lw     s10, 25 * 4(sp)
    lw     s11, 26 * 4(sp)
    lw     t3,  27 * 4(sp)
    lw     t4,  28 * 4(sp)
    lw     t5,  29 * 4(sp)
    lw     t6,  30 * 4(sp)

    addi   sp, sp, 4 * 32

; Exception return will restore remaining context
    mret
    END
