/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEOv2 PWM Output Example
 *
 * This example demonstrates PWM signal generation using QEOv2 peripheral:
 * 1. Software Position Injection Mode:
 *    - Generates PWM signals based on software-injected positions
 *    - Uses force output tables to control PWM channels
 *
 * 2. Hardware Position Input Mode:
 *    - Generates PWM signals based on hardware-provided positions
 *    - Supports safety mode with configurable safety tables
 *    - Demonstrates real-time PWM generation with position input
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeov2_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "pwm.h"
#include "hardware_pos.h"

/* Configuration defines */
#define TEST_QEO BOARD_QEO_PWM                /* QEO instance for PWM output */
#define QEO_POSITION_MAX_VAL  (0x100000000UL) /* Maximum position value (32-bit) */
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS_PWM  /* TRGM position suorce */
#define TEST_QEO_PWM_LINES    (4U)           /* Number of resolution lines */
#define TEST_QEO_SAMPLE_COUNT (256U)         /* Number of position samples per phase for software Position Injection Mode */

/*
 * Configure hardware trigger for PWM safety mode
 * This function sets up the trigger source to force PWM into safety mode
 * Safety mode is exited when new position data is received
 */
void qeo_hardware_trig_into_pwm_safety(void)
{
    trgm_output_update_source(HPM_TRGM0, BOARD_QEO_PWM_SAFETY_TRGM, HPM_TRGM0_INPUT_SRC_VDD);
    trgm_output_update_source(HPM_TRGM0, BOARD_QEO_PWM_SAFETY_TRGM, HPM_TRGM0_INPUT_SRC_VSS);
}

/*
 * Generate PWM signals using software position injection
 *
 * This function demonstrates:
 * 1. PWM force output table configuration for 4 phases
 * 2. Position-based PWM generation using software injection
 */
void qeo_gen_pwm_signal_software(void)
{
    printf("QEO generate PWM signal with software inject postion\n");
    pwm_config_output();

    /* Initialize QEO_PWM mode configuration */
    qeo_pwm_phase_output_table_t table0, table1, table2, table3;
    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;  /* Configure for 4-phase operation */
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, TEST_QEO_PWM_LINES);

    /* Configure Phase 0 force table - PWM channels 0-3 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table0);
    table0.pwm0_output = qeo_pwm_output_force_1; /* PWM channel 0 forced high */
    table0.pwm1_output = qeo_pwm_output_force_0; /* PWM channel 1 forced low */
    table0.pwm2_output = qeo_pwm_output_force_1; /* PWM channel 2 forced high */
    table0.pwm3_output = qeo_pwm_output_force_0; /* PWM channel 3 forced low */
    qeo_pwm_config_phase_table(TEST_QEO, 0, &table0);

    /* Configure Phase 1 force table - Alternate pattern */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table1);
    table1.pwm0_output = qeo_pwm_output_force_0;
    table1.pwm1_output = qeo_pwm_output_force_1;
    table1.pwm2_output = qeo_pwm_output_force_0;
    table1.pwm3_output = qeo_pwm_output_force_1;
    qeo_pwm_config_phase_table(TEST_QEO, 1, &table1);

    /* Configure Phase 2 force table - Same as Phase 0 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table2);
    table2.pwm0_output = qeo_pwm_output_force_1;
    table2.pwm1_output = qeo_pwm_output_force_0;
    table2.pwm2_output = qeo_pwm_output_force_1;
    table2.pwm3_output = qeo_pwm_output_force_0;
    qeo_pwm_config_phase_table(TEST_QEO, 2, &table2);

    /* Configure Phase 3 force table - Same as Phase 1 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table3);
    table3.pwm0_output = qeo_pwm_output_force_0;
    table3.pwm1_output = qeo_pwm_output_force_1;
    table3.pwm2_output = qeo_pwm_output_force_0;
    table3.pwm3_output = qeo_pwm_output_force_1;
    qeo_pwm_config_phase_table(TEST_QEO, 3, &table3);

    /* Enable PWM output */
    qeo_pwm_enable_output(TEST_QEO);

    /* Inject positions to rotation */
    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_PWM_LINES / TEST_QEO_SAMPLE_COUNT;
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_SAMPLE_COUNT * TEST_QEO_PWM_LINES; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);
        board_delay_us(10U);  /* Delay between position updates */
    }
    qeo_disable_software_position_inject(TEST_QEO);
}

/*
 * Generate PWM signals using hardware position input
 *
 * This function demonstrates:
 * 1. PWM generation with hardware position input
 * 2. Safety mode configuration and triggering
 * 3. Mixed force and non-force PWM outputs
 */
void qeo_gen_pwm_signal_hardware(void)
{
    printf("QEO generate PWM signal with hardware provide postion\n");

    /* Initialize QEO_PWM mode configuration */
    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;  /* Configure for 4-phase operation */
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, TEST_QEO_PWM_LINES);

    /* Configure Phase 0 table - Mix of forced and non-forced outputs */
    qeo_pwm_phase_output_table_t table0, table1, table2, table3;
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table0);
    table0.pwm0_output = qeo_pwm_output_not_force; /* PWM channel 0 normal PWM */
    table0.pwm1_output = qeo_pwm_output_force_0;   /* PWM channel 1 forced low */
    table0.pwm2_output = qeo_pwm_output_not_force; /* PWM channel 2 normal PWM */
    table0.pwm3_output = qeo_pwm_output_force_0;   /* PWM channel 3 forced low */
    qeo_pwm_config_phase_table(TEST_QEO, 0, &table0);

    /* Configure Phase 1-3 tables with alternating patterns */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table1);
    table1.pwm0_output = qeo_pwm_output_force_0;
    table1.pwm1_output = qeo_pwm_output_not_force;
    table1.pwm2_output = qeo_pwm_output_force_0;
    table1.pwm3_output = qeo_pwm_output_not_force;
    qeo_pwm_config_phase_table(TEST_QEO, 1, &table1);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table2);
    table2.pwm0_output = qeo_pwm_output_not_force;
    table2.pwm1_output = qeo_pwm_output_force_0;
    table2.pwm2_output = qeo_pwm_output_not_force;
    table2.pwm3_output = qeo_pwm_output_force_0;
    qeo_pwm_config_phase_table(TEST_QEO, 2, &table2);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table3);
    table3.pwm0_output = qeo_pwm_output_force_0;
    table3.pwm1_output = qeo_pwm_output_not_force;
    table3.pwm2_output = qeo_pwm_output_force_0;
    table3.pwm3_output = qeo_pwm_output_not_force;
    qeo_pwm_config_phase_table(TEST_QEO, 3, &table3);

    /* Configure safety mode table */
    qeo_pwm_safety_output_table_t safety_table;
    qeo_pwm_get_default_safety_table_config(TEST_QEO, &safety_table);
    qeo_pwm_config_safety_table(TEST_QEO, &safety_table);

    /* Setup safety mode triggering */
    qeo_hardware_trig_into_pwm_safety();

    /* Enable PWM output */
    qeo_pwm_enable_output(TEST_QEO);

    /* Configure PWM for center-aligned operation */
    pwm_generate_central_aligned_waveform();

    /* Start hardware position generation */
    hardware_gen_postion_to_qeo();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

/*
 * Main function
 * Demonstrates both software and hardware PWM generation modes
 */
int main(void)
{
    board_init();
    printf("QEO PWM example\n");

    /* Run software position injection demo */
    qeo_gen_pwm_signal_software();

    board_delay_ms(100U);  /* Delay between demos */

    /* Run hardware position input demo */
    qeo_gen_pwm_signal_hardware();

    while (1) {
    }

    return 0;
}
