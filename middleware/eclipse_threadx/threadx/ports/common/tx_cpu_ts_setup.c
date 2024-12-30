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
#ifdef TX_LOW_POWER
/* Store the machine timer compare value before configure machine timer */
volatile uint64_t cmp_before_wfi;

/* SOC may enter wfi state without configure the timer, so we alloc a flag to
 * indicate wether we have configured the timer. */
volatile bool flag_mtmr_configured = false;
VOID low_power_timer_config(ULONG ticks)
{
    volatile uint32_t hart_id;
    if (ticks < 1)
        return;
    __asm volatile("csrr %0, mhartid"
                   : "=r"(hart_id));
    p_mchtmr_cmp_reg = (volatile uint64_t *)(p_mchtmr_cmp_reg_base + (hart_id * sizeof(uint64_t)));
    cmp_before_wfi = os_next_tic;
    os_next_tic += (uint64_t)(timer_increment_per_tick * ticks);

    /* Set machine timer's compare register to tick that we want to wake up */
    *p_mchtmr_cmp_reg = os_next_tic;
    flag_mtmr_configured = true;
}

ULONG low_power_timer_adjust(VOID)
{
    uint64_t current_time;
    if (flag_mtmr_configured == false)
        return 0;
    else
        flag_mtmr_configured = false;
    uint32_t complete_periods;
    /* 8-byte typer so high 32-bit word is 4 bytes up. */
    volatile uint64_t *const p_mchtmr_counter = (volatile uint64_t *)(OS_MTIME_BASE_ADDRESS);
    volatile uint32_t hart_id;

    __asm volatile("csrr %0, mhartid"
                   : "=r"(hart_id));
    p_mchtmr_cmp_reg = (volatile uint64_t *)(p_mchtmr_cmp_reg_base + (hart_id * sizeof(uint64_t)));

    current_time = *p_mchtmr_counter;
    /* Has the specified wake-up time been reached */
    if (current_time < cmp_before_wfi) {
        complete_periods = 0;
        *p_mchtmr_cmp_reg = cmp_before_wfi;
        os_next_tic = cmp_before_wfi;
    } else {
        complete_periods = (current_time - cmp_before_wfi) / timer_increment_per_tick + 1;
        *p_mchtmr_cmp_reg = cmp_before_wfi + ((complete_periods + 0) * timer_increment_per_tick);
        os_next_tic = *p_mchtmr_cmp_reg;
    }
    return complete_periods;
}
#endif
#else
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
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

#ifdef TX_LOW_POWER
#include "hpm_sysctl_drv.h"
#include "hpm_pcfg_drv.h"
#ifndef THREADX_LOWPOWER_TIMER_RESOURCE
    #define THREADX_LOWPOWER_TIMER_RESOURCE      BOARD_THREADX_LOWPOWER_TIMER
#endif

#ifndef THREADX_LOWPOWER_TIMER_CH
    #define THREADX_LOWPOWER_TIMER_CH            BOARD_THREADX_LOWPOWER_TIMER_CHANNEL
#endif

#ifndef THREADX_LOWPOWER_TIMER_IRQ
    #define THREADX_LOWPOWER_TIMER_IRQ           BOARD_THREADX_LOWPOWER_TIMER_IRQ
#endif

#ifndef THREADX_LOWPOWER_TIMER_CLOCK
    #define THREADX_LOWPOWER_TIMER_CLOCK         BOARD_THREADX_LOWPOWER_TIMER_CLK_NAME
#endif

/* Store the count number of lowpower timer during one system timer tick period */
volatile uint32_t lowpower_timer_count_per_tick;

/* Flag that indicate wether the lowpower timer is already reloaded,
 *    Ture: the lowpower timer is reloaded, SOC has sleep for expected ticks
 *    False: SOC wake up by other interrupts, time during sleep should be calculated.
 */
volatile bool lowpower_reloaded = false;

/* Store the expected number of ticks */
volatile uint32_t expect_ticks;

/* SOC may enter wfi state without configure the lowpower timer, so we alloc a flag to
 * indicate wether we have configured the lowpower timer. */
volatile bool flag_lowpower_timer_configed = false;
SDK_DECLARE_EXT_ISR_M(BOARD_THREADX_LOWPOWER_TIMER_IRQ, lowpower_tick_isr)
void lowpower_tick_isr(void)
{
    if (gptmr_check_status(THREADX_LOWPOWER_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(THREADX_LOWPOWER_TIMER_CH))) {
        gptmr_clear_status(THREADX_LOWPOWER_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(THREADX_LOWPOWER_TIMER_CH));
        lowpower_reloaded = true;
    }
}
#endif /* TX_LOW_POWER */
SDK_DECLARE_EXT_ISR_M(THREADX_TIMER_IRQ, system_tick_isr)
void system_tick_isr(void)
{
    if (gptmr_check_status(THREADX_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(THREADX_TIMER_CH))) {
        gptmr_clear_status(THREADX_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(THREADX_TIMER_CH));
        _tx_timer_interrupt();
    }
}
void tx_cpu_ts_setup(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(THREADX_TIMER_CLOCK, 0);
    gptmr_channel_get_default_config(THREADX_TIMER_RESOURCE, &config);
    gptmr_freq = clock_get_frequency(THREADX_TIMER_CLOCK);
    config.reload = gptmr_freq / TX_TIMER_TICKS_PER_SECOND;
    gptmr_channel_config(THREADX_TIMER_RESOURCE, THREADX_TIMER_CH, &config, false);
    gptmr_start_counter(THREADX_TIMER_RESOURCE, THREADX_TIMER_CH);

    gptmr_enable_irq(THREADX_TIMER_RESOURCE, GPTMR_CH_RLD_IRQ_MASK(THREADX_TIMER_CH));
    intc_m_enable_irq_with_priority(THREADX_TIMER_IRQ, 2);
#ifdef TX_LOW_POWER
    /* Configure the lowpower timer at start */
    clock_add_to_group(THREADX_LOWPOWER_TIMER_CLOCK, 0);
    gptmr_channel_get_default_config(THREADX_LOWPOWER_TIMER_RESOURCE, &config);
    config.debug_mode = false;
    gptmr_freq = clock_get_frequency(THREADX_LOWPOWER_TIMER_CLOCK);
    config.reload = gptmr_freq / TX_TIMER_TICKS_PER_SECOND;
    gptmr_channel_config(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH, &config, false);
    gptmr_enable_irq(THREADX_LOWPOWER_TIMER_RESOURCE, GPTMR_CH_RLD_IRQ_MASK(THREADX_LOWPOWER_TIMER_CH));
    intc_m_enable_irq_with_priority(THREADX_LOWPOWER_TIMER_IRQ, 2);
    lowpower_timer_count_per_tick = gptmr_freq / TX_TIMER_TICKS_PER_SECOND;
    /* Only shut down CPU clock, enter cpu WAIT mode */
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_gate_cpu_clock);
    pcfg_disable_power_trap(HPM_PCFG);
#endif
}
#ifdef TX_LOW_POWER
VOID low_power_timer_config(ULONG ticks)
{
    uint32_t reload_value;
    /* Sleep for (ticks -1) period, because the system tick source will resume after wakeup */
    reload_value = (ticks) * lowpower_timer_count_per_tick;
    expect_ticks = ticks;
    if (expect_ticks == 0)
        return;
    /* Stop the system timer, we use lowpower timer during the sleep time */
    gptmr_stop_counter(THREADX_TIMER_RESOURCE, THREADX_TIMER_CH);
    
    /* Set the new reload value. */
    gptmr_channel_config_update_reload(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH, reload_value);
    gptmr_channel_update_count(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH, 0);
    /* Start lowpower timer. */
    gptmr_start_counter(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH);
    flag_lowpower_timer_configed = true;
}

ULONG low_power_timer_adjust(VOID)
{
    uint32_t complete_periods;

    /* Do nothing if lowpower timer is not configured */
    if (flag_lowpower_timer_configed == false)
        return 0;
    else
        flag_lowpower_timer_configed = false;
    /* Stop the lowpower timer */
    gptmr_stop_counter(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH);
    if (lowpower_reloaded) {
        complete_periods = expect_ticks;
    } else {
        uint32_t current = gptmr_channel_get_counter(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH, gptmr_counter_type_normal);
        complete_periods = current / lowpower_timer_count_per_tick;
    }
    gptmr_channel_update_count(THREADX_LOWPOWER_TIMER_RESOURCE, THREADX_LOWPOWER_TIMER_CH, 0);

    /* Start the system tick, so we recover the point before sleep */
    gptmr_start_counter(THREADX_TIMER_RESOURCE, THREADX_TIMER_CH);
    return complete_periods;
}
#endif
#endif
