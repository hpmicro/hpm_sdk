/*
 * Copyright (c) 2021-2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_i2s_drv.h"

#define HPM_I2S_DRV_DEFAULT_RETRY_COUNT 5000U

#ifndef HPM_I2S_BCLK_TOLERANCE
#define HPM_I2S_BCLK_TOLERANCE (4U)
#endif

#ifndef HPM_I2S_SOFTWARE_RESET_DELAY
#define HPM_I2S_SOFTWARE_RESET_DELAY (10000U)
#endif

/* Wrapper macros for resetting each sub-module. */
#if defined(HPM_IP_FEATURE_I2S_HAS_SOFTWARE_RESET_STATUS) && (HPM_IP_FEATURE_I2S_HAS_SOFTWARE_RESET_STATUS)
#define I2S_DO_TX_RESET(ptr)     i2s_do_software_reset(ptr, I2S_CTRL_SFTRST_TX_MASK,    I2S_STA_SFTRST_TX_DONE_MASK)
#define I2S_DO_RX_RESET(ptr)     i2s_do_software_reset(ptr, I2S_CTRL_SFTRST_RX_MASK,    I2S_STA_SFTRST_RX_DONE_MASK)
#define I2S_DO_CLKGEN_RESET(ptr) i2s_do_software_reset(ptr, I2S_CTRL_SFTRST_CLKGEN_MASK, I2S_STA_SFTRST_CLKGEN_DONE_MASK)
#else
#define I2S_DO_TX_RESET(ptr)     i2s_do_software_reset(ptr, I2S_CTRL_SFTRST_TX_MASK,    0U)
#define I2S_DO_RX_RESET(ptr)     i2s_do_software_reset(ptr, I2S_CTRL_SFTRST_RX_MASK,    0U)
#define I2S_DO_CLKGEN_RESET(ptr) i2s_do_software_reset(ptr, I2S_CTRL_SFTRST_CLKGEN_MASK, 0U)
#endif

#define HPM_I2S_SLOT_MASK I2S_TXDSLOT_EN_MASK /* TX/RX has same SLOT MASK */

/* Clock configuration backup used during software reset */
typedef struct {
    uint32_t cfgr;
    uint32_t misc_cfgr;
    bool changed;
} i2s_cfg_backup_t;

static bool i2s_audio_depth_is_valid(uint8_t bits)
{
    /* i2s audio depth only support 16bits, 24bits, 32bits */
    if (bits == i2s_audio_depth_16_bits || bits == i2s_audio_depth_24_bits || bits == i2s_audio_depth_32_bits) {
        return true;
    }
    return false;
}

static bool i2s_channel_length_is_valid(uint8_t bits)
{
    /* i2s channel length only support 16bits or 32bits */
    if (bits == i2s_channel_length_16_bits || bits == i2s_channel_length_32_bits) {
        return true;
    }
    return false;
}

/* The software reset relies on a working internal BCLK */
static bool i2s_internal_bclk_is_valid(I2S_Type *ptr)
{
    uint32_t cfgr = ptr->CFGR;
    uint32_t misc_cfgr = ptr->MISC_CFGR;

    /* Internal BCLK is valid when:
     * - BCLK source is internal (BCLK_SEL_OP = 0)
     * - BCLK is not gated off (BCLK_GATEOFF = 0)
     * - MCLK is not gated off (MCLK_GATEOFF = 0, BCLK is derived from MCLK)
     * - BCLK divider is non-zero
     */
    if ((cfgr & (I2S_CFGR_BCLK_SEL_OP_MASK | I2S_CFGR_BCLK_GATEOFF_MASK)) != 0U) {
        return false;
    }
    if ((misc_cfgr & I2S_MISC_CFGR_MCLK_GATEOFF_MASK) != 0U) {
        return false;
    }
    if (I2S_CFGR_BCLK_DIV_GET(cfgr) == 0U) {
        return false;
    }
    return true;
}

/* work around: fill dummy data into TX fifo to avoid TX underflow during tx start */
hpm_stat_t i2s_fill_tx_dummy_data(I2S_Type *ptr, i2s_line_num_t data_line, uint8_t data_count)
{
    uint32_t retry = 0;

    if (data_count > I2S_SOC_MAX_TX_FIFO_DEPTH) {
        return status_invalid_argument;
    }

    /* check dummy data count in TX FIFO */
    while (i2s_get_tx_line_fifo_level(ptr, data_line) < data_count) {
        ptr->TXD[data_line] = 0;
        if (retry > HPM_I2S_DRV_DEFAULT_RETRY_COUNT * data_count) {
            return status_timeout;
        }
        retry++;
    }

    return status_success;
}


/* Reset an I2S sub-module (TX/RX/CLKGEN) by software.
 * If sta_mask is non-zero, poll STA[sta_mask] for self-clear; otherwise fall
 * back to a fixed delay loop for SOCs without a software-reset-done bit.
 */
static hpm_stat_t i2s_do_software_reset(I2S_Type *ptr, uint32_t ctrl_mask, uint32_t sta_mask)
{
    uint32_t retry = 0;

    /* software reset */
    ptr->CTRL |= ctrl_mask;
    ptr->CTRL &= ~ctrl_mask;

    if (sta_mask != 0U) {
        while ((ptr->STA & sta_mask) == 0U) {
            if (retry > HPM_I2S_DRV_DEFAULT_RETRY_COUNT) {
                return status_timeout;
            }
            retry++;
        }
    } else {
        /* No reset-done bit on this SOC, delay as fallback */
        for (volatile uint32_t i = 0; i < HPM_I2S_SOFTWARE_RESET_DELAY; i++) {
        }
    }

    return status_success;
}

/* Restore the clock configuration saved by i2s_prepare_bclk_for_reset */
static void i2s_restore_clk_config(I2S_Type *ptr, const i2s_cfg_backup_t *backup)
{
    if (backup->changed) {
        ptr->CFGR = backup->cfgr;
        ptr->MISC_CFGR = backup->misc_cfgr;
    }
}

/* The software reset relies on a valid BCLK. Save the current clock
 * configuration and switch to internal BCLK if the current BCLK is not valid
 */
static hpm_stat_t i2s_prepare_bclk_for_reset(I2S_Type *ptr, i2s_cfg_backup_t *backup)
{
    hpm_stat_t status = status_success;
    backup->changed = false;
    if (i2s_internal_bclk_is_valid(ptr)) {
        return status;
    }
    backup->changed = true;
    backup->cfgr = ptr->CFGR;
    backup->misc_cfgr = ptr->MISC_CFGR;
    ptr->CFGR = 0x0020008d;

    /* workaround for SFTRST_CLKGEN timing */
    ptr->MISC_CFGR |= I2S_MISC_CFGR_MCLK_GATEOFF_MASK; /* gateoff MCLK */

    for (volatile uint32_t i = 0; i < HPM_I2S_SOFTWARE_RESET_DELAY; i++) {  /* delay */
    }

    ptr->MISC_CFGR &= ~I2S_MISC_CFGR_MCLK_GATEOFF_MASK; /* open MCLK */

    status = I2S_DO_CLKGEN_RESET(ptr);

    if (status != status_success) {
        i2s_restore_clk_config(ptr, backup);
        backup->changed = false;
    }

    return status;
}


hpm_stat_t i2s_reset_tx(I2S_Type *ptr)
{
    i2s_cfg_backup_t cfg_backup;
    hpm_stat_t status = status_timeout;

    /* disable I2S TX module */
    ptr->CTRL &= ~I2S_CTRL_TX_EN_MASK;

    /* Software reset relies on a valid BCLK, switch to internal BCLK if needed */
     status = i2s_prepare_bclk_for_reset(ptr, &cfg_backup);

    if (status == status_success) {
        status = I2S_DO_TX_RESET(ptr);
    }

    i2s_restore_clk_config(ptr, &cfg_backup);

    return status;
}

hpm_stat_t i2s_reset_rx(I2S_Type *ptr)
{
    i2s_cfg_backup_t cfg_backup;
    hpm_stat_t status = status_timeout;

    /* disable I2S RX module */
    ptr->CTRL &= ~I2S_CTRL_RX_EN_MASK;

    /* Software reset relies on a valid BCLK, switch to internal BCLK if needed */
    status = i2s_prepare_bclk_for_reset(ptr, &cfg_backup);

    if (status == status_success) {
        status = I2S_DO_RX_RESET(ptr);
    }

    i2s_restore_clk_config(ptr, &cfg_backup);

    return status;
}

hpm_stat_t i2s_reset_tx_rx(I2S_Type *ptr)
{
    return i2s_reset_all(ptr);
}

/* The I2S software reset function relies on a working BCLK */
hpm_stat_t i2s_reset_all(I2S_Type *ptr)
{
    i2s_cfg_backup_t cfg_backup;
    hpm_stat_t status = status_timeout;

    /* disable I2S */
    ptr->CTRL &= ~I2S_CTRL_I2S_EN_MASK;

    /* Software reset relies on a valid BCLK, switch to internal BCLK if needed */
    status = i2s_prepare_bclk_for_reset(ptr, &cfg_backup);

    /* Reset TX and RX sequentially, they cannot be asserted at the same time */
    if (status == status_success) {
        status = I2S_DO_TX_RESET(ptr);
    }
    if (status == status_success) {
        status = I2S_DO_RX_RESET(ptr);
    }

    i2s_restore_clk_config(ptr, &cfg_backup);

    return status;
}

void i2s_get_default_config(I2S_Type *ptr, i2s_config_t *config)
{
    (void) ptr;
    config->invert_mclk_out = false;
    config->invert_mclk_in = false;
    config->use_external_mclk = false;
    config->invert_bclk_out = false;
    config->invert_bclk_in = false;
    config->use_external_bclk = false;
    config->invert_fclk_out = false;
    config->invert_fclk_in = false;
    config->use_external_fclk = false;
    config->enable_mclk_out = false;
    config->frame_start_at_rising_edge = false;
    config->tx_fifo_threshold = 4;
    config->rx_fifo_threshold = 4;
}

hpm_stat_t i2s_init(I2S_Type *ptr, i2s_config_t *config)
{
    hpm_stat_t status;

    status = i2s_reset_all(ptr);
    if (status != status_success) {
        return status;
    }

    ptr->CFGR = I2S_CFGR_INV_MCLK_OUT_SET(config->invert_mclk_out)
        | I2S_CFGR_INV_MCLK_IN_SET(config->invert_mclk_in)
        | I2S_CFGR_MCK_SEL_OP_SET(config->use_external_mclk)
        | I2S_CFGR_INV_BCLK_OUT_SET(config->invert_bclk_out)
        | I2S_CFGR_INV_BCLK_IN_SET(config->invert_bclk_in)
        | I2S_CFGR_BCLK_SEL_OP_SET(config->use_external_bclk)
        | I2S_CFGR_INV_FCLK_OUT_SET(config->invert_fclk_out)
        | I2S_CFGR_INV_FCLK_IN_SET(config->invert_fclk_in)
        | I2S_CFGR_FCLK_SEL_OP_SET(config->use_external_fclk)
        | I2S_CFGR_FRAME_EDGE_SET(config->frame_start_at_rising_edge);
    ptr->MISC_CFGR = (ptr->MISC_CFGR
            & ~(I2S_MISC_CFGR_MCLKOE_MASK
                | I2S_MISC_CFGR_MCLK_GATEOFF_MASK))
        | I2S_MISC_CFGR_MCLKOE_SET(config->enable_mclk_out);
    ptr->FIFO_THRESH = I2S_FIFO_THRESH_TX_SET(config->tx_fifo_threshold)
        | I2S_FIFO_THRESH_RX_SET(config->rx_fifo_threshold);

#if defined(HPM_IP_FEATURE_I2S_BUFF_ALIGN_FRAME) && (HPM_IP_FEATURE_I2S_BUFF_ALIGN_FRAME)
    /* make the buffer always frame aligned even in case of buffer underflow or overflow */
    ptr->CTRL |= I2S_CTRL_FRC_ALIGN_FBUF_MASK;
#endif

    return status;
}

static void i2s_config_cfgr(I2S_Type *ptr,
                            uint32_t bclk_div,
                            i2s_transfer_config_t *config)
{
    i2s_gate_bclk(ptr);
    ptr->CFGR = (ptr->CFGR & ~(I2S_CFGR_BCLK_DIV_MASK | I2S_CFGR_TDM_EN_MASK | I2S_CFGR_CH_MAX_MASK | I2S_CFGR_STD_MASK | I2S_CFGR_DATSIZ_MASK | I2S_CFGR_CHSIZ_MASK))
                | I2S_CFGR_BCLK_DIV_SET(bclk_div)
                | I2S_CFGR_TDM_EN_SET(config->enable_tdm_mode)
                | I2S_CFGR_CH_MAX_SET(config->channel_num_per_frame)
                | I2S_CFGR_STD_SET(config->protocol)
                | I2S_CFGR_DATSIZ_SET(I2S_CFGR_DATASIZ(config->audio_depth))
                | I2S_CFGR_CHSIZ_SET(I2S_CFGR_CHSIZ(config->channel_length));
    i2s_ungate_bclk(ptr);
}

static void i2s_config_cfgr_slave(I2S_Type *ptr,
                            i2s_transfer_config_t *config)
{
    ptr->CFGR = (ptr->CFGR & ~(I2S_CFGR_TDM_EN_MASK | I2S_CFGR_CH_MAX_MASK | I2S_CFGR_STD_MASK | I2S_CFGR_DATSIZ_MASK | I2S_CFGR_CHSIZ_MASK))
              | I2S_CFGR_TDM_EN_SET(config->enable_tdm_mode)
              | I2S_CFGR_CH_MAX_SET(config->channel_num_per_frame)
              | I2S_CFGR_STD_SET(config->protocol)
              | I2S_CFGR_DATSIZ_SET(I2S_CFGR_DATASIZ(config->audio_depth))
              | I2S_CFGR_CHSIZ_SET(I2S_CFGR_CHSIZ(config->channel_length));
}

static bool i2s_calculate_bclk_divider(uint32_t mclk_in_hz, uint32_t bclk_in_hz, uint32_t *div_out)
{
    uint32_t bclk_div;
    uint32_t delta1, delta2;

    bclk_div = mclk_in_hz / bclk_in_hz;

    if ((bclk_div > (I2S_CFGR_BCLK_DIV_MASK >> I2S_CFGR_BCLK_DIV_SHIFT))) {
        return false;
    }

    delta1 = mclk_in_hz - bclk_in_hz * bclk_div;
    delta2 = bclk_in_hz * (bclk_div + 1) - mclk_in_hz;
    if (delta2 < delta1) {
        bclk_div++;
        if ((bclk_div > (I2S_CFGR_BCLK_DIV_MASK >> I2S_CFGR_BCLK_DIV_SHIFT))) {
            return false;
        }
    }

    if (MIN(delta1, delta2) && ((MIN(delta1, delta2) * 100 / bclk_in_hz) > HPM_I2S_BCLK_TOLERANCE)) {
        return false;
    }

    *div_out = bclk_div;
    return true;
}

static hpm_stat_t _i2s_config_tx(I2S_Type *ptr, i2s_transfer_config_t *config)
{
    /* channel_num_per_frame has to even. non TDM mode, it has be 2 */
    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);
    if (!i2s_audio_depth_is_valid(config->audio_depth)
        || !i2s_channel_length_is_valid(config->channel_length)
        || !config->sample_rate
        || !channel_num_per_frame
        || (channel_num_per_frame > I2S_SOC_MAX_CHANNEL_NUM)
        || ((!config->enable_tdm_mode) && (channel_num_per_frame > 2))
        || ((config->channel_slot_mask & HPM_I2S_SLOT_MASK) == 0)) {
        return status_invalid_argument;
    }

    ptr->TXDSLOT[config->data_line] = config->channel_slot_mask;

    /* workaround: fill dummy data into TX fifo to avoid TX underflow during tx start */
    if (i2s_fill_tx_dummy_data(ptr, config->data_line, config->channel_num_per_frame) != status_success) {
        return status_invalid_argument;
    }

    ptr->CTRL = (ptr->CTRL & ~(I2S_CTRL_TX_EN_MASK))
        | I2S_CTRL_TX_EN_SET(1 << config->data_line);

    return status_success;
}

static hpm_stat_t _i2s_config_rx(I2S_Type *ptr, i2s_transfer_config_t *config)
{
    /* channel_num_per_frame has to even. non TDM mode, it has be 2 */
    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);
    if (!i2s_audio_depth_is_valid(config->audio_depth)
        || !i2s_channel_length_is_valid(config->channel_length)
        || !config->sample_rate
        || !channel_num_per_frame
        || (channel_num_per_frame > I2S_SOC_MAX_CHANNEL_NUM)
        || ((!config->enable_tdm_mode) && (channel_num_per_frame > 2))
        || ((config->channel_slot_mask & HPM_I2S_SLOT_MASK) == 0)) {
        return status_invalid_argument;
    }

    ptr->RXDSLOT[config->data_line] = config->channel_slot_mask;
    ptr->CTRL = (ptr->CTRL & ~(I2S_CTRL_RX_EN_MASK))
            | I2S_CTRL_RX_EN_SET(1 << config->data_line);

    return status_success;
}

static hpm_stat_t _i2s_config_transfer(I2S_Type *ptr, i2s_transfer_config_t *config)
{
    /* channel_num_per_frame has to even. non TDM mode, it has be 2 */
    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);
    if (!i2s_audio_depth_is_valid(config->audio_depth)
        || !i2s_channel_length_is_valid(config->channel_length)
        || !config->sample_rate
        || !channel_num_per_frame
        || (channel_num_per_frame > I2S_SOC_MAX_CHANNEL_NUM)
        || ((!config->enable_tdm_mode) && (channel_num_per_frame > 2))
        || ((config->channel_slot_mask & HPM_I2S_SLOT_MASK) == 0)) {
        return status_invalid_argument;
    }

    /* Suppose RX and TX use same channel */
    ptr->RXDSLOT[config->data_line] = config->channel_slot_mask;
    ptr->TXDSLOT[config->data_line] = config->channel_slot_mask;

    /* workaround: fill dummy data into TX fifo to avoid TX underflow during tx start */
    if (i2s_fill_tx_dummy_data(ptr, config->data_line, config->channel_num_per_frame) != status_success) {
        return status_invalid_argument;
    }

    ptr->CTRL = (ptr->CTRL & ~(I2S_CTRL_RX_EN_MASK | I2S_CTRL_TX_EN_MASK))
            | I2S_CTRL_RX_EN_SET(1 << config->data_line)
            | I2S_CTRL_TX_EN_SET(1 << config->data_line);

    return status_success;
}

hpm_stat_t i2s_config_tx(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config)
{
    uint32_t bclk_in_hz;
    uint32_t bclk_div;
    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);

    bclk_in_hz = config->sample_rate * config->channel_length * channel_num_per_frame;
    if (!i2s_calculate_bclk_divider(mclk_in_hz, bclk_in_hz, &bclk_div)) {
        return status_invalid_argument;
    }

    i2s_stop(ptr);
    i2s_config_cfgr(ptr, bclk_div, config);

    return _i2s_config_tx(ptr, config);
}

hpm_stat_t i2s_config_tx_slave(I2S_Type *ptr, i2s_transfer_config_t *config)
{
    i2s_stop(ptr);
    i2s_config_cfgr_slave(ptr, config);

    return _i2s_config_tx(ptr, config);
}

hpm_stat_t i2s_config_rx(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config)
{
    uint32_t bclk_in_hz;
    uint32_t bclk_div;

    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);
    bclk_in_hz = config->sample_rate * config->channel_length * channel_num_per_frame;
    if (!i2s_calculate_bclk_divider(mclk_in_hz, bclk_in_hz, &bclk_div)) {
        return status_invalid_argument;
    }

    i2s_stop(ptr);
    i2s_config_cfgr(ptr, bclk_div, config);

    return _i2s_config_rx(ptr, config);
}

hpm_stat_t i2s_config_rx_slave(I2S_Type *ptr, i2s_transfer_config_t *config)
{
    i2s_stop(ptr);
    i2s_config_cfgr_slave(ptr, config);

    return _i2s_config_rx(ptr, config);
}

hpm_stat_t i2s_config_transfer(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config)
{
    uint32_t bclk_in_hz;
    uint32_t bclk_div;

    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);
    bclk_in_hz = config->sample_rate * config->channel_length * channel_num_per_frame;
    if (!i2s_calculate_bclk_divider(mclk_in_hz, bclk_in_hz, &bclk_div)) {
        return status_invalid_argument;
    }

    i2s_stop(ptr);
    i2s_config_cfgr(ptr, bclk_div, config);

    return _i2s_config_transfer(ptr, config);
}

hpm_stat_t i2s_config_transfer_slave(I2S_Type *ptr, i2s_transfer_config_t *config)
{
    i2s_stop(ptr);
    i2s_config_cfgr_slave(ptr, config);

    return _i2s_config_transfer(ptr, config);
}

hpm_stat_t i2s_config_multiline_transfer(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_multiline_transfer_config_t *config)
{
    uint32_t bclk_in_hz;
    uint32_t bclk_div;
    uint8_t channel_num_per_frame = HPM_NUM_TO_EVEN_CEILING(config->channel_num_per_frame);
    uint8_t tx_line_en_mask = 0;
    uint8_t rx_line_en_mask = 0;

    if (!i2s_audio_depth_is_valid(config->audio_depth)
        || !i2s_channel_length_is_valid(config->channel_length)
        || !config->sample_rate
        || !channel_num_per_frame
        || (channel_num_per_frame > I2S_SOC_MAX_CHANNEL_NUM)
        || ((!config->enable_tdm_mode) && (channel_num_per_frame > 2))) {
        return status_invalid_argument;
    }

    bclk_in_hz = config->sample_rate * config->channel_length * channel_num_per_frame;
    if (!i2s_calculate_bclk_divider(mclk_in_hz, bclk_in_hz, &bclk_div)) {
        return status_invalid_argument;
    }

    i2s_stop(ptr);

    if (config->master_mode) {
        i2s_gate_bclk(ptr);
        ptr->CFGR = (ptr->CFGR & ~(I2S_CFGR_BCLK_DIV_MASK | I2S_CFGR_TDM_EN_MASK | I2S_CFGR_CH_MAX_MASK | I2S_CFGR_STD_MASK | I2S_CFGR_DATSIZ_MASK | I2S_CFGR_CHSIZ_MASK))
                    | I2S_CFGR_BCLK_DIV_SET(bclk_div)
                    | I2S_CFGR_TDM_EN_SET(config->enable_tdm_mode)
                    | I2S_CFGR_CH_MAX_SET(config->channel_num_per_frame)
                    | I2S_CFGR_STD_SET(config->protocol)
                    | I2S_CFGR_DATSIZ_SET(I2S_CFGR_DATASIZ(config->audio_depth))
                    | I2S_CFGR_CHSIZ_SET(I2S_CFGR_CHSIZ(config->channel_length));
        i2s_ungate_bclk(ptr);
    } else {
        ptr->CFGR = (ptr->CFGR & ~(I2S_CFGR_BCLK_DIV_MASK | I2S_CFGR_TDM_EN_MASK | I2S_CFGR_CH_MAX_MASK | I2S_CFGR_STD_MASK | I2S_CFGR_DATSIZ_MASK | I2S_CFGR_CHSIZ_MASK))
                    | I2S_CFGR_TDM_EN_SET(config->enable_tdm_mode)
                    | I2S_CFGR_CH_MAX_SET(config->channel_num_per_frame)
                    | I2S_CFGR_STD_SET(config->protocol)
                    | I2S_CFGR_DATSIZ_SET(I2S_CFGR_DATASIZ(config->audio_depth))
                    | I2S_CFGR_CHSIZ_SET(I2S_CFGR_CHSIZ(config->channel_length));
    }

    for (uint8_t i = 0; i < 4; i++) {
        ptr->RXDSLOT[i] = config->rx_channel_slot_mask[i];
        ptr->TXDSLOT[i] = config->tx_channel_slot_mask[i];
        if (config->rx_data_line_en[i]) {
            rx_line_en_mask |= 1 << i;
        }
        if (config->tx_data_line_en[i]) {
            tx_line_en_mask |= 1 << i;
        }
    }

    /* workaround: fill dummy data into TX fifo to avoid TX underflow during tx start */
    for (uint8_t i = 0; i < 4; i++) {
        if (config->tx_data_line_en[i]) {
            if (i2s_fill_tx_dummy_data(ptr, i, config->channel_num_per_frame) != status_success) {
                return status_invalid_argument;
            }
        }
    }

    ptr->CTRL = (ptr->CTRL & ~(I2S_CTRL_RX_EN_MASK | I2S_CTRL_TX_EN_MASK))
               | I2S_CTRL_RX_EN_SET(rx_line_en_mask)
               | I2S_CTRL_TX_EN_SET(tx_line_en_mask);

    return status_success;
}

uint32_t i2s_send_buff(I2S_Type *ptr, i2s_line_num_t tx_line_index, uint8_t samplebits, uint8_t *src, uint32_t size)
{
    uint32_t data;
    uint32_t retry = 0;
    uint8_t bytes = samplebits / 8U;
    uint32_t left;

    if (!i2s_audio_depth_is_valid(samplebits)) {
        return 0;
    }

    if ((size % bytes) != 0) {
        return 0;
    }

    left = size;
    while (left) {
        /* check fifo status */
        if (i2s_get_tx_line_fifo_level(ptr, tx_line_index) < I2S_FIFO_THRESH_TX_GET(ptr->FIFO_THRESH)) {
            /* Move valid data to high position */
            data = *((uint32_t *)(src)) << (32 - samplebits);
            ptr->TXD[tx_line_index] = data;
            src += bytes;
            left -= bytes;
            retry = 0;
        } else {
            if (retry > HPM_I2S_DRV_DEFAULT_RETRY_COUNT) {
                break;
            }
            retry++;
        }
    }

    return size - left;
}

uint32_t i2s_receive_buff(I2S_Type *ptr, i2s_line_num_t rx_line_index, uint8_t samplebits, uint8_t *dst, uint32_t size)
{
    uint32_t data;
    uint32_t left;
    uint32_t retry = 0;
    uint8_t bytes = samplebits / 8U;

    if (!i2s_audio_depth_is_valid(samplebits)) {
        return 0;
    }

    if ((size % bytes) != 0) {
        return 0;
    }

    left = size;
    while (left) {
        /* check fifo status */
        if (i2s_get_rx_line_fifo_level(ptr, rx_line_index) < I2S_FIFO_THRESH_RX_GET(ptr->FIFO_THRESH)) {
            /* valid data on high position */
            data = ptr->RXD[rx_line_index] >> (32 - samplebits);
            for (uint8_t n = 0; n < bytes; n++) {
                *dst = (uint8_t)(data >> (8U * n)) & 0xFFU;
                dst++;
                left--;
                retry = 0;
            }
        } else {
            if (retry > HPM_I2S_DRV_DEFAULT_RETRY_COUNT) {
                break;
            }
            retry++;
        }
    }

    return size - left;
}

#if defined(HPMSOC_HAS_HPMSDK_PDM) || defined(HPMSOC_HAS_HPMSDK_PDMLITE)
void i2s_get_default_transfer_config_for_pdm(i2s_transfer_config_t *transfer)
{
    transfer->sample_rate = PDM_SOC_SAMPLE_RATE_IN_HZ;
    transfer->channel_num_per_frame = 8;
    transfer->channel_length = i2s_channel_length_32_bits;
    transfer->audio_depth = i2s_audio_depth_32_bits;
    transfer->enable_tdm_mode = true;
    transfer->protocol = I2S_PROTOCOL_MSB_JUSTIFIED;
    transfer->master_mode = true;
    transfer->data_line = I2S_DATA_LINE_0;
    transfer->channel_slot_mask = 0x11;
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_DAO)
void i2s_get_default_transfer_config_for_dao(i2s_transfer_config_t *transfer)
{
    transfer->sample_rate = DAO_SOC_SAMPLE_RATE_IN_HZ;
    transfer->channel_num_per_frame = 2;
    transfer->channel_length = i2s_channel_length_32_bits;
    transfer->audio_depth = i2s_audio_depth_32_bits;
    transfer->enable_tdm_mode = false;
    transfer->protocol = I2S_PROTOCOL_MSB_JUSTIFIED;
    transfer->master_mode = true;
    transfer->data_line = I2S_DATA_LINE_0;
    transfer->channel_slot_mask = 0x3;
}
#endif

void i2s_get_default_transfer_config(i2s_transfer_config_t *transfer)
{
    transfer->sample_rate = 48000U;
    transfer->channel_num_per_frame = 2;
    transfer->channel_length = i2s_channel_length_32_bits;
    transfer->audio_depth = i2s_audio_depth_32_bits;
    transfer->enable_tdm_mode = false;
    transfer->master_mode = true;
    transfer->protocol = I2S_PROTOCOL_MSB_JUSTIFIED;
    transfer->data_line = I2S_DATA_LINE_0;
    transfer->channel_slot_mask = 0x3;
}

void i2s_get_default_multiline_transfer_config(i2s_multiline_transfer_config_t *transfer)
{
    transfer->sample_rate = 48000U;
    transfer->channel_num_per_frame = 2;
    transfer->channel_length = i2s_channel_length_32_bits;
    transfer->audio_depth = i2s_audio_depth_32_bits;
    transfer->enable_tdm_mode = false;
    transfer->master_mode = true;
    transfer->protocol = I2S_PROTOCOL_MSB_JUSTIFIED;

    transfer->rx_data_line_en[0] = false;
    transfer->rx_data_line_en[1] = false;
    transfer->rx_data_line_en[2] = false;
    transfer->rx_data_line_en[3] = false;
    transfer->rx_channel_slot_mask[0] = 0x03;
    transfer->rx_channel_slot_mask[1] = 0x03;
    transfer->rx_channel_slot_mask[2] = 0x03;
    transfer->rx_channel_slot_mask[3] = 0x03;
    transfer->tx_data_line_en[0] = false;
    transfer->tx_data_line_en[1] = false;
    transfer->tx_data_line_en[2] = false;
    transfer->tx_data_line_en[3] = false;
    transfer->tx_channel_slot_mask[0] = 0x03;
    transfer->tx_channel_slot_mask[1] = 0x03;
    transfer->tx_channel_slot_mask[2] = 0x03;
    transfer->tx_channel_slot_mask[3] = 0x03;
}
