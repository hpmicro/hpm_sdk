/*
 * FreeRTOS Kernel V10.5.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef PORTCONTEXT_H
#define PORTCONTEXT_H

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

#include "freertos_risc_v_chip_specific_extensions.h"
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
#define portIRQ_PREEMPTIVE   1
#else
#define portIRQ_PREEMPTIVE   0
#endif
/* Only the standard core registers are stored by default.  Any additional
 * registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
 * portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
 * specific version of freertos_risc_v_chip_specific_extensions.h.  See the
 * notes at the top of portASM.S file. */
#ifdef __riscv_32e
    #define portCONTEXT_SIZE ( 16 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET 13
    #define portMSTATUS_OFFSET  14
#else
    #define portCONTEXT_SIZE ( 32 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET 29
    #define portMSTATUS_OFFSET  30
#endif

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

/* offset 0 * FREGBYTES - mepc
 * offset 1 * FREGBYTES - mstatus
 * offset (2 - 32) * FREGBYTES - float registers
 */
#define portFPU_CONTEXT_WORDS   36
#define portFPU_CONTEXT_SIZE (portFPU_CONTEXT_WORDS * FREGBYTES)

.macro portasmSAVE_FPU_REGISTERS
    addi    sp, sp, -portFPU_CONTEXT_SIZE
    /* get FS field from mstatus */
    li t0, MSTATUS_FS
    csrr    t1, mstatus
    store_x t1, 33 * FREGBYTES(sp)
    and t0, t1, t0
    li t2, MSTATUS_FS_DIRTY
    bne t2, t0, 1f
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
1:
    .endm

.macro portasmRESTORE_FPU_REGISTERS
    load_x  t1, 33 * FREGBYTES(sp)
    /* get FS field from mstatus */
    li t0, MSTATUS_FS
    and t0, t1, t0
    li t2, MSTATUS_FS_DIRTY
    bne t2, t0, 1f
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

    /* must set FS to clean */
    csrc    mstatus, t0
    li  t1, MSTATUS_FS_CLEAN
    csrs    mstatus, t1
1:
    addi    sp, sp, portFPU_CONTEXT_SIZE
    .endm
#endif
/*-----------------------------------------------------------*/

.extern pxCurrentTCB
.extern xISRStackTop
.extern xCriticalNesting
.extern pxCriticalNesting
.extern freertos_vector_save_context
.extern freertos_vector_restore_context
/*-----------------------------------------------------------*/

.macro portcontextSAVE_CONTEXT_INTERNAL
    addi sp, sp, -portCONTEXT_SIZE
    store_x x1, 1 * portWORD_SIZE( sp )
    call freertos_vector_save_context
    .endm
/*-----------------------------------------------------------*/

.macro portcontextSAVE_EXCEPTION_CONTEXT
    portcontextSAVE_CONTEXT_INTERNAL
    csrr a0, mcause
    csrr a1, mepc
    addi a1, a1, 4                      /* Synchronous so update exception return address to the instruction after the instruction that generated the exception. */
    store_x a1, 0( sp )                 /* Save updated exception return address. */
    csrr t0, mscratch                   /* Only save SP in not nested situation */
#if portIRQ_PREEMPTIVE                  /* If we enable irq preemptive */
    bne t0, x0, skip_switch_stack2
#endif
    load_x sp, xISRStackTop             /* Switch to ISR stack. */
#if portIRQ_PREEMPTIVE
skip_switch_stack2:
#endif
    addi t0, t0, 1
    csrw mscratch, t0                   /* Increase nested value */
    .endm
/*-----------------------------------------------------------*/

.macro portcontextSAVE_INTERRUPT_CONTEXT
    portcontextSAVE_CONTEXT_INTERNAL
    csrr a0, mcause
    csrr a1, mepc
    store_x a1, 0( sp )                 /* Asynchronous interrupt so save unmodified exception return address. */
    csrr t0, mscratch                   /* Only save SP in not nested situation */
#if portIRQ_PREEMPTIVE                  /* If we enable irq preemptive */
    bne t0, x0, 1f
#endif
    load_x sp, xISRStackTop             /* Switch to ISR stack. */
#if portIRQ_PREEMPTIVE                  /* If we enable irq preemptive */
1:
#endif
    addi t0, t0, 1
    csrw mscratch, t0                   /* Increase nested value */
    .endm
/*-----------------------------------------------------------*/

#endif /* PORTCONTEXT_H */
