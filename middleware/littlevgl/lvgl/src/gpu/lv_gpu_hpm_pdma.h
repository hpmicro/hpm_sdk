/**
 * @file lv_gpu_hpm_pdma.h
 *
 */

#ifndef LV_GPU_HPM_PDMA_H
#define LV_GPU_HPM_PDMA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area.h"
#include "../misc/lv_color.h"
#include "../hal/lv_hal_disp.h"

/*********************
 *      DEFINES
 *********************/
#define LV_GPU_HPM_PDMA_ID HPM_PDMA

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_gpu_hpm_pdma_init(void);

/**
 * Fill an area in the buffer with a color
 * @param buf a buffer which should be filled
 * @param buf_w width of the buffer in pixels
 * @param color fill color
 * @param fill_w width to fill in pixels (<= buf_w)
 * @param fill_h height to fill in pixels
 * @note `buf_w - fill_w` is offset to the next line after fill
 */
void lv_gpu_hpm_pdma_fill(lv_color_t * buf, lv_coord_t buf_w,
                          lv_color_t color, lv_coord_t fill_w,
                          lv_coord_t fill_h, lv_opa_t opa);

/**
 * Blend a map (e.g. ARGB image or RGB image with opacity) to a buffer
 *
 * By default, image is copied directly, with optional opacity configured by \p opa.
 * Color keying can be enabled by calling lv_gpu_hpm_pdma_enable_color_key() before calling this function.
 * Recoloring can be enabled by calling lv_gpu_hpm_pdma_enable_recolor() before calling this function.
 * Note that color keying and recoloring at the same time is not supported and black rectangle is rendered.
 
 * @param buf a buffer where `map` should be copied
 * @param buf_w width of the buffer in pixels
 * @param map an "image" to copy
 * @param opa opacity of `map`
 * @param map_w width of the map in pixels
 * @param copy_w width of the area to copy in pixels (<= buf_w)
 * @param copy_h height of the area to copy in pixels
 * @note `map_w - fill_w` is offset to the next line after copy
 */
void lv_gpu_hpm_pdma_blit(lv_color_t *dst, lv_coord_t dst_width,
                          const lv_color_t * src, lv_coord_t src_width,
                          lv_coord_t width, lv_coord_t height, lv_opa_t opa);

/**
 * @brief Disable color keying for subsequent calls
 *
 */
void lv_gpu_hpm_pdma_disable_color_key(void);

/**
 * @brief Enable color keying for subsequent calls
 *
 */
void lv_gpu_hpm_pdma_enable_color_key(void);

/**
 * @brief Enable recolor feature for subsequent calls
 *
 * @param[in] c recolor value
 * @param[in] opa effect opacity
 */
void lv_gpu_hpm_pdma_enable_recolor(lv_color_t c, lv_opa_t opa);

/**
 * @brief Disable recolor feature for subsequent calls
 */
void lv_gpu_hpm_pdma_disable_recolor(void);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GPU_HPM_PDMA_H*/
