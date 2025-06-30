/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "assert.h"
#include "board.h"
#include "hpm_mtgv2_regs.h"
#include "hpm_soc_feature.h"
#include "hpm_trgm_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_qeiv2_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_mtgv2_drv.h"

#ifndef APP_QEIV2_BASE
#define APP_QEIV2_BASE BOARD_BLDC_QEIV2_BASE
#endif

#ifndef APP_QEIV2_IRQ
#define APP_QEIV2_IRQ BOARD_BLDC_QEIV2_IRQ
#endif

#ifndef APP_MOTOR_CLK
#define APP_MOTOR_CLK BOARD_BLDC_QEI_CLOCK_SOURCE
#endif

#ifndef APP_MTG_BASE
#define APP_MTG_BASE  HPM_MTG0
#endif

#ifndef APP_MTG_TRA_INDEX
#define APP_MTG_TRA_INDEX  MTGV2_TRA_0
#endif

#ifndef APP_QEI_COUNT_PER_CYCLE
#define APP_QEI_COUNT_PER_CYCLE   4096
#endif

#ifndef APP_MOTOR_TARGET_SPEED_TURN_PER_SECOND
#define APP_MOTOR_TARGET_SPEED_TURN_PER_SECOND   10
#endif

#define QEI_TIMEOUT     (clock_get_frequency(APP_MOTOR_CLK) / APP_QEI_COUNT_PER_CYCLE / APP_MOTOR_TARGET_SPEED_TURN_PER_SECOND * 2)

static void mtg_test_loop(void)
{
    mtg_lock_value_t mtg_lock_val = {0};
    int32_t qei_rev;
    uint32_t qei_pos;
    float delta;
    uint32_t mot_clock = clock_get_frequency(APP_MOTOR_CLK);

    printf("time, QEI_REV, QEI_POS, MTG_REV, MTG_POS, MTG_VEL, MTG_ACC,MTG_REV_POS-QEI_REV_POS\r\n");
    while (1) {
        mtg_get_tra_lock_result(APP_MTG_BASE, APP_MTG_TRA_INDEX, &mtg_lock_val);
        qei_rev = qeiv2_get_current_count(APP_QEIV2_BASE, qeiv2_counter_type_z);
        qei_pos = qeiv2_get_current_count(APP_QEIV2_BASE, qeiv2_counter_type_speed);
        delta = mtg_lock_val.rev + 1.0f * (float)mtg_lock_val.pos / (float)0xFFFFFFFF - qei_rev - (float)qei_pos * 1.0f / (float)0xFFFFFFFF;

        printf("%f,%d,%f,%d,%f,%d,%d,%f\n", mtg_lock_val.time_stamp * 1.0f / mot_clock, \
                                               qei_rev, \
                                               (float)qei_pos * 1.0f / (float)0xFFFFFFFF, \
                                               mtg_lock_val.rev, \
                                               (float)mtg_lock_val.pos * 1.0f / (float)0xFFFFFFFF, \
                                               mtg_lock_val.vel, \
                                               mtg_lock_val.acc, \
                                               delta);
        board_delay_ms(100);
    }
}

static void qeiv2_init(void)
{
    qeiv2_mode_config_t mode_config = {0};

    mode_config.work_mode = qeiv2_work_mode_abz;
    mode_config.spd_tmr_content_sel = qeiv2_spd_tmr_as_pos_angle;
    mode_config.z_count_inc_mode = qeiv2_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = APP_QEI_COUNT_PER_CYCLE;
    mode_config.z_cali_enable = false;
    mode_config.z_cali_ignore_ab = false;
    mode_config.phcnt_idx = 0;
    qeiv2_config_mode(APP_QEIV2_BASE, &mode_config);
}

void mtg_init(MTGV2_Type *base)
{
    mtg_filter_param_t filter_config = { 0 };
    mtg_tra_shift_t tra_shift_config = { 0 };

    /* Global Reset Mtg First */
    mtg_set_global_reset(base);
    board_delay_ms(1);
    mtg_stop_global_reset(base);

    /* Setup Filter Configuration */
    mtg_filter_get_default_filter_stage_param(&filter_config);
    filter_config.enable = true;
    filter_config.err_bypass_en = true;
    filter_config.err_init = false;

    mtg_setup_filter(base, &filter_config);

    /* Setup Time Model */
    mtg_time_init_param_t time_cfg = { 0 };
    time_cfg.enable = true;
    time_cfg.source = mtg_time_input_from_filter;
    time_cfg.adjust_value = 0;
    time_cfg.index = APP_MTG_TRA_INDEX;
    mtg_setup_time(base, &time_cfg);

    /* Setup Trajectory Configutration */
    /* Setup Trajectory Shift, This is Binding To Filter Configuration */
    mtg_get_default_tra_shift(&tra_shift_config);
    mtg_tra_set_shift(base, APP_MTG_TRA_INDEX, tra_shift_config.jerk_shift, tra_shift_config.acc_shift, tra_shift_config.vel_shift);
    /* Setup Trajectory Limit and One Way Mode Configuration */
    mtg_tra_limit_param_t tra_limit_cfg = { 0 };
    tra_limit_cfg.pos_step_limit_en = false;
    tra_limit_cfg.vel_step_limit_en = false;

    mtg_setup_tra_limit(base, APP_MTG_TRA_INDEX, &tra_limit_cfg);
    mtg_disable_tra_pos_one_way_mode(base, APP_MTG_TRA_INDEX);

    /* Enable Mtg Timeout Functions */
    mtg_timeout_param_t timeout_cfg = { 0 };
    mtg_tra_cmd_cfg_t cmd_cfg = { 0 };
    timeout_cfg.enable = true;
    timeout_cfg.timeout_clock_count = QEI_TIMEOUT;
    mtg_setup_timeout(base, &timeout_cfg);

    /* Link Timeout Event To Trajectory0, When Timeout Event Occur, Reset Velocity To Zero */
    cmd_cfg.index = MTGV2_CMD_0;
    cmd_cfg.vel_preset = 0;
    cmd_cfg.acc_preset = 0;
    cmd_cfg.mode = cmd_mode_new_value;
    cmd_cfg.object = cmd_object_vel | cmd_object_acc;
    mtg_setup_link_cfg(base, APP_MTG_TRA_INDEX, link_event_timeout, &cmd_cfg);
    mtg_event_param_t event_param = {0};
    event_param.obj = event_object_vel;
    mtg_setup_event(base, 0, &event_param);
}

int main(void)
{
    board_init();

    synt_enable_timestamp(HPM_SYNT, true);

    trgm_pos_matrix_config(HPM_TRGM0, trgm_pos_matrix_output_to_mtg0, trgm_pos_matrix_in_from_qei0, false);

    init_qeiv2_ab_pins(APP_QEIV2_BASE);

    qeiv2_init();

    mtg_init(APP_MTG_BASE);

    printf("mtg qei example\n");

    mtg_test_loop();

    return 0;
}