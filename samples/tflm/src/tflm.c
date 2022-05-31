/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_cam_drv.h"
#include "hpm_camera.h"
#include "main_functions.h"
#include "hpm_gptmr_drv.h"
#include "hpm_pdma_drv.h"

#ifndef TEST_CAM
#define TEST_CAM HPM_CAM0
#endif

#define LCD BOARD_LCD_BASE
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define CAM_I2C BOARD_CAM_I2C_BASE

#define _CONCAT3(x, y, z) x ## y ## z
#define CONCAT3(x, y, z) _CONCAT3(x, y, z)

#if (CAMERA_MAX_IMAGE_OUTPUT_WIDTH > BOARD_LCD_WIDTH)
#define IMAGE_WIDTH  BOARD_LCD_WIDTH
#else
#define IMAGE_WIDTH  CAMERA_MAX_IMAGE_OUTPUT_WIDTH
#endif

#if (CAMERA_MAX_IMAGE_OUTPUT_HEIGHT > BOARD_LCD_HEIGHT)
#define IMAGE_HEIGHT BOARD_LCD_HEIGHT
#else
#define IMAGE_HEIGHT CAMERA_MAX_IMAGE_OUTPUT_HEIGHT
#endif

#define PIXEL_FORMAT display_pixel_format_rgb565
#define CAMERA_INTERFACE camera_interface_dvp

#define COLOR_SIZE 16
typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

#define IMAGE_HIGH_OFFSET 100


#define BOARD_TFLM_TMR_1MS                       BOARD_TMR_1MS
#define BOARD_TFLM_TMR_CH                        BOARD_TMR_1MS_CH
#define BOARD_TFLM_TMR_CMP                       BOARD_TMR_1MS_CMP
#define BOARD_TFLM_TMR_IRQ                       BOARD_TMR_1MS_IRQ
#define BOARD_TFLM_TMR_RELOAD                    BOARD_TMR_1MS_RELOAD

color_t buffer[IMAGE_WIDTH*(IMAGE_HEIGHT+IMAGE_HIGH_OFFSET)] __attribute__((section(".framebuffer")));
uint32_t tf_run_inference;
void init_lcd(void)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

    lcdc_get_default_config(LCD, &config);

    config.resolution_x = LCD_WIDTH;
    config.resolution_y = LCD_HEIGHT;

    lcdc_init(LCD, &config);

    lcdc_get_default_layer_config(LCD, &layer, PIXEL_FORMAT, 0);

    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = IMAGE_WIDTH;
    layer.height = IMAGE_HEIGHT;

    layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&buffer[IMAGE_HIGH_OFFSET * IMAGE_WIDTH]);
    layer.alphablend.src_alpha = 0xF4; /* src */
    layer.alphablend.dst_alpha = 0xF0; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_override;
    layer.alphablend.dst_alpha_op = display_alpha_op_override;
    layer.background.u= 0xFFFF0000;
    layer.alphablend.mode = display_alphablend_mode_xor;


    if (status_success != lcdc_config_layer(LCD, 0, &layer, true)) {
        printf("failed to configure layer\n");
        while(1);
    }
}

void init_camera_device(void)
{
    camera_context_t camera_context = {NULL, NULL, NULL, NULL};
    camera_config_t camera_config;

    camera_context.ptr = CAM_I2C;
    camera_context.delay_ms = board_delay_ms;
#ifdef BOARD_SUPPORT_CAM_RESET
    camera_context.write_rst = board_write_cam_rst;
#endif
#ifdef BOARD_SUPPORT_CAM_PWDN
    camera_context.write_pwdn = board_write_cam_pwdn;
#endif

    camera_config.width = IMAGE_WIDTH;
    camera_config.height = IMAGE_HEIGHT;
    camera_config.pixel_format = PIXEL_FORMAT;
    camera_config.interface = CAMERA_INTERFACE;

    if (status_success != camera_device_init(&camera_context, &camera_config)) {
        printf("failed to init camera device\n");
        while(1);
    }
}

void init_cam(void)
{
    cam_config_t cam_config;

    cam_get_default_config(TEST_CAM, &cam_config, PIXEL_FORMAT);

    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low = true;
    cam_config.buffer1 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
    if (PIXEL_FORMAT == display_pixel_format_rgb565) {
        cam_config.color_format = CAM_COLOR_FORMAT_YUV444;
    }
    cam_init(TEST_CAM, &cam_config);
}
void writefount2screen(uint16_t or_x, uint16_t or_y, uint16_t x_end, uint16_t y_end, uint8_t assic_id, uint16_t colour,
 uint8_t clearflag, uint8_t *str_fount, uint32_t screen_addr, uint16_t fount_size)
{
	uint8_t *strdisp;
	uint16_t x, y;
	uint8_t bit;
	uint8_t temp1;
	strdisp = (uint8_t *)screen_addr;
	str_fount += fount_size * (assic_id - 0x20);/*get end encode*/
	bit = 0;
	for (y = or_y; y <= y_end; y++) {
		for (x = or_x; x <= x_end; x++) {
			if (clearflag == true) {
                *(strdisp+y*(IMAGE_WIDTH * 2) + 2 * x) = colour&0x00ff;
                *(strdisp+y*(IMAGE_WIDTH * 2) + 2 * x + 1) = colour>>8;
			} else {
				temp1 = (*str_fount)>>bit;
				if ((temp1&0x01) == 0x01) {
                    *(strdisp+y*(IMAGE_WIDTH * 2) + 2*x) = colour&0x00ff;
                    *(strdisp+y*(IMAGE_WIDTH * 2) + 2*x+1) = colour>>8;
				} else {
                    *(strdisp+y*(IMAGE_WIDTH * 2) + 2*x) = 0;
					*(strdisp+y*(IMAGE_WIDTH * 2) + 2*x + 1) = 0;
                }
				bit++;
				if (bit == 8) {
					bit = 0;
					str_fount += 1;
				}
			}
		}
	}
}

char string2fount(uint16_t line, uint16_t column, uint8_t *string, uint8_t string_num, uint16_t colour,
 uint8_t *str_fount, uint32_t screen_addr, uint8_t fount_width, uint8_t fount_height)
{
	uint8_t i = 0, j = 0, numtemp = 0;
	uint16_t or_x, or_y, x_end, y_end;
    uint16_t fount_stroage_size;
	or_x = column * fount_width;
	or_y = line * fount_height;
	x_end = or_x + fount_width - 1;
	y_end = or_y + fount_height - 1;
    fount_stroage_size = fount_width * fount_height / 8;
	for (numtemp = 0; numtemp < string_num; numtemp++) {
        if ((*(string + numtemp) != 10) && (*(string + numtemp) != 0)) {/*enter or end*/
            if (*(string+numtemp) != 8) {/*delete*/
                writefount2screen(or_x+fount_width*i, or_y+fount_height*j, x_end+fount_width*i, y_end+fount_height*j,
                 *(string+numtemp), colour, false, str_fount, screen_addr, fount_stroage_size);
            } else {
                writefount2screen(or_x+fount_width*i, or_y+fount_height*j, x_end+fount_width*i, y_end+fount_height*j,
                 *(string+numtemp), colour, true, str_fount, screen_addr, fount_stroage_size);
            }
        } else if (*(string + numtemp) == 10) {
            i = 19;/* jump next line */
        } else if (*(string + numtemp) == 0) {
            return true;
        }
        i++;
        if (i * fount_width == IMAGE_WIDTH) {
            j++;
            i = 0;
        }
	}
	return true;
}

void isr_gptmr(void)
{
    volatile uint32_t s = BOARD_TFLM_TMR_1MS->SR;
    BOARD_TFLM_TMR_1MS->SR = s;
    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_TFLM_TMR_CH, BOARD_TFLM_TMR_CMP)) {
        tf_run_inference++;
    }

}
SDK_DECLARE_EXT_ISR_M(BOARD_TFLM_TMR_IRQ, isr_gptmr)

/*Timer init 1ms_isr*/
static void timer_init(void)
{
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(BOARD_TFLM_TMR_1MS, &config);
    config.cmp[0] = BOARD_TFLM_TMR_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_TFLM_TMR_RELOAD+1;

    gptmr_enable_irq(BOARD_TFLM_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_TFLM_TMR_CH, BOARD_TFLM_TMR_CMP));
    gptmr_channel_config(BOARD_TFLM_TMR_1MS, BOARD_TFLM_TMR_CH, &config, true);
}

extern const unsigned char nAsciiDot24x48[];
extern int8_t person_score;
int main(void)
{
    uint32_t x, y;
    char fount_display_buf[50];
    uint32_t screen_addr;
    uint16_t *str_screen;
    board_init();
    timer_init();
    setup();
    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_lcd();
    board_init_cam_pins();
    init_camera_device();
    init_lcd();
    init_cam();
    cam_start(TEST_CAM);
    lcdc_turn_on_display(LCD);
    screen_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&buffer[IMAGE_HEIGHT * IMAGE_WIDTH]);
    str_screen = (uint16_t *)screen_addr;
    for (y = 0; y < IMAGE_HIGH_OFFSET; y++) {
        for (x = 0; x < IMAGE_WIDTH; x++) {
            str_screen[y*IMAGE_WIDTH+x] = 0;
        }
    }
    while (1) {
        sprintf(fount_display_buf, "    Inference:%03d ", tf_run_inference);
        string2fount(1, 1, (uint8_t *)fount_display_buf, sizeof(fount_display_buf), 0xff00, (uint8_t *)nAsciiDot24x48, screen_addr, 24, 48);
        if (person_score > -10) {
            sprintf(fount_display_buf, "    Person:%03d    ", abs(person_score));
            string2fount(0, 1, (uint8_t *)fount_display_buf, sizeof(fount_display_buf), 0x1f, (uint8_t *)nAsciiDot24x48, screen_addr, 24, 48);
        } else {
            sprintf(fount_display_buf, "    No Person:%03d ", -person_score);
            string2fount(0, 1, (uint8_t *)fount_display_buf, sizeof(fount_display_buf), 0xf450, (uint8_t *)nAsciiDot24x48, screen_addr, 24, 48);
        }
        tf_run_inference = 0;
        loop();
    }
}
