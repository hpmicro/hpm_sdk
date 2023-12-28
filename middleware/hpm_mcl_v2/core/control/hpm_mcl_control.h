/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_CONTROL_H
#define HPM_MCL_CONTROL_H
#include "hpm_mcl_common.h"
#include "hpm_mcl_math.h"

/**
 * @brief Control Module Status
 *
 */
typedef enum {
    control_status_null = 0,
    control_status_init = 1,
    control_status_run = 2,
    control_status_fail = 3,
} mcl_control_status_t;

/**
 * @brief Six sectors of svpwm
 *
 */
typedef enum {
    svpwm_sector1 = 1,
    svpwm_sector2 = 2,
    svpwm_sector3 = 3,
    svpwm_sector4 = 4,
    svpwm_sector5 = 5,
    svpwm_sector6 = 6,
} mcl_svpwm_sector_t;

/**
 * @brief Configuration data for pid
 *
 */
typedef struct {
    hpm_mcl_type_t kp;
    hpm_mcl_type_t ki;
    hpm_mcl_type_t kd;
    hpm_mcl_type_t integral_max;    /**< Maximum value of integral */
    hpm_mcl_type_t integral_min;    /**< Minimum value of the integral */
    hpm_mcl_type_t output_max;      /**< Maximum value of output */
    hpm_mcl_type_t output_min;      /**< Minimum value of output */
} mcl_control_pid_cfg_t;

/**
 * @brief pid running data
 *
 */
typedef struct {
    mcl_control_pid_cfg_t cfg;
    hpm_mcl_type_t integral;
} mcl_control_pid_t;

/**
 * @brief pwm duty cycle
 *
 */
typedef struct {
    hpm_mcl_type_t a;
    hpm_mcl_type_t b;
    hpm_mcl_type_t c;
    /**
     * @brief step motor pwm
     *
     */
    hpm_mcl_type_t a0;
    hpm_mcl_type_t a1;
    hpm_mcl_type_t b0;
    hpm_mcl_type_t b1;
} mcl_control_svpwm_duty_t;

/**
 * @brief Supported control algorithms
 *
 */
typedef struct {
    hpm_mcl_type_t (*sin_x)(hpm_mcl_type_t x);
    hpm_mcl_type_t (*cos_x)(hpm_mcl_type_t x);
    hpm_mcl_type_t (*arctan_x)(hpm_mcl_type_t y, hpm_mcl_type_t x);
    hpm_mcl_stat_t (*park)(hpm_mcl_type_t alpha, hpm_mcl_type_t beta,
                hpm_mcl_type_t sin_x, hpm_mcl_type_t cos_x,
                hpm_mcl_type_t *d, hpm_mcl_type_t *q);
    hpm_mcl_stat_t (*clarke)(hpm_mcl_type_t ia, hpm_mcl_type_t ib, hpm_mcl_type_t ic,
                hpm_mcl_type_t *alpha, hpm_mcl_type_t *beta);
    hpm_mcl_stat_t (*currentd_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*currentq_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*speed_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*position_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*invpark)(hpm_mcl_type_t d, hpm_mcl_type_t q, hpm_mcl_type_t sin_x,
                    hpm_mcl_type_t cos_x, hpm_mcl_type_t *alpha, hpm_mcl_type_t *beta);
    hpm_mcl_stat_t (*svpwm)(hpm_mcl_type_t alpha, hpm_mcl_type_t beta, hpm_mcl_type_t vbus, mcl_control_svpwm_duty_t *duty);
    hpm_mcl_stat_t (*step_svpwm)(hpm_mcl_type_t alpha, hpm_mcl_type_t beta, hpm_mcl_type_t vbus, mcl_control_svpwm_duty_t *duty);
} mcl_control_method_t;

/**
 * @brief callback function
 *
 */
typedef struct {
    void (*init)(void); /**< User-defined function initialisation */
    _FUNC_OPTIONAL_ mcl_control_method_t method;    /**< User-defined algorithms */
} mcl_control_callback_t;

/**
 * @brief Control Function Configuration
 *
 */
typedef struct {
    mcl_control_callback_t callback;
    mcl_control_pid_t currentd_pid_cfg;
    mcl_control_pid_t currentq_pid_cfg;
    mcl_control_pid_t speed_pid_cfg;
    mcl_control_pid_t position_pid_cfg;
} mcl_control_cfg_t;

/**
 * @brief Control function running data
 *
 */
typedef struct {
    mcl_control_cfg_t *cfg;
    mcl_control_method_t method;
} mcl_control_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise the operating data for the control function
 *
 * @param control @ref mcl_control_t
 * @param cfg @ref mcl_control_cfg_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_control_init(mcl_control_t *control, mcl_control_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif
