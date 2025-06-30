/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_pdm_drv.h"

#ifndef HPM_PDM_CLK_TOLERANCE
#define HPM_PDM_CLK_TOLERANCE (4U)
#endif

#define PDM_DECIMATION_RATIO_AFTER_CIC (3U)

/*
* @brief  Get default configuration
*
* sample_rate = MCLK / (2 * (PDM_CLK_HFDIV + 1)) / cic_dec_ratio / PDM_DECIMATION_RATIO_AFTER_CIC
*/
void pdm_get_default_config(PDM_Type *ptr, pdm_config_t *config)
{
    (void) ptr;
    config->sof_at_ref_clk_falling_edge = true;
    config->bypass_pdm_clk_div = false;
    config->enable_pdm_clk_out = true;
    config->pdm_clk_div = 3;
    config->capture_delay = 1;
    config->dec_after_cic = PDM_DECIMATION_RATIO_AFTER_CIC; /* Deprecated: This parameter is no longer used */
    config->post_scale = 12;
    config->sigma_delta_order = PDM_CIC_SIGMA_DELTA_ORDER_6;
    config->cic_dec_ratio = 64;
    config->enable_hpf = true;
}

hpm_stat_t pdm_init(PDM_Type *ptr, pdm_config_t *config)
{
    if (pdm_is_running(ptr)) {
        pdm_stop(ptr);
    }
    /* pdm_software_reset(ptr); */

    /* ptr->CTRL = PDM_CTRL_DIS_CLK_GATE_MASK; */
    ptr->CTRL = PDM_CTRL_SOF_FEDGE_SET(config->sof_at_ref_clk_falling_edge)
        | PDM_CTRL_DEC_AFT_CIC_SET(PDM_DECIMATION_RATIO_AFTER_CIC) /* decimation rate after cic fix to 3 */
        | PDM_CTRL_CAPT_DLY_SET(config->capture_delay)
        | PDM_CTRL_PDM_CLK_HFDIV_SET(config->pdm_clk_div)
        | PDM_CTRL_PDM_CLK_DIV_BYPASS_SET(config->bypass_pdm_clk_div)
        | PDM_CTRL_PDM_CLK_OE_SET(config->enable_pdm_clk_out)
        | PDM_CTRL_HPF_EN_SET(config->enable_hpf);

    ptr->CH_CTRL = 0xF000FF;
    ptr->CH_CFG = 0x50000;
    ptr->CIC_CFG = PDM_CIC_CFG_POST_SCALE_SET(config->post_scale)
        | PDM_CIC_CFG_SGD_SET(config->sigma_delta_order)
        | PDM_CIC_CFG_CIC_DEC_RATIO_SET(config->cic_dec_ratio);

    return status_success;
}

static bool pdm_calculate_clk_divider(uint32_t mclk_in_hz, uint32_t sample_rate_hz, uint8_t cic_dec_ratio, uint8_t *div_out)
{
    uint8_t div;
    uint32_t k, remainder;
    uint32_t temp, delta;

    /* sample_rate = MCLK / (2 * (PDM_CLK_HFDIV + 1)) / cic_dec_ratio / PDM_DECIMATION_RATIO_AFTER_CIC */
    k = sample_rate_hz * PDM_DECIMATION_RATIO_AFTER_CIC * cic_dec_ratio * 2;
    div = mclk_in_hz / k - 1;
    remainder = mclk_in_hz % k;
    if (remainder >= k / 2U) {
        div++;
    }

    if ((div < 1) || (div > 15)) {
        return false;
    }

    temp = sample_rate_hz * (2 * (div + 1)) * cic_dec_ratio * PDM_DECIMATION_RATIO_AFTER_CIC;
    delta = (mclk_in_hz > temp) ? (mclk_in_hz - temp) : (temp - mclk_in_hz);
    if ((delta * 100 / mclk_in_hz) > HPM_PDM_CLK_TOLERANCE) {
        return false;
    }

    *div_out = div;
    return true;
}

hpm_stat_t pdm_config_sample_rate(PDM_Type *ptr, uint32_t mclk_in_hz, uint32_t sample_rate_hz)
{
    uint8_t div;
    uint8_t cic_dec_ratio = PDM_CIC_CFG_CIC_DEC_RATIO_GET(ptr->CIC_CFG);
    if (!pdm_calculate_clk_divider(mclk_in_hz, sample_rate_hz, cic_dec_ratio, &div)) {
        return status_invalid_argument;
    }

    pdm_config_divider(ptr, div, false);

    return status_success;
}
