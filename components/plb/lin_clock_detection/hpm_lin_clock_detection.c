/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_lin_clock_detection.h"
#include "board.h"

#define CLOCK_BIT_LENGTH (8U)

hpm_stat_t plb_lin_clock_detection_init(plb_lin_clock_t *cfg)
{
    trgm_output_t trgm_output_cfg;
    trgm_filter_t trgm_filter_cfg = {0};
    dma_handshake_config_t config;
    hpm_stat_t stat;

    cfg->run_data.threshold = (cfg->freq / cfg->lin_max_freq) * cfg->clock_error;
    cfg->run_data.index_save = 0;
    cfg->run_data.index_read = 0;
    cfg->run_data.freq_hz = -1;
    cfg->run_data.start_buf[0] = 0;
    cfg->run_data.start_buf[1] = 0;
    cfg->run_data.start_buf[2] = 0;
    cfg->run_data.run_buffer_len_half = cfg->run_buffer_len / 2;

    plb_type_b_inject_by_sw(cfg->device.plb, cfg->device.plb_type_b_chn, 0);

    for (uint8_t i = plb_type_b_slice_0; i <= plb_type_b_slice_15; i++) {
        plb_type_b_set_lut(cfg->device.plb, cfg->device.plb_type_b_chn, i, plb_slice_opt_keep);
    }
    plb_type_b_set_lut(cfg->device.plb, cfg->device.plb_type_b_chn, plb_type_b_slice_0, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->device.plb, cfg->device.plb_type_b_chn, plb_type_b_slice_1, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->device.plb, cfg->device.plb_type_b_chn, plb_type_b_slice_2, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->device.plb, cfg->device.plb_type_b_chn, plb_type_b_slice_3, plb_slice_opt_add_one);
    plb_type_b_inject_by_sw(cfg->device.plb, cfg->device.plb_type_b_chn, 0);

    trgm_filter_cfg.sync = true;
    trgm_filter_cfg.invert = false;
    trgm_filter_cfg.mode = trgm_filter_mode_rapid_change;
    trgm_filter_cfg.filter_length = cfg->filter_length;
    trgm_filter_config(cfg->device.trgm, BOARD_PLB_TRGM_FILTER_GPIO_INPUT0 + (cfg->device.trgmux_pin - HPM_TRGM0_INPUT_SRC_TRGM0_P00), &trgm_filter_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_both_edge;
    trgm_output_cfg.input  = cfg->device.trgmux_pin;
    trgm_output_config(cfg->device.trgm, TRGM_TRGOCFG_TRGM_DMA0 + cfg->device.trgm_dma_src_index, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = cfg->device.trgmux_pin;
    trgm_output_config(cfg->device.trgm, TRGM_TRGOCFG_PLB_IN_00 + cfg->device.plb_type_b_chn * 4, &trgm_output_cfg);


    dmamux_config(cfg->device.dma_mux, DMA_SOC_CHN_TO_DMAMUX_CHN(cfg->device.dma, cfg->device.dma_chn),
                HPM_DMA_SRC_MOT_0 + cfg->device.trgm_dma_dst_index, true);
    trgm_dma_request_config(cfg->device.trgm, TRGM_DMACFG_0 + cfg->device.trgm_dma_dst_index, BOARD_PLB_TRGM_DMA_REQ0 + cfg->device.trgm_dma_src_index);


    dma_default_handshake_config(cfg->device.dma, &config);
    config.ch_index = cfg->device.dma_chn;
    config.dst = (uint32_t)cfg->dma_buffer_addr;
    config.dst_fixed = false;
    config.src = (uint32_t)&cfg->device.plb->TYPE_B[cfg->device.plb_type_b_chn].SW_INJECT;
    config.src_fixed = true;
    config.data_width = DMA_TRANSFER_WIDTH_WORD;
    config.size_in_byte = 4;

    stat = dma_setup_handshake(cfg->device.dma, &config, true);
    if (stat != status_success) {
        return stat;
    }
    dma_set_infinite_loop_mode(cfg->device.dma, cfg->device.dma_chn, true);

    return status_success;
}

int32_t plb_lin_buffer_check_clock(uint32_t *buffer, plb_lin_clock_t *cfg)
{
    uint32_t *data = cfg->run_buffer;
    uint32_t count = 0;
    int32_t start = -1, end = -1;
    uint32_t clock;
    uint32_t delta = 0;
    uint32_t delta_last = 0;
    int32_t delta_delta;
    uint32_t delta_start;
    uint16_t i, j;

    data[cfg->run_data.index_save++] = *buffer;
    if (cfg->run_data.index_save >= cfg->run_data.run_buffer_len_half) {
        cfg->run_data.index_save = 0;
    }
    for (i = cfg->run_data.run_buffer_len_half; i < cfg->run_buffer_len; i++) {
        data[i] = 0;
    }
    if (cfg->run_data.index_save >= cfg->run_data.index_read) {
        j = 0;
        for (i = cfg->run_data.index_read; i < cfg->run_data.index_save; i++) {
            data[cfg->run_data.run_buffer_len_half + j] = data[i];
            j++;
        }
    } else {
        j = 0;
        for (i = cfg->run_data.index_read; i < cfg->run_data.run_buffer_len_half; i++) {
            data[cfg->run_data.run_buffer_len_half + j] = data[i];
            j++;
        }
        for (i = 0; i < cfg->run_data.index_save; i++) {
            data[cfg->run_data.run_buffer_len_half + j] = data[i];
            j++;
        }
    }

    for (i = cfg->run_data.run_buffer_len_half; i < cfg->run_buffer_len; i++) {
        delta = data[i] - data[i-1];
        delta_delta = delta - delta_last;
        delta_last = delta;
        if ((abs(delta_delta) < cfg->run_data.threshold) && (delta != 0)) {
            count++;
            if (count == 1) {
                start = i - 1;
            }
        } else {
            if (count >= CLOCK_BIT_LENGTH) {
                end = i - 1;
                break;
            }
            count = 0;
        }
    }
    if (count >= CLOCK_BIT_LENGTH) {
        end = i - 1;
    }
    if (end != -1) {
        cfg->run_data.index_read += end - cfg->run_data.run_buffer_len_half;
        if (cfg->run_data.index_read >= cfg->run_data.run_buffer_len_half) {
            cfg->run_data.index_read -= cfg->run_data.run_buffer_len_half;
        }
    }

    if (count >= CLOCK_BIT_LENGTH) {
        if (start >= (cfg->run_data.run_buffer_len_half + 3)) {
            clock = cfg->freq / ((data[end] - data[start]) / count);
            delta = data[start] - data[start - 1];
            delta_start = data[start - 2] - data[start - 3];
            if ((delta_start / 11) < delta) {
                clock = -1;
            }
            if (clock == 0) {
                clock = -1;
            }
        } else {
            clock = cfg->freq / ((data[end] - data[start]) / count);
            delta = data[start] - data[start - 1];
            uint32_t buf0, buf1;
            switch (start) {
            case 0:
                buf0 = cfg->run_data.start_buf[1];
                buf1 = cfg->run_data.start_buf[0];
                break;
            case 1:
                buf0 = cfg->run_data.start_buf[2];
                buf1 = cfg->run_data.start_buf[1];
                break;
            case 2 :
                buf0 = data[0];
                buf1 = cfg->run_data.start_buf[2];
                break;
            default:
                buf0 = 0;
                buf1 = 0;
                break;
            }
            delta_start = buf0 - buf1;
            if ((delta_start / 11) < delta) {
                clock = -1;
            }
            if (clock == 0) {
                clock = -1;
            }
        }
    } else {
        clock = -1;
    }

    cfg->run_data.start_buf[0] = data[cfg->run_buffer_len - 3];
    cfg->run_data.start_buf[1] = data[cfg->run_buffer_len - 2];
    cfg->run_data.start_buf[2] = data[cfg->run_buffer_len - 1];

    return clock;
}

void plb_lin_reset_clock(plb_lin_clock_t *cfg, uint8_t chn)
{
    plb_type_b_inject_by_sw(cfg->device.plb, chn, 0);
}

void plb_lin_dma_isr_function(plb_lin_clock_t *cfg)
{
    uint32_t status;

    status = dma_check_transfer_status(cfg->device.dma, cfg->device.dma_chn);
    if ((status & DMA_CHANNEL_STATUS_TC) != 0) {
        cfg->run_data.freq_hz = plb_lin_buffer_check_clock((uint32_t *)cfg->dma_buffer, cfg);
        if (cfg->run_data.freq_hz != -1) {
            plb_lin_reset_clock(cfg, cfg->device.dma_chn);
            if (cfg->call_back != NULL) {
                cfg->call_back(cfg->run_data.freq_hz);
                cfg->run_data.freq_hz = -1;
            }
        }
    }
}