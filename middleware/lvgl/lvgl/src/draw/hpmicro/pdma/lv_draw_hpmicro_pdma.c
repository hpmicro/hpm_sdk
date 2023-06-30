/**
 * @file lv_draw_hpmicro_pdma.c
 *
 */

/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_hpmicro_pdma.h"

#if defined(LV_USE_GPU_HPMICRO_PDMA) && LV_USE_GPU_HPMICRO_PDMA

#include "../../../core/lv_refr.h"
#include "hpm_pdma_drv.h"
#include "hpm_l1c_drv.h"

/*********************
 *      DEFINES
 *********************/

/* Minimum area (in pixels) for HPMICRO PDMA processing, otherwise draw of sw is uesd. */
#ifndef LV_GPU_HPMICRO_PDMA_MINIMUM_SIZE
    #define LV_GPU_HPMICRO_PDMA_MINIMUM_SIZE 128 * 128
#endif

#define LV_HPM_PDMA_BASE HPM_PDMA
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM static void lv_draw_hpmicro_pdma_wait_for_finish(lv_draw_ctx_t *draw_ctx);
LV_ATTRIBUTE_FAST_MEM static void lv_draw_hpmicro_pdma_blend_basic(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc);
static void lv_gpu_hpmicro_pdma_fill(lv_color_t *dest_buf, const lv_area_t *dest_area, lv_coord_t dest_stride,
                         lv_color_t color, lv_opa_t opa);
static void lv_gpu_hpmicro_pdma_blit(lv_color_t *dest_buf, const lv_area_t *dest_area, lv_coord_t dest_stride,
                         const lv_color_t *src_buf, lv_coord_t src_stride, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_hpmicro_pdma_ctx_init(lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_hpmicro_pdma_ctx_t *hpmicro_pdma_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;
    hpmicro_pdma_draw_ctx->blend = lv_draw_hpmicro_pdma_blend_basic;
    hpmicro_pdma_draw_ctx->base_draw.wait_for_finish = lv_draw_hpmicro_pdma_wait_for_finish;
}

void lv_draw_hpmicro_pdma_ctx_deinit(lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx)
{
    lv_draw_sw_deinit_ctx(drv, draw_ctx);
}

static void lv_draw_hpmicro_pdma_wait_for_finish(lv_draw_ctx_t *draw_ctx)
{

}

LV_ATTRIBUTE_FAST_MEM void lv_draw_hpmicro_pdma_blend_basic(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc)
{
    lv_opa_t *mask;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;

    if (mask || dsc->blend_mode != LV_BLEND_MODE_NORMAL ||
        (lv_area_get_size(&blend_area) < LV_GPU_HPMICRO_PDMA_MINIMUM_SIZE)) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }

    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    lv_color_t *dest_buf = draw_ctx->buf;
    if(disp->driver->set_px_cb == NULL) {
        if(disp->driver->screen_transp == 0) {
            dest_buf += dest_stride *(blend_area.y1 - draw_ctx->buf_area->y1) + (blend_area.x1 - draw_ctx->buf_area->x1);
        }
        else {
            /*With LV_COLOR_DEPTH 16 it means ARGB8565 (3 bytes format)*/
            lv_draw_sw_blend_basic(draw_ctx, dsc);
            return;
        }
    }

    const lv_color_t *src_buf = dsc->src_buf;
    lv_coord_t src_stride;
    if(src_buf) {
        src_stride = lv_area_get_width(dsc->blend_area);
        src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 - dsc->blend_area->x1);
    }
    else {
        src_stride = 0;
    }

    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    if(disp->driver->set_px_cb) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }
#if LV_COLOR_SCREEN_TRANSP
    else if(disp->driver->screen_transp) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }
#endif
    else if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(dsc->src_buf == NULL) {
            lv_gpu_hpmicro_pdma_fill(dest_buf, &blend_area, dest_stride, dsc->color, dsc->opa);
        }
        else {
            lv_gpu_hpmicro_pdma_blit(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa);
        }
    }
    else {
#if LV_DRAW_COMPLEX
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
#endif
    }
}

static void lv_gpu_hpmicro_pdma_fill(lv_color_t *dest_buf, const lv_area_t *dest_area, lv_coord_t dest_stride,
                         lv_color_t color, lv_opa_t opa)
{
#if defined(LV_COLOR_DEPTH) && LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
    display_pixel_format_t format = display_pixel_format_rgb565;
#elif defined(LV_COLOR_DEPTH) && LV_COLOR_DEPTH == 32
    display_pixel_format_t format = display_pixel_format_argb8888;
#else
#error "hpmicro pdma support 16(no swap) or 32 color depth only!!!"
#endif

    uint32_t width = lv_area_get_width(dest_area);
    uint32_t height = lv_area_get_height(dest_area);

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dest_buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dest_buf + dest_stride * height * sizeof(lv_color_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }

    pdma_config_t config;
    pdma_plane_config_t plane_src;
    pdma_plane_config_t plane_dst;
    display_yuv2rgb_coef_t yuv2rgb_coef;
    pdma_output_config_t output;

    pdma_get_default_config(LV_HPM_PDMA_BASE, &config, format);
    pdma_get_default_plane_config(LV_HPM_PDMA_BASE, &plane_src, format);
    pdma_get_default_plane_config(LV_HPM_PDMA_BASE, &plane_dst, format);
    pdma_get_default_yuv2rgb_coef_config(LV_HPM_PDMA_BASE, &yuv2rgb_coef, format);
    pdma_get_default_output_config(LV_HPM_PDMA_BASE, &output, format);

    config.enable_plane = pdma_plane_both;
    config.block_size = pdma_blocksize_16x16;

    pdma_init(LV_HPM_PDMA_BASE, &config);

    plane_src.buffer = (uint32_t)dest_buf;
    plane_src.width = width;
    plane_src.height = height;
    plane_src.pitch = dest_stride * sizeof(lv_color_t);
    plane_src.background = 0;
    plane_src.use_background_as_clear = false;

    uint32_t bg_color = lv_color_to32(color);
    plane_dst.buffer = (uint32_t)dest_buf;
    plane_dst.width = width;
    plane_dst.height = height;
    plane_dst.pitch = dest_stride * sizeof(lv_color_t);
    plane_dst.background = (uint32_t)opa<<24 | (bg_color & 0x00fffffful);
    plane_dst.use_background_as_clear = true;
    pdma_config_planes(LV_HPM_PDMA_BASE, &plane_src, &plane_dst, &yuv2rgb_coef);

    output.plane[pdma_plane_src].x = 0;
    output.plane[pdma_plane_src].y = 0;
    output.plane[pdma_plane_src].width = width;
    output.plane[pdma_plane_src].height = height;

    output.plane[pdma_plane_dst].x = 0;
    output.plane[pdma_plane_dst].y = 0;
    output.plane[pdma_plane_dst].width = 0;
    output.plane[pdma_plane_dst].height = 0;

    output.buffer = plane_dst.buffer;
    output.pitch = plane_dst.pitch;
    output.width = plane_dst.width;
    output.height = plane_dst.height;

    output.alphablend.mode = display_alphablend_mode_plus;
    output.alphablend.src_alpha = 0xff - opa;
    output.alphablend.src_alpha_op = display_alpha_op_override;
    output.alphablend.dst_alpha = opa;
    output.alphablend.dst_alpha_op = display_alpha_op_invalid;

    pdma_config_output(LV_HPM_PDMA_BASE, &output);
    pdma_start(LV_HPM_PDMA_BASE);
    hpm_stat_t stat;
    uint32_t status;
    do {
        stat = pdma_check_status(LV_HPM_PDMA_BASE, &status);
    } while ((stat != status_pdma_done) && (stat != status_pdma_error));
    pdma_stop(LV_HPM_PDMA_BASE);
}

void lv_gpu_hpmicro_pdma_blit(lv_color_t *dest_buf, const lv_area_t *dest_area, lv_coord_t dest_stride,
                         const lv_color_t *src_buf, lv_coord_t src_stride, lv_opa_t opa)
{
#if defined(LV_COLOR_DEPTH) && LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
    display_pixel_format_t format = display_pixel_format_rgb565;
#elif defined(LV_COLOR_DEPTH) && LV_COLOR_DEPTH == 32
    display_pixel_format_t format = display_pixel_format_argb8888;
#else
#error "hpmicro pdma support 16(no swap) or 32 color depth only!!!"
#endif

    uint32_t width = lv_area_get_width(dest_area);
    uint32_t height = lv_area_get_height(dest_area);

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src_buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src_buf + src_stride * height * sizeof(lv_color_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dest_buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dest_buf + dest_stride * height * sizeof(lv_color_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }

    pdma_config_t config;
    pdma_plane_config_t plane_src;
    pdma_plane_config_t plane_dst;
    display_yuv2rgb_coef_t yuv2rgb_coef;
    pdma_output_config_t output;

    pdma_get_default_config(LV_HPM_PDMA_BASE, &config, format);
    pdma_get_default_plane_config(LV_HPM_PDMA_BASE, &plane_src, format);
    pdma_get_default_plane_config(LV_HPM_PDMA_BASE, &plane_dst, format);
    pdma_get_default_yuv2rgb_coef_config(LV_HPM_PDMA_BASE, &yuv2rgb_coef, format);
    pdma_get_default_output_config(LV_HPM_PDMA_BASE, &output, format);

    config.enable_plane = pdma_plane_both;
    config.block_size = pdma_blocksize_16x16;

    pdma_init(LV_HPM_PDMA_BASE, &config);

    plane_src.buffer = (uint32_t)src_buf;
    plane_src.width = width;
    plane_src.height = height;
    plane_src.pitch = src_stride * sizeof(lv_color_t);
    plane_src.background = 0;
    plane_src.use_background_as_clear = false;
    
    plane_dst.buffer = (uint32_t)dest_buf;
    plane_dst.width = width;
    plane_dst.height = height;
    plane_dst.pitch = dest_stride * sizeof(lv_color_t);
    plane_dst.background = 0;
    plane_dst.use_background_as_clear = false;
    pdma_config_planes(LV_HPM_PDMA_BASE, &plane_src, &plane_dst, &yuv2rgb_coef);

    output.plane[pdma_plane_src].x = 0;
    output.plane[pdma_plane_src].y = 0;
    output.plane[pdma_plane_src].width = width;
    output.plane[pdma_plane_src].height = height;

    output.plane[pdma_plane_dst].x = 0;
    output.plane[pdma_plane_dst].y = 0;
    output.plane[pdma_plane_dst].width = width;
    output.plane[pdma_plane_dst].height = height;

    output.buffer = (uint32_t)dest_buf;
    output.pitch = dest_stride * sizeof(lv_color_t);
    output.width = width;
    output.height = height;

    output.alphablend.mode = display_alphablend_mode_plus;
    output.alphablend.src_alpha = opa;
    output.alphablend.src_alpha_op = display_alpha_op_override;
    output.alphablend.dst_alpha = 0xff - opa;
    output.alphablend.dst_alpha_op = display_alpha_op_override;

    pdma_config_output(LV_HPM_PDMA_BASE, &output);
    pdma_start(LV_HPM_PDMA_BASE);

    hpm_stat_t stat;
    uint32_t status;
    do {
        stat = pdma_check_status(LV_HPM_PDMA_BASE, &status);
    } while ((stat != status_pdma_done) && (stat != status_pdma_error));
    pdma_stop(LV_HPM_PDMA_BASE);
}

#endif /*LV_USE_GPU_HPMICRO_PDMA*/