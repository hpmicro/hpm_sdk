/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#include "hpm_csr_regs.h"

#define portasmHAS_SIFIVE_CLINT         0
#define portasmHANDLE_INTERRUPT         irq_handler_trap
#ifndef portasmHAS_MTIME
#define portasmHAS_MTIME                0
#endif

.equ  HPMICRO_PLIC_M_THRESHOLD,      0xE4200000

.extern irq_handler_trap

#ifdef __riscv_dsp
#define portasmADDITIONAL_CONTEXT_DSP_SIZE 4
#else
#define portasmADDITIONAL_CONTEXT_DSP_SIZE 0
#endif

#if (defined (USE_SYSCALL_INTERRUPT_PRIORITY)) && USE_SYSCALL_INTERRUPT_PRIORITY
#define portasmADDITIONAL_CONTEXT_SYSCALL_SIZE 4
#else
#define portasmADDITIONAL_CONTEXT_SYSCALL_SIZE 0
#endif

#define portasmADDITIONAL_CONTEXT_MCCTL_SIZE 4

#define portasmADDITIONAL_CONTEXT_SIZE  (portasmADDITIONAL_CONTEXT_DSP_SIZE + portasmADDITIONAL_CONTEXT_SYSCALL_SIZE + portasmADDITIONAL_CONTEXT_MCCTL_SIZE)

.macro portasmSAVE_ADDITIONAL_REGISTERS
    addi sp, sp, -(4 * portasmADDITIONAL_CONTEXT_MCCTL_SIZE)
    csrr t0, CSR_MCCTLBEGINADDR
    sw   t0, 2 * 4( sp )
    csrr t0, CSR_MCCTLDATA
    sw   t0, 1 * 4( sp )
#ifdef __riscv_dsp
    addi sp, sp, -(4 * portasmADDITIONAL_CONTEXT_DSP_SIZE)
    rdov t0
    sw   t0, 1 * 4( sp )
#endif
#if (defined (USE_SYSCALL_INTERRUPT_PRIORITY)) && USE_SYSCALL_INTERRUPT_PRIORITY
    addi sp, sp, -(4 * portasmADDITIONAL_CONTEXT_SYSCALL_SIZE)
    csrr t1, mie
    sw   t1, 2 * 4( sp )
    li t0, HPMICRO_PLIC_M_THRESHOLD
    load_x t1, 0( t0 )
    sw   t1, 1 * 4( sp )
#endif
    .endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
#if (defined (USE_SYSCALL_INTERRUPT_PRIORITY)) && USE_SYSCALL_INTERRUPT_PRIORITY
    lw   a0, 1 * 4( sp )
    li   t0, HPMICRO_PLIC_M_THRESHOLD
    sw   a0, 0( t0 )
    lw   a0, 2 * 4( sp )
    csrw mie, a0
    addi sp, sp, (4 * portasmADDITIONAL_CONTEXT_SYSCALL_SIZE)
#endif
#ifdef __riscv_dsp
    lw   t0, 1 * 4( sp )
    csrw ucode, t0
    addi sp, sp, (4 * portasmADDITIONAL_CONTEXT_DSP_SIZE)
#endif
    lw   t0, 1 * 4( sp )
    csrw CSR_MCCTLDATA, t0
    lw   t0, 2 * 4( sp )
    csrw CSR_MCCTLBEGINADDR, t0
    addi sp, sp, (4 * portasmADDITIONAL_CONTEXT_MCCTL_SIZE)
    .endm

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
