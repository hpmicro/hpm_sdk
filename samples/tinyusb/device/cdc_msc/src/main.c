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
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tusb.h"

/*---------------------------------------------------------------------*
 * Macro Enum Declaration
 *---------------------------------------------------------------------*/
/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

/*---------------------------------------------------------------------*
 * Macro Const Declaration
 *---------------------------------------------------------------------*/
#define USB_APP_DELAY_INTERVAL (1U)  /* 1 ms */

/*---------------------------------------------------------------------*
 * Variable Definition
 *---------------------------------------------------------------------*/

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

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

/*---------------------------------------------------------------------*
 * Device callback
 *---------------------------------------------------------------------*/
/* Invoked when device is mounted */
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

/* Invoked when device is unmounted */
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

/* Invoked when usb bus is suspended
 * remote_wakeup_en : if host allow us  to perform remote wakeup
 * Within 7ms, device must draw an average of current less than 2.5 mA from bus
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

/* Invoked when usb bus is resumed */
void tud_resume_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

/*---------------------------------------------------------------------*
 * USB CDC
 *---------------------------------------------------------------------*/
void cdc_task(void)
{
    /* connected() check for DTR bit
     * Most but not all terminal client set this when making connection
     * if ( tud_cdc_connected() ) {
     */
        /* connected and there are data available */
        if (tud_cdc_available()) {
            /* read datas */
            char buf[512];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            (void) count;

            /* Echo back
            *  Note: Skip echo by commenting out write() and write_flush()
            *  for throughput test e.g
            *  $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
            */
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
    /*}*/
}

/* Invoked when cdc when line state changed e.g connected/disconnected */
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void) itf;
    (void) rts;

    /* TODO set some indicator */
    if (dtr) {
        /* Terminal connected */
    } else {
        /* Terminal disconnected */
    }
}

/* Invoked when CDC interface received data from host */
void tud_cdc_rx_cb(uint8_t itf)
{
    (void) itf;
}

 /* Blinking Task */
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    /* Blink every interval ms */
    if (board_millis() - start_ms < blink_interval_ms) {
        return; /* not enough time */
    }

    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; /* toggle */
}

/* main */
int main(void)
{
    board_init();
    board_init_led_pins();

    if (BOARD_DEVICE_RHPORT_NUM == 0) {
        board_init_usb(HPM_USB0);
#ifdef HPM_USB1
    } else if (BOARD_DEVICE_RHPORT_NUM == 1) {
        board_init_usb(HPM_USB1);
#endif
    } else {
        printf("Don't support HPM_USB%d!\n", BOARD_DEVICE_RHPORT_NUM);
        while (1) {
            ;
        }
    }

    board_timer_create(USB_APP_DELAY_INTERVAL, board_timer_callback);

    printf("USB%d Device - CDC MSC Demo\r\n", BOARD_DEVICE_RHPORT_NUM);

    tusb_init();

    while (1) {
        tud_task(); /* tinyusb device task */
        led_blinking_task();
        cdc_task();
    }

    return 0;
}
