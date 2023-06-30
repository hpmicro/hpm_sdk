/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tx_api.h"
#include "board.h"
#include "hpm_mchtmr_drv.h"

extern const unsigned int __threadx_irq_stack_top[];

/* This value is passed to _tx_initialize_unused_memory.
 * In case of user managing it without enough protection,
 * it is set to -1. User can declear a global array and
 * create a byte_mem_pool or a block_mem_pool to manage
 * memory instead.
 * BE SURE YOU KNOW WHAT WILL HAPPEN WHEN YOU MODIFY IT!
 */
#define UNUSED_MEM_PTR  (-1)

extern VOID *_tx_initialize_unused_memory;

extern VOID _tx_timer_interrupt(VOID);
extern void __disable_interrupt(void);
#define OS_MTIME_BASE_ADDRESS (HPM_MCHTMR_BASE)
#define OS_MTIMECMP_BASE_ADDRESS (HPM_MCHTMR_BASE + 8UL)
#define OS_CPU_CLOCK_HZ ((uint32_t)24000000)
#define OS_TICK_RATE_HZ 100U
const uint32_t isr_stack_top = (uint32_t)__threadx_irq_stack_top;
uint64_t os_next_tic = 0ULL;
const uint64_t *p_os_next_tic = &os_next_tic;
volatile uint64_t *p_mchtmr_cmp_reg = NULL;
uint32_t const p_mchtmr_cmp_reg_base = OS_MTIMECMP_BASE_ADDRESS;
size_t const timer_increment_per_tick = (size_t)((OS_CPU_CLOCK_HZ) / (OS_TICK_RATE_HZ));

void CPU_TS_Setup(void);
VOID _tx_initialize_low_level(VOID)
{
    __disable_interrupt();

    __asm volatile("sw      sp, _tx_thread_system_stack_ptr, t0");

    _tx_initialize_unused_memory = (VOID *)UNUSED_MEM_PTR;
    mchtmr_init_counter(HPM_MCHTMR, 0);
    /* enable CPU cycle counter */
    CPU_TS_Setup();
}

void CPU_TS_Setup(void)
{
    uint32_t current_time_h, current_time_l;
    /* 8-byte typer so high 32-bit word is 4 bytes up. */
    volatile uint32_t *const p_mchtmr_counter_h = (volatile uint32_t *const)((OS_MTIME_BASE_ADDRESS) + 4UL);
    volatile uint32_t *const p_mchtmr_counter_l = (volatile uint32_t *const)(OS_MTIME_BASE_ADDRESS);
    volatile uint32_t hart_id;

    __asm volatile("csrr %0, mhartid"
                   : "=r"(hart_id));
    p_mchtmr_cmp_reg = (volatile uint64_t *)(p_mchtmr_cmp_reg_base + (hart_id * sizeof(uint64_t)));

    do {
        current_time_h = *p_mchtmr_counter_h;
        current_time_l = *p_mchtmr_counter_l;
    } while (current_time_h != *p_mchtmr_counter_h);

    os_next_tic = (uint64_t)current_time_h;
    os_next_tic <<= 32ULL; /* High 4-byte word is 32-bits up. */
    os_next_tic |= (uint64_t)current_time_l;
    os_next_tic += (uint64_t)timer_increment_per_tick;
    *p_mchtmr_cmp_reg = os_next_tic;
    os_next_tic += (uint64_t)timer_increment_per_tick;
}