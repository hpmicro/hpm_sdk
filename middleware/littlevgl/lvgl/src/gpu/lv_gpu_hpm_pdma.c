/**
 * @file lv_gpu_hpm_pdma.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_GPU_HPM_PDMA
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_pdma_drv.h"
#include "hpm_l1c_drv.h"

#include "../core/lv_disp.h"
#include "../core/lv_refr.h"
#include "lv_gpu_hpm_pdma.h"

/*********************
 *      DEFINES
 *********************/

#if !((LV_COLOR_DEPTH == 16) || (LV_COLOR_DEPTH == 32))
    #error Only 16bit or 32bit color depth is supported. Set LV_COLOR_DEPTH to 16 or 32.
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool enable_recolor = false;
static bool enable_colorkey = false;
static lv_color_t recolor = {.full = 0};
static lv_opa_t recolor_opa = 0;
static uint32_t colorkey;
static display_pixel_format_t format;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_gpu_hpm_pdma_init(void)
{
    enable_recolor = false;
    enable_colorkey = false;
    recolor.full = 0;
    colorkey = lv_color_to32(LV_COLOR_CHROMA_KEY);
#if LV_COLOR_DEPTH == 16
    format = display_pixel_format_rgb565;
#else
    format = display_pixel_format_argb8888;
#endif
}

static void _lv_gpu_hpm_pdma_blit(lv_color_t *dst, lv_coord_t dst_width,
                          const lv_color_t * src, lv_coord_t src_width,
                          lv_coord_t width, lv_coord_t height, lv_opa_t opa)
{
    uint32_t status;
    hpm_stat_t stat;
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * height * display_get_pixel_size_in_byte(format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }

    pdma_config_t config;
    pdma_plane_config_t plane_src;
    pdma_plane_config_t plane_dst;
    display_yuv2rgb_coef_t yuv2rgb_coef;
    pdma_output_config_t output;

    pdma_get_default_config(LV_GPU_HPM_PDMA_ID, &config, format);
    pdma_get_default_plane_config(LV_GPU_HPM_PDMA_ID, &plane_src, format);
    pdma_get_default_plane_config(LV_GPU_HPM_PDMA_ID, &plane_dst, format);
    pdma_get_default_yuv2rgb_coef_config(LV_GPU_HPM_PDMA_ID, &yuv2rgb_coef, format);
    pdma_get_default_output_config(LV_GPU_HPM_PDMA_ID, &output, format);

    config.enable_plane = pdma_plane_both;
    if (width <= 16) {
        config.block_size = pdma_blocksize_8x8;
    } else {
        config.block_size = pdma_blocksize_16x16;
    }
    pdma_init(LV_GPU_HPM_PDMA_ID, &config);

    plane_src.buffer = (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)src);
    plane_src.width = width;
    plane_src.height = height;
    plane_src.pitch = display_get_pitch_length_in_byte(format, src_width);
    if (enable_colorkey) {
        plane_src.colorkey_high = colorkey;
        plane_src.colorkey_low = colorkey;
    }

    plane_dst.buffer = (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)dst);
    plane_dst.width = width;
    plane_dst.height = height;
    plane_dst.pitch = display_get_pitch_length_in_byte(format, dst_width);
    pdma_config_planes(LV_GPU_HPM_PDMA_ID, &plane_src, &plane_dst, &yuv2rgb_coef);

    output.buffer = (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)dst);

    output.plane[pdma_plane_src].x = 0;
    output.plane[pdma_plane_src].y = 0;
    output.plane[pdma_plane_src].width = width;
    output.plane[pdma_plane_src].height = height;

    output.plane[pdma_plane_dst].x = 0;
    output.plane[pdma_plane_dst].y = 0;
    output.plane[pdma_plane_dst].width = width;
    output.plane[pdma_plane_dst].height = height;

    output.alphablend.src_alpha = opa;
    output.alphablend.dst_alpha = 0xFF - opa;
    output.alphablend.src_alpha_op = display_alpha_op_override;
    output.alphablend.dst_alpha_op = display_alpha_op_override;
    output.alphablend.mode = display_alphablend_mode_plus;

    output.width = width;
    output.height = height;
    output.pitch = display_get_pitch_length_in_byte(format, dst_width);

    pdma_config_output(LV_GPU_HPM_PDMA_ID, &output);
    pdma_start(LV_GPU_HPM_PDMA_ID);

    do {
        stat = pdma_check_status(LV_GPU_HPM_PDMA_ID, &status);
    } while((stat != status_pdma_done) && (stat != status_pdma_error));
    pdma_stop(LV_GPU_HPM_PDMA_ID);
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * height * display_get_pixel_size_in_byte(format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
    }
}

void lv_gpu_hpm_pdma_fill(lv_color_t *dst, lv_coord_t dst_width,
        lv_color_t color, lv_coord_t width, lv_coord_t height, lv_opa_t opa)
{
    uint32_t status;
    hpm_stat_t stat;
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * height * display_get_pixel_size_in_byte(format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }

    stat = pdma_fill_color(LV_GPU_HPM_PDMA_ID,
            (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)dst),
            dst_width, width, height,
            lv_color_to32(color), opa, format, true, &status);

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * height * display_get_pixel_size_in_byte(format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate((uint32_t)aligned_start, aligned_size);
    }
    if (stat != status_success) {
        /* avoid status be optimized */
        while(status);
    }
}

static void _lv_gpu_hpm_pdma_blit_recolor(lv_color_t *dst, lv_coord_t dst_width,
                          const lv_color_t * src, lv_coord_t src_width,
                          lv_coord_t width, lv_coord_t height, lv_opa_t opa,
                          lv_color_t rc, lv_opa_t rc_opa)
{
    uint32_t status;
    hpm_stat_t stat;

    if (enable_colorkey) {
        lv_gpu_hpm_pdma_fill(dst, dst_width, lv_color_black(), width, height, opa);
        return;
    }

    if (opa > LV_OPA_MAX) {
        if (l1c_dc_is_enabled()) {
            uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
            uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * height * display_get_pixel_size_in_byte(format));
            uint32_t aligned_size = aligned_end - aligned_start;
            l1c_dc_writeback(aligned_start, aligned_size);

            aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
            aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * height * display_get_pixel_size_in_byte(format));
            aligned_size = aligned_end - aligned_start;
            l1c_dc_writeback(aligned_start, aligned_size);
        }
        pdma_config_t config;
        pdma_plane_config_t plane_src;
        pdma_plane_config_t plane_dst;
        display_yuv2rgb_coef_t yuv2rgb_coef;
        pdma_output_config_t output;

        pdma_get_default_config(LV_GPU_HPM_PDMA_ID, &config, format);
        pdma_get_default_plane_config(LV_GPU_HPM_PDMA_ID, &plane_src, format);
        pdma_get_default_plane_config(LV_GPU_HPM_PDMA_ID, &plane_dst, format);
        pdma_get_default_yuv2rgb_coef_config(LV_GPU_HPM_PDMA_ID, &yuv2rgb_coef, format);
        pdma_get_default_output_config(LV_GPU_HPM_PDMA_ID, &output, format);

        config.enable_plane = pdma_plane_both;
        if (width <= 16) {
            config.block_size = pdma_blocksize_8x8;
        } else {
            config.block_size = pdma_blocksize_16x16;
        }
        pdma_init(LV_GPU_HPM_PDMA_ID, &config);

        plane_src.buffer = (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)src);
        plane_src.width = width;
        plane_src.height = height;
        plane_src.pitch = display_get_pitch_length_in_byte(format, src_width);

        plane_dst.buffer = (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)dst);
        plane_dst.width = 1;
        plane_dst.height = 1;
        plane_dst.background = lv_color_to32(rc);
        plane_dst.pitch = display_get_pitch_length_in_byte(format, dst_width);
        pdma_config_planes(LV_GPU_HPM_PDMA_ID, &plane_src, &plane_dst, &yuv2rgb_coef);

        output.buffer = (uint32_t)core_local_mem_to_sys_address(0, (uint32_t)dst);

        output.plane[pdma_plane_src].x = 0;
        output.plane[pdma_plane_src].y = 0;
        output.plane[pdma_plane_src].width = width;
        output.plane[pdma_plane_src].height = height;

        output.plane[pdma_plane_dst].x = 0;
        output.plane[pdma_plane_dst].y = 0;
        output.plane[pdma_plane_dst].width = width;
        output.plane[pdma_plane_dst].height = height;

        output.alphablend.src_alpha = opa;
        output.alphablend.dst_alpha = rc_opa;
        output.alphablend.src_alpha_op = display_alpha_op_override;
        output.alphablend.dst_alpha_op = display_alpha_op_override;
        output.alphablend.mode = display_alphablend_mode_plus;

        output.width = width;
        output.height = height;
        output.pitch = display_get_pitch_length_in_byte(format, dst_width);

        pdma_config_output(LV_GPU_HPM_PDMA_ID, &output);
        pdma_start(LV_GPU_HPM_PDMA_ID);

        do {
            stat = pdma_check_status(LV_GPU_HPM_PDMA_ID, &status);
        } while((stat != status_pdma_done) && (stat != status_pdma_error));
        pdma_stop(LV_GPU_HPM_PDMA_ID);
        if (l1c_dc_is_enabled()) {
            uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
            uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * height * display_get_pixel_size_in_byte(format));
            uint32_t aligned_size = aligned_end - aligned_start;
            l1c_dc_invalidate(aligned_start, aligned_size);
        }
    } else {
        lv_color_t * tmpBuf  = (lv_color_t *) lv_mem_buf_get(width * height * sizeof(lv_color_t));
        _lv_gpu_hpm_pdma_blit_recolor(tmpBuf, width, src, src_width, width, height, LV_OPA_COVER, recolor,
                                    recolor_opa);

        _lv_gpu_hpm_pdma_blit(dst, dst_width, tmpBuf, width, width, height, opa);
        lv_mem_buf_release(tmpBuf);
    }
}

void lv_gpu_hpm_pdma_blit(lv_color_t *dst, lv_coord_t dst_width,
                          const lv_color_t * src, lv_coord_t src_width,
                          lv_coord_t width, lv_coord_t height, lv_opa_t opa)
{
    if (enable_recolor) {
        _lv_gpu_hpm_pdma_blit_recolor(dst, dst_width, src, src_width,
                                    width, height, opa, recolor, recolor_opa);
    } else {
        _lv_gpu_hpm_pdma_blit(dst, dst_width, src, src_width, width, height, opa);
    }
}

void lv_gpu_hpm_pdma_enable_color_key(void)
{
    enable_colorkey = true;
}

void lv_gpu_hpm_pdma_disable_color_key(void)
{
    enable_colorkey = false;
}

void lv_gpu_hpm_pdma_enable_recolor(lv_color_t c, lv_opa_t opa)
{
    recolor = c;
    recolor_opa = opa;
    enable_recolor = true;
}

void lv_gpu_hpm_pdma_disable_recolor(void)
{
    enable_recolor = false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif
