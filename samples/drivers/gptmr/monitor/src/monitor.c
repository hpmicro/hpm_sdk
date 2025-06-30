/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"

#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME

#define APP_GPTMR_MONITOR_SEL         monitor_signal_period
#define APP_GPTMR_MONITOR_MIN_NS      (2000)
#define APP_GPTMR_MONITOR_MAX_NS      (1000000)

static void monitor_config(void);

volatile bool monitor_is_done;
volatile uint32_t gptmr_freq;

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH));
            monitor_is_done = true;
            /* Disable interrupts for processing */
            gptmr_disable_irq(APP_BOARD_GPTMR, GPTMR_CH_CAP_IRQ_MASK(APP_BOARD_GPTMR_CH));
    }
}

int main(void)
{
    monitor_is_done = false;
    board_init();
    board_init_gptmr_channel_pin(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, false);
    printf("gptmr monitor test, sel: %s\n", (APP_GPTMR_MONITOR_SEL == monitor_signal_period) ? "period" : "high_level_time");
    monitor_config();
    while (1) {
        while (!monitor_is_done) {
            __asm("nop");
        }
        printf("monitor the input signal is not within range <%lu, %lu> ns\n", APP_GPTMR_MONITOR_MIN_NS, APP_GPTMR_MONITOR_MAX_NS);
        printf("Ensure PWM input and signal frequency range\n\n");
        monitor_is_done = false;
        /* Re-enable interrupt monitoring */
        gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_CAP_IRQ_MASK(APP_BOARD_GPTMR_CH));
    }
    return 0;
}

static void monitor_config(void)
{
    gptmr_channel_config_t config;
    uint32_t gptmr_freq, gptmr_tick_ns;

    board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    printf("gptmr freq: %lu\n", gptmr_freq);
    gptmr_tick_ns = (1000000000 / gptmr_freq);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    config.enable_cmp_output = false;
    config.enable_monitor = true;
    config.monitor_config.monitor_type = APP_GPTMR_MONITOR_SEL;
    config.monitor_config.min_value = (APP_GPTMR_MONITOR_MIN_NS / gptmr_tick_ns);
    config.monitor_config.max_value = (APP_GPTMR_MONITOR_MAX_NS / gptmr_tick_ns);
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_CAP_IRQ_MASK(APP_BOARD_GPTMR_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

