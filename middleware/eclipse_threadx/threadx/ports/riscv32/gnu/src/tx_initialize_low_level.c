/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tx_api.h"
#include "board.h"
#include "hpm_mchtmr_drv.h"

extern void *_tx_initialize_unused_memory;
extern void tx_cpu_ts_setup(void);
extern void __disable_interrupt(void);

#define UNUSED_MEM_PTR  (-1)

void _tx_initialize_low_level(void)
{
    __disable_interrupt();

    __asm volatile("sw      sp, _tx_thread_system_stack_ptr, t0");

    _tx_initialize_unused_memory = (void *)UNUSED_MEM_PTR;
#if defined(portasmHAS_MTIME) && (portasmHAS_MTIME != 0)
    mchtmr_init_counter(HPM_MCHTMR, 0);
#endif
    /* enable CPU cycle counter */
    tx_cpu_ts_setup();
}