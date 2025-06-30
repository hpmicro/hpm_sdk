/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_mcl_loop.h"
#include "hpm_mcl_uvw.h"
#include "hpm_mcl_detect.h"

#define MOTOR0_SPD                  (20.0)  /* r/s   delta:0.1r/s    -40 - -5 r/s 40 - 5 r/s */
#define SPEED_MAX                   (40)
#define SPEED_MIN                   (5)
#define TIMER_TS                    (1000)   /**<  1000us */
#define BLOCK_TMR_RLD               (BOARD_BLDC_TMR_RELOAD * MCL_USEC_TO_MSEC(TIMER_TS))
#define MOTOR_POLE_NUM              (2)
#define PWM_FREQUENCY               (20000)
#define PWM_RELOAD                  ((motor_clock_hz/PWM_FREQUENCY) - 1) /*20K hz  = 200 000 000/PWM_RELOAD */
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM
#if defined(BOARD_BLDC_HALL_DIR_INV) && BOARD_BLDC_HALL_DIR_INV
#define MOTOR_ANGLE_DETA    -MCL_PI_DIV3 / MOTOR_POLE_NUM
#else
#define MOTOR_ANGLE_DETA    MCL_PI_DIV3 / MOTOR_POLE_NUM
#endif

#ifdef HPMSOC_HAS_HPMSDK_QEIV2
#define BOARD_QEI_BASE              BOARD_BLDC_QEIV2_BASE
#endif

#ifndef BOARD_BLDC_BLOCK_SPEED_KP
#define BOARD_BLDC_BLOCK_SPEED_KP (0.0005f)
#define BOARD_BLDC_BLOCK_SPEED_KI (0.000009f)
#endif

int32_t motor_clock_hz;

typedef struct {
    mcl_encoder_t encoder;
    mcl_filter_iir_df1_t encoder_iir;
    mcl_filter_iir_df1_memory_t encoder_iir_mem[2];
    mcl_drivers_t drivers;
    mcl_control_t control;
    mcl_loop_t loop;
    mcl_analog_t analog;
    mcl_detect_t detect;
    struct {
        mcl_cfg_t mcl;
        mcl_encoer_cfg_t encoder;
        mcl_filter_iir_df1_cfg_t encoder_iir;
        mcl_filter_iir_df1_matrix_t encoder_iir_mat[2];
        mcl_drivers_cfg_t drivers;
        mcl_control_cfg_t control;
        mcl_loop_cfg_t loop;
        mcl_detect_cfg_t detect;
    } cfg;
} motor0_t;

motor0_t motor0;
float encoder_theta;

hpm_mcl_stat_t enable_all_pwm_output(void);
hpm_mcl_stat_t disable_all_pwm_output(void);
hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty);
hpm_mcl_stat_t pwm_enable_channel(mcl_drivers_channel_t chn);
hpm_mcl_stat_t pwm_disable_channel(mcl_drivers_channel_t chn);
void pwm_init(void);
hpm_mcl_stat_t encoder_init(void);
hpm_mcl_stat_t encoder_get_theta(float *theta);
hpm_mcl_stat_t encoder_get_abs_theta(float *theta);
hpm_mcl_stat_t encoder_get_uvw_level(mcl_encoder_uvw_level_t *level);

void motor0_control_init(void)
{

}

hpm_mcl_stat_t analog_update_sample_location(mcl_analog_chn_t chn, uint32_t tick)
{
    (void)chn;
    (void)tick;

    return mcl_success;
}

hpm_mcl_stat_t encoder_start_sample(void)
{
    return mcl_success;
}

void motor_init(void)
{
    motor0.cfg.mcl.physical.board.num_current_sample_res = 2;
    motor0.cfg.mcl.physical.board.pwm_dead_time_tick = 0;
    motor0.cfg.mcl.physical.board.pwm_frequency = PWM_FREQUENCY;
    motor0.cfg.mcl.physical.board.pwm_reload = PWM_RELOAD;
    motor0.cfg.mcl.physical.motor.i_max = 9;
    motor0.cfg.mcl.physical.motor.inertia = 0.075;
    motor0.cfg.mcl.physical.motor.ls = 0.00263;
    motor0.cfg.mcl.physical.motor.pole_num = MOTOR_POLE_NUM;
    motor0.cfg.mcl.physical.motor.power = 50;
    motor0.cfg.mcl.physical.motor.res = 0.0011;
    motor0.cfg.mcl.physical.motor.rpm_max = 3500;
    motor0.cfg.mcl.physical.motor.vbus = 24;
    motor0.cfg.mcl.physical.motor.hall = phase_120;
    motor0.cfg.mcl.physical.time.encoder_process_ts = MCL_USEC_TO_SEC(TIMER_TS);
    motor0.cfg.mcl.physical.time.speed_loop_ts = MCL_USEC_TO_SEC(TIMER_TS) * 2;
    motor0.cfg.mcl.physical.time.mcu_clock_tick = clock_get_frequency(clock_cpu0);
    motor0.cfg.mcl.physical.time.pwm_clock_tick = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);

    motor0.cfg.encoder.communication_interval_us = 0;
    motor0.cfg.encoder.disable_start_sample_interrupt = true;
    motor0.cfg.encoder.period_call_time_s = MCL_USEC_TO_SEC(TIMER_TS);
    motor0.cfg.encoder.precision = 6;
    motor0.cfg.encoder.speed_abs_switch_m_t = 5;
    motor0.cfg.encoder.speed_cal_method = encoder_method_m;
    motor0.cfg.encoder.timeout_s = 0.5;

    /**
     * @brief loop pass fpass 100 fstop 2000
     *
     */
    motor0.cfg.encoder_iir.section = 2;
    motor0.cfg.encoder_iir.matrix = motor0.cfg.encoder_iir_mat;
    motor0.cfg.encoder_iir_mat[0].a1 = -1.947404031871316831825424742419272661209f;
    motor0.cfg.encoder_iir_mat[0].a2 = 0.95152023575172306468772376319975592196f;
    motor0.cfg.encoder_iir_mat[0].b0 = 1;
    motor0.cfg.encoder_iir_mat[0].b1 = 2;
    motor0.cfg.encoder_iir_mat[0].b2 = 1;
    motor0.cfg.encoder_iir_mat[0].scale = 0.001029050970101526990552187612593115773f;

    motor0.cfg.encoder_iir_mat[1].a1 = -1.88285893096534651114382086234400048852f;
    motor0.cfg.encoder_iir_mat[1].a2 = 0.886838706662149367510039610351668670774f;
    motor0.cfg.encoder_iir_mat[1].b0 = 1;
    motor0.cfg.encoder_iir_mat[1].b1 = 2;
    motor0.cfg.encoder_iir_mat[1].b2 = 1;
    motor0.cfg.encoder_iir_mat[1].scale = 0.000994943924200649039424337871651005116f;

    motor0.cfg.control.callback.init = motor0_control_init;

    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 0.95;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -0.95;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 1;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -1;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = BOARD_BLDC_BLOCK_SPEED_KP;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = BOARD_BLDC_BLOCK_SPEED_KI;

    motor0.cfg.drivers.callback.init = pwm_init;
    motor0.cfg.drivers.callback.enable_all_drivers = enable_all_pwm_output;
    motor0.cfg.drivers.callback.disable_all_drivers = disable_all_pwm_output;
    motor0.cfg.drivers.callback.update_duty_cycle = pwm_duty_set;
    motor0.cfg.drivers.callback.disable_drivers = pwm_disable_channel;
    motor0.cfg.drivers.callback.enable_drivers = pwm_enable_channel;

    motor0.cfg.encoder.callback.init = encoder_init;
    motor0.cfg.encoder.callback.start_sample = encoder_start_sample;
    motor0.cfg.encoder.callback.get_theta = encoder_get_theta;
    motor0.cfg.encoder.callback.get_absolute_theta = encoder_get_abs_theta;
    motor0.cfg.encoder.callback.get_uvw_level = encoder_get_uvw_level;

    motor0.cfg.loop.mode = mcl_mode_block;
    motor0.cfg.loop.enable_speed_loop = true;

    motor0.cfg.detect.enable_detect = true;
    motor0.cfg.detect.en_submodule_detect.analog = false;
    motor0.cfg.detect.en_submodule_detect.drivers = true;
    motor0.cfg.detect.en_submodule_detect.encoder = true;
    motor0.cfg.detect.en_submodule_detect.loop = true;
    motor0.cfg.detect.callback.disable_output = disable_all_pwm_output;

    hpm_mcl_filter_iir_df1_init(&motor0.encoder_iir, &motor0.cfg.encoder_iir, &motor0.encoder_iir_mem[0]);
    hpm_mcl_encoder_init(&motor0.encoder, &motor0.cfg.mcl, &motor0.cfg.encoder, &motor0.encoder_iir);
    hpm_mcl_drivers_init(&motor0.drivers, &motor0.cfg.drivers);
    hpm_mcl_control_init(&motor0.control, &motor0.cfg.control);
    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                    &motor0.encoder, &motor0.analog, &motor0.control, &motor0.drivers, NULL);
    hpm_mcl_detect_init(&motor0.detect, &motor0.cfg.detect, &motor0.loop, &motor0.encoder, &motor0.analog, &motor0.drivers);
}

hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty)
{
    (void)chn;
    uint32_t pwm_reload;
    uint32_t pwm_cmp_half, pwm_reload_half;

    pwm_reload = PWM_RELOAD * 0.98;
    pwm_cmp_half = (uint32_t)(duty * pwm_reload) >> 1;
    pwm_reload_half =  PWM_RELOAD >> 1;
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload_half - pwm_cmp_half)));
    pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload_half + pwm_cmp_half)));
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (BOARD_BLDCPWM_CMP_INDEX_0 + 1), (pwm_reload_half - pwm_cmp_half), 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (BOARD_BLDCPWM_CMP_INDEX_1 + 1), (pwm_reload_half + pwm_cmp_half), 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif
    return mcl_success;
}

hpm_mcl_stat_t encoder_get_theta(float *theta)
{
    *theta = encoder_theta;

    return mcl_success;
}

hpm_mcl_stat_t encoder_get_abs_theta(float *theta)
{
    *theta = encoder_theta;

    return mcl_success;
}



hpm_mcl_stat_t enable_all_pwm_output(void)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_disable_sw_force(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
#endif
    return mcl_success;
}

hpm_mcl_stat_t disable_all_pwm_output(void)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_config_force_cmd_timing(MOTOR0_BLDCPWM, pwm_force_immediately);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
    pwm_set_force_output(MOTOR0_BLDCPWM,
                        PWM_FORCE_OUTPUT(BOARD_BLDC_UH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_UL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WL_PWM_OUTPIN, pwm_output_0));
    pwm_enable_sw_force(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)

    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif

    return mcl_success;
}

#if defined(HPMSOC_HAS_HPMSDK_PWM)
hpm_mcl_stat_t pwm_enable_channel(mcl_drivers_channel_t chn)
{
    switch (chn) {
    case mcl_drivers_chn_ah:
        pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_al:
        pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bh:
        pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bl:
        pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_ch:
        pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_cl:
        pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
        break;
    default:
        return mcl_invalid_argument;
        break;
    }

    return mcl_success;
}

hpm_mcl_stat_t pwm_disable_channel(mcl_drivers_channel_t chn)
{
    switch (chn) {
    case mcl_drivers_chn_ah:
        pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_al:
        pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bh:
        pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bl:
        pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_ch:
        pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_cl:
        pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
        break;
    default:
        return mcl_invalid_argument;
        break;
    }

    return mcl_success;
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
hpm_mcl_stat_t pwm_enable_channel(mcl_drivers_channel_t chn)
{
    switch (chn) {
    case mcl_drivers_chn_ah:
        pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_al:
        pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bh:
        pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bl:
        pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_ch:
        pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_cl:
        pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
        break;
    default:
        return mcl_invalid_argument;
        break;
    }

    return mcl_success;
}

hpm_mcl_stat_t pwm_disable_channel(mcl_drivers_channel_t chn)
{
    switch (chn) {
    case mcl_drivers_chn_ah:
        pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_al:
        pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bh:
        pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_bl:
        pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_ch:
        pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
        break;
    case mcl_drivers_chn_cl:
        pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
        break;
    default:
        return mcl_invalid_argument;
        break;
    }

    return mcl_success;
}
#endif

void mcl_user_delay_us(uint64_t tick)
{
    board_delay_us(tick);
}

hpm_mcl_stat_t encoder_get_uvw_level(mcl_encoder_uvw_level_t *level)
{
#ifdef HPMSOC_HAS_HPMSDK_HALL
    uint8_t hall_stat;
    /* the following u, v, w count value read out on read event at u signal toggle */
    hall_stat = hall_get_current_uvw_stat(BOARD_BLDC_HALL_BASE);
    level->w = ((hall_stat >> 0) & 0x01);
    level->v = ((hall_stat >> 1) & 0x01);
    level->u = ((hall_stat >> 2) & 0x01);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    uint32_t pos;
    const uint8_t level_tbl[6] = {5, 4, 6, 2, 3, 1};
    pos = qeiv2_get_postion(BOARD_QEI_BASE);
    pos /= QEIV2_ANGLE_PRE;
    pos = (pos) / 2;
    level->w = ((level_tbl[pos] >> 0) & 0x01);
    level->v = ((level_tbl[pos] >> 1) & 0x01);
    level->u = ((level_tbl[pos] >> 2) & 0x01);
#endif

    return mcl_success;
}

#ifdef HPMSOC_HAS_HPMSDK_HALL
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_HALL_IRQ, isr_hall)
void isr_hall(void)
{
    hall_clear_status(BOARD_BLDC_HALL_BASE, hall_get_status(BOARD_BLDC_HALL_BASE));
    hpm_mcl_uvw_get_theta(BOARD_BLDC_HALL_BASE, NULL, MOTOR_ANGLE_DETA, &encoder_theta);
    hpm_mcl_loop_refresh_block(&motor0.loop);
}
#endif

#ifdef HPMSOC_HAS_HPMSDK_QEIV2
uint8_t last_position;
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_QEIV2_IRQ, isr_qei)
void isr_qei(void)
{
    uint32_t status = qeiv2_get_status(BOARD_BLDC_QEIV2_BASE);

    qeiv2_clear_status(BOARD_BLDC_QEIV2_BASE, status);
    hpm_mcl_uvw_get_theta(BOARD_BLDC_QEIV2_BASE, &last_position, MOTOR_ANGLE_DETA, &encoder_theta);
    hpm_mcl_loop_refresh_block(&motor0.loop);
}
#endif

hpm_mcl_stat_t encoder_init(void)
{
#ifdef HPMSOC_HAS_HPMSDK_HALL
    trgm_output_t config = {0};

    encoder_theta = 0;
    init_hall_trgm_pins();

    config.invert = false;
    config.input = BOARD_BLDC_HALL_TRGM_HALL_U_SRC;
    trgm_output_config(BOARD_BLDC_HALL_TRGM, TRGM_TRGOCFG_HALL_U, &config);

    config.input = BOARD_BLDC_HALL_TRGM_HALL_V_SRC;
    trgm_output_config(BOARD_BLDC_HALL_TRGM, TRGM_TRGOCFG_HALL_V, &config);

    config.input = BOARD_BLDC_HALL_TRGM_HALL_W_SRC;
    trgm_output_config(BOARD_BLDC_HALL_TRGM, TRGM_TRGOCFG_HALL_W, &config);

    intc_m_enable_irq_with_priority(BOARD_BLDC_HALL_IRQ, 1);

    hall_counter_reset_assert(BOARD_BLDC_HALL_BASE);
    hall_phase_config(BOARD_BLDC_HALL_BASE, 1, hall_count_delay_start_after_uvw_toggle);
    hall_load_read_trigger_event_enable(BOARD_BLDC_HALL_BASE, HALL_EVENT_PHUPT_FLAG_MASK);
    hall_counter_reset_release(BOARD_BLDC_HALL_BASE);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    qeiv2_uvw_config_t uvw_config;

    last_position = 0;
    init_qeiv2_uvw_pins(BOARD_BLDC_QEIV2_BASE);
    qeiv2_reset_counter(BOARD_BLDC_QEIV2_BASE);
    qeiv2_set_work_mode(BOARD_BLDC_QEIV2_BASE, qeiv2_work_mode_uvw);
    qeiv2_config_abz_uvw_signal_edge(BOARD_BLDC_QEIV2_BASE, true, true, true, true, true);
    qeiv2_get_uvw_position_defconfig(&uvw_config);
    uvw_config.pos_opt = qeiv2_uvw_pos_opt_next;
    (void)qeiv2_config_uvw_position(BOARD_BLDC_QEIV2_BASE, &uvw_config);

#if defined(HPM_IP_FEATURE_QEIV2_SW_RESTART_TRG) && HPM_IP_FEATURE_QEIV2_SW_RESTART_TRG
    qeiv2_enable_trig_pulse0(BOARD_BLDC_QEIV2_BASE);
#endif
    qeiv2_set_pulse0_num(BOARD_BLDC_QEIV2_BASE, 1);
    qeiv2_enable_irq(BOARD_BLDC_QEIV2_BASE, QEIV2_EVENT_PULSE0_FLAG_MASK);
#if defined(HPM_IP_FEATURE_QEIV2_SW_RESTART_TRG) && HPM_IP_FEATURE_QEIV2_SW_RESTART_TRG
    qeiv2_sw_restart_pulse0(BOARD_BLDC_QEIV2_BASE);
    qeiv2_sw_restart_cycle0(BOARD_BLDC_QEIV2_BASE);
#endif
    intc_m_enable_irq_with_priority(BOARD_BLDC_QEIV2_IRQ, 1);
    qeiv2_release_counter(BOARD_BLDC_QEIV2_BASE);
#endif
    return mcl_success;
}

#if defined(HPMSOC_HAS_HPMSDK_PWM)
void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(MOTOR0_BLDCPWM);
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
    pwm_get_default_pwm_config(MOTOR0_BLDCPWM, &pwm_config);

    pwm_set_reload(MOTOR0_BLDCPWM, 0, PWM_RELOAD);
    pwm_set_start_count(MOTOR0_BLDCPWM, 0, 0);

    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = PWM_RELOAD + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = PWM_RELOAD + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_shlk;

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 100;
    pwm_config.invert_output = false;

    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }

    /*force pwm*/
    pwm_config_force_cmd_timing(MOTOR0_BLDCPWM, pwm_force_immediately);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
    pwm_set_force_output(MOTOR0_BLDCPWM,
                        PWM_FORCE_OUTPUT(BOARD_BLDC_UH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_UL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WL_PWM_OUTPIN, pwm_output_0));
    pwm_enable_sw_force(MOTOR0_BLDCPWM);
    pwm_start_counter(MOTOR0_BLDCPWM);
    pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, BOARD_BLDCPWM_CMP_INDEX_1,
                   PWM_RELOAD >> 1, PWM_RELOAD >> 1);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
void pwm_init(void)
{
    pwmv2_cmp_config_t cmp_cfg[2];
    pwmv2_config_t pwm_cfg;

    cmp_cfg[0].cmp = PWM_RELOAD;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = cmp_value_from_shadow_val;
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(1);
    cmp_cfg[0].update_trigger = pwm_shadow_register_update_on_reload;
    cmp_cfg[1].cmp = PWM_RELOAD;
    cmp_cfg[1].enable_half_cmp = false;
    cmp_cfg[1].enable_hrcmp = false;
    cmp_cfg[1].cmp_source = cmp_value_from_shadow_val;
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(2);
    cmp_cfg[1].update_trigger = pwm_shadow_register_update_on_reload;

    pwm_cfg.enable_output = true;
    pwm_cfg.enable_async_fault = false;
    pwm_cfg.enable_sync_fault = false;
    pwm_cfg.invert_output = false;
    pwm_cfg.enable_four_cmp = false;
    pwm_cfg.update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.force_shadow_trigger = pwm_force_update_shadow_immediately;
    pwm_cfg.force_trigger = pwm_force_immediately;

    pwmv2_disable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_reset_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);

    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(0), PWM_RELOAD, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    pwmv2_setup_waveform(MOTOR0_BLDCPWM, pwm_channel_0, &pwm_cfg, PWMV2_CMP_INDEX(0), &cmp_cfg[0], 2);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, pwm_channel_1, &pwm_cfg, PWMV2_CMP_INDEX(2), &cmp_cfg[0], 2);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, pwm_channel_2, &pwm_cfg, PWMV2_CMP_INDEX(4), &cmp_cfg[0], 2);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, pwm_channel_3, &pwm_cfg, PWMV2_CMP_INDEX(6), &cmp_cfg[0], 2);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, pwm_channel_4, &pwm_cfg, PWMV2_CMP_INDEX(8), &cmp_cfg[0], 2);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, pwm_channel_5, &pwm_cfg, PWMV2_CMP_INDEX(10), &cmp_cfg[0], 2);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_1, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_2, pwm_reload_update_on_reload);

    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(9), 1, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_immediately);

    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_update_shadow_immediately);

    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_output_0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
}
#endif

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    if (gptmr_check_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP))) {
        gptmr_clear_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
        hpm_mcl_detect_loop(&motor0.detect);
        hpm_mcl_encoder_process(&motor0.encoder, motor0.cfg.mcl.physical.time.mcu_clock_tick / MCL_USEC_TO_HZ(TIMER_TS));
        hpm_mcl_loop(&motor0.loop);
    }
}

static void timer_init(void)
{
    gptmr_channel_config_t config;

    clock_add_to_group(BOARD_BLDC_TMR_CLOCK, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.debug_mode = 0;
    config.reload = BLOCK_TMR_RLD + 1;
    config.cmp[0] = BLOCK_TMR_RLD;
    config.cmp[1] = config.reload;
    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);
}

int main(void)
{
    char input_data[100], input_end;
    uint8_t i;
    float speed;
    mcl_user_value_t  user_speed;

    board_init();
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    init_pwm_pins(MOTOR0_BLDCPWM);
    printf("motor test example\n");
    motor_init();
    pwm_init();
    timer_init();
    /*start motor By Hall position , Get motor step*/
    hpm_mcl_loop_start_block(&motor0.loop);
#ifdef HPMSOC_HAS_HPMSDK_HALL
    hall_irq_enable(BOARD_BLDC_HALL_BASE, HALL_EVENT_PHUPT_FLAG_MASK);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    qeiv2_enable_irq(BOARD_BLDC_QEIV2_BASE, QEIV2_EVENT_PULSE0_FLAG_MASK);
#endif
    hpm_mcl_loop_enable(&motor0.loop);
    speed = MOTOR0_SPD;
    user_speed.enable = true;
    user_speed.value = speed * MCL_2PI * MOTOR_POLE_NUM;
    hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
    printf("\r\nSpeed mode, motor run, speed is: %f.\r\nInput speed:\r\n", speed);
    while (1) {
        memset(input_data, 0, sizeof(input_data));
        input_end = 1;
        i = 0;
        while (input_end) {
            char option = getchar();

            switch (option) {
            case '\n':
            case '\r':
                input_end = 0;
                break;
            default:
                input_data[i++] = option;
                break;
            }
            if (i >= sizeof(input_data)) {
                i = 0;
                printf("Input Err. Please try again.\r\n");
                break;
            }
        }
        if (i != 0) {
            speed = atof(input_data);
            if (speed > SPEED_MAX) {
                speed = SPEED_MAX;
            } else if (speed < -SPEED_MAX) {
                speed = -SPEED_MAX;
            }
            if ((speed < SPEED_MIN) && (speed > 0)) {
                speed = SPEED_MIN;
            } else if ((speed > -SPEED_MIN) && (speed < 0)) {
                speed = -SPEED_MIN;
            }
            user_speed.value = speed * MCL_2PI * MOTOR_POLE_NUM;
            hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
            printf("\r\nSpeed mode, motor run, speed is: %f.\r\nInput speed:\r\n", speed);
        }
    }
    return 0;
}

