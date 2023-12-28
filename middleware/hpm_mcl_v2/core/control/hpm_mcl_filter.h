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

#ifdef __cplusplus
}
#endif

#endif