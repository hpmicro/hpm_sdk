/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeov2_drv.h"

#define TEST_QEO BOARD_QEO
#define TEST_QEO_WAVE_LINES   (4U)
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS
#define QEO_POSITION_MAX_VAL  (0x100000000UL)
#define TEST_QEO_SAMPLE_COUNT (256U)

uint32_t wave0_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];
uint32_t wave1_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];
uint32_t wave2_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];

/* QEO generates wave signals based on the software injection position */
void qeo_gen_wave_signal_software(void)
{
    qeo_wave_mode_t config;
    qeo_wave_get_default_mode_config(TEST_QEO, &config);
    config.wave_type = qeo_wave_cosine;
    qeo_wave_config_mode(TEST_QEO, &config);
    qeo_wave_set_resolution_lines(TEST_QEO, TEST_QEO_WAVE_LINES);

    qeo_wave_set_phase_shift(TEST_QEO, 1, 120);   /* 120° */
    qeo_wave_set_phase_shift(TEST_QEO, 2, 240);   /* 240° */

    /* Limit the numerical range of output, set to 0 for not limit(MAX value 0xFFFFFFFF) */
    /* qeo_wave_set_pwm_cycle(TEST_QEO, 16000); */

    printf("QEO generate wave with sofeware inject postion\n");

    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_WAVE_LINES / TEST_QEO_SAMPLE_COUNT;
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);
        board_delay_us(10U);
        wave0_val[i] = qeo_get_wave_output_val(TEST_QEO, 0);
        wave1_val[i] = qeo_get_wave_output_val(TEST_QEO, 1);
        wave2_val[i] = qeo_get_wave_output_val(TEST_QEO, 2);
    }
    qeo_disable_software_position_inject(TEST_QEO);

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

int main(void)
{
    board_init();
    printf("QEO DAC wave example\n");
    qeo_gen_wave_signal_software();

    while (1) {
    }

    return 0;
}
