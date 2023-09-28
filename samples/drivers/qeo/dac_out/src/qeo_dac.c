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
#define TEST_QEO_WAVE_LINES   (4U)
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS
#define QEO_POSITION_MAX_VAL  (0x100000000UL)
#define TEST_QEO_SAMPLE_COUNT (256U)

uint16_t wave0_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];
uint16_t wave1_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];
uint16_t wave2_val[TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT];

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

    printf("QEO generate wave with sofeware inject postion\n");

    uint32_t pos_unit = QEO_POSITION_MAX_VAL / TEST_QEO_WAVE_LINES / TEST_QEO_SAMPLE_COUNT;
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_WAVE_LINES * TEST_QEO_SAMPLE_COUNT; i++) {
        qeo_software_position_inject(TEST_QEO, pos_unit * i);
        board_delay_us(10U);
        while (!qeo_check_calculate_finish(TEST_QEO)) {
        }
        wave0_val[i] = qeo_get_wave_output_val(TEST_QEO, 0);
        wave1_val[i] = qeo_get_wave_output_val(TEST_QEO, 1);
        wave2_val[i] = qeo_get_wave_output_val(TEST_QEO, 2);
    }
    qeo_disable_software_position_inject(TEST_QEO);

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

/* QEO generates wave signals based on the hardware provide position */
void qeo_gen_wave_signal_hardware(void)
{
    qeo_wave_mode_t config;
    qeo_wave_get_default_mode_config(TEST_QEO, &config);
    config.wave_type = qeo_wave_cosine;
    qeo_wave_config_mode(TEST_QEO, &config);
    qeo_wave_set_resolution_lines(TEST_QEO, TEST_QEO_WAVE_LINES);

    qeo_wave_set_phase_shift(TEST_QEO, 1, 120);   /* 120° */
    qeo_wave_set_phase_shift(TEST_QEO, 2, 240);   /* 240° */

    printf("QEO generate wave base on hardware(MMC) provide position\n");

    trgm_pos_matrix_config(HPM_TRGM0, QEO_TRGM_POS, trgm_pos_matrix_in_from_mmc0_pos0, false);
    mmc_open_loop_pred();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

int main(void)
{
    board_init();
    printf("QEO DAC wave example\n");

    init_dac(HPM_DAC0);
    init_dac(HPM_DAC1);
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac0, trgm_dac_matrix_in_from_qeo0_dac0, false);
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac1, trgm_dac_matrix_in_from_qeo0_dac1, false);

    qeo_gen_wave_signal_software();

    board_delay_ms(100);

    qeo_gen_wave_signal_hardware();

    while (1) {
    }

    return 0;
}
