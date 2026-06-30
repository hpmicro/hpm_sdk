/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif
#include "hpm_clock_drv.h"

#define APP_BOARD_GPTMR               BOARD_GPTMR_PWM
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_PWM_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_PWM_IRQ
#define APP_BOARD_GPTMR_CMPINIT       (0)         /*it's mean compare output start state, now is low*/
#define APP_BOARD_RELOAD_MS           (1000)
#define APP_BOARD_CMP_MS              (300)

#ifndef APP_GPTMR_TARGET_FREQ
#define APP_GPTMR_TARGET_FREQ         (100000UL) /* 100KHz */
#endif

static void output_compare_config(void);

volatile bool time_is_up;
volatile uint32_t count;

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0));
        time_is_up = true;
        count = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_normal);
    }
}

int main(void)
{
    time_is_up = false;
    board_init();
    init_gptmr_pins(APP_BOARD_GPTMR);
    printf("gptmr output compare test\n");
    output_compare_config();
    while (1) {
        while (!time_is_up) {
            __asm("nop");
        }
        time_is_up = false;
        printf("compare time is up: %d\n", count);
    }
    return 0;
}

static void output_compare_config(void)
{
    gptmr_channel_config_t config;
    uint32_t gptmr_freq;

    gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
    /* Target timer clock = 100KHz.
     * prescaler = source_clock / target = gptmr_freq / 100000
     * E.g. 100MHz / 1000 = 100KHz, each tick = 10us.
     */
    config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
    config.reload = ((uint64_t)APP_BOARD_RELOAD_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
    config.cmp[0] = ((uint64_t)APP_BOARD_CMP_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
#else
    config.reload = gptmr_freq / 1000 * APP_BOARD_RELOAD_MS;
    config.cmp[0] = gptmr_freq / 1000 * APP_BOARD_CMP_MS;
#endif
    config.cmp[1] = config.reload;
    config.cmp_initial_polarity_high = APP_BOARD_GPTMR_CMPINIT;

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_CMP_IRQ_MASK(APP_BOARD_GPTMR_CH, 0));
    if (gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false) != status_success) {
        printf("config gptmr channel failed\n");
        return;
    }
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}
