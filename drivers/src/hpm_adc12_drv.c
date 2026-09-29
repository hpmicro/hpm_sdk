/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_adc12_drv.h"
#include "hpm_soc_feature.h"

#ifndef ADC12_RETRY_TO_GET_RESULT_COUNT
#define ADC12_RETRY_TO_GET_RESULT_COUNT (100U)
#endif

#ifndef ADC12_PRD_PERIOD_TOL_PERCENT
#define ADC12_PRD_PERIOD_TOL_PERCENT (5U)
#endif

static uint64_t adc12_prd_ticks_to_ns(uint64_t ticks, uint32_t adc_clk_hz)
{
    uint64_t sec;
    uint64_t rem;

    sec = ticks / adc_clk_hz;
    rem = ticks % adc_clk_hz;
    return (sec * 1000000000ULL) + ((rem * 1000000000ULL) / adc_clk_hz);
}

void adc12_get_default_config(adc12_config_t *config)
{
    config->res                = adc12_res_12_bits;
    config->conv_mode          = adc12_conv_mode_oneshot;
    config->adc_clk_div        = adc12_clock_divider_1;
    config->wait_dis           = true;
    config->sel_sync_ahb       = true;
    config->adc_ahb_en         = false; /* Deprecated field; ignored by @ref adc12_init. */
}

void adc12_get_channel_default_config(adc12_channel_config_t *config)
{
    config->ch                 = 0;
    config->diff_sel           = adc12_sample_signal_single_ended;
    config->sample_cycle       = 10;
    config->sample_cycle_shift = 0;
    config->thshdh             = 0xfff;
    config->thshdl             = 0x000;
    config->wdog_int_en        = false;
}

static hpm_stat_t adc12_do_calibration(ADC12_Type *ptr, adc12_sample_signal_t diff_sel)
{
    uint8_t cal_out;
    uint32_t loop_cnt = ADC12_SOC_CALIBRATION_WAITING_LOOP_CNT;

    if (ADC12_IS_SIGNAL_TYPE_INVALID(diff_sel)) {
        return status_invalid_argument;
    }

    /*Set diff_sel temporarily */
    ptr->SAMPLE_CFG[0] &= ~ADC12_SAMPLE_CFG_DIFF_SEL_MASK;
    ptr->SAMPLE_CFG[0] |= ADC12_SAMPLE_CFG_DIFF_SEL_SET(diff_sel);

    /* Set resetcal and resetadc */
    ptr->ANA_CTRL0 |= ADC12_ANA_CTRL0_RESETCAL_MASK | ADC12_ANA_CTRL0_RESETADC_MASK;

    /* Clear resetcal and resetadc */
    ptr->ANA_CTRL0 &= ~(ADC12_ANA_CTRL0_RESETCAL_MASK | ADC12_ANA_CTRL0_RESETADC_MASK);

    /* Set startcal */
    ptr->ANA_CTRL0 |= ADC12_ANA_CTRL0_STARTCAL_MASK;

    /* Clear startcal */
    ptr->ANA_CTRL0 &= ~ADC12_ANA_CTRL0_STARTCAL_MASK;

    /* Set HW rearm_en */
    ptr->ANA_CTRL0 |= ADC12_ANA_CTRL0_REARM_EN_MASK;

    /* Polling calibration status */
    while (ADC12_ANA_STATUS_CALON_GET(ptr->ANA_STATUS) && loop_cnt--) {
        /* TODO: Call a common delay function */
    }

    /* Check if the calibration is timeout */
    if (loop_cnt == 0) {
        return status_timeout;
    }

    /* Read calculation result */
    cal_out = ADC12_ANA_STATUS_CAL_OUT_GET(ptr->ANA_STATUS);

    /* Update cal_out */
    if (diff_sel == adc12_sample_signal_single_ended) {
        ptr->ANA_CTRL0 = (ptr->ANA_CTRL0 & ~ADC12_ANA_CTRL0_CAL_VAL_SE_MASK)
                       | ADC12_ANA_CTRL0_CAL_VAL_SE_SET(cal_out);
    } else {
        ptr->ANA_CTRL0 = (ptr->ANA_CTRL0 & ~ADC12_ANA_CTRL0_CAL_VAL_DIFF_MASK)
                       | ADC12_ANA_CTRL0_CAL_VAL_DIFF_SET(cal_out);
    }

    return status_success;
}

hpm_stat_t adc12_deinit(ADC12_Type *ptr)
{
    /* disable all interrupts */
    ptr->INT_EN = 0;

    return status_success;
}

hpm_stat_t adc12_init(ADC12_Type *ptr, adc12_config_t *config)
{
    uint32_t adc_clk_div;
    uint32_t adc_ahb_en;

    /**
     * disable adc
     * When the adc is processing data, it will generate an error to initialize the adc again,
     * so you need to shut down the adc before initializing it.
     */

    ptr->ANA_CTRL0 &= ~(ADC12_ANA_CTRL0_ENADC_MASK);

    /* Check the resolution */
    if (config->res > adc12_res_12_bits) {
        return status_invalid_argument;
    }

    /* Set resolution */
    ptr->ANA_CTRL1 = (ptr->ANA_CTRL1 & ~ADC12_ANA_CTRL1_SELRES_MASK)
                   | ADC12_ANA_CTRL1_SELRES_SET(config->res);

    /* Set convert clock number and clock period */
    if ((config->adc_clk_div - 1) > ADC12_CONV_CFG1_CLOCK_DIVIDER_MASK)  {
        return status_invalid_argument;
    }

    /* Set ADC minimum conversion cycle and ADC clock divider */
    ptr->CONV_CFG1 = ADC12_CONV_CFG1_CONVERT_CLOCK_NUMBER_SET(2 * config->res + 7)
                   | ADC12_CONV_CFG1_CLOCK_DIVIDER_SET(config->adc_clk_div - 1);

    /* Set ADC_CFG0; program ADC_AHB_EN from conv_mode. */
    if ((config->conv_mode == adc12_conv_mode_sequence) || (config->conv_mode == adc12_conv_mode_preemption)) {
        adc_ahb_en = 1U;
    } else {
        adc_ahb_en = 0U;
    }
    ptr->ADC_CFG0 = ADC12_ADC_CFG0_SEL_SYNC_AHB_SET(config->sel_sync_ahb)
                  | ADC12_ADC_CFG0_ADC_AHB_EN_SET(adc_ahb_en);

    /* Set wait_dis */
    ptr->BUF_CFG0 = ADC12_BUF_CFG0_WAIT_DIS_SET(config->wait_dis);

    /*-------------------------------------------------------------------------------
     *                                 Calibration
     *------------------------------------------------------------------------------
     */
    /* Set enldo */
    ptr->ANA_CTRL0 |= ADC12_ANA_CTRL0_ENLDO_MASK;

    /* TODO: wait 20us after setting enlado for adc0~adc2 */

    adc_clk_div = config->adc_clk_div;

    /* calibration uses a fixed 1:4; restore the app divider after if it differs */
    if (adc_clk_div != adc12_clock_divider_4) {
        ptr->CONV_CFG1 = (ptr->CONV_CFG1 & ~ADC12_CONV_CFG1_CLOCK_DIVIDER_MASK)
                         | ADC12_CONV_CFG1_CLOCK_DIVIDER_SET(adc12_clock_divider_4 - 1);
    }

    /* Set enadc */
    ptr->ANA_CTRL0 |= ADC12_ANA_CTRL0_ENADC_MASK;

    /* Do a calibration corresponding to the configured mode */
    adc12_do_calibration(ptr, config->diff_sel);

    /* Set ADC clock divider */
    if (adc_clk_div != adc12_clock_divider_4) {
        ptr->CONV_CFG1 = (ptr->CONV_CFG1 & ~ADC12_CONV_CFG1_CLOCK_DIVIDER_MASK)
                       | ADC12_CONV_CFG1_CLOCK_DIVIDER_SET(adc_clk_div - 1);
    }

    /*-------------------------------------------------------------------------------
     *                                 End of calibration
     *------------------------------------------------------------------------------
     */

    return status_success;
}

hpm_stat_t adc12_init_channel(ADC12_Type *ptr, adc12_channel_config_t *config)
{
    /* Check the specified channel number */
    if (ADC12_IS_CHANNEL_INVALID(config->ch)) {
        return status_invalid_argument;
    }

    /* Check sample cycle */
    if (ADC12_IS_CHANNEL_SAMPLE_CYCLE_INVALID(config->sample_cycle)) {
        return status_invalid_argument;
    }

    /* Set warning threshold */
    ptr->PRD_CFG[config->ch].PRD_THSHD_CFG = ADC12_PRD_CFG_PRD_THSHD_CFG_THSHDH_SET(config->thshdh)
                                           | ADC12_PRD_CFG_PRD_THSHD_CFG_THSHDL_SET(config->thshdl);

    /* Select single-ended mode or differential mode */
    /* Set ADC sample cycles multiple */
    /* Set ADC sample cycles */
    ptr->SAMPLE_CFG[config->ch] = ADC12_SAMPLE_CFG_DIFF_SEL_SET(config->diff_sel)
                                | ADC12_SAMPLE_CFG_SAMPLE_CLOCK_NUMBER_SHIFT_SET(config->sample_cycle_shift)
                                | ADC12_SAMPLE_CFG_SAMPLE_CLOCK_NUMBER_SET(config->sample_cycle);

#if defined(ADC12_SOC_WDOG_INT_EN_DEFERRED) && (ADC12_SOC_WDOG_INT_EN_DEFERRED)
    /* Watchdog IRQ enable is deferred: see adc12_enable_wdog_interrupt() after in-window conversion. */
    (void) config->wdog_int_en;
#else
    /* Enable WDOG interrupt together with threshold (SoCs / callers without deferred mode). */
    if (config->wdog_int_en) {
        ptr->INT_EN |= 1U << config->ch;
    }
#endif

    return status_success;
}

hpm_stat_t adc12_get_channel_threshold(ADC12_Type *ptr, uint8_t ch, adc12_channel_threshold_t *config)
{
    /* Check the specified channel number */
    if (ADC12_IS_CHANNEL_INVALID(ch)) {
        return status_invalid_argument;
    }

    config->ch     = ch;
    config->thshdh = ADC12_PRD_CFG_PRD_THSHD_CFG_THSHDH_GET(ptr->PRD_CFG[ch].PRD_THSHD_CFG);
    config->thshdl = ADC12_PRD_CFG_PRD_THSHD_CFG_THSHDL_GET(ptr->PRD_CFG[ch].PRD_THSHD_CFG);

    return status_success;
}

hpm_stat_t adc12_init_seq_dma(ADC12_Type *ptr, adc12_dma_config_t *dma_config)
{
    /* Check the DMA buffer length  */
    if (ADC12_IS_SEQ_DMA_BUFF_LEN_INVLAID(dma_config->buff_len_in_4bytes)) {
        return status_invalid_argument;
    }

    /* Reset ADC DMA  */
    ptr->SEQ_DMA_CFG |= ADC12_SEQ_DMA_CFG_DMA_RST_MASK;

    /* Reset memory to clear all of cycle bits */
    memset(dma_config->start_addr, 0x00, dma_config->buff_len_in_4bytes * sizeof(uint32_t));

    /* De-reset ADC DMA */
    ptr->SEQ_DMA_CFG &= ~ADC12_SEQ_DMA_CFG_DMA_RST_MASK;

    /* Set ADC DMA target address which should be 4-byte aligned */
    ptr->SEQ_DMA_ADDR = (uint32_t)dma_config->start_addr & ADC12_SEQ_DMA_ADDR_TAR_ADDR_MASK;

    /* Set ADC DMA memory dword length */
    ptr->SEQ_DMA_CFG = (ptr->SEQ_DMA_CFG & ~ADC12_SEQ_DMA_CFG_BUF_LEN_MASK)
                     | ADC12_SEQ_DMA_CFG_BUF_LEN_SET(dma_config->buff_len_in_4bytes - 1);

    /* STOP_EN and STOP_POS are programmed independently. */
    ptr->SEQ_DMA_CFG = (ptr->SEQ_DMA_CFG
                        & ~(ADC12_SEQ_DMA_CFG_STOP_EN_MASK | ADC12_SEQ_DMA_CFG_STOP_POS_MASK))
                     | ADC12_SEQ_DMA_CFG_STOP_POS_SET(dma_config->stop_pos)
                     | (dma_config->stop_en ? ADC12_SEQ_DMA_CFG_STOP_EN_MASK : 0U);

    return status_success;
}

hpm_stat_t adc12_set_prd_config(ADC12_Type *ptr, adc12_prd_config_t *config)
{
    uint16_t result;
    uint32_t buf_cfg0_bkp;
    uint8_t retry_cnt = 0;

    /* Check the specified channel number */
    if (ADC12_IS_CHANNEL_INVALID(config->ch)) {
        return status_invalid_argument;
    }

    /* Check the prescale */
    if (config->prescale > (ADC12_PRD_CFG_PRD_CFG_PRESCALE_MASK >> ADC12_PRD_CFG_PRD_CFG_PRESCALE_SHIFT)) {
        return status_invalid_argument;
    }

    /* save BUF_CFG0 */
    buf_cfg0_bkp = ptr->BUF_CFG0;

    ptr->BUF_CFG0 |= ADC16_BUF_CFG0_WAIT_DIS_MASK;

    /* The result will be synced to the register PRD_CFG[config->ch].PRD_RESULT after ADC finishs one conversion in oneshot mode. */
    adc12_get_oneshot_result(ptr, config->ch, &result);

    /* Set periodic prescale */
    ptr->PRD_CFG[config->ch].PRD_CFG = (ptr->PRD_CFG[config->ch].PRD_CFG & ~ADC12_PRD_CFG_PRD_CFG_PRESCALE_MASK)
                                     | ADC12_PRD_CFG_PRD_CFG_PRESCALE_SET(config->prescale);

    /* Set period count */
    ptr->PRD_CFG[config->ch].PRD_CFG = (ptr->PRD_CFG[config->ch].PRD_CFG & ~ADC12_PRD_CFG_PRD_CFG_PRD_MASK)
                                     | ADC12_PRD_CFG_PRD_CFG_PRD_SET(config->period_count);

    /* The following logic is applied in the periodic mode to obtain the first valid result as early as possible. */
    while (adc12_get_oneshot_result(ptr, config->ch, &result) != status_success) {
        if (retry_cnt++ > ADC12_RETRY_TO_GET_RESULT_COUNT) {
            break;
        }
    }

    /* restore BUF_CFG0 */
    ptr->BUF_CFG0 = buf_cfg0_bkp;

    return status_success;
}

hpm_stat_t adc12_trigger_seq_by_sw(ADC12_Type *ptr)
{
    if (ADC12_INT_STS_SEQ_SW_CFLCT_GET(ptr->INT_STS)) {
        return status_fail;
    }
    ptr->SEQ_CFG0 |= ADC12_SEQ_CFG0_SW_TRIG_MASK;

    return status_success;
}

/* Note: the sequence length can not be larger or equal than 2 in HPM6750EVK Revision A0 */
hpm_stat_t adc12_set_seq_config(ADC12_Type *ptr, adc12_seq_config_t *config)
{
    /* Check sequence length */
    if (ADC12_IS_SEQ_LEN_INVLAID(config->seq_len)) {
        return status_invalid_argument;
    }

    ptr->SEQ_CFG0 = ADC12_SEQ_CFG0_SEQ_LEN_SET(config->seq_len - 1)
                  | ADC12_SEQ_CFG0_RESTART_EN_SET(config->restart_en)
                  | ADC12_SEQ_CFG0_CONT_EN_SET(config->cont_en)
                  | ADC12_SEQ_CFG0_SW_TRIG_EN_SET(config->sw_trig_en)
                  | ADC12_SEQ_CFG0_HW_TRIG_EN_SET(config->hw_trig_en);

    /* Set sequence queue */
    for (int i = 0; i < config->seq_len; i++) {
        /* Check the specified channel number */
        if (ADC12_IS_CHANNEL_INVALID(config->queue[i].ch)) {
            return status_invalid_argument;
        }

        ptr->SEQ_QUE[i] = ADC12_SEQ_QUE_SEQ_INT_EN_SET(config->queue[i].seq_int_en)
                        | ADC12_SEQ_QUE_CHAN_NUM_4_0_SET(config->queue[i].ch);
    }

    return status_success;
}

hpm_stat_t adc12_trigger_pmt_by_sw(ADC12_Type *ptr, uint8_t trig_ch)
{
    ptr->TRG_SW_STA = ADC12_TRG_SW_STA_TRG_SW_STA_MASK | ADC12_TRG_SW_STA_TRIG_SW_INDEX_SET(trig_ch);

    return status_success;
}

hpm_stat_t adc12_set_pmt_config(ADC12_Type *ptr, adc12_pmt_config_t *config)
{
    uint32_t temp = 0;

    /* Check the specified trigger length */
    if (ADC12_IS_TRIG_LEN_INVLAID(config->trig_len)) {
        return status_invalid_argument;
    }

	/* Check the trigger channel */
    if (ADC12_IS_TRIG_CH_INVLAID(config->trig_ch)) {
        return status_invalid_argument;
    }

    temp |= ADC12_CONFIG_TRIG_LEN_SET(config->trig_len - 1);

    for (int i = 0; i < config->trig_len; i++) {
        if (ADC12_IS_CHANNEL_INVALID(config->adc_ch[i])) {
            return status_invalid_argument;
        }

        temp |= config->inten[i] << (ADC12_CONFIG_INTEN0_SHIFT + i * ADC_SOC_CONFIG_INTEN_CHAN_BIT_SIZE)
             |  config->adc_ch[i] << (ADC12_CONFIG_CHAN0_SHIFT + i * ADC_SOC_CONFIG_INTEN_CHAN_BIT_SIZE);
    }

    ptr->CONFIG[config->trig_ch] = temp;

    return status_success;
}

hpm_stat_t adc12_get_oneshot_result(ADC12_Type *ptr, uint8_t ch, uint16_t *result)
{
    uint32_t bus_res;

    /* Check the specified channel number */
    if (ADC12_IS_CHANNEL_INVALID(ch)) {
        return status_invalid_argument;
    }

    bus_res = ptr->BUS_RESULT[ch];
    *result = ADC12_BUS_RESULT_CHAN_RESULT_GET(bus_res);

    if (ADC12_BUF_CFG0_WAIT_DIS_GET(ptr->BUF_CFG0)) {
        if (!ADC12_BUS_RESULT_VALID_GET(bus_res)) {
            return status_fail;
        }
    }

    return status_success;
}

hpm_stat_t adc12_get_prd_result(ADC12_Type *ptr, uint8_t ch, uint16_t *result)
{
    /* Check the specified channel number */
    if (ADC12_IS_CHANNEL_INVALID(ch)) {
        return status_invalid_argument;
    }

    *result = ADC12_PRD_CFG_PRD_RESULT_CHAN_RESULT_GET(ptr->PRD_CFG[ch].PRD_RESULT);

    return status_success;
}

hpm_stat_t adc12_calc_clock_divider(uint32_t input_hz, uint32_t target_conv_hz, uint32_t *div)
{
    uint32_t d;

    if ((input_hz == 0) || (target_conv_hz == 0) || (div == NULL)) {
        return status_invalid_argument;
    }

    d = (input_hz + target_conv_hz - 1U) / target_conv_hz;
    if (d < adc12_clock_divider_1) {
        d = adc12_clock_divider_1;
    }
    if ((d > adc12_clock_divider_16) || ((input_hz / d) > target_conv_hz)) {
        return status_invalid_argument;
    }

    *div = d;
    return status_success;
}

hpm_stat_t adc12_calc_prd_config(uint32_t adc_clk_hz, uint64_t target_period_ns, adc12_prd_config_t *cfg)
{
    uint8_t prescale;
    uint8_t prescale_max;
    uint8_t best_prescale;
    uint8_t best_period_count;
    uint16_t reload;
    uint64_t ticks;
    uint64_t actual_ns;
    uint64_t diff;
    uint64_t best_diff;

    if ((adc_clk_hz == 0) || (target_period_ns == 0) || (cfg == NULL)) {
        return status_invalid_argument;
    }

    prescale_max = (uint8_t)(ADC12_PRD_CFG_PRD_CFG_PRESCALE_MASK >> ADC12_PRD_CFG_PRD_CFG_PRESCALE_SHIFT);
    best_diff = UINT64_MAX;
    best_prescale = 0;
    best_period_count = 1;

    /* nearest discrete 2^prescale*prd step; period timer resolution is coarse at long periods */
    for (prescale = 0; prescale <= prescale_max; prescale++) {
        for (reload = 2; reload <= 256; reload++) {
            ticks = (1ULL << prescale) * reload;
            actual_ns = adc12_prd_ticks_to_ns(ticks, adc_clk_hz);
            if (actual_ns > target_period_ns) {
                diff = actual_ns - target_period_ns;
            } else {
                diff = target_period_ns - actual_ns;
            }
            if (diff < best_diff) {
                best_diff = diff;
                best_prescale = prescale;
                best_period_count = (uint8_t)(reload - 1U);
            }
        }
    }

    if ((best_diff * 100ULL) > (target_period_ns * ADC12_PRD_PERIOD_TOL_PERCENT)) {
        return status_invalid_argument;
    }

    cfg->prescale = best_prescale;
    cfg->period_count = best_period_count;
    return status_success;
}

uint32_t adc12_get_convert_cycles(uint8_t res)
{
    return (2U * (uint32_t)res) + 8U;
}

hpm_stat_t adc12_calc_sample_rate(uint32_t conv_hz, uint32_t sample_cycle, uint32_t convert_cycles, uint32_t *fs_hz)
{
    uint32_t cycles;

    if ((conv_hz == 0) || (sample_cycle == 0) || (convert_cycles == 0) || (fs_hz == NULL)) {
        return status_invalid_argument;
    }

    cycles = sample_cycle + convert_cycles;
    *fs_hz = conv_hz / cycles;
    return status_success;
}
