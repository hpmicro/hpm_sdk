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
#include "hpm_tsns_drv.h"
#include "hpm_gptmr_drv.h"

#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT2
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT3
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#define GPTMR               BOARD_GPTMR
#define GPTMR_CH            BOARD_GPTMR_CHANNEL
#define GPTMR_IRQ           BOARD_GPTMR_IRQ
#define GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME
#define TICK_MS                   (1)

#ifndef TEST_LOOP
#define TEST_LOOP (200)
#endif

#ifndef TSNS
#define TSNS HPM_TSNS
#endif

#ifndef TSNS_IRQ
#define TSNS_IRQ IRQn_TSNS
#endif

#define PWM_PERIOD_IN_MS (1)
#define TSNS_TEMP_DETA (5.0f)
uint32_t reload;
trgm_pwmv2_calibration_mode_t calibration_mode;
uint8_t calibration_times;


void hrpwm_init_calibration(TRGM_Type *trgm)
{
    uint8_t times = 0;

    calibration_mode = trgm_pwmv2_calibration_mode_begin;
    while (calibration_mode != trgm_pwmv2_calibration_mode_end) {
        trgm_pwmv2_calibrate_delay_chain(trgm, &calibration_mode);
        board_delay_us(10);
        times++;
        if (times > TEST_LOOP) {
            printf("calibration failed\n");
            while (1) {
            }
        }
    }
}

SDK_DECLARE_EXT_ISR_M(GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    if (gptmr_check_status(GPTMR, GPTMR_CH_RLD_STAT_MASK(GPTMR_CH))) {
        gptmr_clear_status(GPTMR, GPTMR_CH_RLD_STAT_MASK(GPTMR_CH));
        trgm_pwmv2_calibrate_delay_chain(TRGM, &calibration_mode);
        calibration_times++;
        if (calibration_times > TEST_LOOP) {
            gptmr_disable_irq(GPTMR, GPTMR_CH_RLD_IRQ_MASK(GPTMR_CH));
            printf("calibration failed\n");
        }
        if (calibration_mode == trgm_pwmv2_calibration_mode_end) {
            gptmr_disable_irq(GPTMR, GPTMR_CH_RLD_IRQ_MASK(GPTMR_CH));
            printf("calibration done\n");
        }
    }
}

void timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(GPTMR_CLOCK, 0);
    gptmr_channel_get_default_config(GPTMR, &config);
    gptmr_freq = clock_get_frequency(GPTMR_CLOCK);
    config.reload = gptmr_freq / 1000 * TICK_MS;
    gptmr_channel_config(GPTMR, GPTMR_CH, &config, false);
    gptmr_start_counter(GPTMR, GPTMR_CH);

    intc_m_enable_irq_with_priority(GPTMR_IRQ, 1);
}
void generate_hrpwm_waveform(void)
{
    pwmv2_deinit(PWM);
    pwmv2_shadow_register_unlock(PWM);

    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload >> 1, 0, false);
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
    pwmv2_enable_hrpwm(PWM);
    hrpwm_init_calibration(TRGM);
    pwmv2_start_pwm_output(PWM, pwm_counter_0);
    while (1) {
        for (uint16_t i = 0; i < 255; i++) {
            pwmv2_shadow_register_unlock(PWM);
            board_delay_ms(100);
            pwmv2_set_shadow_val(PWM, PWMV2_SHADOW_INDEX(1), reload >> 1, i, false);
            pwmv2_shadow_register_lock(PWM);
        }
    }
}

SDK_DECLARE_EXT_ISR_M(TSNS_IRQ, tsns_isr)
void tsns_isr(void)
{
    uint32_t flag;
    float tsns_temp;

    flag = tsns_get_flag_value(TSNS);
    tsns_clear_with_mask(TSNS, flag);

    tsns_temp = tsns_get_current_temp(TSNS);
    tsns_configure_limit_event(TSNS, (tsns_temp + TSNS_TEMP_DETA) * TSNS_TEMP_SCALE, (tsns_temp - TSNS_TEMP_DETA) * TSNS_TEMP_SCALE, tsns_event_irq);
    calibration_mode = trgm_pwmv2_calibration_mode_begin;
    gptmr_enable_irq(GPTMR, GPTMR_CH_RLD_IRQ_MASK(GPTMR_CH));
}

void tsns_init(TSNS_Type *tsns)
{
    float tsns_temp;

    clock_add_to_group(clock_tsns, 0);
    tsns_enable_continuous_mode(tsns);
    tsns_enable(tsns);
    tsns_temp = tsns_get_current_temp(tsns);
    tsns_configure_limit_event(tsns, (tsns_temp + TSNS_TEMP_DETA) * TSNS_TEMP_SCALE, (tsns_temp - TSNS_TEMP_DETA) * TSNS_TEMP_SCALE, tsns_event_irq);
    tsns_set_average(tsns, 7);
    intc_m_enable_irq_with_priority(TSNS_IRQ, 1);
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

    timer_config();
    tsns_init(TSNS);

    printf("\n\n>> Generate edge aligned waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose hrpwm cycle will be updated from 0 - 255 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_hrpwm_waveform();
    return 0;
}
