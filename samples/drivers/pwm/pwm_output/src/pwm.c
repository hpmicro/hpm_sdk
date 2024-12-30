/*
 * Copyright (c) 2021 HPMicro
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

#ifndef PWM
#define PWM BOARD_APP_PWM
#define PWM_CLOCK_NAME BOARD_APP_PWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_PWM_OUT1
#define PWM_OUTPUT_PIN2 BOARD_APP_PWM_OUT2
#define TRGM BOARD_APP_TRGM
#define TRGM_PWM_OUTPUT BOARD_APP_TRGM_PWM_OUTPUT
#endif

#ifndef TEST_LOOP
#define TEST_LOOP (200)
#endif

#define PWM_PERIOD_IN_MS (5)
uint32_t reload = 0;

void config_hw_event(uint8_t cmp_index, uint32_t cmp) 
{
    pwm_cmp_config_t cmp_config = {0};
    cmp_config.mode = pwm_cmp_mode_output_compare;
    cmp_config.cmp = cmp;
    cmp_config.update_trigger = pwm_shadow_register_update_on_modify;
    pwm_config_cmp(PWM, cmp_index, &cmp_config);
}

void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(PWM);
    trgm_output_update_source(TRGM, TRGM_PWM_OUTPUT, 1);
    trgm_output_update_source(TRGM, TRGM_PWM_OUTPUT, 0);
}

void generate_edge_aligned_waveform(void)
{
    uint8_t cmp_index = 0;
    float duty = 0;
    bool increase_duty_cycle = true;
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(PWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(PWM, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;

    /*
     * reload and start counter
     */
    pwm_set_reload(PWM, 0, reload);
    pwm_set_start_count(PWM, 0, 0);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_modify;
    /*
     * config pwm as output driven by cmp
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    cmp_config[0].cmp = reload >> 1;
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 1, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    pwm_load_cmp_shadow_on_match(PWM, cmp_index + 2, &cmp_config[1]);
   
    pwm_start_counter(PWM);
    pwm_issue_shadow_register_lock_event(PWM);
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        pwm_update_duty_edge_aligned(PWM, cmp_index, duty);
        if (increase_duty_cycle) {
            if (duty == 100) {
                increase_duty_cycle = false;
                continue;
            }
            duty++;
        } else {
            if (duty == 0) {
                increase_duty_cycle = true;
                continue;
            }
            duty--;
        }
        board_delay_ms(100);
    }
}

void generate_edge_aligned_waveform_fault_mode(void)
{
    uint8_t cmp_index = 0;
    float duty = 0;
    bool increase_duty_cycle = true;
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};
    pwm_fault_source_config_t fault_config = {0};

    pwm_stop_counter(PWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(PWM, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    pwm_config.fault_mode = pwm_fault_mode_force_output_0;
    pwm_config.fault_recovery_trigger = pwm_fault_recovery_immediately;

    fault_config.source_mask = pwm_fault_source_debug;
    pwm_config_fault_source(PWM, &fault_config);

    /*
     * reload and start counter
     */
    pwm_set_reload(PWM, 0, reload);
    pwm_set_start_count(PWM, 0, 0);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_modify;
    /*
     * config pwm as output driven by cmp
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    cmp_config[0].cmp = reload >> 1;
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 1, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    pwm_load_cmp_shadow_on_match(PWM, cmp_index + 2, &cmp_config[1]);

    pwm_start_counter(PWM);
    pwm_issue_shadow_register_lock_event(PWM);
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        pwm_update_duty_edge_aligned(PWM, cmp_index, duty);
        if (increase_duty_cycle) {
            if (duty == 100) {
                increase_duty_cycle = false;
                continue;
            }
            duty++;
        } else {
            if (duty == 0) {
                increase_duty_cycle = true;
                continue;
            }
            duty--;
        }
        board_delay_ms(100);
    }
}

void generate_central_aligned_waveform(void)
{
    uint8_t cmp_index = 0;
    float duty = 0;
    bool increase_duty_cycle = true;
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
     * config cmp1 and cmp2 and cmp3
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].mode = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = reload>>1;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;

    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = reload;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 2, &cmp_config[2], 1)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    pwm_load_cmp_shadow_on_match(PWM, cmp_index + 3,  &cmp_config[3]);
    pwm_start_counter(PWM);
    pwm_issue_shadow_register_lock_event(PWM);
    increase_duty_cycle = true;
    for (uint32_t i = 0; i <= TEST_LOOP; i++) {
        pwm_update_duty_central_aligned(PWM, cmp_index, cmp_index + 1, duty);
        if (increase_duty_cycle) {
            if (duty == 100) {
                increase_duty_cycle = false;
                continue;
            }
            duty++;
        } else {
            if (duty == 0) {
                increase_duty_cycle = true;
                continue;
            }
            duty--;
        }
        board_delay_ms(100);
    }
}

void generate_central_aligned_waveform_in_pair(void)
{
    uint8_t cmp_index = 0;
    float duty = 0;
    bool increase_duty_cycle = true;
    pwm_cmp_config_t cmp_config[3] = {0};
    pwm_pair_config_t pwm_pair_config = {0};

    pwm_stop_counter(PWM);
    reset_pwm_counter();

    /*
     * reload and start counter
     */
    pwm_set_reload(PWM, 0, reload);
    pwm_set_start_count(PWM, 0, 0);

    /*
     * config cmp1 and cmp2
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].mode = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = reload;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_get_default_pwm_pair_config(PWM, &pwm_pair_config);
    pwm_pair_config.pwm[0].enable_output = true;
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = 8000;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = 16000;
    pwm_pair_config.pwm[1].invert_output = false;

    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform_in_pair(PWM, PWM_OUTPUT_PIN1, &pwm_pair_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    pwm_load_cmp_shadow_on_match(PWM, cmp_index + 2,  &cmp_config[2]);
    pwm_start_counter(PWM);
    pwm_issue_shadow_register_lock_event(PWM);

    increase_duty_cycle = true;
    for (uint32_t i = 0; i <= TEST_LOOP; i++) {
        pwm_update_duty_central_aligned(PWM, cmp_index, cmp_index + 1, duty);
        if (increase_duty_cycle) {
            if (duty == 100) {
                increase_duty_cycle = false;
                continue;
            }
            duty++;
        } else {
            if (duty == 0) {
                increase_duty_cycle = true;
                continue;
            }
            duty--;
        }
        board_delay_ms(100);
    }
}

void generate_pwm_cmpjit_edge_aligned_waveform(void)
{
    uint8_t cmp_index = 0;
    float duty = 0;
    bool increase_duty_cycle = true;
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(PWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(PWM, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;

    /*
     * reload and start counter
     */
    pwm_set_reload(PWM, 0, reload);
    pwm_set_start_count(PWM, 0, 0);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].jitter_cmp = 4;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_modify;
    /*
     * config pwm as output driven by cmp
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while (1) {
            ;
        }
    }
    cmp_config[0].cmp = reload >> 1;
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(PWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 1, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while (1) {
            ;
        }
    }
    pwm_load_cmp_shadow_on_match(PWM, cmp_index + 2, &cmp_config[1]);

    pwm_start_counter(PWM);
    pwm_issue_shadow_register_lock_event(PWM);
    increase_duty_cycle = true;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        pwm_update_duty_edge_aligned(PWM, cmp_index, duty);
        if (increase_duty_cycle) {
            if (duty == 100) {
                increase_duty_cycle = false;
                continue;
            }
            duty++;
        } else {
            if (duty == 0) {
                increase_duty_cycle = true;
                continue;
            }
            duty--;
        }
        board_delay_ms(100);
    }
}

void test_pwm_force_output(void)
{
    pwm_config_force_cmd_timing(PWM, pwm_force_immediately);
    pwm_enable_pwm_sw_force_output(PWM, PWM_OUTPUT_PIN1);
    pwm_enable_pwm_sw_force_output(PWM, PWM_OUTPUT_PIN2);

    printf("Output high\n");
    pwm_set_force_output(PWM,
            PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN1, pwm_output_1)
            | PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN2, pwm_output_1));
    pwm_enable_sw_force(PWM);
    board_delay_ms(5000);
    printf("Output low\n");
    pwm_set_force_output(PWM,
            PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN1, pwm_output_0)
            | PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN2, pwm_output_0));
    board_delay_ms(5000);
    pwm_disable_sw_force(PWM);

    pwm_disable_pwm_sw_force_output(PWM, PWM_OUTPUT_PIN1);
    pwm_disable_pwm_sw_force_output(PWM, PWM_OUTPUT_PIN2);
}

void disable_all_pwm_output(void)
{
    pwm_disable_output(PWM, PWM_OUTPUT_PIN1);
    pwm_disable_output(PWM, PWM_OUTPUT_PIN2);
}

int main(void)
{
    uint32_t freq;
    board_init();
    init_pwm_pins(PWM);
    printf("pwm example\n");

    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;

    printf("\n\n>> Test force PWM output on P%d and P%d\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2);
    test_pwm_force_output();
    printf("\n\n>> Generate edge aligned waveform\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_edge_aligned_waveform();
    printf("\n\n>> Generate edge aligned waveform and fault mode enable\n");
    printf("Two waveforms will be generated, PWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100 and back to 0; PWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_edge_aligned_waveform_fault_mode();
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
    disable_all_pwm_output();
    printf("test done\n");
    while(1);
    return 0;
}
