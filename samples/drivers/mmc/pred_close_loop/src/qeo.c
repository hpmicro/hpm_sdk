/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeo_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "moto.h"

#define TEST_QEO HPM_QEO0
#define TEST_QEO_ABZ_MAX_FREQ (250000U)

void qeo_hardware_trig_into_pwm_safety(void)
{
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1, HPM_TRGM0_INPUT_SRC_VDD);
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1, HPM_TRGM0_INPUT_SRC_VSS);
}

hpm_stat_t qeo_gen_abz_signal(void)
{
    init_qeo_pins(TEST_QEO);

    qeo_abz_mode_t config;
    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    /* TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us, speed should less than 1s / 4000us = 250 r/s */
    qeo_abz_set_resolution_lines(TEST_QEO, 1024U);
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return status_fail;
    }

    return status_success;
}

#define QEO_PWM_LINE (50U)
void qeo_gen_pwm_output(void)
{
    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;     /* 4-phase */
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, QEO_PWM_LINE);

    /* config phase table */
    qeo_pwm_phase_output_table_t table0, table1, table2, table3;

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table0);
    table0.pwm2_output = qeo_pwm_output_not_force; /* PWM channel 2 */
    table0.pwm3_output = qeo_pwm_output_force_0;   /* PWM channel 3 */
    table0.pwm4_output = qeo_pwm_output_not_force; /* PWM channel 4 */
    table0.pwm5_output = qeo_pwm_output_force_0;   /* PWM channel 5 */
    qeo_pwm_config_phase_table(TEST_QEO, 0, &table0);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table1);
    table1.pwm2_output = qeo_pwm_output_force_0;
    table1.pwm3_output = qeo_pwm_output_not_force;
    table1.pwm4_output = qeo_pwm_output_not_force;
    table1.pwm5_output = qeo_pwm_output_force_0;
    qeo_pwm_config_phase_table(TEST_QEO, 1, &table1);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table2);
    table2.pwm2_output = qeo_pwm_output_force_0;
    table2.pwm3_output = qeo_pwm_output_not_force;
    table2.pwm4_output = qeo_pwm_output_force_0;
    table2.pwm5_output = qeo_pwm_output_not_force;
    qeo_pwm_config_phase_table(TEST_QEO, 2, &table2);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table3);
    table3.pwm2_output = qeo_pwm_output_not_force;
    table3.pwm3_output = qeo_pwm_output_force_0;
    table3.pwm4_output = qeo_pwm_output_force_0;
    table3.pwm5_output = qeo_pwm_output_not_force;
    qeo_pwm_config_phase_table(TEST_QEO, 3, &table3);

    /* config safety table */
    qeo_pwm_safety_output_table_t safety_table;
    qeo_pwm_get_default_safety_table_config(TEST_QEO, &safety_table);
    qeo_pwm_config_safety_table(TEST_QEO, &safety_table);

    /* trigger QEO PWM into safety mode, when receive bew postion, exit safety mode */
    qeo_hardware_trig_into_pwm_safety();

    pwm_config_central_aligned_waveform();
}
