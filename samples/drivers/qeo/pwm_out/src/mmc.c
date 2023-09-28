/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_mmc_drv.h"
#include "moto.h"

#define TEST_MMC HPM_MMC0

void mmc_open_loop_pred(void)
{
    mmc_set_sysclk_freq(TEST_MMC, clock_get_frequency(BOARD_MOTOR_CLK_NAME));

    /* config pred mode */
    mmc_pred_mode_t config;
    mmc_pred_get_default_mode_config(TEST_MMC, &config);
    config.open_loop_mode = true;
    config.pred_mode = mmc_pred_continuously_repeat;
    mmc_pred_config_mode(TEST_MMC, 0, &config);

    /* config pred initial position */
    mmc_pos_or_delta_pos_input_t pred_pos = {0};
    mmc_get_default_pos_or_delta_pos_para(TEST_MMC, &pred_pos);
    pred_pos.speed = 20.0; /* r/s */
    pred_pos.cmd_mask = mmc_pos_update_speed;
    mmc_pred_config_pos_para(TEST_MMC, 0, &pred_pos, true);

    mmc_enable_pred(TEST_MMC, 0);
    mmc_enable_module(TEST_MMC);
}
