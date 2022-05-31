/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

/*---------------------------------------------------------------------*
 * Common Configuration
 *---------------------------------------------------------------------
 */

/* defined by CMakeLists.txt */
#ifndef CFG_TUSB_MCU
	#error CFG_TUSB_MCU must be defined
#endif

/* RHPort number used for host can be defined by CMakeLists.txt, default to port 0 */
#ifndef BOARD_HOST_RHPORT_NUM
    #define BOARD_HOST_RHPORT_NUM     0
#endif

#if   BOARD_HOST_RHPORT_NUM == 0
	#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_HOST | OPT_MODE_HIGH_SPEED)
#elif BOARD_HOST_RHPORT_NUM == 1
	#define CFG_TUSB_RHPORT1_MODE       (OPT_MODE_HOST | OPT_MODE_HIGH_SPEED)
#endif

#ifndef CFG_TUSB_OS
	#define CFG_TUSB_OS                 OPT_OS_NONE
#endif

 #define CFG_TUSB_DEBUG           1

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
	#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
	#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))
#endif

#ifndef CFG_TUH_EP_MAX
	#define CFG_TUH_EP_MAX             8
#endif

/*---------------------------------------------------------------------*
 * Configuration
 *---------------------------------------------------------------------
 */
#define CFG_TUH_HUB                 0
#define CFG_TUH_CDC                 0
#define CFG_TUH_HID_KEYBOARD        0
#define CFG_TUH_HID_MOUSE           0
#define CFG_TUSB_HOST_HID_GENERIC   0
#define CFG_TUH_MSC                 1
#define CFG_TUH_VENDOR              0

#define CFG_TUSB_HOST_DEVICE_MAX    (CFG_TUH_HUB ? 5 : 1) /* normal hub has 4 ports */

#endif /* TUSB_CONFIG_H */
