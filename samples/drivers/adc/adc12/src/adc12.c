/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_adc12_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#include "hpm_synt_drv.h"

#ifndef BOARD_APP_ADC_TRIG_PWM0
#define BOARD_APP_ADC_TRIG_PWM0    HPM_PWM0
#endif

#ifndef BOARD_APP_ADC_TRIG_PWM1
#define BOARD_APP_ADC_TRIG_PWM1    HPM_PWM1
#endif

#ifndef BOARD_APP_ADC_TRIG_TRGM0
#define BOARD_APP_ADC_TRIG_TRGM0    HPM_TRGM0
#endif

#ifndef BOARD_APP_ADC_TRIG_TRGM1
#define BOARD_APP_ADC_TRIG_TRGM1    HPM_TRGM1
#endif

#ifndef BOARD_APP_ADC_TRIG_PWM_SYNC
#define BOARD_APP_ADC_TRIG_PWM_SYNC HPM_SYNT
#endif

#ifndef BOARD_APP_ADC_SINGLE_CONV_CNT
#define BOARD_APP_ADC_SINGLE_CONV_CNT   (6)
#endif

#ifndef BOARD_APP_ADC12_IRQn
#define BOARD_APP_ADC12_IRQn IRQn_ADC0
#endif

#define BOARD_APP_CORE  HPM_CORE0 /* Before programming the demo application, please double check the core number! */

__attribute__ ((aligned(ADC_SOC_DMA_ADDR_ALIGNMENT))) uint32_t seq_buff[BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES];
__attribute__ ((aligned(ADC_SOC_DMA_ADDR_ALIGNMENT))) uint32_t pmt_buff[BOARD_APP_ADC_PMT_DMA_SIZE_IN_4BYTES];

uint8_t seq_dma_stop_flag;
uint8_t trig_complete_flag;

void isr_adc12(void)
{
    uint32_t status;

    status = adc12_get_status_flags(BOARD_APP_ADC12_BASE);

    if (ADC12_INT_STS_SEQ_DMAABT_GET(status)) {
        /* Clear seq_dmaabt status */
        adc12_clear_status_flags(BOARD_APP_ADC12_BASE, adc12_event_seq_dma_abort);
        /* Set flag to read memory data */
        seq_dma_stop_flag = 1;
    }

    if (ADC12_INT_STS_TRIG_CMPT_GET(status)) {
        /* Clear trig_cmpt status */
        adc12_clear_status_flags(BOARD_APP_ADC12_BASE, adc12_event_trig_complete);
        /* Set flag to read memory data */
        trig_complete_flag = 1;
    }
}
SDK_DECLARE_EXT_ISR_M(IRQn_ADC0, isr_adc12)

void process_seq_data(uint32_t *buff, uint32_t len)
{
    adc12_seq_dma_data_t dma_data[len];

    printf("%s\n", __FUNCTION__);

    for (int i; i < len; i++) {
        printf("Cycle Bit: %02x\n", dma_data[i].cycle_bit);
        printf("ADC Channel: %02x\n", dma_data[i].adc_ch);
        printf("Sequence Number:%d\n", dma_data[i].seq_num);
        printf("Result: %02x\n", dma_data[i].result);
    }
}

void process_pmt_data(uint32_t *buff, uint32_t len)
{
    adc12_pmt_dma_data_t dma_data[len];

    printf("%s\n", __FUNCTION__);

    for (int i; i < len; i++) {
        printf("Cycle Bit: %02x\n", dma_data[i].cycle_bit);
        printf("ADC Channel: %02x\n", dma_data[i].adc_ch);
        printf("Trig Index:%d\n", dma_data[i].trig_index);
        printf("Trig Channel:%d\n", dma_data[i].trig_ch);
        printf("Result: %02x\n", dma_data[i].result);
    }
}

void init_trigger(PWM_Type * ptr)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;

    /* TODO: Set PWM Clock Source and divider */

    /* 10KHz reload at 200MHz */
    pwm_set_reload(ptr, 0, 399);

    /* Set comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp  = false;
    pwm_cmp_cfg.mode           = pwm_cmp_mode_output_compare;

    /* Select comp8 and trigger at 0.5us */
    pwm_cmp_cfg.cmp = 200;
    pwm_config_cmp(ptr, 8, &pwm_cmp_cfg);

    /* Select comp9 and trigger at 1.0us */
    pwm_cmp_cfg.cmp = 400;
    pwm_config_cmp(ptr, 9, &pwm_cmp_cfg);

    /* Select comp10 and trigger at 1.5us */
    pwm_cmp_cfg.cmp = 600;
    pwm_config_cmp(ptr, 10, &pwm_cmp_cfg);

    /* Select comp12 and generate interrupt at 0.6us */
    pwm_cmp_cfg.cmp = 240;
    pwm_config_cmp(ptr, 12, &pwm_cmp_cfg);

    /* Write lock and load shadow register */
    pwm_shadow_register_lock(ptr);

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = 8;  /* start channel 8 */
    pwm_output_ch_cfg.cmp_end_index   = 8;  /* end channel 8 */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, 8, &pwm_output_ch_cfg);

    /* Set comparator channel for trigger b */
    pwm_output_ch_cfg.cmp_start_index = 9;   /* start channel 9 */
    pwm_output_ch_cfg.cmp_end_index   = 9;   /* end channel 9 */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, 9, &pwm_output_ch_cfg);

    /* Set comparator channel for trigger c */
    pwm_output_ch_cfg.cmp_start_index = 10;  /* start channel 10 */
    pwm_output_ch_cfg.cmp_end_index   = 10;  /* end channel 10 */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, 10, &pwm_output_ch_cfg);

    /* Set comparator channel for irq */
    pwm_output_ch_cfg.cmp_start_index = 12;  /* start channel 12 */
    pwm_output_ch_cfg.cmp_end_index   = 12;  /* end channel 12 */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, 12, &pwm_output_ch_cfg);
}

void init_trigger_mux(TRGM_Type * ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_SYNT0_CH0;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
    trgm_output_config(ptr, TRGM_TRGOCFG_PWM_SYNCI, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;

    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PWM0_CH8REF;
    trgm_output_config(ptr, TRGM_TRGOCFG_ADCX_PTRGI0A, &trgm_output_cfg);

    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PWM0_CH9REF;
    trgm_output_config(ptr, TRGM_TRGOCFG_ADCX_PTRGI0B, &trgm_output_cfg);

    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PWM0_CH10REF;
    trgm_output_config(ptr, TRGM_TRGOCFG_ADCX_PTRGI0C, &trgm_output_cfg);
}

void init_pwm_sync(SYNT_Type *ptr)
{
    synt_enable_counter(ptr, false);
    synt_reset_counter(ptr);
    synt_set_reload(ptr, 19);
    synt_set_comparator(ptr, SYNT_CMP_0, 15);
}

void init_trigger_cfg(ADC12_Type *ptr, uint8_t trig_ch, bool inten)
{
    adc12_pmt_config_t pmt_cfg;
    pmt_cfg.trig_ch   = trig_ch;
    pmt_cfg.trig_len  = BOARD_APP_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.adc_ch[0] = BOARD_APP_ADC12_CH;
    pmt_cfg.inten[0]  = inten;

    adc12_set_pmt_config(ptr, &pmt_cfg);
}

void init_common_config(adc12_conversion_mode_t conv_mode)
{
    adc12_config_t cfg;
    adc12_channel_config_t ch_cfg;

    /* initialize an ADC instance */
    adc12_get_default_config(&cfg);

    cfg.res            = adc12_res_12_bits;
    cfg.conv_mode      = conv_mode;
    cfg.adc_clk_div    = 1;
    cfg.sel_sync_ahb   = true;
    if (cfg.conv_mode == adc12_conv_mode_sequence ||
        cfg.conv_mode == adc12_conv_mode_preemption) {
        cfg.adc_ahb_en = true;
    }

    adc12_init(BOARD_APP_ADC12_BASE, &cfg);

    /* initialize an ADC channel */
    ch_cfg.ch           = BOARD_APP_ADC12_CH;
    ch_cfg.diff_sel     = adc12_sample_signal_single_ended;
    ch_cfg.sample_cycle = 20;

    adc12_init_channel(BOARD_APP_ADC12_BASE, &ch_cfg);
}

void init_oneshot_config(void)
{
}

void oneshot_handler(void)
{
    uint16_t result;

    if (adc12_get_wait_dis_status(BOARD_APP_ADC12_BASE) == false) {
        adc12_get_oneshot_result(BOARD_APP_ADC12_BASE, BOARD_APP_ADC12_CH, &result);
    }
    else {
        do {
            adc12_get_oneshot_result(BOARD_APP_ADC12_BASE, BOARD_APP_ADC12_CH, &result);
        } while (adc12_get_conv_valid_status(BOARD_APP_ADC12_BASE, BOARD_APP_ADC12_CH) == false);
    }

    printf("%s [channel %d] result: %08x\n", BOARD_APP_ADC12_NAME, BOARD_APP_ADC12_CH, result);
}

void init_period_config(void)
{
    adc12_prd_config_t prd_cfg;

    prd_cfg.ch           = BOARD_APP_ADC12_CH;
    prd_cfg.prescale     = 0;  /* Set divider 0: 1 x clock */
    prd_cfg.period_in_ms = 100;   /* 500ns when AHB clock at 200MHz is ADC clock source */
    /* TODO: Call a clock API to get ADC clock source frequency (unit in Hz) */
    /* prd_cfg.clk_src_freq_in_hz =  */

    adc12_set_prd_config(BOARD_APP_ADC12_BASE, &prd_cfg);
}

void period_handler(void)
{
    uint16_t result;

    adc12_get_prd_result(BOARD_APP_ADC12_BASE, BOARD_APP_ADC12_CH, &result);
    printf("%s [channel %d] result: %08x\n", BOARD_APP_ADC12_NAME, BOARD_APP_ADC12_CH, result);
}

void init_sequence_config(void)
{
    adc12_seq_config_t seq_cfg;
    adc12_dma_config_t dma_cfg;

    /* Set a sequence config */
    seq_cfg.seq_len    = ADC_SOC_MAX_SEQ_LEN;
    seq_cfg.restart_en = true;
    seq_cfg.cont_en    = true;
    seq_cfg.sw_trig_en = true;
    seq_cfg.hw_trig_en = true;

    /* Initialize a sequence */
    adc12_set_seq_config(BOARD_APP_ADC12_BASE, &seq_cfg);

    /* Set DMA cfg */
    dma_cfg.start_addr     = (uint32_t *)core_local_mem_to_sys_address(BOARD_APP_CORE, (uint32_t)seq_buff);
    dma_cfg.size_in_4bytes = BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES;
    dma_cfg.stop_en        = true;
    dma_cfg.stop_pos       = BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES / 2;

    /* Initialize DMA for the sequence mode */
    adc12_init_seq_dma(BOARD_APP_ADC12_BASE, &dma_cfg);

    /* Enable DMA abort interrupt */
    adc12_enable_interrupts(BOARD_APP_ADC12_BASE, adc12_event_seq_dma_abort);

    /* SW trigger and wait for an interrupt of DMA abort */
    adc12_trigger_seq_by_sw(BOARD_APP_ADC12_BASE);
}

void sequence_handler(void)
{
    static uint16_t seq_dma_stop_pos_index = BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES / 2;

    while (seq_dma_stop_flag == 0) {}

    /* Switch stop position index */
    seq_dma_stop_pos_index = seq_dma_stop_pos_index == 0 ? (BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES / 2) : 0;

    /* Update the stop_pos fields */
    adc12_set_seq_stop_pos(BOARD_APP_ADC12_BASE, seq_dma_stop_pos_index);

    /* Process data */
    process_seq_data(&seq_buff[seq_dma_stop_pos_index], BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES / 2);

    /* Clear the flag */
    seq_dma_stop_flag = 0;
}

void init_preemption_config(void)
{
    /* Trigger source initialization */
    init_trigger(BOARD_APP_ADC_TRIG_PWM0);
    init_trigger(BOARD_APP_ADC_TRIG_PWM1);

    /* Trigger mux initialization */
    init_trigger_mux(BOARD_APP_ADC_TRIG_TRGM0);
    init_trigger_mux(BOARD_APP_ADC_TRIG_TRGM1);

    /* PWM timer sync initialization */
    init_pwm_sync(BOARD_APP_ADC_TRIG_PWM_SYNC);

    /* Trigger config initialization */
    init_trigger_cfg(BOARD_APP_ADC12_BASE, ADC12_CONFIG_TRG0A, false);
    init_trigger_cfg(BOARD_APP_ADC12_BASE, ADC12_CONFIG_TRG0B, false);
    init_trigger_cfg(BOARD_APP_ADC12_BASE, ADC12_CONFIG_TRG0C, false);

    init_trigger_cfg(BOARD_APP_ADC12_BASE, ADC12_CONFIG_TRG1A, false);
    init_trigger_cfg(BOARD_APP_ADC12_BASE, ADC12_CONFIG_TRG1B, false);
    init_trigger_cfg(BOARD_APP_ADC12_BASE, ADC12_CONFIG_TRG1C, true);

    /* Set DMA start address for preemption mode */
    adc12_init_pmt_dma(BOARD_APP_ADC12_BASE, core_local_mem_to_sys_address(BOARD_APP_CORE, (uint32_t)pmt_buff));
}

void preemption_handler(void)
{
    /* Wait for a complete of conversion */
    while (trig_complete_flag == 0) {}

    /* Process data */
    process_pmt_data(pmt_buff, BOARD_APP_ADC_SINGLE_CONV_CNT);

    /* Clear the flag */
    trig_complete_flag = 0;
}

int main(void)
{
    uint8_t conv_mode;

    /* Bsp initialization */
    board_init();

    /* ADC pin initialization */
    board_init_adc12_pins();

    /* TODO: Get sample mode from console */
    conv_mode = adc12_conv_mode_oneshot;

    /* ADC12 common initialization */
    init_common_config(conv_mode);

    /* enable irq */
    intc_m_enable_irq_with_priority(BOARD_APP_ADC12_IRQn, 1);

    printf("This is an ADC12 demo:\n");

    /* ADC12 read patter and DMA initialization */
    switch (conv_mode) {
        case adc12_conv_mode_oneshot:
            init_oneshot_config();
            break;

        case adc12_conv_mode_period:
            init_period_config();
            break;

        case adc12_conv_mode_sequence:
            init_sequence_config();
            break;

        case adc12_conv_mode_preemption:
            init_preemption_config();
            break;

        default:
            break;
    }

    /* Main loop */
    while (1) {
        if (conv_mode == adc12_conv_mode_oneshot) {
            oneshot_handler();
        } else if (conv_mode == adc12_conv_mode_period) {
            period_handler();
        } else if (conv_mode == adc12_conv_mode_sequence) {
            sequence_handler();
        } else if (conv_mode == adc12_conv_mode_preemption) {
            preemption_handler();
        } else {
            printf("Conversion mode is not supported!\n");
        }
    }
}
