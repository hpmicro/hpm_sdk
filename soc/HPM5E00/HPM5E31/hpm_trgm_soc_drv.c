/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_trgm_soc_drv.h"

void trgm_adc_matrix_config(TRGM_Type *ptr, trgm_adc_matrix_out_t consumer, trgm_adc_matrix_in_t src, bool inv_val)
{
    uint8_t sel;

    sel = (uint8_t)src;
    if (inv_val) {
        sel |= BIT7_MASK;
    }

    switch (consumer) {
    case trgm_adc_matrix_output_to_qei0_adc0:
        ptr->ADC_MATRIX_SEL0 = (ptr->ADC_MATRIX_SEL0 & ~TRGM_ADC_MATRIX_SEL0_QEI0_ADC0_SEL_MASK)
                             | TRGM_ADC_MATRIX_SEL0_QEI0_ADC0_SEL_SET(sel);
        break;
    case trgm_adc_matrix_output_to_qei0_adc1:
        ptr->ADC_MATRIX_SEL0 = (ptr->ADC_MATRIX_SEL0 & ~TRGM_ADC_MATRIX_SEL0_QEI0_ADC1_SEL_MASK)
                             | TRGM_ADC_MATRIX_SEL0_QEI0_ADC1_SEL_SET(sel);
        break;
    case trgm_adc_matrix_output_to_qei1_adc0:
        ptr->ADC_MATRIX_SEL0 = (ptr->ADC_MATRIX_SEL0 & ~TRGM_ADC_MATRIX_SEL0_QEI1_ADC0_SEL_MASK)
                             | TRGM_ADC_MATRIX_SEL0_QEI1_ADC0_SEL_SET(sel);
        break;
    case trgm_adc_matrix_output_to_qei1_adc1:
        ptr->ADC_MATRIX_SEL0 = (ptr->ADC_MATRIX_SEL0 & ~TRGM_ADC_MATRIX_SEL0_QEI1_ADC1_SEL_MASK)
                             | TRGM_ADC_MATRIX_SEL0_QEI1_ADC1_SEL_SET(sel);
        break;
    default:
        break;
    }
}

void trgm_dac_matrix_config(TRGM_Type *ptr, trgm_dac_matrix_out_t consumer, trgm_dac_matrix_in_t src, bool inv_val)
{
    uint8_t sel;

    sel = (uint8_t)src;
    if (inv_val) {
        sel |= BIT7_MASK;
    }

    switch (consumer) {
    case trgm_dac_matrix_output_to_acmp0_ch0:
        ptr->DAC_MATRIX_SEL0 = (ptr->DAC_MATRIX_SEL0 & ~TRGM_DAC_MATRIX_SEL0_ACMP0_CH0_DAC_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL0_ACMP0_CH0_DAC_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_acmp0_ch1:
        ptr->DAC_MATRIX_SEL0 = (ptr->DAC_MATRIX_SEL0 & ~TRGM_DAC_MATRIX_SEL0_ACMP0_CH1_DAC_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL0_ACMP0_CH1_DAC_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm0_dac0:
        ptr->DAC_MATRIX_SEL0 = (ptr->DAC_MATRIX_SEL0 & ~TRGM_DAC_MATRIX_SEL0_PWM0_DAC0_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL0_PWM0_DAC0_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm0_dac1:
        ptr->DAC_MATRIX_SEL0 = (ptr->DAC_MATRIX_SEL0 & ~TRGM_DAC_MATRIX_SEL0_PWM0_DAC1_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL0_PWM0_DAC1_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm0_dac2:
        ptr->DAC_MATRIX_SEL1 = (ptr->DAC_MATRIX_SEL1 & ~TRGM_DAC_MATRIX_SEL1_PWM0_DAC2_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL1_PWM0_DAC2_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm0_dac3:
        ptr->DAC_MATRIX_SEL1 = (ptr->DAC_MATRIX_SEL1 & ~TRGM_DAC_MATRIX_SEL1_PWM0_DAC3_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL1_PWM0_DAC3_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm1_dac0:
        ptr->DAC_MATRIX_SEL1 = (ptr->DAC_MATRIX_SEL1 & ~TRGM_DAC_MATRIX_SEL1_PWM1_DAC0_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL1_PWM1_DAC0_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm1_dac1:
        ptr->DAC_MATRIX_SEL1 = (ptr->DAC_MATRIX_SEL1 & ~TRGM_DAC_MATRIX_SEL1_PWM1_DAC1_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL1_PWM1_DAC1_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm1_dac2:
        ptr->DAC_MATRIX_SEL2 = (ptr->DAC_MATRIX_SEL2 & ~TRGM_DAC_MATRIX_SEL2_PWM1_DAC2_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL2_PWM1_DAC2_SEL_SET(sel);
        break;
    case trgm_dac_matrix_output_to_pwm1_dac3:
        ptr->DAC_MATRIX_SEL2 = (ptr->DAC_MATRIX_SEL2 & ~TRGM_DAC_MATRIX_SEL2_PWM1_DAC3_SEL_MASK)
                             | TRGM_DAC_MATRIX_SEL2_PWM1_DAC3_SEL_SET(sel);
        break;
    default:
        break;
    }
}

void trgm_pos_matrix_config(TRGM_Type *ptr, trgm_pos_matrix_out_t consumer, trgm_pos_matrix_in_t src, bool inv_val)
{
    uint8_t sel;

    sel = (uint8_t)src;
    if (inv_val) {
        sel |= BIT7_MASK;
    }

    switch (consumer) {
    case trgm_pos_matrix_output_to_qeo0:
        ptr->POS_MATRIX_SEL0 = (ptr->POS_MATRIX_SEL0 & ~TRGM_POS_MATRIX_SEL0_QEO0_POSIN_SEL_MASK)
                             | TRGM_POS_MATRIX_SEL0_QEO0_POSIN_SEL_SET(sel);
        break;
    case trgm_pos_matrix_output_to_qeo1:
        ptr->POS_MATRIX_SEL0 = (ptr->POS_MATRIX_SEL0 & ~TRGM_POS_MATRIX_SEL0_QEO1_POSIN_SEL_MASK)
                             | TRGM_POS_MATRIX_SEL0_QEO1_POSIN_SEL_SET(sel);
        break;
    default:
        break;
    }
}
