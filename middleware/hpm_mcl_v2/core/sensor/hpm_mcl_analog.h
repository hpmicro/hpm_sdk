/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_ANALOG_H
#define HPM_MCL_ANALOG_H

#include "hpm_mcl_common.h"
#include "hpm_mcl_filter.h"

enum {
    mcl_analog_init_error = MAKE_STATUS(mcl_group_analog, 0),
    mcl_analog_not_ready = MAKE_STATUS(mcl_group_analog, 1),
    mcl_analog_update_location_error = MAKE_STATUS(mcl_group_analog, 2),
    mcl_analog_get_value_error = MAKE_STATUS(mcl_group_analog, 3),
};

typedef enum {
    analog_status_null = 0,
    analog_status_init = 1,
    analog_status_run = 2,
    analog_status_fail = 3,
} mcl_analog_status_t;

/**
 * @brief Analog function channel numbers
 *
 */
typedef enum {
    analog_a_current = 0,
    analog_b_current = 1,
    analog_c_current = 2,
    analog_a_voltage = 3,
    analog_b_voltage = 4,
    analog_c_voltage = 5,
    analog_vbus = 6,
} mcl_analog_chn_t;

/**
 * @brief callback function
 *
 */
typedef struct {
    hpm_mcl_stat_t (*init)(void);
    hpm_mcl_stat_t (*update_sample_location)(mcl_analog_chn_t chn, uint32_t tick);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*process_by_user)(mcl_analog_chn_t chn, int32_t adc_value, float *output);
    hpm_mcl_stat_t (*get_value)(mcl_analog_chn_t chn, int32_t *value);
    hpm_mcl_stat_t (*step_convert)(mcl_analog_chn_t chn, float value, float angle, float *output, bool is_closed_loop);
} mcl_analog_callback_t;


/**
 * @brief Configuration of analog functions
 *
 */
typedef struct {
    mcl_analog_callback_t callback;
    bool enable_a_current;
    bool enable_b_current;
    bool enable_c_current;
    bool enable_a_voltage;
    bool enable_b_voltage;
    bool enable_c_voltage;
    bool enable_vbus;
    bool enable_process_by_user;
    bool enable_step_convert_by_user;
    bool enable_filter[MCL_ANALOG_CHN_NUM];
} mcl_analog_cfg_t;

/**
 * @brief Operational data of the analog function
 *
 */
typedef struct {
    mcl_analog_cfg_t *cfg;
    physical_board_analog_t *board_parameter;
    mcl_filter_iir_df1_t *iir[MCL_ANALOG_CHN_NUM];
    float current[MCL_ANALOG_CHN_NUM];
    int32_t *num_current_sample_res;
    mcl_analog_status_t status;
} mcl_analog_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize data structures for analog functions
 *
 * @param analog @ref mcl_analog_t
 * @param cfg @ref mcl_analog_cfg_t
 * @param mcl_cfg @ref mcl_cfg_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_analog_init(mcl_analog_t *analog, mcl_analog_cfg_t *cfg, mcl_cfg_t *mcl_cfg);

/**
 * @brief Get the value of the analog channel
 *
 * @param analog @ref mcl_analog_t
 * @param chn @ref mcl_analog_chn_t
 * @param value Processed Sample Values
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_analog_get_value(mcl_analog_t *analog, mcl_analog_chn_t chn, float *value);

/**
 * @brief Initializing the iir filter for analog functions
 *
 * @param analog @ref mcl_analog_t
 * @param chn @ref mcl_analog_chn_t
 * @param iir @ref mcl_filter_iir_df1_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_analog_iir_filter_init(mcl_analog_t *analog, mcl_analog_chn_t chn, mcl_filter_iir_df1_t *iir);

/**
 * @brief Turn off the iir filter for analog functions
 *
 * @param analog @ref mcl_analog_t
 * @param chn @ref mcl_analog_chn_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_analog_disable_iir_filter(mcl_analog_t *analog, mcl_analog_chn_t chn);

/**
 * @brief Convert step motor current
 *
 * @param analog @ref mcl_analog_t
 * @param value Current value
 * @param chn @ref mcl_analog_chn_t
 * @param angle Current angle
 * @param output Output current value
 * @param is_closed_loop Whether to use closed loop mode
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_analog_step_convert(mcl_analog_t *analog, float value, mcl_analog_chn_t chn, float angle, float *output, bool is_closed_loop);

#ifdef __cplusplus
}
#endif

#endif
