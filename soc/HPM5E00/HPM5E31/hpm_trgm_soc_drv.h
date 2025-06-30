/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_TRGM_SOC_DRV_H
#define HPM_TRGM_SOC_DRV_H

#include "hpm_soc.h"
#include "hpm_trgm_regs.h"

typedef enum {
    trgm_adc_matrix_output_to_qei0_adc0 = 0,
    trgm_adc_matrix_output_to_qei0_adc1,
    trgm_adc_matrix_output_to_qei1_adc0,
    trgm_adc_matrix_output_to_qei1_adc1,
} trgm_adc_matrix_out_t;

typedef enum {
    trgm_adc_matrix_in_from_adc0 = 0,
    trgm_adc_matrix_in_from_adc1,
    trgm_adc_matrix_in_from_sdm_adc0,
    trgm_adc_matrix_in_from_sdm_adc1,
    trgm_adc_matrix_in_from_sdm_adc2,
    trgm_adc_matrix_in_from_sdm_adc3,
} trgm_adc_matrix_in_t;

typedef enum {
    trgm_dac_matrix_output_to_acmp0_ch0 = 0,
    trgm_dac_matrix_output_to_acmp0_ch1,
    trgm_dac_matrix_output_to_pwm0_dac0,
    trgm_dac_matrix_output_to_pwm0_dac1,
    trgm_dac_matrix_output_to_pwm0_dac2,
    trgm_dac_matrix_output_to_pwm0_dac3,
    trgm_dac_matrix_output_to_pwm1_dac0,
    trgm_dac_matrix_output_to_pwm1_dac1,
    trgm_dac_matrix_output_to_pwm1_dac2,
    trgm_dac_matrix_output_to_pwm1_dac3,
} trgm_dac_matrix_out_t;

typedef enum {
    trgm_dac_matrix_in_from_qeo0_dac0 = 0,
    trgm_dac_matrix_in_from_qeo0_dac1,
    trgm_dac_matrix_in_from_qeo0_dac2,
    trgm_dac_matrix_in_from_qeo1_dac0,
    trgm_dac_matrix_in_from_qeo1_dac1,
    trgm_dac_matrix_in_from_qeo1_dac2,
} trgm_dac_matrix_in_t;

typedef enum  {
    trgm_pos_matrix_output_to_qeo0 = 0,
    trgm_pos_matrix_output_to_qeo1,
} trgm_pos_matrix_out_t;

typedef enum {
    trgm_pos_matrix_in_from_qei0 = 0,
    trgm_pos_matrix_in_from_qei1,
} trgm_pos_matrix_in_t;

#if defined(__cplusplus)
extern "C" {
#endif

void trgm_adc_matrix_config(TRGM_Type *ptr, trgm_adc_matrix_out_t consumer, trgm_adc_matrix_in_t src, bool inv_val);

void trgm_dac_matrix_config(TRGM_Type *ptr, trgm_dac_matrix_out_t consumer, trgm_dac_matrix_in_t src, bool inv_val);

void trgm_pos_matrix_config(TRGM_Type *ptr, trgm_pos_matrix_out_t consumer, trgm_pos_matrix_in_t src, bool inv_val);

#if defined(__cplusplus)
}
#endif

#endif /* HPM_TRGM_SOC_DRV_H  */
