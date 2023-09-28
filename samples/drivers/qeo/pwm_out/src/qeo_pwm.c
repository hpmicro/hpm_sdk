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
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "moto.h"

#define TEST_QEO BOARD_QEO
#define QEO_POSITION_MAX_VAL  (0x100000000UL)
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS
#define TEST_QEO_PWM_LINES    (4U)
#define TEST_QEO_SAMPLE_COUNT (256U)

void qeo_hardware_trig_into_pwm_safety(void)
{
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1, HPM_TRGM0_INPUT_SRC_VDD);
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1, HPM_TRGM0_INPUT_SRC_VSS);
}

void qeo_gen_pwm_signal_software(void)
{
    printf("QEO generate PWM signal with software inject postion\n");
    pwm_config_output();

    qeo_pwm_phase_output_table_t table0, table1, table2, table3;
    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, TEST_QEO_PWM_LINES);

    /* config force table */
    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table0);
    table0.pwm2_output = qeo_pwm_output_force_1; /* PWM channel 2 */
    table0.pwm3_output = qeo_pwm_output_force_0; /* PWM channel 3 */
    table0.pwm4_output = qeo_pwm_output_force_1; /* PWM channel 4 */
    table0.pwm5_output = qeo_pwm_output_force_0; /* PWM channel 5 */
    qeo_pwm_config_phase_table(TEST_QEO, 0, &table0);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table1);
    table1.pwm2_output = qeo_pwm_output_force_0;
    table1.pwm3_output = qeo_pwm_output_force_1;
    table1.pwm4_output = qeo_pwm_output_force_0;
    table1.pwm5_output = qeo_pwm_output_force_1;
    qeo_pwm_config_phase_table(TEST_QEO, 1, &table1);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table2);
    table2.pwm2_output = qeo_pwm_output_force_1;
    table2.pwm3_output = qeo_pwm_output_force_0;
    table2.pwm4_output = qeo_pwm_output_force_1;
    table2.pwm5_output = qeo_pwm_output_force_0;
    qeo_pwm_config_phase_table(TEST_QEO, 2, &table2);

    qeo_pwm_get_default_phase_table_config(TEST_QEO, &table3);
    table3.pwm2_output = qeo_pwm_output_force_0;
    table3.pwm3_output = qeo_pwm_output_force_1;
    table3.pwm4_output = qeo_pwm_output_force_0;
    table3.pwm5_output = qeo_pwm_output_force_1;
    qeo_pwm_config_phase_table(TEST_QEO, 3, &table3);

    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_PWM_LINES / TEST_QEO_SAMPLE_COUNT;
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_SAMPLE_COUNT * TEST_QEO_PWM_LINES; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);
        board_delay_us(10U);
        while (!qeo_check_calculate_finish(TEST_QEO)) {
        }
    }
    qeo_disable_software_position_inject(TEST_QEO);
}

void qeo_gen_pwm_signal_hardware(void)
{
    printf("QEO generate PWM signal with hardware(MMC) provide postion\n");
    pwm_config_central_aligned_waveform();

    qeo_pwm_mode_t config;
    qeo_pwm_get_default_mode_config(TEST_QEO, &config);
    config.phase_num = 4U;
    qeo_pwm_config_mode(TEST_QEO, &config);
    qeo_pwm_set_resolution_lines(TEST_QEO, TEST_QEO_PWM_LINES);

    /* config force table */
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

    /* config safety table */
    qeo_pwm_safety_output_table_t safety_table;
    qeo_pwm_get_default_safety_table_config(TEST_QEO, &safety_table);
    qeo_pwm_config_safety_table(TEST_QEO, &safety_table);

    /* trigger QEO PWM into safety mode, when receive bew postion, exit safety mode */
    qeo_hardware_trig_into_pwm_safety();

    /* mmc generate postion to qeo */
    trgm_pos_matrix_config(HPM_TRGM0, QEO_TRGM_POS, trgm_pos_matrix_in_from_mmc0_pos0, false);
    mmc_open_loop_pred();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

int main(void)
{
    board_init();
    printf("QEO PWM example\n");

    qeo_gen_pwm_signal_software();

    board_delay_ms(100U);

    qeo_gen_pwm_signal_hardware();

    while (1) {
    }

    return 0;
}
