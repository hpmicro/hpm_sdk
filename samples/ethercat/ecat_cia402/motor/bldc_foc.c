/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif
#include "hpm_trgm_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_QEI)
#include "hpm_qei_drv.h"
#endif
#include "hpm_gptmr_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_QEIV2)
#include "hpm_qeiv2_drv.h"
#endif
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_adc.h"
#include "hpm_mcl_loop.h"
#include "hpm_mcl_abz.h"
#include "hpm_mcl_detect.h"

#if defined(HPMSOC_HAS_HPMSDK_VSC) && defined(HPMSOC_HAS_HPMSDK_CLC) && defined(HPMSOC_HAS_HPMSDK_QEOV2)
#define HW_CURRENT_FOC_ENABLE   1
#endif

#if defined(HW_CURRENT_FOC_ENABLE)
#include "hpm_vsc_drv.h"
#include "hpm_clc_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_qeov2_drv.h"
#include "hpm_synt_drv.h"
#endif
#include "bldc_foc.h"

#define MOTOR0_SPD                  (20.0)  /*r/s   delta:0.1r/s    1-40r/s */
#define CURRENT_SET_TIME_MS    (200)
#define SPEED_MAX              (40)
#define PWM_FREQUENCY               (20000)
#define PWM_RELOAD                  ((motor_clock_hz/PWM_FREQUENCY) - 1)
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM
#define PWM_DEAD_AREA_TICK   (100)
#define MOTOR0_CURRENT_LOOP_BANDWIDTH (500)
#define ADCU_INDEX 0
#define ADCV_INDEX 1

volatile ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES];
int32_t motor_clock_hz;
float abs_position_theta;
adc_type hpm_adc_u = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_U_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_U_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};
adc_type hpm_adc_v = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_V_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_V_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};

typedef struct {
    mcl_encoder_t encoder;
    mcl_filter_iir_df1_t encoder_iir;
    mcl_filter_iir_df1_memory_t encoder_iir_mem[2];
    mcl_analog_t analog;
    mcl_drivers_t drivers;
    mcl_control_t control;
    mcl_loop_t loop;
    mcl_detect_t detect;
    struct {
        mcl_cfg_t mcl;
        mcl_encoer_cfg_t encoder;
        mcl_filter_iir_df1_cfg_t encoder_iir;
        mcl_filter_iir_df1_matrix_t encoder_iir_mat[2];
        mcl_analog_cfg_t analog;
        mcl_drivers_cfg_t drivers;
        mcl_control_cfg_t control;
        mcl_loop_cfg_t loop;
        mcl_detect_cfg_t detect;
        mcl_hardware_clc_cfg_t clc;
    } cfg;
} motor0_t;

motor0_t motor0;
uint32_t adc_u_midpoint, adc_v_midpoint;
mcl_user_value_t user_set_theta;

hpm_mcl_stat_t enable_all_pwm_output(void);
hpm_mcl_stat_t disable_all_pwm_output(void);
hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty);
void pwm_init(void);
hpm_mcl_stat_t qei_init(void);
hpm_mcl_stat_t adc_init(void);
hpm_mcl_stat_t adc_value_get(mcl_analog_chn_t chn, int32_t *value);
hpm_mcl_stat_t encoder_get_theta(float *theta);
hpm_mcl_stat_t encoder_get_abs_theta(float *theta);
#if defined(HW_CURRENT_FOC_ENABLE)
void motor0_clc_set_currentloop_value(mcl_loop_chn_t chn, int32_t val);
int32_t motor0_clc_float_convert_clc(float realdata);
#endif
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
    motor0.cfg.mcl.physical.board.analog[analog_a_current].adc_reference_vol = 3.3;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].opamp_gain = 10;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].sample_precision = 4095;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].sample_res = 0.01;

    motor0.cfg.mcl.physical.board.analog[analog_b_current].adc_reference_vol = 3.3;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].opamp_gain = 10;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].sample_precision = 4095;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].sample_res = 0.01;

    motor0.cfg.mcl.physical.board.num_current_sample_res = 2;
    motor0.cfg.mcl.physical.board.pwm_dead_time_tick = PWM_DEAD_AREA_TICK;
    motor0.cfg.mcl.physical.board.pwm_frequency = PWM_FREQUENCY;
    motor0.cfg.mcl.physical.board.pwm_reload = PWM_RELOAD;
    motor0.cfg.mcl.physical.motor.i_max = 9;
    motor0.cfg.mcl.physical.motor.inertia = 0.075;
    motor0.cfg.mcl.physical.motor.ls = 0.00263;
    motor0.cfg.mcl.physical.motor.pole_num = 2;
    motor0.cfg.mcl.physical.motor.power = 50;
    motor0.cfg.mcl.physical.motor.res = 0.0011;
    motor0.cfg.mcl.physical.motor.rpm_max = 3500;
    motor0.cfg.mcl.physical.motor.vbus = 24;
    motor0.cfg.mcl.physical.motor.flux = 0.0015;
    motor0.cfg.mcl.physical.motor.ld = 0.0026;
    motor0.cfg.mcl.physical.motor.lq = 0.0026;
    motor0.cfg.mcl.physical.time.adc_sample_ts = 1.0f / (PWM_FREQUENCY);
    motor0.cfg.mcl.physical.time.current_loop_ts = (1.0f / (PWM_FREQUENCY));
    motor0.cfg.mcl.physical.time.encoder_process_ts = (1.0f / (PWM_FREQUENCY));
    motor0.cfg.mcl.physical.time.speed_loop_ts = (1.0f / (PWM_FREQUENCY)) * 5;
    motor0.cfg.mcl.physical.time.position_loop_ts = (1.0f / (PWM_FREQUENCY)) * 20;
    motor0.cfg.mcl.physical.time.mcu_clock_tick = clock_get_frequency(clock_cpu0);
    motor0.cfg.mcl.physical.time.pwm_clock_tick = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);

    motor0.cfg.analog.enable_a_current = true;
    motor0.cfg.analog.enable_b_current = true;
    memset(motor0.cfg.analog.enable_filter, false, MCL_ANALOG_CHN_NUM);
    motor0.cfg.analog.enable_vbus = true;

    motor0.cfg.encoder.communication_interval_us = 0;
    motor0.cfg.encoder.disable_start_sample_interrupt = true;
    motor0.cfg.encoder.period_call_time_s = 1.0f / (PWM_FREQUENCY);
    motor0.cfg.encoder.precision = BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV;
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
    motor0.cfg.control.currentd_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.currentd_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.currentd_pid_cfg.cfg.output_max = 15;
    motor0.cfg.control.currentd_pid_cfg.cfg.output_min = -15;
    motor0.cfg.control.currentd_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_max = 15;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_min = -15;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;

    motor0.cfg.control.dead_area_compensation_cfg.cfg.lowpass_k = 0.1;

#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 20;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -20;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 10;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -10;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.01;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.001;
#else
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.0074;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.0001;
#endif

    motor0.cfg.control.position_pid_cfg.cfg.integral_max = 10;
    motor0.cfg.control.position_pid_cfg.cfg.integral_min = -10;
    motor0.cfg.control.position_pid_cfg.cfg.output_max = 50;
    motor0.cfg.control.position_pid_cfg.cfg.output_min = -50;
#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.control.position_pid_cfg.cfg.kp = 34.7;
#else
    motor0.cfg.control.position_pid_cfg.cfg.kp = 154.7;
#endif
    motor0.cfg.control.position_pid_cfg.cfg.ki = 0.113;

    motor0.cfg.drivers.callback.init = pwm_init;
    motor0.cfg.drivers.callback.enable_all_drivers = enable_all_pwm_output;
    motor0.cfg.drivers.callback.disable_all_drivers = disable_all_pwm_output;
    motor0.cfg.drivers.callback.update_duty_cycle = pwm_duty_set;
    motor0.cfg.analog.callback.init = adc_init;
    motor0.cfg.analog.callback.update_sample_location = analog_update_sample_location;
    motor0.cfg.analog.callback.get_value = adc_value_get;
#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.encoder.callback.init = NULL;
#else
    motor0.cfg.encoder.callback.init = qei_init;
#endif
    motor0.cfg.encoder.callback.start_sample = encoder_start_sample;
    motor0.cfg.encoder.callback.get_theta = encoder_get_theta;
    motor0.cfg.encoder.callback.get_absolute_theta = encoder_get_abs_theta;

#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.clc.clc_set_val = motor0_clc_set_currentloop_value;
    motor0.cfg.clc.convert_float_to_clc_val = motor0_clc_float_convert_clc;
    motor0.loop.hardware = &motor0.cfg.clc;
#endif

#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.loop.mode = mcl_mode_hardware_foc;
#else
    motor0.cfg.loop.mode = mcl_mode_foc;
#endif
    motor0.cfg.loop.enable_speed_loop = true;

    motor0.cfg.detect.enable_detect = true;
    motor0.cfg.detect.en_submodule_detect.analog = true;
    motor0.cfg.detect.en_submodule_detect.drivers = true;
    motor0.cfg.detect.en_submodule_detect.encoder = true;
    motor0.cfg.detect.en_submodule_detect.loop = true;
    motor0.cfg.detect.callback.disable_output = disable_all_pwm_output;

    hpm_mcl_analog_init(&motor0.analog, &motor0.cfg.analog, &motor0.cfg.mcl);
    hpm_mcl_filter_iir_df1_init(&motor0.encoder_iir, &motor0.cfg.encoder_iir, &motor0.encoder_iir_mem[0]);
    hpm_mcl_encoder_init(&motor0.encoder, &motor0.cfg.mcl, &motor0.cfg.encoder, &motor0.encoder_iir);
    hpm_mcl_drivers_init(&motor0.drivers, &motor0.cfg.drivers);
    hpm_mcl_control_init(&motor0.control, &motor0.cfg.control);
    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                    &motor0.encoder, &motor0.analog, &motor0.control, &motor0.drivers, NULL);
    hpm_mcl_detect_init(&motor0.detect, &motor0.cfg.detect, &motor0.loop, &motor0.encoder, &motor0.analog, &motor0.drivers);
    hpm_mcl_enable_dq_axis_decoupling(&motor0.loop);
    hpm_mcl_enable_dead_area_compensation(&motor0.loop);
}

void motor0_speed_loop_para_init(void)
{

#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 20;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -20;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 10;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -10;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.01;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.001;
#else
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 10;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -10;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.5;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.01;

    hpm_mcl_disable_dead_area_compensation(&motor0.loop);

    motor0.cfg.control.currentd_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
#endif
}

void motor0_position_loop_para_init(void)
{

#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 20;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -20;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 10;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -10;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.01;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.001;
#else
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.05;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.001;

    hpm_mcl_enable_dead_area_compensation(&motor0.loop);

    motor0.cfg.control.currentd_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5f;
#endif
}

hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty)
{
#if !defined(HW_CURRENT_FOC_ENABLE)
    uint32_t pwm_reload;
    uint32_t pwm_cmp_half, pwm_reload_half;
    uint32_t index0, index1;

    pwm_reload = (int32_t)(PWM_RELOAD * 0.98f);
    pwm_cmp_half = (uint32_t)(duty * pwm_reload) >> 1;
    pwm_reload_half =  PWM_RELOAD >> 1;
    switch (chn) {
    case mcl_drivers_chn_a:
            index0 = BOARD_BLDCPWM_CMP_INDEX_0;
            index1 = BOARD_BLDCPWM_CMP_INDEX_1;
        break;
    case mcl_drivers_chn_b:
            index0 = BOARD_BLDCPWM_CMP_INDEX_2;
            index1 = BOARD_BLDCPWM_CMP_INDEX_3;
        break;
    case mcl_drivers_chn_c:
            index0 = BOARD_BLDCPWM_CMP_INDEX_4;
            index1 = BOARD_BLDCPWM_CMP_INDEX_5;
        break;
    default:
        return mcl_fail;
    }
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_cmp_force_value(MOTOR0_BLDCPWM, index0, PWM_CMP_CMP_SET((pwm_reload_half - pwm_cmp_half)));
    pwm_cmp_force_value(MOTOR0_BLDCPWM, index1, PWM_CMP_CMP_SET((pwm_reload_half + pwm_cmp_half)));
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (index0 + 1), (pwm_reload_half - pwm_cmp_half), 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (index1 + 1), (pwm_reload_half + pwm_cmp_half), 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif
#else
    (void)chn;
    (void)duty;
#endif
    return mcl_success;
}

hpm_mcl_stat_t encoder_get_theta(float *theta)
{
#ifdef HPMSOC_HAS_HPMSDK_QEI
    return hpm_mcl_abz_get_theta(BOARD_BLDC_QEI_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,\
        -((MCL_PI * 2) / BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV), theta);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    return hpm_mcl_abz_get_theta(BOARD_BLDC_QEIV2_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,\
        -((MCL_PI * 2) / BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV), theta);
#endif

}

hpm_mcl_stat_t encoder_get_abs_theta(float *theta)
{
#ifdef HPMSOC_HAS_HPMSDK_QEI
    return hpm_mcl_abz_get_abs_theta(BOARD_BLDC_QEI_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,\
        ((MCL_PI * 2) / BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV), -abs_position_theta, theta);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    return hpm_mcl_abz_get_abs_theta(BOARD_BLDC_QEIV2_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,\
        ((MCL_PI * 2) / BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV), -abs_position_theta, theta);
#endif
}

hpm_mcl_stat_t adc_value_get(mcl_analog_chn_t chn, int32_t *value)
{
    int32_t sens_value;

    switch (chn) {
    case analog_a_current:
        sens_value = MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[ADCU_INDEX][BOARD_BLDC_ADC_TRG*4]);
        *value = sens_value - adc_u_midpoint;
        break;
    case analog_b_current:
        sens_value = MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[ADCV_INDEX][BOARD_BLDC_ADC_TRG*4]);
        *value = sens_value - adc_v_midpoint;
        break;
    default:
        return mcl_fail;
    }

    return mcl_success;
}

void reset_pwm_counter(void)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)

#endif
}

hpm_mcl_stat_t enable_all_pwm_output(void)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_disable_sw_force(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_disable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_disable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_disable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_disable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_disable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_disable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
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
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif
    return mcl_success;
}
#if defined(HPMSOC_HAS_HPMSDK_PWM)
void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_pair_config_t pwm_pair_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    pwm_deinit(MOTOR0_BLDCPWM);
    pwm_stop_counter(MOTOR0_BLDCPWM);
    reset_pwm_counter();
    pwm_set_reload(MOTOR0_BLDCPWM, 0, PWM_RELOAD);
    pwm_set_start_count(MOTOR0_BLDCPWM, 0, 0);
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = PWM_RELOAD + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = PWM_RELOAD + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].enable_ex_cmp  = false;
    cmp_config[2].mode           = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = 5;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = PWM_RELOAD;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_get_default_pwm_pair_config(MOTOR0_BLDCPWM, &pwm_pair_config);
    pwm_pair_config.pwm[0].enable_output = true;
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;
    pwm_pair_config.pwm[1].invert_output = false;

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_pair_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_pair_config, cmp_index+2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_pair_config, cmp_index+4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    pwm_load_cmp_shadow_on_match(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP,  &cmp_config[3]);

    pwm_config_cmp(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &cmp_config[2]);

    pwm_start_counter(MOTOR0_BLDCPWM);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)

#if defined(HW_CURRENT_FOC_ENABLE)

void pwm_init(void)
{
    pwmv2_cmp_config_t cmp_cfg[2];
    pwmv2_pair_config_t pwm_cfg;
    pwmv2_cmp_calculate_cfg_t cal;

    cmp_cfg[0].cmp = PWM_RELOAD;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = cmp_value_from_calculate;
    cmp_cfg[0].cmp_source_index = PWMV2_CALCULATE_INDEX(0);
    cmp_cfg[0].update_trigger = pwm_shadow_register_update_on_reload;
    cmp_cfg[1].cmp = PWM_RELOAD;
    cmp_cfg[1].enable_half_cmp = false;
    cmp_cfg[1].enable_hrcmp = false;
    cmp_cfg[1].cmp_source = cmp_value_from_calculate;
    cmp_cfg[1].cmp_source_index = PWMV2_CALCULATE_INDEX(1);
    cmp_cfg[1].update_trigger = pwm_shadow_register_update_on_reload;

    pwm_cfg.pwm[0].enable_output = true;
    pwm_cfg.pwm[0].enable_async_fault = false;
    pwm_cfg.pwm[0].enable_sync_fault = false;
    pwm_cfg.pwm[0].invert_output = false;
    pwm_cfg.pwm[0].enable_four_cmp = false;
    pwm_cfg.pwm[0].update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.pwm[0].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;
    pwm_cfg.pwm[1].enable_output = true;
    pwm_cfg.pwm[1].enable_async_fault = false;
    pwm_cfg.pwm[1].enable_sync_fault = false;
    pwm_cfg.pwm[1].invert_output = false;
    pwm_cfg.pwm[1].enable_four_cmp = false;
    pwm_cfg.pwm[1].update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.pwm[1].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;

    pwmv2_disable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_reset_counter(MOTOR0_BLDCPWM, pwm_counter_0);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(0), PWM_RELOAD, 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(1), 1, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    pwmv2_setup_waveform_in_pair(MOTOR0_BLDCPWM, pwm_channel_0, &pwm_cfg, PWMV2_CMP_INDEX(0), &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_CALCULATE_INDEX(2);
    cmp_cfg[1].cmp_source_index = PWMV2_CALCULATE_INDEX(3);
    pwmv2_setup_waveform_in_pair(MOTOR0_BLDCPWM, pwm_channel_2, &pwm_cfg, PWMV2_CMP_INDEX(4), &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_CALCULATE_INDEX(4);
    cmp_cfg[1].cmp_source_index = PWMV2_CALCULATE_INDEX(5);
    pwmv2_setup_waveform_in_pair(MOTOR0_BLDCPWM, pwm_channel_4, &pwm_cfg, PWMV2_CMP_INDEX(8), &cmp_cfg[0], 2);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_1, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_2, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(MOTOR0_BLDCPWM, 16, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_set_trigout_cmp_index(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_OUT_CHN, 16);

    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    cal.t_param = 4;
    cal.d_param = -4;
    cal.enbale_low_limit = false;
    cal.enable_up_limit = false;
    cal.counter_index = pwm_counter_0;
    cal.in_index = pwm_dac_channel_0;
    cal.in_offset_index = PWMV2_CAL_SHADOW_OFFSET_ZERO;
    pwmv2_setup_cmp_calculate(MOTOR0_BLDCPWM, PWMV2_CALCULATE_INDEX(0), &cal);
    cal.d_param = 4;
    pwmv2_setup_cmp_calculate(MOTOR0_BLDCPWM, PWMV2_CALCULATE_INDEX(1), &cal);

    cal.counter_index = pwm_counter_1;
    cal.in_index = pwm_dac_channel_1;
    cal.d_param = -4;
    pwmv2_setup_cmp_calculate(MOTOR0_BLDCPWM, PWMV2_CALCULATE_INDEX(2), &cal);
    cal.d_param = 4;
    pwmv2_setup_cmp_calculate(MOTOR0_BLDCPWM, PWMV2_CALCULATE_INDEX(3), &cal);

    cal.counter_index = pwm_counter_2;
    cal.in_index = pwm_dac_channel_2;
    cal.d_param = -4;
    pwmv2_setup_cmp_calculate(MOTOR0_BLDCPWM, PWMV2_CALCULATE_INDEX(4), &cal);
    cal.d_param = 4;
    pwmv2_setup_cmp_calculate(MOTOR0_BLDCPWM, PWMV2_CALCULATE_INDEX(5), &cal);

    /* start counter0,counter1,counter2 */
    pwmv2_enable_multi_counter_sync(MOTOR0_BLDCPWM, 0x07);
    pwmv2_start_pwm_output_sync(MOTOR0_BLDCPWM, 0x07);
}

#else
void pwm_init(void)
{
    pwmv2_cmp_config_t cmp_cfg[2];
    pwmv2_pair_config_t pwm_cfg;

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

    pwm_cfg.pwm[0].enable_output = true;
    pwm_cfg.pwm[0].enable_async_fault = false;
    pwm_cfg.pwm[0].enable_sync_fault = false;
    pwm_cfg.pwm[0].invert_output = false;
    pwm_cfg.pwm[0].enable_four_cmp = false;
    pwm_cfg.pwm[0].update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.pwm[0].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;
    pwm_cfg.pwm[1].enable_output = true;
    pwm_cfg.pwm[1].enable_async_fault = false;
    pwm_cfg.pwm[1].enable_sync_fault = false;
    pwm_cfg.pwm[1].invert_output = false;
    pwm_cfg.pwm[1].enable_four_cmp = false;
    pwm_cfg.pwm[1].update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.pwm[1].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;

    pwmv2_disable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_reset_counter(MOTOR0_BLDCPWM, pwm_counter_0);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(0), PWM_RELOAD, 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(9), 1, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    pwmv2_setup_waveform_in_pair(MOTOR0_BLDCPWM, pwm_channel_0, &pwm_cfg, PWMV2_CMP_INDEX(0), &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(3);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(4);
    pwmv2_setup_waveform_in_pair(MOTOR0_BLDCPWM, pwm_channel_2, &pwm_cfg, PWMV2_CMP_INDEX(4), &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(5);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(6);
    pwmv2_setup_waveform_in_pair(MOTOR0_BLDCPWM, pwm_channel_4, &pwm_cfg, PWMV2_CMP_INDEX(8), &cmp_cfg[0], 2);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_1, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_2, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(9));
    pwmv2_set_trigout_cmp_index(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_OUT_CHN, BOARD_BLDCPWM_CMP_TRIG_CMP);
    pwmv2_cmp_select_counter(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP, pwm_counter_0);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    /* start counter0,counter1,counter2 */
    pwmv2_enable_multi_counter_sync(MOTOR0_BLDCPWM, 0x07);
    pwmv2_start_pwm_output_sync(MOTOR0_BLDCPWM, 0x07);
}
#endif
#endif

hpm_mcl_stat_t qei_init(void)
{
#ifdef HPMSOC_HAS_HPMSDK_QEI
    trgm_output_t trgm_config = {0};
    qei_mode_config_t mode_config = {0};
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    qeiv2_mode_config_t mode_config = {0};
#endif
    init_qei_trgm_pins();
#ifdef HPMSOC_HAS_HPMSDK_QEI
    trgm_config.invert = false;
    trgm_config.input = BOARD_BLDC_QEI_TRGM_QEI_A_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &trgm_config);
    trgm_config.input = BOARD_BLDC_QEI_TRGM_QEI_B_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_B, &trgm_config);

    mode_config.work_mode = qei_work_mode_abz;
    mode_config.z_count_inc_mode = qei_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.phcnt_idx = 0;
    qei_config_mode(BOARD_BLDC_QEI_BASE, &mode_config);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    qeiv2_config_filter(BOARD_BLDC_QEIV2_BASE, qeiv2_filter_phase_a, false, qeiv2_filter_mode_delay, true, 100);
    qeiv2_config_filter(BOARD_BLDC_QEIV2_BASE, qeiv2_filter_phase_b, false, qeiv2_filter_mode_delay, true, 100);

    mode_config.work_mode = qeiv2_work_mode_abz;
    mode_config.spd_tmr_content_sel = qeiv2_spd_tmr_as_spd_tm;
    mode_config.z_count_inc_mode = qeiv2_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.z_cali_ignore_ab = false;
    mode_config.phcnt_idx = 0;
    qeiv2_config_mode(BOARD_BLDC_QEIV2_BASE, &mode_config);
#endif
    return mcl_success;
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    if (gptmr_check_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP))) {
        gptmr_clear_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
        hpm_mcl_detect_loop(&motor0.detect);
    }
}

static void timer_init(void)
{
    gptmr_channel_config_t config;

    clock_add_to_group(BOARD_BLDC_TMR_CLOCK, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD + 1;
    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 6);
}

void init_trigger_mux(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_BLDC_PWM_TRG_ADC;
    trgm_output_config(ptr, BOARD_BLDC_TRG_ADC, &trgm_output_cfg);
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_IRQn, isr_adc)
void isr_adc(void)
{
    uint32_t status;

    status = hpm_adc_get_status_flags(&hpm_adc_u);
    if ((status & BOARD_BLDC_ADC_TRIG_FLAG) != 0) {
        hpm_adc_clear_status_flags(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
        hpm_mcl_encoder_process(&motor0.encoder, motor0.cfg.mcl.physical.time.mcu_clock_tick / PWM_FREQUENCY);
        hpm_mcl_loop(&motor0.loop);
    }
}

void init_trigger_cfg(uint8_t trig_ch, bool inten)
{
    adc_pmt_config_t pmt_cfg;

    pmt_cfg.module = BOARD_BLDC_ADC_MODULE;
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.trig_ch   = trig_ch;
    pmt_cfg.config.adc12.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc12.inten[0] = inten;
#else
    pmt_cfg.config.adc16.trig_ch   = trig_ch;
    pmt_cfg.config.adc16.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc16.inten[0] = inten;
#endif
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_V;
    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
#else
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_V;
    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_V_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_U;
    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
#else
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_U;
    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_U_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
}

hpm_mcl_stat_t adc_init(void)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;
    cfg.module = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_default_config(&cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.config.adc12.res            = adc12_res_12_bits;
    cfg.config.adc12.conv_mode      = adc12_conv_mode_preemption;
    cfg.config.adc12.diff_sel       = adc12_sample_signal_single_ended;
    cfg.config.adc12.adc_clk_div    = adc12_clock_divider_3;
    cfg.config.adc12.sel_sync_ahb   = false;
    cfg.config.adc12.adc_ahb_en = true;

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);
#else
    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.config.adc16.res            = adc16_res_16_bits;
    cfg.config.adc16.conv_mode      = adc16_conv_mode_preemption;
    cfg.config.adc16.adc_clk_div    = adc16_clock_divider_4;
    cfg.config.adc16.sel_sync_ahb   = false;
    cfg.config.adc16.adc_ahb_en = true;

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);

#endif
    ch_cfg.module                        = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_channel_default_config(&ch_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    ch_cfg.config.adc12_ch.diff_sel      = adc12_sample_signal_single_ended;
    ch_cfg.config.adc12_ch.sample_cycle  = 15;

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_U_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_V_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_V;
    hpm_adc_channel_init(&ch_cfg);

#else
    ch_cfg.config.adc16_ch.sample_cycle  = 20;

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_U_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_V_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_V;
    hpm_adc_channel_init(&ch_cfg);

#endif

    init_trigger_mux(BOARD_BLDCPWM_TRGM);
    init_trigger_cfg(BOARD_BLDC_ADC_TRG, true);

#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC16
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_U_BASE, BOARD_BLDC_ADC_TRG);
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_V_BASE, BOARD_BLDC_ADC_TRG);
#endif

    hpm_adc_init_pmt_dma(&hpm_adc_u, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCU_INDEX]));
    hpm_adc_init_pmt_dma(&hpm_adc_v, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCV_INDEX]));
#if defined(HW_CURRENT_FOC_ENABLE)
    adc16_enable_motor(BOARD_BLDC_ADC_U_BASE);
    adc16_enable_motor(BOARD_BLDC_ADC_V_BASE);
#endif
    return mcl_success;
}

#if defined(HW_CURRENT_FOC_ENABLE)

void trigmux_init_1(void)
{
    trgm_output_t trgm_config;

    /* pwm trigout0 trig adc and vsc */
    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = BOARD_BLDC_PWM_TRG_ADC;
    trgm_output_config(HPM_TRGM0, BOARD_BLDC_TRG_ADC, &trgm_config);
    trgm_output_config(HPM_TRGM0, BOARD_BLDC_TRG_VSC, &trgm_config);
}

void trigmux_init_2(void)
{
    /* vsc adc data from adc */
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_ADC_MATRIX_TO_VSC_ADC0, BOARD_BLDC_TRGM_ADC_MATRIX_FROM_ADC_U, false);
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_ADC_MATRIX_TO_VSC_ADC1, BOARD_BLDC_TRGM_ADC_MATRIX_FROM_ADC_V, false);
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_ADC_MATRIX_TO_VSC_ADC2, BOARD_BLDC_TRGM_ADC_MATRIX_FROM_ADC_W, false);

    /* clc id/iq data from vsc */
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_ADC_MATRIX_TO_CLC_ID_ADC, BOARD_BLDC_TRGM_ADC_MATRIX_FROM_VSC_ID_ADC, false);
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_ADC_MATRIX_TO_CLC_IQ_ADC, BOARD_BLDC_TRGM_ADC_MATRIX_FROM_VSC_IQ_ADC, false);

    /* qeo vd/vq data from clc */
    trgm_dac_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_DAC_MATRIX_TO_QEO_VD_DAC, BOARD_BLDC_TRGM_DAC_MATRIX_FROM_CLC_VD_DAC, false);
    trgm_dac_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_DAC_MATRIX_TO_QEO_VQ_DAC, BOARD_BLDC_TRGM_DAC_MATRIX_FROM_CLC_VQ_DAC, false);

    /* pwm duty data from qeo */
    trgm_dac_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_DAC_MATRIX_TO_PWM_DAC0, BOARD_BLDC_TRGM_DAC_MATRIX_FROM_QEO_DAC0, false);
    trgm_dac_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_DAC_MATRIX_TO_PWM_DAC1, BOARD_BLDC_TRGM_DAC_MATRIX_FROM_QEO_DAC1, false);
    trgm_dac_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_DAC_MATRIX_TO_PWM_DAC2, BOARD_BLDC_TRGM_DAC_MATRIX_FROM_QEO_DAC2, false);
}

void trigmux_init_3(void)
{
    /* pos data from qei */
    trgm_pos_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_POS_MATRIX_TO_VSC, BOARD_BLDC_TRGM_POS_MATRIX_FROM_QEI, true);    /* position invert: motor foreward rotation, position decrease */
    trgm_pos_matrix_config(HPM_TRGM0, BOARD_BLDC_TRGM_POS_MATRIX_TO_QEO, BOARD_BLDC_TRGM_POS_MATRIX_FROM_QEI, true);
}

void vsc_init(void)
{
    vsc_config_t vsc_config;

    vsc_get_default_config(BOARD_VSC, &vsc_config);
    vsc_config.phase_mode = vsc_ab_phase;
    vsc_config.pole_pairs = motor0.cfg.mcl.physical.motor.pole_num;
    vsc_config.a_adc_config.adc_sel = vsc_sel_adc0;
    vsc_config.b_adc_config.adc_sel = vsc_sel_adc1;
    vsc_config.a_adc_config.adc_chn = BOARD_BLDC_ADC_CH_U;
    vsc_config.b_adc_config.adc_chn = BOARD_BLDC_ADC_CH_V;

    /*
    * adc software use 12bit, so need shif more four bit
    */
    vsc_config.a_adc_config.adc_offset = (adc_u_midpoint) << (16 + 4);
    vsc_config.b_adc_config.adc_offset = (adc_v_midpoint) << (16 + 4);

    vsc_config.a_data_cnt = 1;
    vsc_config.b_data_cnt = 1;

    vsc_config_init(BOARD_VSC, &vsc_config);
    vsc_set_enable(BOARD_VSC, true);
}

void clc_init(void)
{
    clc_param_config_t clc_param;
    clc_coeff_config_t clc_coeff0;
    clc_coeff_config_t clc_coeff1;
    clc_coeff_config_t clc_coeff2;

    clc_param.eadc_lowth = 0xA0000000;
    clc_param.eadc_mid_lowth = 0xD0000000;
    clc_param.eadc_mid_highth = 0x30000000;
    clc_param.eadc_highth = 0x60000000;
    clc_param._2p2z_clamp_lowth = 0x80000000;
    clc_param._2p2z_clamp_highth = 0x7FFFFFFF;
    clc_param._3p3z_clamp_lowth = 0x80000000;
    clc_param._3p3z_clamp_highth = 0x7FFFFFFF;
    clc_param.output_forbid_lowth = 0;
    clc_param.output_forbid_mid = 0;
    clc_param.output_forbid_highth = 0;
    clc_config_param(BOARD_CLC, clc_vd_chn, &clc_param);
    clc_config_param(BOARD_CLC, clc_vq_chn, &clc_param);

    clc_coeff0.b0 = 1.2;
    clc_coeff0.b1 = 0.1;
    clc_coeff0.b2 = 0.12;
    clc_coeff0.b3 = 0; /* b3 value must be between -1 ~ 1. */
    clc_coeff0.a0 = 0.1;
    clc_coeff0.a1 = 0.1;
    clc_coeff0.a2 = 0; /* a2 value must be between -1 ~ 1. */
    clc_config_coeff(BOARD_CLC, clc_vd_chn, clc_coeff_zone_0, &clc_coeff0);
    clc_config_coeff(BOARD_CLC, clc_vq_chn, clc_coeff_zone_0, &clc_coeff0);
    clc_coeff1.b0 = 1.2;
    clc_coeff1.b1 = 0.1;
    clc_coeff1.b2 = 0.12;
    clc_coeff1.b3 = 0; /* b3 value must be between -1 ~ 1. */
    clc_coeff1.a0 = 0.1;
    clc_coeff1.a1 = 0.1;
    clc_coeff1.a2 = 0; /* a2 value must be between -1 ~ 1. */
    clc_config_coeff(BOARD_CLC, clc_vd_chn, clc_coeff_zone_1, &clc_coeff1);
    clc_config_coeff(BOARD_CLC, clc_vq_chn, clc_coeff_zone_1, &clc_coeff1);
    clc_coeff2.b0 = 1.2;
    clc_coeff2.b1 = 0.1;
    clc_coeff2.b2 = 0.12;
    clc_coeff2.b3 = 0; /* b3 value must be between -1 ~ 1. */
    clc_coeff2.a0 = 0.1;
    clc_coeff2.a1 = 0.1;
    clc_coeff2.a2 = 0; /* a2 value must be between -1 ~ 1. */
    clc_config_coeff(BOARD_CLC, clc_vd_chn, clc_coeff_zone_2, &clc_coeff2);
    clc_config_coeff(BOARD_CLC, clc_vq_chn, clc_coeff_zone_2, &clc_coeff2);

    clc_set_adc_chn_offset(BOARD_CLC, clc_vd_chn, 0, 0);
    clc_set_adc_chn_offset(BOARD_CLC, clc_vq_chn, 0, 0);
    clc_set_pwm_period(BOARD_CLC, clc_vd_chn, 0);
    clc_set_pwm_period(BOARD_CLC, clc_vq_chn, 0);

    clc_set_expect_adc_value(BOARD_CLC, clc_vd_chn, 0);
    clc_set_expect_adc_value(BOARD_CLC, clc_vq_chn, 0);

    clc_set_enable(BOARD_CLC, clc_vd_chn, true);
    clc_set_enable(BOARD_CLC, clc_vq_chn, true);
}

void qeov2_init(void)
{
    qeo_wave_mode_t config;
    qeo_wave_get_default_mode_config(BOARD_BLDC_QEO, &config);
    config.dq_valid_trig_enable = true;
    config.pos_valid_trig_enable = true;
    config.vd_vq_inject_enable = true;
    config.vd_vq_from_sw = false;
    /* config.wave_type = qeo_wave_cosine; */
    config.wave_type = qeo_wave_saddle;
    config.saddle_type = qeo_saddle_standard;
    /* config.saddle_type = qeo_saddle_triple; */
    qeo_wave_config_mode(BOARD_BLDC_QEO, &config);
    qeo_wave_set_resolution_lines(BOARD_BLDC_QEO, motor0.cfg.mcl.physical.motor.pole_num);

    qeo_wave_set_phase_shift(BOARD_BLDC_QEO, 0, 180.0);
    qeo_wave_set_phase_shift(BOARD_BLDC_QEO, 1, 60.0);
    qeo_wave_set_phase_shift(BOARD_BLDC_QEO, 2, 300.0);

    qeo_wave_set_pwm_cycle(BOARD_BLDC_QEO, (PWM_RELOAD << 8));
}

void motor0_clc_set_currentloop_value(mcl_loop_chn_t chn, int32_t val)
{
    const clc_chn_t clc_chn[2] = {clc_vd_chn, clc_vq_chn};

    clc_set_expect_adc_value(BOARD_CLC, clc_chn[chn], val);
}

int32_t motor0_clc_float_convert_clc(float realdata)
{
    int32_t data0;
    double data1 = realdata;
    data0 = data1 * 0x6400000;

    return data0;
}
#endif

void adc_isr_enable(void)
{
    hpm_adc_enable_interrupts(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
    intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_IRQn, 6);
}

void motor_angle_align(void)
{
    mcl_motor_alignment_cfg_t alignment_cfg;

    /* Select three-stage alignment algorithm */
    alignment_cfg.algorithm = mcl_alignment_algorithm_three_stage;

#if defined(HW_CURRENT_FOC_ENABLE)
    /* Hardware FOC specific initialization */
    qeo_enable_software_position_inject(BOARD_BLDC_QEO);
    qeo_software_position_inject(BOARD_BLDC_QEO, 0);
    qeo_disable_software_position_inject(BOARD_BLDC_QEO);
    vsc_sw_inject_pos_value(BOARD_VSC, 0);

    /* Configure three-stage alignment parameters for hardware mode */
    alignment_cfg.config.three_stage.stage1.d_current = 8.0f;    /* High current for stage 1 */
    alignment_cfg.config.three_stage.stage1.q_current = 0.5f;    /* Q-axis disturbance for stage 1 */
    alignment_cfg.config.three_stage.stage1.delay_ms = 500;      /* Stage 1 delay time */

    alignment_cfg.config.three_stage.stage2.d_current = 5.0f;    /* Moderate current for stage 2 */
    alignment_cfg.config.three_stage.stage2.delay_ms = 800;      /* Stage 2 delay time */

    alignment_cfg.config.three_stage.stage3.d_current = 3.0f;    /* Low current for stage 3 */
    alignment_cfg.config.three_stage.stage3.delay_ms = 400;      /* Stage 3 delay time */
#else
    /* Configure three-stage alignment parameters for software mode */
    alignment_cfg.config.three_stage.stage1.d_current = 4.0f;    /* High current for stage 1 */
    alignment_cfg.config.three_stage.stage1.q_current = 0.6f;    /* Q-axis disturbance for stage 1 */
    alignment_cfg.config.three_stage.stage1.delay_ms = 500;      /* Stage 1 delay time */

    alignment_cfg.config.three_stage.stage2.d_current = 1.5f;    /* Moderate current for stage 2 */
    alignment_cfg.config.three_stage.stage2.delay_ms = 800;      /* Stage 2 delay time */

    alignment_cfg.config.three_stage.stage3.d_current = 1.0f;    /* Low current for stage 3 */
    alignment_cfg.config.three_stage.stage3.delay_ms = 400;      /* Stage 3 delay time */
#endif

    alignment_cfg.config.three_stage.final_delay_ms = 100;       /* Final delay */

    /* Call the enhanced alignment function from middleware */
    hpm_mcl_motor_angle_alignment(&motor0.loop, &alignment_cfg);

#if defined(HW_CURRENT_FOC_ENABLE)
    /* Hardware FOC specific post-alignment setup */
    qei_init();
    trigmux_init_3();
#endif
}

void motor_adc_midpoint(void)
{
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint32_t times = 0;

    do {
        adc_u_sum += MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[0][BOARD_BLDC_ADC_TRG*4]);
        adc_v_sum += MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[1][BOARD_BLDC_ADC_TRG*4]);
        times++;
        board_delay_ms(1);
        if (times >= CURRENT_SET_TIME_MS) {
            break;
        }
    } while (1);
    adc_u_midpoint = adc_u_sum / CURRENT_SET_TIME_MS;
    adc_v_midpoint = adc_v_sum / CURRENT_SET_TIME_MS;
}

void mcl_user_delay_us(uint64_t tick)
{
    board_delay_us(tick);
}

#define TEST_PWM_CONTROL 1
#define CIA402_PSITION_COEFFICIENT (10000)
#define CIA402_SPEED_COEFFICIENT (0.10922)
mcl_user_value_t motor_speed;
mcl_user_value_t motor_position;

void motor_function_init(void)
{
    init_adc_bldc_pins();
    init_pwm_pins(MOTOR0_BLDCPWM);
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    motor_init();
    adc_isr_enable();
    timer_init();
    motor_adc_midpoint();
    hpm_mcl_loop_enable(&motor0.loop);
#if defined(HW_CURRENT_FOC_ENABLE)
    trigmux_init_1();
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
    vsc_init();
    clc_init();
    qeov2_init();
    trigmux_init_2();
#endif
    motor_angle_align();
}

void motor_speed_loop_init(void)
{
    if (motor_position.enable) {
        motor_position.enable = false;
    }
    motor_speed.enable = true;
    motor0_speed_loop_para_init();
}

void motor_speed_loop_set(int32_t target_speed)
{
    motor_speed.value = (float)target_speed * CIA402_SPEED_COEFFICIENT * MCL_2PI;
    hpm_mcl_loop_set_speed(&motor0.loop, motor_speed);
}

void motor_postion_loop_init(void)
{
    if (motor_speed.enable) {
        motor_speed.enable = false;
        hpm_mcl_loop_set_speed(&motor0.loop, motor_speed);
    }
    motor_position.enable = true;
    motor0_position_loop_para_init();
    hpm_mcl_enable_position_loop(&motor0.loop);
}

void motor_position_loop_set(int32_t target_position)
{
    motor_position.value = (float)target_position / CIA402_PSITION_COEFFICIENT * MCL_2PI;
    hpm_mcl_loop_set_position(&motor0.loop, motor_position);
}

int32_t motor_get_actual_speed(void)
{
    return (int32_t)(motor0.encoder.result.speed / MCL_2PI / CIA402_SPEED_COEFFICIENT);
}

int32_t motor_get_actual_position(void)
{
    float position;
    hpm_mcl_encoder_get_absolute_theta(&motor0.encoder, &position);

    return (int32_t)(position / MCL_2PI * CIA402_PSITION_COEFFICIENT);;
}

void motor_stop(void)
{
    if (motor_speed.enable) {
        motor_speed.value = 0;
        hpm_mcl_loop_set_speed(&motor0.loop, motor_speed);
    } else if (motor_position.enable) {
        float position;
        hpm_mcl_encoder_get_absolute_theta(&motor0.encoder, &position);
        motor_position.value = position;
        hpm_mcl_loop_set_position(&motor0.loop, motor_position);
    }
}

void motor_enable(void)
{
#if TEST_PWM_CONTROL
    enable_all_pwm_output();
#else

#endif
}

void motor_disable(void)
{
#if TEST_PWM_CONTROL
    disable_all_pwm_output();
#else
    motor_stop();
#endif
}
