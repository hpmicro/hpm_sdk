/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_gptmrv2_drv.h"

void gptmr_channel_get_default_config(GPTMRV2_Type *ptr, gptmr_channel_config_t *config)
{
    (void) ptr;
    config->mode = gptmr_work_mode_no_capture;
    config->dma_request_event = gptmr_dma_request_disabled;
    config->synci_edge = gptmr_synci_edge_none;
    for (uint8_t i = 0; i < GPTMR_CH_CMP_COUNT; i++) {
        config->cmp[i] = 0xFFFFFEUL;
    }
    config->reload = 0xFFFFFEUL;
    config->cmp_initial_polarity_high = true;
    config->enable_cmp_output = true;
    config->enable_sync_follow_previous_channel = false;
    config->enable_software_sync = false;
    config->debug_mode = true;

    config->counter_mode = gptmr_counter_mode_internal;
    config->enable_opmode = false;
    config->enable_monitor = false;
    config->enable_shadow = false;
    config->prescaler = 1;
    gptmr_channel_get_default_monitor_config(ptr, &config->monitor_config);
}

hpm_stat_t gptmr_channel_config(GPTMRV2_Type *ptr,
                         uint8_t ch_index,
                         gptmr_channel_config_t *config,
                         bool enable)
{
    uint32_t v = 0;
    uint32_t tmp_value;

    if (config->enable_sync_follow_previous_channel && !ch_index) {
        return status_invalid_argument;
    }
    if (config->prescaler <= 0) {
        return status_invalid_argument;
    }
    for (uint8_t i = 0; i < GPTMR_CH_CMP_COUNT; i++) {
        if (config->cmp[i] > 0xFFFFFFUL) {
            return status_gptmr_cmp_overflow;
        }
    }
    if (config->reload > 0xFFFFFFUL) {
        return status_gptmr_reload_overflow;
    }
    if (config->dma_request_event != gptmr_dma_request_disabled) {
        v |= GPTMRV2_CHANNEL_CR_DMAEN_MASK
            | GPTMRV2_CHANNEL_CR_DMASEL_SET(config->dma_request_event);
    }
    /* Prescaler: 0 = no division (timer clock), 1 = divide-by-2, N = divide-by-(N+1) */
    ptr->CHANNEL[ch_index].BURST_CFG = GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_SET(config->prescaler - 1);
    v |= GPTMRV2_CHANNEL_CR_CAPMODE_SET(config->mode)
        | GPTMRV2_CHANNEL_CR_DBGPAUSE_SET(config->debug_mode)
        | GPTMRV2_CHANNEL_CR_SWSYNCIEN_SET(config->enable_software_sync)
        | GPTMRV2_CHANNEL_CR_CMPINIT_SET(config->cmp_initial_polarity_high)
        | GPTMRV2_CHANNEL_CR_SYNCFLW_SET(config->enable_sync_follow_previous_channel)
        | config->synci_edge;
    v |= GPTMRV2_CHANNEL_CR_CNT_MODE_SET(config->counter_mode);
    v |= GPTMRV2_CHANNEL_CR_OPMODE_SET(config->enable_opmode);
    v |= GPTMRV2_CHANNEL_CR_SHADOW_EN_SET(config->enable_shadow);
    for (uint8_t i = GPTMR_CH_CMP_COUNT; i > 0; i--) {
        tmp_value = config->cmp[i - 1];
        if ((tmp_value > 0)  && (tmp_value != 0xFFFFFFFFu)) {
            tmp_value--;
        }
        ptr->CHANNEL[ch_index].CMP[i - 1] = GPTMRV2_CHANNEL_CMP_CMP_SET(tmp_value);
    }
    tmp_value = config->reload;
    if ((tmp_value > 0) && (tmp_value != 0xFFFFFFFFu)) {
        tmp_value--;
    }
    ptr->CHANNEL[ch_index].RLD = GPTMRV2_CHANNEL_RLD_RLD_SET(tmp_value);
    ptr->CHANNEL[ch_index].CR = v;
    /* the initial polarity must be configured before enabling the output compare function */
    if (config->enable_cmp_output == true) {
        v = ptr->CHANNEL[ch_index].CR | GPTMRV2_CHANNEL_CR_CMPEN_MASK | GPTMRV2_CHANNEL_CR_CEN_SET(enable);
    } else {
        v = (ptr->CHANNEL[ch_index].CR & ~GPTMRV2_CHANNEL_CR_CMPEN_MASK) | GPTMRV2_CHANNEL_CR_CEN_SET(enable);
    }
    ptr->CHANNEL[ch_index].CR = v;
    gptmr_channel_monitor_config(ptr, ch_index, &config->monitor_config, config->enable_monitor);

    return status_success;
}

void gptmr_channel_get_default_monitor_config(GPTMRV2_Type *ptr, gptmr_channel_monitor_config_t *config)
{
    (void) ptr;
    config->max_value = 0;
    config->min_value = 0;
    config->monitor_type = monitor_signal_high_level_time;
}

hpm_stat_t gptmr_channel_monitor_config(GPTMRV2_Type *ptr, uint8_t ch_index, gptmr_channel_monitor_config_t *config, bool enable)
{
    if ((ptr == NULL) || (config->max_value < config->min_value)) {
        return status_invalid_argument;
    }
    if (enable == true) {
        gptmr_channel_set_monitor_type(ptr, ch_index, config->monitor_type);
        gptmr_update_cmp(ptr, ch_index, 0, config->min_value);
        gptmr_update_cmp(ptr, ch_index, 1, config->max_value);
        gptmr_channel_config_update_reload(ptr, ch_index, 0xFFFFFFFF);
        gptmr_channel_set_capmode(ptr, ch_index, gptmr_work_mode_measure_width);
        gptmr_channel_reset_count(ptr, ch_index);
        gptmr_channel_enable_monitor(ptr, ch_index);
    } else {
        gptmr_channel_disable_monitor(ptr, ch_index);
    }
    return status_success;
}

hpm_stat_t gptmr_channel_burst_mode_start_counter(GPTMRV2_Type *ptr, uint8_t ch_index, gptmr_burst_counter_mode_t mode)
{
    uint32_t target_burst_count, current_burst_count;
    hpm_stat_t stat = status_success;
    if (gptmr_channel_is_burst_mode(ptr, ch_index) == false) {
        return status_invalid_argument;
    }
    switch (mode) {
    case gptmr_burst_counter_restart:
        /* enables next count: need disable counter first, then enable counter */
        gptmr_stop_counter(ptr, ch_index);
        /* reset counter to restart burst */
        gptmr_channel_reset_count(ptr, ch_index);
        gptmr_start_counter(ptr, ch_index);
        break;

    case gptmr_burst_counter_continue:
        target_burst_count = gptmr_channel_get_target_burst_count(ptr, ch_index);
        current_burst_count = gptmr_channel_get_current_burst_count(ptr, ch_index);
        /* if current count reaches the configured limit, stop counter to reset state */
        if (current_burst_count >= target_burst_count) {
            gptmr_stop_counter(ptr, ch_index);
            /* reset counter to restart burst */
            gptmr_channel_reset_count(ptr, ch_index);
        }
        gptmr_start_counter(ptr, ch_index);
        break;

    default:
        stat = status_invalid_argument;
        break;
    }
    return stat;
}
