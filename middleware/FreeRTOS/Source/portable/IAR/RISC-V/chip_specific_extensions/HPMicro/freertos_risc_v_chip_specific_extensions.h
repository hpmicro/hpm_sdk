/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_SIFIVE_CLINT         0
#define portasmADDITIONAL_CONTEXT_SIZE  0
#define portasmHANDLE_INTERRUPT         irq_handler_trap

portasmSAVE_ADDITIONAL_REGISTERS MACRO
    ENDM

portasmRESTORE_ADDITIONAL_REGISTERS MACRO
    ENDM


#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
