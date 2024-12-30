/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "timer.h"
#include "hpm_sysctl_drv.h"
#include "hpm_gptmr_drv.h"

#define TIMER_GPTMR               BOARD_GPTMR
#define TIMER_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define TIMER_GPTMR_IRQ           BOARD_GPTMR_IRQ
#define TIMER_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME
#define TIMER_REGISTER_COUNT      (10U)
#define TIMER_TICK_MS             (1)

typedef struct {
    bool registered;
    uint32_t count;
    hpm_timer_t timer;
} timer_cfg_t;

static volatile timer_cfg_t timer_table[TIMER_REGISTER_COUNT];

SDK_DECLARE_EXT_ISR_M(TIMER_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    if (gptmr_check_status(TIMER_GPTMR, GPTMR_CH_RLD_STAT_MASK(TIMER_GPTMR_CH))) {
        gptmr_clear_status(TIMER_GPTMR, GPTMR_CH_RLD_STAT_MASK(TIMER_GPTMR_CH));
        for (uint32_t i = 0; i < (sizeof(timer_table) / sizeof(timer_cfg_t)); i++) {
            if (timer_table[i].registered == true) {
                timer_table[i].count++;
                if (timer_table[i].count == timer_table[i].timer.timing_ms) {
                    timer_table[i].count = 0;
                    timer_table[i].timer.callback();
                }
            }
        }
    }
}

void timer_init(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(TIMER_GPTMR_CLOCK, 0);
    gptmr_channel_get_default_config(TIMER_GPTMR, &config);

    gptmr_freq = clock_get_frequency(TIMER_GPTMR_CLOCK);
    config.reload = gptmr_freq / 1000 * TIMER_TICK_MS;
    gptmr_channel_config(TIMER_GPTMR, TIMER_GPTMR_CH, &config, false);
    gptmr_start_counter(TIMER_GPTMR, TIMER_GPTMR_CH);

    gptmr_enable_irq(TIMER_GPTMR, GPTMR_CH_RLD_IRQ_MASK(TIMER_GPTMR_CH));
    intc_m_enable_irq_with_priority(TIMER_GPTMR_IRQ, 1);
    for (uint32_t i = 0; i < (sizeof(timer_table) / sizeof(timer_cfg_t)); i++) {
        timer_table[i].registered = false;
    }
}

hpm_stat_t timer_register(hpm_timer_t *timer)
{
    hpm_stat_t sta = status_fail;
    if ((timer->timing_ms == 0) || (timer->callback == NULL)) {
        return status_invalid_argument;
    }
    for (uint32_t i = 0; i < (sizeof(timer_table) / sizeof(timer_cfg_t)); i++) {
        if (timer_table[i].registered == false) {
            timer_table[i].registered = true;
            timer_table[i].count = 0;
            timer_table[i].timer.timing_ms = timer->timing_ms;
            timer_table[i].timer.callback = timer->callback;
            sta = status_success;
            break;
        }
    }
    return sta;
}
