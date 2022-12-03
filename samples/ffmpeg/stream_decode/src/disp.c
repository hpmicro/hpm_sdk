
/*********************
 *      INCLUDES
 *********************/
#include "board.h"
#include "hpm_dma_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_lcdc_drv.h"

#include "hpm_dma_drv.h"

#include "disp.h"
#include "osd.h"
uint16_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) framebuffer[2][BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT];
static lcdc_layer_config_t layer;

#define LV_LCDC_TRANS_BUFFER_ADDR ((void *)(&framebuffer[0]))

#ifndef RUNNING_CORE_INDEX
#define RUNNING_CORE_INDEX HPM_CORE0
#endif

#define LCD_CONTROLLER BOARD_LCD_BASE
#define LCD_LAYER_INDEX (0)

/* Initialize your display and the required peripherals. */
void init_lcd(void)
{
    display_pixel_format_t pixel_format;
    lcdc_config_t config = {0};
    lcdc_get_default_config(LCD_CONTROLLER, &config);

    config.resolution_x = BOARD_LCD_WIDTH;
    config.resolution_y = BOARD_LCD_HEIGHT;

    config.vsync.back_porch_pulse = 23;
    config.vsync.front_porch_pulse = 10;
    config.vsync.pulse_width = 3;
    config.hsync.back_porch_pulse = 46;
    config.hsync.front_porch_pulse = 50;
    config.hsync.pulse_width = 10;
#if (DISPLAY_MODE_422)
    pixel_format = display_pixel_format_ycbcr422;
    printf("pixel format YCbCr422\r\n");
#elif (DISPLAY_MODE_RGB)
    pixel_format = display_pixel_format_rgb565;
    printf("pixel format rgb565\r\n");
#elif (DISPLAY_MODE_YUV)
    pixel_format = display_pixel_format_yuv422;
    printf("pixel format yuv422\r\n");
#else
#error "check DISPLAY_MODE"
#endif
    lcdc_init(LCD_CONTROLLER, &config);
    lcdc_get_default_layer_config(LCD_CONTROLLER, &layer, pixel_format, LCD_LAYER_INDEX);

    memset(framebuffer, 0, sizeof(framebuffer));

    if (!lcd_config_layer2()) {
        printf("config layer2 failed\r\n");
    }
    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = config.resolution_x;
    layer.height = config.resolution_y;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t)LV_LCDC_TRANS_BUFFER_ADDR);
    layer.background.u = 0;
#if DISPLAY_MODE_422
    layer.yuv = display_yuv_mode_422_y1u1y2v1;
    layer.csc_config.yuv2rgb_coef.c0 = 0x14A;
    layer.csc_config.yuv2rgb_coef.uv_offset = 0x180;
    layer.csc_config.yuv2rgb_coef.y_offset = 0x1F0;
    layer.csc_config.yuv2rgb_coef.c1 = 0x198;
    layer.csc_config.yuv2rgb_coef.c2 = 0x730;
    layer.csc_config.yuv2rgb_coef.c3 = 0x79C;
    layer.csc_config.yuv2rgb_coef.c4 = 0x208;
    // layer.alphablend.mode = display_alphablend_mode_src_over;
    layer.alphablend.src_alpha = 0xFF; /* src */
    layer.alphablend.dst_alpha = 0xFF; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.background.u = 0;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    layer.alphablend.src_alpha = 0x00; /* src */
    layer.alphablend.dst_alpha = 0xff; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
#endif
    if (status_success != lcdc_config_layer(LCD_CONTROLLER, LCD_LAYER_INDEX, &layer, true)) {
        while (1) {
        }
    }

    lcdc_turn_on_display(LCD_CONTROLLER);
}