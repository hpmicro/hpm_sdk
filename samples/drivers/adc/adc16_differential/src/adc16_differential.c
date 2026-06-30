/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_adc16_drv.h"

#ifndef ADC_SOC_HW_TRIG_SRC
#error "ADC16 differential mode requires hardware trigger"
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#elif defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif

#ifndef ADC_SOC_NO_HW_TRIG_SRC
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#endif

#include <string.h>

#ifndef APP_ADC16_CORE
#define APP_ADC16_CORE BOARD_RUNNING_CORE
#endif

#if defined(BOARD_APP_ADC16_DIFF_PAIR01_BASE_MASTER) && defined(BOARD_APP_ADC16_DIFF_PAIR01_BASE_SLAVE)
#define ADC16_DIFF_PAIR01 1
#else
#define ADC16_DIFF_PAIR01 0
#endif

#if defined(BOARD_APP_ADC16_DIFF_PAIR23_BASE_MASTER) && defined(BOARD_APP_ADC16_DIFF_PAIR23_BASE_SLAVE)
#define ADC16_DIFF_PAIR23 1
#else
#define ADC16_DIFF_PAIR23 0
#endif

#if !ADC16_DIFF_PAIR01 && !ADC16_DIFF_PAIR23
#error "This sample requires BOARD_APP_ADC16_DIFF_PAIR01 or BOARD_APP_ADC16_DIFF_PAIR23 diff pair in board.h"
#endif

#ifndef APP_ADC16_CH_SAMPLE_CYCLE
#define APP_ADC16_CH_SAMPLE_CYCLE            (8)
#endif

#define APP_ADC16_SW_AVE_COUNT (1)  /* software average count over seq DMA results, 1~16; not HW adc_loop */
#define APP_ADC16_SEQ_START_POS              (0U)
#define APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES (APP_ADC16_SW_AVE_COUNT)

#ifndef ADC_SOC_NO_HW_TRIG_SRC
#ifndef APP_ADC16_TRIG_SRC_FREQUENCY
#define APP_ADC16_TRIG_SRC_FREQUENCY         (500U) /* 500 Hz, 2 ms period */
#endif

#define APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A    (8U)
#define APP_ADC16_HW_TRIG_SRC                BOARD_APP_ADC16_HW_TRIG_SRC
#define APP_ADC16_HW_TRGM                    BOARD_APP_ADC16_HW_TRGM
#define APP_ADC16_HW_TRGM_IN                 BOARD_APP_ADC16_HW_TRGM_IN

#if defined(BOARD_APP_ADC16_DIFF_PAIR01_HW_TRGM_OUT_SEQ_MASTER) && defined(BOARD_APP_ADC16_DIFF_PAIR01_HW_TRGM_OUT_SEQ_SLAVE)
#define APP_ADC16_HW_TRGM_OUT_SEQ_MASTER_PAIR01   BOARD_APP_ADC16_DIFF_PAIR01_HW_TRGM_OUT_SEQ_MASTER
#define APP_ADC16_HW_TRGM_OUT_SEQ_SLAVE_PAIR01    BOARD_APP_ADC16_DIFF_PAIR01_HW_TRGM_OUT_SEQ_SLAVE
#endif

#if defined(BOARD_APP_ADC16_DIFF_PAIR23_HW_TRGM_OUT_SEQ_MASTER) && defined(BOARD_APP_ADC16_DIFF_PAIR23_HW_TRGM_OUT_SEQ_SLAVE)
#define APP_ADC16_HW_TRGM_OUT_SEQ_MASTER_PAIR23   BOARD_APP_ADC16_DIFF_PAIR23_HW_TRGM_OUT_SEQ_MASTER
#define APP_ADC16_HW_TRGM_OUT_SEQ_SLAVE_PAIR23    BOARD_APP_ADC16_DIFF_PAIR23_HW_TRGM_OUT_SEQ_SLAVE
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#define APP_ADC16_HW_TRGM_SRC_OUT_CH_MASTER  (0U)
#define APP_ADC16_HW_TRGM_SRC_OUT_CH_SLAVE   (1U)
#endif
#endif

#if ADC16_DIFF_PAIR01
volatile ATTR_PLACE_AT_WITH_ALIGNMENT(".ahb_sram", ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t seq_dma_buff_pair01[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES];
uint8_t seq_ch_master_pair01[APP_ADC16_SW_AVE_COUNT] = {BOARD_APP_ADC16_DIFF_PAIR01_MASTER_CH_1};
uint8_t seq_ch_slave_pair01[APP_ADC16_SW_AVE_COUNT] = {BOARD_APP_ADC16_DIFF_PAIR01_SLAVE_CH_1};
#endif

#if ADC16_DIFF_PAIR23
volatile ATTR_PLACE_AT_WITH_ALIGNMENT(".ahb_sram", ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t seq_dma_buff_pair23[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES];
uint8_t seq_ch_master_pair23[APP_ADC16_SW_AVE_COUNT] = {BOARD_APP_ADC16_DIFF_PAIR23_MASTER_CH_1};
uint8_t seq_ch_slave_pair23[APP_ADC16_SW_AVE_COUNT] = {BOARD_APP_ADC16_DIFF_PAIR23_SLAVE_CH_1};
#endif

#if ADC16_DIFF_PAIR01
static adc16_diff_config_t diff_cfg_pair01_master;
static adc16_diff_config_t diff_cfg_pair01_slave;
#endif

#if ADC16_DIFF_PAIR23
static adc16_diff_config_t diff_cfg_pair23_master;
static adc16_diff_config_t diff_cfg_pair23_slave;
#endif

/* (%d) matches HW bus encoding; no remapping (e.g. zero diff stays 32768 when POS=1). */
static int32_t adc16_diff_raw_to_code(uint32_t raw, const adc16_diff_config_t *cfg)
{
    uint16_t u = (uint16_t)(raw & 0xffffU);

    if (cfg->position_mode == adc16_diff_pos_mode_single_ended) {
        return (int32_t)u;
    }
    if (cfg->full_resolution != 0U) {
        /* FULL=1: HW ignores POS_MODE; clamp diff < 0 to raw 0; bus is unsigned 0..65535 */
        return (int32_t)u;
    }
    return (int32_t)(int16_t)u;
}

#ifndef ADC_SOC_NO_HW_TRIG_SRC
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
void init_trigger_source(PWMV2_Type *ptr)
{
    int mot_clock_freq;

    mot_clock_freq = clock_get_frequency(BOARD_APP_ADC16_HW_TRIG_SRC_CLK_NAME);

    pwmv2_shadow_register_unlock(ptr);
    pwmv2_set_reload_update_time(ptr, pwm_counter_0, pwm_reload_update_on_shlk);
    pwmv2_set_shadow_val(ptr, PWMV2_SHADOW_INDEX(0), (mot_clock_freq/APP_ADC16_TRIG_SRC_FREQUENCY) - 1, 0, false);

    pwmv2_cmp_update_trig_time(ptr, 16, pwm_shadow_register_update_on_shlk);
    pwmv2_set_shadow_val(ptr, PWMV2_SHADOW_INDEX(1), 1, 0, false);
    pwmv2_select_cmp_source(ptr, 16, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));

    pwmv2_shadow_register_lock(ptr);

    pwmv2_counter_select_data_offset_from_shadow_value(ptr, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(ptr, pwm_counter_0);

    if (status_success != pwmv2_set_trigout_cmp_index(ptr, APP_ADC16_HW_TRGM_SRC_OUT_CH_MASTER, 16)) {
        printf("failed to set PWMv2 trigger output compare index\n");
        while (1) {
        }
    }
    if (status_success != pwmv2_set_trigout_cmp_index(ptr, APP_ADC16_HW_TRGM_SRC_OUT_CH_SLAVE, 16)) {
        printf("failed to set PWMv2 trigger output compare index\n");
        while (1) {
        }
    }
}

void stop_trigger_source(PWMV2_Type *ptr)
{
    ptr->CNT_GLBCFG &= ~PWMV2_CNT_GLBCFG_TIMER_ENABLE_SET((1 << pwm_counter_0));
    ptr->CNT_GLBCFG |= PWMV2_CNT_GLBCFG_TIMER_RESET_SET((1 << pwm_counter_0));
}

void start_trigger_source(PWMV2_Type *ptr)
{
    ptr->CNT_GLBCFG = PWMV2_CNT_GLBCFG_TIMER_ENABLE_SET((1 << pwm_counter_0));
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWM)
void init_trigger_source(PWM_Type *ptr)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;
    int mot_clock_freq;

    mot_clock_freq = clock_get_frequency(BOARD_APP_ADC16_HW_TRIG_SRC_CLK_NAME);

    pwm_set_reload(ptr, 0, (mot_clock_freq / APP_ADC16_TRIG_SRC_FREQUENCY) - 1);

    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp  = false;
    pwm_cmp_cfg.mode           = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;
    pwm_cmp_cfg.cmp = ((mot_clock_freq / APP_ADC16_TRIG_SRC_FREQUENCY) - 1) >> 1;
    pwm_config_cmp(ptr, APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A, &pwm_cmp_cfg);

    pwm_issue_shadow_register_lock_event(APP_ADC16_HW_TRIG_SRC);

    pwm_output_ch_cfg.cmp_start_index = APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A;
    pwm_output_ch_cfg.cmp_end_index   = APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A, &pwm_output_ch_cfg);

    pwm_start_counter(ptr);
}

void stop_trigger_source(PWM_Type *ptr)
{
    pwm_stop_counter(ptr);
}

void start_trigger_source(PWM_Type *ptr)
{
    pwm_start_counter(ptr);
}
#endif

void init_trigger_mux(TRGM_Type *ptr, uint8_t input, uint8_t output)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_same_as_input;

    trgm_output_cfg.input  = input;
    trgm_output_config(ptr, output, &trgm_output_cfg);
}
#endif

#if ADC16_DIFF_PAIR01
hpm_stat_t init_common_config_pair01(adc16_conversion_mode_t conv_mode, bool master)
{
    adc16_config_t cfg;
    ADC16_Type *ptr = master ? BOARD_APP_ADC16_DIFF_PAIR01_BASE_MASTER : BOARD_APP_ADC16_DIFF_PAIR01_BASE_SLAVE;

    adc16_get_default_config(&cfg);

    cfg.res         = adc16_res_16_bits;
    cfg.conv_mode   = conv_mode;
    cfg.adc_clk_div = adc16_clock_divider_4;
#if !defined(HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB) || !HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB
    cfg.sel_sync_ahb = (clk_adc_src_ahb0 == clock_get_source(master ? BOARD_APP_ADC16_DIFF_PAIR01_CLK_NAME_MASTER : BOARD_APP_ADC16_DIFF_PAIR01_CLK_NAME_SLAVE)) ? true : false;
#endif

    if (adc16_init(ptr, &cfg) == status_success) {
        return status_success;
    } else {
        printf("%s initialization failed!\n", master ? BOARD_APP_ADC16_DIFF_PAIR01_NAME_MASTER : BOARD_APP_ADC16_DIFF_PAIR01_NAME_SLAVE);
        return status_fail;
    }
}

void init_sequence_config_pair01(bool master)
{
    adc16_seq_config_t seq_cfg;
    adc16_dma_config_t dma_cfg;
    adc16_channel_config_t ch_cfg;
    ADC16_Type *ptr = master ? BOARD_APP_ADC16_DIFF_PAIR01_BASE_MASTER : BOARD_APP_ADC16_DIFF_PAIR01_BASE_SLAVE;

    for (int i = 0; i < APP_ADC16_SW_AVE_COUNT; i++) {
        seq_ch_master_pair01[i] = seq_ch_master_pair01[0];
        seq_ch_slave_pair01[i] = seq_ch_slave_pair01[0];
    }

    adc16_get_channel_default_config(&ch_cfg);

    ch_cfg.sample_cycle = APP_ADC16_CH_SAMPLE_CYCLE;

    for (uint32_t i = 0; i < (master ? sizeof(seq_ch_master_pair01) : sizeof(seq_ch_slave_pair01)); i++) {
        ch_cfg.ch = master ? seq_ch_master_pair01[i] : seq_ch_slave_pair01[i];
        adc16_init_channel(ptr, &ch_cfg);
    }

    seq_cfg.seq_len    = master ? sizeof(seq_ch_master_pair01) : sizeof(seq_ch_slave_pair01);
    seq_cfg.restart_en = false;
    seq_cfg.cont_en    = true;
    seq_cfg.hw_trig_en = true;
    seq_cfg.sw_trig_en = false;

    for (int i = APP_ADC16_SEQ_START_POS; i < seq_cfg.seq_len; i++) {
        seq_cfg.queue[i].seq_int_en = false;
        seq_cfg.queue[i].ch = master ? seq_ch_master_pair01[i] : seq_ch_slave_pair01[i];
    }

    adc16_set_seq_config(ptr, &seq_cfg);

    if (master) {
        dma_cfg.start_addr         = (uint32_t *)core_local_mem_to_sys_address(APP_ADC16_CORE, (uint32_t)seq_dma_buff_pair01);
        dma_cfg.buff_len_in_4bytes = APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES;
        dma_cfg.stop_en            = false;

        adc16_init_seq_dma(ptr, &dma_cfg);
    }

    init_trigger_mux(APP_ADC16_HW_TRGM, APP_ADC16_HW_TRGM_IN, master ? APP_ADC16_HW_TRGM_OUT_SEQ_MASTER_PAIR01 : APP_ADC16_HW_TRGM_OUT_SEQ_SLAVE_PAIR01);

    if (master) {
        init_trigger_source(APP_ADC16_HW_TRIG_SRC);
    }
}
#endif

#if ADC16_DIFF_PAIR23
hpm_stat_t init_common_config_pair23(adc16_conversion_mode_t conv_mode, bool master)
{
    adc16_config_t cfg;
    ADC16_Type *ptr = master ? BOARD_APP_ADC16_DIFF_PAIR23_BASE_MASTER : BOARD_APP_ADC16_DIFF_PAIR23_BASE_SLAVE;

    adc16_get_default_config(&cfg);

    cfg.res         = adc16_res_16_bits;
    cfg.conv_mode   = conv_mode;
    cfg.adc_clk_div = adc16_clock_divider_4;
#if !defined(HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB) || !HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB
    cfg.sel_sync_ahb = (clk_adc_src_ahb0 == clock_get_source(master ? BOARD_APP_ADC16_DIFF_PAIR23_CLK_NAME_MASTER : BOARD_APP_ADC16_DIFF_PAIR23_CLK_NAME_SLAVE)) ? true : false;
#endif

    if (adc16_init(ptr, &cfg) == status_success) {
        return status_success;
    } else {
        printf("%s initialization failed!\n", master ? BOARD_APP_ADC16_DIFF_PAIR23_NAME_MASTER : BOARD_APP_ADC16_DIFF_PAIR23_NAME_SLAVE);
        return status_fail;
    }
}

void init_sequence_config_pair23(bool master)
{
    adc16_seq_config_t seq_cfg;
    adc16_dma_config_t dma_cfg;
    adc16_channel_config_t ch_cfg;
    ADC16_Type *ptr = master ? BOARD_APP_ADC16_DIFF_PAIR23_BASE_MASTER : BOARD_APP_ADC16_DIFF_PAIR23_BASE_SLAVE;

    for (int i = 0; i < APP_ADC16_SW_AVE_COUNT; i++) {
        seq_ch_master_pair23[i] = seq_ch_master_pair23[0];
        seq_ch_slave_pair23[i] = seq_ch_slave_pair23[0];
    }

    adc16_get_channel_default_config(&ch_cfg);

    ch_cfg.sample_cycle = APP_ADC16_CH_SAMPLE_CYCLE;

    for (uint32_t i = 0; i < (master ? sizeof(seq_ch_master_pair23) : sizeof(seq_ch_slave_pair23)); i++) {
        ch_cfg.ch = master ? seq_ch_master_pair23[i] : seq_ch_slave_pair23[i];
        adc16_init_channel(ptr, &ch_cfg);
    }

    seq_cfg.seq_len    = master ? sizeof(seq_ch_master_pair23) : sizeof(seq_ch_slave_pair23);
    seq_cfg.restart_en = false;
    seq_cfg.cont_en    = true;
    seq_cfg.hw_trig_en = true;
    seq_cfg.sw_trig_en = false;

    for (int i = APP_ADC16_SEQ_START_POS; i < seq_cfg.seq_len; i++) {
        seq_cfg.queue[i].seq_int_en = false;
        seq_cfg.queue[i].ch = master ? seq_ch_master_pair23[i] : seq_ch_slave_pair23[i];
    }

    adc16_set_seq_config(ptr, &seq_cfg);

    if (master) {
        dma_cfg.start_addr         = (uint32_t *)core_local_mem_to_sys_address(APP_ADC16_CORE, (uint32_t)seq_dma_buff_pair23);
        dma_cfg.buff_len_in_4bytes = APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES;
        dma_cfg.stop_en            = false;

        adc16_init_seq_dma(ptr, &dma_cfg);
    }

    init_trigger_mux(APP_ADC16_HW_TRGM, APP_ADC16_HW_TRGM_IN, master ? APP_ADC16_HW_TRGM_OUT_SEQ_MASTER_PAIR23 : APP_ADC16_HW_TRGM_OUT_SEQ_SLAVE_PAIR23);

    if (master) {
        init_trigger_source(APP_ADC16_HW_TRIG_SRC);
    }
}
#endif

void sequence_handler(void)
{
#if ADC16_DIFF_PAIR01
    int32_t sum_pair01 = 0;
    uint32_t raw_pair01 = 0;
#endif
#if ADC16_DIFF_PAIR23
    int32_t sum_pair23 = 0;
    uint32_t raw_pair23 = 0;
#endif

    start_trigger_source(APP_ADC16_HW_TRIG_SRC);

    #if ADC16_DIFF_PAIR01
    while (!seq_dma_buff_pair01[APP_ADC16_SW_AVE_COUNT - 1]) {

    }
    #endif

    #if ADC16_DIFF_PAIR23
    while (!seq_dma_buff_pair23[APP_ADC16_SW_AVE_COUNT - 1]) {

    }
    #endif

    #if ADC16_DIFF_PAIR01
    for (int i = 0; i < APP_ADC16_SW_AVE_COUNT; i++) {
        raw_pair01 += seq_dma_buff_pair01[i] & 0xffffU;
        sum_pair01 += adc16_diff_raw_to_code(seq_dma_buff_pair01[i], &diff_cfg_pair01_master);
    }
    raw_pair01 /= APP_ADC16_SW_AVE_COUNT;
    sum_pair01 /= APP_ADC16_SW_AVE_COUNT;
    #endif

    #if ADC16_DIFF_PAIR23
    for (int i = 0; i < APP_ADC16_SW_AVE_COUNT; i++) {
        raw_pair23 += seq_dma_buff_pair23[i] & 0xffffU;
        sum_pair23 += adc16_diff_raw_to_code(seq_dma_buff_pair23[i], &diff_cfg_pair23_master);
    }
    raw_pair23 /= APP_ADC16_SW_AVE_COUNT;
    sum_pair23 /= APP_ADC16_SW_AVE_COUNT;
    #endif

    stop_trigger_source(BOARD_APP_ADC16_HW_TRIG_SRC);

    #if ADC16_DIFF_PAIR01
    printf("ADC0/ADC1 diff result: 0x%04x (%d)\n", raw_pair01 & 0xffffU, sum_pair01);
    seq_dma_buff_pair01[APP_ADC16_SW_AVE_COUNT - 1] = 0;
    #endif

    #if ADC16_DIFF_PAIR23
    printf("ADC2/ADC3 diff result: 0x%04x (%d)\n", raw_pair23 & 0xffffU, sum_pair23);
    seq_dma_buff_pair23[APP_ADC16_SW_AVE_COUNT - 1] = 0;
    #endif
}

int main(void)
{
    adc16_conversion_mode_t conv_mode;

    board_init();

    board_init_adc16_diff_pins();

    printf("ADC16 differential sample\n");

#if ADC16_DIFF_PAIR01
    board_init_adc_clock(BOARD_APP_ADC16_DIFF_PAIR01_BASE_MASTER, true);
    board_init_adc_clock(BOARD_APP_ADC16_DIFF_PAIR01_BASE_SLAVE, true);
#endif

#if ADC16_DIFF_PAIR23
    board_init_adc_clock(BOARD_APP_ADC16_DIFF_PAIR23_BASE_MASTER, true);
    board_init_adc_clock(BOARD_APP_ADC16_DIFF_PAIR23_BASE_SLAVE, true);
#endif

    conv_mode = adc16_conv_mode_sequence;

#if ADC16_DIFF_PAIR01
    init_common_config_pair01(conv_mode, true);
    init_common_config_pair01(conv_mode, false);
#endif

#if ADC16_DIFF_PAIR23
    init_common_config_pair23(conv_mode, true);
    init_common_config_pair23(conv_mode, false);
#endif

#if ADC16_DIFF_PAIR01
    adc16_get_default_diff_mode_config(BOARD_APP_ADC16_DIFF_PAIR01_BASE_MASTER, &diff_cfg_pair01_master);
    diff_cfg_pair01_master.master = true;
    diff_cfg_pair01_master.full_resolution = 0U;
    diff_cfg_pair01_master.position_mode = adc16_diff_pos_mode_differential;
    adc16_get_default_diff_mode_config(BOARD_APP_ADC16_DIFF_PAIR01_BASE_SLAVE, &diff_cfg_pair01_slave);
    diff_cfg_pair01_slave.master = false;
    adc16_enable_diff_mode(BOARD_APP_ADC16_DIFF_PAIR01_BASE_MASTER, &diff_cfg_pair01_master);
    adc16_enable_diff_mode(BOARD_APP_ADC16_DIFF_PAIR01_BASE_SLAVE, &diff_cfg_pair01_slave);
#endif

#if ADC16_DIFF_PAIR23
    adc16_get_default_diff_mode_config(BOARD_APP_ADC16_DIFF_PAIR23_BASE_MASTER, &diff_cfg_pair23_master);
    diff_cfg_pair23_master.master = true;
    diff_cfg_pair23_master.full_resolution = 0U;
    diff_cfg_pair23_master.position_mode = adc16_diff_pos_mode_differential;
    adc16_get_default_diff_mode_config(BOARD_APP_ADC16_DIFF_PAIR23_BASE_SLAVE, &diff_cfg_pair23_slave);
    diff_cfg_pair23_slave.master = false;
    adc16_enable_diff_mode(BOARD_APP_ADC16_DIFF_PAIR23_BASE_MASTER, &diff_cfg_pair23_master);
    adc16_enable_diff_mode(BOARD_APP_ADC16_DIFF_PAIR23_BASE_SLAVE, &diff_cfg_pair23_slave);
#endif

#if ADC16_DIFF_PAIR01
    init_sequence_config_pair01(true);
    init_sequence_config_pair01(false);
#endif

#if ADC16_DIFF_PAIR23
    init_sequence_config_pair23(true);
    init_sequence_config_pair23(false);
#endif

    while (1) {
        sequence_handler();
        board_delay_ms(500);
    }
}
