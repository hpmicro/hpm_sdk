/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ucos_risc_v_chip_specific_extensions.h"
#include "context.h"
#ifndef portasmHANDLE_INTERRUPT
    #error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.
#endif

#ifndef portasmHAS_SIFIVE_CLINT
    #define portasmHAS_SIFIVE_CLINT 0
#endif

#define portCONTEXT_SIZE ( 32 * portWORD_SIZE )
#********************************************************************************************************
#                                          PUBLIC FUNCTIONS
#********************************************************************************************************

    .extern  OSRunning                               # External references
    .extern  OSPrioCur
    .extern  OSPrioHighRdy
    .extern  OSTCBCurPtr
    .extern  OSTCBHighRdyPtr
    .extern  OSIntExit
    .extern  OSTaskSwHook
    .extern  CPU_TS_Setup

    .global  OSStartHighRdy                          # Functions declared in this file
    .global  OSCtxSw
    .global  OSIntCtxSw
    .global  Software_IRQHandler

#********************************************************************************************************
#                                               EQUATES
#********************************************************************************************************

    .equ  RISCV_MSTATUS_MIE,         0x08

    .equ  RISCV_MIE_MSIE,            0x08            # M Soft Interrupt bit

    .equ  RISCV_MEIE_MTIE,           0x880           # M Extral MachineTimer Interrupt bit

    .equ  RISCV_PRCI_BASE_ADDR,      0xE6401000

.global ucos_risc_v_trap_handler
.extern OSTCBCurPtr
.extern ulPortTrapHandler
.extern vTaskSwitchContext
.extern OSTimeTick
.extern OSIntEnter
.extern OSIntExit
.extern Timer_IRQHandler
.extern p_mchtmr_cmp_reg
.extern p_os_next_tic
.extern timer_increment_per_tick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.extern isr_stack_top
.extern portasmHANDLE_INTERRUPT

/*-----------------------------------------------------------*/
.section .isr_vector.ucos_risc_v_trap_handler, "ax"
.balign 4
.type ucos_risc_v_trap_handler, function

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
    
    portasmSAVE_FPU_REGISTERS

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
        call Software_IRQHandler  /* The Software_IRQHandler returns to the thread. Never returns here. */

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
    portasmRESTORE_FPU_REGISTERS
    portasmRESTORE_ADDITIONAL_REGISTERS    /* Defined in ucos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

# Retrieve the address at which exception happened
    lw     t0, 31 * 4(sp)
    csrw   mepc, t0

    li     t0, 0x08
    csrrc  zero, mstatus, t0

# Restore x1 to x31 registers
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

# Exception return will restore remaining context
    mret
