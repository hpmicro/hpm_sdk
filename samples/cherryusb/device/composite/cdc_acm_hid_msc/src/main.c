/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "usb_config.h"

#define LED_FLASH_PERIOD_IN_MS 300
#define USB_BUS_ID 0

extern volatile bool dtr_enable;
extern void cdc_acm_hid_msc_descriptor_init(uint8_t busid, uint32_t reg_base);
extern void hid_mouse_test(uint8_t busid);
extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);

int main(void)
{
    uint32_t u = 0;

    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb composite cdc_acm_hid_msc sample.\n");

    cdc_acm_hid_msc_descriptor_init(USB_BUS_ID, CONFIG_HPM_USBD_BASE);

    while (u < 2) {
        hid_mouse_test(USB_BUS_ID);
        if (dtr_enable) {
            u++;
            board_delay_ms(50);
            cdc_acm_data_send_with_dtr_test(USB_BUS_ID);
        }
    }

    while (1) {
        hid_mouse_test(USB_BUS_ID);
    }

    return 0;
}
