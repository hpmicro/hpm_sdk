/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_PATH_PLAN_H
#define HPM_MCL_PATH_PLAN_H

#include "hpm_mcl_common.h"

/**
 * @brief Trapezoidal Curve Configuration
 *
 */
typedef struct {
    float acc;  /**< acceleration rad/s^2 */
    float dec;  /**< deceleration rad/s^2 */
    float speed;    /**< running speed rsd/s */
    float time; /**< total time */
    float acc_time; /**< acceleration time, s */
    float dec_time; /**< deceleration time, s */
} path_plan_t_cure_cfg_t;;

/**
 * @brief Configuration of path planning
 *
 */
typedef struct {
    path_plan_t_cure_cfg_t t_cure;
    float loop_ts;  /**< Time of cycle call in s */
} mcl_path_plan_cfg_t;

/**
 * @brief Running data for path planning
 *
 */
typedef struct {
    mcl_path_plan_cfg_t *cfg;
    int32_t *pole_num;
    struct {
        path_plan_t_cure_cfg_t next;
        path_plan_t_cure_cfg_t current;
        bool next_empty;
        bool current_empty;
        uint32_t ts;
        uint32_t t0;
        uint32_t t1;
        uint32_t t2;
        float a0;
        float a1;
        float a2;
    } t_cure;
    struct {
        float next;
        float last;
        float current;
    } theta;
} mcl_path_plan_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialising the data structure for path planning
 *
 * @param path @ref mcl_path_plan_t
 * @param cfg @ref mcl_path_plan_cfg_t
 * @param mcl @ref mcl_cfg_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_path_init(mcl_path_plan_t *path, mcl_path_plan_cfg_t *cfg, mcl_cfg_t *mcl);

/**
 * @brief Updating data for trapezoidal curves
 *
 * @param path @ref mcl_path_plan_t
 * @param cfg @ref path_plan_t_cure_cfg_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_path_update_t_cure(mcl_path_plan_t *path, path_plan_t_cure_cfg_t *cfg);

/**
 * @brief Get the angle value after path planning
 *
 * @param path @ref mcl_path_plan_t
 * @return theta, rad
 */
float hpm_mcl_path_get_current_theta(mcl_path_plan_t *path);

/**
 * @brief Get the next angle value after path planning
 *
 * @param path @ref mcl_path_plan_t
 * @return theta, rad
 */
float hpm_mcl_path_get_next_theta(mcl_path_plan_t *path);

/**
 * @brief Get the last angle value after path planning
 *
 * @param path @ref mcl_path_plan_t
 * @return theta, rad
 */
float hpm_mcl_path_get_last_theta(mcl_path_plan_t *path);

/**
 * @brief Generate trapezoidal curves, which need to be called periodically, at the same time as the loop_ts configuration
 *
 * @param path @ref mcl_path_plan_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_path_t_cure_generate(mcl_path_plan_t *path);

/**
 * @brief Get whether the curve is complete or not
 *
 * @param path @ref mcl_path_plan_t
 * @return true The generation of the current curve has been completed
 * @return false Current curve generation in progress
 */
bool hpm_mcl_path_t_cure_complete(mcl_path_plan_t *path);

#ifdef __cplusplus
}
#endif

#endif
