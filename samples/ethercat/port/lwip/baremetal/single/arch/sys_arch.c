/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"

static volatile uint32_t sys_tick = 0;

void sys_timer_callback(void)
{
    sys_tick++;
}

u32_t sys_now(void)
{
    return (u32_t)sys_tick;
}

void sys_arch_msleep(u32_t delay_ms)
{
    uint32_t target_sys_tick = sys_now() + delay_ms;

    while (sys_now() < target_sys_tick) {

    }
}
