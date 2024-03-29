/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __UCOS_RISC_V_EXTENSIONS_H__
#define __UCOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_MTIME                1
#define portasmADDITIONAL_CONTEXT_SIZE  0
#define portasmHANDLE_INTERRUPT         irq_handler_trap

portasmSAVE_ADDITIONAL_REGISTERS MACRO
    ENDM

portasmRESTORE_ADDITIONAL_REGISTERS MACRO
    ENDM
#endif /* __UCOS_RISC_V_EXTENSIONS_H__ */
