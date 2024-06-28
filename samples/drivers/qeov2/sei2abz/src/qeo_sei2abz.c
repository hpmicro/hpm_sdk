/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeov2_drv.h"
#include "hpm_sei_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "motor.h"

#define TEST_QEO BOARD_QEO
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS

#define TEST_QEO_ABZ_LINES    (4096U)
#define TEST_QEO_ABZ_MAX_FREQ (250000U)
#define QEO_POSITION_MAX_VAL  (0x100000000UL)

uint32_t previous_pos;
uint32_t current_pos;
uint32_t qeo_lines = TEST_QEO_ABZ_LINES;


/* QEO generates ABZ signals based on the hardware provide position */
void qeo_gen_abz_signal_hardware(void)
{
    qeo_abz_mode_t config;

    printf("QEO generate ABZ signal with hardware(SEI) provide postion\n");

    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    /* TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us, speed should less than 1s / 4000us = 250 r/s */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    /* config SEI */
    sei_sample_position();

    /* enable timestamp for MOTOR system */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);

    while (!sei_data_valid()) {
    }

    /* sync */
    qeo_abz_enable_position_sync(TEST_QEO, false);
    qeo_enable_software_position_inject(TEST_QEO);

    previous_pos = sei_get_position_data();

    board_delay_ms(100);

    qeo_abz_enable_output(TEST_QEO);

    /* enable SEI sample interrupt */
    intc_m_enable_irq_with_priority(BOARD_SEI_IRQn, 1);
    sei_clear_irq_flag(HPM_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event);
    sei_set_irq_enable(HPM_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event, true);

    printf("device init finish\n");
}

int main(void)
{
    board_init();
    init_qeo_pins(TEST_QEO);
    printf("QEO SEI2ABZ example\n");

    qeo_gen_abz_signal_hardware();

    while (1) {
    }

    return 0;
}
