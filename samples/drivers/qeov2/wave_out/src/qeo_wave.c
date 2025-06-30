/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEOv2 Wave Output Example
 *
 * This example demonstrates waveform generation using QEOv2 peripheral:
 * - Generates three-phase cosine waves with 120° phase shifts
 * - Supports software position injection for waveform generation
 * - Provides wave output value sampling and display
 * - Configurable resolution and amplitude control
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeov2_drv.h"

/* Configuration defines */
#define TEST_QEO BOARD_QEO                    /* QEO instance for wave output */
#define TEST_QEO_WAVE_LINES   (4U)           /* Number of resolution lines */
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS      /* TRGM position source */
#define QEO_POSITION_MAX_VAL  (0x100000000UL) /* Maximum position value (32-bit) */
#define TEST_QEO_SAMPLE_COUNT (256U)         /* Number of samples per wave cycle */

/* Arrays to store wave output values for analysis */
uint32_t wave0_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT]; /* Wave 0 output values */
uint32_t wave1_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT]; /* Wave 1 output values (120° shift) */
uint32_t wave2_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT]; /* Wave 2 output values (240° shift) */

/*
 * Generate wave signals using software position injection
 *
 * This function demonstrates:
 * 1. Wave output configuration for three-phase cosine generation
 * 2. Phase shift configuration (120° between phases)
 * 3. Position-based wave value generation and sampling
 *
 * Wave Generation Process:
 * - Configure wave type as cosine
 * - Set resolution lines for position to wave conversion
 * - Configure 120° phase shifts between outputs
 * - Inject positions and sample wave values
 * - Display sampled wave values for analysis
 */
void qeo_gen_wave_signal_software(void)
{
    /* Initialize wave mode configuration */
    qeo_wave_mode_t config;
    qeo_wave_get_default_mode_config(TEST_QEO, &config);
    config.wave_type = qeo_wave_cosine;  /* Set wave type to cosine */
    qeo_wave_config_mode(TEST_QEO, &config);
    qeo_wave_set_resolution_lines(TEST_QEO, TEST_QEO_WAVE_LINES);

    /* Configure phase shifts for three-phase output */
    qeo_wave_set_phase_shift(TEST_QEO, 1, 120);   /* Wave 1: 120° shift */
    qeo_wave_set_phase_shift(TEST_QEO, 2, 240);   /* Wave 2: 240° shift */

    /* Limit the numerical range of output, set to 0 for not limit(MAX value 0xFFFFFFFF) */
    /* qeo_wave_set_pwm_cycle(TEST_QEO, 16000); */

    printf("QEO generate wave with sofeware inject postion\n");

    /* Calculate position increment for smooth wave generation */
    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_WAVE_LINES / TEST_QEO_SAMPLE_COUNT;

    /* Enable software position injection and sample wave outputs */
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);
        board_delay_us(10U);  /* Delay for stable output */

        /* Sample wave output values */
        wave0_val[i] = qeo_get_wave_output_val(TEST_QEO, 0);  /* Base wave */
        wave1_val[i] = qeo_get_wave_output_val(TEST_QEO, 1);  /* 120° shifted wave */
        wave2_val[i] = qeo_get_wave_output_val(TEST_QEO, 2);  /* 240° shifted wave */
    }
    qeo_disable_software_position_inject(TEST_QEO);

    /* Display sampled wave values */
    printf("qeo wave0 output:\n");
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        printf("%u\n", wave0_val[i]);
    }
    printf("qeo wave1 output:\n");
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        printf("%u\n", wave1_val[i]);
    }
    printf("qeo wave2 output:\n");
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        printf("%u\n", wave2_val[i]);
    }
}

/*
 * Main function
 * Demonstrates wave generation using software position injection
 */
int main(void)
{
    board_init();
    printf("QEO DAC wave example\n");
    qeo_gen_wave_signal_software();

    while (1) {
    }

    return 0;
}
