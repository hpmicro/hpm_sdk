/*
 * Copyright (c) 2023-2024 HPMicro
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

extern void _tx_timer_interrupt(void);
#define OS_MTIME_BASE_ADDRESS (HPM_MCHTMR_BASE)
#define OS_MTIMECMP_BASE_ADDRESS (HPM_MCHTMR_BASE + 8UL)
#define OS_CPU_CLOCK_HZ ((uint32_t)24000000)
uint64_t os_next_tic = 0ULL;
const uint64_t *p_os_next_tic = &os_next_tic;
volatile uint64_t *p_mchtmr_cmp_reg = NULL;
uint32_t const p_mchtmr_cmp_reg_base = OS_MTIMECMP_BASE_ADDRESS;
size_t const timer_increment_per_tick = (size_t)((OS_CPU_CLOCK_HZ) / (TX_TIMER_TICKS_PER_SECOND));

#if defined(portasmHAS_MTIME) && (portasmHAS_MTIME != 0)
void tx_cpu_ts_setup(void)
{
    uint32_t current_time_h, current_time_l;
    /* 8-byte typer so high 32-bit word is 4 bytes up. */
    volatile uint32_t *const p_mchtmr_counter_h = (volatile uint32_t *)((OS_MTIME_BASE_ADDRESS) + 4UL);
    volatile uint32_t *const p_mchtmr_counter_l = (volatile uint32_t *)(OS_MTIME_BASE_ADDRESS);
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
#else
#include "hpm_gptmr_drv.h"
#ifndef THREADX_TIMER_RESOURCE
    #define THREADX_TIMER_RESOURCE      BOARD_THREADX_TIMER
#endif

#ifndef THREADX_TIMER_CH
    #define THREADX_TIMER_CH            BOARD_THREADX_TIMER_CHANNEL
#endif

#ifndef THREADX_TIMER_IRQ
    #define THREADX_TIMER_IRQ           BOARD_THREADX_TIMER_IRQ
#endif

#ifndef THREADX_TIMER_CLOCK
    #define THREADX_TIMER_CLOCK         BOARD_THREADX_TIMER_CLK_NAME
#endif
void system_tick_isr(void)
{
    if (gptmr_check_status(THREADX_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(THREADX_TIMER_CH))) {
        gptmr_clear_status(THREADX_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(THREADX_TIMER_CH));
        _tx_timer_interrupt();
    }
}
SDK_DECLARE_EXT_ISR_M(THREADX_TIMER_IRQ, system_tick_isr);
void tx_cpu_ts_setup(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(THREADX_TIMER_RESOURCE, &config);

    gptmr_freq = clock_get_frequency(THREADX_TIMER_CLOCK);
    config.reload = gptmr_freq / TX_TIMER_TICKS_PER_SECOND;
    gptmr_channel_config(THREADX_TIMER_RESOURCE, THREADX_TIMER_CH, &config, false);
    gptmr_start_counter(THREADX_TIMER_RESOURCE, THREADX_TIMER_CH);

    gptmr_enable_irq(THREADX_TIMER_RESOURCE, GPTMR_CH_RLD_IRQ_MASK(THREADX_TIMER_CH));
    intc_m_enable_irq_with_priority(THREADX_TIMER_IRQ, 2);
}

#endif