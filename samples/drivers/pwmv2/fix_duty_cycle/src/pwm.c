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

#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT1
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT2
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#define PWM_PRIVATE_CLAC_0 0
#define PWM_PRIVATE_CLAC_1 1
#define PWM_PRIVATE_CLAC_2 2
#define PWM_PRIVATE_CLAC_3 3

/*
* use universal calc unit solution
*/
void pwm_calc_hardware_loop_config(uint32_t cmp_value0_factor, uint32_t cmp_value1_factor)
{
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), 0, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, false);

    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);

/* cmp0 cmp1 from universal calc0 calc1  */
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_calculate, PWMV2_CALCULATE_INDEX(0));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_calculate, PWMV2_CALCULATE_INDEX(1));
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(0), pwm_shadow_register_update_on_reload);
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(1), pwm_shadow_register_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_shadow_register_lock(PWM);

    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);

/* set private calc0 */
    pwmv2_counter_set_dac_data_parameter(PWM, pwm_counter_0, 0);

/* set universal calc0 */
    pwmv2_calculate_set_up_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 8);
    pwmv2_calculate_set_low_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_set_period_parameter(PWM, PWMV2_CALCULATE_INDEX(0), cmp_value0_factor);
    pwmv2_calculate_set_dac_value_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 0);

    pwmv2_calculate_select_counter_calculate_index(PWM, PWMV2_CALCULATE_INDEX(0), PWM_PRIVATE_CLAC_0);
    pwmv2_calculate_select_in_offset(PWM, PWMV2_CALCULATE_INDEX(0), 31);
    pwmv2_calculate_select_low_limit_offset(PWM, PWMV2_CALCULATE_INDEX(0), PWMV2_SHADOW_INDEX(1));

/* set universal calc1 */
    pwmv2_calculate_set_up_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 8);
    pwmv2_calculate_set_low_limit_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 0);
    pwmv2_calculate_set_period_parameter(PWM, PWMV2_CALCULATE_INDEX(1), cmp_value1_factor);
    pwmv2_calculate_set_dac_value_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 0);

    pwmv2_calculate_select_counter_calculate_index(PWM, PWMV2_CALCULATE_INDEX(1), PWM_PRIVATE_CLAC_0);
    pwmv2_calculate_select_in_offset(PWM, PWMV2_CALCULATE_INDEX(1), 31);
    pwmv2_calculate_select_low_limit_offset(PWM, PWMV2_CALCULATE_INDEX(1), PWMV2_SHADOW_INDEX(1));
}

void pwm_calc_hardware_loop_change_period(uint32_t period_us)
{
    uint32_t t_reload, t_freq;
    t_freq = clock_get_frequency(PWM_CLOCK_NAME);
    t_reload = t_freq / 1000 / 1000 * period_us - 1;

    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), t_reload, 0, false);
    pwmv2_shadow_register_lock(PWM);
}

void pwm_calc_hardware_loop_test(void)
{
    uint32_t period = 0;
    uint32_t period_step = 800;

    for (uint32_t i = 0; i < 100; i++) {
        period += period_step;
        pwm_calc_hardware_loop_change_period(period);
        board_delay_ms(500);
    }
}

/******************* end of use universal calc unit solution **************************/

/*
* use shadow register change cmp value solution
*/

#define PWM_CMP_VALUE0(reload) (reload / 3)
#define PWM_CMP_VALUE1(reload) (reload * 2 / 3)

void pwm_shadow_handware_loop_config(void)
{
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), 0, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), 0, 0, false);

    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(0), pwm_shadow_register_update_on_reload);
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(1), pwm_shadow_register_update_on_reload);
    pwmv2_enable_pair_mode(PWM, BOARD_APP_PWM_OUT2);

    pwmv2_shadow_register_lock(PWM);

    pwmv2_disable_four_cmp(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT1);
    pwmv2_channel_enable_output(PWM, BOARD_APP_PWM_OUT2);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
}

void pwm_shadow_hardware_loop_change_period(uint32_t period_us)
{
    uint32_t reload, freq;
    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 / 1000 * period_us - 1;

    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), PWM_CMP_VALUE0(reload), 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), PWM_CMP_VALUE1(reload), 0, false);

    pwmv2_shadow_register_lock(PWM);
}

void pwm_shadow_hardware_loop_test(void)
{
    uint32_t period = 0;
    uint32_t period_step = 800;

    for (uint32_t i = 0; i < 100; i++) {
        period += period_step;
        pwm_shadow_hardware_loop_change_period(period);
        board_delay_ms(500);
    }
}

/******************* end of use shadow register change cmp value solution **************************/

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
    board_init();
    init_pwm_pins(PWM);
    init_pwm_fault_pins();
    printf("pwmv2 fix duty cycle example\n");

    printf("\n\n>> Generate fix duty cycle by calc\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    pwm_calc_hardware_loop_config(1, 3);
    pwm_calc_hardware_loop_test();
    pwmv2_deinit(PWM);
    pwm_fault_async();
    printf("\n\n>> Generate fix duty cycle by shadow\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    pwm_shadow_handware_loop_config();
    pwm_shadow_hardware_loop_test();
    disable_all_pwm_output();
    printf("test done\n");
    while (1) {
        ;
    }
    return 0;
}
