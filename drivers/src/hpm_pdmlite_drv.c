/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_pdmlite_drv.h"

#ifndef HPM_PDMLITE_CLK_TOLERANCE
#define HPM_PDMLITE_CLK_TOLERANCE (4U)
#endif

/*
* @brief  Get default configuration
*
* sample_rate = MCLK / (2 * (PDM_CLK_HFDIV + 1)) / cic_dec_ratio
*/
void pdm_get_default_config(PDMLITE_Type *ptr, pdm_config_t *config)
{
    (void) ptr;
    config->sof_at_ref_clk_falling_edge = true;
    config->bypass_pdm_clk_div = false;
    config->enable_pdm_clk_out = true;
    config->pdm_clk_div = 11;
    config->capture_delay = 1;
    config->sigma_delta_order = PDM_CIC_SIGMA_DELTA_ORDER_6; /* cic order */
    config->cic_dec_ratio = 64;                              /* cic ration */
    config->post_scale = 12;                                 /* post scale */
}

hpm_stat_t pdm_init(PDMLITE_Type *ptr, pdm_config_t *config)
{
    if (pdm_is_running(ptr)) {
        pdm_stop(ptr);
    }

    pdm_software_reset(ptr);

    ptr->CTRL = PDMLITE_CTRL_SOF_FEDGE_SET(config->sof_at_ref_clk_falling_edge)
        | PDMLITE_CTRL_CAPT_DLY_SET(config->capture_delay)
        | PDMLITE_CTRL_PDM_CLK_HFDIV_SET(config->pdm_clk_div)
        | PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_SET(config->bypass_pdm_clk_div)
        | PDMLITE_CTRL_PDM_CLK_OE_SET(config->enable_pdm_clk_out);

    /* enable 8 channel, channel 0-3 capture at low-level of PDMLITE_CLK, channel 4-7 capture at high level of PDMLITE_CLK */
    ptr->CH_CTRL = 0xF000FF;
    ptr->CIC_CFG = PDMLITE_CIC_CFG_POST_SCALE_SET(config->post_scale)
        | PDMLITE_CIC_CFG_SGD_SET(config->sigma_delta_order)
        | PDMLITE_CIC_CFG_CIC_DEC_RATIO_SET(config->cic_dec_ratio);

    return status_success;
}

static bool pdm_calculate_clk_divider(uint32_t mclk_in_hz, uint32_t sample_rate_hz, uint8_t cic_dec_ratio, uint8_t *div_out)
{
    uint8_t div;
    uint32_t k, remainder;
    uint32_t temp, delta;

    /* sample_rate = MCLK / (2 * (PDM_CLK_HFDIV + 1)) / cic_dec_ratio */
    k = sample_rate_hz * cic_dec_ratio * 2;
    div = mclk_in_hz / k - 1;
    remainder = mclk_in_hz % k;
    if (remainder >= k / 2U) {
        div++;
    }

    if ((div < 1) || (div > 15)) {
        return false;
    }

    temp = sample_rate_hz * (2 * (div + 1)) * cic_dec_ratio;
    delta = (mclk_in_hz > temp) ? (mclk_in_hz - temp) : (temp - mclk_in_hz);
    if ((delta * 100 / mclk_in_hz) > HPM_PDMLITE_CLK_TOLERANCE) {
        return false;
    }

    *div_out = div;
    return true;
}

hpm_stat_t pdm_config_sample_rate(PDMLITE_Type *ptr, uint32_t mclk_in_hz, uint32_t sample_rate_hz)
{
    uint8_t div;
    uint8_t cic_dec_ratio = PDMLITE_CIC_CFG_CIC_DEC_RATIO_GET(ptr->CIC_CFG);
    if (!pdm_calculate_clk_divider(mclk_in_hz, sample_rate_hz, cic_dec_ratio, &div)) {
        return status_invalid_argument;
    }

    pdm_config_divider(ptr, div, false);

    return status_success;
}
