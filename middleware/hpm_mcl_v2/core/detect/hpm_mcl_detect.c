/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_mcl_detect.h"

hpm_mcl_stat_t hpm_mcl_detect_init(mcl_detect_t *detect, mcl_detect_cfg_t *cfg, mcl_loop_t *loop, mcl_encoder_t *encoder,
    mcl_analog_t *analog, mcl_drivers_t *drivers)
{
    MCL_ASSERT(detect != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop != NULL, mcl_invalid_pointer);
    MCL_ASSERT((cfg->en_submodule_detect.encoder == false) || (encoder != NULL), mcl_invalid_pointer);
    MCL_ASSERT((cfg->en_submodule_detect.analog == false) || (analog != NULL), mcl_invalid_pointer);
    MCL_ASSERT((cfg->en_submodule_detect.drivers == false) || (drivers != NULL), mcl_invalid_pointer);
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);

    detect->drivers = drivers;
    detect->cfg = cfg;
    detect->status.analog = &analog->status;
    detect->status.control_loop = &loop->status;
    detect->status.drivers = &drivers->status;
    detect->status.encoder = &encoder->status;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_detect_loop(mcl_detect_t *detect)
{
    if (detect->cfg->enable_detect) {
        if (((*detect->status.analog == analog_status_fail) && (detect->cfg->en_submodule_detect.analog == true)) || \
            ((*detect->status.control_loop == loop_status_fail) && (detect->cfg->en_submodule_detect.loop == true)) || \
            ((*detect->status.drivers == drivers_status_fail) && (detect->cfg->en_submodule_detect.drivers == true)) || \
            ((*detect->status.encoder == encoder_status_fail) && (detect->cfg->en_submodule_detect.encoder == true))) {
            detect->cfg->callback.disable_output();
            detect->cfg->callback.user_process(detect->status);
        }
    }
    return mcl_success;
}
