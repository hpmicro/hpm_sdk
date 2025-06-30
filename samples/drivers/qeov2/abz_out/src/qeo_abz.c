/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEOv2 (Quadrature Encoder Output Version 2) ABZ Signal Generation Example
 *
 * This example demonstrates how QEOv2 generates ABZ quadrature encoder signals based on position information:
 * 1. QEO takes position input (from software or hardware) and converts it to ABZ signals
 * 2. Based on the configured resolution lines (1024 lines/rev), QEO generates A/B quadrature signals
 * 3. The position range (0 to 0x100000000) is divided according to resolution lines
 * 4. Maximum output frequency is limited to 250kHz to ensure signal quality
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeov2_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "hardware_pos.h"

/* QEOv2 configuration defines */
#define TEST_QEO BOARD_QEO
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS

#define TEST_QEO_ABZ_LINES    (1024U)   /* Number of encoder lines per revolution */
#define TEST_QEO_ABZ_MAX_FREQ (250000U) /* Maximum frequency of ABZ signals in Hz */
#define QEO_POSITION_MAX_VAL  (0x100000000UL)  /* Maximum position value (32-bit) */

/*
 * QEOv2 ABZ Signal Generation with Software Position Injection
 *
 * This function demonstrates ABZ signal generation using software-injected positions:
 * 1. Configures QEO for ABZ signal generation with specified resolution
 * 2. Sets maximum frequency to ensure proper signal timing
 * 3. Simulates forward rotation for 2 cycles:
 *    - Injects position values sequentially
 *    - Each position increment generates corresponding ABZ states
 *    - A/B signals generate quadrature waveforms
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

    /* Set encoder resolution and maximum frequency */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    /* Enable ABZ signal output */
    qeo_abz_enable_output(TEST_QEO);

    /* Calculate position increment per line and delay time */
    post_unit = QEO_POSITION_MAX_VAL / TEST_QEO_ABZ_LINES;  /* Position increment per line */
    if (period_us == 0) {
        delay_time = 1U;
    } else {
        delay_time = period_us;
    }

    /* Enable software position injection and simulate rotation */
    qeo_enable_software_position_inject(TEST_QEO);
    /* Forward rotation 2 cycle */
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
 * QEOv2 ABZ Signal Generation with Hardware Position Input
 *
 * This function demonstrates ABZ signal generation using hardware-provided positions:
 * 1. Configures QEO with same ABZ parameters as software mode
 * 2. Sets up hardware position input
 * 3. QEO continuously generates ABZ signals based on hardware position
 * 4. Maximum speed is limited by configured frequency:
 *    - TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us per revolution
 *    - Maximum speed = 1s / 4000us = 250 revolutions/second
 */
void qeo_gen_abz_signal_hardware(void)
{
    qeo_abz_mode_t config;

    printf("QEO generate ABZ signal with hardware provide position\n");

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

    /* Enable ABZ signal output */
    qeo_abz_enable_output(TEST_QEO);

    /* Configure hardware position generation */
    hardware_gen_postion_to_qeo();

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
