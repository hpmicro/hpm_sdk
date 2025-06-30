/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_soc_feature.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_PWMV2
#include "hpm_pwmv2_drv.h"
#else
#include "hpm_pwm_drv.h"
#endif
#include "hpm_adc16_drv.h"
#include "hpm_qeiv2_drv.h"

#ifndef APP_QEI_BASE
#define APP_QEI_BASE BOARD_APP_QEIV2_BASE
#endif
#ifndef APP_QEI_IRQ
#define APP_QEI_IRQ BOARD_APP_QEIV2_IRQ
#endif
#ifndef APP_MOTOR_CLK
#define APP_MOTOR_CLK BOARD_APP_QEI_CLOCK_SOURCE
#endif

#define PWM_FREQ 30000
#define SIGNAL_FREQ 50

#define RECORD_DATA_SIZE 2000

static volatile bool s_qei_data_ready;
static volatile uint32_t z, pos, ang;
static uint32_t s_record_data[4][RECORD_DATA_SIZE];
volatile ATTR_PLACE_AT_NONCACHEABLE uint32_t s_sin_adc_data[ADC_SOC_PMT_MAX_DMA_BUFF_LEN_IN_4BYTES];

/* Static function declaration */
static void pwm_init(void);
static void trigger_mux_init(void);
static void adc_init(void);
static void qeiv2_init(void);

/* Function definition */
int main(void)
{
    board_init();
    board_init_adc_clock(BOARD_APP_QEI_ADC_SIN_BASE, true);
    board_init_adc_qeiv2_pins();

    pwm_init();
    trigger_mux_init();
    adc_init();
    qeiv2_init();

    printf("qeiv2 sin togi example\n");

    for (uint16_t i = 0; i < RECORD_DATA_SIZE; i++) {
        s_record_data[0][i] = s_sin_adc_data[0] & 0xFFFF;
        s_record_data[1][i] = qeiv2_get_postion(APP_QEI_BASE);
        s_record_data[2][i] = qeiv2_get_angle(APP_QEI_BASE);
        s_record_data[3][i] = qeiv2_get_z_phase(APP_QEI_BASE);
        board_delay_us(20);
    }

    for (uint16_t i = 0; i < RECORD_DATA_SIZE; i++) {
        printf("adc:%#x, pos:%#x, ang:%#x, z:%#x\n", s_record_data[0][i], s_record_data[1][i], s_record_data[2][i], s_record_data[3][i]);
    }

    s_qei_data_ready = false;
    while (1) {
        if (s_qei_data_ready) {
            s_qei_data_ready = false;
            printf("position: 0x%x\n", pos);
            printf("ang: 0x%x\n", ang);
            printf("z: 0x%x\n", z);
        }
    }
    return 0;
}

SDK_DECLARE_EXT_ISR_M(APP_QEI_IRQ, isr_qei)
void isr_qei(void)
{
    uint32_t status = qeiv2_get_status(APP_QEI_BASE);

    qeiv2_clear_status(APP_QEI_BASE, status);
    if ((status & QEIV2_EVENT_POSITION_COMPARE_FLAG_MASK) != 0) {
        if (!s_qei_data_ready) {
            s_qei_data_ready = true;
            pos = qeiv2_get_count_on_read_event(APP_QEI_BASE, qeiv2_counter_type_speed);
            ang = qeiv2_get_count_on_read_event(APP_QEI_BASE, qeiv2_counter_type_timer);
            z = qeiv2_get_count_on_read_event(APP_QEI_BASE, qeiv2_counter_type_z);
        }
    }
}

#ifdef HPMSOC_HAS_HPMSDK_PWMV2
static void pwm_init(void)
{
    uint32_t reload = clock_get_frequency(APP_MOTOR_CLK) / PWM_FREQ;

    pwmv2_disable_counter(BOARD_APP_PWM, pwm_counter_0);
    pwmv2_reset_counter(BOARD_APP_PWM, pwm_counter_0);

    pwmv2_shadow_register_unlock(BOARD_APP_PWM);
    pwmv2_set_shadow_val(BOARD_APP_PWM, PWMV2_SHADOW_INDEX(0), reload - 1, 0, false);
    pwmv2_set_shadow_val(BOARD_APP_PWM, PWMV2_SHADOW_INDEX(1), (reload / 2) - 1, 0, false);
    pwmv2_shadow_register_lock(BOARD_APP_PWM);

    pwmv2_counter_select_data_offset_from_shadow_value(BOARD_APP_PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(BOARD_APP_PWM, pwm_counter_0);
    pwmv2_set_reload_update_time(BOARD_APP_PWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_cmp_update_trig_time(BOARD_APP_PWM, PWMV2_CMP_INDEX(16), pwm_shadow_register_update_on_modify);
    pwmv2_select_cmp_source(BOARD_APP_PWM, PWMV2_CMP_INDEX(16), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_cmp_select_counter(BOARD_APP_PWM, PWMV2_CMP_INDEX(16), pwm_counter_0);

    pwmv2_set_trigout_cmp_index(BOARD_APP_PWM, pwm_channel_0, PWMV2_CMP_INDEX(16));

    pwmv2_enable_counter(BOARD_APP_PWM, pwm_counter_0);
    pwmv2_start_pwm_output(BOARD_APP_PWM, pwm_counter_0);
}

static void trigger_mux_init(void)
{
    trgm_output_t trgm_config;

    /* pwm trigout0 trig adc and vsc */
    trgm_config.invert = false;
    trgm_config.type = trgm_output_pulse_at_input_rising_edge;
    trgm_config.input = BOARD_APP_TRGM_PWM_INPUT;
    trgm_output_config(HPM_TRGM0, BOARD_APP_QEI_TRG_ADC, &trgm_config);
}
#else
static void pwm_init(void)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;
    uint32_t reload = clock_get_frequency(APP_MOTOR_CLK) / PWM_FREQ;

    pwm_set_reload(HPM_PWM0, 0, reload - 1);

    /* Set a comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp = false;
    pwm_cmp_cfg.mode = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_cfg.cmp = (reload / 2) - 1;
    pwm_config_cmp(HPM_PWM0, 8, &pwm_cmp_cfg);

    /* Issue a shadow lock */
    pwm_issue_shadow_register_lock_event(HPM_PWM0);

    /* Set comparator channel to generate a trigger signal */
    pwm_output_ch_cfg.cmp_start_index = 8; /* start channel */
    pwm_output_ch_cfg.cmp_end_index = 8;   /* end channel */
    pwm_output_ch_cfg.invert_output = false;
    pwm_config_output_channel(HPM_PWM0, 8, &pwm_output_ch_cfg);

    /* Start the comparator counter */
    pwm_start_counter(HPM_PWM0);
}

static void trigger_mux_init(void)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input = HPM_TRGM0_INPUT_SRC_PWM0_CH8REF;
    trgm_output_config(HPM_TRGM0, TRGM_TRGOCFG_ADCX_PTRGI0A, &trgm_output_cfg);
}
#endif

static void adc_init(void)
{
    adc16_config_t cfg;
    adc16_channel_config_t ch_cfg;
    adc16_pmt_config_t pmt_cfg;

    /* initialize ADC instances */
    adc16_get_default_config(&cfg);
    cfg.res = adc16_res_16_bits;
    cfg.conv_mode = adc16_conv_mode_preemption;
    cfg.adc_clk_div = adc16_clock_divider_4;
    cfg.sel_sync_ahb = true;
    cfg.adc_ahb_en = true;
    adc16_init(BOARD_APP_QEI_ADC_SIN_BASE, &cfg);

    /* initialize ADC channels */
    adc16_get_channel_default_config(&ch_cfg);
    ch_cfg.sample_cycle = 20;
    ch_cfg.ch = BOARD_APP_QEI_ADC_SIN_CHN;
    adc16_init_channel(BOARD_APP_QEI_ADC_SIN_BASE, &ch_cfg);

    /* initialize preempt config */
    pmt_cfg.adc_ch[0] = BOARD_APP_QEI_ADC_SIN_CHN;
    pmt_cfg.inten[0] = false;
    pmt_cfg.trig_ch = ADC16_CONFIG_TRG0A;
    pmt_cfg.trig_len = 1;
    adc16_set_pmt_config(BOARD_APP_QEI_ADC_SIN_BASE, &pmt_cfg);
    adc16_enable_pmt_queue(BOARD_APP_QEI_ADC_SIN_BASE, pmt_cfg.trig_ch);

    /* Set DMA start address for preemption mode */
    adc16_init_pmt_dma(BOARD_APP_QEI_ADC_SIN_BASE, (uint32_t)&s_sin_adc_data[0]);

    adc16_enable_motor(BOARD_APP_QEI_ADC_SIN_BASE);
}

static void qeiv2_init(void)
{
    qeiv2_adc_config_t adc_config = {0};
    qeiv2_mode_config_t mode_config = {0};
    qeiv2_pos_cmp_match_config_t pos_cmp_config = { 0 };

    /* adc config */
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_QEI_ADC_MATRIX_TO_ADC0, BOARD_APP_QEI_ADC_MATRIX_FROM_ADC_SIN, false);
    adc_config.adc_select = 0;
    adc_config.adc_channel = BOARD_APP_QEI_ADC_SIN_CHN;
    adc_config.offset = 0x80000000;    /* middle point */
    adc_config.param0 = 0x4000;
    adc_config.param1 = 0;
    qeiv2_config_adcx(APP_QEI_BASE, &adc_config, true);

    /* mode config */
    mode_config.work_mode = qeiv2_work_mode_sin;    /* togi need qeiv2_work_mode_sin */
    mode_config.spd_tmr_content_sel = qeiv2_spd_tmr_as_pos_angle;
    mode_config.z_count_inc_mode = qeiv2_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = 1;
    mode_config.z_cali_enable = false;
    mode_config.z_cali_ignore_ab = false;
    mode_config.phcnt_idx = 0;
    qeiv2_config_mode(APP_QEI_BASE, &mode_config);

    /* togi config */
    qeiv2_config_togi_w_param(APP_QEI_BASE, SIGNAL_FREQ, PWM_FREQ);
    qeiv2_set_togi_enable(APP_QEI_BASE, true);

    /* cmp config */
    pos_cmp_config.pos_cmp_value = 0x00000000;
    pos_cmp_config.ignore_pos_dir = true;
    qeiv2_config_position_cmp_match_condition(APP_QEI_BASE, &pos_cmp_config);
    qeiv2_enable_load_read_trigger_event(APP_QEI_BASE, QEIV2_EVENT_POSITION_COMPARE_FLAG_MASK);

    /* irq config */
    qeiv2_enable_irq(APP_QEI_BASE, QEIV2_EVENT_POSITION_COMPARE_FLAG_MASK);
    intc_m_enable_irq_with_priority(APP_QEI_IRQ, 1);
}
