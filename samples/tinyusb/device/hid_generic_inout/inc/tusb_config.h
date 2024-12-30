/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H
/*---------------------------------------------------------------------*
 * Common Configuration
 *---------------------------------------------------------------------*/

/* defined by CMakeLists.txt */
#ifndef CFG_TUSB_MCU
    #error CFG_TUSB_MCU must be defined
#endif

/* RHPort number used for device can be defined by CMakeLists.txt, default to port 0 */
#ifndef BOARD_DEVICE_RHPORT_NUM
    #define BOARD_DEVICE_RHPORT_NUM     0
#endif

/* RHPort max operational speed can defined by by CMakeLists.txt */
/* Default to Highspeed for MCU with internal HighSpeed PHY (can be port specific), otherwise FullSpeed */
#ifndef BOARD_DEVICE_RHPORT_SPEED
    #define BOARD_DEVICE_RHPORT_SPEED   OPT_MODE_HIGH_SPEED
#endif

/* Device mode with rhport and speed defined by CMakeLists.txt */
#if   BOARD_DEVICE_RHPORT_NUM == 0
    #define CFG_TUSB_RHPORT0_MODE     (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)
#elif BOARD_DEVICE_RHPORT_NUM == 1
    #define CFG_TUSB_RHPORT1_MODE     (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)
#else
    #error "Incorrect RHPort configuration"
#endif

#ifndef CFG_TUSB_OS
    #define CFG_TUSB_OS               OPT_OS_NONE
#endif

/* CFG_TUSB_DEBUG is defined by compiler in DEBUG build */
#define CFG_TUSB_DEBUG           0

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
    #define CFG_TUSB_MEM_SECTION      __attribute__ ((section(".noncacheable.non_init")))
#endif

#ifndef CFG_TUSB_MEM_ALIGN
    #define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

/*---------------------------------------------------------------------*
 * Device Configuration
 *---------------------------------------------------------------------*/
#ifndef CFG_TUD_ENDPOINT0_SIZE
    #define CFG_TUD_ENDPOINT0_SIZE    64
#endif

/*------------- Endpoint Number -------------*/
#ifndef CFG_TUD_EPNUM_HID
    #define CFG_TUD_EPNUM_HID         1
#endif

/*------------- Class -------------*/
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_HID               1
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

/*------------- Report ID -------------*/
#define REPORT_ID_COUNT           2   /* This value must not be larger than 2 in this demo. */

#if REPORT_ID_COUNT == 2
    #define REPORT_ID_OUT           1
    #define REPORT_ID_IN            2
#elif REPORT_ID_COUNT == 1
  #define REPORT_ID               1
  #define REPORT_ID_OUT           REPORT_ID
  #define REPORT_ID_IN            REPORT_ID
#endif

#ifndef CFG_TUSB_REPORT_ID_COUNT
    #define CFG_TUSB_REPORT_ID_COUNT  REPORT_ID_COUNT
#endif

/* HID buffer size Should be sufficient to hold ID (if any) + Data */
#define CFG_TUD_HID_EP_BUFSIZE_HS   1024
#define CFG_TUD_HID_EP_BUFSIZE_FS   64

#define CFG_TUD_HID_EP_INTERVAL_HS  4 /* 2^(4-1) * 125us = 1ms */
#define CFG_TUD_HID_EP_INTERVAL_FS  1 /* 1ms */
#define CFG_TUD_HID_EP_BUFSIZE      CFG_TUD_HID_EP_BUFSIZE_HS
#define CFG_TUD_HID_EP_INTERVAL     CFG_TUD_HID_EP_INTERVAL_HS

#endif /* TUSB_CONFIG_H */