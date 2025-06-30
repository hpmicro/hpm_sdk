/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_i2s_over_spi.h"
#include "hpm_spi.h"
#include "hpm_clock_drv.h"

static hpm_stat_t i2s_clocks_init(hpm_i2s_over_spi_t *i2s, uint32_t lrck_hz, uint32_t audio_depth, uint32_t size);
static hpm_stat_t hpm_i2s_master_over_spi_tx_config(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz, uint32_t audio_depth, uint32_t size);

void hpm_i2s_master_over_spi_transfer_complete_callback(hpm_i2s_over_spi_t *i2s)
{
    uint32_t addr;
    uint32_t channel;
    if (i2s == NULL) {
        return;
    }
    hpm_i2s_gptmr_context_t *transfer_time = &i2s->transfer_time;
    if (gptmr_check_status(transfer_time->ptr, GPTMR_CH_RLD_STAT_MASK(transfer_time->channel))) {
        gptmr_clear_status(transfer_time->ptr, GPTMR_CH_RLD_STAT_MASK(transfer_time->channel));
        if (i2s->i2s_rx == true) {
            if (i2s->rx_callback) {
                channel = i2s->rx_dma.resource->channel;
                addr = i2s->rx_dma.resource->base->CHCTRL[channel].LLPOINTER;
                if (core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)i2s->rx_dma.descriptors) == addr) {
                    i2s->rx_callback(0);
                } else {
                    i2s->rx_callback(1);
                }
            }
        } else {
            i2s->has_done = true;
            gptmr_disable_irq(transfer_time->ptr, GPTMR_CH_RLD_IRQ_MASK(transfer_time->channel));
        }
    }
}

hpm_stat_t hpm_i2s_master_over_spi_init(hpm_i2s_over_spi_t *i2s)
{
    hpm_stat_t stat = status_success;
    spi_initialize_config_t spi_config;
    if (i2s == NULL) {
        return status_invalid_argument;
    }
    i2s->transfer_complete = hpm_i2s_master_over_spi_transfer_complete_callback;
    hpm_spi_get_default_init_config(&spi_config);
    spi_config.clk_phase = spi_sclk_sampling_odd_clk_edges;
    spi_config.clk_polarity = spi_sclk_low_idle;
    spi_config.data_len = 32;
    spi_config.direction = spi_msb_first;
    spi_config.mode = spi_slave_mode;
    stat = hpm_spi_initialize(i2s->spi_slave.ptr, &spi_config);
    return stat;
}

bool hpm_i2s_master_over_spi_tx_is_busy(hpm_i2s_over_spi_t *i2s)
{
    return ((i2s->has_done == true) ? false : true);
}

hpm_stat_t hpm_i2s_master_over_spi_tx_stop(hpm_i2s_over_spi_t *i2s)
{
    if ((i2s == NULL) || (i2s->bclk.ptr == NULL) || (i2s->lrck.ptr == NULL)) {
        return status_invalid_argument;
    }
    gptmr_stop_counter(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_stop_counter(i2s->lrck.ptr, i2s->lrck.channel);
    if (i2s->mclk.ptr) {
        gptmr_stop_counter(i2s->mclk.ptr, i2s->mclk.channel);
        gptmr_channel_reset_count(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_channel_reset_count(i2s->lrck.ptr, i2s->lrck.channel);
    gptmr_channel_reset_count(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_stop_counter(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    gptmr_channel_reset_count(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    i2s->spi_slave.write_cs(i2s->spi_slave.cs_pin, true);
    return status_success;
}

hpm_stat_t hpm_i2s_master_over_spi_tx_buffer(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz, uint8_t audio_depth, uint8_t *data, uint32_t size)
{
    hpm_stat_t stat;
    stat = hpm_i2s_master_over_spi_tx_buffer_blocking(i2s, protocol, lrck_hz, audio_depth, data, size, 0xFFFFFFFF);
    return stat;
}

hpm_stat_t hpm_i2s_master_over_spi_tx_buffer_nonblocking(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz, uint8_t audio_depth, uint8_t *data, uint32_t size)
{
    dma_mgr_chn_conf_t chg_config;
    uint8_t data_width;
    hpm_stat_t stat;
    stat = hpm_i2s_master_over_spi_tx_config(i2s, protocol, lrck_hz, audio_depth, size);
    if (stat != status_success) {
        return stat;
    }
    i2s->has_done = false;
    if (audio_depth == 16U) {
        data_width = DMA_MGR_TRANSFER_WIDTH_HALF_WORD;
        spi_set_data_bits(i2s->spi_slave.ptr, 16);
    } else {
        data_width = DMA_MGR_TRANSFER_WIDTH_WORD;
        spi_set_data_bits(i2s->spi_slave.ptr, 32);
    }

    if ((size / (1 << data_width)) > SPI_SOC_TRANSFER_COUNT_MAX) {
        return status_invalid_argument;
    }
    i2s->i2s_rx = false;
    hpm_spi_transmit_setup_dma(i2s->spi_slave.ptr, size);
    /*spi tx data dma*/
    dma_mgr_get_default_chn_config(&chg_config);
    chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
    chg_config.src_width = data_width;
    chg_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)data);
    chg_config.src_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
    chg_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
    chg_config.dst_addr = (uint32_t)&i2s->spi_slave.ptr->DATA;
    chg_config.dst_width = data_width;
    chg_config.en_dmamux = true;
    chg_config.dmamux_src = i2s->spi_slave.txdma_src;
    chg_config.priority = DMA_MGR_CHANNEL_PRIORITY_HIGH;
    chg_config.size_in_byte = (size << chg_config.src_width) >> data_width;
    dma_mgr_setup_channel(i2s->tx_dma.resource, &chg_config);
    dma_mgr_enable_channel(i2s->tx_dma.resource);
    i2s->spi_slave.write_cs(i2s->spi_slave.cs_pin, false);
    gptmr_enable_irq(i2s->transfer_time.ptr, GPTMR_CH_RLD_IRQ_MASK(i2s->transfer_time.channel));
    if (i2s->mclk.ptr) {
        gptmr_start_counter(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_start_counter(i2s->lrck.ptr, i2s->lrck.channel);
    gptmr_start_counter(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_start_counter(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    return status_success;
}

hpm_stat_t hpm_i2s_master_over_spi_tx_buffer_blocking(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz, uint8_t audio_depth,
                                                        uint8_t *data, uint32_t size, uint32_t timeout)
{
    dma_mgr_chn_conf_t chg_config;
    uint8_t data_width;
    hpm_stat_t stat;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    bool is_timeout = false;
    stat = hpm_i2s_master_over_spi_tx_config(i2s, protocol, lrck_hz, audio_depth, size);
    if (stat != status_success) {
        return stat;
    }
    i2s->has_done = false;
    if (audio_depth == 16U) {
        data_width = DMA_MGR_TRANSFER_WIDTH_HALF_WORD;
        spi_set_data_bits(i2s->spi_slave.ptr, 16);
    } else {
        data_width = DMA_MGR_TRANSFER_WIDTH_WORD;
        spi_set_data_bits(i2s->spi_slave.ptr, 32);
    }

    if ((size / (1 << data_width)) > SPI_SOC_TRANSFER_COUNT_MAX) {
        return status_invalid_argument;
    }
    i2s->i2s_rx = false;
    hpm_spi_transmit_setup_dma(i2s->spi_slave.ptr, size);
    /*spi tx data dma*/
    dma_mgr_get_default_chn_config(&chg_config);
    chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
    chg_config.src_width = data_width;
    chg_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)data);
    chg_config.src_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
    chg_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
    chg_config.dst_addr = (uint32_t)&i2s->spi_slave.ptr->DATA;
    chg_config.dst_width = data_width;
    chg_config.en_dmamux = true;
    chg_config.dmamux_src = i2s->spi_slave.txdma_src;
    chg_config.priority = DMA_MGR_CHANNEL_PRIORITY_HIGH;
    chg_config.size_in_byte = (size << chg_config.src_width) >> data_width;
    dma_mgr_setup_channel(i2s->tx_dma.resource, &chg_config);
    dma_mgr_enable_channel(i2s->tx_dma.resource);
    i2s->spi_slave.write_cs(i2s->spi_slave.cs_pin, false);
    gptmr_enable_irq(i2s->transfer_time.ptr, GPTMR_CH_RLD_IRQ_MASK(i2s->transfer_time.channel));
    if (i2s->mclk.ptr) {
        gptmr_start_counter(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_start_counter(i2s->lrck.ptr, i2s->lrck.channel);
    gptmr_start_counter(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_start_counter(i2s->transfer_time.ptr, i2s->transfer_time.channel);

    while (!i2s->has_done) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            is_timeout = true;
            break;
        }
    };
    gptmr_stop_counter(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_stop_counter(i2s->lrck.ptr, i2s->lrck.channel);
    if (i2s->mclk.ptr) {
        gptmr_stop_counter(i2s->mclk.ptr, i2s->mclk.channel);
        gptmr_channel_reset_count(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_channel_reset_count(i2s->lrck.ptr, i2s->lrck.channel);
    gptmr_channel_reset_count(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_stop_counter(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    gptmr_channel_reset_count(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    i2s->spi_slave.write_cs(i2s->spi_slave.cs_pin, true);
    return ((is_timeout == true) ? status_timeout : status_success);
}

hpm_stat_t hpm_i2s_master_over_spi_rx_config(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz,
                                        uint32_t audio_depth, uint8_t *buffer0, uint8_t *buffer1, uint32_t size)
{
    dma_mgr_chn_conf_t chg_config;
    uint8_t data_width;
    if ((i2s == NULL) || (protocol == I2S_PROTOCOL_PCM) || (protocol == I2S_PROTOCOL_I2S_PHILIPS) ||
        ((buffer0 == NULL) && (buffer1 == NULL)) ||
        (i2s->bclk.ptr == NULL) || (i2s->lrck.ptr == NULL)) {
        return status_invalid_argument;
    }
    i2s->has_done = false;
    if (audio_depth == 16U) {
        data_width = DMA_MGR_TRANSFER_WIDTH_HALF_WORD;
        spi_set_data_bits(i2s->spi_slave.ptr, 16);
    } else {
        data_width = DMA_MGR_TRANSFER_WIDTH_WORD;
        spi_set_data_bits(i2s->spi_slave.ptr, 32);
    }

    if ((size / (1 << data_width)) > SPI_SOC_TRANSFER_COUNT_MAX) {
        return status_invalid_argument;
    }
    i2s->i2s_rx = true;
    hpm_spi_receive_setup_dma(i2s->spi_slave.ptr, size);
    spi_set_rx_fifo_threshold(i2s->spi_slave.ptr, SPI_SOC_FIFO_DEPTH);
    dma_mgr_get_default_chn_config(&chg_config);
    /*spi rx data dma*/
    chg_config.priority = DMA_MGR_CHANNEL_PRIORITY_HIGH;
    chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
    chg_config.src_width = data_width;
    chg_config.src_addr = (uint32_t)&i2s->spi_slave.ptr->DATA;
    chg_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    chg_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
    chg_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer0);
    chg_config.dst_width = data_width;
    chg_config.en_dmamux = true;
    chg_config.dmamux_src = i2s->spi_slave.rxdma_src;
    chg_config.size_in_byte = size;
    if (buffer1 != NULL) {
        chg_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s->rx_dma.descriptors[1]);
        dma_mgr_config_linked_descriptor(i2s->rx_dma.resource,
                                     &chg_config, (dma_mgr_linked_descriptor_t *)&i2s->rx_dma.descriptors[0]);
        chg_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer1);
        chg_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s->rx_dma.descriptors[0]);
        dma_mgr_config_linked_descriptor(i2s->rx_dma.resource,
                                     &chg_config, (dma_mgr_linked_descriptor_t *)&i2s->rx_dma.descriptors[1]);

        chg_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer0);
        chg_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s->rx_dma.descriptors[1]);
    }
    dma_mgr_setup_channel(i2s->rx_dma.resource, &chg_config);
    i2s_clocks_init(i2s, lrck_hz, audio_depth, size);
    if (i2s->mclk.ptr) {
        gptmr_channel_reset_count(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_channel_reset_count(i2s->lrck.ptr, i2s->lrck.channel);
    gptmr_channel_reset_count(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_channel_reset_count(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    return status_success;
}

hpm_stat_t hpm_i2s_master_over_spi_rx_start(hpm_i2s_over_spi_t *i2s, i2s_rx_data_tc callback)
{
    if ((i2s == NULL) || (callback == NULL)) {
        return status_invalid_argument;
    }
    i2s->i2s_rx = true;
    i2s->rx_callback = callback;
    dma_mgr_enable_channel(i2s->rx_dma.resource);
    gptmr_enable_irq(i2s->transfer_time.ptr, GPTMR_CH_RLD_IRQ_MASK(i2s->transfer_time.channel));
    i2s->spi_slave.write_cs(i2s->spi_slave.cs_pin, false);
    if (i2s->mclk.ptr) {
        gptmr_start_counter(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_start_counter(i2s->lrck.ptr, i2s->lrck.channel);
    gptmr_start_counter(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_start_counter(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    return status_success;
}

hpm_stat_t hpm_i2s_master_over_spi_rx_stop(hpm_i2s_over_spi_t *i2s)
{
    if (i2s == NULL) {
        return status_invalid_argument;
    }
    i2s->i2s_rx = false;
    gptmr_stop_counter(i2s->bclk.ptr, i2s->bclk.channel);
    gptmr_stop_counter(i2s->lrck.ptr, i2s->lrck.channel);
    if (i2s->mclk.ptr) {
        gptmr_stop_counter(i2s->mclk.ptr, i2s->mclk.channel);
        gptmr_channel_reset_count(i2s->mclk.ptr, i2s->mclk.channel);
    }
    gptmr_stop_counter(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    gptmr_channel_reset_count(i2s->transfer_time.ptr, i2s->transfer_time.channel);
    dma_mgr_disable_channel(i2s->rx_dma.resource);
    gptmr_disable_irq(i2s->transfer_time.ptr, GPTMR_CH_RLD_IRQ_MASK(i2s->transfer_time.channel));
    i2s->spi_slave.write_cs(i2s->spi_slave.cs_pin, true);
    return status_success;
}

static hpm_stat_t hpm_i2s_master_over_spi_tx_config(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz, uint32_t audio_depth, uint32_t size)
{
    hpm_stat_t stat = status_success;
    if ((i2s == NULL) ||
        (protocol == I2S_PROTOCOL_PCM) ||
        (protocol == I2S_PROTOCOL_I2S_PHILIPS)) {
        return status_invalid_argument;
    }
    if (protocol == I2S_PROTOCOL_LSB_JUSTIFIED) {
        i2s->spi_slave.ptr->TRANSFMT &= ~SPI_TRANSFMT_MOSIBIDIR_MASK;
    }
    stat = i2s_clocks_init(i2s, lrck_hz, audio_depth, size);
    return stat;
}

static hpm_stat_t i2s_clocks_init(hpm_i2s_over_spi_t *i2s, uint32_t lrck_hz, uint32_t audio_depth, uint32_t size)
{
    hpm_stat_t stat = status_success;
    gptmr_channel_config_t config;
    uint8_t data_width;
    uint32_t actual_reload = 0;
    uint32_t actual_fclk_rld = 0;
    uint32_t actual_fclk_half_rld = 0;
    uint32_t pwm_freq_hz = 0;
    uint32_t bclk_hz = (2 * lrck_hz * audio_depth);
    if (audio_depth == 16U) {
        data_width = DMA_MGR_TRANSFER_WIDTH_HALF_WORD;
    } else {
        data_width = DMA_MGR_TRANSFER_WIDTH_WORD;
    }
    if (((size / (1 << data_width)) > SPI_SOC_TRANSFER_COUNT_MAX) ||
        (i2s->bclk.ptr == NULL) ||
        (i2s->lrck.ptr == NULL) ||
        (i2s->transfer_time.ptr == NULL)) {
        return status_invalid_argument;
    }
    /* set bclk */
    if (i2s->bclk.ptr == NULL) {
        return status_invalid_argument;
    }
    clock_add_to_group(i2s->bclk.clock_name, 0);
    gptmr_channel_get_default_config(i2s->bclk.ptr, &config);
    pwm_freq_hz = clock_get_frequency(i2s->bclk.clock_name);
    actual_reload = pwm_freq_hz / bclk_hz;
    config.cmp[0] = 2;
    config.reload = actual_reload;
    config.cmp[1] = (actual_reload / 2);
    config.cmp_initial_polarity_high = false;
    config.enable_software_sync = false;
    config.enable_cmp_output = true;
    HPM_CHECK_RET(gptmr_channel_config(i2s->bclk.ptr, i2s->bclk.channel, &config, false));
    gptmr_channel_reset_count(i2s->bclk.ptr, i2s->bclk.channel);

    /* set lrck */
    clock_add_to_group(i2s->lrck.clock_name, 0);
    actual_reload *= (audio_depth * 2);
    actual_fclk_rld = actual_reload;
    config.enable_cmp_output = true;
    config.reload = actual_reload;
    config.cmp[0] = 2;
    config.cmp[1] = (actual_reload / 2);
    HPM_CHECK_RET(gptmr_channel_config(i2s->lrck.ptr, i2s->lrck.channel, &config, false));
    gptmr_channel_reset_count(i2s->lrck.ptr, i2s->lrck.channel);

    /* set transfer is completed time */
    clock_add_to_group(i2s->transfer_time.clock_name, 0);
    gptmr_channel_get_default_config(i2s->transfer_time.ptr, &config);
    /* left and right so /2*/
    actual_fclk_half_rld = (actual_reload / 2);
    actual_reload = actual_fclk_half_rld * (size / (audio_depth / 8));
    /* wait 20 half_fclk tick for rx finish */
    actual_reload += (actual_fclk_half_rld * 20);
    config.reload = actual_reload;
    HPM_CHECK_RET(gptmr_channel_config(i2s->transfer_time.ptr, i2s->transfer_time.channel, &config, false));
    gptmr_channel_reset_count(i2s->transfer_time.ptr, i2s->transfer_time.channel);

    /* set mclk*/
    if (i2s->mclk.ptr != NULL) {
        clock_add_to_group(i2s->mclk.clock_name, 0);
        gptmr_channel_get_default_config(i2s->mclk.ptr, &config);
        actual_reload = actual_fclk_rld / 256;
        config.reload = actual_reload;
        config.cmp[0] = actual_reload / 2;
        config.cmp[1] = actual_reload;
        config.enable_cmp_output = true;
        gptmr_channel_config(i2s->mclk.ptr, i2s->mclk.channel, &config, false);
        gptmr_channel_reset_count(i2s->mclk.ptr, i2s->mclk.channel);
    }
    return stat;
}
