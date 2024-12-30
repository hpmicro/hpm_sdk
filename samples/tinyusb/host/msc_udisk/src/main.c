/*
 * Copyright (c) 2021 HPMicro
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
 * Macro Const Definitions
 *---------------------------------------------------------------------*/
#define USB_APP_DELAY_INTERVAL (1U)  /* 1 ms */

/*---------------------------------------------------------------------*
 * Function Prototyes
 *---------------------------------------------------------------------*/
void print_greeting(void);
void led_blinking_task(void);
extern void cdc_task(void);
extern void hid_task(void);

/*---------------------------------------------------------------------*
 * Variable Definitions
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
 * Main
 *---------------------------------------------------------------------*/
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
    print_greeting();
    printf("USB%d Host Mass Storage Demo\r\n", BOARD_TUH_RHPORT);

    tusb_init();

    while (1)
    {
      /* tinyusb host task */
      tuh_task();

      led_blinking_task();

  #if CFG_TUH_CDC
      cdc_task();
  #endif

  #if CFG_TUH_HID_KEYBOARD || CFG_TUH_HID_MOUSE
      hid_task();
  #endif
    }

    return 0;
}

/*---------------------------------------------------------------------*
 * USB CDC
 *---------------------------------------------------------------------*/
#if CFG_TUH_CDC
CFG_TUSB_MEM_SECTION static char serial_in_buffer[64] = { 0 };

void tuh_mount_cb(uint8_t dev_addr)
{
  /* application set-up */
  printf("A device with address %d is mounted\r\n", dev_addr);

  tuh_cdc_receive(dev_addr, serial_in_buffer, sizeof(serial_in_buffer), true); /* schedule first transfer */
}

void tuh_umount_cb(uint8_t dev_addr)
{
    /* application tear-down */
    printf("A device with address %d is unmounted \r\n", dev_addr);
}

/* invoked ISR context */
void tuh_cdc_xfer_isr(uint8_t dev_addr, xfer_result_t event, cdc_pipeid_t pipe_id, uint32_t xferred_bytes)
{
    (void) event;
    (void) pipe_id;
    (void) xferred_bytes;

    printf(serial_in_buffer);
    tu_memclr(serial_in_buffer, sizeof(serial_in_buffer));

    tuh_cdc_receive(dev_addr, serial_in_buffer, sizeof(serial_in_buffer), true); /* waiting for next data */
}

void cdc_task(void)
{
}

#endif

/*---------------------------------------------------------------------*
 * USB HID
 *---------------------------------------------------------------------*/
#if CFG_TUH_HID_KEYBOARD

CFG_TUSB_MEM_SECTION static hid_keyboard_report_t usb_keyboard_report;
uint8_t const keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

/* look up new key in previous keys */
static inline bool find_key_in_report(hid_keyboard_report_t const *p_report, uint8_t keycode)
{
  for(uint8_t i=0; i<6; i++)
  {
    if (p_report->keycode[i] == keycode)  return true;
  }

  return false;
}

static inline void process_kbd_report(hid_keyboard_report_t const *p_new_report)
{
  static hid_keyboard_report_t prev_report = { 0, 0, {0} }; /* previous report to check key released */

  /*------------- example code ignore control (non-printable) key affects -------------*/
  for(uint8_t i=0; i<6; i++)
  {
    if ( p_new_report->keycode[i] )
    {
      if ( find_key_in_report(&prev_report, p_new_report->keycode[i]) )
      {
        /* exist in previous report means the current key is holding */
      }else
      {
        /* not existed in previous report means the current key is pressed */
        bool const is_shift =  p_new_report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
        uint8_t ch = keycode2ascii[p_new_report->keycode[i]][is_shift ? 1 : 0];
        putchar(ch);
        if ( ch == '\r' ) putchar('\n'); /* added new line for enter key */

        fflush(stdout); /* flush right away, else nanolib will wait for newline */
      }
    }
    /* TODO example skips key released */
  }

  prev_report = *p_new_report;
}

void tuh_hid_keyboard_mounted_cb(uint8_t dev_addr)
{
  /* application set-up */
  printf("A Keyboard device (address %d) is mounted\r\n", dev_addr);

  tuh_hid_keyboard_get_report(dev_addr, &usb_keyboard_report);
}

void tuh_hid_keyboard_unmounted_cb(uint8_t dev_addr)
{
  /* application tear-down */
  printf("A Keyboard device (address %d) is unmounted\r\n", dev_addr);
}

/* invoked ISR context */
void tuh_hid_keyboard_isr(uint8_t dev_addr, xfer_result_t event)
{
  (void) dev_addr;
  (void) event;
}

#endif

#if CFG_TUH_HID_MOUSE

CFG_TUSB_MEM_SECTION static hid_mouse_report_t usb_mouse_report;

void cursor_movement(int8_t x, int8_t y, int8_t wheel)
{
  /*------------- X -------------*/
  if ( x < 0)
  {
    printf(ANSI_CURSOR_BACKWARD(%d), (-x)); /* move left */
  }else if ( x > 0)
  {
    printf(ANSI_CURSOR_FORWARD(%d), x); /* move right */
  }else { }

  /*------------- Y -------------*/
  if ( y < 0)
  {
    printf(ANSI_CURSOR_UP(%d), (-y)); /* move up */
  }else if ( y > 0)
  {
    printf(ANSI_CURSOR_DOWN(%d), y); /* move down */
  }else { }

  /*------------- wheel -------------*/
  if (wheel < 0)
  {
    printf(ANSI_SCROLL_UP(%d), (-wheel)); /* scroll up */
  }else if (wheel > 0)
  {
    printf(ANSI_SCROLL_DOWN(%d), wheel); /* scroll down */
  }else { }
}

static inline void process_mouse_report(hid_mouse_report_t const * p_report)
{
  static hid_mouse_report_t prev_report = { 0 };

  /*------------- button state  -------------*/
  uint8_t button_changed_mask = p_report->buttons ^ prev_report.buttons;
  if ( button_changed_mask & p_report->buttons)
  {
    printf(" %c%c%c ",
       p_report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-',
       p_report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-',
       p_report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-');
  }

  /*------------- cursor movement -------------*/
  cursor_movement(p_report->x, p_report->y, p_report->wheel);
}


void tuh_hid_mouse_mounted_cb(uint8_t dev_addr)
{
  /* application set-up */
  printf("A Mouse device (address %d) is mounted\r\n", dev_addr);
}

void tuh_hid_mouse_unmounted_cb(uint8_t dev_addr)
{
  /* application tear-down */
  printf("A Mouse device (address %d) is unmounted\r\n", dev_addr);
}

/* invoked ISR context */
void tuh_hid_mouse_isr(uint8_t dev_addr, xfer_result_t event)
{
  (void) dev_addr;
  (void) event;
}
#endif

void hid_task(void)
{
#if (CFG_TUH_HID_KEYBOARD == 1 || CFG_TUH_HID_MOUSE == 1)
  uint8_t const addr = 1;
#endif

#if CFG_TUH_HID_KEYBOARD
  if ( tuh_hid_keyboard_is_mounted(addr) )
  {
    if ( !tuh_hid_keyboard_is_busy(addr) )
    {
      process_kbd_report(&usb_keyboard_report);
      tuh_hid_keyboard_get_report(addr, &usb_keyboard_report);
    }
  }
#endif

#if CFG_TUH_HID_MOUSE
  if ( tuh_hid_mouse_is_mounted(addr) )
  {
    if ( !tuh_hid_mouse_is_busy(addr) )
    {
      process_mouse_report(&usb_mouse_report);
      tuh_hid_mouse_get_report(addr, &usb_mouse_report);
    }
  }
#endif
}

/*---------------------------------------------------------------------*
 * Blinking Task
 *---------------------------------------------------------------------*/
void led_set_blinking_mounted_interval(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

void led_set_blinking_unmounted_interval(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

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
 * Helper Function
 *---------------------------------------------------------------------*/
void print_greeting(void)
{
  char const * const rtos_name[] =
  {
      [OPT_OS_NONE]      = "None",
      [OPT_OS_FREERTOS]  = "FreeRTOS",
  };

  printf("--------------------------------------------------------------------\r\n");
  printf("- Host example\r\n");
  printf("- if you find any bugs or get any questions, feel free to file an\r\n");
  printf("- issue at https:/*github.com/hathach/tinyusb\r\n");
  printf("--------------------------------------------------------------------\r\n\r\n");

  printf("This Host demo is configured to support:\r\n");
  printf("  - RTOS = %s\r\n", rtos_name[CFG_TUSB_OS]);

#if CFG_TUH_CDC
  printf("  - Communication Device Class\r\n");
#endif

#if CFG_TUH_MSC
  printf("  - Mass Storage\r\n");
#endif

#if CFG_TUH_HID_KEYBOARD
  printf("  - HID Keyboard");
#endif

#if CFG_TUH_HID_MOUSE
  printf("  - HID Mouse");
#endif

}
