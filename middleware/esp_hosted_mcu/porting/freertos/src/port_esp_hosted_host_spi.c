/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 * copyright (c) 2025, HPMicro
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_log.h"

#include "transport_drv.h"
#include "port_esp_hosted_host_spi.h"
#include "port_esp_hosted_host_os.h"
#include "port_esp_hosted_host_log.h"
#include "esp_hosted_os_abstraction.h"
#include "hpm_spi.h"


DEFINE_LOG_TAG(spi_wrapper);

extern void * spi_handle;

#ifndef H_SPI_DMA_TIMEOUT_MS
#define H_SPI_DMA_TIMEOUT_MS      5000000
#endif

#if H_SPI_MODE == 0
#define SPI_CPHA         spi_sclk_sampling_odd_clk_edges
#define SPI_CPOL         spi_sclk_low_idle
#elif H_SPI_MODE == 1
#define SPI_CPHA         spi_sclk_sampling_even_clk_edges
#define SPI_CPOL         spi_sclk_low_idle
#elif H_SPI_MODE == 2
#define SPI_CPHA         spi_sclk_sampling_odd_clk_edges
#define SPI_CPOL         spi_sclk_high_idle
#else // H_SPI_MODE == 3
#define SPI_CPHA         spi_sclk_sampling_even_clk_edges
#define SPI_CPOL         spi_sclk_high_idle
#endif

port_esp_hosted_spi_context_t *spi_ctx = NULL;


void spi_rxdma_complete_callback(DMA_Type *base, uint32_t channel, void *cb_data_ptr)
{
    (void)channel;
    (void)base;
    port_esp_hosted_spi_context_t *ctx = (port_esp_hosted_spi_context_t *)cb_data_ptr;
    g_h.funcs->_h_post_semaphore_from_isr(ctx->rx_dma_mutex);
}


void esp_hosted_spi_register(port_esp_hosted_spi_context_t *ctx)
{
    spi_ctx = ctx;
}

void * hosted_spi_init(void)
{
    spi_initialize_config_t init_config;
    ESP_LOGI(TAG, "Transport: SPI, Mode:%u Freq:%uMHz TxQ:%u RxQ:%u\n GPIOs: CLK:%u MOSI:%u MISO:%u CS:%u HS:%u DR:%u SlaveReset:%u",
            H_SPI_MODE, H_SPI_FD_CLK_MHZ, H_SPI_TX_Q, H_SPI_RX_Q,
            H_GPIO_SCLK_Pin, H_GPIO_MOSI_Pin, H_GPIO_MISO_Pin,
            H_GPIO_CS_Pin, H_GPIO_HANDSHAKE_Pin, H_GPIO_DATA_READY_Pin,
            H_GPIO_PIN_RESET);
    if (!spi_ctx) {
        ESP_LOGE(TAG, "spi_ctx is NULL, call esp_hosted_spi_register first");
        return NULL;
    }
    if (spi_handle) {
        ESP_LOGW(TAG, "SPI already initialized");
        return spi_handle;
    }
    spi_ctx->rx_dma_mutex = g_h.funcs->_h_create_semaphore(1);
    ESP_RETURN_ON_FALSE(spi_ctx->rx_dma_mutex, NULL, TAG, "Failed to create SPI RX DMA mutex");
    g_h.funcs->_h_get_semaphore(spi_ctx->rx_dma_mutex, 0);
    ESP_RETURN_ON_FALSE(spi_ctx->rx_dma_mutex, NULL, TAG, "Failed to create SPI RX DMA mutex"); 
    /* Initialize SPI bus */
    hpm_spi_get_default_init_config(&init_config);
    init_config.mode = spi_master_mode;
    init_config.clk_phase = SPI_CPHA;
    init_config.clk_polarity = SPI_CPOL;
    init_config.data_len = 8;
    if (hpm_spi_initialize(spi_ctx->spi_ptr, &init_config) != status_success) {
        ESP_LOGE(TAG, "hpm_spi_initialize, fail\n");
        return NULL;
    }
    if (hpm_spi_set_sclk_frequency(spi_ctx->spi_ptr, H_SPI_FD_CLK_MHZ * 1000000) != status_success) {
        printf("hpm_spi_set_sclk_frequency fail\n");
        while (1) {
        }
    }
    if (hpm_spi_rx_dma_mgr_install_custom_callback(spi_ctx->spi_ptr, spi_rxdma_complete_callback, spi_ctx) != status_success) {
        ESP_LOGE(TAG, "hpm_spi_rx_dma_mgr_install_custom_callback fail\n");
        return NULL;
    }
    if (hpm_spi_tx_dma_mgr_install_custom_callback(spi_ctx->spi_ptr, NULL, NULL) != status_success) {
        ESP_LOGE(TAG, "hpm_spi_rx_dma_mgr_install_custom_callback fail\n");
        return NULL;
    }
    return spi_ctx;
}

int hosted_spi_deinit(void *handle)
{
    if (!handle) {
        ESP_LOGE(TAG, "Invalid handle for SPI deinit");
        return -1;
    }
    spi_handle = NULL;

    ESP_LOGI(TAG, "SPI deinitialized");
    return 0;
}


int hosted_do_spi_transfer(void *trans)
{
    struct hosted_transport_context_t * spi_trans = trans;
    uint32_t remain_size = spi_trans->tx_buf_size;
    uint32_t transfer_size = 0;
    uint32_t index = 0;
    g_h.funcs->_h_write_gpio(NULL, CONFIG_ESP_HOSTED_SPI_CS_GPIO_PIN, false);
    if (l1c_dc_is_enabled()) {
        uint32_t tx_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)&spi_trans->tx_buf[0]);
        uint32_t tx_end   = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)&spi_trans->tx_buf[0] + spi_trans->tx_buf_size);
        uint32_t tx_size  = tx_end - tx_start;
        l1c_dc_writeback(tx_start, tx_size);
    }
    while (remain_size > 0) {
        transfer_size = MIN(SPI_SOC_TRANSFER_COUNT_MAX, remain_size);
        if (hpm_spi_transmit_receive_nonblocking(spi_ctx->spi_ptr,
                                                 &spi_trans->tx_buf[index],
                                                 &spi_trans->rx_buf[index],
                                                 transfer_size) != status_success) {
            ESP_LOGE(TAG, "hpm_spi_transmit_receive_nonblocking fail\n");
            return -1;
        }
        if (g_h.funcs->_h_get_semaphore(spi_ctx->rx_dma_mutex, HOSTED_BLOCK_MAX) != RET_OK) {
            ESP_LOGE(TAG, "Timeout waiting for SPI RX DMA");
            return -1;
        }
        index += transfer_size;
        remain_size -= transfer_size;
    }
    if (l1c_dc_is_enabled()) {
        uint32_t rx_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)&spi_trans->rx_buf[0]);
        uint32_t rx_end   = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)&spi_trans->rx_buf[0] + spi_trans->tx_buf_size);
        uint32_t rx_size  = rx_end - rx_start;
        l1c_dc_invalidate(rx_start, rx_size);
    }
    g_h.funcs->_h_write_gpio(NULL, CONFIG_ESP_HOSTED_SPI_CS_GPIO_PIN, true);
    return 0;
}
