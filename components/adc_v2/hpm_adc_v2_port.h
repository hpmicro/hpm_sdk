/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_ADC_V2_PORT_H
#define HPM_ADC_V2_PORT_H

#include "hpm_adc_v2.h"

#ifdef HPMSOC_HAS_HPMSDK_ADC12
#include "hpm_adc12_drv.h"
#endif
#ifdef HPMSOC_HAS_HPMSDK_ADC16
#include "hpm_adc16_drv.h"
#endif
#include "hpm_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

adc_v2_ip_t adc_v2_port_detect_ip(uint32_t base);
hpm_stat_t adc_v2_port_resolution_to_hw(adc_v2_ip_t ip, uint8_t resolution_bits, uint8_t *hw_res);
hpm_stat_t adc_v2_port_resolution_from_hw(adc_v2_ip_t ip, uint8_t hw_res, uint8_t *resolution_bits);
uint32_t adc_v2_port_event_to_hw(adc_v2_ip_t ip, uint32_t event_mask);
uint32_t adc_v2_port_event_from_hw(adc_v2_ip_t ip, uint32_t hw_mask);
void adc_v2_port_parse_pmt_dma_word(adc_v2_ip_t ip, uint32_t raw, adc_v2_dma_sample_t *sample);
void adc_v2_port_parse_seq_dma_word(adc_v2_ip_t ip, uint32_t raw, bool seq_raw_16bit, adc_v2_dma_sample_t *sample);

#ifdef __cplusplus
}
#endif

#endif
