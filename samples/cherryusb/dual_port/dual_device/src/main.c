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
extern void cdc_acm_init(uint8_t busid, uint32_t reg_base);
extern void msc_ram_init(uint8_t busid, uint32_t reg_base);
extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);

int main(void)
{
    uint32_t u = 0;

    board_init();
    board_init_led_pins();
    board_init_usb(HPM_USB0);
    board_init_usb(HPM_USB1);

    intc_set_irq_priority(IRQn_USB0, 2);
    intc_set_irq_priority(IRQn_USB1, 3);

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("Cherryusb dual port device sample. USB0 is VCOM, USB1 is MSC\n");

    cdc_acm_init(0, HPM_USB0_BASE);
    msc_ram_init(1, HPM_USB1_BASE);

    while (u < 2) {
        if (dtr_enable) {
            u++;
            board_delay_ms(50);
            cdc_acm_data_send_with_dtr_test(0);
        }
    }

    while (1) {
        ;
    }
    return 0;
}
