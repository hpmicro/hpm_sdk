/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_FILTER_H
#define _HPM_FILTER_H

#include "hpm_plb_drv.h"
#include "hpm_trgm_drv.h"

typedef enum filter_mode {
    filter_mode_rapid_change = 0,
    filter_mode_delay,
    filter_mode_stable_low,
    filter_mode_stable_high,
} filter_mode_t;

typedef struct filter_config {
    PLB_Type *plb;
    TRGM_Type *trgm;
    filter_mode_t mode;
    plb_chn_t plb_type_a_chn;
    plb_chn_t plb_type_b_chn;
    uint32_t filter_len;
    uint16_t input_trgmux_pin;
    uint16_t output_trgmux_pin;
} filter_config_t;

void plb_filter_init(filter_config_t *cfg);

#endif
