/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <math.h>
#include "hpm_mcl_encoder.h"
#include "hpm_mcl_physical.h"

hpm_mcl_stat_t hpm_mcl_encoder_init(mcl_encoder_t *encoder, mcl_cfg_t *mcl_cfg, mcl_encoer_cfg_t *encoder_cfg, mcl_filter_iir_df1_t *iir)
{
    MCL_ASSERT(encoder != NULL, mcl_invalid_pointer);
    encoder->status = encoder_status_null;
    /**
     * @brief parameter check
     *
     */
    MCL_ASSERT(mcl_cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(encoder_cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(iir != NULL, mcl_invalid_pointer);
    MCL_ASSERT((encoder_cfg->callback.start_sample != NULL), mcl_invalid_pointer);
    MCL_ASSERT(((encoder_cfg->disable_start_sample_interrupt == true) || (encoder_cfg->callback.get_lead_tick != NULL)), mcl_invalid_pointer);
    MCL_ASSERT(((encoder_cfg->disable_start_sample_interrupt == true) || (encoder_cfg->callback.update_trig_lead_tick != NULL)), mcl_invalid_pointer);
    MCL_ASSERT((encoder_cfg->callback.get_theta != NULL), mcl_invalid_pointer);
    MCL_ASSERT((encoder_cfg->speed_cal_method != encoder_method_user) || (encoder_cfg->callback.process_by_user != NULL), mcl_invalid_argument);
    /**
     * @brief null function initialisation
     *
     */
    memset(encoder->cal_speed.memory, 0, MCL_ENCODER_CAL_STRUCT_MAX_MEMMORY * sizeof(uint32_t));
    encoder->mcu_clock_tick = &mcl_cfg->physical.time.mcu_clock_tick;
    encoder->cal_speed.pll_method.cfg = (mcl_encoder_cal_speed_pll_cfg_t *)&encoder_cfg->cal_speed_pll_cfg;
    encoder->cal_speed.pll_method.period_call_time_s = (float *)&encoder_cfg->period_call_time_s;
    encoder->iirfilter = iir;
    encoder->current_loop_ts = &mcl_cfg->physical.time.current_loop_ts;
    encoder->pole_num = &mcl_cfg->physical.motor.pole_num;
    /**
     * @brief parameter initial
     *
     */
    encoder->cfg = encoder_cfg;
    encoder->phase = &mcl_cfg->physical.motor.hall;
    encoder->result.speed = 0;
    encoder->result.theta = 0;
    encoder->result.theta_forecast = 0;
    memset(iir->mem, 0, iir->cfg->section * sizeof(mcl_filter_iir_df1_memory_t));
    encoder->force_set_theta.enable = false;
    encoder->force_set_theta.value = 0;

    encoder->status = encoder_status_init;
    if (encoder->cfg->callback.init != NULL) {
        encoder->cfg->callback.init();
    }
    MCL_ASSERT(encoder->cfg->callback.start_sample() == mcl_success, mcl_encoder_start_sample_error);
    mcl_user_delay_us(encoder->cfg->communication_interval_us * 2);
    MCL_ASSERT((encoder->cfg->callback.get_theta(&encoder->theta_initial) == mcl_success), mcl_encoder_get_theta_error);
    encoder->status = encoder_status_run;
    encoder->sensorless.enable = NULL;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_encoder_start_sample(mcl_encoder_t *encoder)
{
    uint32_t tick;

    MCL_ASSERT_EXEC_CODE_AND_RETURN_OPT(encoder != NULL, encoder->status = encoder_status_fail, mcl_invalid_pointer);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(encoder->cfg->callback.start_sample() == mcl_success,
        encoder->status = encoder_status_fail, mcl_encoder_start_sample_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(encoder->cfg->callback.get_lead_tick(&tick) == mcl_success,
        encoder->status = encoder_status_fail, mcl_encoder_get_tick_error);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(encoder->cfg->callback.update_trig_lead_tick(tick) == mcl_success,
        encoder->status = encoder_status_fail, mcl_encoder_update_tick_error);

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_encoder_set_initial_theta(mcl_encoder_t *encoder, float theta)
{
    MCL_ASSERT(encoder != NULL, mcl_invalid_pointer);
    MCL_ASSERT((encoder->status == encoder_status_run), mcl_encoder_not_ready);
    encoder->theta_initial = theta;
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_encoder_process(mcl_encoder_t *encoder, uint32_t tick_deta)
{
    float ts;
    float theta;
    float speed;
    float theta_forecast;
    float theta_integrator;
    float theta_delta;
    hpm_mcl_stat_t status;

    MCL_ASSERT_OPT(encoder != NULL, mcl_invalid_pointer);
    MCL_ASSERT(tick_deta != 0, mcl_invalid_argument);
    MCL_ASSERT((encoder->status == encoder_status_run), mcl_encoder_not_ready);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(encoder->cfg->callback.get_theta(&theta) == mcl_success,
        encoder->status = encoder_status_fail, mcl_encoder_get_theta_error);
    if ((encoder->sensorless.enable != NULL) && (*encoder->sensorless.enable == true)) {
        theta = *encoder->sensorless.theta;
    }
    if (encoder->force_set_theta.enable) {
        theta = encoder->force_set_theta.value;
    } else {
        theta = MCL_ANGLE_MOD_X(0, 2 * MCL_PI, theta - encoder->theta_initial);
    }
    switch (encoder->cfg->speed_cal_method) {
    case encoder_method_user:
        status = encoder->cfg->callback.process_by_user(theta, &speed, &theta_forecast);
        MCL_ASSERT(status == mcl_success, status);
        break;
    case encoder_method_m:
        ts = (float)tick_deta / (*encoder->mcu_clock_tick);
        theta_delta = MCL_GET_ANGLE_DELTA((theta - encoder->cal_speed.m_method.theta_last), MCL_2PI);
        if (MCL_FLOAT_IS_ZERO(theta_delta)) {
            encoder->cal_speed.m_method.ts_sigma += ts;
        } else {
            encoder->cal_speed.m_method.ts_sigma = 0;
        }
        speed = theta_delta / ts;
        if (encoder->cal_speed.m_method.ts_sigma >= encoder->cfg->timeout_s) {
            speed = 0;
            encoder->cal_speed.m_method.ts_sigma = encoder->cfg->timeout_s + 1;
        }
        encoder->cal_speed.m_method.theta_last = theta;
        break;
    case encoder_method_t:
        ts = tick_deta / (*encoder->mcu_clock_tick);
        theta_delta = MCL_GET_ANGLE_DELTA((theta - encoder->cal_speed.t_method.theta_last), MCL_2PI);
        encoder->cal_speed.t_method.ts_sigma += ts;
        if (MCL_FLOAT_IS_ZERO(theta_delta)) {
            speed = encoder->cal_speed.t_method.speed_last;
        } else {
            speed = theta_delta / encoder->cal_speed.t_method.ts_sigma;
            encoder->cal_speed.t_method.ts_sigma = 0;
        }
        if (encoder->cal_speed.t_method.ts_sigma >= encoder->cfg->timeout_s) {
            speed = 0;
            encoder->cal_speed.t_method.ts_sigma = encoder->cfg->timeout_s + 1;
        }
        encoder->cal_speed.t_method.theta_last = theta;
        encoder->cal_speed.t_method.speed_last = speed;
        break;
    case encoder_method_m_t:
        ts = tick_deta / (*encoder->mcu_clock_tick);
        theta_delta = MCL_GET_ANGLE_DELTA((theta - encoder->cal_speed.m_t_method.theta_last), MCL_2PI);
        encoder->cal_speed.m_t_method.ts_sigma += ts;
        encoder->cal_speed.m_t_method.theta_sigma += theta_delta;
        if ((float)fabs((double)encoder->cal_speed.m_t_method.speed_filter_last) > encoder->cfg->speed_abs_switch_m_t) {
            speed = encoder->cal_speed.m_t_method.theta_sigma / encoder->cal_speed.m_t_method.ts_sigma;
            encoder->cal_speed.m_t_method.ts_sigma = 0;
            encoder->cal_speed.m_t_method.theta_sigma = 0;
        } else {
            if (theta != encoder->cal_speed.m_t_method.theta_last) {
                speed = encoder->cal_speed.m_t_method.theta_sigma / encoder->cal_speed.m_t_method.ts_sigma;
                encoder->cal_speed.m_t_method.ts_sigma = 0;
                encoder->cal_speed.m_t_method.theta_sigma = 0;
            } else {
                speed = encoder->cal_speed.m_t_method.speed_last;
            }
        }
        encoder->cal_speed.m_t_method.speed_last = speed;
        encoder->cal_speed.m_t_method.theta_last = theta;
        break;
    case encoder_method_pll:
        theta_delta = sinf(theta) * cosf(encoder->cal_speed.pll_method.theta_last) - cosf(theta) * sinf(encoder->cal_speed.pll_method.theta_last);
        encoder->cal_speed.pll_method.pi_integrator += theta_delta * encoder->cal_speed.pll_method.cfg->ki;
        speed = theta_delta * encoder->cal_speed.pll_method.cfg->kp + encoder->cal_speed.pll_method.pi_integrator;
        theta_integrator = speed * (*encoder->cal_speed.pll_method.period_call_time_s) + encoder->cal_speed.pll_method.theta_last;
        theta_integrator = MCL_ANGLE_MOD_X(0, 2 * MCL_PI, theta_integrator);
        encoder->cal_speed.pll_method.theta_last = theta_integrator;
        break;
    default:
        return mcl_invalid_argument;
    }
    /**
     * @brief iir filter
     *
     */
    speed = hpm_mcl_filter_iir_df1(encoder->iirfilter, speed);
    theta = MCL_ANGLE_MOD_X(0, 2 * MCL_PI, theta * (*encoder->pole_num));
    theta_forecast = MCL_ANGLE_MOD_X(0, 2 * MCL_PI, theta + speed * (*encoder->current_loop_ts) * (*encoder->pole_num));

    encoder->result.theta = theta;
    encoder->result.speed = speed;
    encoder->result.theta_forecast = theta_forecast;
    encoder->cal_speed.m_t_method.speed_filter_last = encoder->result.speed;
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_encoder_set_status(mcl_encoder_t *encoder, mcl_encoder_status_t status)
{
    MCL_ASSERT(encoder != NULL, mcl_invalid_pointer);
    encoder->status = status;
    return mcl_success;
}

void hpm_mcl_encoder_force_theta(mcl_encoder_t *encoder, float theta, bool enable)
{
    encoder->force_set_theta.enable = enable;
    encoder->force_set_theta.value = theta;
}

float hpm_mcl_encoder_get_raw_theta(mcl_encoder_t *encoder)
{
    float theta;

    MCL_ASSERT(encoder->cfg->callback.get_theta(&theta) == mcl_success, -mcl_fail);

    return theta;
}

float hpm_mcl_encoder_get_theta(mcl_encoder_t *encoder)
{
    if ((encoder->sensorless.enable != NULL) && (*encoder->sensorless.enable == true)) {
        return *encoder->sensorless.theta;
    } else {
        return encoder->result.theta;
    }
}

float hpm_mcl_encoder_get_sensorless_theta(mcl_encoder_t *encoder)
{
    return *encoder->sensorless.theta;
}

float hpm_mcl_encoder_get_speed(mcl_encoder_t *encoder)
{
    return encoder->result.speed;
}

float hpm_mcl_encoder_get_forecast_theta(mcl_encoder_t *encoder)
{
    return encoder->result.theta_forecast;
}

hpm_mcl_stat_t hpm_mcl_encoder_get_absolute_theta(mcl_encoder_t *encoder, float *theta)
{
    MCL_ASSERT(encoder->cfg->callback.get_absolute_theta(theta) == mcl_success, mcl_encoder_get_theta_error);
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_encoder_get_uvw_status(mcl_encoder_t *encoder, mcl_encoder_uvw_level_t *level)
{
    MCL_ASSERT(encoder->cfg->callback.get_uvw_level(level) == mcl_success, mcl_encoder_get_uvw_error);
    return mcl_success;
}
