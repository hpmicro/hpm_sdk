/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_rdc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "math.h"
#include "hpm_adc16_drv.h"

#define DAC_MODE 0
#if defined(DAC_MODE) && DAC_MODE
#include "hpm_dac_drv.h"
#include "hpm_synt_drv.h"
#endif

void init_trigger_cfg(uint8_t trig_ch, bool inten)
{
    adc16_pmt_config_t pmt_cfg;

    pmt_cfg.trig_ch   = trig_ch;
    pmt_cfg.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.adc_ch[0] = BOARD_RDC_ADC_I_CHN;
    pmt_cfg.inten[0] = inten;
    adc16_set_pmt_config(BOARD_RDC_ADC_I_BASE, &pmt_cfg);
    pmt_cfg.adc_ch[0] = BOARD_RDC_ADC_Q_CHN;
    adc16_set_pmt_config(BOARD_RDC_ADC_Q_BASE, &pmt_cfg);
}

void adc_init(void)
{
    adc16_config_t cfg;
    adc16_channel_config_t ch_cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_preemption;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = true;
    cfg.adc_ahb_en = true;
    adc16_init(BOARD_RDC_ADC_I_BASE, &cfg);
    adc16_init(BOARD_RDC_ADC_Q_BASE, &cfg);

    adc16_get_channel_default_config(&ch_cfg);
    ch_cfg.sample_cycle  = 20;
    ch_cfg.ch            = BOARD_RDC_ADC_I_CHN;
    adc16_init_channel(BOARD_RDC_ADC_I_BASE, &ch_cfg);
    ch_cfg.sample_cycle  = 20;
    ch_cfg.ch            = BOARD_RDC_ADC_Q_CHN;
    adc16_init_channel(BOARD_RDC_ADC_Q_BASE, &ch_cfg);

    adc16_enable_motor(BOARD_RDC_ADC_I_BASE);
    adc16_enable_motor(BOARD_RDC_ADC_Q_BASE);
}

void init_pwm_pla_trgm(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

/*The GPIO is designed for debug */
#ifdef RDC_SAMPLE_TEST_GPIO_OUTPUT
    /* trig0 p0 */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input  = BOARD_RDC_TRG_IN;
    trgm_output_config(ptr, BOARD_RDC_TRG_OUT, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1);
#endif

    /* trig0 adc */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input  = BOARD_RDC_TRG_IN;
    trgm_output_config(ptr, BOARD_RDC_TRG_ADC, &trgm_output_cfg);
}

void rdc_cfg(RDC_Type *rdc)
{
    rdc_output_cfg_t cfg = {0};
    rdc_input_cfg_t inputcfg = {0};

#if defined(DAC_MODE) && DAC_MODE
    cfg.mode = rdc_output_dac;
#else
    cfg.mode = rdc_output_pwm;
#endif
    cfg.excitation_period_cycle = 35840;
    cfg.excitation_precision = rdc_output_precision_64_point;
    cfg.pwm_period = rdc_output_pwm_period_1_sample;
    cfg.output_swap = true;
    cfg.amp_offset = 0;
    cfg.amp_man = 2;
    cfg.amp_exp = 3;
    cfg.pwm_dither_enable = false;
    cfg.pwm_exc_n_low_active = false;
    cfg.pwm_exc_p_low_active = false;
    rdc_output_config(rdc, &cfg);
#if defined(HPM_IP_FEATURE_RDC_IIR) && (HPM_IP_FEATURE_RDC_IIR)
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_RDC_ADC_MATRIX_TO_ADC0, BOARD_APP_RDC_ADC_MATRIX_FROM_ADC_I, false);
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_RDC_ADC_MATRIX_TO_ADC1, BOARD_APP_RDC_ADC_MATRIX_FROM_ADC_Q, false);
#endif
    rdc_output_trig_offset_config(rdc, trigger_out_0, 150);
    rdc_output_trig_enable(rdc, trigger_out_0);
    rdc_output_trig_disable(rdc, trigger_out_1);
    rdc_exc_enable(rdc);

    inputcfg.rectify_signal_sel = rdc_rectify_signal_exc_0_ph;
    inputcfg.acc_cycle_len = 4;
    inputcfg.acc_input_chn_i = BOARD_RDC_ADC_I_CHN;
    inputcfg.acc_input_chn_q = BOARD_RDC_ADC_Q_CHN;
    inputcfg.acc_input_port_i = 0;
    inputcfg.acc_input_port_q = 1;
    inputcfg.acc_stamp = rdc_acc_stamp_end_of_acc;
    rdc_input_config(rdc, &inputcfg);

    rdc_acc_cfg_t acc_cfg;
    acc_cfg.continue_edge_num = 4;
    acc_cfg.edge_distance = 1;
    acc_cfg.exc_carrier_period = 35840;
    acc_cfg.right_shift_without_sign = 8;
    rdc_set_acc_config(rdc, &acc_cfg);

    rdc_acc_enable(rdc);
    rdc_interrupt_config(BOARD_RDC_BASE, acc_vld_i_stat | acc_vld_q_stat);
    intc_m_enable_irq_with_priority(BOARD_RDC_IRQ, 1);
}

#if defined(DAC_MODE) && DAC_MODE
void init_dac(void)
{
    dac_config_t config = {0};
    init_dac_pins(BOARD_DAC_BASE);
    synt_enable_timestamp(HPM_SYNT, true);
    dac_get_default_config(&config);
    config.dac_mode = dac_mode_trig; /* dac_mode_direct */
    dac_init(BOARD_DAC_BASE, &config);

    dac_enable_conversion(BOARD_DAC_BASE, true);
}
#endif

uint32_t val_acc_i, val_acc_q;
volatile bool value_status;

SDK_DECLARE_EXT_ISR_M(BOARD_RDC_IRQ, isr_acc_i_q_sample)
void isr_acc_i_q_sample(void)
{
    uint32_t status = get_interrupt_status(BOARD_RDC_BASE);
    static bool acc_i_status = false;
    static bool acc_q_status = false;
    if (RDC_INT_EN_ACC_VLD_I_EN_GET(status)) {
        rdc_interrupt_clear_flag_bits(BOARD_RDC_BASE, RDC_INT_EN_ACC_VLD_I_EN_MASK);
        acc_i_status = true;

    }
    if (RDC_INT_EN_ACC_VLD_Q_EN_GET(status)) {
        rdc_interrupt_clear_flag_bits(BOARD_RDC_BASE, RDC_INT_EN_ACC_VLD_Q_EN_MASK);
        acc_q_status = true;
    }
    if ((acc_i_status) && (acc_q_status)) {
        acc_i_status = false;
        acc_q_status = false;
        value_status = true;
        val_acc_i = rdc_get_acc_avl(BOARD_RDC_BASE, rdc_acc_chn_i);
        val_acc_q = rdc_get_acc_avl(BOARD_RDC_BASE, rdc_acc_chn_q);
    }
}

int main(void)
{
    uint32_t freq;
    int32_t val_max_i, val_min_i;
    uint32_t val_delay_i;
    int32_t val_max_q, val_min_q;
    uint32_t val_delay_q;
    int32_t val_middle_i, val_middle_q;
    uint8_t num;
    float theta;
    float init_theta  = -1;

    board_init();
    board_init_adc_clock(BOARD_RDC_ADC_I_BASE, true);
    board_init_adc_clock(BOARD_RDC_ADC_Q_BASE, true);
#if defined(DAC_MODE) && DAC_MODE
    init_dac();
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac0, trgm_dac_matrix_in_from_rdc_dac0, false);
#endif
    init_rdc_pin();
    adc_init();
    init_pwm_pla_trgm(BOARD_RDC_TRGM);
    init_trigger_cfg(BOARD_RDC_ADC_TRG, true);
    adc16_enable_pmt_queue(BOARD_RDC_ADC_I_BASE, BOARD_RDC_ADC_TRG);
    adc16_enable_pmt_queue(BOARD_RDC_ADC_Q_BASE, BOARD_RDC_ADC_TRG);
    freq = clock_get_frequency(BOARD_PLB_PWM_CLOCK_NAME);
    printf("freq:%d.\r\n", freq);
    rdc_cfg(BOARD_RDC_BASE);
    rdc_output_trig_sw(BOARD_RDC_BASE);
    num = 0;
    rdc_clear_i_maxval(BOARD_RDC_BASE);
    rdc_clear_i_minval(BOARD_RDC_BASE);
    rdc_clear_q_maxval(BOARD_RDC_BASE);
    rdc_clear_q_minval(BOARD_RDC_BASE);
    do {
        num++;
        board_delay_ms(1);
        val_max_i = rdc_get_i_maxval(BOARD_RDC_BASE);
        val_min_i = rdc_get_i_minval(BOARD_RDC_BASE);

        val_max_q = rdc_get_q_maxval(BOARD_RDC_BASE);
        val_min_q = rdc_get_q_minval(BOARD_RDC_BASE);
    } while (num < 20);
    val_middle_i =  -((-val_max_i + val_min_i) / 2 - val_min_i + 0x800000);
    rdc_set_edge_detection_offset(BOARD_RDC_BASE, rdc_acc_chn_i, val_middle_i);
    val_middle_q =  -((-val_max_q + val_min_q) / 2 - val_min_q + 0x800000);
    rdc_set_edge_detection_offset(BOARD_RDC_BASE, rdc_acc_chn_q, val_middle_q);
    num = 0;
    val_delay_i = 1;
    do {
        num++;
        val_delay_i += rdc_get_rise_delay_i(BOARD_RDC_BASE);
        board_delay_ms(1);
    } while (num < 100);
    val_delay_i /= num;
    rdc_set_acc_sync_delay(BOARD_RDC_BASE, rdc_acc_chn_i, val_delay_i >> 1);

    num = 0;
    val_delay_q = 1;
    do {
        num++;
        val_delay_q += rdc_get_rise_delay_q(BOARD_RDC_BASE);
        board_delay_ms(1);
    } while (num < 100);
    val_delay_q /= num;
    rdc_set_acc_sync_delay(BOARD_RDC_BASE, rdc_acc_chn_q, val_delay_q >> 1);
    board_delay_ms(100);
    value_status = false;
    rdc_interrupt_enable(BOARD_RDC_BASE);
    while (1) {
        if (value_status) {
            value_status = false;
            theta = (atanf(((float)val_acc_i)/val_acc_q) * 180 / HPM_PI) + 90;
            if (init_theta < 0) {
                init_theta = theta;
            }
            theta -= init_theta;
            if (theta < 0) {
                theta += 180;
            }
            printf("theta:%f.\r\n", theta);
        }
    };
}