/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_mcl_common.h"
#include "hpm_mcl_filter.h"

hpm_mcl_stat_t hpm_mcl_filter_iir_df1_init(mcl_filter_iir_df1_t *iir, mcl_filter_iir_df1_cfg_t *cfg, mcl_filter_iir_df1_memory_t *mem)
{
    MCL_ASSERT(iir != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(mem != NULL, mcl_invalid_pointer);
    iir->mem = mem;
    iir->cfg = cfg;
    memset(iir->mem, 0, iir->cfg->section * sizeof(mcl_filter_iir_df1_memory_t));

    return mcl_success;
}

float hpm_mcl_filter_iir_df1(mcl_filter_iir_df1_t *iir, float input)
{
    uint8_t i = 0;
    float y = 0;

    MCL_ASSERT_OPT(iir != NULL, mcl_invalid_pointer);
    iir->mem[0].x0 = input;
    for (i = 0; i < iir->cfg->section; i++) {
        y = iir->cfg->matrix[i].b0 * iir->mem[i].x0 +
            iir->cfg->matrix[i].b1 * iir->mem[i].x1 +
            iir->cfg->matrix[i].b2 * iir->mem[i].x2 -
            iir->cfg->matrix[i].a1 * iir->mem[i].y1 -
            iir->cfg->matrix[i].a2 * iir->mem[i].y2;
        iir->mem[i].x2 = iir->mem[i].x1;
        iir->mem[i].x1 = iir->mem[i].x0;
        iir->mem[i].y2 = iir->mem[i].y1;
        iir->mem[i].y1 = y;
        y *= iir->cfg->matrix[i].scale;
        if ((i+1) < iir->cfg->section) {
            iir->mem[i+1].x0 = y;
        }
    }

    return y;
}

