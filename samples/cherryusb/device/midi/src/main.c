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

#define LED_FLASH_PERIOD_IN_MS 286

extern void midi_set_auto_play(bool set);
extern void midi_init(uint8_t busid, uint32_t reg_base);
extern void midi_task_286ms(uint8_t busid);

void timer_cb(void)
{
    midi_task_286ms(0);
}

int main(void)
{
    char selection;

    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    printf("cherry usb midi device sample.\n");

    midi_init(0, CONFIG_HPM_USBD_BASE);
    board_timer_create(LED_FLASH_PERIOD_IN_MS, timer_cb);

    while (1) {
        printf("\r\n***********Select Work Mode**********\r\n");
        printf("1 - Auto Play\r\n");
        printf("2 - Manual Play\r\n");
        printf("\r\nPlease Enter Play Number:\r\n");

        selection = getchar();
        printf("%c\r\n\r\n", selection);

        if (selection == '1') {
            midi_set_auto_play(true);
        } else {
            midi_set_auto_play(false);
        }
    }
    return 0;
}
