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
