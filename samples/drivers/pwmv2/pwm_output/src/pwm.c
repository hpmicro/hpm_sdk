/*
 * Copyright (c) 2021 HPMicro
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

#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT2
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT3
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#ifndef TEST_LOOP
#define TEST_LOOP (200)
#endif

#define PWM_PERIOD_IN_MS (1)
uint32_t reload;
void pwm_fault_async(void);

void generate_edge_aligned_waveform(void)
{
    uint32_t duty, duty_step;
    bool increase_duty_cycle = true;

    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload + 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload + 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), reload >> 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(4), reload, 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(2), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(3));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(3), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(4));

    pwmv2_shadow_register_lock(PWM);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, false);
    pwmv2_shadow_register_lock(PWM);
    pwm_fault_async();

    board_delay_ms(100);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload, 0, false);
    pwmv2_shadow_register_lock(PWM);

    duty_step = reload / 100;
    duty = reload / 100;
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        if (increase_duty_cycle) {
            if ((duty + duty_step) >= reload) {
                increase_duty_cycle = false;
                continue;
            }
            duty += duty_step;
        } else {
            if (duty <= duty_step) {
                increase_duty_cycle = true;
                continue;
            }
            duty -= duty_step;
        }
        pwmv2_shadow_register_unlock(PWM);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload - duty, 0, false);
        pwmv2_shadow_register_lock(PWM);
        board_delay_ms(50);
    }
    /**
     * @brief frequency
     *
     */
    increase_duty_cycle = true;
    uint32_t times = 0;
    uint32_t test_reload = 0;
    do {
        for (uint32_t i = 0; i < 100; i++) {
            if (increase_duty_cycle) {
                test_reload += 50000000 / 10000;
            } else {
                test_reload -= 50000000 / 10000;
            }
            if (test_reload <= 10) {
                test_reload = 50000000 / 10000;
            }
            pwmv2_shadow_register_unlock(PWM);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), test_reload, 0, false);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), test_reload >> 1, 0, false);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), test_reload + 1, 0, false);
            pwmv2_shadow_register_lock(PWM);
            board_delay_ms(20);
        }
        times++;
        increase_duty_cycle = !increase_duty_cycle;
    } while (times < 10);
    /**
     * @brief resume reload
     *
     */
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), test_reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), test_reload + 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), test_reload + 1, 0, false);
    pwmv2_shadow_register_lock(PWM);
    /**
     * @brief 100% duty
     *
     */
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), test_reload + 1, 0, false);
    pwmv2_shadow_register_lock(PWM);

}

void generate_central_aligned_waveform(void)
{
    uint32_t duty, duty_step;
    bool increase_duty_cycle = true;

    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload + 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), reload >> 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(4), reload, 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(2), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(3));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(3), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(4));

    pwmv2_shadow_register_lock(PWM);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
    pwm_fault_async();

    duty_step = reload / 100;
    duty = reload / 100;
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        if (increase_duty_cycle) {
            if ((duty + duty_step) >= reload) {
                increase_duty_cycle = false;
                continue;
            }
            duty += duty_step;
        } else {
            if (duty <= duty_step) {
                increase_duty_cycle = true;
                continue;
            }
            duty -= duty_step;
        }
        pwmv2_shadow_register_unlock(PWM);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), (reload - duty) >> 1, 0, false);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), (reload + duty) >> 1, 0, false);
        pwmv2_shadow_register_lock(PWM);
        board_delay_ms(100);
    }

}

void generate_central_aligned_waveform_in_pair(void)
{
    uint32_t duty, duty_step;
    bool increase_duty_cycle = true;

    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload, 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_set_dead_area(PWM, BOARD_APP_PWM_OUT1, 50);
    pwmv2_set_dead_area(PWM, BOARD_APP_PWM_OUT2, 50);

    pwmv2_shadow_register_lock(PWM);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
    pwm_fault_async();

    duty_step = reload / 100;
    duty = reload / 100;
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        if (increase_duty_cycle) {
            if ((duty + duty_step) >= reload) {
                increase_duty_cycle = false;
                continue;
            }
            duty += duty_step;
        } else {
            if (duty <= duty_step) {
                increase_duty_cycle = true;
                continue;
            }
            duty -= duty_step;
        }
        pwmv2_shadow_register_unlock(PWM);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), (reload - duty) >> 1, 0, false);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), (reload + duty) >> 1, 0, false);
        pwmv2_shadow_register_lock(PWM);
        board_delay_ms(10);
    }
}

void generate_pwm_cmpjit_edge_aligned_waveform(void)
{
    uint32_t duty, duty_step;
    bool increase_duty_cycle = true;

    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), reload, 0, true);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(4), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(5), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(6), reload, 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_counter_burst_disable(PWM, pwm_counter_1);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);
    pwmv2_set_reload_update_time(PWM, pwm_counter_1, pwm_reload_update_on_reload);
    pwmv2_enable_hrpwm(PWM);

    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(2), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(3));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(3), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(4));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(4), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(5));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(5), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(6));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(6), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(5));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(7), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(6));

    pwmv2_shadow_register_lock(PWM);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT3);
    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT4);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT3);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT4);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
    pwmv2_enable_counter(PWM, pwm_counter_1);
    pwmv2_start_pwm_output(PWM, pwm_counter_1);
    pwmv2_enable_hrpwm(PWM);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), 0, 100, true);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(5), 0, 0, false);
    pwmv2_shadow_register_lock(PWM);

    board_delay_ms(100);
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), reload, 100, true);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(5), reload, 0, false);
    pwmv2_add_delay_tick_after_dead_area(PWM, 100);
    pwmv2_shadow_register_lock(PWM);
    pwm_fault_async();

    duty_step = reload / 100;
    duty = reload / 100;
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        if (increase_duty_cycle) {
            if ((duty + duty_step) >= reload) {
                increase_duty_cycle = false;
                continue;
            }
            duty += duty_step;
        } else {
            if (duty <= duty_step) {
                increase_duty_cycle = true;
                continue;
            }
            duty -= duty_step;
        }
        pwmv2_shadow_register_unlock(PWM);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload - duty, 0, false);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), reload - duty, 0, false);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(5), reload - duty, 0, false);
        pwmv2_shadow_register_lock(PWM);
        board_delay_ms(50);
    }
}

void pwm_force_output(void)
{
    board_delay_ms(500);
    pwmv2_enable_shadow_lock_feature(PWM);
    pwmv2_set_force_update_time(PWM, BOARD_APP_PWM_OUT1, pwm_force_immediately);
    pwmv2_set_force_update_time(PWM, BOARD_APP_PWM_OUT2, pwm_force_immediately);

    pwmv2_force_update_time_by_shadow(PWM, BOARD_APP_PWM_OUT1, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(PWM, BOARD_APP_PWM_OUT2, pwm_force_update_shadow_immediately);

    pwmv2_enable_force_by_software(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_force_by_software(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_enable_software_force(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_software_force(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT1, pwm_force_output_1, false);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT2, pwm_force_output_1, false);
    pwmv2_shadow_register_lock(PWM);
    board_delay_ms(500);
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT1, pwm_force_output_0, false);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT2, pwm_force_output_0, false);
    pwmv2_shadow_register_lock(PWM);
    board_delay_ms(500);
    pwmv2_disable_force_by_software(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_disable_force_by_software(PWM, BOARD_APP_PWM_OUT2);
}

void generate_four_cmp_waveform_in_pair(void)
{
    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload >> 4, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload >> 3, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), reload >> 2, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(4), reload >> 1, 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(2), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(3));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(3), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(4));

    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_shadow_register_lock(PWM);
    pwmv2_enable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, false);
    pwmv2_shadow_register_lock(PWM);
    pwm_fault_async();

    board_delay_ms(100);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload, 0, false);
    pwmv2_shadow_register_lock(PWM);

    board_delay_ms(1000);

    printf("four cmp output end.\r\n");
    printf("four cmp logic and.\r\n");
    pwmv2_set_four_cmp_logic(PWM, BOARD_APP_PWM_OUT1, pwm_logic_four_cmp_and);
    board_delay_ms(1000);

    printf("four cmp logic xor.\r\n");
    pwmv2_set_four_cmp_logic(PWM, BOARD_APP_PWM_OUT1, pwm_logic_four_cmp_xor);
    board_delay_ms(1000);

    printf("four cmp logic cd.\r\n");
    pwmv2_set_four_cmp_logic(PWM, BOARD_APP_PWM_OUT1, pwm_logic_four_cmp_cd);
    board_delay_ms(1000);

}

void generate_pwm_dac_output(void)
{
    uint32_t duty, duty_step;
    bool increase_duty_cycle = true;

    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), 0, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload + 1, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload + 1, 0, false);

    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_calculate, pwm_dac_channel_0);
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_calculate, pwm_dac_channel_1);
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(0), pwm_shadow_register_update_on_reload);
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(1), pwm_shadow_register_update_on_reload);

    pwmv2_enable_software_dac_mode(PWM, pwm_dac_channel_0);
    pwmv2_enable_software_dac_mode(PWM, pwm_dac_channel_1);
    pwmv2_enable_software_dac_mode(PWM, pwm_dac_channel_2);

    pwmv2_set_dac_value(PWM, pwm_dac_channel_2, reload);
    pwmv2_conuter_select_dac_index(PWM, pwm_counter_0, pwm_dac_channel_2);
    pwmv2_counter_set_dac_data_parameter(PWM, pwm_counter_0, 8);
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));

    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_shadow_register_lock(PWM);

    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);

/**
 * @brief cal 0
 *
 */
    pwmv2_calculate_set_up_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 8);
    pwmv2_calculate_set_low_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_set_period_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_set_dac_value_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 8);

    pwmv2_calculate_select_counter_calculate_index(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_select_in_value(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_select_in_offset(PWM, PWMV2_CALCULATE_INDEX(0), 31);
    pwmv2_calculate_select_low_limit_offset(PWM, PWMV2_CALCULATE_INDEX(0), 1);

/**
 * @brief cal 1
 *
 */
    pwmv2_calculate_set_up_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 8);
    pwmv2_calculate_set_low_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 0);
    pwmv2_calculate_set_period_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 0);
    pwmv2_calculate_set_dac_value_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 8);

    pwmv2_calculate_select_counter_calculate_index(PWM, PWMV2_CALCULATE_INDEX(1), 0);
    pwmv2_calculate_select_in_value(PWM, PWMV2_CALCULATE_INDEX(1), 1);
    pwmv2_calculate_select_in_offset(PWM, PWMV2_CALCULATE_INDEX(1), 31);
    pwmv2_calculate_select_low_limit_offset(PWM, PWMV2_CALCULATE_INDEX(1), 1);

    pwmv2_set_dac_value(PWM, pwm_dac_channel_0, reload);
    pwmv2_set_dac_value(PWM, pwm_dac_channel_1, reload);
    pwm_fault_async();
    duty_step = reload / 100;
    duty = reload / 100;
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        if (increase_duty_cycle) {
            if ((duty + duty_step) >= reload) {
                increase_duty_cycle = false;
                continue;
            }
            duty += duty_step;
        } else {
            if (duty <= duty_step) {
                increase_duty_cycle = true;
                continue;
            }
            duty -= duty_step;
        }
        pwmv2_set_dac_value(PWM, pwm_dac_channel_0, reload - duty);
        board_delay_ms(50);
    }
/**
 * @brief frequency
 *
 */
    increase_duty_cycle = true;
    uint32_t times = 0;
    uint32_t test_reload = 0;
    do {
        for (uint32_t i = 0; i < 100; i++) {
            if (increase_duty_cycle) {
                test_reload += 50000000 / 10000;
            } else {
                test_reload -= 50000000 / 10000;
            }
            if (test_reload <= 10) {
                test_reload = 50000000 / 10000;
            }
            pwmv2_set_dac_value(PWM, pwm_dac_channel_2, test_reload);
            pwmv2_set_dac_value(PWM, pwm_dac_channel_0, test_reload >> 2);
            pwmv2_set_dac_value(PWM, pwm_dac_channel_1, test_reload >> 1);
            board_delay_ms(20);
        }
        times++;
        increase_duty_cycle = !increase_duty_cycle;
    } while (times < 10);
}

void disable_all_pwm_output(void)
{
    board_delay_ms(500);
    pwmv2_enable_shadow_lock_feature(PWM);
    pwmv2_set_force_update_time(PWM, BOARD_APP_PWM_OUT1, pwm_force_immediately);
    pwmv2_set_force_update_time(PWM, BOARD_APP_PWM_OUT2, pwm_force_immediately);
    pwmv2_set_force_update_time(PWM, BOARD_APP_PWM_OUT3, pwm_force_immediately);
    pwmv2_set_force_update_time(PWM, BOARD_APP_PWM_OUT4, pwm_force_immediately);

    pwmv2_force_update_time_by_shadow(PWM, BOARD_APP_PWM_OUT1, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(PWM, BOARD_APP_PWM_OUT2, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(PWM, BOARD_APP_PWM_OUT3, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(PWM, BOARD_APP_PWM_OUT4, pwm_force_update_shadow_immediately);

    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT3);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT4);

    pwmv2_enable_force_by_software(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_force_by_software(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_force_by_software(PWM, BOARD_APP_PWM_OUT3);
    pwmv2_enable_force_by_software(PWM, BOARD_APP_PWM_OUT4);

    pwmv2_enable_software_force(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_software_force(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_software_force(PWM, BOARD_APP_PWM_OUT3);
    pwmv2_enable_software_force(PWM, BOARD_APP_PWM_OUT4);

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT1, pwm_force_output_0, false);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT2, pwm_force_output_0, false);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT3, pwm_force_output_0, false);
    pwmv2_force_output(PWM, BOARD_APP_PWM_OUT4, pwm_force_output_0, false);
    pwmv2_shadow_register_lock(PWM);
}

void pwm_fault_async(void)
{
    pwmv2_async_fault_source_config_t fault_cfg;

    fault_cfg.async_signal_from_pad_index = BOARD_APP_PWM_FAULT_PIN;
    fault_cfg.fault_async_pad_level = pad_fault_active_high;
    pwmv2_config_async_fault_source(PWM, BOARD_APP_PWM_OUT1, &fault_cfg);
    pwmv2_config_async_fault_source(PWM, BOARD_APP_PWM_OUT2, &fault_cfg);
    pwmv2_set_fault_mode(PWM, BOARD_APP_PWM_OUT1, pwm_fault_output_0);
    pwmv2_set_fault_mode(PWM, BOARD_APP_PWM_OUT2, pwm_fault_output_0);
    pwmv2_enable_async_fault(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_enable_async_fault(PWM, BOARD_APP_PWM_OUT2);
}

int main(void)
{
    uint32_t freq;
    board_init();
    init_pwm_pins(PWM);
    init_pwm_fault_pins();
    printf("pwm example\n");

    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;

    printf("\n\n>> Test force PWM output on P%d and P%d\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2);
    pwm_force_output();
    printf("\n\n>> Generate edge aligned waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_edge_aligned_waveform();
    printf("\n\n>> Generate central aligned waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_central_aligned_waveform();
    printf("\n\n>> Generate central aligned waveform in pair\n");
    printf("Two waveforms will be generated in pair, PWM P%d and P%d are target\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2);
    printf("waveforms whose duty cycle will be updated from 0 - 100 and back to 0\n");
    generate_central_aligned_waveform_in_pair();
    printf("\n\n>> Generate edge aligned jit waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_pwm_cmpjit_edge_aligned_waveform();
    printf("\n\n>> Generate fout cmp waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_four_cmp_waveform_in_pair();
    printf("\n\n>> Generate pwm dac waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_pwm_dac_output();
    disable_all_pwm_output();
    printf("test done\n");
    while (1) {
        ;
    }
    return 0;
}
