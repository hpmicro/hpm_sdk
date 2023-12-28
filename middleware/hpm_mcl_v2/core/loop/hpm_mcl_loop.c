/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_loop.h"

hpm_mcl_stat_t hpm_mcl_loop_init(mcl_loop_t *loop, mcl_loop_cfg_t *cfg, mcl_cfg_t *mcl_cfg,
                                mcl_encoder_t *encoder, mcl_analog_t *analog,
                                mcl_control_t *control, mcl_drivers_t *drivers, mcl_path_plan_t *path)
{
    MCL_ASSERT(loop != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(mcl_cfg != NULL, mcl_invalid_pointer);
    if (cfg->mode != mcl_mode_step_foc) {
        MCL_ASSERT(encoder != NULL, mcl_invalid_pointer);
    } else {
        MCL_ASSERT(path != NULL, mcl_invalid_pointer);
    }
    MCL_ASSERT(analog != NULL, mcl_invalid_pointer);
    MCL_ASSERT(control != NULL, mcl_invalid_pointer);
    MCL_ASSERT(drivers != NULL, mcl_invalid_pointer);

    loop->enable = false;
    loop->cfg = cfg;
    loop->path = path;
    loop->status = loop_status_run;
#ifndef HPM_MCL_Q_EN
    loop->const_vbus = &mcl_cfg->physical.motor.vbus;
#else
    loop->const_vbus = &mcl_cfg->physical_q.motor.vbus;
#endif
    loop->const_time.current_ts = &mcl_cfg->physical.time.current_loop_ts;
    loop->const_time.position_ts = &mcl_cfg->physical.time.speed_loop_ts;
    loop->const_time.speed_ts = &mcl_cfg->physical.time.speed_loop_ts;
    loop->encoder = encoder;
    loop->analog = analog;
    loop->control = control;
    loop->drivers = drivers;
    loop->time.position_ts = 0;
    loop->time.speed_ts = 0;
    loop->exec_ref.id = 0;
    loop->exec_ref.iq = 0;
    loop->exec_ref.position = 0;
    loop->exec_ref.speed = 0;

    hpm_mcl_loop_disable_all_user_set_value(loop);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_loop_set_current_d(mcl_loop_t *loop, mcl_user_value_t id)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->ref_id = id;
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_loop_set_current_q(mcl_loop_t *loop, mcl_user_value_t iq)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->ref_iq = iq;
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_loop_set_speed(mcl_loop_t *loop, mcl_user_value_t speed)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->ref_speed = speed;
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_loop_set_position(mcl_loop_t *loop, mcl_user_value_t position)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->ref_position = position;
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_loop_disable_all_user_set_value(mcl_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->ref_id.enable = false;
    loop->ref_iq.enable = false;
    loop->ref_position.enable = false;
    loop->ref_speed.enable = false;
    return mcl_success;
}


hpm_mcl_stat_t hpm_mcl_bldc_loop(mcl_loop_t *loop)
{
    float ref_speed = 0;
    float ref_position = 0;
    float ia, ib, ic;
    float theta;
    float alpha, beta;
    float sinx, cosx;
    float sens_d, sens_q;
    float ref_d = 0, ref_q = 0;
    float ud, uq;
    float theta_abs;
    mcl_control_svpwm_duty_t duty;
#if defined(MCL_CFG_EN_THETA_FORECAST) && MCL_CFG_EN_THETA_FORECAST
    float sinx_, cosx_;
    float theta_forecast;
#endif

    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    theta = hpm_mcl_encoder_get_theta(loop->encoder);
#if defined(MCL_CFG_EN_THETA_FORECAST) && MCL_CFG_EN_THETA_FORECAST
    theta_forecast = hpm_mcl_encoder_get_forecast_theta(loop->encoder);
#endif
    if (loop->enable) {
        if (loop->cfg->enable_position_loop) {
            loop->time.position_ts += *loop->const_time.current_ts;
            MCL_FUNCTION_SET_IF_ELSE_TRUE(loop->ref_position.enable, ref_position, loop->ref_position.value, loop->exec_ref.position);
            if (loop->time.position_ts >= *loop->const_time.position_ts) {
                loop->time.position_ts = 0;
                MCL_ASSERT_EXEC_CODE_AND_RETURN(hpm_mcl_encoder_get_absolute_theta(loop->encoder, &theta_abs) == mcl_success,
                loop->status = loop_status_fail, mcl_fail);
                loop->control->method.position_pid(ref_position, theta_abs, &loop->control->cfg->position_pid_cfg, &loop->exec_ref.speed);
            }
        } else {
            loop->exec_ref.speed = 0;
            loop->time.position_ts = 0;
        }
        if (loop->cfg->enable_speed_loop) {
            loop->time.speed_ts += *loop->const_time.current_ts;
            MCL_FUNCTION_SET_IF_ELSE_TRUE(loop->ref_speed.enable, ref_speed, loop->ref_speed.value, loop->exec_ref.speed);
            if (loop->time.speed_ts >= *loop->const_time.speed_ts) {
                loop->time.speed_ts = 0;
                loop->control->method.speed_pid(ref_speed, hpm_mcl_encoder_get_speed(loop->encoder),
                &loop->control->cfg->speed_pid_cfg, &loop->exec_ref.iq);
            }
        } else {
            loop->time.speed_ts = 0;
            loop->exec_ref.iq = 0;
        }
        switch (loop->cfg->mode) {
        case mcl_mode_foc:
            /**
             * @brief current sample
             *
             */
            MCL_STATUS_SET_IF_TRUE(mcl_success != hpm_mcl_analog_get_value(loop->analog, analog_a_current, &ia), loop->status, loop_status_fail);
            MCL_STATUS_SET_IF_TRUE(mcl_success != hpm_mcl_analog_get_value(loop->analog, analog_b_current, &ib), loop->status, loop_status_fail);
            ic = -(ia + ib);

            MCL_VALUE_SET_IF_TRUE(loop->ref_id.enable, ref_d, loop->ref_id.value);
            MCL_FUNCTION_SET_IF_ELSE_TRUE(loop->ref_iq.enable, ref_q, loop->ref_iq.value, loop->exec_ref.iq);
            loop->control->method.clarke(ia, ib, ic, &alpha, &beta);
            sinx = loop->control->method.sin_x(theta);
            cosx = loop->control->method.cos_x(theta);
            loop->control->method.park(alpha, beta, sinx, cosx, &sens_d, &sens_q);
            loop->control->method.currentd_pid(ref_d, sens_d, &loop->control->cfg->currentd_pid_cfg, &ud);
            loop->control->method.currentq_pid(ref_q, sens_q, &loop->control->cfg->currentq_pid_cfg, &uq);
    #if defined(MCL_CFG_EN_THETA_FORECAST) && MCL_CFG_EN_THETA_FORECAST
            sinx_ = loop->control->method.sin_x(theta_forecast);
            cosx_ = loop->control->method.cos_x(theta_forecast);
            loop->control->method.invpark(ud, uq, sinx_, cosx_, &alpha, &beta);
    #else
            loop->control->method.invpark(ud, uq, sinx, cosx, &alpha, &beta);
    #endif
            loop->control->method.svpwm(alpha, beta, *loop->const_vbus, &duty);
            hpm_mcl_drivers_update_bldc_duty(loop->drivers, duty.a, duty.b, duty.c);

            break;
        case mcl_mode_hardware_foc:
            MCL_ASSERT_EXEC_CODE_AND_RETURN(false, loop->status = loop_status_fail, mcl_in_development);
            break;
        default:
            break;
        }
    } else {
        duty.a = 0;
        duty.b = 0;
        duty.c = 0;
        hpm_mcl_drivers_update_bldc_duty(loop->drivers, duty.a, duty.b, duty.c);
    }
    return  mcl_success;
}

hpm_mcl_stat_t hpm_mcl_step_loop(mcl_loop_t *loop)
{
    float ia, ib;
    float theta;
    float alpha, beta;
    float sinx, cosx;
    float sens_d, sens_q;
    float ref_d = 0;
    float ud, uq;
    float sinx_, cosx_;
    float theta_;
    mcl_control_svpwm_duty_t duty;

    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    theta =  hpm_mcl_path_get_current_theta(loop->path);
    theta_ = MCL_ANGLE_MOD_X(0, MCL_PI * 2, (hpm_mcl_path_get_next_theta(loop->path) - (0.25 * MCL_PI)));

    if (loop->enable) {
        MCL_STATUS_SET_IF_TRUE(mcl_success != hpm_mcl_analog_get_value(loop->analog, analog_a_current, &ia), loop->status, loop_status_fail);
        MCL_STATUS_SET_IF_TRUE(mcl_success != hpm_mcl_analog_get_value(loop->analog, analog_b_current, &ib), loop->status, loop_status_fail);
        MCL_STATUS_SET_IF_TRUE(mcl_success != hpm_mcl_analog_step_convert(loop->analog, ia, analog_a_current, theta, &ia), loop->status, loop_status_fail);
        MCL_STATUS_SET_IF_TRUE(mcl_success != hpm_mcl_analog_step_convert(loop->analog, ib, analog_b_current, theta, &ib), loop->status, loop_status_fail);

        theta =  MCL_ANGLE_MOD_X(0, MCL_PI * 2, (theta - (0.25 * MCL_PI)));
        MCL_VALUE_SET_IF_TRUE(loop->ref_id.enable, ref_d, loop->ref_id.value);
        alpha = ia;
        beta = ib;
        sinx = loop->control->method.sin_x(theta);
        cosx = loop->control->method.cos_x(theta);
        loop->control->method.park(alpha, beta, sinx, cosx, &sens_d, &sens_q);
        loop->control->method.currentd_pid(ref_d, sens_d, &loop->control->cfg->currentd_pid_cfg, &ud);
        loop->control->method.currentq_pid(0, sens_q, &loop->control->cfg->currentq_pid_cfg, &uq);
        sinx_ = loop->control->method.sin_x(theta_);
        cosx_ = loop->control->method.cos_x(theta_);
        loop->control->method.invpark(ud, uq, sinx_, cosx_, &alpha, &beta);
        loop->control->method.step_svpwm(alpha, beta, *loop->const_vbus, &duty);
        hpm_mcl_drivers_update_step_duty(loop->drivers, duty.a0, duty.a1, duty.b0, duty.b1);
    } else {
        duty.a0 = 0;
        duty.a1 = 0;
        duty.b0 = 0;
        duty.b1 = 0;
        hpm_mcl_drivers_update_step_duty(loop->drivers, duty.a0, duty.a1, duty.b0, duty.b1);
    }
    return  mcl_success;
}

hpm_mcl_stat_t hpm_mcl_loop(mcl_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    switch (loop->cfg->mode) {
    case mcl_mode_foc:
    case mcl_mode_hardware_foc:
        return hpm_mcl_bldc_loop(loop);
    case mcl_mode_block:
        MCL_ASSERT_EXEC_CODE_AND_RETURN(false, loop->status = loop_status_fail, mcl_in_development);
        break;
    case mcl_mode_step_foc:
        return hpm_mcl_step_loop(loop);
        break;
    default:
        MCL_ASSERT_EXEC_CODE_AND_RETURN(false, loop->status = loop_status_fail, mcl_fail);
        break;
    }
    return mcl_fail;
}
