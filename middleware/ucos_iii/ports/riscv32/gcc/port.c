/*
 * Copyright (c) 2023-2025 HPMicro
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
#include "os.h"
#include "ucos_timer_source.h"

#define OS_TICK_RATE_HZ OS_CFG_TICK_RATE_HZ

#ifndef CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR
    #define OS_MTIME_BASE_ADDRESS (HPM_MCHTMR_BASE)
    #define OS_MTIMECMP_BASE_ADDRESS (HPM_MCHTMR_BASE + 8UL)
    #define OS_CPU_CLOCK_HZ ((uint32_t)24000000)
    uint64_t os_next_tic = 0ULL;
    const uint64_t *p_os_next_tic = &os_next_tic;
    volatile uint64_t *p_mchtmr_cmp_reg = NULL;
    uint32_t const p_mchtmr_cmp_reg_base = OS_MTIMECMP_BASE_ADDRESS;
    size_t const timer_increment_per_tick = (size_t)((OS_CPU_CLOCK_HZ) / (OS_TICK_RATE_HZ));
#else
    #define OS_CPU_CLOCK_HZ ((uint32_t)clock_get_frequency(UCOS_TIMER_CLOCK))

#endif

#if !defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)
#error "uCOS-III does not support vector mode presently!"
#endif

extern const uint32_t __ucos_iii_irq_stack_top[];
const uint32_t isr_stack_top = (uint32_t)__ucos_iii_irq_stack_top;

void CPU_TS_Setup(void);
void CPU_TS_TmrInit(void)
{
    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)OS_CPU_CLOCK_HZ);
    CPU_TS_Setup();
}

CPU_TS_TMR CPU_TS_TmrRd(void)
{
#ifndef CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR
    return mchtmr_get_count(HPM_MCHTMR);
#else
    return gptmr_channel_get_counter(UCOS_TIMER_RESOURCE, UCOS_TIMER_CH, gptmr_counter_type_normal);
#endif
}

void CPU_TS_Setup(void)
{
#ifndef CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR
    uint32_t current_time_h, current_time_l;
    mchtmr_init_counter(HPM_MCHTMR, 0);
    /* 8-byte typer so high 32-bit word is 4 bytes up. */
    volatile uint32_t *const p_mchtmr_counter_h = (volatile uint32_t *const)((OS_MTIME_BASE_ADDRESS) + 4UL);
    volatile uint32_t *const p_mchtmr_counter_l = (volatile uint32_t *const)(OS_MTIME_BASE_ADDRESS);

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
#else

    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(UCOS_TIMER_CLOCK, 0);
    gptmr_channel_get_default_config(UCOS_TIMER_RESOURCE, &config);
    gptmr_freq = clock_get_frequency(UCOS_TIMER_CLOCK);
    config.reload = gptmr_freq / OS_TICK_RATE_HZ;
    gptmr_channel_config(UCOS_TIMER_RESOURCE, UCOS_TIMER_CH, &config, false);
    gptmr_start_counter(UCOS_TIMER_RESOURCE, UCOS_TIMER_CH);

    gptmr_enable_irq(UCOS_TIMER_RESOURCE, GPTMR_CH_RLD_IRQ_MASK(UCOS_TIMER_CH));

    intc_m_enable_irq_with_priority(UCOS_TIMER_IRQ, 1);
#endif
}

#ifdef CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR
SDK_DECLARE_EXT_ISR_M(UCOS_TIMER_IRQ, vPortSysTimerIsr)
void vPortSysTimerIsr(void)
{
    if (gptmr_check_status(UCOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(UCOS_TIMER_CH))) {
        gptmr_clear_status(UCOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(UCOS_TIMER_CH));
        OSTimeTick();
    }
}
#endif
