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

static void cntmode_config(void);

static volatile bool reload_flag;

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        board_led_toggle();
        reload_flag = true;
    }
}

int main(void)
{
    reload_flag = false;
    board_init();
    board_init_led_pins();
    board_init_gptmr_channel_pin(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, false);
    printf("gptmr cnt_mode test\n");
    cntmode_config();
    while (1) {
        if (reload_flag == true) {
            reload_flag = false;
            printf("reload trigger\n");
        }
    }
    return 0;
}

static void cntmode_config(void)
{
    gptmr_channel_config_t config;

    board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    config.counter_mode = gptmr_counter_mode_external;
    config.reload = 0xFFFF;
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

