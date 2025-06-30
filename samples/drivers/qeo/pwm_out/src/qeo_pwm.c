/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEO (Quadrature Encoder Output) Peripheral Example
 *
 * This example demonstrates how to use QEO peripheral to control stepper motors.
 * QEO can generate PWM outputs based on:
 * 1. Position input (from software or hardware like MMC)
 * 2. Predefined PWM commutation tables
 * 3. Resolution lines configuration
 *
 * The example shows both software position injection and hardware position input modes,
 * which can be used to control stepper motors (e.g. 2-phase 4-step stepper motor).
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeo_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "moto.h"

/* QEO configuration defines */
#define TEST_QEO BOARD_QEO                    /* QEO instance used in this example */
#define QEO_POSITION_MAX_VAL  (0x100000000UL) /* Maximum position value (32-bit) */
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS       /* TRGM position input source */
#define TEST_QEO_PWM_LINES    (4U)            /* Number of resolution lines */
#define TEST_QEO_SAMPLE_COUNT (256U)          /* Number of position samples per phase for software Position Injection Mode */

/**
 * @brief Trigger QEO into PWM safety mode
 *
 * This function configures TRGM to trigger QEO into safety mode.
 * In safety mode, PWM outputs will follow the safety table configuration.
 * QEO will exit safety mode when receiving new position input.
 */
void qeo_hardware_trig_into_pwm_safety(void)
{
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1, HPM_TRGM0_INPUT_SRC_VDD);
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1, HPM_TRGM0_INPUT_SRC_VSS);
}

/**
 * @brief Generate PWM signals using software position injection
 *
 * This function demonstrates QEO PWM generation with software position input:
 * 1. Configures 4-phase PWM output tables for 2-phase stepper motor control
 * 2. Sets resolution to 4 lines (4-step commutation)
 * 3. Injects position values via software to rotate the motor
 *
 * PWM output pattern for 2-phase stepper motor:
 * Phase   PWM2  PWM3  PWM4  PWM5
 * 0       1     0     1     0
 * 1       0     1     0     1
 * 2       1     0     1     0
 * 3       0     1     0     1
 */
void qeo_gen_pwm_signal_software(void)
{
    printf("QEO generate PWM signal with software inject postion\n");
    pwm_config_output();

    /* Configure QEO PWM mode with 4 phases */
    qeo_pwm_phase_output_table_t table0, table1, table2, table3;
    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, TEST_QEO_PWM_LINES);

    /* Configure phase 0 output table: PWM2=1, PWM3=0, PWM4=1, PWM5=0 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table0);
    table0.pwm2_output = qeo_pwm_output_force_1; /* PWM channel 2 */
    table0.pwm3_output = qeo_pwm_output_force_0; /* PWM channel 3 */
    table0.pwm4_output = qeo_pwm_output_force_1; /* PWM channel 4 */
    table0.pwm5_output = qeo_pwm_output_force_0; /* PWM channel 5 */
    qeo_pwm_config_phase_table(TEST_QEO, 0, &table0);

    /* Configure phase 1 output table: PWM2=0, PWM3=1, PWM4=0, PWM5=1 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table1);
    table1.pwm2_output = qeo_pwm_output_force_0;
    table1.pwm3_output = qeo_pwm_output_force_1;
    table1.pwm4_output = qeo_pwm_output_force_0;
    table1.pwm5_output = qeo_pwm_output_force_1;
    qeo_pwm_config_phase_table(TEST_QEO, 1, &table1);

    /* Configure phase 2 output table: PWM2=1, PWM3=0, PWM4=1, PWM5=0 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table2);
    table2.pwm2_output = qeo_pwm_output_force_1;
    table2.pwm3_output = qeo_pwm_output_force_0;
    table2.pwm4_output = qeo_pwm_output_force_1;
    table2.pwm5_output = qeo_pwm_output_force_0;
    qeo_pwm_config_phase_table(TEST_QEO, 2, &table2);

    /* Configure phase 3 output table: PWM2=0, PWM3=1, PWM4=0, PWM5=1 */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table3);
    table3.pwm2_output = qeo_pwm_output_force_0;
    table3.pwm3_output = qeo_pwm_output_force_1;
    table3.pwm4_output = qeo_pwm_output_force_0;
    table3.pwm5_output = qeo_pwm_output_force_1;
    qeo_pwm_config_phase_table(TEST_QEO, 3, &table3);

    /* Calculate position increment per step */
    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_PWM_LINES / TEST_QEO_SAMPLE_COUNT;

    /* Inject positions via software to rotate the motor */
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_SAMPLE_COUNT * TEST_QEO_PWM_LINES; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);
        board_delay_us(10U);
        while (!qeo_check_calculate_finish(TEST_QEO)) {
        }
    }
    qeo_disable_software_position_inject(TEST_QEO);
}

/**
 * @brief Generate PWM signals using hardware (MMC) position input
 *
 * This function demonstrates QEO PWM generation with hardware position input:
 * 1. Configures 4-phase PWM output tables with not-forced states
 * 2. Sets up safety mode configuration
 * 3. Uses MMC to provide position input for motor control
 *
 * In this mode, PWM outputs can be modulated by the PWM peripheral,
 * allowing for more sophisticated motor control.
 */
void qeo_gen_pwm_signal_hardware(void)
{
    printf("QEO generate PWM signal with hardware(MMC) provide postion\n");

    /* Configure QEO PWM mode with 4 phases */
    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, TEST_QEO_PWM_LINES);

    /* Configure phase tables with not-forced states to allow PWM modulation */
    qeo_pwm_phase_output_table_t table0, table1, table2, table3;
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table0);
    table0.pwm2_output = qeo_pwm_output_not_force; /* PWM channel 2 */
    table0.pwm3_output = qeo_pwm_output_force_0;   /* PWM channel 3 */
    table0.pwm4_output = qeo_pwm_output_not_force; /* PWM channel 4 */
    table0.pwm5_output = qeo_pwm_output_force_0;   /* PWM channel 5 */
    qeo_pwm_config_phase_table(TEST_QEO, 0, &table0);

    /* Configure remaining phase tables similarly */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table1);
    table1.pwm2_output = qeo_pwm_output_force_0;
    table1.pwm3_output = qeo_pwm_output_not_force;
    table1.pwm4_output = qeo_pwm_output_force_0;
    table1.pwm5_output = qeo_pwm_output_not_force;
    qeo_pwm_config_phase_table(TEST_QEO, 1, &table1);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table2);
    table2.pwm2_output = qeo_pwm_output_not_force;
    table2.pwm3_output = qeo_pwm_output_force_0;
    table2.pwm4_output = qeo_pwm_output_not_force;
    table2.pwm5_output = qeo_pwm_output_force_0;
    qeo_pwm_config_phase_table(TEST_QEO, 2, &table2);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table3);
    table3.pwm2_output = qeo_pwm_output_force_0;
    table3.pwm3_output = qeo_pwm_output_not_force;
    table3.pwm4_output = qeo_pwm_output_force_0;
    table3.pwm5_output = qeo_pwm_output_not_force;
    qeo_pwm_config_phase_table(TEST_QEO, 3, &table3);

    /* Configure safety table for safe state during transitions */
    qeo_pwm_safety_output_table_t safety_table;
    qeo_pwm_get_default_safety_table_config(TEST_QEO, &safety_table);
    qeo_pwm_config_safety_table(TEST_QEO, &safety_table);

    /* trigger QEO PWM into safety mode, when receive new postion, exit safety mode */
    qeo_hardware_trig_into_pwm_safety();

    /* Configure PWM for center-aligned operation */
    pwm_config_central_aligned_waveform();

    /* Configure MMC position output to QEO */
    trgm_pos_matrix_config(HPM_TRGM0, QEO_TRGM_POS, trgm_pos_matrix_in_from_mmc0_pos0, false);
    mmc_open_loop_pred();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

/**
 * @brief Main function demonstrating QEO-based stepper motor control
 *
 * The example shows two methods of stepper motor control using QEO:
 * 1. Software position injection - Basic stepping with forced PWM states
 * 2. Hardware position input - Advanced control with PWM modulation
 */
int main(void)
{
    board_init();
    printf("QEO PWM example\n");

    /* Demonstrate software position control */
    qeo_gen_pwm_signal_software();

    board_delay_ms(100U);

    /* Demonstrate hardware position control */
    qeo_gen_pwm_signal_hardware();

    while (1) {
    }

    return 0;
}
