/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * QEOv2 SEI to ABZ Signal Conversion Example
 *
 * This example demonstrates how to:
 * 1. Use SEI (Serial Encoder Interface) to acquire position information
 * 2. Convert SEI position to QEO ABZ quadrature signals
 * 3. Dynamically adjust ABZ signal width in SEI sampling interrupt
 */

#include "board.h"
#include <stdio.h>
#include "hpm_qeov2_drv.h"
#include "hpm_sei_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "motor.h"

/* Configuration defines */
#define TEST_QEO BOARD_QEO                    /* QEO instance for ABZ output */
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS      /* TRGM position source */

/* ABZ signal parameters */
#define TEST_QEO_ABZ_LINES    (4096U)        /* Encoder resolution (lines/rev) */
#define TEST_QEO_ABZ_MAX_FREQ (250000U)      /* Maximum ABZ frequency in Hz */
#define QEO_POSITION_MAX_VAL  (0x100000000UL) /* Maximum position value (32-bit) */

/* Position tracking variables */
uint32_t previous_pos;  /* Previous position from SEI */
uint32_t current_pos;   /* Current position from SEI */
uint32_t qeo_lines = TEST_QEO_ABZ_LINES;  /* Current encoder resolution */

/*
 * Generate ABZ signals using SEI hardware position input
 *
 * This function demonstrates:
 * 1. QEO ABZ signal configuration with SEI position input
 * 2. SEI position sampling setup and synchronization
 * 3. Dynamic ABZ signal width adjustment
 *
 * Signal Generation Process:
 * - Configure QEO for ABZ output with specified resolution
 * - Setup SEI for position sampling
 * - Enable position synchronization between SEI and QEO
 * - Use SEI interrupt for dynamic signal width adjustment
 */
void qeo_gen_abz_signal_hardware(void)
{
    qeo_abz_mode_t config;

    printf("QEO generate ABZ signal with hardware(SEI) provide postion\n");

    /* Initialize QEO ABZ mode configuration */
    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);

    /* Configure ABZ resolution and maximum frequency */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    /* TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us, speed should less than 1s / 4000us = 250 r/s */
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ)) {
        printf("config QEO abz max frequency failed\n");
        return;
    }

    /* Configure SEI for position sampling */
    sei_sample_position();

    /* Enable motor system timestamp for synchronization */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);

    /* Wait for valid SEI data */
    while (!sei_data_valid()) {
    }

    /* Setup position synchronization */
    qeo_abz_enable_position_sync(TEST_QEO, false);  /* Disable auto sync initially */
    qeo_enable_software_position_inject(TEST_QEO);   /* Enable software position control */

    /* Store initial position */
    previous_pos = sei_get_position_data();

    board_delay_ms(100);  /* Delay for stable operation */

    /* Enable QEO ABZ output */
    qeo_abz_enable_output(TEST_QEO);

    /* Configure SEI interrupt for dynamic width adjustment
     * The interrupt handler will:
     * 1. Sample new position from SEI
     * 2. Calculate position change rate
     * 3. Adjust ABZ signal width for uniform output
     */
    sei_clear_irq_flag(HPM_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event);
    sei_set_irq_enable(HPM_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event, true);
    intc_m_enable_irq_with_priority(BOARD_SEI_IRQn, 1);

    printf("device init finish\n");
}

/*
 * Main function
 * Initializes and runs the SEI to ABZ conversion
 */
int main(void)
{
    board_init();
    init_qeo_pins(TEST_QEO);  /* Initialize QEO output pins */
    printf("QEO SEI2ABZ example\n");

    qeo_gen_abz_signal_hardware();

    while (1) {
    }

    return 0;
}
