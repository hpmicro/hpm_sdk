/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "moto.h"
#include "hpm_dac_drv.h"

void init_dac(DAC_Type *ptr)
{
    init_dac_pins(ptr);

    dac_config_t config;
    dac_get_default_config(&config);
    config.dac_mode = dac_mode_trig;
    dac_init(ptr, &config);

    dac_enable_conversion(ptr, true);
}