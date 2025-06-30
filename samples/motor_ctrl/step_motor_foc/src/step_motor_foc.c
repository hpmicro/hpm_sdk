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

#define CURRENT_SET_TIME_MS    (200)
#define PWM_FREQUENCY               (40000)
#define PWM_RELOAD                  ((motor_clock_hz/PWM_FREQUENCY) - 1)
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM
#define ADCU_INDEX 0
#define ADCV_INDEX 1
#define T_CURE_TIMES (1)

volatile ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES];
int32_t motor_clock_hz;

adc_type hpm_adc_a = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_U_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_U_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};
adc_type hpm_adc_b = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_V_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_V_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};

typedef struct {
    mcl_analog_t analog;
    mcl_drivers_t drivers;
    mcl_control_t control;
    mcl_loop_t loop;
    mcl_detect_t detect;
    mcl_path_plan_t path;
    struct {
        mcl_cfg_t mcl;
        mcl_analog_cfg_t analog;
        mcl_drivers_cfg_t drivers;
        mcl_control_cfg_t control;
        mcl_loop_cfg_t loop;
        mcl_detect_cfg_t detect;
        mcl_path_plan_cfg_t path;
    } cfg;
} motor0_t;

motor0_t motor0;
uint32_t adc_u_midpoint, adc_v_midpoint;
uint32_t t_cure_times;

hpm_mcl_stat_t enable_all_pwm_output(void);
hpm_mcl_stat_t disable_all_pwm_output(void);
hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty);
void pwm_init(void);
hpm_mcl_stat_t adc_init(void);
hpm_mcl_stat_t adc_value_get(mcl_analog_chn_t chn, int32_t *value);
void motor0_control_init(void)
{
}

hpm_mcl_stat_t analog_update_sample_location(mcl_analog_chn_t chn, uint32_t tick)
{
    (void)chn;
    (void)tick;
    return mcl_success;
}


void motor_init(void)
{
    motor0.cfg.mcl.physical.board.analog[analog_a_current].adc_reference_vol = 3.3;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].opamp_gain = 1;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].sample_precision = 4095;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].sample_res = 2.4;

    motor0.cfg.mcl.physical.board.analog[analog_b_current].adc_reference_vol = 3.3;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].opamp_gain = 1;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].sample_precision = 4095;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].sample_res = 2.4;

    motor0.cfg.mcl.physical.board.num_current_sample_res = 2;
    motor0.cfg.mcl.physical.board.pwm_dead_time_tick = 0;
    motor0.cfg.mcl.physical.board.pwm_frequency = PWM_FREQUENCY;
    motor0.cfg.mcl.physical.board.pwm_reload = PWM_RELOAD;
    motor0.cfg.mcl.physical.motor.i_max = 5;
    motor0.cfg.mcl.physical.motor.inertia = 0.005;
    motor0.cfg.mcl.physical.motor.ls = 0.00263;
    motor0.cfg.mcl.physical.motor.pole_num = 50;
    motor0.cfg.mcl.physical.motor.power = 20;
    motor0.cfg.mcl.physical.motor.res = 0.0011;
    motor0.cfg.mcl.physical.motor.rpm_max = 600;
    motor0.cfg.mcl.physical.motor.vbus = 24;
    motor0.cfg.mcl.physical.time.adc_sample_ts = MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY);
    motor0.cfg.mcl.physical.time.current_loop_ts = MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY);
    motor0.cfg.mcl.physical.time.mcu_clock_tick = clock_get_frequency(clock_cpu0);
    motor0.cfg.mcl.physical.time.pwm_clock_tick = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);

    motor0.cfg.analog.enable_a_current = true;
    motor0.cfg.analog.enable_b_current = true;
    memset(motor0.cfg.analog.enable_filter, false, MCL_ANALOG_CHN_NUM);
    motor0.cfg.analog.enable_vbus = true;

    motor0.cfg.control.callback.init = motor0_control_init;
    motor0.cfg.control.currentd_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.currentd_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.currentd_pid_cfg.cfg.output_max = 15;
    motor0.cfg.control.currentd_pid_cfg.cfg.output_min = -15;
    motor0.cfg.control.currentd_pid_cfg.cfg.kp = 2.4;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = 0.1;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_max = 100;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_min = -100;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_max = 15;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_min = -15;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = 2.4;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = 0.1;


    motor0.cfg.drivers.callback.init = pwm_init;
    motor0.cfg.drivers.callback.enable_all_drivers = enable_all_pwm_output;
    motor0.cfg.drivers.callback.disable_all_drivers = disable_all_pwm_output;
    motor0.cfg.drivers.callback.update_duty_cycle = pwm_duty_set;
    motor0.cfg.analog.callback.init = adc_init;
    motor0.cfg.analog.callback.update_sample_location = analog_update_sample_location;
    motor0.cfg.analog.callback.get_value = adc_value_get;

    motor0.cfg.loop.mode = mcl_mode_step_foc;
    motor0.cfg.loop.enable_step_motor_closed_loop = false;
    motor0.cfg.loop.enable_speed_loop = true;

    motor0.cfg.detect.enable_detect = true;
    motor0.cfg.detect.callback.disable_output = disable_all_pwm_output;

    motor0.cfg.path.loop_ts = 1.0f / (PWM_FREQUENCY) * T_CURE_TIMES;
    motor0.cfg.path.t_cure.time = 0;

    hpm_mcl_path_init(&motor0.path, &motor0.cfg.path, &motor0.cfg.mcl);
    hpm_mcl_analog_init(&motor0.analog, &motor0.cfg.analog, &motor0.cfg.mcl);
    hpm_mcl_drivers_init(&motor0.drivers, &motor0.cfg.drivers);
    hpm_mcl_control_init(&motor0.control, &motor0.cfg.control);
    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                    NULL, &motor0.analog, &motor0.control, &motor0.drivers, &motor0.path);
    hpm_mcl_detect_init(&motor0.detect, &motor0.cfg.detect, &motor0.loop, NULL, &motor0.analog, &motor0.drivers);
}

hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty)
{
    uint32_t pwm_reload;
    uint32_t pwm_cmp_half, pwm_reload_half;
    uint32_t index0, index1;

    pwm_reload = PWM_RELOAD;
    pwm_cmp_half = (uint32_t)(duty * pwm_reload) >> 1;
    pwm_reload_half =  pwm_reload >> 1;
    switch (chn) {
    case mcl_drivers_chn_a0:
        index0 = BOARD_BLDCPWM_CMP_INDEX_0;
        index1 = BOARD_BLDCPWM_CMP_INDEX_1;
        break;
    case mcl_drivers_chn_a1:
        index0 = BOARD_BLDCPWM_CMP_INDEX_2;
        index1 = BOARD_BLDCPWM_CMP_INDEX_3;
        break;
    case mcl_drivers_chn_b0:
        index0 = BOARD_BLDCPWM_CMP_INDEX_4;
        index1 = BOARD_BLDCPWM_CMP_INDEX_5;
        break;
    case mcl_drivers_chn_b1:
        index0 = BOARD_BLDCPWM_CMP_INDEX_6;
        index1 = BOARD_BLDCPWM_CMP_INDEX_7;
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

    return mcl_success;
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
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
    pwm_set_force_output(MOTOR0_BLDCPWM,
                        PWM_FORCE_OUTPUT(BOARD_BLDC_VH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WL_PWM_OUTPIN, pwm_output_0));
    pwm_enable_sw_force(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_immediately);

    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_update_shadow_immediately);

    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
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
    pwm_config_t pwm_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    pwm_deinit(MOTOR0_BLDCPWM);
    pwm_stop_counter(MOTOR0_BLDCPWM);
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
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

    pwm_get_default_pwm_config(MOTOR0_BLDCPWM, &pwm_config);
    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = true;

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_config, cmp_index + 2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_config, cmp_index + 4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_config, cmp_index + 6, &cmp_config[0], 2)) {
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
    /**
     * @brief The motor pre-driver chip of the stepper motor driver board uses slow attenuation mode,
     *  so it needs to be high level by default.
     *  Please refer to the driver manual for the attenuation mode.
     *
     */
    pwm_cfg.invert_output = true;
    pwm_cfg.enable_four_cmp = false;
    pwm_cfg.update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.dead_zone_in_half_cycle = 0;

    pwmv2_disable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_reset_counter(MOTOR0_BLDCPWM, pwm_counter_0);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(0), PWM_RELOAD, 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(9), 1, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_VH_PWM_OUTPIN * 2, &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(3);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(4);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_VL_PWM_OUTPIN * 2, &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(5);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(6);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_WH_PWM_OUTPIN * 2, &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(7);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(8);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_WL_PWM_OUTPIN * 2, &cmp_cfg[0], 2);

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
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD + 1;
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
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

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_IRQn, isr_adc)
void isr_adc(void)
{
    uint32_t status;

    status = hpm_adc_get_status_flags(&hpm_adc_a);
    if ((status & BOARD_BLDC_ADC_TRIG_FLAG) != 0) {
        hpm_adc_clear_status_flags(&hpm_adc_a, BOARD_BLDC_ADC_TRIG_FLAG);
        t_cure_times++;
        if (t_cure_times >= T_CURE_TIMES) {
            t_cure_times = 0;
            hpm_mcl_path_t_cure_generate(&motor0.path);
        }
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

    hpm_adc_init_pmt_dma(&hpm_adc_a, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCU_INDEX]));
    hpm_adc_init_pmt_dma(&hpm_adc_b, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCV_INDEX]));

    return mcl_success;
}

void adc_isr_enable(void)
{
    hpm_adc_enable_interrupts(&hpm_adc_a, BOARD_BLDC_ADC_TRIG_FLAG);
    intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_IRQn, 1);
}


void motor_adc_midpoint(void)
{
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint32_t times = 0;

    pwm_duty_set(mcl_drivers_chn_a, 0.5);
    pwm_duty_set(mcl_drivers_chn_b, 0.5);
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
    mcl_user_value_t id;
    path_plan_t_cure_cfg_t path;

    board_init();
    printf("step motor demo.\r\n");
    init_adc_bldc_pins();
    init_pwm_pins(MOTOR0_BLDCPWM);
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    motor_init();
    adc_isr_enable();
    timer_init();
    motor_adc_midpoint();
    hpm_mcl_loop_enable(&motor0.loop);
    id.enable = true;
    id.value = 0.65;
    hpm_mcl_loop_set_current_d(&motor0.loop, id);
    while (1) {
        path.speed = MCL_2PI * 1;
        path.acc_time = 0.05;
        path.dec_time = 0.05;
        path.acc = path.speed / path.acc_time;
        path.dec = path.speed / path.dec_time;
        path.time = 2;
        printf("speed: 1 r/s.\r\n");
        hpm_mcl_path_update_t_cure(&motor0.path, &path);
        while (!hpm_mcl_path_t_cure_complete(&motor0.path)) {
        }
        path.speed = -MCL_2PI * 2;
        path.acc_time = 0.05;
        path.dec_time = 0.05;
        path.acc = path.speed / path.acc_time;
        path.dec = path.speed / path.dec_time;
        path.time = 3;
        printf("speed: -2 r/s.\r\n");
        hpm_mcl_path_update_t_cure(&motor0.path, &path);
        while (!hpm_mcl_path_t_cure_complete(&motor0.path)) {
        }
        path.speed = MCL_2PI * 3;
        path.acc_time = 0.05;
        path.dec_time = 0.05;
        path.acc = path.speed / path.acc_time;
        path.dec = path.speed / path.dec_time;
        path.time = 4;
        printf("speed: 3 r/s.\r\n");
        hpm_mcl_path_update_t_cure(&motor0.path, &path);
        while (!hpm_mcl_path_t_cure_complete(&motor0.path)) {
        }
        path.speed = -MCL_2PI * 4;
        path.acc_time = 0.05;
        path.dec_time = 0.05;
        path.acc = path.speed / path.acc_time;
        path.dec = path.speed / path.dec_time;
        path.time = 5;
        printf("speed: -4 r/s.\r\n");
        hpm_mcl_path_update_t_cure(&motor0.path, &path);
        while (!hpm_mcl_path_t_cure_complete(&motor0.path)) {
        }
        path.speed = MCL_2PI * 5;
        path.acc_time = 0.05;
        path.dec_time = 0.05;
        path.acc = path.speed / path.acc_time;
        path.dec = path.speed / path.dec_time;
        path.time = 4;
        printf("speed: 5 r/s.\r\n");
        hpm_mcl_path_update_t_cure(&motor0.path, &path);
        while (!hpm_mcl_path_t_cure_complete(&motor0.path)) {
        }
        path.speed = -MCL_2PI * 6;
        path.acc_time = 0.05;
        path.dec_time = 0.05;
        path.acc = path.speed / path.acc_time;
        path.dec = path.speed / path.dec_time;
        path.time = 3;
        printf("speed: -6 r/s.\r\n");
        hpm_mcl_path_update_t_cure(&motor0.path, &path);
        while (!hpm_mcl_path_t_cure_complete(&motor0.path)) {
        }
    }
}
