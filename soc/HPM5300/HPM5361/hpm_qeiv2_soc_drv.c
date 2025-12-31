/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "hpm_qeiv2_soc_drv.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
#define OPAMP0_ADC_CHN_BASE (16U)
#define OPAMP1_ADC_CHN_BASE (24U)

/***********************************************************************************************************************
 * Codes
 **********************************************************************************************************************/
uint8_t qeiv2_adc_channel_convert(qeiv2_adc_channel_t adc_chn, uint8_t opamp_chn)
{
    uint8_t qei_adc_chn;

    if (adc_chn == qeiv2_adc_chn_opamp0) {
        qei_adc_chn = OPAMP0_ADC_CHN_BASE + opamp_chn;
    } else if (adc_chn == qeiv2_adc_chn_opamp1) {
        qei_adc_chn = OPAMP1_ADC_CHN_BASE + opamp_chn;
    } else {
        qei_adc_chn = adc_chn;
    }

    return qei_adc_chn;
}

