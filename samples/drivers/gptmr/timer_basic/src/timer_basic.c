/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif
#include "hpm_debug_console.h"

#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ

#define APP_TICK_MS                   (1000)

#ifndef APP_GPTMR_TARGET_FREQ
#define APP_GPTMR_TARGET_FREQ         (100000UL) /* 100KHz */
#endif

static void timer_config(void);

static volatile bool timer_flag;

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        board_led_toggle();
        timer_flag = true;
    }
}

int main(void)
{
    timer_flag = false;
    board_init();
    board_init_led_pins();
    init_gptmr_pins(APP_BOARD_GPTMR);
    printf("gptmr timer basic test\n");
    timer_config();
    while (1) {
        if (timer_flag == true) {
            timer_flag = false;
            printf("timer tick %d ms\n", APP_TICK_MS);
        }
    }
}

static void timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
    /* Target timer clock = 100KHz.
     * prescaler = source_clock / target = gptmr_freq / 100000
     * E.g. 100MHz / 1000 = 100KHz, each tick = 10us.
     */
    config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
    config.reload = ((uint64_t)APP_TICK_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
#else
    config.reload = gptmr_freq / 1000 * APP_TICK_MS;
#endif
    if (gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false) != status_success) {
        printf("config gptmr channel failed\n");
        return;
    }
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

