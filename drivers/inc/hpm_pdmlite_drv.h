/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PDMLITE_DRV_H
#define HPM_PDMLITE_DRV_H

#include "hpm_common.h"
#include "hpm_pdmlite_regs.h"

/**
 * @brief PDM Lite driver APIs
 * @defgroup pdmlite_interface PDM driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief PDM event
 */
#define PDM_EVENT_OFIFO_OVERFLOW_ERROR (PDM_ST_OFIFO_OVFL_ERR_MASK)
#define PDM_EVENT_CIC_OVERLOAD_ERROR (PDM_ST_CIC_OVLD_ERR_MASK)
#define PDM_EVENT_CIC_SAT_ERROR (PDM_ST_CIC_SAT_ERR_MASK)

/**
 * @brief PDM CIC sidma-delta filter order
 */
#define PDM_CIC_SIGMA_DELTA_ORDER_5 (2U)
#define PDM_CIC_SIGMA_DELTA_ORDER_6 (1U)
#define PDM_CIC_SIGMA_DELTA_ORDER_7 (0U)

/**
 * @brief PDM config
 */
typedef struct pdm_config {
    bool sof_at_ref_clk_falling_edge; /* sof at ref clk falling edge */
    bool bypass_pdm_clk_div; /* bypass PDM clock divider */
    bool enable_pdm_clk_out; /* enable PDM clock output */
    uint8_t pdm_clk_div;     /* PDM clock divider */
    uint8_t capture_delay;   /* adjust capture timing */
    uint8_t post_scale;      /* cic post scale */
    uint8_t sigma_delta_order; /* cic sigma_delta filter order */
    uint8_t cic_dec_ratio;     /* cic decimation rate */
} pdm_config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief check whether PDM is running
 *
 * @param [in] ptr PDM base address
 * @retval true in PDM is running
 */
static inline bool pdm_is_running(PDMLITE_Type *ptr)
{
    return ptr->RUN & PDMLITE_RUN_PDM_EN_MASK;
}

/**
 * @brief stop pdm
 *
 * @param [in] ptr PDM base address
 */
static inline void pdm_stop(PDMLITE_Type *ptr)
{
    ptr->RUN &= ~PDMLITE_RUN_PDM_EN_MASK;
}

/**
 * @brief start pdm
 *
 * @param [in] ptr PDM base address
 */
static inline void pdm_start(PDMLITE_Type *ptr)
{
    ptr->RUN |= PDMLITE_RUN_PDM_EN_MASK;
}

/**
 * @brief disable channel
 *
 * @param [in] ptr PDM base address
 * @param [in] channel_disable_mask channel mask
 */
static inline void pdm_disable_channel(PDMLITE_Type *ptr,
                                         uint16_t channel_disable_mask)
{
    ptr->CH_CTRL &= ~PDMLITE_CH_CTRL_CH_EN_SET(channel_disable_mask);
}

/**
 * @brief enable channel
 *
 * @param [in] ptr PDM base address
 * @param [in] capture_high_level_mask capture when PDM_CLK is high
 * @param [in] channel_enable_mask channel mask
 */
static inline void pdm_enable_channel(PDMLITE_Type *ptr,
                                         uint16_t capture_high_level_mask,
                                         uint16_t channel_enable_mask)
{
    ptr->CH_CTRL |= PDMLITE_CH_CTRL_CH_POL_SET(capture_high_level_mask)
        | PDMLITE_CH_CTRL_CH_EN_SET(channel_enable_mask);
}

/**
 * @brief disable pdm clock out
 *
 * @param [in] ptr PDM base address
 */
static inline void pdm_disable_pdm_clock_out(PDMLITE_Type *ptr)
{
    ptr->CTRL &= ~PDMLITE_CTRL_PDM_CLK_OE_MASK;
}

/**
 * @brief enable pdm clock out
 *
 * @param [in] ptr PDM base address
 */
static inline void pdm_enable_pdm_clock_out(PDMLITE_Type *ptr)
{
    ptr->CTRL |= PDMLITE_CTRL_PDM_CLK_OE_MASK;
}

/**
 * @brief pdm config cic
 *
 * @param [in] ptr PDM base address
 * @param [in] sigma_delta_order sidma-delta filter order
 * @param [in] dec_ratio Rate of down sampling
 * @param [in] post_scale output value post scale
 */
static inline void pdm_config_cic(PDMLITE_Type *ptr,
                                     uint8_t sigma_delta_order,
                                     uint8_t dec_ratio,
                                     uint8_t post_scale)
{
    ptr->CIC_CFG = PDMLITE_CIC_CFG_POST_SCALE_SET(post_scale)
        | PDMLITE_CIC_CFG_SGD_SET(sigma_delta_order)
        | PDMLITE_CIC_CFG_CIC_DEC_RATIO_SET(dec_ratio);
}

/**
 * @brief pdm software reset
 *
 * @param [in] ptr PDM base address
 */
static inline void pdm_software_reset(PDMLITE_Type *ptr)
{
    ptr->CTRL |= PDMLITE_CTRL_SFTRST_MASK;
    ptr->CTRL &= ~PDMLITE_CTRL_SFTRST_MASK;
}

/**
 * @brief pdm enable irq
 *
 * @param [in] ptr PDM base address
 * @param [in] mask pdm irq mask in ST register
 */
static inline void pdm_enable_irq(PDMLITE_Type *ptr, uint8_t mask)
{
    ptr->CTRL |= mask << (PDMLITE_CTRL_CIC_SAT_ERR_IE_SHIFT - PDMLITE_ST_CIC_SAT_ERR_SHIFT);
}

/**
 * @brief pdm disable irq
 *
 * @param [in] ptr PDM base address
 * @param [in] mask pdm irq mask in ST register
 */
static inline void pdm_disable_irq(PDMLITE_Type *ptr, uint8_t mask)
{
    ptr->CTRL &= ~(mask << (PDMLITE_CTRL_CIC_SAT_ERR_IE_SHIFT - PDMLITE_ST_CIC_SAT_ERR_SHIFT));
}

/**
 * @brief pdm initialization
 *
 * @param [in] ptr PDM base address
 * @param [in] config pdm_config_t
 * @retval hpm_stat_t status_invalid_argument or status_success
 */
hpm_stat_t pdm_init(PDMLITE_Type *ptr, pdm_config_t *config);

/**
 * @brief pdm get default config
 *
 * @note if the PDM source clock is 24.576MHz, the sample rate by default configuration is 16000Hz.
 *
 * @param [in] ptr PDM base address
 * @param [out] config pdm_config_t
 */
void pdm_get_default_config(PDMLITE_Type *ptr, pdm_config_t *config);

/**
 * @brief pdm config sample rate
 *
 * @note This function will calculate and configure PDM clock divider to get target sample rate.
 * if sample rate is not supported, should adjust mclk_in_hz or change cic_dec_ratio in pdm_init() function.
 *
 * @param [in] ptr PDM base address
 * @param [in] mclk_in_hz PDM source clock
 * @param [in] sample_rate_hz target sample rate
 * @retval hpm_stat_t status_invalid_argument or status_success
 */
hpm_stat_t pdm_config_sample_rate(PDMLITE_Type *ptr, uint32_t mclk_in_hz, uint32_t sample_rate_hz);

/**
 * @brief pdm config clock divider
 *
 * @param [in] ptr PDM base address
 * @param [in] div divider value
 * @param [in] bypass true: bypass divider
 */
static inline void pdm_config_divider(PDMLITE_Type *ptr, uint8_t div, bool bypass)
{
    assert((div >= 1U) && (div <= 15U));
    ptr->CTRL = (ptr->CTRL & ~(PDMLITE_CTRL_PDM_CLK_HFDIV_MASK | PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_MASK))
                | (PDMLITE_CTRL_PDM_CLK_HFDIV_SET(div) | PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_SET(bypass));
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* HPM_PDMLITE_DRV_H */

