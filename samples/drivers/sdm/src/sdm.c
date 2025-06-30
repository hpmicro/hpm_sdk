/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This example demonstrates the usage of SDM (Sigma-Delta Modulator) module
 * SDM is used to decode delta-sigma modulated signals from current measurements
 * or rotor position measurements in motor control applications.
 *
 * The example shows:
 * 1. Basic data sampling using polling mode
 * 2. Amplitude threshold detection using comparator
 * 3. Data sampling using interrupt mode
 * 4. Synchronized data sampling using external trigger
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

/* Board specific SDM configurations */
#define TEST_SDM         BOARD_SDM
#define TEST_SDM_IRQ     BOARD_SDM_IRQ
#define TEST_SDM_CHANNEL BOARD_SDM_CHANNEL

/* trgmux configurations for synchronization */
#define TEST_SDM_TRGM                     BOARD_SDM_TRGM
#define TEST_SDM_TRGM_GPTMR               BOARD_SDM_TRGM_GPTMR
#define TEST_SDM_TRGM_GPTMR_CLK           BOARD_SDM_TRGM_GPTMR_CLK
#define TEST_SDM_TRGM_GPTMR_CH            BOARD_SDM_TRGM_GPTMR_CH
#define TEST_SDM_TRGM_INPUT_SRC           BOARD_SDM_TRGM_INPUT_SRC
#define TEST_SDM_TRGM_OUTPUT_DST          BOARD_SDM_TRGM_OUTPUT_DST
#define TEST_SDM_SYNC_SRC                 BOARD_SDM_TRGM_SYNC_SRC

/* High threshold for amplitude detection (240mV) */
#define SDM_AMPLITUDE_HIGH_THRESHOLD (240)

/* Buffer for storing filtered data samples */
#define  TEST_DATA_COUNT  (16U)
int32_t filter_result[TEST_DATA_COUNT];
uint32_t count;

/* Status flags */
volatile bool receive_finish;              /* Flag indicating data reception complete */
volatile bool receive_failed;              /* Flag indicating data reception failed */
volatile bool comparator_out_high_threshold; /* Flag indicating amplitude exceeded threshold */
volatile bool data_ready_in_sync_mode;     /* Flag indicating data ready in sync mode */

/* Function declarations */
double sdm_data_value_to_voltage(sdm_filter_config_t *config, int32_t sdm_data);
double sdm_amplitude_value_to_voltage(sdm_filter_type_t filter, uint16_t oversample, uint16_t amplitude);
uint16_t voltage_to_sdm_amplitude_value(sdm_filter_type_t filter, uint16_t oversample, double voltage);

/* SDM interrupt handler */
SDK_DECLARE_EXT_ISR_M(TEST_SDM_IRQ, sdm_isr)
void sdm_isr(void)
{
    /* Handle data error events */
    if (sdm_get_channel_data_error_status(TEST_SDM, TEST_SDM_CHANNEL)) {
        /* Check for amplitude threshold crossing */
        if (sdm_get_channel_comparator_status(TEST_SDM, BOARD_SDM_CHANNEL) & sdm_comparator_event_out_high_threshold) {
            comparator_out_high_threshold = true;
            /* Stop SDM channel and comparator when threshold exceeded */
            sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);
            sdm_enable_channel_comparator(TEST_SDM, BOARD_SDM_CHANNEL, false);
            /* Only when the actual over the limit disappears, the corresponding flag can be cleared */
            sdm_clean_channel_comparator_status(TEST_SDM, BOARD_SDM_CHANNEL, sdm_comparator_event_out_high_threshold);
        }

        /* Check for data overflow or saturation errors */
        if (sdm_get_channel_filter_status(TEST_SDM, BOARD_SDM_CHANNEL) & (SDM_CH_SDST_DOV_ERR_MASK | SDM_CH_SDST_DSAT_ERR_MASK)) {
            /* stop sdm channel data function */
            sdm_enable_channel_filter(TEST_SDM, TEST_SDM_CHANNEL, false);
            receive_failed = true;
            sdm_clean_channel_filter_status(TEST_SDM, TEST_SDM_CHANNEL, (SDM_CH_SDST_DOV_ERR_MASK | SDM_CH_SDST_DSAT_ERR_MASK));
        }
    }

    /* Handle data ready event */
    if (sdm_get_channel_data_ready_status(TEST_SDM, TEST_SDM_CHANNEL)) {
        data_ready_in_sync_mode = true; /* Set flag for sync mode */
        /* Read data from FIFO based on threshold */
        uint8_t fifo_threshold = sdm_get_ch_fifo_threshold(TEST_SDM, TEST_SDM_CHANNEL);
        for (uint8_t i = 0; i < fifo_threshold; i++) {
            if (sdm_output_is_32bit_data(TEST_SDM, TEST_SDM_CHANNEL)) {
                filter_result[count++] = sdm_get_channel_fifo_data(TEST_SDM, TEST_SDM_CHANNEL);
            } else {
                filter_result[count++] = sdm_get_channel_fifo_16bit_data(TEST_SDM, TEST_SDM_CHANNEL);
            }
        }
        /* Stop receiving when buffer is full */
        if (count >= TEST_DATA_COUNT) {
            /* stop sdm channel data function */
            sdm_enable_channel_filter(TEST_SDM, TEST_SDM_CHANNEL, false);
            receive_finish = true;
        }
    }
}

/* Test function for basic data sampling using polling mode */
void test_sdm_filter_receive(void)
{
    hpm_stat_t stat;
    sdm_control_t control;
    sdm_filter_config_t filter_config;

    /* config sdm module: clock signal sync, data signal sync and interrupt */
    sdm_get_default_module_control(TEST_SDM, &control);
    sdm_init_module(TEST_SDM, &control);

    /* Configure channel sampling mode */
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

    /* Start SDM channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* receive data from fifo, data is 32bit length */
    do {
        stat = sdm_receive_filter_data(TEST_SDM, TEST_SDM_CHANNEL, true, (int8_t *)filter_result, TEST_DATA_COUNT, 4U);
    } while (stat != status_success);

    /* Stop SDM channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);

    /* Print results */
    printf("sdm sample data in polling mode.\n");
    for (uint32_t i = 0; i < TEST_DATA_COUNT; i++) {
        double voltage = sdm_data_value_to_voltage(&filter_config, filter_result[i]);
        printf("%.2lfmV\n", voltage);
    }
}

/* sdm detecte amplitude over high limit */
/* set high limit to SDM_AMPLITUDE_HIGH_THRESHOLD */
/* when input voltage high than SDM_AMPLITUDE_HIGH_THRESHOLD, generate channel error interrupt */
void test_sdm_comparator_interrupt(void)
{
    sdm_control_t control;
    sdm_comparator_config_t cmp_config;
    comparator_out_high_threshold = false;

    /* config sdm module: clock signal sync, data signal sync and interrupt */
    sdm_get_default_module_control(TEST_SDM, &control);
    control.interrupt_en = true;
    sdm_init_module(TEST_SDM, &control);

    /* Enable interrupt handling */
    intc_m_enable_irq_with_priority(TEST_SDM_IRQ, 1);

    /* Config channel: sampling mode and interrupt */
    sdm_channel_common_config_t ch_config;
    ch_config.sampling_mode = sdm_sampling_rising_clk_edge;
    ch_config.enable_err_interrupt = true;
    ch_config.enable_data_ready_interrupt = false;
    sdm_config_channel_common_setting(TEST_SDM, TEST_SDM_CHANNEL, &ch_config);

    /* Config comparator: threshold value, filter type, oversampling rate, etc. */
    sdm_get_channel_default_comparator_config(TEST_SDM, &cmp_config);
    cmp_config.en_high_threshold_int = true;
    cmp_config.filter_type = sdm_filter_sinc3;
    cmp_config.ignore_invalid_samples = 2;
    uint16_t threshold = voltage_to_sdm_amplitude_value(cmp_config.filter_type, cmp_config.oversampling_rate, SDM_AMPLITUDE_HIGH_THRESHOLD);
    cmp_config.high_threshold = threshold; /* uint16_t */
    sdm_config_channel_comparator(TEST_SDM, TEST_SDM_CHANNEL, &cmp_config);

    printf("Set amplitude high threshold %dmV\n", SDM_AMPLITUDE_HIGH_THRESHOLD);

    /* Start SDM channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* Wait for threshold crossing */
    while (1) {
        if (comparator_out_high_threshold) {
            printf("sdm channel comparator detected amplitude over the high threshold\n");
            uint16_t amplitude = sdm_get_channel_comparator_data(TEST_SDM, TEST_SDM_CHANNEL);
            double voltage = sdm_amplitude_value_to_voltage(cmp_config.filter_type, cmp_config.oversampling_rate, amplitude);
            printf("voltage: %.2lfmV\n", voltage);
            break;
        }
    }

    /* Stop SDM channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);
}

/* sdm sample data by interrupt mode */
/* generate channel data ready interrupt when the data in FIFO reaches the threshold */
/* when sample data count = TEST_DATA_COUNT, stop sdm receive data function */
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

    /* Enable interrupt handling */
    intc_m_enable_irq_with_priority(TEST_SDM_IRQ, 1);

    /* Configure channel with both error and data ready interrupts */
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

    /* Start data reception */
    count = 0; /* received data count clean */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* Wait for reception complete or failure */
    while ((!receive_finish) && (!receive_failed)) {
    }

    /* Print results */
    if (receive_failed) {
        printf("sdm sample data failed in interrupt mode.\n");
    } else {
        printf("sdm sample data in interrupt mode.\n");
        for (uint32_t i = 0; i < TEST_DATA_COUNT; i++) {
            double voltage = sdm_data_value_to_voltage(&filter_config, filter_result[i]);
            printf("%.2lfmV\n", voltage);
        }
    }

    /* Stop SDM channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);
}

/* sdm sample data by interrupt mode and sync signal */
/* when sync event occur, start to sample data into FIFO*/
/* whene data in FIFO reaches the threshold generate data ready interrupt and hardware will automatically clear sync flag */
/* when sync flag is cleared, stop to sample data into FIFO until sync enver occur again */
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

    /* Enable interrupt handling */
    intc_m_enable_irq_with_priority(TEST_SDM_IRQ, 1);

    /* Configure channel with both error and data ready interrupts */
    sdm_channel_common_config_t ch_config;
    ch_config.sampling_mode = sdm_sampling_rising_clk_edge;
    ch_config.enable_err_interrupt = true;
    ch_config.enable_data_ready_interrupt = true;
    sdm_config_channel_common_setting(TEST_SDM, TEST_SDM_CHANNEL, &ch_config);

    /* config filter: filter type, oversampling rate, output data length, etc. */
    sdm_get_channel_default_filter_config(TEST_SDM, &filter_config);
    filter_config.fifo_threshold = 8;
    filter_config.filter_type = sdm_filter_sinc3;
    filter_config.oversampling_rate = 256; /** 1- 256 */
    filter_config.ignore_invalid_samples = 2;
    filter_config.sync_source = TEST_SDM_SYNC_SRC;
    filter_config.fifo_clean_on_sync = 1; /** new sync event will clear fifo */
    filter_config.wtsynaclr = 1; /** when data ready, hardware will automatically clear sync event flag and stop receiving data until sync event flag is set again */
    filter_config.wtsyncen = 1; /** after sync occurred, push data into fifo */
    sdm_config_channel_filter(TEST_SDM, TEST_SDM_CHANNEL, &filter_config);

    /* Start SDM channel */
    count = 0; /* received data count clean */
    data_ready_in_sync_mode = false;
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, true);

    /* Configure trigger mux for sync signal */
    trgm_output_update_source(TEST_SDM_TRGM, TEST_SDM_TRGM_OUTPUT_DST, TEST_SDM_TRGM_INPUT_SRC);

    /* Configure timer to generate sync signal (4s period) */
    gptmr_channel_config_t config;
    clock_add_to_group(TEST_SDM_TRGM_GPTMR_CLK, 0);
    gptmr_channel_get_default_config(TEST_SDM_TRGM_GPTMR, &config);
    uint32_t freq = clock_get_frequency(TEST_SDM_TRGM_GPTMR_CLK);
    config.reload = 4 * freq; /* 4s period */
    config.cmp[0] = 2 * freq; /* 2s duty */
    config.cmp[1] = config.reload;
    config.cmp_initial_polarity_high = false;
    gptmr_channel_config(TEST_SDM_TRGM_GPTMR, TEST_SDM_TRGM_GPTMR_CH, &config, false);
    gptmr_start_counter(TEST_SDM_TRGM_GPTMR, TEST_SDM_TRGM_GPTMR_CH);

    /* Sample data for 4 sync cycles */
    for (uint8_t i = 0; i < 4; i++) {
        while ((!data_ready_in_sync_mode) && (!receive_failed)) {
        }

        if (receive_failed) {
            printf("sdm sample data failed with sync signal.\n");
        } else {
            printf("sdm sample data with sync signal. %d time\n", i + 1);
            for (uint32_t i = 0; i < count; i++) {
                double voltage = sdm_data_value_to_voltage(&filter_config, filter_result[i]);
                printf("%.2lfmV\n", voltage);
            }
            count = 0;
        }
        data_ready_in_sync_mode = false;
    }

    /* Stop SDM channel */
    sdm_enable_channel(TEST_SDM, TEST_SDM_CHANNEL, false);
}

#if defined(BOARD_SDM_SENSOR_REQUIRE_CLK) && BOARD_SDM_SENSOR_REQUIRE_CLK
/* Generate PWM clock for SDM sensor if required */
void gen_pwm_as_sdm_clock(void)
{
    uint32_t freq, reload;
    pwm_counter_t pwm_counter = BOARD_SDM_CLK_PWM_OUT / 2;
    uint8_t pwm_cmp_1 = BOARD_SDM_CLK_PWM_OUT * 2;
    uint8_t pwm_cmp_2 = BOARD_SDM_CLK_PWM_OUT * 2 + 1;

    freq = clock_get_frequency(BOARD_SDM_CLK_PWM_CLK_NAME);
    reload = freq / 10000000; /* Generate 10MHz clock */

    init_pwm_pin_as_sdm_clock();

    /* deinit PWM peripheral */
    pwmv2_deinit(BOARD_SDM_CLK_PWM);
    /* disable shadow lock function, shadow will always access */
    pwmv2_shadow_register_unlock(BOARD_SDM_CLK_PWM);

    /* set shdow register value */
    pwmv2_set_shadow_val(BOARD_SDM_CLK_PWM, PWMV2_SHADOW_INDEX(0), reload, 0, false);
    pwmv2_set_shadow_val(BOARD_SDM_CLK_PWM, PWMV2_SHADOW_INDEX(1), ((reload >> 1) - (reload >> 2)), 0, false);
    pwmv2_set_shadow_val(BOARD_SDM_CLK_PWM, PWMV2_SHADOW_INDEX(2), ((reload >> 1) + (reload >> 2)), 0, false);

    /* config reload value from shadow, update valid when issue shadow lock */
    pwmv2_counter_select_data_offset_from_shadow_value(BOARD_SDM_CLK_PWM, pwm_counter, PWMV2_SHADOW_INDEX(0));
    pwmv2_set_reload_update_time(BOARD_SDM_CLK_PWM, pwm_counter, pwm_reload_update_on_shlk);

    /* config two cmp value from shadow, update valid when issue shadow lock */
    pwmv2_select_cmp_source(BOARD_SDM_CLK_PWM, pwm_cmp_1, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(1));
    pwmv2_cmp_update_trig_time(BOARD_SDM_CLK_PWM, pwm_cmp_1, pwm_shadow_register_update_on_shlk);
    pwmv2_select_cmp_source(BOARD_SDM_CLK_PWM, pwm_cmp_2, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(2));
    pwmv2_cmp_update_trig_time(BOARD_SDM_CLK_PWM, pwm_cmp_2, pwm_shadow_register_update_on_shlk);

    /* lock shadow register, the reload and cmp value will be updated by shadow */
    pwmv2_shadow_register_lock(BOARD_SDM_CLK_PWM);

    /* disable burst mode */
    pwmv2_counter_burst_disable(BOARD_SDM_CLK_PWM, pwm_counter);

    /* generate pwm with two cmp rather than 4 cmp */
    pwmv2_disable_four_cmp(BOARD_SDM_CLK_PWM, BOARD_SDM_CLK_PWM_OUT & 0xFFFE); /* Need to configure even channels */

    /* enable output channel */
    pwmv2_channel_enable_output(BOARD_SDM_CLK_PWM, BOARD_SDM_CLK_PWM_OUT);

    /* enable counter */
    pwmv2_enable_counter(BOARD_SDM_CLK_PWM, pwm_counter);

    /* start pwm counter */
    pwmv2_start_pwm_output(BOARD_SDM_CLK_PWM, pwm_counter);
}
#endif

/* Display menu of available test options */
void show_help(void)
{
    static const char help_info[] = ""
                                    "*********************************************************************\n"
                                    "*                                                                   *\n"
                                    "*                         SDM Example Menu                          *\n"
                                    "* Please enter one of following SDM function test(e.g. 1 or 2 ...): *\n"
                                    "* 1 - SDM sample data in polling mode                               *\n"
                                    "* 2 - SDM amplitude threshold check                                 *\n"
                                    "* 3 - SDM sample data in interrupt mode                             *\n"
                                    "* 4 - SDM sample data with sync signal                              *\n"
                                    "*                                                                   *\n"
                                    "*********************************************************************\n";
    printf("%s\n", help_info);
}

int main(void)
{
    /* Initialize board and clock */
    board_init();
    clock_add_to_group(clock_sdm0, 0);
    init_sdm_pins();
    printf("sdm example\n");

#if defined(BOARD_SDM_SENSOR_REQUIRE_CLK) && BOARD_SDM_SENSOR_REQUIRE_CLK
    gen_pwm_as_sdm_clock();
#endif

    /* Menu-driven test selection */
    char option;
    show_help();
    while (true) {
        option = getchar();
        printf("Selected option: %c\n\n", option);

        switch (option) {
        case '1':
            test_sdm_filter_receive();
            break;
        case '2':
            test_sdm_comparator_interrupt();
            break;
        case '3':
            test_sdm_filter_receive_interrupt();
            break;
        case '4':
            test_sdm_sync_filter_receive();
            break;
        default:
            show_help();
            break;
        }
    }

    while (1) {
    };

    return 0;
}

/* Calculate maximum output value for different filter types */
uint32_t sdm_filter_max_output_value(sdm_filter_type_t filter, uint16_t oversample)
{
    switch (filter) {
    case sdm_filter_sinc1:
        return oversample;
    case sdm_filter_sinc2:
        return oversample * oversample;
    case sdm_filter_sinc3:
        return oversample * oversample * oversample;
    case sdm_filter_fast_sinc2:
        return 2 * oversample * oversample;
    default:
        return 0;
    }
}

/* The following conversions are applicable to AD7400/NSI1306 */
/* if sigma-delta modulator different, please check these conversions */
#define MODULATOR_INPUT_MAX_VOLTAGE (320) /* input full-scale ±320 mV */
double sdm_data_value_to_voltage(sdm_filter_config_t *config, int32_t sdm_data)
{
    sdm_filter_type_t filter = config->filter_type;
    uint16_t oversample = config->oversampling_rate;
    uint8_t data_shift = 0;

    /* When outputting 16 bit data, the sample data is shifted and store at the high bit of the register */
    if (config->output_32bit == 0) {
        data_shift = config->output_offset; /* get shift config */
    }

    /* (vin + MODULATOR_INPUT_MAX_VOLTAGE) / (2 * MODULATOR_INPUT_MAX_VOLTAGE) = (sdm_data + max_val) / (2 * max_val) */
    uint32_t max_val = sdm_filter_max_output_value(filter, oversample) >> data_shift;
    double voltage = (((double)sdm_data  + max_val) / (2 * max_val)) * (2 * MODULATOR_INPUT_MAX_VOLTAGE) - MODULATOR_INPUT_MAX_VOLTAGE;
    return voltage;
}

/* Convert amplitude value to voltage */
double sdm_amplitude_value_to_voltage(sdm_filter_type_t filter, uint16_t oversample, uint16_t amplitude)
{
    /* (vin + MODULATOR_INPUT_MAX_VOLTAGE) / (2 * MODULATOR_INPUT_MAX_VOLTAGE) = (sdm_data / * max_val) */
    uint32_t max_val = sdm_filter_max_output_value(filter, oversample);
    double voltage = ((double)amplitude / max_val) * (2 * MODULATOR_INPUT_MAX_VOLTAGE) - MODULATOR_INPUT_MAX_VOLTAGE;
    return voltage;
}

/* Convert voltage to amplitude value */
uint16_t voltage_to_sdm_amplitude_value(sdm_filter_type_t filter, uint16_t oversample, double voltage)
{
    /* (vin + MODULATOR_INPUT_MAX_VOLTAGE) / (2 * MODULATOR_INPUT_MAX_VOLTAGE) = (sdm_data / * max_val) */
    uint32_t max_val = sdm_filter_max_output_value(filter, oversample);
    uint16_t sdm_amplitude_value = (uint16_t)(((voltage + MODULATOR_INPUT_MAX_VOLTAGE) / (2 * MODULATOR_INPUT_MAX_VOLTAGE)) * max_val);
    return sdm_amplitude_value;
}


