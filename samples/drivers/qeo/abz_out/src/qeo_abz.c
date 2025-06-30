/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEO (Quadrature Encoder Output) ABZ Signal Generation Example
 *
 * This example demonstrates how QEO generates incremental ABZ signals based on position information:
 * 1. QEO takes position input (from software or hardware) and converts it to ABZ quadrature signals
 * 2. Based on the configured resolution lines, QEO generates A/B quadrature signals and Z index pulse
 * 3. The position range (0 to 0x100000000) is divided according to resolution lines
 * 4. Each line generates 4 states in A/B signals (4x resolution)
 * 5. Z signal generates one pulse per revolution
 * 6. Supports position synchronization for ABZ signals output
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
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS

#define TEST_QEO_ABZ_LINES    (1024U)   /* Number of encoder lines per revolution */
#define TEST_QEO_ABZ_MAX_FREQ (250000U) /* Maximum frequency of ABZ signals in Hz */
#define QEO_POSITION_MAX_VAL  (0x100000000UL)  /* Maximum position value (32-bit) */

/*
 * QEO ABZ Signal Generation with Software Position Injection
 *
 * This function demonstrates ABZ signal generation using software-injected positions:
 * 1. Configures QEO for ABZ signal generation with specified resolution
 * 2. Sets maximum frequency to ensure proper signal timing
 * 3. Simulates forward rotation for 2 cycles:
 *    - Injects position values sequentially
 *    - Each position increment generates corresponding ABZ states
 *    - A/B signals generate quadrature waveforms
 *    - Z signal generates index pulse once per revolution
 */
void qeo_gen_abz_signal_software(void)
{
    qeo_abz_mode_t config;
    uint32_t delay_time;
    uint32_t post_unit;
    uint32_t period_us = 1000000 / TEST_QEO_ABZ_MAX_FREQ;  /* Period in microseconds */

    printf("QEO generate ABZ signal with software inject position\n");

    /* Initialize QEO with default ABZ mode configuration */
    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    /* Set encoder resolution (lines per revolution) */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    /* Configure maximum frequency based on system clock and target frequency */
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    /* Calculate position increment per line (total position range / number of lines) */
    post_unit = QEO_POSITION_MAX_VAL / TEST_QEO_ABZ_LINES;
    /* Calculate delay time to match target frequency */
    if (period_us == 0) {
        delay_time = 1U;
    } else {
        delay_time = period_us;
    }

    /* Enable software position injection and simulate rotation */
    qeo_enable_software_position_inject(TEST_QEO);
    for (uint32_t i = 0; i < TEST_QEO_ABZ_LINES * 2; i += 16) {
        qeo_software_position_inject(TEST_QEO, post_unit * i);  /* Inject new position */
        /* Wait for ABZ signals to reach injected position (16 steps per delay) */
        board_delay_us(delay_time * 16);
    }
    /* Reset position to 0 and disable software injection */
    qeo_software_position_inject(TEST_QEO, 0);
    qeo_disable_software_position_inject(TEST_QEO);
}

/*
 * QEO ABZ Signal Generation with Hardware Position Input
 *
 * This function demonstrates ABZ signal generation using hardware-provided positions:
 * 1. Configures QEO with same ABZ parameters as software mode
 * 2. Sets up hardware position input from MMC through TRGM
 * 3. QEO continuously generates ABZ signals based on MMC position
 * 4. Maximum speed is limited by configured frequency:
 *    - TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us per revolution
 *    - Maximum speed = 1s / 4000us = 250 revolutions/second
 */
void qeo_gen_abz_signal_hardware(void)
{
    qeo_abz_mode_t config;

    printf("QEO generate ABZ signal with hardware(MMC) provide position\n");

    /* Initialize QEO with default ABZ mode configuration */
    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    /* Configure resolution and maximum frequency */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    /* TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us, speed should less than 1s / 4000us = 250 r/s */
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    /* Configure hardware position input from MMC */
    trgm_pos_matrix_config(HPM_TRGM0, QEO_TRGM_POS, trgm_pos_matrix_in_from_mmc0_pos0, false);
    mmc_open_loop_pred();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

int main(void)
{
    board_init();
    init_qeo_pins(TEST_QEO);  /* Initialize QEO output pins for ABZ signals */
    printf("QEO ABZ example\n");

    /* Demonstrate software position injection mode */
    qeo_gen_abz_signal_software();

    board_delay_ms(100U);

    /* Demonstrate hardware position input mode */
    qeo_gen_abz_signal_hardware();

    while (1) {
    }

    return 0;
}
