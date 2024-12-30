/*
 * Copyright (c) 2023 HPMicro
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
#include "hpm_romapi.h"

#ifndef HRPWM
#define HRPWM BOARD_APP_HRPWM
#define PWM_CLOCK_NAME BOARD_APP_HRPWM_CLOCK_NAME
#define PWM_OUTPUT_PIN1 BOARD_APP_HRPWM_OUT1
#define PWM_OUTPUT_PIN2 BOARD_APP_HRPWM_OUT2
#define TRGM BOARD_APP_HRPWM_TRGM
#endif

#ifndef TEST_LOOP
#define TEST_LOOP (100)
#endif

#define PWM_PERIOD_IN_MS (0.01)
#define HRPWM_SET_IN_PWM_CLK (128)

uint32_t reload;
uint32_t pwm_clk_freq;

uint32_t get_chip_product_ver(void)
{
    return (ROM_API_TABLE_ROOT->otp_driver_if->read_from_shadow(OTP_TABLE_CHIP_ID_OFFSET) >> 12) & 0xff;
}

uint32_t get_chip_to_ver(void)
{
    return (ROM_API_TABLE_ROOT->otp_driver_if->read_from_shadow(OTP_TABLE_CHIP_ID_OFFSET) >> 28) & 0xf;
}

bool is_enable_pwm_hw_recovery(void)
{
    if (get_chip_product_ver() == 0x3) {
        if (get_chip_to_ver() >= 0x2) {
            return true;
        } else {
            return false;
       }
    } else {
        return false;
    }
}

void config_hw_event(uint8_t cmp_index, uint32_t cmp)
{
    pwm_cmp_config_t cmp_config = {0};
    cmp_config.mode = pwm_cmp_mode_output_compare;
    cmp_config.cmp = cmp;
    cmp_config.update_trigger = pwm_shadow_register_update_on_modify;
    pwm_config_cmp(HRPWM, cmp_index, &cmp_config);
}

void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(HRPWM);
    trgm_output_update_source(TRGM, TRGM_TRGOCFG_PWM_SYNCI, 1);
    trgm_output_update_source(TRGM, TRGM_TRGOCFG_PWM_SYNCI, 0);
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_HRPWM_IRQ, isr_pwm)
void isr_pwm(void)
{
    uint32_t status;
    status = pwm_get_status(HRPWM);
    pwm_clear_status(HRPWM, status);
    if ((status & PWM_IRQ_CMP(BOARD_APP_HRPWM_FAULT_CAP_CMP_INDEX))) {
        pwm_recovery_hrpwm_output(HRPWM);
    }
}

void config_pwm_fault_capture(void)
{
    pwm_cmp_config_t cmp_config;
    trgm_output_t trgm_output_cfg;

    cmp_config.mode = pwm_cmp_mode_input_capture;
    pwm_config_cmp(HRPWM, BOARD_APP_HRPWM_FAULT_CAP_CMP_INDEX, &cmp_config);
    intc_m_enable_irq_with_priority(BOARD_APP_HRPWM_IRQ, 1);
    pwm_enable_irq(HRPWM, PWM_IRQ_CMP(BOARD_APP_HRPWM_FAULT_CAP_CMP_INDEX));

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_APP_HRPWM_FAULT_TRGM_SRC;
    trgm_output_config(BOARD_APP_HRPWM_TRGM, BOARD_APP_HRPWM_FAULT_TRGM_OUT, &trgm_output_cfg);

}

void generate_edge_aligned_waveform_fault_mode(void)
{
    uint8_t cmp_index = 0;
    uint8_t fault_recovery_cmp;
    uint32_t duty, duty_step;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_config_t pwm_config = {0};
    pwm_fault_source_config_t fault_config = {0};

    /**
     * @brief define fault recovery cmp
     *
     */
    fault_recovery_cmp = 5;

    pwm_stop_counter(HRPWM);
    pwm_disable_hrpwm(HRPWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(HRPWM, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    pwm_config.fault_mode = pwm_fault_mode_force_output_0;
    pwm_config.fault_recovery_trigger = pwm_fault_recovery_on_hw_event;

    pwm_cal_hrpwm_chn_start(HRPWM, cmp_index);
    pwm_cal_hrpwm_chn_start(HRPWM, cmp_index + 1);
    pwm_cal_hrpwm_chn_wait(HRPWM, cmp_index);
    pwm_cal_hrpwm_chn_wait(HRPWM, cmp_index + 1);

    pwm_enable_hrpwm(HRPWM);
    /*
     * reload and start counter
     */
    pwm_set_hrpwm_reload(HRPWM, 0, reload);
    pwm_set_start_count(HRPWM, 0, 0);
    fault_config.fault_external_0_active_low = true;
    fault_config.source_mask = pwm_fault_source_debug;
    /**
     * @brief set fault recovery cmp index
     *
     */
    fault_config.fault_output_recovery_trigger = fault_recovery_cmp;
    pwm_config_fault_source(HRPWM, &fault_config);

    if (!is_enable_pwm_hw_recovery()) {
        config_pwm_fault_capture();
    }

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].enable_hrcmp = true;
    cmp_config[0].hrcmp = 0;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload + 1;
    cmp_config[1].enable_hrcmp = true;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].mode = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = reload;
    cmp_config[2].enable_hrcmp = true;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;
    /*
     * config pwm as output driven by cmp
     */
    if (status_success != pwm_setup_waveform(HRPWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        };
    }
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(HRPWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        };
    }
    pwm_load_cmp_shadow_on_match(HRPWM, cmp_index + 4, &cmp_config[2]);

    /**
     * @brief set fault recovery cmp vaule
     *
     */
    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = reload >> 1;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_hw_event;
    pwm_config_cmp(HRPWM, fault_recovery_cmp, &cmp_config[3]);
    pwm_issue_shadow_register_lock_event(HRPWM);


    pwm_start_counter(HRPWM);
    pwm_issue_shadow_register_lock_event(HRPWM);

    duty_step = reload / TEST_LOOP;
    duty = reload / TEST_LOOP;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {
        if ((duty + duty_step) >= reload) {
            duty = duty_step;
        } else {
            duty += duty_step;
        }
        pwm_update_raw_hrcmp_edge_aligned(HRPWM, cmp_index, reload - duty, HRPWM_SET_IN_PWM_CLK);
        pwm_update_raw_hrcmp_edge_aligned(HRPWM, cmp_index + 2, reload - duty, 0);
        /**
         * @brief update fault recovery cmp value, Need to stagger the two edges of the waveform
         *
         */
        pwm_fault_recovery_update_cmp_value(HRPWM, fault_recovery_cmp, (reload - duty) >> 1);
        board_delay_ms(100);
    }
}

void generate_central_aligned_waveform(void)
{
    uint8_t cmp_index = 0;
    uint32_t duty, duty_step;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(HRPWM);
    reset_pwm_counter();
    pwm_disable_hrpwm(HRPWM);
    pwm_get_default_pwm_config(HRPWM, &pwm_config);

    pwm_cal_hrpwm_chn_start(HRPWM, cmp_index);
    pwm_cal_hrpwm_chn_start(HRPWM, cmp_index + 1);
    pwm_cal_hrpwm_chn_wait(HRPWM, cmp_index);
    pwm_cal_hrpwm_chn_wait(HRPWM, cmp_index + 1);


    pwm_enable_hrpwm(HRPWM);
    /*
     * reload and start counter
     */
    pwm_set_reload(HRPWM, 0, reload);
    pwm_set_start_count(HRPWM, 0, 0);

    /*
     * config cmp1 and cmp2 and cmp3
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].enable_hrcmp = true;
    cmp_config[0].hrcmp = 0;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload + 1;
    cmp_config[1].enable_hrcmp = true;
    cmp_config[1].hrcmp = 0;
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
    if (status_success != pwm_setup_waveform(HRPWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        };
    }
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(HRPWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 2, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        };
    }
    pwm_load_cmp_shadow_on_match(HRPWM, cmp_index + 3,  &cmp_config[3]);
    pwm_start_counter(HRPWM);
    pwm_issue_shadow_register_lock_event(HRPWM);
    duty_step = reload / TEST_LOOP;
    duty = reload / TEST_LOOP;
    for (uint32_t i = 0; i < TEST_LOOP; i++) {

        if ((duty + duty_step) >= reload) {
            duty = duty_step;
        } else {
            duty += duty_step;
        }
        pwm_update_raw_hrcmp_central_aligned(HRPWM, cmp_index, cmp_index + 1, (reload - duty) >> 1, (reload + duty) >> 1, 0, HRPWM_SET_IN_PWM_CLK);
        board_delay_ms(100);
    }
}

void set_hrpwm_frequency(float freq)
{
    float val;
    uint32_t reload, hr_reload;

    val = pwm_clk_freq / freq;
    reload = (uint32_t)val;
    hr_reload = (uint8_t)(255 * (val - reload));
    pwm_set_hrpwm_reload(HRPWM, hr_reload, reload);
}

void generate_hrpwm_frequency_variable_waveform(void)
{
    uint8_t cmp_index = 0;
    uint32_t freq;
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(HRPWM);
    pwm_disable_hrpwm(HRPWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(HRPWM, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;

    pwm_cal_hrpwm_chn_start(HRPWM, cmp_index);
    pwm_cal_hrpwm_chn_start(HRPWM, cmp_index + 1);
    pwm_cal_hrpwm_chn_wait(HRPWM, cmp_index);
    pwm_cal_hrpwm_chn_wait(HRPWM, cmp_index + 1);

    pwm_enable_hrpwm(HRPWM);
    /*
     * reload and start counter
     */
    pwm_set_hrpwm_reload(HRPWM, 0, reload);
    pwm_set_start_count(HRPWM, 0, 0);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload >> 1;
    cmp_config[0].enable_hrcmp = true;
    cmp_config[0].hrcmp = 0;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_modify;
    /*
     * config pwm as output driven by cmp
     */
    if (status_success != pwm_setup_waveform(HRPWM, PWM_OUTPUT_PIN1, &pwm_config, cmp_index, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while (1) {
        };
    }
    cmp_config[0].cmp = reload >> 1;
    /*
     * config pwm as reference
     */
    if (status_success != pwm_setup_waveform(HRPWM, PWM_OUTPUT_PIN2, &pwm_config, cmp_index + 1, &cmp_config[0], 1)) {
        printf("failed to setup waveform\n");
        while (1) {
        };
    }
    pwm_load_cmp_shadow_on_match(HRPWM, cmp_index + 2, &cmp_config[1]);

    pwm_start_counter(HRPWM);
    pwm_issue_shadow_register_lock_event(HRPWM);
    pwm_set_cnt_shadow_trig_reload(HRPWM, true);

    freq = 10;
    for (uint8_t i = 0; i < TEST_LOOP; i++) {
        freq += 100.01;
        set_hrpwm_frequency(freq);
        board_delay_ms(100);
    }
}

void test_pwm_force_output(void)
{
    pwm_config_force_cmd_timing(HRPWM, pwm_force_immediately);
    pwm_enable_pwm_sw_force_output(HRPWM, PWM_OUTPUT_PIN1);
    pwm_enable_pwm_sw_force_output(HRPWM, PWM_OUTPUT_PIN2);

    printf("Output high\n");
    pwm_set_force_output(HRPWM,
            PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN1, pwm_output_1)
            | PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN2, pwm_output_1));
    pwm_enable_sw_force(HRPWM);
    board_delay_ms(5000);
    printf("Output low\n");
    pwm_set_force_output(HRPWM,
            PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN1, pwm_output_0)
            | PWM_FORCE_OUTPUT(PWM_OUTPUT_PIN2, pwm_output_0));
    board_delay_ms(5000);
    pwm_disable_sw_force(HRPWM);

    pwm_disable_pwm_sw_force_output(HRPWM, PWM_OUTPUT_PIN1);
    pwm_disable_pwm_sw_force_output(HRPWM, PWM_OUTPUT_PIN2);
}

void disable_all_pwm_output(void)
{
    pwm_disable_output(HRPWM, PWM_OUTPUT_PIN1);
    pwm_disable_output(HRPWM, PWM_OUTPUT_PIN2);
}

int main(void)
{
    board_init();
    init_hrpwm_pins(HRPWM);
    printf("hr pwm example\n");

    pwm_clk_freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = pwm_clk_freq / 1000 * PWM_PERIOD_IN_MS - 1;

    printf("\n\n>> Test force HRPWM output on P%d and P%d\n", PWM_OUTPUT_PIN1, PWM_OUTPUT_PIN2);
    test_pwm_force_output();
    printf("\n\n>> Generate edge aligned waveform\n");
    printf("Two waveforms will be generated, HRPWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100; HRPWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_edge_aligned_waveform_fault_mode();
    printf("\n\n>> Generate central aligned waveform\n");
    printf("Two waveforms will be generated, HRPWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    printf("whose duty cycle will be updated from 0 - 100; HRPWM P%d is a reference\n", PWM_OUTPUT_PIN2);
    generate_hrpwm_frequency_variable_waveform();
    printf("\n\n>> Generate frequency-variable waveforms\n");
    printf("whose frequency will be updated; HRPWM P%d is the target waveform\n", PWM_OUTPUT_PIN1);
    disable_all_pwm_output();
    printf("test done\n");
    while (1) {
    };
    return 0;
}
