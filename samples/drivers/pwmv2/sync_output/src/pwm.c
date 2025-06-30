/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_pwmv2_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_synt_drv.h"

#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT1
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT3
#define PWM_OUTPUT_PIN3 BOARD_APP_PWM_OUT5
#define TRGM BOARD_APP_TRGM
#endif

#define PWM_PERIOD_IN_MS (36)

uint32_t reload, freq;

void pwm_fault_async(void)
{
    pwmv2_async_fault_source_config_t fault_cfg;

    fault_cfg.async_signal_from_pad_index = BOARD_APP_PWM_FAULT_PIN;
    fault_cfg.fault_async_pad_level = pad_fault_active_high;
    pwmv2_config_async_fault_source(PWM, PWM_OUTPUT_PIN1, &fault_cfg);
    pwmv2_config_async_fault_source(PWM, PWM_OUTPUT_PIN2, &fault_cfg);
    pwmv2_config_async_fault_source(PWM, PWM_OUTPUT_PIN3, &fault_cfg);
    pwmv2_set_fault_mode(PWM, PWM_OUTPUT_PIN1, pwm_fault_output_0);
    pwmv2_set_fault_mode(PWM, PWM_OUTPUT_PIN2, pwm_fault_output_0);
    pwmv2_set_fault_mode(PWM, PWM_OUTPUT_PIN3, pwm_fault_output_0);
    pwmv2_enable_async_fault(PWM, PWM_OUTPUT_PIN1);
    pwmv2_enable_async_fault(PWM, PWM_OUTPUT_PIN2);
    pwmv2_enable_async_fault(PWM, PWM_OUTPUT_PIN3);
}

static void init_synt_timebase(void)
{
    synt_reset_counter(HPM_SYNT);
    synt_set_reload(HPM_SYNT, reload);
    synt_set_comparator(HPM_SYNT, SYNT_CMP_0, (reload >> 3));
    synt_set_comparator(HPM_SYNT, SYNT_CMP_1, (reload >> 2));
    synt_set_comparator(HPM_SYNT, SYNT_CMP_2, (reload >> 1));
}

static void sync_start(void)
{
    synt_enable_counter(HPM_SYNT, true);
}

static void sync_stop(void)
{
    synt_enable_counter(HPM_SYNT, false);
    synt_reset_counter(HPM_SYNT);
}

static void init_start_trgm_connect(void)
{
    trgm_output_t trgm0_io_config0 = {0};
    trgm0_io_config0.invert = 0;
    trgm0_io_config0.type = trgm_output_pulse_at_input_rising_edge;
    trgm0_io_config0.input = HPM_TRGM0_INPUT_SRC_SYNT_CH00;
    trgm_output_config(HPM_TRGM0, BOARD_APP_TRGM_PWM_OUTPUT, &trgm0_io_config0);

    trgm_output_t trgm0_io_config1 = {0};
    trgm0_io_config1.invert = 0;
    trgm0_io_config1.type = trgm_output_pulse_at_input_rising_edge;
    trgm0_io_config1.input = HPM_TRGM0_INPUT_SRC_SYNT_CH00;
    trgm_output_config(HPM_TRGM0, BOARD_APP_TRGM_PWM_OUTPUT1, &trgm0_io_config1);

    trgm_output_t trgm0_io_config2 = {0};
    trgm0_io_config2.invert = 0;
    trgm0_io_config2.type = trgm_output_pulse_at_input_rising_edge;
    trgm0_io_config2.input = HPM_TRGM0_INPUT_SRC_SYNT_CH00;
    trgm_output_config(HPM_TRGM0, BOARD_APP_TRGM_PWM_OUTPUT2, &trgm0_io_config2);
}

static void init_phase_trgm_connect(void)
{
    trgm_output_t trgm0_io_config0 = {0};
    trgm0_io_config0.invert = 0;
    trgm0_io_config0.type = trgm_output_pulse_at_input_rising_edge;
    trgm0_io_config0.input = HPM_TRGM0_INPUT_SRC_SYNT_CH00;
    trgm_output_config(HPM_TRGM0, BOARD_APP_TRGM_PWM_OUTPUT, &trgm0_io_config0);

    trgm_output_t trgm0_io_config1 = {0};
    trgm0_io_config1.invert = 0;
    trgm0_io_config1.type = trgm_output_pulse_at_input_rising_edge;
    trgm0_io_config1.input = HPM_TRGM0_INPUT_SRC_SYNT_CH01;
    trgm_output_config(HPM_TRGM0, BOARD_APP_TRGM_PWM_OUTPUT1, &trgm0_io_config1);

    trgm_output_t trgm0_io_config2 = {0};
    trgm0_io_config2.invert = 0;
    trgm0_io_config2.type = trgm_output_pulse_at_input_rising_edge;
    trgm0_io_config2.input = HPM_TRGM0_INPUT_SRC_SYNT_CH02;
    trgm_output_config(HPM_TRGM0, BOARD_APP_TRGM_PWM_OUTPUT2, &trgm0_io_config2);
}

static void pwm_sync_three_submodules_handware_config(void)
{
    pwmv2_enable_shadow_lock_feature(PWM);
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload - 1, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload >> 2, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), (reload >> 2) * 3, 0, 0);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(4), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(5), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(8), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(9), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_reload_select_input_trigger(PWM, pwm_counter_0, 0);
    pwmv2_reload_select_input_trigger(PWM, pwm_counter_1, 1);
    pwmv2_reload_select_input_trigger(PWM, pwm_counter_2, 2);

    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_trigger);
    pwmv2_set_reload_update_time(PWM, pwm_counter_1, pwm_reload_update_on_trigger);
    pwmv2_set_reload_update_time(PWM, pwm_counter_2, pwm_reload_update_on_trigger);

    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_counter_burst_disable(PWM, pwm_counter_1);
    pwmv2_counter_burst_disable(PWM, pwm_counter_2);
    pwmv2_shadow_register_lock(PWM);

    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN1);
    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN2);
    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN3);

    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN1);
    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN2);
    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN3);

    pwmv2_reset_counter(PWM, pwm_counter_0);
    pwmv2_reset_counter(PWM, pwm_counter_1);
    pwmv2_reset_counter(PWM, pwm_counter_2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_enable_counter(PWM, pwm_counter_1);
    pwmv2_enable_counter(PWM, pwm_counter_2);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_1);
    pwmv2_start_pwm_output(PWM, pwm_counter_2);
}

/* restore the initial state of pwmv2 */
void pwm_sync_clean(void)
{
/* clear the values of reload and compare working registers */
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), 0, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), 0, 0, 0);
    pwmv2_shadow_register_lock(PWM);
    board_delay_ms(50);
    sync_stop();
    pwmv2_deinit(PWM);
}

void pwm_start_same_time(void)
{
    init_start_trgm_connect();
    pwm_sync_three_submodules_handware_config();
    board_delay_ms(2000);
    sync_start();
}

void pwm_start_different_phase(void)
{
    init_phase_trgm_connect();
    pwm_sync_three_submodules_handware_config();
    board_delay_ms(2000);
    sync_start();
}

int main(void)
{
    board_init();
    init_pwm_pins(PWM);
    printf("pwmv2 three pwm submodule synchronous example\n");
    printf("choose PWM output channel [P%d P%d P%d]\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2, PWM_OUTPUT_PIN3);
/* set period 36ms */
    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS;
    init_synt_timebase();

    pwm_fault_async();
    printf("\n\n>> P%d P%d P%d generate waveform at same time\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2, PWM_OUTPUT_PIN3);
    printf("P%d is a reference\n", PWM_OUTPUT_PIN1);
    pwm_start_same_time();
    board_delay_ms(1000);
    pwm_sync_clean();
    printf("\n\n>> Phase different P%d P%d P%d is 45 degrees 135 degrees\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2, PWM_OUTPUT_PIN3);
    printf("P%d is a reference\n", PWM_OUTPUT_PIN1);
    pwm_start_different_phase();
    printf("test done\n");
    while (1) {
        ;
    }
    return 0;
}
