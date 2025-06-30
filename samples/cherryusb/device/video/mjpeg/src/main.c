/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usb_config.h"
#include "camera.h"

extern void video_init(uint8_t busid, uintptr_t reg_base);
extern void init_mjpeg(void);
extern void init_camera_device(void);
extern void init_cam(void);

int main(void)
{
    board_init();

    printf("cherryusb video mjpeg sample.\n");

    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);
    intc_m_enable_irq_with_priority(TEST_CAM_IRQ, 2);
    intc_m_enable_irq_with_priority(IRQn_JPEG, 3);

    init_mjpeg();

    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_cam_pins();
    init_camera_device();
    init_cam();

    video_init(0, CONFIG_HPM_USBD_BASE);
    while (1) {
        ;
    }
    return 0;
}
