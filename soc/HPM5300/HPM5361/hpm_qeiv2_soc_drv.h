/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_QEIV2_SOC_DRV_H
#define HPM_QEIV2_SOC_DRV_H

/**
 * @addtogroup qeiv2_interface QEIV2 driver APIs
 * @{
 */

/* @brief qeiv2 adc channel */
typedef enum {
    qeiv2_adc_chn_0 = 0,
    qeiv2_adc_chn_1,
    qeiv2_adc_chn_2,
    qeiv2_adc_chn_3,
    qeiv2_adc_chn_4,
    qeiv2_adc_chn_5,
    qeiv2_adc_chn_6,
    qeiv2_adc_chn_7,
    qeiv2_adc_chn_8,
    qeiv2_adc_chn_9,
    qeiv2_adc_chn_10,
    qeiv2_adc_chn_11,
    qeiv2_adc_chn_12,
    qeiv2_adc_chn_13,
    qeiv2_adc_chn_opamp1,
    qeiv2_adc_chn_opamp0,
} qeiv2_adc_channel_t;

/**
 * @brief Converts the given ADC channel to the corresponding QEI ADC channel.
 *
 * This function takes an ADC channel and an op-amp channel number as input,
 * and computes and returns the corresponding QEI ADC channel number.
 * If the input ADC channel is not an op-amp channel, it directly returns the input ADC channel number.
 *
 * @param adc_chn The ADC channel enum, which can be either an op-amp channel or a regular adc channel.
 * @param opamp_chn The op-amp channel number. when not used OPAMP, please set to 0.
 * @return uint8_t The converted QEI ADC channel number.
 */

uint8_t qeiv2_adc_channel_convert(qeiv2_adc_channel_t adc_chn, uint8_t opamp_chn);

/**
 * @}
 */

#endif /* HPM_QEIV2_SOC_DRV_H */

