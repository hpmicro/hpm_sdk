/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_clock_drv.h"
#include "hpm_mtgv2_drv.h"

int32_t mtg_calc_vel_preset(MTGV2_Type *base, clock_name_t clock, uint8_t tra_index, float speed)
{
    assert(tra_index < 2);
    uint32_t mtg_clock_fre = clock_get_frequency(clock);
    uint32_t shift_vel = MTGV2_TRA_SHIFT_VEL_SHIFT_GET(base->TRA[tra_index].SHIFT);
    assert(shift_vel <= 8);
    uint32_t rev_vel_shift = 12 - shift_vel;
    return (int32_t)(speed * ((uint64_t)1 << (rev_vel_shift * 8)) / mtg_clock_fre);
}

int32_t mtg_calc_acc_preset(MTGV2_Type *base, clock_name_t clock, uint8_t tra_index, float acc)
{
    assert(tra_index < 2);
    uint32_t mtg_clock_fre = clock_get_frequency(clock);
    uint32_t shift_vel = MTGV2_TRA_SHIFT_VEL_SHIFT_GET(base->TRA[tra_index].SHIFT);
    uint32_t shift_acc = MTGV2_TRA_SHIFT_ACC_SHIFT_GET(base->TRA[tra_index].SHIFT);
    assert(shift_acc <= shift_vel);
    uint32_t vel_acc_shift = shift_vel - shift_acc;
    return (int32_t)(acc * ((uint64_t)mtg_calc_vel_preset(base, clock, tra_index, 1) << (vel_acc_shift * 8)) / mtg_clock_fre);
}

int32_t mtg_calc_jer_preset(MTGV2_Type *base, clock_name_t clock, uint8_t tra_index, float jer)
{
    assert(tra_index < 2);
    uint32_t mtg_clock_fre = clock_get_frequency(clock);
    uint32_t shift_jerk = MTGV2_TRA_SHIFT_JER_SHIFT_GET(base->TRA[tra_index].SHIFT);
    uint32_t shift_acc = MTGV2_TRA_SHIFT_ACC_SHIFT_GET(base->TRA[tra_index].SHIFT);
    assert(shift_jerk <= shift_acc);
    uint32_t acc_jer_shift = shift_acc - shift_jerk;
    int32_t temp = mtg_calc_acc_preset(base, clock, tra_index, 1);
    return (int32_t)(jer * ((uint64_t)temp << (acc_jer_shift * 8)) / mtg_clock_fre);
}

void mtg_get_tra_lock_result(MTGV2_Type *base, uint8_t tra_index, mtg_lock_value_t *para)
{
    base->TRA[tra_index].CONTROL |= MTGV2_TRA_CONTROL_SW_LOCK_SET(1);
    base->TRA[tra_index].CONTROL &= ~MTGV2_TRA_CONTROL_SW_LOCK_MASK;
    para->rev = base->TRA[tra_index].LOCK_REV;
    para->pos = base->TRA[tra_index].LOCK_POS;
    para->vel = base->TRA[tra_index].LOCK_VEL;
    para->acc = base->TRA[tra_index].LOCK_ACC;
    para->time_stamp = base->TRA[tra_index].LOCK_TIME;
}

hpm_stat_t mtg_setup_event(MTGV2_Type *base, uint8_t event_index, mtg_event_param_t *param)
{
    assert(event_index < 4);

    switch (param->mode) {
    case event_mode_across:
        base->EVENT[event_index].PRESET_0 = param->preset.cross_param.cross_value_h;
        base->EVENT[event_index].PRESET_1 = param->preset.cross_param.cross_value_l;
        break;
    case event_mode_hold:
        base->EVENT[event_index].PRESET_0 = param->preset.hold_param.hold_value_h;
        base->EVENT[event_index].PRESET_1 = param->preset.hold_param.hold_value_l;
        base->EVENT[event_index].PRESET_2 = param->preset.hold_param.error_limit;
        base->EVENT[event_index].PRESET_3 = param->preset.hold_param.hold_clock_cnt;
        break;
    case event_mode_over_protect:
        base->EVENT[event_index].PRESET_0 = param->preset.over_protect_param.limit_value_h;
        base->EVENT[event_index].PRESET_1 = param->preset.over_protect_param.limit_value_l;
        break;
    case event_mode_time_match:
        base->EVENT[event_index].PRESET_0 = param->preset.time_param.clock_count;
        break;
    default:
        return status_invalid_argument;
    }

    uint32_t tmp = MTGV2_EVENT_CONTROL_ENABLE_SET(param->enable == true ? 1 : 0) |
                   MTGV2_EVENT_CONTROL_SOURCE_MUX_SET(param->obj_postion) |
                   MTGV2_EVENT_CONTROL_OBJECT_SET(param->obj) |
                   MTGV2_EVENT_CONTROL_MODE_SET(param->mode) |
                   MTGV2_EVENT_CONTROL_DIR_SET(param->dir) |
                   MTGV2_EVENT_CONTROL_DIR_MODE_SET(param->dir_mode) |
                   MTGV2_EVENT_CONTROL_TRIG_NUM_SET(param->trig_num);
    base->EVENT[event_index].CONTROL = tmp;
    if (param->irq_en) {
        switch (event_index) {
        case 0:
            mtg_enable_irq(base, irq_event0);
        break;
        case 1:
            mtg_enable_irq(base, irq_event1);
        break;
        case 2:
            mtg_enable_irq(base, irq_event2);
        break;
        case 3:
            mtg_enable_irq(base, irq_event3);
        break;
        default:
        break;
        }
    }
    return status_success;
}

hpm_stat_t mtg_setup_tra_limit(MTGV2_Type *base, uint8_t tra_index, mtg_tra_limit_param_t *param)
{
    assert(tra_index < 2);
    if (param->pos_step_limit_en) {
        base->TRA[tra_index].CONTROL |= MTGV2_TRA_STEP_LIMIT_CTRL_VEL_STEP_EN_SET(1);
        base->TRA[tra_index].POS_STEP_MAX = param->pos_step_max;
        base->TRA[tra_index].POS_STEP_MIN = param->pos_step_min;
    } else {
        base->TRA[tra_index].CONTROL &= ~MTGV2_TRA_STEP_LIMIT_CTRL_VEL_STEP_EN_MASK;
    }
    if (param->vel_step_limit_en) {
        base->TRA[tra_index].CONTROL |= MTGV2_TRA_STEP_LIMIT_CTRL_POS_STEP_EN_SET(1);
        base->TRA[tra_index].VEL_STEP_MAX = param->vel_step_max;
        base->TRA[tra_index].VEL_STEP_MIN = param->vel_step_min;
    } else {
        base->TRA[tra_index].CONTROL &= ~MTGV2_TRA_STEP_LIMIT_CTRL_POS_STEP_EN_MASK;
    }
    return status_success;
}

void mtg_setup_tra_software_pos_one_way_mode(MTGV2_Type *base, uint8_t tra_index, mtg_software_force_one_way_mode_t param)
{
    assert(tra_index < 2);

    base->TRA[tra_index].STEP_LIMIT_CTRL |= MTGV2_TRA_STEP_LIMIT_CTRL_POS_ONE_WAY_EN_SET(1) |
                                            MTGV2_TRA_STEP_LIMIT_CTRL_POS_ONE_WAY_MODE_SET(1) |
                                            MTGV2_TRA_STEP_LIMIT_CTRL_POS_ONE_WAY_FORCE_MODE_SET(param);
}

void mtg_setup_tra_hardware_pos_one_way_mode(MTGV2_Type *base, uint8_t tra_index, mtg_hardware_force_one_way_mode_t *param)
{
    assert(tra_index < 2);
    base->TRA[tra_index].STEP_LIMIT_CTRL |= MTGV2_TRA_STEP_LIMIT_CTRL_POS_ONE_WAY_EN_SET(1) |
                                            MTGV2_TRA_STEP_LIMIT_CTRL_POS_ONE_WAY_MODE_SET(0);
    base->TRA[tra_index].VEL_LIMIT_P = param->vel_limit_p;
    base->TRA[tra_index].VEL_LIMIT_N = param->vel_limit_n;
}

void mtg_disable_tra_pos_one_way_mode(MTGV2_Type *base, uint8_t tra_index)
{
    base->TRA[tra_index].STEP_LIMIT_CTRL &= ~MTGV2_TRA_STEP_LIMIT_CTRL_POS_ONE_WAY_EN_MASK;
}

void mtg_setup_tra_vel_one_way(MTGV2_Type *base, uint8_t tra_index, vel_one_way_mode_t mode, bool enable)
{
    assert(tra_index < 2);
    if (enable) {
        switch (mode) {
        case bigger_or_eq_zero:
            base->TRA[tra_index].CONTROL |= MTGV2_TRA_STEP_LIMIT_CTRL_VEL_ONE_WAY_EN_SET(1) |
                                            MTGV2_TRA_STEP_LIMIT_CTRL_VEL_ONE_WAY_MODE_SET(1);
            break;
        case smaller_or_eq_zero:
            base->TRA[tra_index].CONTROL &= ~MTGV2_TRA_STEP_LIMIT_CTRL_VEL_ONE_WAY_MODE_MASK;
            base->TRA[tra_index].CONTROL |= MTGV2_TRA_STEP_LIMIT_CTRL_VEL_ONE_WAY_EN_SET(1);
            break;
        }
    } else {
        base->TRA[tra_index].CONTROL &= ~MTGV2_TRA_STEP_LIMIT_CTRL_VEL_ONE_WAY_EN_MASK;
    }
}

void mtg_filter_get_default_filter_stage_param(mtg_filter_param_t *param)
{
    param->err_init = true;
    param->err_bypass_mode = mtg_filter_err_bypass_mode_hw;
    param->first_load_mode = mtg_filter_first_load_mode_without_acc;
    param->err_bypass_f_i_en = true;
    param->err_bypass_i_f_en = true;
    param->filter_time_constant_tp = 50000 / 5;
    param->filter_time_constant_tz = 1000000 / 5;
    param->filter_time_constant_tz_1 = 1000000 / 5;
    param->filter_gain = MTGV2_FILTER_GAIN_GAIN_T0_EN_SET(1) |
                         MTGV2_FILTER_GAIN_GAIN_T1_EN_SET(0) |
                         MTGV2_FILTER_GAIN_K_SET(256);

    param->filter_stage_shift[0] = MTGV2_FILTER_STAGE_SHIFT0_STAGE3_SHIFT1_SET(5);

    param->filter_stage_shift[1] = MTGV2_FILTER_STAGE_SHIFT1_STAGE5_SHIFT1_SET(7);

    param->filter_param_shift = MTGV2_FILTER_PARAM_SHIFT_ACC_SHIFT_PARAM_SET(0) |
                                MTGV2_FILTER_PARAM_SHIFT_VEL_SHIFT_PARAM_SET(7) |
                                MTGV2_FILTER_PARAM_SHIFT_GAIN_K_SHIFT_SET(6) |
                                MTGV2_FILTER_PARAM_SHIFT_GAIN_T0_SHIFT_SET(4) |
                                MTGV2_FILTER_PARAM_SHIFT_GAIN_T1_SHIFT_SET(3);

    param->filter_time_shift = MTGV2_FILTER_TIME_SHIFT_ACC_SHIFT_TIME1_SET(7) |
                               MTGV2_FILTER_TIME_SHIFT_VEL_SHIFT_TIME1_SET(7) |
                               MTGV2_FILTER_TIME_SHIFT_ACC_SHIFT_TIME0_SET(7) |
                               MTGV2_FILTER_TIME_SHIFT_VEL_SHIFT_TIME0_SET(7);

    param->filter_ff_shift = MTGV2_FILTER_FF_SHIFT_OUTPUT_ACC_SHIFT_SET(3) |
                             MTGV2_FILTER_FF_SHIFT_FILTER_ACC_SHIFT_SET(3) |
                             MTGV2_FILTER_FF_SHIFT_OUTPUT_VEL_SHIFT_SET(3) |
                             MTGV2_FILTER_FF_SHIFT_FILTER_VEL_SHIFT_SET(3);

    param->vel_init_value = 0;
    param->acc_init_value = 0;
    param->timeout_en = true;
}

void mtg_setup_filter(MTGV2_Type *base, mtg_filter_param_t *param)
{
    base->FILTER_STAGE_SHIFT0 = param->filter_stage_shift[0];
    base->FILTER_STAGE_SHIFT1 = param->filter_stage_shift[1];
    base->FILTER_TIME_CONSTANT_TP = param->filter_time_constant_tp;
    base->FILTER_TIME_CONSTANT_TZ = param->filter_time_constant_tz;
    base->FILTER_TIME_CONSTANT_TZ_1 = param->filter_time_constant_tz_1;
    base->FILTER_GAIN = param->filter_gain;
    base->FILTER_PARAM_SHIFT = param->filter_param_shift;
    base->FILTER_TIME_SHIFT = param->filter_time_shift;
    base->FILTER_FF_SHIFT = param->filter_ff_shift;
    base->FILTER_ACC_FF = param->acc_init_value;
    base->FILTER_VEL_FF = param->vel_init_value;
    uint32_t mask = MTGV2_FILTER_CONTROL_ERR_INIT_EN_MASK | MTGV2_FILTER_CONTROL_ERR_BYPASS_EN_MASK | \
                    MTGV2_FILTER_CONTROL_ERR_BYPASS_MODE_MASK | MTGV2_FILTER_CONTROL_ERR_BYPASS_I_F_EN_MASK | \
                    MTGV2_FILTER_CONTROL_ERR_BYPASS_F_I_EN_MASK | MTGV2_FILTER_CONTROL_SW_LOCK_MASK | \
                    MTGV2_FILTER_CONTROL_TIMEOUT_EN_MASK | MTGV2_FILTER_CONTROL_A_EN_MASK | \
                    MTGV2_FILTER_CONTROL_FF_MODE_MASK | MTGV2_FILTER_CONTROL_FF_EN_MASK | \
                    MTGV2_FILTER_CONTROL_FIRST_LOAD_MODE_MASK | MTGV2_FILTER_CONTROL_ENABLE_MASK;

    uint32_t tmp = MTGV2_FILTER_CONTROL_ERR_INIT_EN_SET(param->err_init) |
                   MTGV2_FILTER_CONTROL_ERR_BYPASS_EN_SET(param->err_bypass_en) |
                   MTGV2_FILTER_CONTROL_ERR_BYPASS_MODE_SET(param->err_bypass_mode) |
                   MTGV2_FILTER_CONTROL_ERR_BYPASS_I_F_EN_SET(param->err_bypass_i_f_en) |
                   MTGV2_FILTER_CONTROL_ERR_BYPASS_F_I_EN_SET(param->err_bypass_f_i_en) |
                   MTGV2_FILTER_CONTROL_A_EN_SET(param->acceleration_en) |
                   MTGV2_FILTER_CONTROL_FF_MODE_SET(param->ff_mode) |
                   MTGV2_FILTER_CONTROL_FF_EN_SET(param->ff_en) |
                   MTGV2_FILTER_CONTROL_FIRST_LOAD_MODE_SET(param->first_load_mode) |
                   MTGV2_FILTER_CONTROL_ENABLE_SET(param->enable);
    base->FILTER_CONTROL &= ~mask;
    base->FILTER_CONTROL |= tmp;
}

void mtg_setup_time(MTGV2_Type *base, mtg_time_init_param_t *param)
{
    if (param->enable) {
        if (param->index == 0) {
            base->FILTER_TIME0_SW_ADJUST = param->adjust_value;
            base->FILTER_CONTROL &= ~MTGV2_FILTER_CONTROL_SEL_TIME0_MASK;
            base->FILTER_CONTROL |= MTGV2_FILTER_CONTROL_SEL_TIME0_SET(param->source);
            base->FILTER_CONTROL |= MTGV2_FILTER_CONTROL_EN_TIME0_SET(1);
        } else if (param->index == 1) {
            base->FILTER_TIME1_SW_ADJUST = param->adjust_value;
            base->FILTER_CONTROL &= ~MTGV2_FILTER_CONTROL_SEL_TIME1_MASK;
            base->FILTER_CONTROL |= MTGV2_FILTER_CONTROL_SEL_TIME1_SET(param->source);
            base->FILTER_CONTROL |= MTGV2_FILTER_CONTROL_EN_TIME1_SET(1);
        }
    } else {
        if (param->index == 0) {
            base->FILTER_CONTROL &= ~MTGV2_FILTER_CONTROL_EN_TIME0_MASK;
        } else if (param->index == 1) {
            base->FILTER_CONTROL &= ~MTGV2_FILTER_CONTROL_EN_TIME1_MASK;
        }
    }
}

void mtg_setup_timeout(MTGV2_Type *base, mtg_timeout_param_t *param)
{
    if (param->enable) {
        base->FILTER_TIMEOUT_CNT = param->timeout_clock_count;
        base->FILTER_CONTROL |= MTGV2_FILTER_CONTROL_TIMEOUT_EN_SET(1);
    } else {
        base->FILTER_CONTROL &= ~MTGV2_FILTER_CONTROL_TIMEOUT_EN_MASK;
    }
}

void mtg_setup_link_cfg(MTGV2_Type *base, uint8_t tra_index, mtg_link_cfg_t link_cfg, mtg_tra_cmd_cfg_t *cmd_cfg)
{
    assert((tra_index < 2) && (cmd_cfg->index < 4));
    switch (cmd_cfg->index) {
    case 0:
        base->TRA[tra_index].LINK &= ~MTGV2_TRA_LINK_LINK_CFG_0_MASK;
        base->TRA[tra_index].LINK |= MTGV2_TRA_LINK_LINK_CFG_0_SET(link_cfg);
        break;
    case 1:
        base->TRA[tra_index].LINK &= ~MTGV2_TRA_LINK_LINK_CFG_1_MASK;
        base->TRA[tra_index].LINK |= MTGV2_TRA_LINK_LINK_CFG_1_SET(link_cfg);
        break;
    case 2:
        base->TRA[tra_index].LINK &= ~MTGV2_TRA_LINK_LINK_CFG_2_MASK;
        base->TRA[tra_index].LINK |= MTGV2_TRA_LINK_LINK_CFG_2_SET(link_cfg);
        break;
    case 3:
        base->TRA[tra_index].LINK &= ~MTGV2_TRA_LINK_LINK_CFG_3_MASK;
        base->TRA[tra_index].LINK |= MTGV2_TRA_LINK_LINK_CFG_3_SET(link_cfg);
        break;
    }

    base->TRA[tra_index].CMD[cmd_cfg->index].JER_PRESET = cmd_cfg->jer_preset;
    base->TRA[tra_index].CMD[cmd_cfg->index].ACC_PRESET = cmd_cfg->acc_preset;
    base->TRA[tra_index].CMD[cmd_cfg->index].VEL_PRESET = cmd_cfg->vel_preset;
    base->TRA[tra_index].CMD[cmd_cfg->index].POS_PRESET = cmd_cfg->pos_preset;
    base->TRA[tra_index].CMD[cmd_cfg->index].REV_PRESET = cmd_cfg->rev_preset;
    base->TRA[tra_index].CMD[cmd_cfg->index].CONTROL &= ~MTGV2_TRA_CMD_CONTROL_MODE_MASK;
    base->TRA[tra_index].CMD[cmd_cfg->index].CONTROL &= ~MTGV2_TRA_CMD_CONTROL_OBJECT_MASK;
    base->TRA[tra_index].CMD[cmd_cfg->index].CONTROL |= MTGV2_TRA_CMD_CONTROL_OBJECT_SET(cmd_cfg->object) |
                                                    MTGV2_TRA_CMD_CONTROL_MODE_SET(cmd_cfg->mode);
}

void mtg_soft_event_trigger(MTGV2_Type *base)
{
    base->SW_EVENT = 1;
    base->SW_EVENT = 0;
}

void mtg_get_default_tra_shift(mtg_tra_shift_t *cfg)
{
    cfg->jerk_shift = 0;
    cfg->acc_shift = 4;
    cfg->vel_shift = 8;
}
