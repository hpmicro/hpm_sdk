/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sdm_drv.h"
#include "hpm_trgmmux_src.h"
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#if defined(BOARD_SDM_SENSOR_REQUIRE_CLK) && BOARD_SDM_SENSOR_REQUIRE_CLK
#include "hpm_pwmv2_drv.h"
#endif

#define TEST_SDM         BOARD_SDM
#define TEST_SDM_IRQ     BOARD_SDM_IRQ
#define TEST_SDM_CHANNEL BOARD_SDM_CHANNEL

/* trgmux info */
#define TEST_SDM_TRGM                     BOARD_SDM_TRGM
#define TEST_SDM_TRGM_GPTMR               BOARD_SDM_TRGM_GPTMR
#define TEST_SDM_TRGM_GPTMR_CH            BOARD_SDM_TRGM_GPTMR_CH
#define TEST_SDM_TRGM_INPUT_SRC           BOARD_SDM_TRGM_INPUT_SRC
#define TEST_SDM_TRGM_OUTPUT_DST          BOARD_SDM_TRGM_OUTPUT_DST
#define TEST_SDM_SYNC_SRC                 BOARD_SDM_TRGM_SYNC_SRC

#define  TEST_DATA_COUNT  (1024U)
int32_t filter_result[TEST_DATA_COUNT];

uint32_t count;
volatile bool receive_finish;
volatile bool receive_failed;
volatile bool comparator_out_high_threshold;

void sdm_isr(void)
{
    /* data error */
    if (sdm_get_channel_data_error_status(TEST_SDM, TEST_SDM_CHANNEL)) {
        /* comparator out threshold event */
        if (sdm_get_channel_comparator_status(TEST_SDM, BOARD_SDM_CHANNEL) & sdm_comparator_event_out_high_threshold) {
            comparator_out_high_threshold = true;
        }
        /* stop sdm channel */
        sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);
        receive_failed = true;
    }

    /* data ready */
    if (sdm_get_channel_data_ready_status(TEST_SDM, TEST_SDM_CHANNEL)) {
        uint8_t fifo_threshold = sdm_get_ch_fifo_threshold(TEST_SDM, TEST_SDM_CHANNEL);
        for (uint8_t i = 0; i < fifo_threshold; i++) {
            filter_result[count++] = sdm_get_channel_fifo_data(TEST_SDM, TEST_SDM_CHANNEL);
        }
        if (count >= TEST_DATA_COUNT) {
            /* stop sdm channel */
            sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);
            receive_finish = true;
        }
    }
}

SDK_DECLARE_EXT_ISR_M(TEST_SDM_IRQ, sdm_isr)

void test_sdm_filter_receive(void)
{
    hpm_stat_t stat;
    sdm_control_t control;
    sdm_filter_config_t filter_config;

    /* config sdm module: clock signal sync, data signal sync and interrupt */
    sdm_get_default_module_control(TEST_SDM, &control);
    sdm_init_module(TEST_SDM, &control);

    /* config channel: sampling mode and interrupt */
    sdm_channel_common_config_t ch_config;
    ch_config.sampling_mode = sdm_sampling_rising_clk_edge;
    ch_config.enable_err_interrupt = false;
    ch_config.enable_data_ready_interrupt = false;
    sdm_config_channel_common_setting(TEST_SDM, TEST_SDM_CHANNEL, &ch_config);

    /* config filter: filter type, oversampling rate, output data length, etc. */
    sdm_get_channel_default_filter_config(TEST_SDM, &filter_config);
    filter_config.filter_type = sdm_filter_sinc3;
    filter_config.oversampling_rate = 256; /* 1- 256 */
    filter_config.ignore_invalid_samples = 2;
    sdm_config_channel_filter(TEST_SDM, TEST_SDM_CHANNEL, &filter_config);

    /* start sdm channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* receive data from fifo, data is 32bit length */
    do {
        stat = sdm_receive_filter_data(TEST_SDM, TEST_SDM_CHANNEL, true, (int8_t *)filter_result, TEST_DATA_COUNT, 4U);
    } while (stat != status_success);

    /* stop sdm channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);

    printf("printf sdm received data.\n");
    for (uint32_t i = 0; i < TEST_DATA_COUNT; i++) {
        printf("%d\n", filter_result[i]);
    }
}

void test_sdm_comparator_interrupt(void)
{
    sdm_control_t control;
    sdm_comparator_config_t cmp_config;
    comparator_out_high_threshold = false;

    /* config sdm module: clock signal sync, data signal sync and interrupt */
    sdm_get_default_module_control(TEST_SDM, &control);
    control.interrupt_en = true;
    sdm_init_module(TEST_SDM, &control);

    /* enable interrupt */
    intc_m_enable_irq_with_priority(TEST_SDM_IRQ, 1);

    /* config channel: sampling mode and interrupt */
    sdm_channel_common_config_t ch_config;
    ch_config.sampling_mode = sdm_sampling_rising_clk_edge;
    ch_config.enable_err_interrupt = true;
    ch_config.enable_data_ready_interrupt = false;
    sdm_config_channel_common_setting(TEST_SDM, TEST_SDM_CHANNEL, &ch_config);

    /* config comparator: threshold value, filter type, oversampling rate, etc. */
    sdm_get_channel_default_comparator_config(TEST_SDM, &cmp_config);
    cmp_config.high_threshold = 0x7700U; /* uint16_t */
    cmp_config.en_high_threshold_int = true;
    cmp_config.filter_type = sdm_filter_sinc3;
    cmp_config.ignore_invalid_samples = 2;
    sdm_config_channel_comparator(TEST_SDM, TEST_SDM_CHANNEL, &cmp_config);

    /* start sdm channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    while (1) {
        if (comparator_out_high_threshold) {
            printf("sdm channel comparator out high threshold\n");
            printf("comparator value: 0x%x\n", sdm_get_channel_comparator_data(TEST_SDM, TEST_SDM_CHANNEL));
            break;
        }
    }
}

void test_sdm_filter_receive_interrupt(void)
{
    sdm_control_t control;
    sdm_filter_config_t filter_config;
    receive_finish = false;
    receive_failed = false;

    /* config sdm module: clock signal sync, data signal sync and interrupt */
    sdm_get_default_module_control(TEST_SDM, &control);
    control.interrupt_en = true;
    sdm_init_module(TEST_SDM, &control);

    /* enable interrupt */
    intc_m_enable_irq_with_priority(TEST_SDM_IRQ, 1);

    /* config channel: sampling mode and interrupt */
    sdm_channel_common_config_t ch_config;
    ch_config.sampling_mode = sdm_sampling_rising_clk_edge;
    ch_config.enable_err_interrupt = true;
    ch_config.enable_data_ready_interrupt = true;
    sdm_config_channel_common_setting(TEST_SDM, TEST_SDM_CHANNEL, &ch_config);

    /* config filter: filter type, oversampling rate, output data length, etc. */
    sdm_get_channel_default_filter_config(TEST_SDM, &filter_config);
    filter_config.filter_type = sdm_filter_sinc3;
    filter_config.oversampling_rate = 256; /* 1- 256 */
    filter_config.ignore_invalid_samples = 2;
    sdm_config_channel_filter(TEST_SDM, TEST_SDM_CHANNEL, &filter_config);

    /* start sdm channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* waiting for received data */
    while ((!receive_finish) && (!receive_failed)) {
    }

    if (receive_failed) {
        printf("printf sdm received data failed.\n");
    } else {
        printf("printf sdm received data.\n");
        for (uint32_t i = 0; i < TEST_DATA_COUNT; i++) {
            printf("%d\n", filter_result[i]);
        }
    }
}

void test_sdm_sync_filter_receive(void)
{
    sdm_control_t control;
    sdm_filter_config_t filter_config;
    receive_finish = false;
    receive_failed = false;

    /* config sdm module: clock signal sync, data signal sync and interrupt */
    sdm_get_default_module_control(TEST_SDM, &control);
    control.interrupt_en = true;
    sdm_init_module(TEST_SDM, &control);

    /* enable interrupt */
    intc_m_enable_irq_with_priority(TEST_SDM_IRQ, 1);

    /* config channel: sampling mode and interrupt */
    sdm_channel_common_config_t ch_config;
    ch_config.sampling_mode = sdm_sampling_rising_clk_edge;
    ch_config.enable_err_interrupt = true;
    ch_config.enable_data_ready_interrupt = true;
    sdm_config_channel_common_setting(TEST_SDM, TEST_SDM_CHANNEL, &ch_config);

    /* config filter: filter type, oversampling rate, output data length, etc. */
    sdm_get_channel_default_filter_config(TEST_SDM, &filter_config);
    filter_config.filter_type = sdm_filter_sinc3;
    filter_config.oversampling_rate = 256; /** 1- 256 */
    filter_config.ignore_invalid_samples = 2;
    filter_config.sync_source = TEST_SDM_SYNC_SRC;
    filter_config.fifo_clean_on_sync = 0; /** new sync event will clear fifo */
    filter_config.wtsynaclr = 0; /** sync flag clean by software */
    filter_config.wtsynmclr = 0;
    filter_config.wtsyncen = 1; /** after sync occurred, push data into fifo */
    sdm_config_channel_filter(TEST_SDM, TEST_SDM_CHANNEL, &filter_config);

    /* start sdm channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* config gptmr to trim sdm */
    trgm_output_update_source(TEST_SDM_TRGM, TEST_SDM_TRGM_OUTPUT_DST, TEST_SDM_TRGM_INPUT_SRC);

    /* config gptmr */
    gptmr_channel_config_t config;
    gptmr_channel_get_default_config(TEST_SDM_TRGM_GPTMR, &config);
    config.reload = 0xFFFFFFFFUL;
    config.cmp[0] = 0x20000;
    config.cmp_initial_polarity_high = false;
    gptmr_channel_config(TEST_SDM_TRGM_GPTMR, TEST_SDM_TRGM_GPTMR_CH, &config, false);
    gptmr_start_counter(TEST_SDM_TRGM_GPTMR, TEST_SDM_TRGM_GPTMR_CH);

    /* waiting for received data */
    while ((!receive_finish) && (!receive_failed)) {
    }

    if (receive_failed) {
        printf("printf sdm received data failed.\n");
    } else {
        printf("printf sdm received data.\n");
        for (uint32_t i = 0; i < TEST_DATA_COUNT; i++) {
            printf("%d\n", filter_result[i]);
        }
    }
}

#if defined(BOARD_SDM_SENSOR_REQUIRE_CLK) && BOARD_SDM_SENSOR_REQUIRE_CLK
void gen_pwm_as_sdm_clock(void)
{
    uint32_t freq, reload;
    freq = clock_get_frequency(BOARD_SDM_CLK_PWM_CLK_NAME);
    reload = freq / 10000000; /* 10MHz */

    init_pwm_pin_as_sdm_clock();

    pwmv2_disable_counter(BOARD_SDM_CLK_PWM, pwm_counter_1);
    pwmv2_reset_counter(BOARD_SDM_CLK_PWM, pwm_counter_1);
    pwmv2_shadow_register_unlock(BOARD_SDM_CLK_PWM);

    pwmv2_set_shadow_val(BOARD_SDM_CLK_PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(BOARD_SDM_CLK_PWM, PWMV2_SHADOW_INDEX(1), ((reload >> 1) - (reload >> 2)), 0, false);
    pwmv2_set_shadow_val(BOARD_SDM_CLK_PWM, PWMV2_SHADOW_INDEX(2), ((reload >> 1) + (reload >> 2)), 0, false);

    pwmv2_counter_select_data_offset_from_shadow_value(BOARD_SDM_CLK_PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(BOARD_SDM_CLK_PWM, pwm_counter_1);
    pwmv2_set_reload_update_time(BOARD_SDM_CLK_PWM, pwm_counter_1, pwm_reload_update_on_reload);

    pwmv2_select_cmp_source(BOARD_SDM_CLK_PWM, PWMV2_CMP_INDEX(4), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(BOARD_SDM_CLK_PWM, PWMV2_CMP_INDEX(5), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_select_cmp_source(BOARD_SDM_CLK_PWM, PWMV2_CMP_INDEX(6), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_select_cmp_source(BOARD_SDM_CLK_PWM, PWMV2_CMP_INDEX(7), cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));

    pwmv2_shadow_register_lock(BOARD_SDM_CLK_PWM);
    pwmv2_disable_four_cmp(BOARD_SDM_CLK_PWM, BOARD_SDM_CLK_PWM_OUT);
    pwmv2_channel_enable_output(BOARD_SDM_CLK_PWM, BOARD_SDM_CLK_PWM_OUT);
    pwmv2_enable_counter(BOARD_SDM_CLK_PWM, pwm_counter_1);
    pwmv2_start_pwm_output(BOARD_SDM_CLK_PWM, pwm_counter_1);
}
#endif

int main(void)
{
    board_init();
    init_sdm_pins();
    printf("sdm example\n");

#if defined(BOARD_SDM_SENSOR_REQUIRE_CLK) && BOARD_SDM_SENSOR_REQUIRE_CLK
    gen_pwm_as_sdm_clock();
#endif

    /* sdm receive filter data by polling mode */
    test_sdm_filter_receive();

    /* sdm comparator */
    /* test_sdm_comparator_interrupt(); */

    /* sdm receive filter data by interrupt mode */
    /* test_sdm_filter_receive_interrupt(); */

    /* sdm receive filter data with sync signal */
    /* test_sdm_sync_filter_receive(); */

    while (1) {
    };

    return 0;
}
