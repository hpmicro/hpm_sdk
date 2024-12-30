/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "assert.h"
#include "board.h"
#include "hpm_mtg_drv.h"
#include "hpm_mtg_regs.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "hpm_synt_drv.h"

#ifndef APP_QEIV2_BASE
#define APP_QEIV2_BASE BOARD_BLDC_QEIV2_BASE
#endif

#ifndef APP_QEIV2_IRQ
#define APP_QEIV2_IRQ BOARD_BLDC_QEIV2_IRQ
#endif

#ifndef APP_QEIV2_CLK_SRC
#define APP_QEIV2_CLK_SRC BOARD_BLDC_QEI_CLOCK_SOURCE
#endif

#ifndef APP_MTG_BASE
#define APP_MTG_BASE HPM_MTG0
#endif
#ifndef APP_MTG_IRQ
#define APP_MTG_IRQ IRQn_MTG0
#endif
#ifndef APP_MTG_CLK_SRC
#define APP_MTG_CLK_SRC clock_mtg0
#endif

#ifndef APP_MTG_TRA_INDEX
#define APP_MTG_TRA_INDEX  MTG_TRA_0
#endif

#define APP_MTG_ACC (10)
#define APP_MTG_THRESHOLD (50)

#define MTG_TRA_EVENT_INDEX_0 (0)
#define MTG_TRA_EVENT_INDEX_1 (1)
#define MTG_TRA_EVENT_INDEX_2 (2)

#define MTG_TRA_CMD_INDEX0 (0)
#define MTG_TRA_CMD_INDEX1 (1)
#define MTG_TRA_CMD_INDEX2 (2)
#define MTG_TRA_CMD_INDEX3 (3)

/* Static function declaration */
static void mtg_init(MTG_Type *base);
static void mtg_result_output(MTG_Type *base);

SDK_DECLARE_EXT_ISR_M(APP_MTG_IRQ, isr_mtg)
void isr_mtg(void)
{
    if (mtg_get_irq_status(APP_MTG_BASE, 0)) {
        /* clear iqr flags */
        mtg_clear_irq_status(APP_MTG_BASE, 0);
    }
    if (mtg_get_irq_status(APP_MTG_BASE, 1)) {
        /* clear iqr flags */
        mtg_clear_irq_status(APP_MTG_BASE, 1);
    }
    if (mtg_get_irq_status(APP_MTG_BASE, 2)) {
        /* clear iqr flags */
        mtg_clear_irq_status(APP_MTG_BASE, 2);
    }
}

/* Function definition */
int main(void)
{
    board_init();

    synt_enable_timestamp(HPM_SYNT, true);

    mtg_init(APP_MTG_BASE);

    intc_m_enable_irq_with_priority(APP_MTG_IRQ, 1);

    printf("mtg open loop trajectory example\n");

    mtg_result_output(APP_MTG_BASE);

    return 0;
}

void mtg_init(MTG_Type *base)
{
    mtg_tra_cmd_cfg_t cmd_cfg = { 0 };
    mtg_tra_shift_t tra_shift_config = { 0 };

    /* Global Reset Mtg First */
    mtg_set_global_reset(base);
    mtg_get_default_tra_shift(&tra_shift_config);
    mtg_tra_set_shift(base, APP_MTG_TRA_INDEX, tra_shift_config.jerk_shift, tra_shift_config.acc_shift, tra_shift_config.vel_shift);

    /* Use TRA_CMD0 & SW TRIGGER */
    cmd_cfg.index = MTG_TRA_CMD_INDEX0;
    cmd_cfg.mode = cmd_mode_new_value;
    cmd_cfg.acc_preset = mtg_calc_acc_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, APP_MTG_ACC); /* Change Acc To APP_MTG_ACC r/s2 */
    cmd_cfg.pos_preset = 0;
    cmd_cfg.vel_preset = 0;
    cmd_cfg.jer_preset = 0;
    cmd_cfg.object =
        cmd_object_rev | cmd_object_pos | cmd_object_vel | cmd_object_acc | cmd_object_jer;
    mtg_setup_link_cfg(base, APP_MTG_TRA_INDEX, link_sw_trigger, &cmd_cfg);
    mtg_soft_event_trigger(base);

    /* When vel is upto APP_MTG_THRESHOLD r/s, set acc to zero
     * Use TRA_CMD1 & EVENT0
     */
    mtg_event_param_t param = {0};
    param.obj = event_object_vel;
    param.mode = event_mode_across;
    param.enable = true;
    param.dir = event_dir_positive;
    param.obj_postion = event_source_tra0_output;
    param.trig_num = event_trig_repeat;
    param.preset.cross_param.cross_value_h = 0;
    param.preset.cross_param.cross_value_l = mtg_calc_vel_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, APP_MTG_THRESHOLD);
    param.irq_en = true;
    mtg_setup_event(HPM_MTG0, MTG_TRA_EVENT_INDEX_0, &param);

    cmd_cfg.acc_preset = 0; /* Change Acc To 0 */
    cmd_cfg.object = cmd_object_acc;
    cmd_cfg.index = MTG_TRA_CMD_INDEX1;
    mtg_setup_link_cfg(base, APP_MTG_TRA_INDEX, link_event0, &cmd_cfg);

    /* When vel hold APP_MTG_THRESHOLD +- 1 for 1s clocks, set acc to -APP_MTG_ACC
     * Use TRA_CMD2 & EVENT1
     */
    memset(&param, 0, sizeof(param));
    param.obj = event_object_vel;
    param.mode = event_mode_hold;
    param.enable = true;
    param.dir = event_dir_both;
    param.obj_postion = event_source_tra0_output;
    param.trig_num = event_trig_repeat;
    param.preset.hold_param.hold_value_l = mtg_calc_vel_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, APP_MTG_THRESHOLD);
    param.preset.hold_param.hold_value_h = 0;
    param.preset.hold_param.error_limit = mtg_calc_vel_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, 1);
    param.preset.hold_param.hold_clock_cnt = clock_get_frequency(APP_QEIV2_CLK_SRC);
    param.irq_en = true;
    mtg_setup_event(HPM_MTG0, MTG_TRA_EVENT_INDEX_1, &param);

    cmd_cfg.acc_preset = mtg_calc_acc_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, -1 * APP_MTG_ACC); /* Change Acc To -APP_MTG_ACC r/s2 */
    cmd_cfg.object = cmd_object_acc;
    cmd_cfg.index = MTG_TRA_CMD_INDEX2;
    mtg_setup_link_cfg(base, APP_MTG_TRA_INDEX, link_event1, &cmd_cfg);

    /* When vel is down to -APP_MTG_THRESHOLD r/s, set acc to APP_MTG_ACC r/s2
     * Use TRA_CMD3 & EVENT2
     */
    memset(&param, 0, sizeof(param));
    param.obj = event_object_vel;
    param.mode = event_mode_hold;
    param.enable = true;
    param.dir = event_dir_both;
    param.obj_postion = event_source_tra0_output;
    param.trig_num = event_trig_repeat;
    param.preset.hold_param.hold_value_l = mtg_calc_vel_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, -1 * APP_MTG_THRESHOLD);
    param.preset.hold_param.hold_value_h = 0;
    param.preset.hold_param.error_limit = mtg_calc_vel_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, 1);
    param.preset.hold_param.hold_clock_cnt = 1;
    param.irq_en = true;
    mtg_setup_event(HPM_MTG0, MTG_TRA_EVENT_INDEX_2, &param);

    cmd_cfg.acc_preset = mtg_calc_acc_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, APP_MTG_ACC); /* Change Acc To APP_MTG_ACC r/s2 */
    cmd_cfg.object = cmd_object_acc;
    cmd_cfg.index = MTG_TRA_CMD_INDEX3;
    mtg_setup_link_cfg(base, APP_MTG_TRA_INDEX, link_event2, &cmd_cfg);
}

void mtg_result_output(MTG_Type *base)
{
    mtg_lock_value_t mtg_lock_val = {0};
    while (1) {
        mtg_get_tra_lock_result(base, APP_MTG_TRA_INDEX, &mtg_lock_val);
        printf("/*LOCK_REV_POS=%f,LOCK_VEL=%f,LOCK_ACC=%f,LOCK_TIME=0x%x*/\n",
               mtg_lock_val.rev + (float)mtg_lock_val.pos * 1.0f / (float)0xFFFFFFFF,\
               1.0f * (int32_t)mtg_lock_val.vel / mtg_calc_vel_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, 1),\
               1.0f * (int32_t)mtg_lock_val.acc / mtg_calc_acc_preset(base, APP_MTG_CLK_SRC, MTG_TRA_CMD_INDEX0, 1),\
               mtg_lock_val.time_stamp);
        board_delay_ms(1000);
    }
}
