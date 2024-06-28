/*
 * Copyright (c) 2021 HPMicro
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

.extern irq_handler_trap
#ifdef __riscv_dsp
#define portasmADDITIONAL_CONTEXT_SIZE  1
#else
#define portasmADDITIONAL_CONTEXT_SIZE  0
#endif

#ifdef __riscv_dsp
.macro portasmSAVE_ADDITIONAL_REGISTERS
    addi sp, sp, -(4 * portasmADDITIONAL_CONTEXT_SIZE)
    rdov t0
    sw   t0, 1 * 4( sp )
    .endm
#else
.macro portasmSAVE_ADDITIONAL_REGISTERS
    /* No additional registers to save, so this macro does nothing. */
    .endm
#endif

#ifdef __riscv_dsp
.macro portasmRESTORE_ADDITIONAL_REGISTERS
    lw   t0, 1 * 4( sp )
    csrw ucode, t0
    addi sp, sp, (4 * portasmADDITIONAL_CONTEXT_SIZE)
    .endm
#else
.macro portasmRESTORE_ADDITIONAL_REGISTERS
    /* No additional registers to restore, so this macro does nothing. */
    .endm
#endif
#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
