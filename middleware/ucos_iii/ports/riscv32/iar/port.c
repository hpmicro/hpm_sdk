/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*  UCOS III includes. */
#include "cpu.h"
#include "cpu_core.h"
#include "os_cfg_app.h"

/*  HPM example includes. */
#include "board.h"
#include "hpm_mchtmr_drv.h"

extern const uint32_t CSTACK$$Limit[];
#define _stack CSTACK$$Limit

#define OS_MTIME_BASE_ADDRESS (HPM_MCHTMR_BASE)
#define OS_MTIMECMP_BASE_ADDRESS (HPM_MCHTMR_BASE + 8UL)
#define OS_CPU_CLOCK_HZ ((uint32_t)24000000)
#define OS_TICK_RATE_HZ OS_CFG_TICK_RATE_HZ
extern const uint32_t __ucos_iii_irq_stack_top[];
const uint32_t isr_stack_top = (uint32_t)__ucos_iii_irq_stack_top;
uint64_t os_next_tic = 0ULL;
const uint64_t *p_os_next_tic = &os_next_tic;
volatile uint64_t *p_mchtmr_cmp_reg = NULL;
uint32_t const p_mchtmr_cmp_reg_base = OS_MTIMECMP_BASE_ADDRESS;
size_t const timer_increment_per_tick = (size_t)((OS_CPU_CLOCK_HZ) / (OS_TICK_RATE_HZ));

void CPU_TS_Setup(void);
void CPU_TS_TmrInit(void)
{
    mchtmr_init_counter(HPM_MCHTMR, 0);
    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)OS_CPU_CLOCK_HZ);
    CPU_TS_Setup();
}

CPU_TS_TMR CPU_TS_TmrRd(void)
{
    return mchtmr_get_count(HPM_MCHTMR);
}

void CPU_TS_Setup(void)
{
    uint32_t current_time_h, current_time_l;
    /* 8-byte typer so high 32-bit word is 4 bytes up. */
    volatile uint32_t *const p_mchtmr_counter_h = (volatile uint32_t *)((OS_MTIME_BASE_ADDRESS) + 4UL);
    volatile uint32_t *const p_mchtmr_counter_l = (volatile uint32_t *)(OS_MTIME_BASE_ADDRESS);

    p_mchtmr_cmp_reg = (volatile uint64_t *)p_mchtmr_cmp_reg_base;

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