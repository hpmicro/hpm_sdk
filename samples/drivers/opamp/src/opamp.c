/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_opamp_drv.h"

#ifndef BOARD_APP_OPAMP_INP_PAD
#define BOARD_APP_OPAMP_INP_PAD inp_pad_vip0
#endif

void init_opamp(void)
{
    opamp_cfg_t opamp_cfg;

    opamp_cfg.mode = mode_follow;
    opamp_cfg.negative_input_pin = inm_pad_floating;
    opamp_cfg.positive_input_pin = BOARD_APP_OPAMP_INP_PAD;
    opamp_cfg.miller_cap = miller_cap_x_7;
    opamp_cfg.enable_phase_margin_cap = true;
    opamp_init(BOARD_APP_OPAMP, &opamp_cfg);
#if !defined(HPM_IP_FEATURE_OPAMP_NO_OUTPUT_SEL) || (HPM_IP_FEATURE_OPAMP_NO_OUTPUT_SEL == 0)
    /* OPAOUT_SEL 0: VOUT0 to OUT0, sense on pin */
    opamp_output_path_select(BOARD_APP_OPAMP, opamp_out0_sense_on);
#endif
    opamp_enable(BOARD_APP_OPAMP);
}

int main(void)
{
    board_init();
    clock_add_to_group(BOARD_APP_OPAMP_CLOCK, 0);
    init_opamp_pins();
    init_opamp();
    while (1) {
        ;
    }
}
