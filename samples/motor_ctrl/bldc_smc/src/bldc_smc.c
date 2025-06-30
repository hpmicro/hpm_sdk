/*
 * Copyright (c) 2023-2024 HPMicro
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
#include "hpm_gptmr_drv.h"

#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"

#include "hpm_bldc_define.h"
#include "bldc_smc_cfg.h"
#include "hpm_foc.h"
#include "hpm_smc.h"

#include "hpm_gpio_drv.h"
#include "hpm_adc.h"

#ifndef SEGGER_RTT_ENABLE
#define SEGGER_RTT_ENABLE 0
#endif

#if SEGGER_RTT_ENABLE
#include "SEGGER_RTT.h"
#endif


int32_t motor_clock_hz;

#define PWM_FREQUENCY               (20000)
#define PWM_RELOAD                  ((motor_clock_hz/PWM_FREQUENCY) - 1)

#define MOTOR0_SPD                  (15)
#define BLDC_ANGLE_SET_TIME_MS      (2000)
#define BLDC_CURRENT_SET_TIME_MS    (200)
#define SENSORLESS_TMR_RELOAD       (BOARD_BLDC_TMR_RELOAD / 10)

#if (BLDC_CURRENT_SET_TIME_MS > 250)
#error "BLDC_CURRENT_SET_TIME_MS must be smaller than 250"
#endif

volatile ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES];

void motor0_highspeed_loop(void);

uint8_t is_being_started;
float user_setspeed = MOTOR0_SPD;
volatile uint8_t timer_flag;
int16_t drag_angle;

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

typedef struct motor0_par {
    BLDC_CONTROL_FOC_PARA       foc_para;
    hpm_mcl_para_t       smc_para;
    hpm_smc_pll_para_t          smc_pll;
    BLDC_CONTRL_PID_PARA        speedloop_para;
    void (*adc_trig_event_callback)(void);
} MOTOR0_PARA;

MOTOR0_PARA motor0 = {  BLDC_CONTROL_FOC_PARA_DEFAULTS,
                        BLDC_CONTROL_SMC_PARA_DEFAULTS,
                        BLDC_CONTROL_SMC_PLL_PARA_DEFAULTS,
                        BLDC_CONTRL_PID_PARA_DEFAULTS,
                        NULL
                    };
BLDC_CONTRL_SPD_PARA        pll_speed_control;

#if SEGGER_RTT_ENABLE
int  JS_RTT_Channel;
#pragma pack(push, 1)
struct {
    float pwm_output;
    float speed_smc;
    float theta_pll;
    float theta_smc;
    float cur_u;
    float cur_v;
    uint8_t is_run;
} rtt_disp_val;
#pragma pack(pop)
#endif

void bldc_init_par(void)
{
    BLDC_CONTROL_FOC_PARA *par = &motor0.foc_para;
    par->motorpar.func_smc_const = (void(*)(void *))&hpm_mcl_smc_const_cal;
    par->motorpar.i_lstator_h = 0.00263;
    par->motorpar.i_maxspeed_rs = 35;
    par->motorpar.i_phasecur_a = 0.125;
    par->motorpar.i_phasevol_v = 24;
    par->motorpar.i_poles_n = 2;
    par->motorpar.i_rstator_ohm = 0.0011;
    par->motorpar.i_samplingper_s = 0.00005;
    par->motorpar.func_smc_const(&par->motorpar);

    pll_speed_control.i_speedacq = 20;
    pll_speed_control.i_speedfilter = 0.0075;
    pll_speed_control.i_speedlooptime_s = 0.00005*20;
    pll_speed_control.i_motorpar = &par->motorpar;
    pll_speed_control.func_getspd = (void(*)(void *))&hpm_mcl_bldc_foc_al_speed;

    par->currentdpipar.i_kp = 10;
    par->currentdpipar.i_ki = 0.01;
    par->currentdpipar.i_max = 6000;
    par->currentdpipar.func_pid = (void(*)(void *))&hpm_mcl_bldc_foc_pi_contrl;

    par->currentqpipar.i_kp = 10;
    par->currentqpipar.i_ki = 0.01;
    par->currentqpipar.i_max = 6000;
    par->currentqpipar.func_pid = (void(*)(void *))&hpm_mcl_bldc_foc_pi_contrl;

    par->pwmpar.func_spwm = (void(*)(void *))&hpm_mcl_bldc_foc_svpwm;
    par->pwmpar.i_pwm_reload_max = PWM_RELOAD*0.95;
    par->pwmpar.pwmout.func_set_pwm = (void(*)(void *))&hpm_mcl_bldc_foc_pwmset;
    par->pwmpar.pwmout.i_pwm_reload = PWM_RELOAD;
    par->pwmpar.pwmout.i_motor_id = BLDC_MOTOR0_INDEX;

    par->samplcurpar.func_sampl = (void(*)(void *))&hpm_mcl_bldc_foc_current_cal;
    par->func_dqsvpwm =  (void (*)(void *, void *, void *, void *))hpm_mcl_smc_loop;

    motor0.speedloop_para.func_pid  = (void(*)(void *))hpm_mcl_bldc_foc_pi_contrl;
    motor0.speedloop_para.i_kp      = 46;
    motor0.speedloop_para.i_ki      = 0.095;
    motor0.speedloop_para.i_max     = 3500;

    motor0.adc_trig_event_callback = &motor0_highspeed_loop;

    motor0.smc_para.zero = 0.25;
    motor0.smc_para.ksmc = 160;
    motor0.smc_para.filter_coeff  = 0.025;
    motor0.smc_para.i_motorpar = &par->motorpar;
    motor0.smc_para.ualpha = &par->ualpha;
    motor0.smc_para.ubeta = &par->ubeta;
    motor0.smc_para.ialpha = &par->ialpha;
    motor0.smc_para.ibeta = &par->ibeta;
    motor0.smc_para.func_smc = (void(*)(void *))hpm_mcl_smc_pos_cal;
    motor0.smc_pll.kp = 0.314;
    motor0.smc_pll.ki = 0.007;
    motor0.smc_pll.loop_in_sec = 0.00005;
    motor0.smc_pll.theta0 = 90;
    motor0.smc_pll.max_i = 5000;
    motor0.smc_pll.min_i = -5000;
    motor0.smc_pll.max_o = 5000;
    motor0.smc_pll.min_o = -5000;
    par->pos_estimator_par.func = motor0.smc_para.func_smc;
    par->pos_estimator_par.par = &motor0.smc_para;
}

void enable_all_pwm_output(void)
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
}

void disable_all_pwm_output(void)
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

}

#if defined(HPMSOC_HAS_HPMSDK_PWM)
void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_pair_config_t pwm_pair_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg = {0};

    pwm_stop_counter(MOTOR0_BLDCPWM);
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
    pwm_set_reload(MOTOR0_BLDCPWM, 0, PWM_RELOAD);
    pwm_set_start_count(MOTOR0_BLDCPWM, 0, 0);
    /*
     * config cmp1 and cmp2
     */
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
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = 100;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = 100;
    pwm_pair_config.pwm[1].invert_output = false;

    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_pair_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_pair_config, cmp_index+2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_pair_config, cmp_index+4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
    }
    pwm_load_cmp_shadow_on_match(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP,  &cmp_config[3]);
    pwm_config_cmp(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &cmp_config[2]);
    pwm_start_counter(MOTOR0_BLDCPWM);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
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
    pwm_cfg.pwm[0].dead_zone_in_half_cycle = 50;
    pwm_cfg.pwm[1].enable_output = true;
    pwm_cfg.pwm[1].enable_async_fault = false;
    pwm_cfg.pwm[1].enable_sync_fault = false;
    pwm_cfg.pwm[1].invert_output = false;
    pwm_cfg.pwm[1].enable_four_cmp = false;
    pwm_cfg.pwm[1].update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.pwm[1].dead_zone_in_half_cycle = 50;

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
    pwmv2_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);

    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_2);
}
#endif

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    static int32_t start_times = 0;
    static uint32_t timer_1ms = 0;
    static float last_set_speed = 0;

    if (gptmr_check_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP))) {
        gptmr_clear_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
        timer_1ms++;
        if (timer_1ms >= 10) {
            start_times++;
            timer_1ms = 0;
            timer_flag = !timer_flag;
            motor0.speedloop_para.target = user_setspeed;
            motor0.speedloop_para.cur = pll_speed_control.o_speedout_filter;
            motor0.speedloop_para.func_pid(&motor0.speedloop_para);
            motor0.foc_para.currentqpipar.target =  motor0.speedloop_para.outval;
            motor0.foc_para.currentdpipar.target =  0;
            /**
             * @brief Speed too low to use smc
             *
             */
            if (fabs(user_setspeed) < 5) {
                user_setspeed = 0;
                start_times = 0;
                last_set_speed = 0;
                disable_all_pwm_output();
            }
            /**
             * @brief Dragging at start-up
             *
             */
            if ((last_set_speed == 0) && (fabs(user_setspeed) >= 5)) {
                enable_all_pwm_output();
                if (start_times < 1000) {
                    is_being_started = 1;
                    motor0.foc_para.currentdpipar.target = 800;
                    if (user_setspeed > 0) {
                        motor0.speedloop_para.mem = 70;
                        drag_angle = (drag_angle + 10) % 360;
                    } else {
                        motor0.speedloop_para.mem = -70;
                        if (drag_angle > 10) {
                            drag_angle -= 10;
                        } else {
                            drag_angle += 350;
                        }
                    }
                    motor0.foc_para.currentqpipar.target = 0;
                    motor0.foc_para.currentdpipar.mem = 0;
                    motor0.foc_para.currentqpipar.mem = 0;
                } else {
                    start_times = 0;
                    last_set_speed = user_setspeed;
                    is_being_started = 0;
                }
                start_times++;
            }
        }
#if SEGGER_RTT_ENABLE
        rtt_disp_val.pwm_output = motor0.speedloop_para.outval;
        rtt_disp_val.speed_smc = pll_speed_control.o_speedout_filter;
        rtt_disp_val.theta_pll = pll_speed_control.speedtheta;
        rtt_disp_val.cur_u = motor0.foc_para.samplcurpar.cal_u;
        rtt_disp_val.cur_v = motor0.foc_para.samplcurpar.cal_v;
        rtt_disp_val.is_run = is_being_started;
        SEGGER_RTT_Write(JS_RTT_Channel, &rtt_disp_val, sizeof(rtt_disp_val));
#endif
    }
}

static void timer_init(void)
{
    gptmr_channel_config_t config;

    clock_add_to_group(BOARD_BLDC_TMR_CLOCK, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.debug_mode = 0;
    config.reload = SENSORLESS_TMR_RELOAD + 1;
    config.cmp[0] = SENSORLESS_TMR_RELOAD;
    config.cmp[1] = config.reload;
    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);
}

void init_trigger_mux(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_BLDC_PWM_TRG_ADC;
    trgm_output_config(ptr, BOARD_BLDC_TRG_ADC, &trgm_output_cfg);
}

void motor0_current_loop(float angle)
{
    uint8_t smc_en;

    smc_en = !is_being_started;
    motor0.foc_para.samplcurpar.adc_u = GET_ADC_12BIT_VALID_DATA(adc_buff[ADCU_INDEX][BOARD_BLDC_ADC_TRG*4]);
    motor0.foc_para.samplcurpar.adc_v = GET_ADC_12BIT_VALID_DATA(adc_buff[ADCV_INDEX][BOARD_BLDC_ADC_TRG*4]);
    motor0.foc_para.electric_angle = angle;
    motor0.foc_para.func_dqsvpwm(&motor0.foc_para, &motor0.smc_para, &motor0.smc_pll, &smc_en);
    motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);
}
void motor0_highspeed_loop(void)
{
    motor0_current_loop(drag_angle);
    pll_speed_control.speedtheta = motor0.smc_pll.theta;
    pll_speed_control.func_getspd(&pll_speed_control);
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_IRQn, isr_adc)
void isr_adc(void)
{
    uint32_t status;

    status = hpm_adc_get_status_flags(&hpm_adc_u);
    if ((status & BOARD_BLDC_ADC_TRIG_FLAG) != 0) {
        hpm_adc_clear_status_flags(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
        motor0.adc_trig_event_callback();
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

    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_W_BASE;
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

void adc_init(void)
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

    /* Set DMA start address for preemption mode */
    hpm_adc_init_pmt_dma(&hpm_adc_u, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCU_INDEX]));
    hpm_adc_init_pmt_dma(&hpm_adc_v, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCV_INDEX]));
}

void set_adval_middle(void)
{
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint8_t times = 0;

    motor0.foc_para.pwmpar.pwmout.pwm_u = PWM_RELOAD >> 1;
    motor0.foc_para.pwmpar.pwmout.pwm_v = PWM_RELOAD >> 1;
    motor0.foc_para.pwmpar.pwmout.pwm_w = PWM_RELOAD >> 1;
    motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);
    do {
        if (timer_flag == 1) {
            adc_u_sum += GET_ADC_12BIT_VALID_DATA(adc_buff[ADCU_INDEX][BOARD_BLDC_ADC_TRG*4]);
            adc_v_sum += GET_ADC_12BIT_VALID_DATA(adc_buff[ADCV_INDEX][BOARD_BLDC_ADC_TRG*4]);
            times++;
            if (times >= BLDC_CURRENT_SET_TIME_MS) {
                break;
            }
            timer_flag = 0;
        }
    } while (1);
    motor0.foc_para.samplcurpar.adc_u_middle = adc_u_sum / BLDC_CURRENT_SET_TIME_MS;
    motor0.foc_para.samplcurpar.adc_v_middle = adc_v_sum / BLDC_CURRENT_SET_TIME_MS;
}

int main(void)
{
    uint8_t is_add = 1;

#if SEGGER_RTT_ENABLE
    char JS_RTT_UpBuffer[4096];
#endif

    board_init();
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    init_adc_bldc_pins();
    init_pwm_pins(MOTOR0_BLDCPWM);
    bldc_init_par();
    adc_init();
    pwm_init();
    timer_init();
#if SEGGER_RTT_ENABLE
    JS_RTT_Channel = 1;
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_F4F4F4F4F4F4U1", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#endif
    set_adval_middle();
    intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_IRQn, 1);
    hpm_adc_enable_interrupts(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
    motor0.adc_trig_event_callback = &motor0_highspeed_loop;
    board_delay_ms(1000);
    while (is_being_started) {
        ;
    }
    printf("bldc smc demo.\r\n");
    while (!is_being_started) {
        board_delay_ms(200);
        if (is_add) {
            user_setspeed += 1;
        } else {
            user_setspeed -= 1;
        }
        if (user_setspeed >= 40) {
            is_add = 0;
        }
        if (user_setspeed <= 15) {
            is_add = 1;
        }
    }
}
