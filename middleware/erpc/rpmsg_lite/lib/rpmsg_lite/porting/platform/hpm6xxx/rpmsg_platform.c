/*
 * Copyright (c) 2022 HPMicro
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>

#include "rpmsg_platform.h"
#include "rpmsg_env.h"

#include "hpm_soc.h"
#include "hpm_clock_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_ipc_event_mgr.h"
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include "portmacro.h"
#endif


#if defined(RL_USE_ENVIRONMENT_CONTEXT) && (RL_USE_ENVIRONMENT_CONTEXT == 1)
#error "This RPMsg-Lite port requires RL_USE_ENVIRONMENT_CONTEXT set to 0"
#endif

static int32_t s_disable_counter = 0;

static void rpmsg_event_handler(uint16_t vring_idx, void *context)
{
    env_isr((uint32_t)vring_idx);
}

static void platform_global_isr_disable(void)
{
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

static void platform_global_isr_enable(void)
{
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
}

int32_t platform_init_interrupt(uint32_t vector_id, void *isr_data)
{
    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);

    return 0;
}

int32_t platform_deinit_interrupt(uint32_t vector_id)
{
    /* Unregister ISR from environment layer */
    env_unregister_isr(vector_id);

    return 0;
}

void platform_notify(uint32_t vector_id)
{
    (void)ipc_tigger_event(ipc_remote_rpmsg_event, (uint16_t)RL_GET_Q_ID(vector_id));
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(uint32_t num_msec)
{
    clock_cpu_delay_ms(num_msec);
}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */
int32_t platform_in_isr(void)
{
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    return xPortIsInsideInterrupt();
#else
    return 0u;
#endif
}

/**
 * platform_interrupt_enable
 *
 * Enable peripheral-related interrupt
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int32_t platform_interrupt_enable(uint32_t vector_id)
{
    RL_ASSERT(0 < s_disable_counter);

    platform_global_isr_disable();
    s_disable_counter--;

    if (s_disable_counter == 0)
    {
        ipc_enable_event_interrupt(2);
    }
    platform_global_isr_enable();
    return ((int32_t)vector_id);
}

/**
 * platform_interrupt_disable
 *
 * Disable peripheral-related interrupt.
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int32_t platform_interrupt_disable(uint32_t vector_id)
{
    RL_ASSERT(0 <= s_disable_counter);

    platform_global_isr_disable();
    /* virtqueues use the same NVIC vector
       if counter is set - the interrupts are disabled */
    if (s_disable_counter == 0)
    {
        ipc_disable_event_interrupt();
    }

    s_disable_counter++;
    platform_global_isr_enable();
    return ((int32_t)vector_id);
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(uint32_t vrt_addr, uint32_t phy_addr, uint32_t size, uint32_t flags)
{
}

/**
 * platform_cache_all_flush_invalidate
 */
void platform_cache_all_flush_invalidate(void)
{
    l1c_dc_flush_all();
}

/**
 * platform_cache_disable
 */
void platform_cache_disable(void)
{
    l1c_dc_disable();
}

/**
 * platform_vatopa
 */
uint32_t platform_vatopa(void *addr)
{
    return ((uint32_t)(char *)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(uint32_t addr)
{
    return ((void *)(char *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int32_t platform_init(void)
{
    hpm_stat_t state;

    state = ipc_register_event(ipc_remote_rpmsg_event, rpmsg_event_handler, ((void *)0));

    return (state == status_success) ? 0 : -1;
}

/**
 * platform_deinit
 *
 * platform/environment deinit process
 */
int32_t platform_deinit(void)
{
    return 0;
}

