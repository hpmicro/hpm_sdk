/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "portContextInternal.h"

    PUBLIC freertos_vector_save_context
    PUBLIC freertos_vector_restore_context

    SECTION `.isr_vector`:CODE:NOROOT(2)
    ALIGN 2
    CODE
freertos_vector_save_context:

    store_x x5, 2 * portWORD_SIZE( sp )
    store_x x6, 3 * portWORD_SIZE( sp )
    store_x x7, 4 * portWORD_SIZE( sp )
    store_x x8, 5 * portWORD_SIZE( sp )
    store_x x9, 6 * portWORD_SIZE( sp )
    store_x x10, 7 * portWORD_SIZE( sp )
    store_x x11, 8 * portWORD_SIZE( sp )
    store_x x12, 9 * portWORD_SIZE( sp )
    store_x x13, 10 * portWORD_SIZE( sp )
    store_x x14, 11 * portWORD_SIZE( sp )
    store_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    store_x x16, 13 * portWORD_SIZE( sp )
    store_x x17, 14 * portWORD_SIZE( sp )
    store_x x18, 15 * portWORD_SIZE( sp )
    store_x x19, 16 * portWORD_SIZE( sp )
    store_x x20, 17 * portWORD_SIZE( sp )
    store_x x21, 18 * portWORD_SIZE( sp )
    store_x x22, 19 * portWORD_SIZE( sp )
    store_x x23, 20 * portWORD_SIZE( sp )
    store_x x24, 21 * portWORD_SIZE( sp )
    store_x x25, 22 * portWORD_SIZE( sp )
    store_x x26, 23 * portWORD_SIZE( sp )
    store_x x27, 24 * portWORD_SIZE( sp )
    store_x x28, 25 * portWORD_SIZE( sp )
    store_x x29, 26 * portWORD_SIZE( sp )
    store_x x30, 27 * portWORD_SIZE( sp )
    store_x x31, 28 * portWORD_SIZE( sp )
#endif

    load_x  t0, xCriticalNesting         /* Load the value of xCriticalNesting into t0. */
    store_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Store the critical nesting value to the stack. */


    csrr t0, mstatus                     /* Required for MPIE bit. */
    store_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )


    portasmSAVE_ADDITIONAL_REGISTERS     /* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

#ifdef __riscv_flen
    portasmSAVE_FPU_REGISTERS
#endif

#if portIRQ_PREEMPTIVE                   /* If we enable irq preemptive */
    csrr t0, mscratch                    /* Only save SP in not nested situation */
    bne t0, x0, skip_save_sp
#endif
    load_x  t0, pxCurrentTCB             /* Load pxCurrentTCB. */
    store_x  sp, 0( t0 )                 /* Write sp to first TCB member. */
#if portIRQ_PREEMPTIVE
skip_save_sp:
#endif
    ret

    SECTION `.isr_vector`:CODE:NOROOT(2)
    ALIGN 2
    CODE
freertos_vector_restore_context:
    csrr t0, mscratch
    /* mscrach needs to be updated */
    addi t0, t0, -1
    csrw mscratch, t0
#if portIRQ_PREEMPTIVE
    bne t0, x0, skip_load_sp
#endif
    load_x  t1, pxCurrentTCB                /* Load pxCurrentTCB. */
    load_x  sp, 0( t1 )                     /* Read sp from first TCB member. */
#if portIRQ_PREEMPTIVE
skip_load_sp:
#endif
    /* Load mepc with the address of the instruction in the task to run next. */
    load_x t0, 0( sp )
    csrw mepc, t0

#ifdef __riscv_flen
    portasmRESTORE_FPU_REGISTERS
#endif
    /* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */
    portasmRESTORE_ADDITIONAL_REGISTERS

    /* Load mstatus with the interrupt enable bits used by the task. */
    load_x  t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )
    csrw mstatus, t0                        /* Required for MPIE bit. */

    load_x  t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp )    /* Obtain xCriticalNesting value for this task from task's stack. */
    load_x  t1, pxCriticalNesting           /* Load the address of xCriticalNesting into t1. */
    store_x t0, 0( t1 )                     /* Restore the critical nesting value for this task. */

    load_x  x1, 1 * portWORD_SIZE( sp )
    load_x  x5, 2 * portWORD_SIZE( sp )
    load_x  x6, 3 * portWORD_SIZE( sp )
    load_x  x7, 4 * portWORD_SIZE( sp )
    load_x  x8, 5 * portWORD_SIZE( sp )
    load_x  x9, 6 * portWORD_SIZE( sp )
    load_x  x10, 7 * portWORD_SIZE( sp )
    load_x  x11, 8 * portWORD_SIZE( sp )
    load_x  x12, 9 * portWORD_SIZE( sp )
    load_x  x13, 10 * portWORD_SIZE( sp )
    load_x  x14, 11 * portWORD_SIZE( sp )
    load_x  x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    load_x  x16, 13 * portWORD_SIZE( sp )
    load_x  x17, 14 * portWORD_SIZE( sp )
    load_x  x18, 15 * portWORD_SIZE( sp )
    load_x  x19, 16 * portWORD_SIZE( sp )
    load_x  x20, 17 * portWORD_SIZE( sp )
    load_x  x21, 18 * portWORD_SIZE( sp )
    load_x  x22, 19 * portWORD_SIZE( sp )
    load_x  x23, 20 * portWORD_SIZE( sp )
    load_x  x24, 21 * portWORD_SIZE( sp )
    load_x  x25, 22 * portWORD_SIZE( sp )
    load_x  x26, 23 * portWORD_SIZE( sp )
    load_x  x27, 24 * portWORD_SIZE( sp )
    load_x  x28, 25 * portWORD_SIZE( sp )
    load_x  x29, 26 * portWORD_SIZE( sp )
    load_x  x30, 27 * portWORD_SIZE( sp )
    load_x  x31, 28 * portWORD_SIZE( sp )
#endif
    addi sp, sp, portCONTEXT_SIZE

    mret
    END
