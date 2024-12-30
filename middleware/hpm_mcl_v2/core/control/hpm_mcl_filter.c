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

void hpm_mcl_filter_pll_type_i(mcl_filter_pll_type_i_t *pll, float sin_val, float cos_val, mcl_filter_pll_output_t *output)
{
    /* Define intermediate variables a0 to a4 for temporary storage during calculations. */
    float a0, a1;

    /* Calculate a0 based on the current phase estimate and the sine and cosine components of the input signal. */
    a0 = cosf(pll->x2) * pll->cfg.k1 * sin_val - sinf(pll->x2) * pll->cfg.k1 * cos_val;

    /* Pi controller */
    pll->x0 += pll->cfg.ki * a0;
    a1 = pll->x0 + a0 * pll->cfg.kp;

    /* Low pass filter */
    pll->x1 += pll->cfg.lowpass_k * (a1 - pll->x1);

    /* Integrator */
    pll->x2 += a1 * pll->cfg.c;
    pll->x2 = MCL_ANGLE_MOD_X(0, MCL_2PI, pll->x2);

    output->theta = pll->x2;
    output->speed = pll->x1;
}

void hpm_mcl_filter_pll_type_ii(mcl_filter_pll_type_ii_t *pll, float sin_val, float cos_val, mcl_filter_pll_output_t *output)
{
    /* Define intermediate variables a0 to a4 for temporary storage during calculations. */
    float a0, a1, a2, a3, a4;

    /* Calculate a0 based on the current phase estimate and the sine and cosine components of the input signal. */
    a0 = cosf(pll->x3) * pll->cfg.k1 * sin_val - sinf(pll->x3) * pll->cfg.k1 * cos_val;

    /* Update the state variable x0 by integrating the phase error. */
    pll->x0 += a0;
    a1 = pll->x0 * pll->cfg.c;

    /* Calculate a2 as the weighted difference between the current a1 and the previous state variable x1. */
    a2 = a1 - pll->x1 * pll->cfg.a;
    a3 = a2 * pll->cfg.b + a2;

    /* Update the state variable x2 by integrating a3. */
    pll->x2 += a3;
    a4 = pll->x2 * pll->cfg.c;

    /*  Update the state variables x1 and x3 for the next iteration. */
    pll->x1 = a2;
    pll->x3 = MCL_ANGLE_MOD_X(0, MCL_2PI, a4);

    /* Store the computed phase and speed in the output structure. */
    output->theta = pll->x3;
    output->speed = a3;
}
