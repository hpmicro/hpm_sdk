/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_I2S_OVER_SPI_H
#define _HPM_I2S_OVER_SPI_H
#include "hpm_common.h"
#include "hpm_i2s_common.h"
#include "hpm_gptmr_drv.h"
#include "hpm_spi_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_dma_mgr.h"

typedef void (*i2s_rx_data_tc)(uint32_t cb_data_ptr);

typedef struct {
    GPTMR_Type *ptr;
    clock_name_t clock_name;
    uint8_t channel;
} hpm_i2s_gptmr_context_t;

typedef struct {
    SPI_Type *ptr;
    clock_name_t clock_name;
    /* dmamux source */
    uint16_t txdma_src;
    uint16_t rxdma_src;
    /* need external cs line to drive spi slave */
    uint32_t cs_pin;
    void (*write_cs)(uint32_t cs_pin, uint8_t state);
} hpm_i2s_spi_context_t;

typedef struct {
    dma_resource_t *resource;
    dma_linked_descriptor_t *descriptors;
} hpm_i2s_dma_context_t;

typedef struct hpm_i2s_over_spi {
    /* bit clock */
    hpm_i2s_gptmr_context_t bclk;
    /* word select */
    hpm_i2s_gptmr_context_t lrck;
    /* master clock */
    hpm_i2s_gptmr_context_t mclk;
    /* spi slave as tx and rx data line */
    hpm_i2s_spi_context_t spi_slave;
    hpm_i2s_dma_context_t tx_dma;
    hpm_i2s_dma_context_t rx_dma;
    /* switch rx */
    bool i2s_rx;
    i2s_rx_data_tc rx_callback;
    /* record the time when an i2s transfer is completed */
    hpm_i2s_gptmr_context_t transfer_time;
    bool has_done;
    void (*transfer_complete)(struct hpm_i2s_over_spi *i2s);
} hpm_i2s_over_spi_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Transfer complete callback for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 */
void hpm_i2s_master_over_spi_transfer_complete_callback(hpm_i2s_over_spi_t *i2s);

/**
 * @brief Initialization for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 *
 * @retval status_success if no error occurred
 */
hpm_stat_t hpm_i2s_master_over_spi_init(hpm_i2s_over_spi_t *i2s);

/**
 * @brief Transmit for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 * @param [in] protocol i2s protocol, only support I2S_PROTOCOL_MSB_JUSTIFIED I2S_PROTOCOL_LSB_JUSTIFIED
 * @param [in] lrck_hz switch left and right channels frequency, unit: hz
 * @param [in] audio_depth audio depth only support 16bits and 32bits
 * @param [in] data data pointer
 * @param [in] size transmit size
 *
 * @retval status_success if no error occurred
 */
hpm_stat_t hpm_i2s_master_over_spi_tx_buffer(hpm_i2s_over_spi_t *i2s,
                                             uint8_t protocol, uint32_t lrck_hz,
                                             uint8_t audio_depth, uint8_t *data,
                                             uint32_t size);

/**
 * @brief Stop Transmission for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 *
 * @retval status_success if no error occurred
 */
hpm_stat_t hpm_i2s_master_over_spi_tx_stop(hpm_i2s_over_spi_t *i2s);

/**
 * @brief Receiving configuration for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 * @param [in] protocol i2s protocol, only support I2S_PROTOCOL_MSB_JUSTIFIED I2S_PROTOCOL_LSB_JUSTIFIED
 * @param [in] lrck_hz switch left and right channels frequency, unit: hz
 * @param [in] audio_depth audio depth only support 16bits and 32bits
 * @param [in] buffer0 buffer0 pointer
 * @param [in] buffer1 buffer1 pointer
 * @param [in] size receive size
 *
 * @retval status_success if no error occurred
 */
hpm_stat_t hpm_i2s_master_over_spi_rx_config(hpm_i2s_over_spi_t *i2s, uint8_t protocol,
                                      uint32_t lrck_hz, uint32_t audio_depth,
                                      uint8_t *buffer0, uint8_t *buffer1,
                                      uint32_t size);

/**
 * @brief Start receiving for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 * @param [in] callback callback after each buffer is received
 *
 * @retval status_success if no error occurred
 */
hpm_stat_t hpm_i2s_master_over_spi_rx_start(hpm_i2s_over_spi_t *i2s, i2s_rx_data_tc callback);

/**
 * @brief Stop receiving for i2s master over spi
 *
 * @param [in] i2s i2s over spi context
 *
 * @retval status_success if no error occurred
 */
hpm_stat_t hpm_i2s_master_over_spi_rx_stop(hpm_i2s_over_spi_t *i2s);

#ifdef __cplusplus
}
#endif

#endif /* _HPM_I2S_OVER_SPI_H */

