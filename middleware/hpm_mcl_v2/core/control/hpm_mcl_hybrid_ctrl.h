/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_HYBRID_CTRL_H
#define HPM_MCL_HYBRID_CTRL_H

#include "hpm_mcl_common.h"

/**
 * @brief Hybrid Force-Position Control Library
 *
 * This library implements the hybrid control algorithm for force-position control:
 * tau = tau_ff + kp * (q_des - q) + kd * (dq_des - dq)
 *
 * Features:
 * - Simple single-function API
 * - Real-time parameter adjustment
 * - Impedance/compliance control
 * - Suitable for robot joint control and servo systems
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hybrid control parameters structure
 */
typedef struct {
    float kp;           /**< Position stiffness gain (N*m/rad) */
    float kd;           /**< Damping gain (N*m*s/rad) */
    float tau_ff;       /**< Feedforward torque for gravity compensation (N*m) */
    float q_des;        /**< Desired joint position (rad) */
    float dq_des;       /**< Desired joint velocity (rad/s), typically 0 for position control */
    float tau_max;      /**< Maximum output torque limit (N*m), 0 = no limit */
    float tau_min;      /**< Minimum output torque limit (N*m), 0 = no limit */
    float speed_lpf_alpha;  /**< Speed low-pass filter coefficient (0-1), 0 = disabled, typical 0.05 */
    float speed_deadzone;   /**< Speed deadzone (rad/s), speed < deadzone will be set to 0 */
} mcl_hybrid_ctrl_cfg_t;

/**
 * @brief Hybrid control state/output structure
 */
typedef struct {
    float q_actual;     /**< Current joint position (rad) */
    float dq_actual;    /**< Current joint velocity (rad/s) */
    float tau_output;   /**< Output torque command (N*m) */
    float pos_error;    /**< Position tracking error (rad) */
    float vel_error;    /**< Velocity tracking error (rad/s) */
    float speed_lpf;    /**< Internal low-pass filtered speed (rad/s) */
} mcl_hybrid_ctrl_state_t;

/**
 * @brief Initialize hybrid control parameters with default values
 *
 * Default values:
 * - kp = 0.0 N*m/rad
 * - kd = 0.0 N*m*s/rad
 * - tau_ff = 0.0 N*m
 * - q_des = 0.0 rad
 * - dq_des = 0.0 rad/s
 * - tau_max = 0.0 N*m (no limit)
 * - tau_min = 0.0 N*m (no limit)
 * - speed_lpf_alpha = 0.0 (disabled)
 * - speed_deadzone = 0.0 (disabled)
 *
 * @param cfg Pointer to configuration structure
 */
void mcl_hybrid_ctrl_init(mcl_hybrid_ctrl_cfg_t *cfg);

/**
 * @brief Execute one step of hybrid control
 *
 * Control law:
 * tau = tau_ff + kp * (q_des - q_actual) + kd * (dq_des - dq_actual)
 *
 * @param cfg Pointer to configuration structure (input)
 * @param state Pointer to state structure (input/output)
 *              Input: q_actual, dq_actual
 *              Output: tau_output, pos_error, vel_error
 */
void mcl_hybrid_ctrl_step(const mcl_hybrid_ctrl_cfg_t *cfg, mcl_hybrid_ctrl_state_t *state);

/**
 * @brief Set position stiffness gain
 *
 * @param cfg Pointer to configuration structure
 * @param kp Position stiffness gain (N*m/rad)
 *           Higher value = stiffer, faster response
 *           Lower value = more compliant, softer
 */
static inline void mcl_hybrid_ctrl_set_kp(mcl_hybrid_ctrl_cfg_t *cfg, float kp)
{
    cfg->kp = kp;
}

/**
 * @brief Set damping gain
 *
 * @param cfg Pointer to configuration structure
 * @param kd Damping gain (N*m*s/rad)
 */
static inline void mcl_hybrid_ctrl_set_kd(mcl_hybrid_ctrl_cfg_t *cfg, float kd)
{
    cfg->kd = kd;
}

/**
 * @brief Set feedforward torque
 *
 * @param cfg Pointer to configuration structure
 * @param tau_ff Feedforward torque (N*m)
 *               Used for gravity compensation, friction compensation, etc.
 */
static inline void mcl_hybrid_ctrl_set_tau_ff(mcl_hybrid_ctrl_cfg_t *cfg, float tau_ff)
{
    cfg->tau_ff = tau_ff;
}

/**
 * @brief Set desired position
 *
 * @param cfg Pointer to configuration structure
 * @param q_des Desired joint position (rad)
 */
static inline void mcl_hybrid_ctrl_set_position(mcl_hybrid_ctrl_cfg_t *cfg, float q_des)
{
    cfg->q_des = q_des;
}

/**
 * @brief Set desired velocity
 *
 * @param cfg Pointer to configuration structure
 * @param dq_des Desired joint velocity (rad/s)
 *               Typically 0 for position holding
 */
static inline void mcl_hybrid_ctrl_set_velocity(mcl_hybrid_ctrl_cfg_t *cfg, float dq_des)
{
    cfg->dq_des = dq_des;
}

/**
 * @brief Set output torque limits
 *
 * @param cfg Pointer to configuration structure
 * @param tau_min Minimum output torque (N*m)
 * @param tau_max Maximum output torque (N*m)
 */
static inline void mcl_hybrid_ctrl_set_limits(mcl_hybrid_ctrl_cfg_t *cfg, float tau_min, float tau_max)
{
    cfg->tau_min = tau_min;
    cfg->tau_max = tau_max;
}

/**
 * @brief Set speed filter parameters
 *
 * @param cfg Pointer to configuration structure
 * @param lpf_alpha Low-pass filter coefficient (0-1), 0 = disabled
 *                  Smaller value = stronger filtering (e.g., 0.05 for 95% old + 5% new)
 * @param deadzone Speed deadzone (rad/s), speed within [-deadzone, +deadzone] will be set to 0
 */
static inline void mcl_hybrid_ctrl_set_speed_filter(mcl_hybrid_ctrl_cfg_t *cfg, float lpf_alpha, float deadzone)
{
    cfg->speed_lpf_alpha = lpf_alpha;
    cfg->speed_deadzone = deadzone;
}

/**
 * @brief Set all parameters at once
 *
 * @param cfg Pointer to configuration structure
 * @param kp Position stiffness gain (N*m/rad)
 * @param kd Damping gain (N*m*s/rad)
 * @param tau_ff Feedforward torque (N*m)
 */
static inline void mcl_hybrid_ctrl_set_params(mcl_hybrid_ctrl_cfg_t *cfg, float kp, float kd, float tau_ff)
{
    cfg->kp = kp;
    cfg->kd = kd;
    cfg->tau_ff = tau_ff;
}

/**
 * @brief Set desired trajectory
 *
 * @param cfg Pointer to configuration structure
 * @param q_des Desired position (rad)
 * @param dq_des Desired velocity (rad/s)
 */
static inline void mcl_hybrid_ctrl_set_trajectory(mcl_hybrid_ctrl_cfg_t *cfg, float q_des, float dq_des)
{
    cfg->q_des = q_des;
    cfg->dq_des = dq_des;
}

/**
 * @brief Get output torque
 *
 * @param state Pointer to state structure
 * @return Output torque (N*m)
 */
static inline float mcl_hybrid_ctrl_get_torque(const mcl_hybrid_ctrl_state_t *state)
{
    return state->tau_output;
}

/**
 * @brief Get position error
 *
 * @param state Pointer to state structure
 * @return Position error (rad)
 */
static inline float mcl_hybrid_ctrl_get_pos_error(const mcl_hybrid_ctrl_state_t *state)
{
    return state->pos_error;
}

/**
 * @brief Get velocity error
 *
 * @param state Pointer to state structure
 * @return Velocity error (rad/s)
 */
static inline float mcl_hybrid_ctrl_get_vel_error(const mcl_hybrid_ctrl_state_t *state)
{
    return state->vel_error;
}

#ifdef __cplusplus
}
#endif

#endif /* HPM_MCL_HYBRID_CTRL_H */

