/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_plb_drv.h"
#include "hpm_qei_encoder.h"

#define PHASE_COUNT_DEFAULT BOARD_PLB_PHASE_COUNT_DEFAULT
#define FILTER_LENGTH_DEFAULT BOARD_PLB_FILTER_LENGTH_DEFAULT
#define QEI_A_PIN_SOURCE BOARD_PLB_QEI_A_PIN_SOURCE
#define QEI_B_PIN_SOURCE BOARD_PLB_QEI_B_PIN_SOURCE
#define QEI_Z_PIN_SOURCE BOARD_PLB_QEI_Z_PIN_SOURCE

#define PLB_ABZ_USE_CHANNEL plb_qei_encoder_channel_0

int main(void)
{
    /* Define a configuration structure for the PLB QEI */
    plb_qei_encoder_cfg_t cfg;
    hpm_stat_t status;

    /* Initialize the board */
    board_init();
    /* Initialize the PLB A/B pins */
    init_plb_ab_pins();
    /* Configure the PLB QEI settings */
    cfg.phase_cnt = PHASE_COUNT_DEFAULT;
    cfg.trgm_input.qei_a = QEI_A_PIN_SOURCE;
    cfg.trgm_input.qei_b = QEI_B_PIN_SOURCE;
    cfg.trgm_input.qei_z = QEI_Z_PIN_SOURCE;
    cfg.mode = plb_abz_mode;
    cfg.filter_length = FILTER_LENGTH_DEFAULT;
    cfg.enable_z_pin = false;
    cfg.enable_phase = true;

    /* Initialize the PLB ABZ encoder */
    status = plb_qei_encoder_init(BOARD_PLB_COUNTER, BOARD_PLB_TRGM, PLB_ABZ_USE_CHANNEL, &cfg);
    if (status == status_fail) {
        printf("PLB ABZ encoder init failed.\r\n");
    } else if (status == status_unsupport_plb) {
        printf("PLB ABZ encoder is not supported this soc.\r\n");
    }
    /* Print the PLB DEMO message */
    printf("PLB DEMO.\r\n");

    while (1) {
        /* Print the current phase count and lap count */
        printf("phase:%d. z:%d\r\n",
               plb_qei_encoder_get_phase_count(BOARD_PLB_COUNTER, PLB_ABZ_USE_CHANNEL),
               plb_qei_encoder_get_laps_number(BOARD_PLB_COUNTER, PLB_ABZ_USE_CHANNEL));

        /* Delay for 100 milliseconds */
        board_delay_ms(100);
    }

    return 0;
}

