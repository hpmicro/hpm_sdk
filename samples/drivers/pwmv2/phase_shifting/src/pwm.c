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
#include "hpm_gptmr_drv.h"

#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT1
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT3
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME
#define APP_TICK_MS                   (3000)

#define PWM_PRIVATE_CLAC_0 0
#define PWM_PRIVATE_CLAC_1 1
#define PWM_PRIVATE_CLAC_2 2
#define PWM_PRIVATE_CLAC_3 3

#define PWM_PERIOD_IN_MS (36)
#define PWM_PHASE_INITIAL_SHIFT (60)
#define PWM_PHASE_SHIFT_STEP (10)
#define PWM_PHASE_ALL_STEP (360)

uint32_t reload, freq;
uint32_t phase_shift;
volatile bool sync_flag;

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

void init_pwm_trgm_connect(void)
{
/* PWM1 sync trgm config, input is always high */
    trgm_output_t trgm0_io_config1 = {0};
    trgm0_io_config1.invert = false;
    trgm0_io_config1.type = trgm_output_pulse_at_input_falling_edge;
    trgm0_io_config1.input = 1;
    trgm_output_config(TRGM, TRGM_PWM_OUTPUT, &trgm0_io_config1);
}

static uint32_t pwm_sync_get_phase_shift_value(uint32_t period, uint32_t phase, bool is_forward)
{
    uint32_t phase_value;

    phase_value = period / PWM_PHASE_ALL_STEP * phase;
    return is_forward ? (phase_value) : (period - phase_value);
}

void pwm_sync_hardware_config(void)
{
    uint32_t phase;

    phase = pwm_sync_get_phase_shift_value(reload, phase_shift, 0);
    pwmv2_enable_shadow_lock_feature(PWM);
    pwmv2_shadow_register_unlock(PWM);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, 0); /* set counter0 counter1 period */
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload >> 3, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(2), (reload >> 2) * 3, 0, 0);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(20), 0, 0, 0); /* waveform0 phase shift value */
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(21), phase, 0, 0); /* waveform1 phase shift value */

    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_select_data_offset_from_shadow_value(PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(0), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(1), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(4), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(PWM, PWMV2_CMP_INDEX(5), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_set_reload_update_time(PWM, pwm_counter_0, pwm_reload_update_on_reload);
    pwmv2_set_reload_update_time(PWM, pwm_counter_1, pwm_reload_update_on_reload);
    pwmv2_counter_burst_disable(PWM, pwm_counter_0);
    pwmv2_counter_burst_disable(PWM, pwm_counter_1);
    pwmv2_shadow_register_lock(PWM);

    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN1);
    pwmv2_disable_four_cmp(PWM, PWM_OUTPUT_PIN2);
    pwmv2_enable_pair_mode(PWM, PWM_OUTPUT_PIN1);
    pwmv2_enable_pair_mode(PWM, PWM_OUTPUT_PIN2);

    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN1);
    pwmv2_channel_enable_output(PWM, PWM_OUTPUT_PIN2);
    pwmv2_reset_counter(PWM, pwm_counter_0);
    pwmv2_reset_counter(PWM, pwm_counter_1);
    pwmv2_enable_counter(PWM, pwm_counter_0);
    pwmv2_enable_counter(PWM, pwm_counter_1);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
    pwmv2_start_pwm_output(PWM, pwm_counter_1);

/* set universal calc to update counter value */
    pwmv2_calculate_set_period_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_set_dac_value_parameter(PWM, PWMV2_CALCULATE_INDEX(0), 0);
    pwmv2_calculate_select_counter_calculate_index(PWM, PWMV2_CALCULATE_INDEX(0), PWM_PRIVATE_CLAC_0);
    pwmv2_calculate_select_in_offset(PWM, PWMV2_CALCULATE_INDEX(0), PWMV2_SHADOW_INDEX(20));
    pwmv2_counter_update_trig0(PWM, pwm_counter_0, 0);
    pwmv2_counter_enable_update_trig0(PWM, pwm_counter_0);
    pwmv2_counter_set_trig0_calculate_cell_index(PWM, pwm_counter_0, PWMV2_CALCULATE_INDEX(0));

    pwmv2_calculate_set_period_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 0);
    pwmv2_calculate_set_dac_value_parameter(PWM, PWMV2_CALCULATE_INDEX(1), 0);
    pwmv2_calculate_select_counter_calculate_index(PWM, PWMV2_CALCULATE_INDEX(1), PWM_PRIVATE_CLAC_1);
    pwmv2_calculate_select_in_offset(PWM, PWMV2_CALCULATE_INDEX(1), PWMV2_SHADOW_INDEX(21));
    pwmv2_counter_update_trig0(PWM, pwm_counter_1, 0);
    pwmv2_counter_enable_update_trig0(PWM, pwm_counter_1);
    pwmv2_counter_set_trig0_calculate_cell_index(PWM, pwm_counter_1, PWMV2_CALCULATE_INDEX(1));
}

void pwm_change_phase_after_trigger(void)
{
    uint32_t phase;
    if (sync_flag) {
        sync_flag = false;
        printf("\nP%d and P%d now have a phase difference of %d degrees\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2, phase_shift);
        phase_shift += PWM_PHASE_SHIFT_STEP;
        phase_shift %= PWM_PHASE_ALL_STEP;
        phase = pwm_sync_get_phase_shift_value(reload, phase_shift, 0);
        pwmv2_shadow_register_unlock(PWM);
        pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(21), phase, 0, 0); /* waveform1 phase shift value */
        pwmv2_shadow_register_lock(PWM);
    }
    return;
}

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
/* update counter value, change trigger_in, high low high */
        trgm_output_update_source(TRGM, TRGM_PWM_OUTPUT, 0);
        trgm_output_update_source(TRGM, TRGM_PWM_OUTPUT, 1);
        sync_flag = true;
    }
}

static void timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    init_gptmr_pins(APP_BOARD_GPTMR);
    clock_add_to_group(APP_BOARD_GPTMR_CLOCK, 0);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    config.reload = gptmr_freq / 1000 * APP_TICK_MS;
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

int main(void)
{
    board_init();
    init_pwm_pins(PWM);
    init_pwm_trgm_connect();
    printf("pwmv2 two pwm submodule phase sync example\n");
    printf("choose PWM output channel [P%d P%d]\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2);
/* set period 36ms */
    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS;
    phase_shift = PWM_PHASE_INITIAL_SHIFT;
    sync_flag = false;

    pwmv2_deinit(PWM);
    pwm_fault_async();
    timer_config();
    printf("\n\n>> P%d and P%d generate same phase firstly\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2);
    printf("Two waveforms will be generated\n");
    printf("Every 3 seconds, P%d phase will shift\n", PWM_OUTPUT_PIN2);
    printf("P%d is a reference\n", PWM_OUTPUT_PIN1);
    pwm_sync_hardware_config();
    while (1) {
        pwm_change_phase_after_trigger();
    }
    return 0;
}
