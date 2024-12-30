/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_FILTER_H
#define HPM_MCL_FILTER_H

#include "hpm_common.h"

/**
 * @brief Gain matrix for iir filter type df1
 *
 */
typedef struct {
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
    float scale;
} mcl_filter_iir_df1_matrix_t;

/**
 * @brief Buffer for running data
 *
 */
typedef struct {
    float x0;
    float x1;
    float x2;
    float y1;
    float y2;
} mcl_filter_iir_df1_memory_t;

/**
 * @brief Configuration of iir filter type df1
 *
 */
typedef struct {
    uint8_t section; /**< Sections of the iir filter */
    mcl_filter_iir_df1_matrix_t *matrix;    /**< gain matrix, buffer */

} mcl_filter_iir_df1_cfg_t;

/**
 * @brief iir df1 running data
 *
 */
typedef struct {
    mcl_filter_iir_df1_cfg_t *cfg;
    mcl_filter_iir_df1_memory_t *mem;   /**< The iir order is equal to the array size */
} mcl_filter_iir_df1_t;

/**
 * @brief configuration of pll type ii filter
 *
 */
typedef struct {
    float k1; /**< adc gain */
    float a;  /**< Filter Zero Coefficient */
    float b;  /**< Filter pole coefficient */
    float c;  /**< Integrator gain factor */
} mcl_filter_pll_type_ii_cfg_t;

/**
 * @brief Running data of pll type ii filter
 *
 */
typedef struct {
    mcl_filter_pll_type_ii_cfg_t cfg;
    float x0;   /**< x stands for the intermediate variable, which the user does not need to care about */
    float x1;
    float x2;
    float x3;
} mcl_filter_pll_type_ii_t;

/**
 * @brief configuration of pll type i filter
 *
 */
typedef struct {
    float k1; /**< adc gain */
    float kp;  /**< Filter Zero Coefficient */
    float ki;  /**< Filter pole coefficient */
    float c;  /**< Integrator gain factor */
    float lowpass_k; /**< lowpass filter gain */
} mcl_filter_pll_type_i_cfg_t;

/**
 * @brief Running data of pll type i filter
 *
 */
typedef struct {
    mcl_filter_pll_type_i_cfg_t cfg;
    float x0;   /**< x stands for the intermediate variable, which the user does not need to care about */
    float x1;
    float x2;
} mcl_filter_pll_type_i_t;

/**
 * @brief Phase Locked Loop Filter Output Data
 *
 */
typedef struct {
    float theta;
    float speed;
} mcl_filter_pll_output_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialising the iir filter
 *
 * @param iir @ref mcl_filter_iir_df1_t
 * @param cfg @ref mcl_filter_iir_df1_cfg_t
 * @param mem @ref mcl_filter_iir_df1_memory_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_filter_iir_df1_init(mcl_filter_iir_df1_t *iir, mcl_filter_iir_df1_cfg_t *cfg, mcl_filter_iir_df1_memory_t *mem);

/**
 * @brief iir filtering of input data
 *
 * @param iir @ref mcl_filter_iir_df1_t
 * @param input raw data
 * @return Filtered data
 */
float hpm_mcl_filter_iir_df1(mcl_filter_iir_df1_t *iir, float input);

/**
 * Type II Phase-Locked Loop (PLL) Filter.
 * This function updates the PLL state based on the input sine and cosine values and calculates the output theta and speed.
 *
 * @param pll Pointer to the PLL structure containing the state and configuration parameters.
 * @param sin_val The sine value of the input signal, representing the sine component.
 * @param cos_val The cosine value of the input signal, representing the cosine component.
 * @param output Pointer to the output structure to store the computed theta and speed.
 */
void hpm_mcl_filter_pll_type_ii(mcl_filter_pll_type_ii_t *pll, float sin_val, float cos_val, mcl_filter_pll_output_t *output);

/**
 * Type I Phase-Locked Loop (PLL) Filter.
 * This function processes the input signal to estimate the phase and speed of the signal.
 *
 * @param pll Pointer to the mcl_filter_pll_type_i_t structure, containing the internal state and configuration of the PLL.
 * @param sin_val The sine component of the input signal, used for phase difference calculation.
 * @param cos_val The cosine component of the input signal, used for phase difference calculation.
 * @param output Pointer to the mcl_filter_pll_output_t structure, for storing the output results including theta and speed.
 */
void hpm_mcl_filter_pll_type_i(mcl_filter_pll_type_i_t *pll, float sin_val, float cos_val, mcl_filter_pll_output_t *output);
#ifdef __cplusplus
}
#endif

#endif