/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "hpm_qeiv2_soc_drv.h"


/***********************************************************************************************************************
 * Codes
 **********************************************************************************************************************/
uint8_t qeiv2_adc_channel_convert(qeiv2_adc_channel_t adc_chn, uint8_t opamp_chn)
{
    (void) opamp_chn;

    return adc_chn;
}

