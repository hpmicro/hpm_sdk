/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Include
 *---------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tusb.h"

/* This example demonstrate HID Generic raw Input & Output.
 * It will receive data from Host (In endpoint) and echo back (Out endpoint).
 * HID Report descriptor use vendor for usage page (using template TUD_HID_REPORT_DESC_GENERIC_INOUT)
 */

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
    BLINK_MOUNTED     = 1000,
    BLINK_SUSPENDED   = 2500,
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
 * USB HID
 *---------------------------------------------------------------------*/
/* Invoked when received GET_REPORT control request
 * Application must fill buffer report's content and return its length.
 * Return zero will cause the stack to STALL request
 */
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    /* TODO not Implemented */
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

/* Invoked when received SET_REPORT control request or
 * received data on OUT endpoint ( Report ID = 0, Type = 0 )
 */
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void)itf;
    (void)report_type;
    uint8_t echo_buf[bufsize];

    /* echo back anything we received from host */
    memcpy(echo_buf, buffer, bufsize);

    #if (CFG_TUSB_REPORT_ID_COUNT > 0)
        report_id = REPORT_ID_IN;
        tud_hid_report(report_id, echo_buf, bufsize);
    #else
        tud_hid_report(0, echo_buf, bufsize);
    #endif
}

/*---------------------------------------------------------------------*
 * Blinking Task
 *---------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
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

    printf("USB%d Device - HID Generic Inout Demo\r\n", BOARD_DEVICE_RHPORT_NUM);

    tusb_init();

    while (1) {
        tud_task(); /* tinyusb device task */
        led_blinking_task();
    }

    return 0;
}
