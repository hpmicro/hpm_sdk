/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_adc_v2_port.h"

static bool adc_v2_port_is_valid_adc_base(uint32_t base)
{
#if defined(HPM_ADC0_BASE)
    if (base == HPM_ADC0_BASE) {
        return true;
    }
#endif
#if defined(HPM_ADC1_BASE)
    if (base == HPM_ADC1_BASE) {
        return true;
    }
#endif
#if defined(HPM_ADC2_BASE)
    if (base == HPM_ADC2_BASE) {
        return true;
    }
#endif
#if defined(HPM_ADC3_BASE)
    if (base == HPM_ADC3_BASE) {
        return true;
    }
#endif
    return false;
}

adc_v2_ip_t adc_v2_port_detect_ip(uint32_t base)
{
    if (!adc_v2_port_is_valid_adc_base(base)) {
        return adc_v2_ip_unknown;
    }

#if HPM_ADC_V2_HAS_DUAL_IP
#if defined(HPM_ADC3_BASE)
    if (base == HPM_ADC3_BASE) {
        return adc_v2_ip_adc16;
    }
#endif
    return adc_v2_ip_adc12;
#elif HPM_ADC_V2_HAS_ADC16
    return adc_v2_ip_adc16;
#elif HPM_ADC_V2_HAS_ADC12
    return adc_v2_ip_adc12;
#else
    (void) base;
    return adc_v2_ip_unknown;
#endif
}

hpm_stat_t adc_v2_port_resolution_to_hw(adc_v2_ip_t ip, uint8_t resolution_bits, uint8_t *hw_res)
{
    if (hw_res == NULL) {
        return status_invalid_argument;
    }

    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        switch (resolution_bits) {
        case adc_v2_resolution_6bit:
            *hw_res = (uint8_t) adc12_res_6_bits;
            return status_success;
        case adc_v2_resolution_8bit:
            *hw_res = (uint8_t) adc12_res_8_bits;
            return status_success;
        case adc_v2_resolution_10bit:
            *hw_res = (uint8_t) adc12_res_10_bits;
            return status_success;
        case adc_v2_resolution_12bit:
            *hw_res = (uint8_t) adc12_res_12_bits;
            return status_success;
        default:
            return status_invalid_argument;
        }
#else
        (void) resolution_bits;
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        switch (resolution_bits) {
        case adc_v2_resolution_8bit:
            *hw_res = (uint8_t) adc16_res_8_bits;
            return status_success;
        case adc_v2_resolution_10bit:
            *hw_res = (uint8_t) adc16_res_10_bits;
            return status_success;
        case adc_v2_resolution_12bit:
            *hw_res = (uint8_t) adc16_res_12_bits;
            return status_success;
        case adc_v2_resolution_16bit:
            *hw_res = (uint8_t) adc16_res_16_bits;
            return status_success;
        default:
            return status_invalid_argument;
        }
#else
        (void) resolution_bits;
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t adc_v2_port_resolution_from_hw(adc_v2_ip_t ip, uint8_t hw_res, uint8_t *resolution_bits)
{
    if (resolution_bits == NULL) {
        return status_invalid_argument;
    }

    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        switch ((adc12_resolution_t) hw_res) {
        case adc12_res_6_bits:
            *resolution_bits = adc_v2_resolution_6bit;
            return status_success;
        case adc12_res_8_bits:
            *resolution_bits = adc_v2_resolution_8bit;
            return status_success;
        case adc12_res_10_bits:
            *resolution_bits = adc_v2_resolution_10bit;
            return status_success;
        case adc12_res_12_bits:
            *resolution_bits = adc_v2_resolution_12bit;
            return status_success;
        default:
            return status_invalid_argument;
        }
#else
        (void) hw_res;
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        switch ((adc16_resolution_t) hw_res) {
        case adc16_res_8_bits:
            *resolution_bits = adc_v2_resolution_8bit;
            return status_success;
        case adc16_res_10_bits:
            *resolution_bits = adc_v2_resolution_10bit;
            return status_success;
        case adc16_res_12_bits:
            *resolution_bits = adc_v2_resolution_12bit;
            return status_success;
        case adc16_res_16_bits:
            *resolution_bits = adc_v2_resolution_16bit;
            return status_success;
        default:
            return status_invalid_argument;
        }
#else
        (void) hw_res;
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

static uint32_t adc_v2_port_event_supported_mask(adc_v2_ip_t ip)
{
    uint32_t mask;

    mask = 0U;
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        mask = (uint32_t) adc12_event_trig_complete
             | (uint32_t) adc12_event_trig_sw_conflict
             | (uint32_t) adc12_event_trig_hw_conflict
             | (uint32_t) adc12_event_read_conflict
             | (uint32_t) adc12_event_seq_sw_conflict
             | (uint32_t) adc12_event_seq_hw_conflict
             | (uint32_t) adc12_event_seq_dma_abort
             | (uint32_t) adc12_event_seq_full_complete
             | (uint32_t) adc12_event_seq_single_complete
             | (uint32_t) adc12_event_dma_fifo_full;
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        mask = (uint32_t) adc16_event_trig_complete
             | (uint32_t) adc16_event_trig_sw_conflict
             | (uint32_t) adc16_event_trig_hw_conflict
             | (uint32_t) adc16_event_read_conflict
             | (uint32_t) adc16_event_seq_sw_conflict
             | (uint32_t) adc16_event_seq_hw_conflict
             | (uint32_t) adc16_event_seq_dma_abort
             | (uint32_t) adc16_event_seq_full_complete
             | (uint32_t) adc16_event_seq_single_complete
             | (uint32_t) adc16_event_dma_fifo_full;
#if HPM_ADC_V2_HAS_SEQ_STOP_POS_INT
        mask |= (uint32_t) adc16_event_seq_stop_pos;
#endif
#endif
    }

    return mask;
}

uint32_t adc_v2_port_event_to_hw(adc_v2_ip_t ip, uint32_t event_mask)
{
    return event_mask & adc_v2_port_event_supported_mask(ip);
}

uint32_t adc_v2_port_event_from_hw(adc_v2_ip_t ip, uint32_t hw_mask)
{
    return hw_mask & adc_v2_port_event_supported_mask(ip);
}

#if HPM_ADC_V2_HAS_ADC12
static void adc_v2_port_fill_from_adc12_pmt(adc_v2_dma_sample_t *local, uint32_t raw)
{
    union {
        uint32_t u32;
        adc12_pmt_dma_data_t dma;
    } word;

    word.u32 = raw;
    local->result = (uint16_t) word.dma.result;
    local->seq_num = (uint8_t) word.dma.seq_num;
    local->adc_ch = (uint8_t) word.dma.adc_ch;
    local->trig_ch = (uint8_t) word.dma.trig_ch;
    local->cycle_bit = (word.dma.cycle_bit != 0U);
}

static void adc_v2_port_fill_from_adc12_seq(adc_v2_dma_sample_t *local, uint32_t raw)
{
    union {
        uint32_t u32;
        adc12_seq_dma_data_t dma;
    } word;

    word.u32 = raw;
    local->result = (uint16_t) word.dma.result;
    local->seq_num = (uint8_t) word.dma.seq_num;
    local->adc_ch = (uint8_t) word.dma.adc_ch;
    local->cycle_bit = (word.dma.cycle_bit != 0U);
}
#endif

#if HPM_ADC_V2_HAS_ADC16
static void adc_v2_port_fill_from_adc16_pmt(adc_v2_dma_sample_t *local, uint32_t raw)
{
    union {
        uint32_t u32;
        adc16_pmt_dma_data_t dma;
    } word;

    word.u32 = raw;
    local->result = (uint16_t) word.dma.result;
    local->seq_num = (uint8_t) word.dma.seq_num;
    local->adc_ch = (uint8_t) word.dma.adc_ch;
    local->trig_ch = (uint8_t) word.dma.trig_ch;
    local->cycle_bit = (word.dma.cycle_bit != 0U);
}

static void adc_v2_port_fill_from_adc16_seq(adc_v2_dma_sample_t *local, uint32_t raw)
{
    union {
        uint32_t u32;
        adc16_seq_dma_data_t dma;
    } word;

    word.u32 = raw;
    local->result = (uint16_t) word.dma.result;
    local->seq_num = (uint8_t) word.dma.seq_num;
    local->adc_ch = (uint8_t) word.dma.adc_ch;
    local->cycle_bit = (word.dma.cycle_bit != 0U);
}
#endif

void adc_v2_port_parse_pmt_dma_word(adc_v2_ip_t ip, uint32_t raw, adc_v2_dma_sample_t *sample)
{
    adc_v2_dma_sample_t local;

    if (sample == NULL) {
        return;
    }

    local.result = 0U;
    local.seq_num = 0U;
    local.adc_ch = 0U;
    local.trig_ch = 0U;
    local.cycle_bit = false;
    local.is_raw_16bit = false;

    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc_v2_port_fill_from_adc12_pmt(&local, raw);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc_v2_port_fill_from_adc16_pmt(&local, raw);
#endif
    }

    *sample = local;
}

void adc_v2_port_parse_seq_dma_word(adc_v2_ip_t ip, uint32_t raw, bool seq_raw_16bit, adc_v2_dma_sample_t *sample)
{
    adc_v2_dma_sample_t local;

    if (sample == NULL) {
        return;
    }

    local.result = 0U;
    local.seq_num = 0U;
    local.adc_ch = 0U;
    local.trig_ch = 0U;
    local.cycle_bit = false;
    local.is_raw_16bit = false;

    if (seq_raw_16bit && (ip == adc_v2_ip_adc16)) {
#if HPM_ADC_V2_HAS_ADC16
        local.result = (uint16_t)(raw & 0xffffU);
        local.is_raw_16bit = true;
#endif
    } else if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc_v2_port_fill_from_adc12_seq(&local, raw);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc_v2_port_fill_from_adc16_seq(&local, raw);
#endif
    }

    *sample = local;
}
