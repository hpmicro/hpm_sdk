/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __UCOS_RISC_V_EXTENSIONS_H__
#define __UCOS_RISC_V_EXTENSIONS_H__
#ifndef CONFIG_UCOS_III_TIMER_RESOURCE_NOT_MCHTMR
#define portasmHAS_MTIME                1
#else
#define portasmHAS_MTIME                0
#endif
#define portasmADDITIONAL_CONTEXT_SIZE  0
#define portasmHANDLE_INTERRUPT         irq_handler_trap

.macro portasmSAVE_ADDITIONAL_REGISTERS
    /* No additional registers to save, so this macro does nothing. */
    .endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
    /* No additional registers to restore, so this macro does nothing. */
    .endm
#endif /* __UCOS_RISC_V_EXTENSIONS_H__ */
