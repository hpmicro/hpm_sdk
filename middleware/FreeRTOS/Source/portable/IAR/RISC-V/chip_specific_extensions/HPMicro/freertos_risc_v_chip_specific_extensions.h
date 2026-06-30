/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#include "hpm_csr_regs.h"

#define portasmHAS_SIFIVE_CLINT         0
#if defined(USE_NONVECTOR_MODE) && (USE_NONVECTOR_MODE != 0)
#define portasmHANDLE_INTERRUPT         freertos_handle_interrupt
    EXTERN freertos_handle_interrupt
#else
#define portasmHANDLE_INTERRUPT         irq_handler_trap
#endif
#ifndef portasmHAS_MTIME
#define portasmHAS_MTIME                0
#endif

HPMICRO_PLIC_M_THRESHOLD      DEFINE       0xE4200000

    EXTERN irq_handler_trap

#ifdef __riscv_dsp
#define portasmADDITIONAL_CONTEXT_DSP_SIZE 4
#else
#define portasmADDITIONAL_CONTEXT_DSP_SIZE 0
#endif

#define portasmADDITIONAL_CONTEXT_MCCTL_SIZE 4

#define portasmADDITIONAL_CONTEXT_SIZE  (portasmADDITIONAL_CONTEXT_DSP_SIZE + portasmADDITIONAL_CONTEXT_MCCTL_SIZE)

portasmSAVE_ADDITIONAL_REGISTERS MACRO
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
    ENDM

portasmRESTORE_ADDITIONAL_REGISTERS MACRO
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
    ENDM

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
