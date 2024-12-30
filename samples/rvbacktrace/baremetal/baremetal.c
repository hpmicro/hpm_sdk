/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "rvbacktrace.h"
#include "hpm_debug_console.h"
#include "hpm_gptmr_drv.h"

#define APP_TICK_MS                   (1000)
uint32_t cycle;
volatile static bool timer_flag = false;

int function_recursion(int n)
{
    if (n <= 1) {
        return n;
    }
    cycle++;
    printf("F[%s]: recursion cycle[%u]\n", __func__, cycle);
    rvbacktrace();
    return function_recursion(n - 1);
}

uint32_t function_trap_B(uint32_t *ptr)
{
    volatile uint32_t value;

    printf("GET FAULT ADDRESS, ENTER TRAP\n");
    value = *ptr;
    return value;
}

uint32_t function_trap_A(void)
{
    volatile uint32_t fault_addr = 0xFFFFFFFF;

    function_trap_B((uint32_t *)fault_addr);
    return 0;
}

long exception_handler(long cause, long epc)
{
    rvbacktrace_trap(&cause, &epc);
    while (1) {
    }
    return epc;
}

static int function_isr(void)
{
    printf("ENTER ISR\n");
    timer_flag = true;
    rvbacktrace();
    return 0;
}

SDK_DECLARE_EXT_ISR_M(BOARD_GPTMR_IRQ, timer_ms_isr);
void timer_ms_isr(void)
{
    if (gptmr_check_status(BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(BOARD_GPTMR_CHANNEL))) {
        gptmr_clear_status(BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(BOARD_GPTMR_CHANNEL));
        function_isr();
    }
}

static void timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(BOARD_GPTMR_CLK_NAME, 0);
    gptmr_channel_get_default_config(BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(BOARD_GPTMR_CLK_NAME);
    config.reload = gptmr_freq / 1000 * APP_TICK_MS;
    gptmr_channel_config(BOARD_GPTMR, BOARD_GPTMR_CHANNEL, &config, false);
    gptmr_start_counter(BOARD_GPTMR, BOARD_GPTMR_CHANNEL);

    gptmr_enable_irq(BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(BOARD_GPTMR_CHANNEL));
    intc_m_enable_irq_with_priority(BOARD_GPTMR_IRQ, 1);
}

int main(void)
{
    int n = 5;
    board_init();
    timer_config();
    while (1) {
        if (timer_flag) {
            gptmr_disable_irq(BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(BOARD_GPTMR_CHANNEL));
            break;
        }
    }
    function_recursion(n);
    function_trap_A();
    while (1) {
    }
    return 0;
}
