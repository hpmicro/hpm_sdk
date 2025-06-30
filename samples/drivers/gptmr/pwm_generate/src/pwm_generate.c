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

#define APP_BOARD_PWM                 BOARD_GPTMR_PWM
#define APP_BOARD_PWM_CH              BOARD_GPTMR_PWM_CHANNEL

#define APP_PWM_MAX_FREQ                  (100000U)
#define APP_PWM_MIN_FREQ                  (10000U)
#define APP_PWM_MAX_DUTY                  (100U)
#define APP_PWM_MAX_DUTY_STEP             (2U)
/**
 * @brief set waveform edge aligned pwm frequency
 *
 * @param [in] freq pwm frequency, the unit is HZ
 */

static void set_pwm_waveform_edge_aligned_frequency(uint32_t freq);

/**
 * @brief set waveform edge aligned pwm duty
 *
 * @param [in] duty pwm duty, the value range is 1 to 100
 */

static void set_pwm_waveform_edge_aligned_duty(uint8_t duty);

uint32_t current_reload;

int main(void)
{
    board_init();

    init_gptmr_pins(APP_BOARD_PWM);
    printf("pwm generate testing\n");
    set_pwm_waveform_edge_aligned_frequency(1000);
    set_pwm_waveform_edge_aligned_duty(20);
    while (true) {
        for (uint32_t i = APP_PWM_MIN_FREQ; i < APP_PWM_MAX_FREQ; i++) {
            set_pwm_waveform_edge_aligned_frequency(i);
            for (uint8_t j = APP_PWM_MAX_DUTY_STEP; j < APP_PWM_MAX_DUTY + 1; j += APP_PWM_MAX_DUTY_STEP) {
                set_pwm_waveform_edge_aligned_duty(j);
                board_delay_ms(100);
            }
        }
    }
}

static void set_pwm_waveform_edge_aligned_frequency(uint32_t freq)
{
    gptmr_channel_config_t config;
    uint32_t gptmr_freq;

    gptmr_freq = board_init_gptmr_clock(APP_BOARD_PWM);
    gptmr_channel_get_default_config(APP_BOARD_PWM, &config);
    current_reload = gptmr_freq / freq;
    config.reload = current_reload;
    config.cmp_initial_polarity_high = false;
    gptmr_stop_counter(APP_BOARD_PWM, APP_BOARD_PWM_CH);
    gptmr_channel_config(APP_BOARD_PWM, APP_BOARD_PWM_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_PWM, APP_BOARD_PWM_CH);
    gptmr_start_counter(APP_BOARD_PWM, APP_BOARD_PWM_CH);
}

static void set_pwm_waveform_edge_aligned_duty(uint8_t duty)
{
    uint32_t cmp;
    if (duty > 100) {
        duty = 100;
    }
    cmp = (current_reload * duty) / 100;
    gptmr_update_cmp(APP_BOARD_PWM, APP_BOARD_PWM_CH, 0, cmp);
    gptmr_update_cmp(APP_BOARD_PWM, APP_BOARD_PWM_CH, 1, current_reload);
}

