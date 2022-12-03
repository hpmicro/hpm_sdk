
/*********************
 *      INCLUDES
 *********************/
#include "board.h"
#include "hpm_l1c_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_dma_drv.h"

#include "hpm_dma_drv.h"

uint16_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) framebuffer[2][BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT];
static lcdc_layer_config_t layer;

#define LV_LCDC_TRANS_BUFFER_ADDR        ((void *)(&framebuffer[0]))


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

    pixel_format = display_pixel_format_rgb565;

    lcdc_init(LCD_CONTROLLER, &config);
    lcdc_get_default_layer_config(LCD_CONTROLLER, &layer, pixel_format, LCD_LAYER_INDEX);

    memset(framebuffer, 0, sizeof(framebuffer));

    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = config.resolution_x;
    layer.height = config.resolution_y;
    layer.buffer = (uint32_t) core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t) LV_LCDC_TRANS_BUFFER_ADDR);
    layer.background.u = 0;

    if (status_success != lcdc_config_layer(LCD_CONTROLLER, LCD_LAYER_INDEX, &layer, true)) {
        while (1) {
        }
    }

    lcdc_turn_on_display(LCD_CONTROLLER);

}
