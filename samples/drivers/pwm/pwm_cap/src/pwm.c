/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_clock_drv.h"
#include <math.h>

#define PWM_INPUT_CAP_PIN   BOARD_APP_PWM_OUT2
#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT1
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#define PWM_CENTER_OUTPUT_CMP0_NUM  (8U)
#define PWM_CENTER_OUTPUT_CMP1_NUM  (9U)
#define PWM_TRIG_CMP_NUM            (10U)

#ifndef TEST_LOOP
#define TEST_LOOP (200)
#endif

#define PWM_CAP_TRIG_TYPE pwm_counter_type_capture_falling_edge

#define PWM_PERIOD_IN_MS (5)
uint32_t reload;
uint32_t cap_buf_falling;
uint32_t cap_buf_rising;
bool cap_true;


void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(PWM);
    trgm_output_update_source(TRGM, TRGM_PWM_OUTPUT, 1);
    trgm_output_update_source(TRGM, TRGM_PWM_OUTPUT, 0);
}

void process_cap_data(uint32_t fall, uint32_t rising)
{
    static uint32_t pre_fall, pre_rising;
    if ((fall < PWM_DUTY_CYCLE_FP_MAX) && (rising < PWM_DUTY_CYCLE_FP_MAX)) {
        fall >>= 4;
        rising >>= 4;
        if ((pre_fall != fall) || (pre_rising != rising)) {
            pre_fall = fall;
            pre_rising = rising;
            cap_true = true;
        }
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_PWM_IRQ, isr_pwm_cap)
void isr_pwm_cap(void)
{
    pwm_clear_status(PWM, PWM_IRQ_CMP(PWM_INPUT_CAP_PIN));
    pwm_get_captured_count(PWM, &cap_buf_falling, PWM_CAP_TRIG_TYPE, PWM_INPUT_CAP_PIN, 1);
    pwm_get_captured_count(PWM, &cap_buf_rising, !PWM_CAP_TRIG_TYPE, PWM_INPUT_CAP_PIN, 1);
    process_cap_data(cap_buf_falling, cap_buf_rising);
}

void config_pwm_input_capture(void)
{
    pwm_cmp_config_t cmp_config;
    cmp_config.mode = pwm_cmp_mode_input_capture;
    pwm_config_cmp(PWM, PWM_INPUT_CAP_PIN, &cmp_config);
    intc_m_enable_irq_with_priority(BOARD_APP_PWM_IRQ, 1);
    pwm_enable_irq(PWM, PWM_IRQ_CMP(PWM_INPUT_CAP_PIN));
}

void generate_central_aligned_waveform(void)
{
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(PWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(PWM, &pwm_config);

    /*
     * reload and start counter
     */
    pwm_set_reload(PWM, 0, reload);
    pwm_set_start_count(PWM, 0, 0);

    /*
     * config cmp8-cmp10
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = reload;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN1, &pwm_config, PWM_CENTER_OUTPUT_CMP0_NUM, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while (1) {
            ;
        }
    }
    pwm_load_cmp_shadow_on_match(PWM, PWM_TRIG_CMP_NUM,  &cmp_config[3]);

    pwm_start_counter(PWM);
    pwm_issue_shadow_register_lock_event(PWM);
}

void disable_all_pwm_output(void)
{
    pwm_disable_output(PWM, PWM_OUTPUT_PIN1);
}

int16_t test_pwm_cap(void)
{
    uint32_t m;
    uint32_t duty, duty_step;
    float duty_percent;
    bool increase_duty_cycle = true;
    bool is_cap = false;

    duty_step = reload / TEST_LOOP;
    duty = reload / TEST_LOOP;
    increase_duty_cycle = true;
    config_pwm_input_capture();
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
        duty_percent = (float)duty / reload;
        pwm_update_raw_cmp_central_aligned(PWM, PWM_CENTER_OUTPUT_CMP0_NUM, PWM_CENTER_OUTPUT_CMP1_NUM, (reload - duty) >> 1, (reload + duty) >> 1);
        m = 0;
        is_cap = false;
        do {
            m++;
            board_delay_ms(1);
            if (cap_true) {
                is_cap = true;
                cap_true = false;
                float cap_duty;
                cap_duty =  (float)((cap_buf_falling >> 4) - (cap_buf_rising >> 4)) / reload;
                if (fabs(cap_duty - duty_percent) > 0.1) {
                    return -1;
                }
                printf("Pwm output duty: %f, Pwm cap duty: %f.\r\n", duty_percent, cap_duty);
            }
        } while (m < 100);
        if (!is_cap) {
            return -1;
        }
    }
    return 0;
}

int main(void)
{
    uint32_t freq;
    int rsl;

    board_init();
    init_pwm_pins(PWM);
    printf("pwm capture example\n");

    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;

    printf("\n\n>> Test PWM output on P%d, PWM capture P%d\n", PWM_OUTPUT_PIN1, PWM_INPUT_CAP_PIN);
    generate_central_aligned_waveform();
    rsl = test_pwm_cap();
    if (rsl == 0) {
        printf("PASS.\r\n");
    } else {
        printf("FAIL.\r\n");
    }
    disable_all_pwm_output();
    printf("test done\n");
    while (1) {
        ;
    }
    return 0;
}
