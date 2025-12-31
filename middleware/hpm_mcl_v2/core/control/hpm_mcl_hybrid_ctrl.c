/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_hybrid_ctrl.h"
#include <math.h>

/**
 * @brief Initialize hybrid control parameters with default values
 */
void mcl_hybrid_ctrl_init(mcl_hybrid_ctrl_cfg_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    /* Default parameters for stable control */
    cfg->kp = 0.0f;      /* Position stiffness (N*m/rad) */
    cfg->kd = 0.0f;       /* Damping (N*m*s/rad) */
    cfg->tau_ff = 0.0f;    /* No feedforward by default */
    cfg->q_des = 0.0f;     /* Zero position reference */
    cfg->dq_des = 0.0f;    /* Zero velocity reference */
    cfg->tau_max = 0.0f;   /* No upper limit by default */
    cfg->tau_min = 0.0f;   /* No lower limit by default */
    cfg->speed_lpf_alpha = 0.0f;  /* Speed filter disabled by default */
    cfg->speed_deadzone = 0.0f;   /* Speed deadzone disabled by default */
}

/**
 * @brief Execute one step of hybrid control
 *
 * Implements hybrid force-position control algorithm:
 * tau = tau_ff + kp * (q_des - q) + kd * (dq_des - dq)
 *
 * This is essentially a PD controller with feedforward torque,
 * providing impedance/compliance control suitable for:
 * - Robot joint control
 * - Servo system control
 * - Force-sensitive manipulation
 * - Compliant interaction with environment
 */
void mcl_hybrid_ctrl_step(const mcl_hybrid_ctrl_cfg_t *cfg, mcl_hybrid_ctrl_state_t *state)
{
    float pos_error, vel_error;
    float torque_p, torque_d;
    float speed_filtered;

    if (cfg == NULL || state == NULL) {
        return;
    }

    /* Speed filtering with low-pass filter */
    if (cfg->speed_lpf_alpha > 0.0f) {
        state->speed_lpf = cfg->speed_lpf_alpha * state->dq_actual +
                          (1.0f - cfg->speed_lpf_alpha) * state->speed_lpf;
        speed_filtered = state->speed_lpf;
    } else {
        speed_filtered = state->dq_actual;
    }

    /* Apply speed deadzone */
    if (cfg->speed_deadzone > 0.0f) {
        if (speed_filtered > -cfg->speed_deadzone && speed_filtered < cfg->speed_deadzone) {
            speed_filtered = 0.0f;
        }
    }

    /* Calculate position error: e_p = q_des - q_actual */
    pos_error = cfg->q_des - state->q_actual;

    /* Calculate velocity error: e_v = dq_des - dq_filtered */
    vel_error = cfg->dq_des - speed_filtered;

    /* Store errors for monitoring */
    state->pos_error = pos_error;
    state->vel_error = vel_error;

    /* Proportional term: tau_p = kp * e_p */
    torque_p = cfg->kp * pos_error;

    /* Derivative term: tau_d = kd * e_v */
    torque_d = cfg->kd * vel_error;

    /* Hybrid control law: tau = tau_ff + tau_p + tau_d */
    state->tau_output = cfg->tau_ff + torque_p + torque_d;

    /* Apply torque limits if configured */
    if (state->tau_output > cfg->tau_max) {
        state->tau_output = cfg->tau_max;
    }
    if (state->tau_output < cfg->tau_min) {
        state->tau_output = cfg->tau_min;
    }
}

