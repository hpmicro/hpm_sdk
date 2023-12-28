/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "usb_config.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern volatile bool dtr_enable;
extern void cdc_acm_init(void);
extern void cdc_acm_data_send_with_dtr_test(void);

int main(void)
{
    uint32_t u = 0;

    board_init();
    board_init_led_pins();
    board_init_usb_pins();

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb cdc_acm device sample.\n");

    cdc_acm_init();
    while (u < 2) {
        if (dtr_enable) {
            u++;
            board_delay_ms(50);
            cdc_acm_data_send_with_dtr_test();
        }
    }

    while (1) {
        ;
    }
    return 0;
}
