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
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "hal/lv_hal_disp.h"

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
 * @brief Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_gpu_hpm_pdma_init(void);

/**
 * @brief Fill an area in the buffer with a color
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
 * @brief BLock Image Transfer - copy rectangular image from src buffer to dst buffer with effects.
 *
 * Image is copied directly, with optional opacity configured by \p opa
 *
 * @param[in/out] dst destination buffer
 * @param[in] dst_width width (stride) of destination buffer in pixels
 * @param[in] src source buffer
 * @param[in] src_with width (stride) of source buffer in pixels
 * @param[in] width width of area to be copied from src to dest
 * @param[in] height height of area to be copied from src to dest
 * @param[in] opa opacity of the result
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
