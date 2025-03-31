/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_pwmv2_drv.h"
#include "pwm.h"
#include "board.h"

#define TEST_PWM BOARD_APP_PWM
#define TEST_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME

void pwm_config_output(void)
{
    init_pwm_pins(TEST_PWM);

    pwmv2_disable_counter(TEST_PWM, pwm_counter_0);
    pwmv2_reset_counter(TEST_PWM, pwm_counter_0);
    pwmv2_disable_counter(TEST_PWM, pwm_counter_1);
    pwmv2_reset_counter(TEST_PWM, pwm_counter_1);

    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT2);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT3);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT4);
}

void pwm_generate_central_aligned_waveform(void)
{
    uint32_t freq, reload;
    freq = clock_get_frequency(TEST_CLOCK_NAME);
    reload = freq / 20000 - 1; /* 20KHz */

    init_pwm_pins(TEST_PWM);

    pwmv2_disable_counter(TEST_PWM, pwm_counter_0);
    pwmv2_reset_counter(TEST_PWM, pwm_counter_0);
    pwmv2_disable_counter(TEST_PWM, pwm_counter_1);
    pwmv2_reset_counter(TEST_PWM, pwm_counter_1);
    pwmv2_shadow_register_unlock(TEST_PWM);

    pwmv2_set_shadow_val(TEST_PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(TEST_PWM, PWMV2_SHADOW_INDEX(1), ((reload >> 1) - (reload >> 2)), 0, false);
    pwmv2_set_shadow_val(TEST_PWM, PWMV2_SHADOW_INDEX(2), ((reload >> 1) + (reload >> 2)), 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(TEST_PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(TEST_PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(TEST_PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(TEST_PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(TEST_PWM, pwm_counter_1);
    pwmv2_set_reload_update_time(TEST_PWM, pwm_counter_1, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(2), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(3), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(4), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(5), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(6), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(TEST_PWM, PWMV2_CMP_INDEX(7), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_shadow_register_lock(TEST_PWM);
    pwmv2_disable_four_cmp(TEST_PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT2);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT3);
    pwmv2_channel_enable_output(TEST_PWM, BOARD_APP_PWM_OUT4);
    pwmv2_enable_counter(TEST_PWM, pwm_counter_0);
    pwmv2_start_pwm_output(TEST_PWM, pwm_counter_0);
    pwmv2_enable_counter(TEST_PWM, pwm_counter_1);
    pwmv2_start_pwm_output(TEST_PWM, pwm_counter_1);
}