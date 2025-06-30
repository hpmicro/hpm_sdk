/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_gptmr_drv.h"

void gptmr_channel_get_default_config(GPTMR_Type *ptr, gptmr_channel_config_t *config)
{
    (void) ptr;
    config->mode = gptmr_work_mode_no_capture;
    config->dma_request_event = gptmr_dma_request_disabled;
    config->synci_edge = gptmr_synci_edge_none;
    for (uint8_t i = 0; i < GPTMR_CH_CMP_COUNT; i++) {
        config->cmp[i] = 0xFFFFFFFEUL;
    }
    config->reload = 0xFFFFFFFEUL;
    config->cmp_initial_polarity_high = true;
    config->enable_cmp_output = true;
    config->enable_sync_follow_previous_channel = false;
    config->enable_software_sync = false;
    config->debug_mode = true;

#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE  == 1)
    config->counter_mode = gptmr_counter_mode_internal;
#endif

#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE  == 1)
    config->enable_opmode = false;
#endif

#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
    config->enable_monitor = false;
    gptmr_channel_get_default_monitor_config(ptr, &config->monitor_config);
#endif
}

hpm_stat_t gptmr_channel_config(GPTMR_Type *ptr,
                         uint8_t ch_index,
                         gptmr_channel_config_t *config,
                         bool enable)
{
    uint32_t v = 0;
    uint32_t tmp_value;

    if (config->enable_sync_follow_previous_channel && !ch_index) {
        return status_invalid_argument;
    }

    if (config->dma_request_event != gptmr_dma_request_disabled) {
        v |= GPTMR_CHANNEL_CR_DMAEN_MASK
            | GPTMR_CHANNEL_CR_DMASEL_SET(config->dma_request_event);
    }
    v |= GPTMR_CHANNEL_CR_CAPMODE_SET(config->mode)
        | GPTMR_CHANNEL_CR_DBGPAUSE_SET(config->debug_mode)
        | GPTMR_CHANNEL_CR_SWSYNCIEN_SET(config->enable_software_sync)
        | GPTMR_CHANNEL_CR_CMPINIT_SET(config->cmp_initial_polarity_high)
        | GPTMR_CHANNEL_CR_SYNCFLW_SET(config->enable_sync_follow_previous_channel)
        | GPTMR_CHANNEL_CR_CMPEN_SET(config->enable_cmp_output)
        | GPTMR_CHANNEL_CR_CEN_SET(enable)
        | config->synci_edge;
#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE  == 1)
    v |= GPTMR_CHANNEL_CR_CNT_MODE_SET(config->counter_mode);
#endif
#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE  == 1)
    v |= GPTMR_CHANNEL_CR_OPMODE_SET(config->enable_opmode);
#endif
    for (uint8_t i = GPTMR_CH_CMP_COUNT; i > 0; i--) {
        tmp_value = config->cmp[i - 1];
        if ((tmp_value > 0)  && (tmp_value != 0xFFFFFFFFu)) {
            tmp_value--;
        }
        ptr->CHANNEL[ch_index].CMP[i - 1] = GPTMR_CHANNEL_CMP_CMP_SET(tmp_value);
    }
    tmp_value = config->reload;
    if ((tmp_value > 0) && (tmp_value != 0xFFFFFFFFu)) {
        tmp_value--;
    }
    ptr->CHANNEL[ch_index].RLD = GPTMR_CHANNEL_RLD_RLD_SET(tmp_value);
    ptr->CHANNEL[ch_index].CR = v;
#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
    gptmr_channel_monitor_config(ptr, ch_index, &config->monitor_config, config->enable_monitor);
#endif

    return status_success;
}

#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
void gptmr_channel_get_default_monitor_config(GPTMR_Type *ptr, gptmr_channel_monitor_config_t *config)
{
    (void) ptr;
    config->max_value = 0;
    config->min_value = 0;
    config->monitor_type = monitor_signal_high_level_time;
}

hpm_stat_t gptmr_channel_monitor_config(GPTMR_Type *ptr, uint8_t ch_index, gptmr_channel_monitor_config_t *config, bool enable)
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
#endif

#if defined(HPM_IP_FEATURE_GPTMR_QEI_MODE) && (HPM_IP_FEATURE_GPTMR_QEI_MODE == 1)
void gptmr_config_qei(GPTMR_Type *ptr, gptmr_qei_config_t *qei_config)
{
    gptmr_channel_config_t config;

    gptmr_set_qei_type(ptr, qei_config->ch_group, qei_config->type);

    gptmr_stop_counter(ptr, qei_config->ch_group);
    gptmr_stop_counter(ptr, qei_config->ch_group + 1);

    gptmr_channel_get_default_config(ptr, &config);
    config.counter_mode = gptmr_counter_mode_external;
    config.reload = qei_config->phmax;
    gptmr_channel_reset_count(ptr, qei_config->ch_group);
    gptmr_channel_config(ptr, qei_config->ch_group, &config, false);
    gptmr_channel_reset_count(ptr, qei_config->ch_group + 1);
    gptmr_channel_config(ptr, qei_config->ch_group + 1, &config, false);

    gptmr_start_counter(ptr, qei_config->ch_group);
    gptmr_start_counter(ptr, qei_config->ch_group + 1);
}

void gptmr_set_qei_type(GPTMR_Type *ptr, gptmr_qei_ch_group_t ch_group, gptmr_qei_type_t type)
{
    if (ch_group == gptmr_qei_ch_group_23) {
        ptr->GCR = (ptr->GCR & ~GPTMR_GCR_QEI_TYPE23_MASK) | GPTMR_GCR_QEI_TYPE23_SET(type);
    } else {
        ptr->GCR = (ptr->GCR & ~GPTMR_GCR_QEI_TYPE01_MASK) | GPTMR_GCR_QEI_TYPE01_SET(type);
    }
}

gptmr_qei_type_t gptmr_get_qei_type(GPTMR_Type *ptr, gptmr_qei_ch_group_t ch_group)
{
    gptmr_qei_type_t type;

    if (ch_group == gptmr_qei_ch_group_23) {
        type = (gptmr_qei_type_t)GPTMR_GCR_QEI_TYPE23_GET(ptr->GCR);
    } else {
        type = (gptmr_qei_type_t)GPTMR_GCR_QEI_TYPE01_GET(ptr->GCR);
    }

    return type;
}

uint32_t gptmr_get_qei_phcnt(GPTMR_Type *ptr, gptmr_qei_ch_group_t ch_group)
{
    uint32_t count0, count1, phmax, qei_count;

    count0 = gptmr_channel_get_counter(ptr, ch_group, gptmr_counter_type_normal);
    count1 = gptmr_channel_get_counter(ptr, ch_group + 1, gptmr_counter_type_normal);
    phmax = gptmr_channel_get_reload(ptr, ch_group) + 1;
    if (gptmr_get_qei_type(ptr, ch_group) == gptmr_qei_ab_mode) {
        if (count1 >= count0) {
            qei_count = count1 - count0;
        } else {
            qei_count = phmax + (count1 - count0);
        }
    } else {
        if (count0 >= count1) {
            qei_count = count0 - count1;
        } else {
            qei_count = phmax + (count0 - count1);
        }
    }

    return qei_count;
}
#endif

#if defined(HPM_IP_FEATURE_GPTMR_BURST_MODE) && (HPM_IP_FEATURE_GPTMR_BURST_MODE == 1)

hpm_stat_t gptmr_channel_burst_mode_start_counter(GPTMR_Type *ptr, uint8_t ch_index, gptmr_burst_counter_mode_t mode)
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
#endif
