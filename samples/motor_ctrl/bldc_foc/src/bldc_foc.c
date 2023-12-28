/*
 * Copyright (c) 2021-2023 HPMicro
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
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_qei_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_adc12_drv.h"
#include "hpm_qeiv2_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_adc.h"
#include "hpm_mcl_loop.h"
#include "hpm_mcl_abz.h"
#include "hpm_mcl_detect.h"

#define MOTOR0_SPD                  (20.0)  /*r/s   delta:0.1r/s    1-40r/s */
#define CURRENT_SET_TIME_MS    (200)
#define SPEED_MAX              (40)
#define PWM_FREQUENCY               (20000)
#define PWM_RELOAD                  (motor_clock_hz/PWM_FREQUENCY)
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM
#define PWM_DEAD_TICK   (50)
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
    motor0.cfg.mcl.physical.board.pwm_dead_time_tick = PWM_DEAD_TICK;
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
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_max = 15;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_min = -15;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;

    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.0074;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.0001;

    motor0.cfg.control.position_pid_cfg.cfg.integral_max = 10;
    motor0.cfg.control.position_pid_cfg.cfg.integral_min = -10;
    motor0.cfg.control.position_pid_cfg.cfg.output_max = 50;
    motor0.cfg.control.position_pid_cfg.cfg.output_min = -50;
    motor0.cfg.control.position_pid_cfg.cfg.kp = 154.7;
    motor0.cfg.control.position_pid_cfg.cfg.ki = 0.113;

    motor0.cfg.drivers.callback.init = pwm_init;
    motor0.cfg.drivers.callback.enable_all_drivers = enable_all_pwm_output;
    motor0.cfg.drivers.callback.disable_all_drivers = disable_all_pwm_output;
    motor0.cfg.drivers.callback.update_duty_cycle = pwm_duty_set;
    motor0.cfg.analog.callback.init = adc_init;
    motor0.cfg.analog.callback.update_sample_location = analog_update_sample_location;
    motor0.cfg.analog.callback.get_value = adc_value_get;

    motor0.cfg.encoder.callback.init = qei_init;
    motor0.cfg.encoder.callback.start_sample = encoder_start_sample;
    motor0.cfg.encoder.callback.get_theta = encoder_get_theta;
    motor0.cfg.encoder.callback.get_absolute_theta = encoder_get_abs_theta;

    motor0.cfg.loop.mode = mcl_mode_foc;
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
}

void motor0_speed_loop_para_init(void)
{

    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 10;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -10;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.5;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.01;

    motor0.cfg.control.currentd_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = motor0.cfg.mcl.physical.motor.ls *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = motor0.cfg.mcl.physical.motor.res *
                                                (powf(MOTOR0_CURRENT_LOOP_BANDWIDTH / 100 * 2 * MCL_PI, 2)) *
                                                motor0.cfg.mcl.physical.time.current_loop_ts * 1.5;
}

hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty)
{
    uint32_t pwm_reload;
    uint32_t pwm_cmp_half, pwm_reload_half;
    uint32_t index0, index1;

    pwm_reload = PWM_RELOAD * 0.98;
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
    pwm_cmp_force_value(MOTOR0_BLDCPWM, index0, PWM_CMP_CMP_SET((pwm_reload_half - pwm_cmp_half)));
    pwm_cmp_force_value(MOTOR0_BLDCPWM, index1, PWM_CMP_CMP_SET((pwm_reload_half + pwm_cmp_half)));

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
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
}

hpm_mcl_stat_t enable_all_pwm_output(void)
{
    pwm_disable_sw_force(MOTOR0_BLDCPWM);

    return mcl_success;
}

hpm_mcl_stat_t disable_all_pwm_output(void)
{
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

    return mcl_success;
}

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
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = PWM_DEAD_TICK;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = PWM_DEAD_TICK;
    pwm_pair_config.pwm[1].invert_output = false;

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_pair_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_pair_config, cmp_index+2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_pair_config, cmp_index+4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    pwm_load_cmp_shadow_on_match(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP,  &cmp_config[3]);

    pwm_config_cmp(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &cmp_config[2]);

    pwm_start_counter(MOTOR0_BLDCPWM);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
}

hpm_mcl_stat_t qei_init(void)
{
#ifdef HPMSOC_HAS_HPMSDK_QEI
    trgm_output_t config = {0};
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    qeiv2_phcnt_cmp_match_config_t phcnt_cmp_config = {0};
#endif
    init_qei_trgm_pins();
#ifdef HPMSOC_HAS_HPMSDK_QEI
    config.invert = false;
    config.input = BOARD_BLDC_QEI_TRGM_QEI_A_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &config);
    config.input = BOARD_BLDC_QEI_TRGM_QEI_B_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_B, &config);

    qei_counter_reset_assert(BOARD_BLDC_QEI_BASE);
    qei_phase_config(BOARD_BLDC_QEI_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,
            qei_z_count_inc_on_phase_count_max, true);
    qei_phase_cmp_set(BOARD_BLDC_QEI_BASE, 4,
            false, qei_rotation_dir_cmp_ignore);
    qei_load_read_trigger_event_enable(BOARD_BLDC_QEI_BASE,
            QEI_EVENT_POSITIVE_COMPARE_FLAG_MASK);
    qei_counter_reset_release(BOARD_BLDC_QEI_BASE);
    intc_m_enable_irq_with_priority(BOARD_BLDC_QEI_IRQ, 1);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    qeiv2_reset_counter(BOARD_BLDC_QEIV2_BASE);
    qeiv2_set_work_mode(BOARD_BLDC_QEIV2_BASE, qeiv2_work_mode_abz);
    qeiv2_select_spd_tmr_register_content(BOARD_BLDC_QEIV2_BASE, qeiv2_spd_tmr_as_spd_tm);
    qeiv2_config_z_phase_counter_mode(BOARD_BLDC_QEIV2_BASE, qeiv2_z_count_inc_on_phase_count_max);
    qeiv2_config_phmax_phparam(BOARD_BLDC_QEIV2_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV);
    qeiv2_pause_pos_counter_on_fault(BOARD_BLDC_QEIV2_BASE, true);
    qeiv2_config_abz_uvw_signal_edge(BOARD_BLDC_QEIV2_BASE, true, true, false, true, true);
    phcnt_cmp_config.phcnt_cmp_value = 4;
    phcnt_cmp_config.ignore_rotate_dir = true;
    phcnt_cmp_config.ignore_zcmp = true;
    qeiv2_config_phcnt_cmp_match_condition(BOARD_BLDC_QEIV2_BASE, &phcnt_cmp_config);
    qeiv2_enable_load_read_trigger_event(BOARD_BLDC_QEIV2_BASE, QEIV2_EVENT_POSITION_COMPARE_FLAG_MASK);
    qeiv2_release_counter(BOARD_BLDC_QEIV2_BASE);
    qeiv2_set_z_phase(BOARD_BLDC_QEIV2_BASE, 0);
    qeiv2_set_phase_cnt(BOARD_BLDC_QEIV2_BASE, 0);
    intc_m_enable_irq_with_priority(BOARD_BLDC_QEIV2_IRQ, 1);
#endif
    return mcl_success;
}

void isr_gptmr(void)
{
    if (gptmr_check_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP))) {
        gptmr_clear_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
        hpm_mcl_detect_loop(&motor0.detect);
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)

static void timer_init(void)
{
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD + 1;
    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);

}

void init_trigger_mux(TRGM_Type * ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_BLDC_TRIGMUX_IN_NUM;
    trgm_output_config(ptr, BOARD_BLDC_TRG_NUM, &trgm_output_cfg);
}

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
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_IRQn, isr_adc)

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
    cfg.config.adc12.res            = adc12_res_12_bits;
    cfg.config.adc12.conv_mode      = adc12_conv_mode_preemption;
    cfg.config.adc12.adc_clk_div    = adc12_clock_divider_3;
    cfg.config.adc12.sel_sync_ahb   = false;
    cfg.config.adc12.adc_ahb_en = true;

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);
#else
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
    adc16_set_pmt_queue_enable(BOARD_BLDC_ADC_U_BASE, BOARD_BLDC_ADC_TRG, true);
    adc16_set_pmt_queue_enable(BOARD_BLDC_ADC_V_BASE, BOARD_BLDC_ADC_TRG, true);
#endif

    hpm_adc_init_pmt_dma(&hpm_adc_u, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCU_INDEX]));
    hpm_adc_init_pmt_dma(&hpm_adc_v, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCV_INDEX]));

    return mcl_success;
}

void adc_isr_enable(void)
{
    hpm_adc_enable_interrupts(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
    intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_IRQn, 1);
}

void bldc_foc_angle_align(void)
{
    mcl_user_value_t id, iq;
    id.enable = true;
    id.value = 1;
    hpm_mcl_loop_set_current_d(&motor0.loop, id);
    iq.enable = true;
    iq.value = 0;
    hpm_mcl_loop_set_current_q(&motor0.loop, iq);

    hpm_mcl_encoder_force_theta(&motor0.encoder, 0, true);
    board_delay_ms(1000);
    hpm_mcl_encoder_set_initial_theta(&motor0.encoder, hpm_mcl_encoder_get_raw_theta(&motor0.encoder));
    hpm_mcl_encoder_force_theta(&motor0.encoder, 0, false);
    board_delay_ms(100);

    id.enable = true;
    id.value = 0;
    hpm_mcl_loop_set_current_d(&motor0.loop, id);
    iq.enable = false;
    iq.value = 0;
    hpm_mcl_loop_set_current_q(&motor0.loop, iq);
}

void motor_adc_midpoint(void)
{
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint32_t times = 0;

    pwm_duty_set(mcl_drivers_chn_a, 0.5);
    pwm_duty_set(mcl_drivers_chn_b, 0.5);
    pwm_duty_set(mcl_drivers_chn_c, 0.5);
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

int main(void)
{
    char input_data[100], input_end;
    mcl_user_value_t  user_speed;
    mcl_user_value_t user_position;
    uint8_t i;
    uint8_t user_mode;
    float speed;
    int32_t position;

    board_init();
    init_adc_bldc_pins();
    init_pwm_pins(MOTOR0_BLDCPWM);
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    motor_init();
    adc_isr_enable();
    timer_init();
    motor_adc_midpoint();
    hpm_mcl_loop_enable(&motor0.loop);
    bldc_foc_angle_align();
    speed = MOTOR0_SPD;
    user_speed.enable = true;
    user_speed.value = speed * MCL_PI * 2;
    hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
    user_mode = 0;
    while (1) {
        printf("Mode selection:\r\n");
        printf("0. Location mode.\r\n");
        printf("1. Speed mode.\r\n");
        printf("Enter mode code:\r\n");
        char option = getchar();
        if (option == '0') {
            user_mode = 0;
            break;
        } else if (option == '1') {
            user_mode = 1;
            break;
        }
    }
    if (user_mode == 1) {
        motor0_speed_loop_para_init();
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
                user_speed.value = speed * MCL_PI * 2;
                hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
                printf("\r\nSpeed mode, motor run, speed is: %f.\r\nInput speed:\r\n", speed);
            }
        }
    } else if (user_mode == 0) {
        user_speed.enable = false;
        hpm_mcl_encoder_get_absolute_theta(&motor0.encoder, &abs_position_theta);
        hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
        hpm_mcl_enable_position_loop(&motor0.loop);
        user_position.enable = true;
        user_position.value = 0;
        position = 0;
        hpm_mcl_loop_set_position(&motor0.loop, user_position);
        printf("\r\nLocation mode, motor run, The location is: %d.\r\nInput Location:\r\n ", position);
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
                    printf("Input Err. Please try again.\r\n");
                    i = 0;
                    break;
                }
            }
            if (i != 0) {
                position = atoi(input_data);
                user_position.value = (float)position * 0.00157079632f;
                hpm_mcl_loop_set_position(&motor0.loop, user_position);
                printf("\r\nLocation mode, motor run, The location is: %d.\r\nInput Location:\r\n", position);
            }
        }
    }
}
