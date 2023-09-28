/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "moto.h"

#define TEST_PWM HPM_PWM0

void pwm_config_output(void)
{
    init_pwm_pins(TEST_PWM);

    pwm_config_force_cmd_timing(TEST_PWM, pwm_force_immediately);

    pwm_config_t config = {0};
    pwm_get_default_pwm_config(TEST_PWM, &config);
    config.enable_output = true;
    /* config.dead_zone_in_half_cycle = 50; */

    pwm_config_pwm(TEST_PWM, 0, &config, false);
    pwm_config_pwm(TEST_PWM, 1, &config, false);
    pwm_config_pwm(TEST_PWM, 2, &config, false);
    pwm_config_pwm(TEST_PWM, 3, &config, false);
    pwm_config_pwm(TEST_PWM, 4, &config, false);
    pwm_config_pwm(TEST_PWM, 5, &config, false);
}

static void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(TEST_PWM);
    trgm_output_update_source(HPM_TRGM0, TRGM_TRGOCFG_PWM0_SYNCI, 1);
    trgm_output_update_source(HPM_TRGM0, TRGM_TRGOCFG_PWM0_SYNCI, 0);
}

/* output central aligned pwm */
void pwm_config_central_aligned_waveform(void)
{
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};
    uint32_t reload = (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 20000 - 1); /* 20KHz */

    init_pwm_pins(TEST_PWM);

    pwm_stop_counter(TEST_PWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(TEST_PWM, &pwm_config);

    pwm_config_force_cmd_timing(TEST_PWM, pwm_force_at_reload);
    pwm_set_reload(TEST_PWM, 0, reload);
    pwm_set_start_count(TEST_PWM, 0, 0);

    /* central_aligned and 50% duty cycle */
    uint32_t cmp0 = (reload >> 1) - (reload >> 2);
    uint32_t cmp1 = (reload >> 1) + (reload >> 2);
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = cmp0;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_modify;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = cmp1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_config.enable_output = true;

    for (uint8_t i = 0; i < 6; i++) {
        if (status_success != pwm_setup_waveform(TEST_PWM, i, &pwm_config, 0, cmp_config, 2)) {
            printf("failed to setup waveform\n");
            break;
        }
    }

    pwm_start_counter(TEST_PWM);
    pwm_issue_shadow_register_lock_event(TEST_PWM);
}