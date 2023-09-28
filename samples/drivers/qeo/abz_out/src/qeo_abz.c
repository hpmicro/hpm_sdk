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
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS
#define QEO_CLK_NAME BOARD_MOTOR_CLK_NAME

#define TEST_QEO_ABZ_LINES    (1024U)
#define TEST_QEO_ABZ_MAX_FREQ (250000U)
#define QEO_POSITION_MAX_VAL  (0x100000000UL)

/* QEO generates ABZ signals based on the software injection position */
void qeo_gen_abz_signal_software(void)
{
    qeo_abz_mode_t config;
    uint32_t delay_time;
    uint32_t post_unit;
    uint32_t freq = clock_get_frequency(QEO_CLK_NAME);
    uint32_t period_us = 1000000 / TEST_QEO_ABZ_MAX_FREQ;

    printf("QEO generate ABZ signal with sofeware inject postion\n");

    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    /* Forward rotation 2 cycle */
    post_unit = QEO_POSITION_MAX_VAL / TEST_QEO_ABZ_LINES;
    if (period_us == 0) {
        delay_time = 1U;
    } else {
        delay_time = period_us;
    }
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_ABZ_LINES * 2; i += 16) {
        qeo_software_position_inject(TEST_QEO, post_unit * i);
        /* Wait for the ABZ signal to reach the position of software injection */
        board_delay_us(delay_time * 16);
    }
    qeo_software_position_inject(TEST_QEO, 0);
    qeo_disable_software_position_inject(TEST_QEO);
}

/* QEO generates ABZ signals based on the hardware provide position */
void qeo_gen_abz_signal_hardware(void)
{
    qeo_abz_mode_t config;
    uint32_t freq = clock_get_frequency(QEO_CLK_NAME);

    printf("QEO generate ABZ signal with hardware(MMC) provide postion\n");

    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    /* TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us, speed should less than 1s / 4000us = 250 r/s */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    trgm_pos_matrix_config(HPM_TRGM0, QEO_TRGM_POS, trgm_pos_matrix_in_from_mmc0_pos0, false);
    mmc_open_loop_pred();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

int main(void)
{
    board_init();
    init_qeo_pins(TEST_QEO);
    printf("QEO ABZ example\n");

    qeo_gen_abz_signal_software();

    board_delay_ms(100U);

    qeo_gen_abz_signal_hardware();

    while (1) {
    }

    return 0;
}
