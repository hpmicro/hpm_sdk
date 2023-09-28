/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_otp_drv.h"
#include "hpm_adc16_drv.h"

/* Please confirm the actual VREFH voltage according to your HW design! */
#define ADC_VREFH_VOL   (3270U)

/* Get REF25C data */
#define ADC_GET_REF25_VAL() (otp_read_from_shadow(ADC_SOC_OTP_TSNS_REF25_SHIFT) & ADC_SOC_OTP_TSNS_REF25_MASK)

/* Make sure the sample time from 10us to 100us */
#define ADC_SAMPLE_CYCLE (511U)

/* Scaling for calculation */
#define ADC_SCALING (1000U)

#define ADC_MAX_SAMPLE_VALUE ADC16_SOC_MAX_SAMPLE_VALUE
#define APP_ADC_TEMP_ADC16_BASE HPM_ADC3

void init_common_config(adc16_conversion_mode_t conv_mode)
{
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = conv_mode;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = true;

    if (cfg.conv_mode == adc16_conv_mode_sequence ||
        cfg.conv_mode == adc16_conv_mode_preemption) {
        cfg.adc_ahb_en = true;
    }

    adc16_init(APP_ADC_TEMP_ADC16_BASE, &cfg);
}

void init_oneshot_config(void)
{
    adc16_channel_config_t ch_cfg;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.ch           = ADC16_SOC_TEMP_CH_NUM;
    ch_cfg.sample_cycle = ADC_SAMPLE_CYCLE;

    adc16_init_channel(APP_ADC_TEMP_ADC16_BASE, &ch_cfg);

    /* enable the temperature sensor */
    adc16_enable_temp_sensor(APP_ADC_TEMP_ADC16_BASE);
}

void oneshot_handler(void)
{
    int16_t temp;
    uint16_t result;
    uint16_t vout, vout25c;

    if (adc16_get_oneshot_result(BOARD_APP_ADC16_BASE, ADC16_SOC_TEMP_CH_NUM, &result) == status_success) {
        /* Calculate vout */
        vout = result * ADC_SCALING / ADC_MAX_SAMPLE_VALUE * ADC_VREFH_VOL / ADC_SCALING;

        /* Calculate vout25c */
        vout25c = (uint16_t)(ADC_GET_REF25_VAL() * ADC_SCALING / ADC_SOC_VOUT25C_MAX_SAMPLE_VALUE * ADC_SOC_TEMPSENS_REF25_VOL / ADC_SCALING);

        /* Calculate temperature */
        temp = (int16_t)((vout - vout25c) * ADC_SOC_REF_SLOPE + ADC_SOC_REF_TEMP);

        printf("Current Soc Temp: %d℃\n", temp);
    }
}

int main(void)
{
    /* Bsp initialization */
    board_init();

    /* ADC clock initialization */
    board_init_adc16_clock(APP_ADC_TEMP_ADC16_BASE, true);


    /* ADC common initialization */
    init_common_config(adc16_conv_mode_oneshot);

    /* ADC read patter config */
    init_oneshot_config();

    printf("This is an ADC16 temperature acquisition demo:\n");

    /* Main loop */
    while (1) {
        board_delay_ms(1000);
        oneshot_handler();
    }
}
