/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <board.h>
#include <hpm_lvgl.h>
#include "usbh_core.h"
#include "usbh_hid_lvgl.h"

static void hpm_lvgl_mouse_keyboard_demo(lv_indev_t *indev)
{
    lv_obj_t *scr = lv_scr_act();

    /* Create a label for title */
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "USB Keyboard & Mouse Interaction Test");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    /* Create the textarea */
    lv_obj_t *ta = lv_textarea_create(scr);
    lv_textarea_set_one_line(ta, true);
    lv_obj_set_size(ta, 200, 50);
    lv_obj_align(ta, LV_ALIGN_CENTER, 0, -30);

    /* Setup input group */
    lv_group_t *group = lv_group_create();
    lv_indev_set_group(indev, group);
    lv_group_add_obj(group, ta);
}

static void lvgl_task(void *pvParameters)
{
    (void) pvParameters;
    uint32_t delay;
    lv_indev_t *kb_indev;
    board_init_cap_touch();
    board_init_lcd();
    board_init_usb((USB_Type *)CONFIG_HPM_USBH_BASE);
    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);
    usbh_initialize(0, CONFIG_HPM_USBH_BASE);

    hpm_lvgl_init();
    usbh_hid_lvgl_add_mouse(0);
    kb_indev = usbh_hid_lvgl_add_keyboard();
    hpm_lvgl_mouse_keyboard_demo(kb_indev);

    while (1) {
        delay = lv_timer_handler();
        vTaskDelay(delay);
    }
}

int main(void)
{
    board_init();

    if (xTaskCreate(lvgl_task, "lvgl", 4096, NULL, 5, NULL) != pdPASS) {
        printf("Task creation failed!.\n");
        for (;;) {
        }
    }

    vTaskStartScheduler();
    for (;;) {

    }
    return 0;
}
