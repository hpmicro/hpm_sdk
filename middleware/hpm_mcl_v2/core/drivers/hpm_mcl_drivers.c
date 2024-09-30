/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include"hpm_mcl_drivers.h"

hpm_mcl_stat_t hpm_mcl_drivers_init(mcl_drivers_t *drivers, mcl_drivers_cfg_t *cfg)
{
    drivers->status = drivers_status_init;
    MCL_ASSERT(drivers != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);
    drivers->cfg = cfg;
    drivers->cfg->callback.init();
    drivers->cfg->callback.enable_all_drivers();
    drivers->status = drivers_status_run;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_drivers_update_bldc_duty(mcl_drivers_t *drivers, float duty_a, float duty_b, float duty_c)
{
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_a, duty_a) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_b, duty_b) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_c, duty_c) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_drivers_update_step_duty(mcl_drivers_t *drivers, float duty_a0, float duty_a1, float duty_b0, float duty_b1)
{
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_a0, duty_a0) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_a1, duty_a1) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_b0, duty_b0) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.update_duty_cycle(mcl_drivers_chn_b1, duty_b1) == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_drivers_update_error);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_disable_all_drivers(mcl_drivers_t *drivers)
{
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.disable_all_drivers() == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_fail);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_enable_all_drivers(mcl_drivers_t *drivers)
{
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.enable_all_drivers() == mcl_success,
                                    drivers->status = drivers_status_fail,
                                    mcl_fail);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_disable_drivers(mcl_drivers_t *drivers, mcl_drivers_channel_t chn)
{
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.disable_drivers(chn) == mcl_success,
                                drivers->status = drivers_status_fail,
                                mcl_fail);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_enable_drivers(mcl_drivers_t *drivers, mcl_drivers_channel_t chn)
{
    MCL_ASSERT_EXEC_CODE_AND_RETURN(drivers->cfg->callback.enable_drivers(chn) == mcl_success,
                                drivers->status = drivers_status_fail,
                                mcl_fail);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_drivers_block_update(mcl_drivers_t *drivers, mcl_motor_dir_t dir, uint8_t sector)
{
    if (dir == motor_dir_back) {
        switch (sector) {
        case 2:
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        case 3:
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        case 4:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        case 5:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        case 6:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        case 1:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        default:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        }
    } else if (dir == motor_dir_forward) {
        switch (sector) {
        case 2:
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ch);
            break;
        case 3:
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            break;
        case 4:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_cl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            break;
        case 5:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_cl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            break;
        case 6:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        case 1:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            hpm_mcl_enable_drivers(drivers, mcl_drivers_chn_ch);
            break;
        default:
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ah);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_al);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bh);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_bl);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_ch);
            hpm_mcl_disable_drivers(drivers, mcl_drivers_chn_cl);
            break;
        }
    } else {
        MCL_ASSERT(false, mcl_invalid_argument);
    }
    return mcl_success;
}
