/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tusb.h"

/*---------------------------------------------------------------------*
 * Macro Const Definitions
 *---------------------------------------------------------------------*/
#define USB_APP_DELAY_INTERVAL (1U)  /* 1 ms */
void led_blinking_task(void);

extern void hid_app_task(void);

/*---------------------------------------------------------------------*
 * Variable Definitions
 *---------------------------------------------------------------------*/
static volatile uint64_t millis_ticks = 0;

/*---------------------------------------------------------------------*
 * Board timer callback
 *---------------------------------------------------------------------*/

void board_timer_callback(void)
{
    millis_ticks++;
}

uint64_t board_millis(void)
{
    return millis_ticks;
}

int main(void)
{
    board_init();
    board_init_led_pins();
    if (BOARD_TUH_RHPORT == 0) {
        board_init_usb(HPM_USB0);
#ifdef HPM_USB1
    } else if (BOARD_TUH_RHPORT == 1) {
        board_init_usb(HPM_USB1);
#endif
    } else {
        printf("Don't support HPM_USB%d!\n", BOARD_TUH_RHPORT);
        while (1) {
            ;
        }
    }
    board_timer_create(USB_APP_DELAY_INTERVAL, board_timer_callback);
    printf("TinyUSB Host HID Example\r\n");

    tusb_init();

    while (1)
    {
        /* tinyusb host task */
        tuh_task();
        led_blinking_task();

    #if CFG_TUH_HID
        hid_app_task();
    #endif
    }

    return 0;
}

/*---------------------------------------------------------------------*
 * Blinking Task
 *---------------------------------------------------------------------*/
void led_blinking_task(void)
{
  const uint32_t interval_ms = 1000;
  static uint32_t start_ms = 0;
  static bool led_state = false;

  /* Blink every interval ms */
  if ( board_millis() - start_ms < interval_ms) {
    return; /* not enough time */
  }
  start_ms += interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; /* toggle */
}
