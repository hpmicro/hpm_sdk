/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* -------------------------------------------------------------------- */
/* Common Configuration */
/* -------------------------------------------------------------------- */

/* defined by compiler flags for flexibility */
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        0
#endif

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUH_MEM_SECTION
#define CFG_TUH_MEM_SECTION    __attribute__ ((section(".noncacheable.non_init")))
#endif

#ifndef CFG_TUH_MEM_ALIGN
#define CFG_TUH_MEM_ALIGN     __attribute__ ((aligned(4)))
#endif

/* -------------------------------------------------------------------- */
/* Host Configuration */
/* -------------------------------------------------------------------- */

/* Enable Host stack */
#define CFG_TUH_ENABLED       1

/* #define CFG_TUH_MAX3421       1 */ /* use max3421 as host controller */

#if CFG_TUSB_MCU == OPT_MCU_RP2040
  /* #define CFG_TUH_RPI_PIO_USB   1 */ /* use pio-usb as host controller */

  /* host roothub port is 1 if using either pio-usb or max3421 */
  #if (defined(CFG_TUH_RPI_PIO_USB) && CFG_TUH_RPI_PIO_USB) || (defined(CFG_TUH_MAX3421) && CFG_TUH_MAX3421)
    #define BOARD_TUH_RHPORT      1
  #endif
#endif

/* ------------------------- Board Specific -------------------------- */

/* RHPort number used for host can be defined by board.mk, default to port 0 */
#ifndef BOARD_TUH_RHPORT
#define BOARD_TUH_RHPORT      0
#endif

/* RHPort max operational speed can defined by board.mk */
#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED   OPT_MODE_HIGH_SPEED
#endif

/* Default is max speed that hardware controller could support with on-chip PHY */
#define CFG_TUH_MAX_SPEED     BOARD_TUH_MAX_SPEED

/* -------------------------------------------------------------------- */
/* Driver Configuration */
/* -------------------------------------------------------------------- */

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

#endif /* _TUSB_CONFIG_H_ */