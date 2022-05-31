/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_CAM_DRV_H
#define HPM_CAM_DRV_H

#include "hpm_common.h"
#include "hpm_display_common.h"
#include "hpm_cam_regs.h"

/**
 * @brief CAM driver APIs
 * @defgroup cam_interface CAM driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief CAM data store mode
 */
#define CAM_DATA_STORE_MODE_NORMAL (0U)
#define CAM_DATA_STORE_MODE_Y_UV_PLANES (CAM_CR1_STORAGE_MODE_SET(1))
#define CAM_DATA_STORE_MODE_Y_ONLY (CAM_CR1_STORAGE_MODE_SET(2))
#define CAM_DATA_STORE_MODE_BINARY (CAM_CR1_STORAGE_MODE_SET(3))

/**
 * @brief CAM sensor bitwidth
 */
#define CAM_SENSOR_BITWIDTH_8BITS (CAM_CR1_SENSOR_BIT_WIDTH_SET(0))
#define CAM_SENSOR_BITWIDTH_10BITS (CAM_CR1_SENSOR_BIT_WIDTH_SET(1))

/**
 * @brief CAM IRQ flag
 */
#define CAM_IRQ_UNSUPPORTED_CONFIGURATION (CAM_INT_EN_ERR_CL_BWID_CFG_INT_EN_MASK)
#define CAM_IRQ_HIST_CALCULATION_DONE (CAM_INT_EN_HIST_DONE_INT_EN_MASK)
#define CAM_IRQ_HRESPONSE_ERROR (CAM_INT_EN_HRESP_ERR_EN_MASK)
#define CAM_IRQ_END_OF_FRAME (CAM_INT_EN_EOF_INT_EN_MASK)
#define CAM_IRQ_STAT_FIFO_OVERRUN (CAM_INT_EN_SF_OR_INT_EN_MASK)
#define CAM_IRQ_RX_FIFO_OVERRUN (CAM_INT_EN_RF_OR_INT_EN_MASK)
#define CAM_IRQ_STAT_FIFO_DMA_TRANSFER_DONE (CAM_INT_EN_SFF_DMA_DONE_INT_EN_MASK)
#define CAM_IRQ_FB2_DMA_TRANSFER_DONE (CAM_INT_EN_FB2_DMA_DONE_INT_EN_MASK)
#define CAM_IRQ_FB1_DMA_TRANSFER_DONE (CAM_INT_EN_FB1_DMA_DONE_INT_EN_MASK)
#define CAM_IRQ_START_OF_FRAME (CAM_INT_EN_SOF_INT_EN_MASK)

/**
 * @brief CAM status flag
 */
#define CAM_STATUS_UNSUPPORTED_CONFIGURATION (CAM_STA_ERR_CL_BWID_CFG_MASK)
#define CAM_STATUS_HIST_CALCULATION_DONE (CAM_STA_HIST_DONE_MASK)
#define CAM_STATUS_STAT_FIFO_OVERRUN (CAM_STA_SF_OR_INT_MASK)
#define CAM_STATUS_RX_FIFO_OVERRUN (CAM_STA_RF_OR_INT_MASK)
#define CAM_STATUS_STAT_FIFO_DMA_TRANSFER_DONE (CAM_STA_DMA_TSF_DONE_SFF_MASK)
#define CAM_STATUS_STAT_FIFO_FULL (CAM_STA_STATFF_INT_MASK)
#define CAM_STATUS_FB2_DMA_TRANSFER_DONE (CAM_STA_DMA_TSF_DONE_FB2_MASK)
#define CAM_STATUS_FB1_DMA_TRANSFER_DONE (CAM_STA_DMA_TSF_DONE_FB1_MASK)
#define CAM_STATUS_RX_FIFO_FULL (CAM_STA_RXFF_INT_MASK)
#define CAM_STATUS_END_OF_FRAME (CAM_STA_EOF_INT_MASK)
#define CAM_STATUS_START_OF_FRAME (CAM_STA_SOF_INT_MASK)
#define CAM_STATUS_HRESPONSE_ERROR (CAM_STA_HRESP_ERR_INT_MASK)
#define CAM_STATUS_DATA_READY (CAM_STA_DRDY_MASK)

/**
 * @brief CAM input color format
 */
#define CAM_COLOR_FORMAT_RGB888 (CAM_CR1_COLOR_FORMATS_SET(2))
#define CAM_COLOR_FORMAT_RGB565 (CAM_CR1_COLOR_FORMATS_SET(4))
#define CAM_COLOR_FORMAT_RGB555 (CAM_CR1_COLOR_FORMATS_SET(6))
#define CAM_COLOR_FORMAT_YCBCR422 (CAM_CR1_COLOR_FORMATS_SET(7))
#define CAM_COLOR_FORMAT_YUV444 (CAM_CR1_COLOR_FORMATS_SET(8))

/**
 * @brief CAM config
 */
typedef struct {
    uint32_t width;
    uint32_t height;
    bool pixclk_sampling_falling;
    bool hsync_active_low;
    bool vsync_active_low;
    bool color_ext;
    bool data_pack_msb;
    bool enable_buffer2;
    uint8_t data_store_mode;
    uint8_t color_format;
    uint8_t sensor_bitwidth;
    uint32_t buffer1;
    uint32_t buffer2;
    display_yuv2rgb_config_t csc_config;
} cam_config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CAM set high and low limits of color key
 *
 * @param [in] ptr CAM base address
 * @param [in] high color key high limits
 * @param [in] low color key low limits
 */
static inline void cam_set_color_key(CAM_Type *ptr, uint32_t high, uint32_t low)
{
    ptr->CLRKEY_LOW = CAM_CLRKEY_LOW_LIMIT_SET(low);
    ptr->CLRKEY_HIGH = CAM_CLRKEY_HIGH_LIMIT_SET(high);
}

/**
 * @brief CAM get default config
 *
 * @param [in] ptr CAM base address
 * @param [out] config cam_config_t
 * @param [in] pixel_format display_pixel_format_t
 */
void cam_get_default_config(CAM_Type *ptr, cam_config_t *config, display_pixel_format_t pixel_format);

/**
 * @brief CAM init
 *
 * @param [in] ptr CAM base address
 * @param [in] config cam_config_t
 *
 * @retval hpm_stat_t status_invalid_argument or status_success
 */
hpm_stat_t cam_init(CAM_Type *ptr, cam_config_t *config);

/**
 * @brief CAM start
 *
 * @param [in] ptr CAM base address
 */
void cam_start(CAM_Type *ptr);

/**
 * @brief CAM stop
 *
 * @param [in] ptr CAM base address
 */
void cam_stop(CAM_Type *ptr);

/**
 * @}
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* HPM_CAM_DRV_H */


