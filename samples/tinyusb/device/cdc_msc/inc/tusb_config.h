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

/*------------- Class -------------*/
#define CFG_TUD_CDC              1
#define CFG_TUD_MSC              1
#define CFG_TUD_HID              0
#define CFG_TUD_MIDI             0
#define CFG_TUD_VENDOR           0

/* CDC FIFO size of TX and RX */
#define CFG_TUD_CDC_RX_BUFSIZE   (TUD_OPT_HIGH_SPEED ? 512 : 64)
#define CFG_TUD_CDC_TX_BUFSIZE   (TUD_OPT_HIGH_SPEED ? 512 : 64)

/* CDC Endpoint transfer buffer size, more is faster */
#define CFG_TUD_CDC_EP_BUFSIZE   (TUD_OPT_HIGH_SPEED ? 512 : 64)

/* MSC Buffer size of Device Mass storage */
#define CFG_TUD_MSC_EP_BUFSIZE   512

#endif /* TUSB_CONFIG_H */
