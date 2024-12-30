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

#define APP_ADC16_CH_WDOG_EVENT              (1 << ADC16_SOC_TEMP_CH_NUM)

__IO uint32_t res_out_of_thr_flag;

SDK_DECLARE_EXT_ISR_M(BOARD_APP_ADC16_IRQn, isr_temp)
void isr_temp(void)
{
    uint32_t status;

    status = adc16_get_status_flags(BOARD_APP_ADC16_BASE);

    /* Clear status */
    adc16_clear_status_flags(BOARD_APP_ADC16_BASE, status);

   if (status & APP_ADC16_CH_WDOG_EVENT) {
        adc16_disable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_CH_WDOG_EVENT);
        res_out_of_thr_flag = status & APP_ADC16_CH_WDOG_EVENT;
    }
}

hpm_stat_t init_common_config(adc16_conversion_mode_t conv_mode)
{
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = conv_mode;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = (clk_adc_src_ahb0 == clock_get_source(BOARD_APP_ADC16_CLK_NAME)) ? true : false;

    if (cfg.conv_mode == adc16_conv_mode_sequence ||
        cfg.conv_mode == adc16_conv_mode_preemption) {
        cfg.adc_ahb_en = true;
    }

    adc16_init(APP_ADC_TEMP_ADC16_BASE, &cfg);
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

    if (res_out_of_thr_flag & APP_ADC16_CH_WDOG_EVENT) {
        adc16_get_channel_threshold(BOARD_APP_ADC16_BASE, ADC16_SOC_TEMP_CH_NUM, &threshold);
        printf("Warning - %s [channel %02d] - Sample voltage is out of the thresholds between 0x%04x and 0x%04x !\n", BOARD_APP_ADC16_NAME, ADC16_SOC_TEMP_CH_NUM, threshold.thshdl, threshold.thshdh);
        res_out_of_thr_flag = 0;
        adc16_enable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_CH_WDOG_EVENT);
    }
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

    adc16_set_nonblocking_read(BOARD_APP_ADC16_BASE);

    /* enable the temperature sensor */
    adc16_enable_temp_sensor(APP_ADC_TEMP_ADC16_BASE);
}

void oneshot_handler(void)
{
    int16_t temp;
    uint16_t result;
    uint16_t vout, vout25c;

    if (adc16_get_oneshot_result(BOARD_APP_ADC16_BASE, ADC16_SOC_TEMP_CH_NUM, &result) == status_success) {
        if (adc16_is_nonblocking_mode(BOARD_APP_ADC16_BASE)) {
              /* trigger the next sample */
              adc16_get_oneshot_result(BOARD_APP_ADC16_BASE, ADC16_SOC_TEMP_CH_NUM, &result);
            }

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
    board_init_adc_clock(APP_ADC_TEMP_ADC16_BASE, true);

    /* ADC common initialization */
    init_common_config(adc16_conv_mode_oneshot);

    /* ADC read patter config */
    init_oneshot_config();

    printf("This is an ADC16 temperature acquisition demo:\n");

    /* Main loop */
    while (1) {
        board_delay_ms(1000);
        channel_result_out_of_threshold_handler();
        oneshot_handler();
    }
}
