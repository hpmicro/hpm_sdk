/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_ENCODER_H
#define HPM_MCL_ENCODER_H
#include "hpm_mcl_common.h"
#include "hpm_mcl_filter.h"

enum {
    mcl_encoder_start_sample_error = MAKE_STATUS(mcl_group_encoder, 0),
    mcl_encoder_get_tick_error = MAKE_STATUS(mcl_group_encoder, 1),
    mcl_encoder_update_tick_error = MAKE_STATUS(mcl_group_encoder, 2),
    mcl_encoder_get_theta_error = MAKE_STATUS(mcl_group_encoder, 3),
    mcl_encoder_not_ready = MAKE_STATUS(mcl_group_encoder, 4),
};

typedef enum {
    encoder_status_null = 0,
    encoder_status_init = 1,
    encoder_status_run = 2,
    encoder_status_fail = 3,
} mcl_encoder_status_t;

/**
 * @brief Method of calculating speed
 *
 */
typedef enum {
    encoder_method_t,
    encoder_method_m,
    encoder_method_m_t,
    encoder_method_pll,
    encoder_method_user,
} mcl_encoder_cal_speed_function_t;

/**
 * @brief callback function
 *
 */
typedef struct  mcl_encoder_callback {
    hpm_mcl_stat_t (*init)(void);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*update_trig_lead_tick)(uint32_t lead_tick);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*get_lead_tick)(uint32_t *lead_tick);
    hpm_mcl_stat_t (*start_sample)(void);
    hpm_mcl_stat_t (*get_theta)(float *theta);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*get_absolute_theta)(float *theta);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*process_by_user)(float theta, float *speed, float *theta_forecast);
} mcl_encoder_callback_t;

/**
 * @brief Maximum memory required for computational speed, word
 *
 */
#define MCL_ENCODER_CAL_STRUCT_MAX_MEMMORY  (16)
typedef struct {
    float theta_last;
    float speed_last;
    float ts_sigma;
} mcl_encoder_cal_speed_t_function_t;

typedef struct {
    float theta_last;
    float ts_sigma;
} mcl_encoder_cal_speed_m_function_t;

typedef struct {
    float theta_last;
    float ts_sigma;
    float theta_sigma;
    float speed_filter_last;
    float speed_last;
} mcl_encoder_cal_speed_m_t_function_t;

/**
 * @brief pll filter configuration
 *
 */
typedef struct {
    float kp;   /**< scale factor */
    float ki;   /**< Integration factor */
} mcl_encoder_cal_speed_pll_cfg_t;

/**
 * @brief Running data for the pll method of calculating speed
 *
 */
typedef struct {
    mcl_encoder_cal_speed_pll_cfg_t *cfg;
    float *period_call_time_s;
    float pi_integrator;
    float omega_integrator;
    float theta_last;
} mcl_encoder_cal_speed_pll_function_t;

/**
 * @brief Encoder Configuration
 *
 */
typedef struct {
    uint8_t disable_start_sample_interrupt;
    uint64_t communication_interval_us; /**< The communication interval of the communicating encoder ensures that an angle reading can be completed within the interval. Optical encoders can be read at any time, given as zero. */
    mcl_encoder_cal_speed_function_t speed_cal_method;
    mcl_encoder_cal_speed_pll_cfg_t cal_speed_pll_cfg;
    float timeout_s;                  /**< Timeout time, after this time angle is not updated, the speed will be cleared to zero. */
    float speed_abs_switch_m_t;     /**< Use the m_t method to set the validity, the speed is greater than the set speed, use the m method, the speed is less than the set speed t use the t method to calculate the speed. */
    uint32_t precision;     /**< Sensor accuracy, various encoders converted to number of lines. The maximum number of angular changes that can occur per revolution of the motor. */
    float period_call_time_s;   /**< The time of the cycle call, the handler function should be called periodically, in s. */
    mcl_encoder_callback_t callback;
} mcl_encoer_cfg_t;


/**
 * @brief Encoder operation data
 *
 */
typedef struct {
    union {
        mcl_encoder_cal_speed_t_function_t t_method;
        mcl_encoder_cal_speed_m_function_t m_method;
        mcl_encoder_cal_speed_m_t_function_t m_t_method;
        mcl_encoder_cal_speed_pll_function_t pll_method;
        uint32_t memory[MCL_ENCODER_CAL_STRUCT_MAX_MEMMORY];
    } cal_speed;
    mcl_filter_iir_df1_t *iirfilter;
    mcl_encoder_status_t status;
    mcl_encoer_cfg_t *cfg;
    int32_t *mcu_clock_tick;
    int32_t *pole_num;
    float theta_initial;
    float *current_loop_ts;
    mcl_user_value_t force_set_theta;
    struct {
        float speed;
        float theta;
        float theta_forecast;
    } result;
} mcl_encoder_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize encoder data
 *
 * @param encoder @ref mcl_encoder_t
 * @param mcl_cfg @ref mcl_cfg_t
 * @param encoder_cfg @ref mcl_encoer_cfg_t
 * @param iir @ref mcl_filter_iir_df1_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_encoder_init(mcl_encoder_t *encoder, mcl_cfg_t *mcl_cfg, mcl_encoer_cfg_t *encoder_cfg, mcl_filter_iir_df1_t *iir);

/**
 * @brief Encoder starts sampling
 *
 * @param encoder @ref mcl_encoder_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_encoder_start_sample(mcl_encoder_t *encoder);

/**
 * @brief Setting the initial angle of the encoder
 *
 * @param encoder @ref mcl_encoder_t
 * @param theta Initial angle value
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_encoder_set_initial_theta(mcl_encoder_t *encoder, float theta);

/**
 * @brief Encoder processing, cycle call
 *
 * @param encoder @ref mcl_encoder_t
 * @param tick_deta Difference in the number of cycles between two calls
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_encoder_process(mcl_encoder_t *encoder, uint32_t tick_deta);

/**
 * @brief Force update of encoder status
 *
 * @param encoder @ref mcl_encoder_t
 * @param status @ref mcl_encoder_status_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_encoder_set_status(mcl_encoder_t *encoder, mcl_encoder_status_t status);

/**
 * @brief Get the encoder angle value
 *
 * @param encoder @ref mcl_encoder_t
 * @return angle value, rad
 */
float hpm_mcl_encoder_get_theta(mcl_encoder_t *encoder);

/**
 * @brief Get speed
 *
 * @param encoder @ref mcl_encoder_t
 * @return rad/s
 */
float hpm_mcl_encoder_get_speed(mcl_encoder_t *encoder);

/**
 * @brief Get the angle predicted from the speed
 *
 * @param encoder @ref mcl_encoder_t
 * @return theta, rad
 */
float hpm_mcl_encoder_get_forecast_theta(mcl_encoder_t *encoder);

/**
 * @brief Getting the raw angle data of the encoder
 *
 * @param encoder @ref mcl_encoder_t
 * @return theta, rad
 */
float hpm_mcl_encoder_get_raw_theta(mcl_encoder_t *encoder);

/**
 * @brief Force setting of the output angle of the encoder function
 *
 * @param encoder @ref mcl_encoder_t
 * @param theta rad
 * @param enable Enable or disable the forced output function
 */
void hpm_mcl_encoder_force_theta(mcl_encoder_t *encoder, float theta, bool enable);

/**
 * @brief Get the absolute position of the angle
 *
 * @param encoder @ref mcl_encoder_t
 * @param theta rad
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_encoder_get_absolute_theta(mcl_encoder_t *encoder, float *theta);

#ifdef __cplusplus
}
#endif

#endif