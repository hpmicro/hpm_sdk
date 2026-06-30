/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_adc_v2.h"
#include "hpm_adc_v2_port.h"

adc_v2_ip_t hpm_adc_v2_get_ip_type(adc_v2_handle_t handle)
{
    return adc_v2_port_detect_ip(handle.base);
}

void hpm_adc_v2_get_default_config(adc_v2_handle_t handle, adc_v2_config_t *config)
{
    adc_v2_ip_t ip;

    if (config == NULL) {
        return;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_config_t hw_cfg;
        adc12_get_default_config(&hw_cfg);
        config->resolution_bits = adc_v2_resolution_12bit;
        config->conv_mode = hw_cfg.conv_mode;
        config->clock_div = (uint8_t) hw_cfg.adc_clk_div;
        config->signal_mode = (uint8_t) hw_cfg.diff_sel;
        config->wait_dis = hw_cfg.wait_dis;
        config->sel_sync_ahb = hw_cfg.sel_sync_ahb;
        config->port3_realtime = false;
        config->dma_seq16bit = false;
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_config_t hw_cfg;
        adc16_get_default_config(&hw_cfg);
        config->resolution_bits = adc_v2_resolution_16bit;
        config->conv_mode = hw_cfg.conv_mode;
        config->clock_div = (uint8_t) hw_cfg.adc_clk_div;
        config->signal_mode = adc_v2_signal_mode_single_ended;
        config->wait_dis = hw_cfg.wait_dis;
#if !defined(HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB) || !HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB
        config->sel_sync_ahb = hw_cfg.sel_sync_ahb;
#else
        config->sel_sync_ahb = false;
#endif
        config->port3_realtime = hw_cfg.port3_realtime;
#if HPM_ADC_V2_HAS_DMA_SEQ16BIT
        config->dma_seq16bit = hw_cfg.dma_seq16bit;
#else
        config->dma_seq16bit = false;
#endif
#endif
    }
}

void hpm_adc_v2_get_channel_default_config(adc_v2_handle_t handle, adc_v2_channel_config_t *config)
{
    adc_v2_ip_t ip;

    if (config == NULL) {
        return;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_channel_config_t hw_cfg;
        adc12_get_channel_default_config(&hw_cfg);
        config->ch = hw_cfg.ch;
        config->signal_mode = hw_cfg.diff_sel;
        config->thshdh = hw_cfg.thshdh;
        config->thshdl = hw_cfg.thshdl;
        config->wdog_int_en = hw_cfg.wdog_int_en;
        config->sample_cycle_shift = hw_cfg.sample_cycle_shift;
        config->sample_cycle = hw_cfg.sample_cycle;
        config->adc_loop_exp = 0U;
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_channel_config_t hw_cfg;
        adc16_get_channel_default_config(&hw_cfg);
        config->ch = hw_cfg.ch;
        config->signal_mode = adc_v2_signal_mode_single_ended;
        config->thshdh = hw_cfg.thshdh;
        config->thshdl = hw_cfg.thshdl;
        config->wdog_int_en = hw_cfg.wdog_int_en;
        config->sample_cycle_shift = hw_cfg.sample_cycle_shift;
        config->sample_cycle = hw_cfg.sample_cycle;
#if HPM_ADC_V2_HAS_ADC_LOOP
        config->adc_loop_exp = hw_cfg.adc_loop_exp;
#else
        config->adc_loop_exp = 0U;
#endif
#endif
    }
}

hpm_stat_t hpm_adc_v2_init(adc_v2_handle_t handle, adc_v2_config_t *config)
{
    adc_v2_ip_t ip;
    hpm_stat_t stat;
    uint8_t hw_res;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    stat = adc_v2_port_resolution_to_hw(ip, config->resolution_bits, &hw_res);
    if (stat != status_success) {
        return stat;
    }

    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_config_t hw_cfg;
        adc12_get_default_config(&hw_cfg);
        hw_cfg.res = hw_res;
        hw_cfg.conv_mode = config->conv_mode;
        hw_cfg.diff_sel = config->signal_mode;
        hw_cfg.adc_clk_div = config->clock_div;
        hw_cfg.wait_dis = config->wait_dis;
        hw_cfg.sel_sync_ahb = config->sel_sync_ahb;
        return adc12_init((ADC12_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_config_t hw_cfg;
        adc16_get_default_config(&hw_cfg);
        hw_cfg.res = hw_res;
        hw_cfg.conv_mode = config->conv_mode;
        hw_cfg.adc_clk_div = config->clock_div;
        hw_cfg.wait_dis = config->wait_dis;
        hw_cfg.port3_realtime = config->port3_realtime;
#if !defined(HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB) || !HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB
        hw_cfg.sel_sync_ahb = config->sel_sync_ahb;
#endif
#if HPM_ADC_V2_HAS_DMA_SEQ16BIT
        hw_cfg.dma_seq16bit = config->dma_seq16bit;
#endif
        return adc16_init((ADC16_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_deinit(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_deinit((ADC12_Type *) handle.base);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_deinit((ADC16_Type *) handle.base);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_init_channel(adc_v2_handle_t handle, adc_v2_channel_config_t *config)
{
    adc_v2_ip_t ip;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_channel_config_t hw_cfg;
        adc12_get_channel_default_config(&hw_cfg);
        hw_cfg.ch = config->ch;
        hw_cfg.diff_sel = config->signal_mode;
        hw_cfg.thshdh = config->thshdh;
        hw_cfg.thshdl = config->thshdl;
        hw_cfg.wdog_int_en = config->wdog_int_en;
        hw_cfg.sample_cycle_shift = config->sample_cycle_shift;
        hw_cfg.sample_cycle = config->sample_cycle;
        return adc12_init_channel((ADC12_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_channel_config_t hw_cfg;
        adc16_get_channel_default_config(&hw_cfg);
        hw_cfg.ch = config->ch;
        hw_cfg.thshdh = config->thshdh;
        hw_cfg.thshdl = config->thshdl;
        hw_cfg.wdog_int_en = config->wdog_int_en;
        hw_cfg.sample_cycle_shift = config->sample_cycle_shift;
        hw_cfg.sample_cycle = config->sample_cycle;
#if HPM_ADC_V2_HAS_ADC_LOOP
        hw_cfg.adc_loop_exp = config->adc_loop_exp;
#endif
        return adc16_init_channel((ADC16_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_get_channel_threshold(adc_v2_handle_t handle, uint8_t ch, adc_v2_channel_threshold_t *config)
{
    adc_v2_ip_t ip;
    hpm_stat_t stat;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_channel_threshold_t hw_cfg;
        stat = adc12_get_channel_threshold((ADC12_Type *) handle.base, ch, &hw_cfg);
        if (stat == status_success) {
            config->ch = hw_cfg.ch;
            config->thshdh = hw_cfg.thshdh;
            config->thshdl = hw_cfg.thshdl;
        }
        return stat;
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_channel_threshold_t hw_cfg;
        stat = adc16_get_channel_threshold((ADC16_Type *) handle.base, ch, &hw_cfg);
        if (stat == status_success) {
            config->ch = hw_cfg.ch;
            config->thshdh = hw_cfg.thshdh;
            config->thshdl = hw_cfg.thshdl;
        }
        return stat;
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_set_period_config(adc_v2_handle_t handle, adc_v2_prd_config_t *config)
{
    adc_v2_ip_t ip;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_prd_config_t hw_cfg;
        hw_cfg.ch = config->ch;
        hw_cfg.prescale = config->prescale;
        hw_cfg.period_count = config->period_count;
        return adc12_set_prd_config((ADC12_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_prd_config_t hw_cfg;
        hw_cfg.ch = config->ch;
        hw_cfg.prescale = config->prescale;
        hw_cfg.period_count = config->period_count;
        return adc16_set_prd_config((ADC16_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_set_sequence_config(adc_v2_handle_t handle, adc_v2_seq_config_t *config)
{
    adc_v2_ip_t ip;
    uint32_t i;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_seq_config_t hw_cfg;
        for (i = 0U; i < ADC_SOC_SEQ_MAX_LEN; i++) {
            hw_cfg.queue[i].seq_int_en = config->queue[i].seq_int_en;
            hw_cfg.queue[i].ch = config->queue[i].ch;
        }
        hw_cfg.restart_en = config->restart_en;
        hw_cfg.cont_en = config->cont_en;
        hw_cfg.sw_trig_en = config->sw_trig_en;
        hw_cfg.hw_trig_en = config->hw_trig_en;
        hw_cfg.seq_len = config->seq_len;
        return adc12_set_seq_config((ADC12_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_seq_config_t hw_cfg;
        for (i = 0U; i < ADC_SOC_SEQ_MAX_LEN; i++) {
            hw_cfg.queue[i].seq_int_en = config->queue[i].seq_int_en;
            hw_cfg.queue[i].ch = config->queue[i].ch;
        }
        hw_cfg.restart_en = config->restart_en;
        hw_cfg.cont_en = config->cont_en;
        hw_cfg.sw_trig_en = config->sw_trig_en;
        hw_cfg.hw_trig_en = config->hw_trig_en;
        hw_cfg.seq_len = config->seq_len;
        return adc16_set_seq_config((ADC16_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_set_preempt_config(adc_v2_handle_t handle, adc_v2_pmt_config_t *config)
{
    adc_v2_ip_t ip;
    uint32_t i;
    uint32_t copy_len;

    if (config == NULL) {
        return status_invalid_argument;
    }

    copy_len = config->trig_len;
    if (copy_len > ADC_SOC_MAX_TRIG_CH_LEN) {
        copy_len = ADC_SOC_MAX_TRIG_CH_LEN;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_pmt_config_t hw_cfg;

        for (i = 0U; i < copy_len; i++) {
            hw_cfg.inten[i] = config->inten[i];
            hw_cfg.adc_ch[i] = config->adc_ch[i];
        }
        hw_cfg.trig_ch = config->trig_ch;
        hw_cfg.trig_len = config->trig_len;
        return adc12_set_pmt_config((ADC12_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_pmt_config_t hw_cfg;

        for (i = 0U; i < copy_len; i++) {
            hw_cfg.inten[i] = config->inten[i];
            hw_cfg.adc_ch[i] = config->adc_ch[i];
        }
        hw_cfg.trig_ch = config->trig_ch;
        hw_cfg.trig_len = config->trig_len;
        return adc16_set_pmt_config((ADC16_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

void hpm_adc_v2_set_seq_stop_pos(adc_v2_handle_t handle, uint16_t stop_pos)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_set_seq_stop_pos((ADC12_Type *) handle.base, stop_pos);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_set_seq_stop_pos((ADC16_Type *) handle.base, stop_pos);
#endif
    }
}

void hpm_adc_v2_init_pmt_dma(adc_v2_handle_t handle, uint32_t addr)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_init_pmt_dma((ADC12_Type *) handle.base, addr);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_init_pmt_dma((ADC16_Type *) handle.base, addr);
#endif
    }
}

hpm_stat_t hpm_adc_v2_init_seq_dma(adc_v2_handle_t handle, adc_v2_dma_config_t *config)
{
    adc_v2_ip_t ip;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_dma_config_t hw_cfg;
        hw_cfg.start_addr = config->start_addr;
        hw_cfg.buff_len_in_4bytes = config->buff_len_in_4bytes;
        hw_cfg.stop_pos = config->stop_pos;
        hw_cfg.stop_en = config->stop_en;
        return adc12_init_seq_dma((ADC12_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_dma_config_t hw_cfg;
        hw_cfg.start_addr = config->start_addr;
#if HPM_ADC_V2_HAS_DMA_SEQ16BIT
        hw_cfg.buff_len_in_2bytes = config->buff_len_in_2bytes;
#endif
        hw_cfg.buff_len_in_4bytes = config->buff_len_in_4bytes;
        hw_cfg.stop_pos = config->stop_pos;
        hw_cfg.stop_en = config->stop_en;
        return adc16_init_seq_dma((ADC16_Type *) handle.base, &hw_cfg);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

bool hpm_adc_v2_is_seq_dma_16bit_enabled(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_DMA_SEQ16BIT
        return adc16_is_seq_dma_16bit_enabled((ADC16_Type *) handle.base);
#else
        return false;
#endif
    }

    return false;
}

void hpm_adc_v2_set_nonblocking_read(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_set_nonblocking_read((ADC12_Type *) handle.base);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_set_nonblocking_read((ADC16_Type *) handle.base);
#endif
    }
}

void hpm_adc_v2_set_blocking_read(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_set_blocking_read((ADC12_Type *) handle.base);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_set_blocking_read((ADC16_Type *) handle.base);
#endif
    }
}

bool hpm_adc_v2_is_nonblocking_mode(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_is_nonblocking_mode((ADC12_Type *) handle.base);
#else
        return false;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_is_nonblocking_mode((ADC16_Type *) handle.base);
#else
        return false;
#endif
    }

    return false;
}

uint32_t hpm_adc_v2_get_status_flags(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;
    uint32_t hw_mask;

    ip = hpm_adc_v2_get_ip_type(handle);
    hw_mask = 0U;
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        hw_mask = adc12_get_status_flags((ADC12_Type *) handle.base);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        hw_mask = adc16_get_status_flags((ADC16_Type *) handle.base);
#endif
    }

    return adc_v2_port_event_from_hw(ip, hw_mask);
}

uint32_t hpm_adc_v2_get_hw_event_mask(adc_v2_handle_t handle, uint32_t event_mask)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    return adc_v2_port_event_to_hw(ip, event_mask);
}

void hpm_adc_v2_clear_status_flags(adc_v2_handle_t handle, uint32_t event_mask)
{
    adc_v2_ip_t ip;
    uint32_t hw_mask;

    ip = hpm_adc_v2_get_ip_type(handle);
    hw_mask = adc_v2_port_event_to_hw(ip, event_mask);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_clear_status_flags((ADC12_Type *) handle.base, hw_mask);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_clear_status_flags((ADC16_Type *) handle.base, hw_mask);
#endif
    }
}

void hpm_adc_v2_enable_interrupts(adc_v2_handle_t handle, uint32_t event_mask)
{
    adc_v2_ip_t ip;
    uint32_t hw_mask;

    ip = hpm_adc_v2_get_ip_type(handle);
    hw_mask = adc_v2_port_event_to_hw(ip, event_mask);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_enable_interrupts((ADC12_Type *) handle.base, hw_mask);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_enable_interrupts((ADC16_Type *) handle.base, hw_mask);
#endif
    }
}

void hpm_adc_v2_disable_interrupts(adc_v2_handle_t handle, uint32_t event_mask)
{
    adc_v2_ip_t ip;
    uint32_t hw_mask;

    ip = hpm_adc_v2_get_ip_type(handle);
    hw_mask = adc_v2_port_event_to_hw(ip, event_mask);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        adc12_disable_interrupts((ADC12_Type *) handle.base, hw_mask);
#endif
    } else if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        adc16_disable_interrupts((ADC16_Type *) handle.base, hw_mask);
#endif
    }
}

bool hpm_adc_v2_get_conv_valid_status(adc_v2_handle_t handle, uint8_t ch)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_get_conv_valid_status((ADC12_Type *) handle.base, ch);
#else
        return false;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_get_conv_valid_status((ADC16_Type *) handle.base, ch);
#else
        return false;
#endif
    }

    return false;
}

hpm_stat_t hpm_adc_v2_get_oneshot_result(adc_v2_handle_t handle, uint8_t ch, uint16_t *result)
{
    adc_v2_ip_t ip;

    if (result == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_get_oneshot_result((ADC12_Type *) handle.base, ch, result);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_get_oneshot_result((ADC16_Type *) handle.base, ch, result);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_get_prd_result(adc_v2_handle_t handle, uint8_t ch, uint16_t *result)
{
    adc_v2_ip_t ip;

    if (result == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_get_prd_result((ADC12_Type *) handle.base, ch, result);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_get_prd_result((ADC16_Type *) handle.base, ch, result);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_trigger_seq_by_sw(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_trigger_seq_by_sw((ADC12_Type *) handle.base);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_trigger_seq_by_sw((ADC16_Type *) handle.base);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_trigger_pmt_by_sw(adc_v2_handle_t handle, uint8_t trig_ch)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc12) {
#if HPM_ADC_V2_HAS_ADC12
        return adc12_trigger_pmt_by_sw((ADC12_Type *) handle.base, trig_ch);
#else
        return status_invalid_argument;
#endif
    }

    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_trigger_pmt_by_sw((ADC16_Type *) handle.base, trig_ch);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_enable_pmt_queue(adc_v2_handle_t handle, uint8_t trig_ch)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_enable_pmt_queue((ADC16_Type *) handle.base, trig_ch);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

hpm_stat_t hpm_adc_v2_disable_pmt_queue(adc_v2_handle_t handle, uint8_t trig_ch)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc16) {
#if HPM_ADC_V2_HAS_ADC16
        return adc16_disable_pmt_queue((ADC16_Type *) handle.base, trig_ch);
#else
        return status_invalid_argument;
#endif
    }

    return status_invalid_argument;
}

#if HPM_ADC_V2_HAS_DIFF_MODE
void hpm_adc_v2_get_default_diff_config(adc_v2_handle_t handle, adc_v2_diff_config_t *config)
{
    adc_v2_ip_t ip;

    if (config == NULL) {
        return;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc16) {
        adc16_diff_config_t hw_cfg;

        adc16_get_default_diff_mode_config((ADC16_Type *) handle.base, &hw_cfg);
        config->full_resolution = hw_cfg.full_resolution;
        config->position_mode = hw_cfg.position_mode;
        config->master = hw_cfg.master;
    }
}

hpm_stat_t hpm_adc_v2_enable_diff_mode(adc_v2_handle_t handle, adc_v2_diff_config_t *config)
{
    adc_v2_ip_t ip;
    adc16_diff_config_t hw_cfg;

    if (config == NULL) {
        return status_invalid_argument;
    }

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip != adc_v2_ip_adc16) {
        return status_invalid_argument;
    }

    hw_cfg.full_resolution = config->full_resolution;
    hw_cfg.position_mode = config->position_mode;
    hw_cfg.master = config->master;
    adc16_enable_diff_mode((ADC16_Type *) handle.base, &hw_cfg);
    return status_success;
}

void hpm_adc_v2_disable_diff_mode(adc_v2_handle_t handle)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    if (ip == adc_v2_ip_adc16) {
        adc16_disable_diff_mode((ADC16_Type *) handle.base);
    }
}
#endif

#if HPM_ADC_V2_HAS_MOTOR_MODE
void hpm_adc_v2_enable_motor_mode(adc_v2_handle_t handle)
{
    adc16_enable_motor((ADC16_Type *) handle.base);
}
#endif

void hpm_adc_v2_parse_pmt_dma_word(adc_v2_handle_t handle, uint32_t raw, adc_v2_dma_sample_t *sample)
{
    adc_v2_ip_t ip;

    ip = hpm_adc_v2_get_ip_type(handle);
    adc_v2_port_parse_pmt_dma_word(ip, raw, sample);
}

void hpm_adc_v2_parse_seq_dma_word(adc_v2_handle_t handle, uint32_t raw, adc_v2_dma_sample_t *sample)
{
    adc_v2_ip_t ip;
    bool seq_raw_16bit;

    ip = hpm_adc_v2_get_ip_type(handle);
    seq_raw_16bit = hpm_adc_v2_is_seq_dma_16bit_enabled(handle);
    adc_v2_port_parse_seq_dma_word(ip, raw, seq_raw_16bit, sample);
}
