/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_SIFIVE_CLINT         0
#define portasmHANDLE_INTERRUPT         irq_handler_trap
#ifndef portasmHAS_MTIME
#define portasmHAS_MTIME                0
#endif
    EXTERN irq_handler_trap
#ifdef __riscv_dsp
#define portasmADDITIONAL_CONTEXT_SIZE  1
#else
#define portasmADDITIONAL_CONTEXT_SIZE  0
#endif

#ifdef __riscv_dsp
portasmSAVE_ADDITIONAL_REGISTERS MACRO
    addi sp, sp, -(4 * portasmADDITIONAL_CONTEXT_SIZE)
    rdov t0
    sw   t0, 1 * 4( sp )
    ENDM
#else
portasmSAVE_ADDITIONAL_REGISTERS MACRO
    ENDM
#endif

#ifdef __riscv_dsp
portasmRESTORE_ADDITIONAL_REGISTERS MACRO
    lw   t0, 1 * 4( sp )
    csrw ucode, t0
    addi sp, sp, (4 * portasmADDITIONAL_CONTEXT_SIZE)
    ENDM
#else
portasmRESTORE_ADDITIONAL_REGISTERS MACRO
    ENDM
#endif


#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
