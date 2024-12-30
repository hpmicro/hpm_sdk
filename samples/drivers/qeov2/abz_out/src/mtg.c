/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "motor.h"

#if defined(HPMSOC_HAS_HPMSDK_MTG)
#include "hpm_mtg_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_MTGV2)
#include "hpm_mtgv2_drv.h"
#endif

#ifndef APP_MTG_BASE
#define APP_MTG_BASE HPM_MTG0
#endif

#define MTG_TRA_INDEX_0 (0)
#define MTG_TRA_CMD_INDEX0 (0)

#define MTG_TRA_JER_SHIFT (3)
#define MTG_TRA_ACC_SHIFT (6)
#define MTG_TRA_VEL_SHIFT (8)

void mtg_generate_trajectory(void)
{
    mtg_tra_cmd_cfg_t cmd_cfg = {0};

    /* Global Reset Mtg First */
    mtg_set_global_reset(APP_MTG_BASE);

    mtg_tra_set_shift(
        APP_MTG_BASE, MTG_TRA_INDEX_0, MTG_TRA_JER_SHIFT, MTG_TRA_ACC_SHIFT, MTG_TRA_VEL_SHIFT);

    /* Use TRA_CMD0 & SW TRIGGER */
    cmd_cfg.index = MTG_TRA_CMD_INDEX0;
    cmd_cfg.mode = cmd_mode_new_value;
    cmd_cfg.acc_preset = 0; /* Change Acc To 1 */
    cmd_cfg.rev_preset = 0;
    cmd_cfg.pos_preset = 0;
    cmd_cfg.vel_preset = 100;
    cmd_cfg.jer_preset = 0;
    cmd_cfg.object =
        cmd_object_rev | cmd_object_pos | cmd_object_vel | cmd_object_acc | cmd_object_jer;
    mtg_setup_link_cfg(APP_MTG_BASE, MTG_TRA_INDEX_0, link_sw_trigger, &cmd_cfg);
    mtg_soft_event_trigger(APP_MTG_BASE);
}