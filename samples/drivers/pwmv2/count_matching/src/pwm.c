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
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT3
#define PWM_OUTPUT_PIN3 BOARD_APP_PWM_OUT5
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#define PWM_PERIOD_IN_MS (10)

uint32_t reload, freq;
volatile uint32_t cnt_zero, cnt_full;

void pwm_fault_async(void)
{
    pwmv2_async_fault_source_config_t fault_cfg;

    fault_cfg.async_signal_from_pad_index = BOARD_APP_PWM_FAULT_PIN;
    fault_cfg.fault_async_pad_level = pad_fault_active_high;
    pwmv2_config_async_fault_source(PWM, PWM_OUTPUT_PIN1, &fault_cfg);
    pwmv2_config_async_fault_source(PWM, PWM_OUTPUT_PIN2, &fault_cfg);
    pwmv2_set_fault_mode(PWM, PWM_OUTPUT_PIN1, pwm_fault_output_0);
    pwmv2_set_fault_mode(PWM, PWM_OUTPUT_PIN2, pwm_fault_output_0);
    pwmv2_enable_async_fault(PWM, PWM_OUTPUT_PIN1);
    pwmv2_enable_async_fault(PWM, PWM_OUTPUT_PIN2);
}

void pwm_match_hardware_config(void)
{
    pwmv2_enable_shadow_lock_feature(PWM);
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, 0); /* counter0 counter1 period */
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), 0, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), reload, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), 0, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(4), reload >> 1, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(5), reload >> 2, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(6), (reload >> 2) * 3, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(20), 0, 0, 0); /* zero point */
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(21), reload >> 1, 0, 0); /* midpoint */
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(22), reload - 1, 0, 0); /* full point */

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(4), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(3));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(5), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(4));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(8), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(5));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(9), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(6));
/* use universal compare register */
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(16), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(20));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(17), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(21));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(18), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(22));

    pwmv2_cmp_select_counter(PWM, PWMV2_CMP_INDEX(16), pwm_counter_0);
    pwmv2_cmp_select_counter(PWM, PWMV2_CMP_INDEX(18), pwm_counter_1);
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(16), pwm_shadow_register_update_on_modify);
    pwmv2_cmp_update_trig_time(PWM, PWMV2_CMP_INDEX(18), pwm_shadow_register_update_on_modify);
    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);
    pwmv2_set_reload_update_time(PWM, pwm_counter_1, pwm_reload_update_on_reload);
    pwmv2_set_reload_update_time(PWM, pwm_counter_2, pwm_reload_update_on_reload);
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_counter_burst_disable(PWM, pwm_counter_1);
    pwmv2_counter_burst_disable(PWM, pwm_counter_2);
    pwmv2_shadow_register_lock(PWM);

    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN1);
    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN2);
    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN3);
    pwmv2_enable_pair_mode(PWM, PWM_OUTPUT_PIN1);
    pwmv2_enable_pair_mode(PWM, PWM_OUTPUT_PIN2);

    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN1);
    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN2);
    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN3);

    pwmv2_clear_cmp_irq_status(PWM, 0xFFFFFF);
    pwmv2_enable_cmp_irq(PWM, PWMV2_CMP_INDEX(16));
    pwmv2_enable_cmp_irq(PWM, PWMV2_CMP_INDEX(18));
    intc_m_enable_irq_with_priority(BOARD_APP_PWM_IRQ, 1);
    pwmv2_reset_multi_counter_sync(PWM, (1 << pwm_counter_0 | 1 << pwm_counter_1 | 1 << pwm_counter_2));
    pwmv2_enable_multi_counter_sync(PWM, (1 << pwm_counter_0 | 1 << pwm_counter_1 | 1 << pwm_counter_2));
    pwmv2_start_pwm_output_sync(PWM, (1 << pwm_counter_0 | 1 << pwm_counter_1 | 1 << pwm_counter_2));
}

void pwm_change_per_ten_pulse(void)
{
    uint32_t duty_step = reload / 5;
    uint32_t duty_zero = 0;
    uint32_t duty_full = 0;
    bool increase_duty_cycle_zero = true;
    bool increase_duty_cycle_full = true;

    while (1) {
        if (cnt_zero >= 10) {
            if (increase_duty_cycle_zero) {
                if ((duty_zero + duty_step) >= reload) {
                    increase_duty_cycle_zero = false;
                }
                duty_zero += duty_step;
            } else {
                if (duty_zero <= duty_step) {
                    increase_duty_cycle_zero = true;
                }
                duty_zero -= duty_step;
            }
            pwmv2_shadow_register_unlock(PWM);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), (reload - duty_zero) >> 1, 0, false);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), (reload + duty_zero) >> 1, 0, false);
            pwmv2_shadow_register_lock(PWM);
            cnt_zero -= 10;
        }

        if (cnt_full >= 10) {
            if (increase_duty_cycle_full) {
                if ((duty_full + duty_step) >= reload) {
                    increase_duty_cycle_full = false;
                }
                duty_full += duty_step;
            } else {
                if (duty_full <= duty_step) {
                    increase_duty_cycle_full = true;
                }
                duty_full -= duty_step;
            }
            pwmv2_shadow_register_unlock(PWM);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(3), (reload - duty_full) >> 1, 0, false);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(4), (reload + duty_full) >> 1, 0, false);
            pwmv2_shadow_register_lock(PWM);
            cnt_full -= 10;
        }
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_PWM_IRQ, isr_pwm1)
void isr_pwm1(void)
{
    volatile uint32_t flags;
    flags = pwmv2_get_cmp_irq_status(PWM);
    pwmv2_clear_cmp_irq_status(PWM, flags);

    if (flags & PWM_IRQ_STS_CMP(16)) {
        cnt_zero++;
    }

    if (flags & PWM_IRQ_STS_CMP(18)) {
        cnt_full++;
    }
}

int main(void)
{
    board_init();
    init_pwm_pins(PWM);
    printf("pwmv2 point trigger example\n");
    printf("choose PWM output channel [P%d P%d P%d]\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2, PWM_OUTPUT_PIN3);
    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS;

    pwmv2_deinit(PWM);
    pwm_fault_async();
    printf("\n\n>> P%d and P%d generate pwm waveform P%d is reference\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2, PWM_OUTPUT_PIN3);
    printf("Modify the duty cycle at the midpoint when generating the 11th pulse\n");
    printf("duty cycle will be updated from 0 - 100 and back to 0, the step size is 20%%\n");
    pwm_match_hardware_config();
    board_delay_ms(1000);
    pwm_change_per_ten_pulse();
    while (1) {
        ;
    }
    return 0;
}
