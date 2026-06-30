/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_adc16_drv.h"

#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif

#ifndef ADC_SOC_NO_HW_TRIG_SRC
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#endif

#ifndef APP_ADC16_CORE
#define APP_ADC16_CORE BOARD_RUNNING_CORE
#endif

#define APP_ADC16_CLOCK_BUS  BOARD_APP_ADC16_CLK_BUS

#define APP_ADC16_CH_SAMPLE_CYCLE            (20U)
#define APP_ADC16_CH_WDOG_EVENT              (1 << BOARD_APP_ADC16_CH_1)

#define APP_ADC16_SEQ_START_POS              (0U)
#define APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES (1024U)
#define APP_ADC16_SEQ_IRQ_EVENT              adc16_event_seq_single_complete

#ifndef ADC_SOC_NO_HW_TRIG_SRC
#define APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A    (8U)
#define APP_ADC16_HW_TRIG_SRC                BOARD_APP_ADC16_HW_TRIG_SRC
#define APP_ADC16_HW_TRGM                    BOARD_APP_ADC16_HW_TRGM
#define APP_ADC16_HW_TRGM_IN                 BOARD_APP_ADC16_HW_TRGM_IN
#define APP_ADC16_HW_TRGM_OUT_SEQ            BOARD_APP_ADC16_HW_TRGM_OUT_SEQ
#define APP_ADC16_HW_TRGM_OUT_PMT            BOARD_APP_ADC16_HW_TRGM_OUT_PMT
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#define APP_ADC16_HW_TRGM_SRC_OUT_CH         (0U)
#endif
#endif

#define APP_ADC16_PMT_TRIG_CH                BOARD_APP_ADC16_PMT_TRIG_CH
#define APP_ADC16_PMT_DMA_BUFF_LEN_IN_4BYTES ADC_SOC_PMT_MAX_DMA_BUFF_LEN_IN_4BYTES
#define APP_ADC16_PMT_IRQ_EVENT              adc16_event_trig_complete

#ifndef APP_ADC16_TRIG_SRC_FREQUENCY
#define APP_ADC16_TRIG_SRC_FREQUENCY         (20000U)
#endif

/*
 * SAMPLE_CFG.ADC_LOOP exponent n when IP has ADC_LOOP: repeat count is 2^n (0 = once).
 * Max n is ADC16_SOC_ADC_LOOP_MAX_EXP for the SoC.
 * Override: -DAPP_ADC16_CH_ADC_LOOP_EXP=n.
 */
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
#ifndef APP_ADC16_CH_ADC_LOOP_EXP
#define APP_ADC16_CH_ADC_LOOP_EXP (0U)
#endif
#endif

/*
 * Sequence mode DMA 16-bit (ADC_CFG0 DMA_SEQ16BIT): 0=32-bit packed (default), 1=16-bit per sample.
 * Override: -DAPP_ADC16_ENABLE_SEQ_DMA_16BIT=1.
 */
#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
#ifndef APP_ADC16_ENABLE_SEQ_DMA_16BIT
#define APP_ADC16_ENABLE_SEQ_DMA_16BIT (0)
#endif
#endif

/*
 * 1: after each result, append ", LOOP exp=... (repeat ...)" from adc16_get_channel_adc_loop (exp read as 0 if IP has no ADC_LOOP).
 * 0: omit.
 * Override: -DAPP_ADC16_ENABLE_LOOP_EXP_LOG=1
 */
#ifndef APP_ADC16_ENABLE_LOOP_EXP_LOG
#define APP_ADC16_ENABLE_LOOP_EXP_LOG (0)
#endif

/*
 * Stop after this many printed result lines (all modes). 0 = unlimited.
 * Override: -DAPP_ADC16_CAPTURE_MAX=N.
 */
#ifndef APP_ADC16_CAPTURE_MAX
#define APP_ADC16_CAPTURE_MAX (1000U)
#endif

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t seq_buff[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t pmt_buff[APP_ADC16_PMT_DMA_BUFF_LEN_IN_4BYTES];

/** Oneshot mode: channels to init and poll; log order matches adc16_print_loop per channel. */
uint8_t oneshot_adc_channel[] = {BOARD_APP_ADC16_CH_1};
/** Period mode: channels to init and adc16_set_prd_config; log order per period tick. */
uint8_t period_adc_channel[] = {BOARD_APP_ADC16_CH_1};
/** Sequence mode: channel order in seq queue, DMA and logs (init_sequence_config). */
uint8_t seq_adc_channel[] = {BOARD_APP_ADC16_CH_1};
/** Preemption mode: channels bound to trigger (init_preemption_config). */
uint8_t trig_adc_channel[] = {BOARD_APP_ADC16_CH_1};

uint8_t current_cycle_bit;

__IO uint8_t seq_complete_flag;
__IO uint8_t trig_complete_flag;
__IO uint32_t res_out_of_thr_flag;

static uint32_t s_oneshot_capture_count;
static uint32_t s_sequence_capture_count;
static uint32_t s_period_capture_count;
static uint32_t s_preemption_capture_count;

#if APP_ADC16_ENABLE_LOOP_EXP_LOG
static void adc16_print_loop(uint8_t ch)
{
    uint8_t exp;

    if (adc16_get_channel_adc_loop(BOARD_APP_ADC16_BASE, ch, &exp) == status_success) {
        printf(", LOOP exp=%u (repeat %u)", (unsigned int)exp, (unsigned int)(1U << exp));
    }
}
#else
static void adc16_print_loop(uint8_t ch)
{
    (void)ch;
}
#endif

static uint8_t get_adc_conv_mode(void)
{
    uint8_t ch;

    while (1) {
        printf("1. Oneshot    mode\n");
        printf("2. Period     mode\n");
        printf("3. Sequence   mode\n");
        printf("4. Preemption mode\n");

        printf("Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): ");
        printf("%c\n", ch = getchar());
        ch -= '0' + 1;
        if (ch > adc16_conv_mode_preemption) {
            printf("The ADC mode is not supported!\n");
        } else {
            return ch;
        }
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_ADC16_IRQn, isr_adc16)
void isr_adc16(void)
{
    uint32_t status;

    status = adc16_get_status_flags(BOARD_APP_ADC16_BASE);

    /* Clear status */
    adc16_clear_status_flags(BOARD_APP_ADC16_BASE, status);

    if (ADC16_INT_STS_SEQ_CVC_GET(status)) {
        /* Set flag to read memory data */
        seq_complete_flag = 1;
    }

    if (ADC16_INT_STS_TRIG_CMPT_GET(status)) {
        /* Set flag to read memory data */
        trig_complete_flag = 1;
    }

    if (ADC16_INT_STS_WDOG_GET(status) & APP_ADC16_CH_WDOG_EVENT) {
        adc16_disable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_CH_WDOG_EVENT);
        res_out_of_thr_flag = ADC16_INT_STS_WDOG_GET(status) & APP_ADC16_CH_WDOG_EVENT;
    }
}

/**
 * Seq DMA 16-bit mode: each DMA item is one 16-bit conversion result only (no cycle bit/sequence num /channel flags).
 * 32-bit packed mode: each dword is adc16_seq_dma_data_t as in the reference manual.
 */
hpm_stat_t process_seq_data(uint32_t *buff, int32_t start_pos, uint32_t len, bool seq_dma_16bit)
{
    adc16_seq_dma_data_t *dma_data;

    if (seq_dma_16bit) {
        uint16_t *p = (uint16_t *)buff;
        const uint32_t seq_ch_cnt = (uint32_t)sizeof(seq_adc_channel);
        uint32_t rel;
        uint8_t adc_ch;

        for (uint32_t i = (uint32_t)start_pos; i < (uint32_t)start_pos + len; i++) {
            rel = i - (uint32_t)start_pos;
            adc_ch = seq_adc_channel[rel % seq_ch_cnt];

            printf("Sequence Mode - %s - ", BOARD_APP_ADC16_NAME);
            printf("ADC Channel: %02d - ", (unsigned int)adc_ch);
            printf("Result (16-bit DMA): 0x%04x", p[i]);
            adc16_print_loop(adc_ch);
            printf("\n");
        }
        return status_success;
    }

    dma_data = (adc16_seq_dma_data_t *)buff;
    current_cycle_bit = !current_cycle_bit;

    for (uint32_t i = start_pos; i < start_pos + len; i++) {
        printf("Sequence Mode - %s - ", BOARD_APP_ADC16_NAME);
        printf("Cycle Bit: %02d - ",   dma_data[i].cycle_bit);
        printf("Sequence Number:%02d - ", dma_data[i].seq_num);
        printf("ADC Channel: %02d - ",  dma_data[i].adc_ch);
        printf("Result: 0x%04x", dma_data[i].result);
        adc16_print_loop((uint8_t)dma_data[i].adc_ch);
        printf("\n");

        if (dma_data[i].cycle_bit != current_cycle_bit) {
            printf("Error: Cycle bit is not expected value[%d]!\n", current_cycle_bit);
            while (1) {

            }
        }
    }

    return status_success;
}

hpm_stat_t process_pmt_data(uint32_t *buff, int32_t start_pos, uint32_t len)
{
    adc16_pmt_dma_data_t *dma_data = (adc16_pmt_dma_data_t *)buff;

    if (ADC16_IS_PMT_DMA_BUFF_LEN_INVLAID(len)) {
        return status_invalid_argument;
    }

    current_cycle_bit = 1;

    for (uint32_t i = start_pos; i < start_pos + len; i++) {
        printf("Preemption Mode - %s - ", BOARD_APP_ADC16_NAME);
        printf("Trigger Channel: %02d - ", dma_data[i].trig_ch);
        printf("Cycle Bit: %02d - ", dma_data[i].cycle_bit);
        printf("Sequence Number: %02d - ", dma_data[i].seq_num);
        printf("ADC Channel: %02d - ", dma_data[i].adc_ch);
        printf("Result: 0x%04x", dma_data[i].result);
        adc16_print_loop((uint8_t)dma_data[i].adc_ch);
        printf("\n");

        if (dma_data[i].cycle_bit == current_cycle_bit) {
            dma_data[i].cycle_bit = 0;
        } else {
            printf("Error: Cycle bit is not expected value[%d]!\n", current_cycle_bit);
            while (1) {

            }
        }
    }

    return status_success;
}

#ifndef ADC_SOC_NO_HW_TRIG_SRC

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
void init_trigger_source(PWMV2_Type *ptr)
{
    int mot_clock_freq;

    mot_clock_freq =  clock_get_frequency(BOARD_APP_ADC16_HW_TRIG_SRC_CLK_NAME);

    pwmv2_shadow_register_unlock(ptr);
    pwmv2_set_reload_update_time(ptr, pwm_counter_0, pwm_reload_update_on_reload);
    pwmv2_set_shadow_val(ptr, PWMV2_SHADOW_INDEX(0), (mot_clock_freq/APP_ADC16_TRIG_SRC_FREQUENCY) - 1, 0, false);
    pwmv2_set_shadow_val(ptr, PWMV2_SHADOW_INDEX(1), ((mot_clock_freq/APP_ADC16_TRIG_SRC_FREQUENCY) - 1) >> 1, 0, false);
    pwmv2_select_cmp_source(ptr, 16, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_shadow_register_lock(ptr);

    pwmv2_counter_select_data_offset_from_shadow_value(ptr, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(ptr, pwm_counter_0);

    if (status_success != pwmv2_set_trigout_cmp_index(ptr, APP_ADC16_HW_TRGM_SRC_OUT_CH, 16)) {
        printf("failed to set PWMv2 trigger output compare index\n");
        while (1) {
        }
    }
    pwmv2_enable_counter(ptr, pwm_counter_0);
}

void stop_trigger_source(PWMV2_Type *ptr)
{
    pwmv2_disable_counter(ptr, pwm_counter_0);
}

void start_trigger_source(PWMV2_Type *ptr)
{
    pwmv2_enable_counter(ptr, pwm_counter_0);
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWM)
void init_trigger_source(PWM_Type *ptr)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;

    int mot_clock_freq;

    mot_clock_freq = clock_get_frequency(BOARD_APP_ADC16_HW_TRIG_SRC_CLK_NAME);

    /* reload value */
    pwm_set_reload(ptr, 0, (mot_clock_freq/APP_ADC16_TRIG_SRC_FREQUENCY) - 1);

    /* Set a comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp  = false;
    pwm_cmp_cfg.mode           = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_cfg.cmp = ((mot_clock_freq/APP_ADC16_TRIG_SRC_FREQUENCY) - 1) >> 1;
    pwm_config_cmp(ptr, APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A, &pwm_cmp_cfg);

    /* Issue a shadow lock */
    pwm_issue_shadow_register_lock_event(APP_ADC16_HW_TRIG_SRC);

    /* Set comparator channel to generate a trigger signal */
    pwm_output_ch_cfg.cmp_start_index = APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A;   /* start channel */
    pwm_output_ch_cfg.cmp_end_index   = APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A;   /* end channel */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, APP_ADC16_HW_TRIG_SRC_PWM_REFCH_A, &pwm_output_ch_cfg);

    /* Start the comparator counter */
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

void init_trigger_target(ADC16_Type *ptr, uint8_t trig_ch)
{
    adc16_pmt_config_t pmt_cfg;

    pmt_cfg.trig_len = sizeof(trig_adc_channel);
    pmt_cfg.trig_ch = trig_ch;

    for (int i = 0; i < pmt_cfg.trig_len; i++) {
        pmt_cfg.adc_ch[i] = trig_adc_channel[i];
        pmt_cfg.inten[i] = false;
    }

    pmt_cfg.inten[pmt_cfg.trig_len - 1] = true;

    adc16_set_pmt_config(ptr, &pmt_cfg);
    adc16_enable_pmt_queue(ptr, trig_ch);
}

hpm_stat_t init_common_config(adc16_conversion_mode_t conv_mode)
{
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);

    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = conv_mode;
    cfg.adc_clk_div    = adc16_clock_divider_4;
#if !defined(HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB) || !HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB
    cfg.sel_sync_ahb   = (APP_ADC16_CLOCK_BUS == clock_get_source(BOARD_APP_ADC16_CLK_NAME)) ? true : false;
#endif

#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
    if (cfg.conv_mode == adc16_conv_mode_sequence) {
        cfg.dma_seq16bit = (APP_ADC16_ENABLE_SEQ_DMA_16BIT != 0);
    }
#endif

    /* adc16 initialization */
    if (adc16_init(BOARD_APP_ADC16_BASE, &cfg) == status_success) {
        /* enable irq */
        intc_m_enable_irq_with_priority(BOARD_APP_ADC16_IRQn, 1);
        return status_success;
    } else {
        printf("%s initialization failed!\n", BOARD_APP_ADC16_NAME);
        return status_fail;
    }
}

void channel_result_out_of_threshold_handler(void)
{
    adc16_channel_threshold_t threshold;
    uint32_t i = 31;

    if (res_out_of_thr_flag) {
        while (i--) {
            if ((res_out_of_thr_flag >> i) & 0x01) {
                adc16_get_channel_threshold(BOARD_APP_ADC16_BASE, i, &threshold);
                printf("Warning - %s [channel %02d] - Sample voltage is out of the thresholds between 0x%04x and 0x%04x !\n", BOARD_APP_ADC16_NAME, i, threshold.thshdl, threshold.thshdh);
            }
        }

        res_out_of_thr_flag = 0;
        adc16_enable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_CH_WDOG_EVENT);
    }
}

hpm_stat_t init_oneshot_config(void)
{
    adc16_channel_config_t ch_cfg;
    hpm_stat_t st;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    ch_cfg.sample_cycle = APP_ADC16_CH_SAMPLE_CYCLE;
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
    ch_cfg.adc_loop_exp = APP_ADC16_CH_ADC_LOOP_EXP;
#endif

    for (uint32_t i = 0; i < sizeof(oneshot_adc_channel); i++) {
        ch_cfg.ch = oneshot_adc_channel[i];
        st = adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
        if (st != status_success) {
            return st;
        }
    }

    adc16_set_nonblocking_read(BOARD_APP_ADC16_BASE);

#if defined(ADC_SOC_BUSMODE_ENABLE_CTRL_SUPPORT) && ADC_SOC_BUSMODE_ENABLE_CTRL_SUPPORT
    /* enable oneshot mode */
    adc16_enable_oneshot_mode(BOARD_APP_ADC16_BASE);
#endif
#if APP_ADC16_CAPTURE_MAX > 0
    s_oneshot_capture_count = 0U;
#endif
    return status_success;
}

bool oneshot_handler(void)
{
    uint16_t result;
    uint8_t ch;
    uint32_t printed;

    printed = 0U;
    for (uint32_t i = 0; i < sizeof(oneshot_adc_channel); i++) {
        ch = oneshot_adc_channel[i];
        if (adc16_get_oneshot_result(BOARD_APP_ADC16_BASE, ch, &result) == status_success) {
            if (adc16_is_nonblocking_mode(BOARD_APP_ADC16_BASE)) {
                adc16_get_oneshot_result(BOARD_APP_ADC16_BASE, ch, &result);
            }
            printf("Oneshot Mode - %s [channel %02d] - Result: 0x%04x", BOARD_APP_ADC16_NAME, ch, result);
            adc16_print_loop(ch);
            printf("\n");
            printed++;
        }
    }
    if (printed == 0U) {
        return false;
    }
#if APP_ADC16_CAPTURE_MAX > 0
    s_oneshot_capture_count += printed;
    if (s_oneshot_capture_count >= APP_ADC16_CAPTURE_MAX) {
        printf("Oneshot Test: captured %u results, stop.\n", (unsigned int)APP_ADC16_CAPTURE_MAX);
        return true;
    }
#endif
    return false;
}

hpm_stat_t init_period_config(void)
{
    adc16_channel_config_t ch_cfg;
    adc16_prd_config_t prd_cfg;
    hpm_stat_t st;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    ch_cfg.sample_cycle = APP_ADC16_CH_SAMPLE_CYCLE;
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
    ch_cfg.adc_loop_exp = APP_ADC16_CH_ADC_LOOP_EXP;
#endif

    for (uint32_t i = 0; i < sizeof(period_adc_channel); i++) {
        ch_cfg.ch = period_adc_channel[i];
        st = adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
        if (st != status_success) {
            return st;
        }
    }

    prd_cfg.prescale     = 22;    /* Set divider: 2^22 clocks */
    prd_cfg.period_count = 5;     /* 6 periods */
    for (uint32_t i = 0; i < sizeof(period_adc_channel); i++) {
        prd_cfg.ch = period_adc_channel[i];
        adc16_set_prd_config(BOARD_APP_ADC16_BASE, &prd_cfg);
    }
#if APP_ADC16_CAPTURE_MAX > 0
    s_period_capture_count = 0U;
#endif
    return status_success;
}

bool period_handler(void)
{
    uint16_t result;
    uint8_t ch;

    for (uint32_t i = 0; i < sizeof(period_adc_channel); i++) {
        ch = period_adc_channel[i];
        adc16_get_prd_result(BOARD_APP_ADC16_BASE, ch, &result);
        printf("Period Mode - %s [channel %02d] - Result: 0x%04x", BOARD_APP_ADC16_NAME, ch, result);
        adc16_print_loop(ch);
        printf("\n");
    }
#if APP_ADC16_CAPTURE_MAX > 0
    s_period_capture_count += sizeof(period_adc_channel);
    if (s_period_capture_count >= APP_ADC16_CAPTURE_MAX) {
        printf("Period test: captured %u results, stop.\n", (unsigned int)APP_ADC16_CAPTURE_MAX);
        return true;
    }
#endif
    return false;
}

hpm_stat_t init_sequence_config(void)
{
    adc16_seq_config_t seq_cfg;
    adc16_dma_config_t dma_cfg;
    adc16_channel_config_t ch_cfg;
    hpm_stat_t st;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.sample_cycle = APP_ADC16_CH_SAMPLE_CYCLE;
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
    ch_cfg.adc_loop_exp = APP_ADC16_CH_ADC_LOOP_EXP;
#endif

    for (uint32_t i = 0; i < sizeof(seq_adc_channel); i++) {
        ch_cfg.ch = seq_adc_channel[i];
        st = adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
        if (st != status_success) {
            return st;
        }
    }

    /* Set a sequence config */
    seq_cfg.seq_len    = sizeof(seq_adc_channel);
    seq_cfg.restart_en = false;
    seq_cfg.cont_en    = true;
#ifndef ADC_SOC_NO_HW_TRIG_SRC
    #ifndef __ADC16_USE_SW_TRIG
    seq_cfg.hw_trig_en = true;
    seq_cfg.sw_trig_en = false;
    #else
    seq_cfg.hw_trig_en = false;
    seq_cfg.sw_trig_en = true;
    #endif
#else
    seq_cfg.hw_trig_en = false;
    seq_cfg.sw_trig_en = true;
#endif

    for (int i = APP_ADC16_SEQ_START_POS; i < seq_cfg.seq_len; i++) {
        seq_cfg.queue[i].seq_int_en = false;
        seq_cfg.queue[i].ch = seq_adc_channel[i];
    }

    /* Enable the single complete interrupt for the last conversion */
    seq_cfg.queue[seq_cfg.seq_len - 1].seq_int_en = true;

    /* Initialize a sequence */
    adc16_set_seq_config(BOARD_APP_ADC16_BASE, &seq_cfg);

    /* DMA buffer length: buff_len_in_2bytes when DMA_SEQ16BIT, else buff_len_in_4bytes (same union storage). */
    dma_cfg.start_addr = (uint32_t *)core_local_mem_to_sys_address(APP_ADC16_CORE, (uint32_t)seq_buff);
#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
    if (adc16_is_seq_dma_16bit_enabled(BOARD_APP_ADC16_BASE)) {
        dma_cfg.buff_len_in_2bytes = (uint32_t)sizeof(seq_adc_channel);
    } else {
        dma_cfg.buff_len_in_4bytes = (uint32_t)sizeof(seq_adc_channel);
    }
#else
    dma_cfg.buff_len_in_4bytes = (uint32_t)sizeof(seq_adc_channel);
#endif
    dma_cfg.stop_en            = false;
    dma_cfg.stop_pos           = 0;

    /* Initialize DMA for the sequence mode */
    adc16_init_seq_dma(BOARD_APP_ADC16_BASE, &dma_cfg);

#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    /* TRGM + PWM before IRQ so the first HW edge does not fire before the path is armed */
    init_trigger_mux(APP_ADC16_HW_TRGM, APP_ADC16_HW_TRGM_IN, APP_ADC16_HW_TRGM_OUT_SEQ);
    init_trigger_source(APP_ADC16_HW_TRIG_SRC);
#endif

    /* Enable sequence complete interrupt */
    adc16_enable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_SEQ_IRQ_EVENT);
#if APP_ADC16_CAPTURE_MAX > 0
    s_sequence_capture_count = 0U;
#endif
    return status_success;
}

bool sequence_handler(void)
{
#if defined(ADC_SOC_NO_HW_TRIG_SRC) || defined(__ADC16_USE_SW_TRIG)
    /* SW trigger */
    adc16_trigger_seq_by_sw(BOARD_APP_ADC16_BASE);
#endif

    while (seq_complete_flag == 0) {

    }

#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    adc16_seq_disable_hw_trigger(BOARD_APP_ADC16_BASE);
    /* Stop the trigger source output */
    stop_trigger_source(APP_ADC16_HW_TRIG_SRC);
#endif
    /* Process data (16-bit DMA packs each sample as low halfword then high halfword in memory) */
    process_seq_data(seq_buff, APP_ADC16_SEQ_START_POS, sizeof(seq_adc_channel), adc16_is_seq_dma_16bit_enabled(BOARD_APP_ADC16_BASE));

#if APP_ADC16_CAPTURE_MAX > 0
    s_sequence_capture_count += (uint32_t)sizeof(seq_adc_channel);
    if (s_sequence_capture_count >= APP_ADC16_CAPTURE_MAX) {
        printf("Sequence Test: captured %u results, stop.\n", (unsigned int)APP_ADC16_CAPTURE_MAX);
        seq_complete_flag = 0;
        return true;
    }
#endif

    /* Clear the flag */
    seq_complete_flag = 0;

#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    /* Start the trigger source output */
    start_trigger_source(APP_ADC16_HW_TRIG_SRC);

    adc16_seq_enable_hw_trigger(BOARD_APP_ADC16_BASE);
#endif
    return false;
}

hpm_stat_t init_preemption_config(void)
{
    adc16_channel_config_t ch_cfg;
    hpm_stat_t st;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.sample_cycle = APP_ADC16_CH_SAMPLE_CYCLE;
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
    ch_cfg.adc_loop_exp = APP_ADC16_CH_ADC_LOOP_EXP;
#endif

    for (uint32_t i = 0; i < sizeof(trig_adc_channel); i++) {
        ch_cfg.ch = trig_adc_channel[i];
        st = adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
        if (st != status_success) {
            return st;
        }
    }

    /* Trigger target initialization */
    init_trigger_target(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_TRIG_CH);

    /* Set DMA start address for preemption mode */
    adc16_init_pmt_dma(BOARD_APP_ADC16_BASE, core_local_mem_to_sys_address(APP_ADC16_CORE, (uint32_t)pmt_buff));

#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    /* TRGM + PWM before IRQ so the first HW edge does not fire before the path is armed */
    init_trigger_mux(APP_ADC16_HW_TRGM, APP_ADC16_HW_TRGM_IN, APP_ADC16_HW_TRGM_OUT_PMT);
    init_trigger_source(APP_ADC16_HW_TRIG_SRC);
#endif

    /* Enable trigger complete interrupt */
    adc16_enable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_IRQ_EVENT);
#if APP_ADC16_CAPTURE_MAX > 0
    s_preemption_capture_count = 0U;
#endif
    return status_success;
}

bool preemption_handler(void)
{
#if defined(ADC_SOC_NO_HW_TRIG_SRC) || defined(__ADC16_USE_SW_TRIG)
    /* SW trigger */
    adc16_trigger_pmt_by_sw(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_TRIG_CH);
#endif

    /* Wait for a complete of conversion */
    while (trig_complete_flag == 0) {

    }

#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    /* Stop the trigger source output */
    stop_trigger_source(APP_ADC16_HW_TRIG_SRC);
#endif

    /* Process data */
    process_pmt_data(pmt_buff, APP_ADC16_PMT_TRIG_CH * sizeof(adc16_pmt_dma_data_t), sizeof(trig_adc_channel));

    /* Clear memory */
    memset(pmt_buff, 0x00, sizeof(pmt_buff));

    /* Clear the flag */
    trig_complete_flag = 0;

#if APP_ADC16_CAPTURE_MAX > 0
    s_preemption_capture_count += (uint32_t)sizeof(trig_adc_channel);
    if (s_preemption_capture_count >= APP_ADC16_CAPTURE_MAX) {
        printf("Preemption Test: captured %u results, stop.\n", (unsigned int)APP_ADC16_CAPTURE_MAX);
        return true;
    }
#endif

#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    /* Start the trigger source output */
    start_trigger_source(APP_ADC16_HW_TRIG_SRC);
#endif
    return false;
}

bool abort_handler(uint8_t conv_mode)
{
    if (console_try_receive_byte() == ' ') {

    #if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
        if (conv_mode == adc16_conv_mode_sequence) {
            adc16_seq_disable_hw_trigger(BOARD_APP_ADC16_BASE);
        }

        if (conv_mode == adc16_conv_mode_preemption) {
            adc16_disable_pmt_queue(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_TRIG_CH);
        }

        stop_trigger_source(APP_ADC16_HW_TRIG_SRC);
    #else
        (void) conv_mode;
    #endif
        current_cycle_bit = 0;

        return true;
    } else {
        return false;
    }
}

int main(void)
{
    uint8_t conv_mode;
    hpm_stat_t mode_cfg_st = status_success;

    /* Bsp initialization */
    board_init();

    /* ADC pin initialization */
    board_init_adc16_pins();

    /* ADC clock initialization */
    board_init_adc_clock(BOARD_APP_ADC16_BASE, true);

    printf("This is an ADC16 demo:\n");

    while (1) {
        /* Get a conversion mode from a console window */
        conv_mode = get_adc_conv_mode();

        /* ADC16 common initialization */
        init_common_config(conv_mode);

        /* ADC16 read patter and DMA initialization */
        switch (conv_mode) {
        case adc16_conv_mode_oneshot:
            mode_cfg_st = init_oneshot_config();
            break;

        case adc16_conv_mode_period:
            mode_cfg_st = init_period_config();
            break;

        case adc16_conv_mode_sequence:
            mode_cfg_st = init_sequence_config();
            break;

        case adc16_conv_mode_preemption:
            mode_cfg_st = init_preemption_config();
            break;

        default:
            break;
        }

        if (mode_cfg_st != status_success) {
            printf("ADC mode initialization failed (status=%d). Check channel and sampling parameters, then select again.\n", (int)mode_cfg_st);
            continue;
        }

        /* Main loop */
        while (1) {
            channel_result_out_of_threshold_handler();

            if (conv_mode == adc16_conv_mode_oneshot) {
                if (oneshot_handler()) {
                    break;
                }
            } else if (conv_mode == adc16_conv_mode_period) {
                if (period_handler()) {
                    break;
                }
            } else if (conv_mode == adc16_conv_mode_sequence) {
                if (sequence_handler()) {
                    break;
                }
            } else if (conv_mode == adc16_conv_mode_preemption) {
                if (preemption_handler()) {
                    break;
                }
            } else {
                printf("Conversion mode is not supported!\n");
            }

            if (abort_handler(conv_mode)) {
                break;
            }
        }
    }
}
