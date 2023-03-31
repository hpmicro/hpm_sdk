/*
 * FreeRTOS Kernel V10.4.4
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

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code which tailors the port to a specific RISC-V chip:
 *
 * + The code that is common to all RISC-V chips is implemented in
 *   FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S.  There is only one
 *   portASM.S file because the same file is used no matter which RISC-V chip is
 *   in use.
 *
 * + The code that tailors the kernel's RISC-V port to a specific RISC-V
 *   chip is implemented in freertos_risc_v_chip_specific_extensions.h.  There
 *   is one freertos_risc_v_chip_specific_extensions.h that can be used with any
 *   RISC-V chip that both includes a standard CLINT and does not add to the
 *   base set of RISC-V registers.  There are additional
 *   freertos_risc_v_chip_specific_extensions.h files for RISC-V implementations
 *   that do not include a standard CLINT or do add to the base set of RISC-V
 *   registers.
 *
 * CARE MUST BE TAKEN TO INCLDUE THE CORRECT
 * freertos_risc_v_chip_specific_extensions.h HEADER FILE FOR THE CHIP
 * IN USE.  To include the correct freertos_risc_v_chip_specific_extensions.h
 * header file ensure the path to the correct header file is in the assembler's
 * include path.
 *
 * This freertos_risc_v_chip_specific_extensions.h is for use on RISC-V chips
 * that include a standard CLINT and do not add to the base set of RISC-V
 * registers.
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

#include "freertos_risc_v_chip_specific_extensions.h"

/* Check the freertos_risc_v_chip_specific_extensions.h and/or command line
definitions. */
#if defined( portasmHAS_CLINT ) && defined( portasmHAS_MTIME )
    #error The portasmHAS_CLINT constant has been deprecated.  Please replace it with portasmHAS_MTIME.  portasmHAS_CLINT and portasmHAS_MTIME cannot both be defined at once.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifdef portasmHAS_CLINT
    #warning The portasmHAS_CLINT constant has been deprecated.  Please replace it with portasmHAS_MTIME and portasmHAS_SIFIVE_CLINT.  For now portasmHAS_MTIME and portasmHAS_SIFIVE_CLINT are derived from portasmHAS_CLINT.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
    #define portasmHAS_MTIME portasmHAS_CLINT
    #define portasmHAS_SIFIVE_CLINT portasmHAS_CLINT
#endif

#ifndef portasmHAS_MTIME
    #error freertos_risc_v_chip_specific_extensions.h must define portasmHAS_MTIME to either 1 (MTIME clock present) or 0 (MTIME clock not present).  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef portasmHANDLE_INTERRUPT
    #error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.  portasmHANDLE_INTERRUPT can be defined on the assembler command line or in the appropriate freertos_risc_v_chip_specific_extensions.h header file.  https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef portasmHAS_SIFIVE_CLINT
    #define portasmHAS_SIFIVE_CLINT 0
#endif

/* Only the standard core registers are stored by default.  Any additional
registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
specific version of freertos_risc_v_chip_specific_extensions.h.  See the notes
at the top of this file. */
#define portCONTEXT_SIZE ( 30 * portWORD_SIZE )


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
#define portFPU_CONTEXT_WORDS   34
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

#else

.macro portasmSAVE_FPU_REGISTERS
.endm
.macro portasmRESTORE_FPU_REGISTERS
.endm

#endif

.global xPortStartFirstTask
.global freertos_risc_v_trap_handler
.global pxPortInitialiseStack
.extern pxCurrentTCB
.extern ulPortTrapHandler
.extern vTaskSwitchContext
.extern xTaskIncrementTick
.extern Timer_IRQHandler
.extern pullMachineTimerCompareRegister
.extern pullNextTime
.extern uxTimerIncrementsForOneTick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.extern xISRStackTop
.extern portasmHANDLE_INTERRUPT

/*-----------------------------------------------------------*/
.section .isr_vector
.align 8
.type freertos_risc_v_trap_handler, function
.func
freertos_risc_v_trap_handler:
    addi sp, sp, -portCONTEXT_SIZE
    store_x x1, 1 * portWORD_SIZE( sp )
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

    csrr t0, mstatus                    /* Required for MPIE bit. */
    store_x t0, 29 * portWORD_SIZE( sp )

    portasmSAVE_ADDITIONAL_REGISTERS    /* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

    portasmSAVE_FPU_REGISTERS

    csrr t0, mscratch
    bne t0, x0, skip0
    /* not in isr context, set to tcb stack */
    load_x  t0, pxCurrentTCB            /* Load pxCurrentTCB. */
    store_x  sp, 0( t0 )                /* Write sp to first TCB member. */
skip0:

    csrr a0, mcause
    csrr a1, mepc

test_if_asynchronous:
    srli a2, a0, __riscv_xlen - 1        /* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
    beq a2, x0, handle_synchronous        /* Branch past interrupt handing if not asynchronous. */
    store_x a1, 0( sp )                    /* Asynch so save unmodified exception return address. */

handle_asynchronous:

#if( portasmHAS_MTIME != 0 )

    test_if_mtimer:                        /* If there is a CLINT then the mtimer is used to generate the tick interrupt. */

        addi t0, x0, 1

        slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
        addi t1, t0, 7                    /* 0x8000[]0007 == machine timer interrupt. */
        bne a0, t1, other_interrupts

        load_x t0, pullMachineTimerCompareRegister  /* Load address of compare register into t0. */
        load_x t1, pullNextTime          /* Load the address of ullNextTime into t1. */

        #if( __riscv_xlen == 32 )

            /* Update the 64-bit mtimer compare match value in two 32-bit writes. */
            li t4, -1
            lw t2, 0(t1)                /* Load the low word of ullNextTime into t2. */
            lw t3, 4(t1)                /* Load the high word of ullNextTime into t3. */
            sw t4, 0(t0)                /* Low word no smaller than old value to start with - will be overwritten below. */
            sw t3, 4(t0)                /* Store high word of ullNextTime into compare register.  No smaller than new value. */
            sw t2, 0(t0)                /* Store low word of ullNextTime into compare register. */
            lw t0, uxTimerIncrementsForOneTick    /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
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
            ld t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
            add t4, t0, t2                /* Add ullNextTime to the timer increments for one tick. */
            sd t4, 0(t1)                /* Store ullNextTime. */

        #endif /* __riscv_xlen == 64 */

        csrr t0, mscratch
        bne t0, x0, skip1
        /* not in isr context, load use xISRStackTop as isr stack top, otherwise use current sp */
        load_x sp, xISRStackTop            /* Switch to ISR stack before function call. */
skip1:
        addi t0, t0, 1
        csrw mscratch, t0

        call xTaskIncrementTick
        beqz a0, processed_source        /* Don't switch context if incrementing tick didn't unblock a task. */
        call vTaskSwitchContext
        j processed_source

#endif /* portasmHAS_MTIME */

other_interrupts:
    csrr t0, mscratch
    bne t0, x0, skip2
    /* not in isr context, load use xISRStackTop as isr stack top, otherwise use current sp */
    load_x sp, xISRStackTop               /* Switch to ISR stack before function call. */
skip2:
    addi t0, t0, 1
    csrw mscratch, t0

    call portasmHANDLE_INTERRUPT          /* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
    j processed_source

handle_synchronous:
    addi a1, a1, 4                        /* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
    store_x a1, 0( sp )                   /* Save updated exception return address. */

test_if_environment_call:
    li t0, 11                             /* 11 == environment call. */
    bne a0, t0, is_exception              /* Not an M environment call, so some other exception. */

    csrr t0, mscratch
    bne t0, x0, skip3
    /* not in isr context, load use xISRStackTop as isr stack top, otherwise use current sp */
    load_x sp, xISRStackTop              /* Switch to ISR stack before function call. */
skip3:
    addi t0, t0, 1
    csrw mscratch, t0

    /* Disable task scheduler, if it's already in ISR */
    addi t1, t0, -1
    bgtz t1, processed_source

    call vTaskSwitchContext
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
    load_x  t1, pxCurrentTCB            /* Load pxCurrentTCB. */
    load_x  sp, 0( t1 )                     /* Read sp from first TCB member. */
skip5:
    /* Load mret with the address of the next instruction in the task to run next. */
    load_x t0, 0( sp )
    csrw mepc, t0

    portasmRESTORE_FPU_REGISTERS

    portasmRESTORE_ADDITIONAL_REGISTERS    /* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

    /* Load mstatus with the interrupt enable bits used by the task. */
    load_x  t0, 29 * portWORD_SIZE( sp )
    csrw mstatus, t0                        /* Required for MPIE bit. */

    load_x  x1, 1 * portWORD_SIZE( sp )
    load_x  x5, 2 * portWORD_SIZE( sp )        /* t0 */
    load_x  x6, 3 * portWORD_SIZE( sp )        /* t1 */
    load_x  x7, 4 * portWORD_SIZE( sp )        /* t2 */
    load_x  x8, 5 * portWORD_SIZE( sp )        /* s0/fp */
    load_x  x9, 6 * portWORD_SIZE( sp )        /* s1 */
    load_x  x10, 7 * portWORD_SIZE( sp )    /* a0 */
    load_x  x11, 8 * portWORD_SIZE( sp )    /* a1 */
    load_x  x12, 9 * portWORD_SIZE( sp )    /* a2 */
    load_x  x13, 10 * portWORD_SIZE( sp )    /* a3 */
    load_x  x14, 11 * portWORD_SIZE( sp )    /* a4 */
    load_x  x15, 12 * portWORD_SIZE( sp )    /* a5 */
    load_x  x16, 13 * portWORD_SIZE( sp )    /* a6 */
    load_x  x17, 14 * portWORD_SIZE( sp )    /* a7 */
    load_x  x18, 15 * portWORD_SIZE( sp )    /* s2 */
    load_x  x19, 16 * portWORD_SIZE( sp )    /* s3 */
    load_x  x20, 17 * portWORD_SIZE( sp )    /* s4 */
    load_x  x21, 18 * portWORD_SIZE( sp )    /* s5 */
    load_x  x22, 19 * portWORD_SIZE( sp )    /* s6 */
    load_x  x23, 20 * portWORD_SIZE( sp )    /* s7 */
    load_x  x24, 21 * portWORD_SIZE( sp )    /* s8 */
    load_x  x25, 22 * portWORD_SIZE( sp )    /* s9 */
    load_x  x26, 23 * portWORD_SIZE( sp )    /* s10 */
    load_x  x27, 24 * portWORD_SIZE( sp )    /* s11 */
    load_x  x28, 25 * portWORD_SIZE( sp )    /* t3 */
    load_x  x29, 26 * portWORD_SIZE( sp )    /* t4 */
    load_x  x30, 27 * portWORD_SIZE( sp )    /* t5 */
    load_x  x31, 28 * portWORD_SIZE( sp )    /* t6 */
    addi sp, sp, portCONTEXT_SIZE

    mret
    .endfunc
/*-----------------------------------------------------------*/
.section .isr_vector
.align 8
.type xPortStartFirstTask, function
.func
xPortStartFirstTask:

#if( portasmHAS_SIFIVE_CLINT != 0 )
    /* If there is a clint then interrupts can branch directly to the FreeRTOS
    trap handler.  Otherwise the interrupt controller will need to be configured
    outside of this file. */
    la t0, freertos_risc_v_trap_handler
    csrw mtvec, t0
#endif /* portasmHAS_CLILNT */

    load_x  sp, pxCurrentTCB                /* Load pxCurrentTCB. */
    load_x  sp, 0( sp )                     /* Read sp from first TCB member. */

    load_x  x1, 0( sp ) /* Note for starting the scheduler the exception return address is used as the function return address. */

    portasmRESTORE_FPU_REGISTERS

    portasmRESTORE_ADDITIONAL_REGISTERS      /* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

    load_x  x6, 3 * portWORD_SIZE( sp )      /* t1 */
    load_x  x7, 4 * portWORD_SIZE( sp )      /* t2 */
    load_x  x8, 5 * portWORD_SIZE( sp )      /* s0/fp */
    load_x  x9, 6 * portWORD_SIZE( sp )      /* s1 */
    load_x  x10, 7 * portWORD_SIZE( sp )     /* a0 */
    load_x  x11, 8 * portWORD_SIZE( sp )     /* a1 */
    load_x  x12, 9 * portWORD_SIZE( sp )     /* a2 */
    load_x  x13, 10 * portWORD_SIZE( sp )    /* a3 */
    load_x  x14, 11 * portWORD_SIZE( sp )    /* a4 */
    load_x  x15, 12 * portWORD_SIZE( sp )    /* a5 */
    load_x  x16, 13 * portWORD_SIZE( sp )    /* a6 */
    load_x  x17, 14 * portWORD_SIZE( sp )    /* a7 */
    load_x  x18, 15 * portWORD_SIZE( sp )    /* s2 */
    load_x  x19, 16 * portWORD_SIZE( sp )    /* s3 */
    load_x  x20, 17 * portWORD_SIZE( sp )    /* s4 */
    load_x  x21, 18 * portWORD_SIZE( sp )    /* s5 */
    load_x  x22, 19 * portWORD_SIZE( sp )    /* s6 */
    load_x  x23, 20 * portWORD_SIZE( sp )    /* s7 */
    load_x  x24, 21 * portWORD_SIZE( sp )    /* s8 */
    load_x  x25, 22 * portWORD_SIZE( sp )    /* s9 */
    load_x  x26, 23 * portWORD_SIZE( sp )    /* s10 */
    load_x  x27, 24 * portWORD_SIZE( sp )    /* s11 */
    load_x  x28, 25 * portWORD_SIZE( sp )    /* t3 */
    load_x  x29, 26 * portWORD_SIZE( sp )    /* t4 */
    load_x  x30, 27 * portWORD_SIZE( sp )    /* t5 */
    load_x  x31, 28 * portWORD_SIZE( sp )    /* t6 */

    load_x  x5, 29 * portWORD_SIZE( sp )    /* Initial mstatus into x5 (t0) */
    addi x5, x5, 0x08                        /* Set MIE bit so the first task starts with interrupts enabled - required as returns with ret not eret. */
    csrrw  x0, mstatus, x5                    /* Interrupts enabled from here! */
    load_x  x5, 2 * portWORD_SIZE( sp )        /* Initial x5 (t0) value. */

    addi    sp, sp, portCONTEXT_SIZE
    ret
    .endfunc
/*-----------------------------------------------------------*/

/*
 * Unlike other ports pxPortInitialiseStack() is written in assembly code as it
 * needs access to the portasmADDITIONAL_CONTEXT_SIZE constant.  The prototype
 * for the function is as per the other ports:
 * StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register        ABI Name    Description                        Saver
 * x0               zero        Hard-wired zero                    -
 * x1               ra          Return address                    Caller
 * x2               sp          Stack pointer                     Callee
 * x3               gp          Global pointer                    -
 * x4               tp          Thread pointer                    -
 * x5-7             t0-2        Temporaries                       Caller
 * x8               s0/fp       Saved register/Frame pointer      Callee
 * x9               s1          Saved register                    Callee
 * x10-11           a0-1        Function Arguments/return values  Caller
 * x12-17           a2-7        Function arguments                Caller
 * x18-27           s2-11       Saved registers                   Callee
 * x28-31           t3-6        Temporaries                       Caller
 *
 * The RISC-V context is saved t FreeRTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * [chip specific registers go here]
 * pxCode
 */
.section .isr_vector
.align 8
.type pxPortInitialiseStack, function
.func
pxPortInitialiseStack:

    csrr t0, mstatus                     /* Obtain current mstatus value. */
    andi t0, t0, ~0x8                    /* Ensure interrupts are disabled when the stack is restored within an ISR.  Required when a task is created after the schedulre has been started, otherwise interrupts would be disabled anyway. */
#ifdef __riscv_flen
    addi t1, x0, 0x388                   /* Generate the value 0x3880, which are the MPIE and MPP bits to set and FS bits to 1 in mstatus. */
#else
    addi t1, x0, 0x188                   /* Generate the value 0x1880, which are the MPIE and MPP bits to set in mstatus. */
#endif
    slli t1, t1, 4
    or t0, t0, t1                        /* Set MPIE and MPP bits in mstatus value. */

    addi a0, a0, -portWORD_SIZE
    store_x t0, 0(a0)                      /* mstatus onto the stack. */
    addi a0, a0, -(22 * portWORD_SIZE)     /* Space for registers x11-x31. */
    store_x a2, 0(a0)                      /* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
    addi a0, a0, -(6 * portWORD_SIZE)      /* Space for registers x5-x9. */
    store_x x0, 0(a0)                      /* Return address onto the stack, could be portTASK_RETURN_ADDRESS */
    addi t0, x0, portasmADDITIONAL_CONTEXT_SIZE /* The number of chip specific additional registers. */
chip_specific_stack_frame:                 /* First add any chip specific registers to the stack frame being created. */
    beq t0, x0, 1f                         /* No more chip specific registers to save. */
    addi a0, a0, -portWORD_SIZE            /* Make space for chip specific register. */
    store_x x0, 0(a0)                      /* Give the chip specific register an initial value of zero. */
    addi t0, t0, -1                        /* Decrement the count of chip specific registers remaining. */
    j chip_specific_stack_frame            /* Until no more chip specific registers. */
1:

#ifdef __riscv_flen
    add a0, a0, -portFPU_CONTEXT_SIZE
    mv t2, a0
    addi t0, x0, portFPU_CONTEXT_SIZE
fpu_specific_stack_frame:
    beq t0, x0, 1f                       /* No more chip specific registers to save. */
    store_x x0, 0(t2)                    /* Give the chip specific register an initial value of zero. */
    addi t2, t2, portWORD_SIZE           /* Make space for chip specific register. */
    addi t0, t0, -portWORD_SIZE          /* Decrement the count of chip specific registers remaining. */
    j fpu_specific_stack_frame           /* Until no more chip specific registers. */
    1:
#endif

    addi a0, a0, -portWORD_SIZE
    store_x a1, 0(a0)                    /* mret value (pxCode parameter) onto the stack. */
    ret
    .endfunc
/*-----------------------------------------------------------*/
