/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_mipi_csi_drv.h"
#include "hpm_mipi_csi_phy_drv.h"
#include "hpm_pixelmux_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_cam_drv.h"
#include "hpm_camera.h"

#ifndef TEST_CAM
#define TEST_CAM HPM_CAM0
#endif

#define MIPI_CSI_BASE HPM_MIPI_CSI1
#define MIPI_CSI_PHY_BASE HPM_MIPI_CSI_PHY1
#define MIPI_CSI_CLK clock_csi1
#define PIXELMUX_CONFIG_RX_PHY_MODE pixelmux_config_rx_phy1_mode
#define PIXELMUX_CAM_SEL pixelmux_cam0_sel_mipi_csi1

#define LCD BOARD_LCD_BASE
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define CAM_I2C BOARD_CAM_I2C_BASE

#define IMAGE_WIDTH  800
#define IMAGE_HEIGHT 480

#define CONFIG_DATA_TYPE_YUV 0

#if CONFIG_DATA_TYPE_YUV == 0
#define PIXEL_FORMAT display_pixel_format_rgb565
#else
#define PIXEL_FORMAT display_pixel_format_ycbcr422
#endif

#define CAMERA_INTERFACE camera_interface_mipi

uint32_t buffer[IMAGE_WIDTH * IMAGE_HEIGHT] __attribute__((section(".framebuffer")));
camera_context_t camera_context = {0};
camera_config_t camera_config;

/*
 * Note:
 * The sample is active only when mipi or lvds panel used.
 * rgb panel is inactive.
 */
void init_lcd(void)
{
    uint8_t layer_index = 0;
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

    memset(buffer, 0xff, sizeof(buffer));
    lcdc_get_default_config(LCD, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(LCD, &config);

    lcdc_get_default_layer_config(LCD, &layer, display_pixel_format_argb8888, layer_index);

    layer.position_x = (BOARD_LCD_WIDTH - IMAGE_WIDTH) / 2;
    layer.position_y = (BOARD_LCD_HEIGHT - IMAGE_HEIGHT) / 2;
    layer.width = IMAGE_WIDTH;
    layer.height = IMAGE_HEIGHT;
    layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
    if (status_success != lcdc_config_layer(LCD, layer_index, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        }
    }
}

void board_init_mipi_csi(void)
{
    /*
     * Note: mipi csi clk must greater than image sensor pixel clk.
     * greater than about 10%, generally.
     *
     * for example:
     * 800 * 480 *30 = 11.52MHz
     * mipi csi clk = 594 / 43 = 13.81MHz
     */
    clock_add_to_group(MIPI_CSI_CLK, 0);
    clock_set_source_divider(MIPI_CSI_CLK, clk_src_pll4_clk0, 43U);
    uint32_t freq = clock_get_frequency(MIPI_CSI_CLK);
    printf("mipi csi clk freq: %u Hz\n", freq);

    /* cam0 -> mipi_csi2 phy1 */
    pixelmux_cam0_data_source_enable(PIXELMUX_CAM_SEL);
    PIXELMUX_CONFIG_RX_PHY_MODE(pixelmux_rx_phy_mode_mipi);

    mipi_csi_phy_powerdown(MIPI_CSI_BASE);

    /*phy mode: mipi */
    mipi_csi_phy_config_t mipi_phy_cfg;
    mipi_csi_phy_default_config(&mipi_phy_cfg);
    mipi_csi_phy_init(MIPI_CSI_PHY_BASE, &mipi_phy_cfg);
    mipi_csi_phy_poweron(MIPI_CSI_BASE);

    mipi_csi_config_t mipi_cfg;
    mipi_csi_get_defconfig(&mipi_cfg);
    mipi_cfg.lanes = 2;
#if (CONFIG_DATA_TYPE_YUV == 0)
    mipi_cfg.data_type = mipi_csi_data_type_rgb565;
#else
    mipi_cfg.data_type = mipi_csi_data_type_yuv422_8bit;
#endif
    mipi_csi_init(MIPI_CSI_BASE, &mipi_cfg);
}

void init_camera_device(void)
{
    camera_context.i2c_device_addr = CAMERA_DEVICE_ADDR;
    camera_context.ptr = CAM_I2C;
    camera_context.delay_ms = board_delay_ms;
#ifdef BOARD_SUPPORT_CAM_RESET
    camera_context.write_rst = board_write_mipi_csi_cam_rst;
#endif
#ifdef BOARD_SUPPORT_CAM_PWDN
    camera_context.write_pwdn = board_write_cam_pwdn;
#endif

    camera_config.width = IMAGE_WIDTH;
    camera_config.height = IMAGE_HEIGHT;
    camera_config.pixel_format = PIXEL_FORMAT;
    camera_config.interface = CAMERA_INTERFACE;
    camera_device_get_mipi_param(&camera_context, &camera_config);
    if (status_success != camera_device_init(&camera_context, &camera_config)) {
        printf("failed to init camera device\n");
        while (1) {
        }
    }
}

void init_cam(void)
{
    cam_config_t cam_config;
    cam_get_default_config(TEST_CAM, &cam_config, PIXEL_FORMAT);
#if CONFIG_DATA_TYPE_YUV
    cam_config.csc_config.enable = true;
#endif

    cam_config.color_ext = true;
    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.de_active_low = true;
    cam_config.hsync_active_low  = false;
    cam_config.vsync_active_low  = false;
    cam_config.buffer1 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
    cam_config.buffer2 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
    cam_config.color_format = cam_get_pixel_format(PIXEL_FORMAT);
    cam_config.sensor_bitwidth = CAM_SENSOR_BITWIDTH_24BITS; /* For MIPI-CSI2 */
    if (CAM_COLOR_FORMAT_UNSUPPORTED == cam_config.color_format) {
        printf("cam does not support this pixel format\n");
        return;
    }

    cam_init(TEST_CAM, &cam_config);
}

int main(void)
{
    board_init();
    printf("mipi_csi example\r\n");
    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_lcd();
    board_init_mipi_csi_cam_pins();
    board_init_mipi_csi();
    init_camera_device();
    init_lcd();
    init_cam();
    cam_start(TEST_CAM);

    lcdc_turn_on_display(LCD);

    while (1) {
    }
}
