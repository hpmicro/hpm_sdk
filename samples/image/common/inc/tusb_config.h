/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/*--------------------------------------------------------------------*
 * Board Specific Configuration
 *--------------------------------------------------------------------*/

/* RHPort number used for host can be defined by CMakeLists.txt, default to port 0 */
#ifndef BOARD_TUH_RHPORT
#define BOARD_TUH_RHPORT      0
#endif

/* RHPort max operational speed can defined by CMakeLists.txt */
#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED   OPT_MODE_HIGH_SPEED
#endif

/*---------------------------------------------------------------------*
 * COMMON CONFIGURATION
 *---------------------------------------------------------------------*/

/* defined by CMakeLists.txt */
#ifndef CFG_TUSB_MCU
	#error CFG_TUSB_MCU must be defined
#endif

#if   BOARD_TUH_RHPORT == 0
	#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_HOST | BOARD_TUH_MAX_SPEED)
#elif BOARD_TUH_RHPORT == 1
	#define CFG_TUSB_RHPORT1_MODE       (OPT_MODE_HOST | BOARD_TUH_MAX_SPEED)
#endif

#ifndef CFG_TUSB_OS
	#define CFG_TUSB_OS                 OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        1
#endif

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
	#define CFG_TUSB_MEM_SECTION    __attribute__ ((section(".noncacheable.non_init")))
#endif

#ifndef CFG_TUSB_MEM_ALIGN
	#define CFG_TUSB_MEM_ALIGN      __attribute__ ((aligned(4)))
#endif

#ifndef CFG_TUH_EP_MAX
	#define CFG_TUH_EP_MAX             8
#endif

/*---------------------------------------------------------------------*
 * CONFIGURATION
 *---------------------------------------------------------------------*/

/* Size of buffer to hold descriptors and other data used for enumeration */
#define CFG_TUH_ENUMERATION_BUFSIZE 512

#define CFG_TUH_HUB                 0
#define CFG_TUH_CDC                 0
#define CFG_TUH_HID                 0
#define CFG_TUH_MSC                 1
#define CFG_TUH_VENDOR              0

/* max device support (excluding hub device) */
#define CFG_TUH_DEVICE_MAX          (CFG_TUH_HUB ? 4 : 1) /* hub typically has 4 ports */


#ifdef __cplusplus
 }
#endif

#endif /* TUSB_CONFIG_H */
