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

static void input_capture_config(void);

volatile bool capture_is_done;
volatile int frist_rising_count;
volatile int next_rising_count;
volatile bool frist_rised;
volatile uint32_t gptmr_freq;

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH));
        if (frist_rised == false) {
            frist_rising_count = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_rising_edge);
            frist_rised = true;
        } else {
            next_rising_count = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_rising_edge);
            frist_rised = false;
            capture_is_done = true;
        }
    }
}

int main(void)
{
    int tmp_time;
    capture_is_done = false;
    frist_rised = false;
    board_init();
    init_gptmr_pins(APP_BOARD_GPTMR);
    printf("gptmr input capture test\n");
    input_capture_config();
    while (1) {
        while (!capture_is_done) {
            __asm("nop");
        }
        /* tick depends on the gptmr clock, if the clock is 100Mhz, the tick is 10ns*/
        tmp_time = abs(frist_rising_count - next_rising_count);
        printf("capture the pulse width : %d tick\n", tmp_time);
        capture_is_done = false;
    }
    return 0;
}

static void input_capture_config(void)
{
    gptmr_channel_config_t config;

    board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    config.mode = gptmr_work_mode_capture_at_rising_edge;
    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_CAP_IRQ_MASK(APP_BOARD_GPTMR_CH));
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

