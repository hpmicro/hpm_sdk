/*
 * Copyright (c) 2023-2024 HPMicro
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
#include "hpm_hall_drv.h"
#include "hpm_qei_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_adc.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_bldc_define.h"
#include "bldc_over_zero_cfg.h"
#include "hpm_block.h"
#include "hpm_over_zero.h"

#ifndef SEGGER_RTT_ENABLE
#define SEGGER_RTT_ENABLE 0
#endif

#if SEGGER_RTT_ENABLE
#include "SEGGER_RTT.h"
#endif

/* motor_speed set */
#define MOTOR0_SPD                  (15.0)
/* USER define */
#define QEI_WDOG_TIMEOUT            (200000)
#define MOTOR0_PWM_DUTY_INIT        (5900)
#define TIMER_TIMES_1MS             (10)

int32_t motor_clock_hz;
#define PWM_RELOAD                  ((motor_clock_hz/PWM_FREQUENCY) - 1)
#define SENSORLESS_TMR_RELOAD       (BOARD_BLDC_TMR_RELOAD / 10)

hpm_mcl_over_zero_cfg_t sensorless_cfg;

#if SEGGER_RTT_ENABLE
#define RTT_CHANNEL         (1)
#endif

volatile ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES];

uint8_t motor_dir = (MOTOR0_SPD > 0 ? hpm_motor_dir_forward : hpm_motor_dir_reverse);
float user_setspeed = MOTOR0_SPD;
hpm_mcl_over_zero_spd_para_t speed_para;
hpm_mcl_over_zero_pi_para_t pi_para;
bool motor_run;

/**
 * @brief 1MS timer
 *
 */
uint32_t timer_times;

/**< adc struct */
adc_type hpm_adc_u = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_PHASE_U_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_PHASE_U_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};
adc_type hpm_adc_v = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_PHASE_V_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_PHASE_V_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};
adc_type hpm_adc_w = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_PHASE_W_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_PHASE_W_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};

#if SEGGER_RTT_ENABLE
#pragma pack(push, 1)
  struct {
    uint16_t adc_u;
    uint16_t adc_v;
    uint16_t adc_w;
    uint8_t step_x;
    uint8_t step_x_delay;
    uint8_t step_s;
    float speed;
    float speed_filter;
  } rtt_disp_val;
#pragma pack(pop)
#endif

void bldc_block_motor0_duty_set(uint32_t duty)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, BOARD_BLDCPWM_CMP_INDEX_1,
        (PWM_RELOAD - duty) >> 1, (PWM_RELOAD + duty) >> 1);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (BOARD_BLDCPWM_CMP_INDEX_0 + 1), (PWM_RELOAD - duty) >> 1, 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (BOARD_BLDCPWM_CMP_INDEX_1 + 1), (PWM_RELOAD + duty) >> 1, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif
}

int32_t pival_to_pwmoutput(float pi)
{
    uint32_t  pwm_out;

    if (pi > PI_PWM_OUT_MAX) {
        pi = PI_PWM_OUT_MAX;
    } else if (pi < PI_PWM_OUT_MIN) {
        pi = PI_PWM_OUT_MIN;
    }
    pwm_out = pi;

    return pwm_out;
}

#if defined(HPMSOC_HAS_HPMSDK_PWM)
void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[3] = {0};
    pwm_config_t pwm_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    pwm_stop_counter(MOTOR0_BLDCPWM);
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
    pwm_get_default_pwm_config(MOTOR0_BLDCPWM, &pwm_config);
    pwm_set_reload(MOTOR0_BLDCPWM, 0, PWM_RELOAD);
    pwm_set_start_count(MOTOR0_BLDCPWM, 0, 0);

    /*
     * config cmp1 and cmp2
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = PWM_RELOAD >> 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = PWM_RELOAD >> 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[2].mode           = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = PWM_RELOAD >> 1;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;

    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
    }

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    pwm_config_cmp(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &cmp_config[2]);



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

}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
void pwm_init(void)
{
    pwmv2_cmp_config_t cmp_cfg[2];
    pwmv2_config_t pwm_cfg;

    cmp_cfg[0].cmp = PWM_RELOAD >> 1;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = cmp_value_from_shadow_val;
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(1);
    cmp_cfg[0].update_trigger = pwm_shadow_register_update_on_reload;
    cmp_cfg[1].cmp = PWM_RELOAD >> 1;
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
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(9), PWM_RELOAD >> 1, 0, false);
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

void init_trigger_cfg(uint8_t trig_ch, bool inten)
{
    adc_pmt_config_t pmt_cfg;

    pmt_cfg.module = BOARD_BLDC_ADC_MODULE;

#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.trig_ch   = trig_ch;
    pmt_cfg.config.adc12.trig_len  = BOARD_BLDC_ADC_PHASE_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_PHASE_CH_U;
    pmt_cfg.config.adc12.adc_ch[1] = BOARD_BLDC_ADC_PHASE_CH_W;
    pmt_cfg.config.adc12.adc_ch[2] = BOARD_BLDC_ADC_PHASE_CH_V;
    pmt_cfg.config.adc12.inten[0] = inten;
    pmt_cfg.config.adc12.inten[1] = inten;
    pmt_cfg.config.adc12.inten[2] = inten;

    pmt_cfg.adc_base.adc12 = hpm_adc_w.adc_base.adc12;
#else
    pmt_cfg.config.adc16.trig_ch   = trig_ch;
    pmt_cfg.config.adc16.trig_len  = BOARD_BLDC_ADC_PHASE_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_PHASE_CH_U;
    pmt_cfg.config.adc16.adc_ch[1] = BOARD_BLDC_ADC_PHASE_CH_V;
    pmt_cfg.config.adc16.adc_ch[2] = BOARD_BLDC_ADC_PHASE_CH_W;
    pmt_cfg.config.adc16.inten[0] = inten;
    pmt_cfg.config.adc16.inten[1] = inten;
    pmt_cfg.config.adc16.inten[2] = inten;

    pmt_cfg.adc_base.adc16 = hpm_adc_w.adc_base.adc16;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
}

void init_trigger_mux(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_BLDC_PWM_TRG_ADC;
    trgm_output_config(ptr, BOARD_BLDC_TRG_ADC, &trgm_output_cfg);
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_PHASE_IRQn, isr_adc)
void isr_adc(void)
{
    uint32_t status;
    status = hpm_adc_get_status_flags(&hpm_adc_u);

    if ((status & BOARD_BLDC_ADC_PHASE_TRIG_FLAG) != 0) {
        hpm_adc_clear_status_flags(&hpm_adc_u, BOARD_BLDC_ADC_PHASE_TRIG_FLAG);
        sensorless_cfg.adc_u = ((adc_buff[0][BOARD_BLDC_ADC_PHASE_TRG * 4]&0xffff)>>4)&0xfff;
        sensorless_cfg.adc_v = ((adc_buff[0][BOARD_BLDC_ADC_PHASE_TRG * 4 + 1]&0xffff)>>4)&0xfff;
        sensorless_cfg.adc_w = ((adc_buff[0][BOARD_BLDC_ADC_PHASE_TRG * 4 + 2]&0xffff)>>4)&0xfff;
    }
}

void adc_init(void)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;

    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.module = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_default_config(&cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    cfg.config.adc12.res            = adc12_res_12_bits;
    cfg.config.adc12.conv_mode      = adc12_conv_mode_preemption;
    cfg.config.adc12.diff_sel       = adc12_sample_signal_single_ended;
    cfg.config.adc12.adc_clk_div    = 2;
    cfg.config.adc12.sel_sync_ahb   = false;
    cfg.config.adc12.adc_ahb_en = true;

    cfg.adc_base.adc12 = hpm_adc_u.adc_base.adc12;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = hpm_adc_v.adc_base.adc12;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = hpm_adc_w.adc_base.adc12;
    hpm_adc_init(&cfg);
#else
    cfg.config.adc16.res            = adc16_res_16_bits;
    cfg.config.adc16.conv_mode      = adc16_conv_mode_preemption;
    cfg.config.adc16.adc_clk_div    = adc16_clock_divider_4;
    cfg.config.adc16.sel_sync_ahb   = false;
    cfg.config.adc16.adc_ahb_en = true;

    cfg.adc_base.adc16 = hpm_adc_u.adc_base.adc16;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc16 = hpm_adc_v.adc_base.adc16;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc16 = hpm_adc_w.adc_base.adc16;
    hpm_adc_init(&cfg);
#endif
    ch_cfg.module                        = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_channel_default_config(&ch_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    ch_cfg.config.adc12_ch.diff_sel      = adc12_sample_signal_single_ended;
    ch_cfg.config.adc12_ch.sample_cycle  = 15;

    ch_cfg.adc_base.adc12                = hpm_adc_u.adc_base.adc12;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_PHASE_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc12                = hpm_adc_v.adc_base.adc12;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_PHASE_CH_V;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc12                = hpm_adc_w.adc_base.adc12;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_PHASE_CH_W;
    hpm_adc_channel_init(&ch_cfg);
#else
    ch_cfg.config.adc16_ch.sample_cycle  = 20;

    ch_cfg.adc_base.adc16                = hpm_adc_u.adc_base.adc16;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_PHASE_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc16                = hpm_adc_v.adc_base.adc16;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_PHASE_CH_V;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc16                = hpm_adc_w.adc_base.adc16;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_PHASE_CH_W;
    hpm_adc_channel_init(&ch_cfg);
#endif

    init_trigger_mux(BOARD_BLDCPWM_TRGM);
    init_trigger_cfg(BOARD_BLDC_ADC_PHASE_TRG, true);

#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC16
    adc16_enable_pmt_queue(hpm_adc_u.adc_base.adc16, BOARD_BLDC_ADC_PHASE_TRG);
#endif

     /* Set DMA start address for preemption mode */
    hpm_adc_init_pmt_dma(&hpm_adc_u, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[0]));
}

void disable_all_pwm_output(void)
{
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    int8_t step_delay;
    uint32_t block_pwm_out;
    float current_speed;

    if (gptmr_check_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP))) {
        gptmr_clear_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
        step_delay = hpm_mcl_over_zero_step_get(&sensorless_cfg);
        if (motor_run == true) {
            timer_times++;
            if (timer_times >= TIMER_TIMES_1MS) {
                timer_times = 0;
                speed_para.speed = hpm_mcl_over_zero_cal_speed(&sensorless_cfg);
                current_speed = hpm_mcl_over_zero_speed_filter(&speed_para);
                pi_para.target = user_setspeed;
                pi_para.cur = current_speed;
                hpm_mcl_over_zero_pi_contrl(&pi_para);
                block_pwm_out = pival_to_pwmoutput(pi_para.outval);
                bldc_block_motor0_duty_set(block_pwm_out);
            }
        } else {
            pi_para.mem = MOTOR0_PWM_DUTY_INIT;
            bldc_block_motor0_duty_set(MOTOR0_PWM_DUTY_INIT);
        }
#if SEGGER_RTT_ENABLE
        rtt_disp_val.adc_u = sensorless_cfg.adc_u;
        rtt_disp_val.adc_v = sensorless_cfg.adc_v;
        rtt_disp_val.adc_w = sensorless_cfg.adc_w;
        rtt_disp_val.step_x = sensorless_cfg.next_interval;
        rtt_disp_val.step_x_delay = step_delay;
        rtt_disp_val.speed_filter = current_speed;
        rtt_disp_val.speed = speed_para.speed;
        SEGGER_RTT_Write(RTT_CHANNEL, &rtt_disp_val, sizeof(rtt_disp_val));
#endif
        if (motor_run == true) {
            hpm_mcl_over_zero_pwm_ctrl(BLDC_MOTOR0_INDEX, step_delay);
        }
    }
}

/*Timer init 1ms_isr*/
void timer_init(void)
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

void init_over_zero_para(hpm_mcl_over_zero_cfg_t *cfg)
{
    cfg->motor_dir = motor_dir;
    cfg->fsm = hpm_mcl_over_zero_fsm_init;
    cfg->pole_num = MOTOR0_POLE_PAIR;
    cfg->loop_time_in_sec = 0.0001;

    speed_para.err = 0;
    speed_para.err_last = 0;
    speed_para.filter_coef = 0.0304; /* 100hz */
    speed_para.mem = 0;
    speed_para.kp = 0.2814;
    speed_para.ki = 0.0145;
    speed_para.integral_max = 1000;
    speed_para.integral_min = -1000;
    speed_para.output_max = 100;
    speed_para.output_min = -100;

    pi_para.kp = PI_P_VAL;
    pi_para.ki = PI_I_VAL;
    pi_para.integral_max = PI_PWM_OUT_MAX;
    pi_para.integral_min = -PI_PWM_OUT_MAX;
    pi_para.output_max = PI_PWM_OUT_MAX;
    pi_para.output_min = PI_PWM_OUT_MIN;
    pi_para.mem = 0;
}

int main(void)
{
    uint32_t times = 0;
    uint8_t motor_step = 1;
    bool isadd = true;
#if SEGGER_RTT_ENABLE
    char JS_RTT_UpBuffer[4096];
#endif

    board_init();
    motor_run = false;
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
#if SEGGER_RTT_ENABLE
    SEGGER_RTT_ConfigUpBuffer(RTT_CHANNEL, "JScope_U2U2U2U1U1U1F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#endif
    init_pwm_pins(MOTOR0_BLDCPWM);
    init_motor_over_zero_sensorless_adc_pins();
    pwm_init();
    adc_init();
    timer_init();
    init_over_zero_para(&sensorless_cfg);
    bldc_block_motor0_duty_set(0);
    intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_PHASE_IRQn, 1);
    hpm_adc_enable_interrupts(&hpm_adc_u, BOARD_BLDC_ADC_PHASE_TRIG_FLAG);

    if (user_setspeed < 0) {
        user_setspeed = -user_setspeed;
    }

    times = 0;
    motor_run = false;
    if (motor_dir == hpm_motor_dir_forward) {
        while (times < 100) {
            motor_step++;
            times++;
            if (motor_step == 7) {
                motor_step = 1;
            }
#if SEGGER_RTT_ENABLE
            rtt_disp_val.step_s = motor_step;
#endif
            board_delay_us(4960);
            hpm_mcl_over_zero_pwm_ctrl(BLDC_MOTOR0_INDEX, motor_step);
        }
    } else {
        motor_step = 7;
        while (times < 100) {
            motor_step--;
            times++;
            if (motor_step == 0) {
                motor_step = 6;
            }
#if SEGGER_RTT_ENABLE
            rtt_disp_val.step_s = motor_step;
#endif
            board_delay_us(4960);
            hpm_mcl_over_zero_pwm_ctrl(BLDC_MOTOR0_INDEX, motor_step);
        }
    }
    /**
     * @brief Speed increase or decrease test
     *
     */
    while (1) {
        motor_run = true;
        while (1) {
            board_delay_ms(100);
            if (isadd) {
                user_setspeed++;
            } else {
                user_setspeed--;
            }
            if (user_setspeed >= 40) {
                isadd = false;
            } else if (user_setspeed <= 5) {
                isadd = true;
            }
        }
    }
    return 0;
}
