/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_SIFIVE_CLINT         0
#define portasmHAS_MTIME                1
#define portasmADDITIONAL_CONTEXT_SIZE  0
#define portasmHANDLE_INTERRUPT         irq_handler_trap

.macro portasmSAVE_ADDITIONAL_REGISTERS
    /* No additional registers to save, so this macro does nothing. */
    .endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
    /* No additional registers to restore, so this macro does nothing. */
    .endm

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
