/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hardware_pos.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgm_soc_drv.h"

#if defined(HPMSOC_HAS_HPMSDK_MTG)
#include "hpm_mtg_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_MTGV2)
#include "hpm_mtgv2_drv.h"
#endif

#if defined(HPMSOC_HAS_HPMSDK_QEIV2)
#include "hpm_qeiv2_drv.h"
#endif

#if defined(HPMSOC_HAS_HPMSDK_MTG) || defined(HPMSOC_HAS_HPMSDK_MTGV2)

#ifndef APP_MTG_BASE
#define APP_MTG_BASE HPM_MTG0
#endif

#define MTG_TRA_INDEX_0 (0)
#define MTG_TRA_CMD_INDEX0 (0)

#define MTG_TRA_JER_SHIFT (3)
#define MTG_TRA_ACC_SHIFT (6)
#define MTG_TRA_VEL_SHIFT (8)

static void mtg_generate_trajectory(void)
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

#elif defined(HPMSOC_HAS_HPMSDK_QEIV2)

#ifndef APP_QEI_BASE
#define APP_QEI_BASE BOARD_BLDC_QEIV2_BASE
#endif
#ifndef APP_QEI_IRQ
#define APP_QEI_IRQ  BOARD_BLDC_QEIV2_IRQ
#endif
#ifndef APP_MOTOR_CLK
#define APP_MOTOR_CLK BOARD_BLDC_QEI_CLOCK_SOURCE
#endif
#ifndef APP_ENCODER_PHASE_COUNT_PER_REV
#define APP_ENCODER_PHASE_COUNT_PER_REV BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV
#endif

static void qei_generate_position(void)
{
    qeiv2_mode_config_t mode_config = {0};

    /* config qei pins */
    init_qeiv2_ab_pins(APP_QEI_BASE);/* Attention : If there is no z-phase signal, please do not configure the z-phase pin !!! */

    /*  mode config */
    mode_config.work_mode = qeiv2_work_mode_abz;
    mode_config.spd_tmr_content_sel = qeiv2_spd_tmr_as_spd_tm;
    mode_config.z_count_inc_mode = qeiv2_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = APP_ENCODER_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.z_cali_ignore_ab = false;
    mode_config.phcnt_idx = 0;
    qeiv2_config_mode(APP_QEI_BASE, &mode_config);
}
#endif

void hardware_gen_postion_to_qeo(void)
{
#if defined(HPMSOC_HAS_HPMSDK_MTG) || defined(HPMSOC_HAS_HPMSDK_MTGV2)
    /* mtg generate position */
    printf("MTG generate position to QEO\n");

    trgm_pos_matrix_config(HPM_TRGM0, BOARD_QEO_TRGM_POS_PWM, trgm_pos_matrix_in_from_mtg0_pos0, false);

    mtg_generate_trajectory();
#elif defined(HPMSOC_HAS_HPMSDK_QEIV2)
    /* qei generate position */
    printf("QEI generate position to QEO\n");

    trgm_pos_matrix_config(HPM_TRGM0, BOARD_QEO_TRGM_POS_PWM, BOARD_QEO_PWM_TRGM_POS_IN, false);

    qei_generate_position();
#else
    #error no hardware peripheral generate position to qeo!!!
#endif
}