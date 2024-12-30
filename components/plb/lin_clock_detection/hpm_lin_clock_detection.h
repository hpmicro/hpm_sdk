/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_QEI_ENCODER_H
#define _HPM_QEI_ENCODER_H

#include "hpm_soc_feature.h"
#include "hpm_plb_drv.h"
#include "hpm_trgm_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include <math.h>

/**
 * @brief LIN clock detection
 *
 */
typedef struct {
    uint32_t lin_max_freq;
    uint32_t *dma_buffer;
    uint32_t dma_buffer_addr;
    uint32_t *run_buffer;
    uint16_t run_buffer_len;
    uint32_t freq;
    uint16_t filter_length;
    float clock_error;
    struct {
        PLB_Type *plb;
        TRGM_Type *trgm;
        uint8_t plb_type_b_chn;
        DMAMUX_Type *dma_mux;
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
        DMAV2_Type *dma;
#else
        DMA_Type *dma;
#endif
        uint8_t dma_chn;
        uint16_t trgmux_pin;
        uint16_t trgm_dma_src_index;
        uint16_t trgm_dma_dst_index;
    } device;
    void (*call_back)(uint32_t clk_hz);
    struct {
        uint16_t index_save;
        uint16_t index_read;
        uint16_t run_buffer_len_half;
        uint32_t threshold;
        int32_t freq_hz;
        uint32_t start_buf[3];
    } run_data;

} plb_lin_clock_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief plb lin clock detection isr function
 *
 * @param cfg @ref plb_lin_clock_t
 */
void plb_lin_dma_isr_function(plb_lin_clock_t *cfg);

/**
 * @brief plb lin clock detection init
 *
 * @param cfg @ref plb_lin_clock_t
 * @return hpm_stat_t
 */
hpm_stat_t plb_lin_clock_detection_init(plb_lin_clock_t *cfg);

/**
 * @brief get detection status
 *
 * The interface is not available after the callback function is enabled
 *
 * @param cfg @ref plb_lin_clock_t
 * @return hpm_stat_t
 */
static inline bool plb_lin_clock_get_detection_status(plb_lin_clock_t *cfg)
{
    if (cfg->run_data.freq_hz == -1) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief get detection freq
 *
 * The interface is not available after the callback function is enabled
 *
 * @param cfg @ref plb_lin_clock_t
 * @return uint32_t
 */
static inline uint32_t plb_lin_clock_get_detection_freq(plb_lin_clock_t *cfg)
{
    uint32_t clock;

    clock = cfg->run_data.freq_hz;
    cfg->run_data.freq_hz = -1;

    return clock;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
