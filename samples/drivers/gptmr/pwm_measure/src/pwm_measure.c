/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_debug_console.h"

#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME

static void pwm_measure_config(void);

volatile uint32_t gptmr_freq;

int main(void)
{
    uint32_t period, duty;
    board_init();
    init_gptmr_pins(APP_BOARD_GPTMR);
    printf("gptmr pwm measure test\n");
    pwm_measure_config();
    while (1) {
        /* please make sure deplay time is more than one pwm cycle */
        board_delay_ms(200);
        period = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_measured_period);
        duty = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_measured_duty_cycle);
        /* in pwm measure mode, the CAPF will set to 1 when capture the rising edge signal,so can used the flag when whether to collect pwm */
        if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH))) {
            gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH));
            printf("measured frequency: %f\n", (float) gptmr_freq / period);
            printf("measured duty cycle: %.2f%%\n", ((float) duty / period) * 100);
        }
    }
    return 0;
}

static void pwm_measure_config(void)
{
    gptmr_channel_config_t config;
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    config.mode = gptmr_work_mode_measure_width;
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
}


