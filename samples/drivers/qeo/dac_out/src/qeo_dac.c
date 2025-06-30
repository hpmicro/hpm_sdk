/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEO (Quadrature Encoder Output) Wave Generation Example
 *
 * This example demonstrates how QEO generates wave signals (sine/cosine...) based on position information:
 * 1. QEO takes position input (from software or hardware) as phase angle
 * 2. Based on the configured resolution lines and wave type, QEO calculates the output waveform
 * 3. The position range (0 to 0x100000000) is divided into segments according to resolution lines
 * 4. For each position value, QEO generates corresponding wave amplitude values
 * 5. Multiple wave outputs can be generated with different phase shifts
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeo_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "moto.h"

/* QEO configuration defines */
#define TEST_QEO BOARD_QEO
#define TEST_QEO_WAVE_LINES   (4U)    /* Number of resolution lines, determines wave period */
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS
#define QEO_POSITION_MAX_VAL  (0x100000000UL)  /* Maximum position value (32-bit) */
#define TEST_QEO_SAMPLE_COUNT (256U)   /* Number of samples per wave line */

/* Arrays to store wave output values for three channels */
uint16_t wave0_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];
uint16_t wave1_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];
uint16_t wave2_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];

/*
 * QEO Wave Generation with Software Position Injection
 *
 * This function demonstrates wave generation using software-injected positions:
 * 1. Configures QEO for cosine wave generation
 * 2. Sets resolution lines to define wave periods
 * 3. Configures phase shifts for wave1 (120°) and wave2 (240°)
 * 4. Injects position values sequentially to generate wave samples
 * 5. Collects and prints output values for all three waves
 */
void qeo_gen_wave_signal_software(void)
{
    qeo_wave_mode_t config;
    qeo_wave_get_default_mode_config(TEST_QEO, &config);
    config.wave_type = qeo_wave_cosine;  /* Set wave type to cosine */
    qeo_wave_config_mode(TEST_QEO, &config);
    qeo_wave_set_resolution_lines(TEST_QEO, TEST_QEO_WAVE_LINES);

    /* Configure 3-phase wave outputs with 120° phase differences */
    qeo_wave_set_phase_shift(TEST_QEO, 1, 120);   /* wave1: 120° shift */
    qeo_wave_set_phase_shift(TEST_QEO, 2, 240);   /* wave2: 240° shift */

    printf("QEO generate wave with software inject position\n");

    /* Calculate position increment for each sample */
    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_WAVE_LINES / TEST_QEO_SAMPLE_COUNT;

    qeo_enable_software_position_inject(TEST_QEO);
    /* Generate wave samples by injecting positions */
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);  /* Inject position value */
        board_delay_us(10U);
        /* Wait for wave calculation to complete */
        while (!qeo_check_calculate_finish(TEST_QEO)) {
        }
        /* Store wave output values for all three channels */
        wave0_val[i] = qeo_get_wave_output_val(TEST_QEO, 0);
        wave1_val[i] = qeo_get_wave_output_val(TEST_QEO, 1);
        wave2_val[i] = qeo_get_wave_output_val(TEST_QEO, 2);
    }
    qeo_disable_software_position_inject(TEST_QEO);

    /* Print wave output values for analysis */
    printf("qeo wave0 output:\n");
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        printf("%d\n", wave0_val[i]);
    }
    printf("qeo wave1 output:\n");
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        printf("%d\n", wave1_val[i]);
    }
    printf("qeo wave2 output:\n");
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        printf("%d\n", wave2_val[i]);
    }
}

/*
 * QEO Wave Generation with Hardware Position Input
 *
 * This function demonstrates wave generation using hardware-provided positions:
 * 1. Configures QEO for cosine wave generation with same parameters as software mode
 * 2. Sets up hardware position input from MMC through TRGM
 * 3. Enables motor timestamp for synchronization
 * 4. QEO will continuously generate waves based on MMC position input
 */
void qeo_gen_wave_signal_hardware(void)
{
    qeo_wave_mode_t config;
    qeo_wave_get_default_mode_config(TEST_QEO, &config);
    config.wave_type = qeo_wave_cosine;
    qeo_wave_config_mode(TEST_QEO, &config);
    qeo_wave_set_resolution_lines(TEST_QEO, TEST_QEO_WAVE_LINES);

    /* Configure 3-phase wave outputs with 120° phase differences */
    qeo_wave_set_phase_shift(TEST_QEO, 1, 120);   /* wave1: 120° shift */
    qeo_wave_set_phase_shift(TEST_QEO, 2, 240);   /* wave2: 240° shift */

    printf("QEO generate wave base on hardware(MMC) provide position\n");

    /* Configure hardware position input from MMC through TRGM */
    trgm_pos_matrix_config(HPM_TRGM0, QEO_TRGM_POS, trgm_pos_matrix_in_from_mmc0_pos0, false);
    mmc_open_loop_pred();

    /* Enable motor timestamp for synchronization */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

int main(void)
{
    board_init();
    printf("QEO DAC wave example\n");

    /* Initialize DAC and configure TRGM routing for QEO outputs */
    clock_add_to_group(clock_dac0, 0);
    clock_add_to_group(clock_dac1, 0);
    init_dac(HPM_DAC0);
    init_dac(HPM_DAC1);
    /* Route QEO wave outputs to DAC through TRGM */
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac0, trgm_dac_matrix_in_from_qeo0_dac0, false);
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac1, trgm_dac_matrix_in_from_qeo0_dac1, false);

    /* Demonstrate software position injection mode */
    qeo_gen_wave_signal_software();

    board_delay_ms(100);

    /* Demonstrate hardware position input mode */
    qeo_gen_wave_signal_hardware();

    while (1) {
    }

    return 0;
}
