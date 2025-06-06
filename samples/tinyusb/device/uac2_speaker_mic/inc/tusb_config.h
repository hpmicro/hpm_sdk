/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ha Thach (tinyusb.org)
 * Copyright (c) 2020 Jerzy Kasenberg
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

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------*/
/*                    COMMON CONFIGURATION                             */
/*---------------------------------------------------------------------*/
#include "usb_descriptors.h"

/* defined by compiler flags for flexibility */
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

/* RHPort number used for device can be defined by board.mk, default to port 0 */
#ifndef BOARD_DEVICE_RHPORT_NUM
  #define BOARD_DEVICE_RHPORT_NUM     0
#endif
/* RHPort max operational speed can defined by board.mk */
/* Default to Highspeed for MCU with internal HighSpeed PHY (can be port specific), otherwise FullSpeed */
#ifndef BOARD_DEVICE_RHPORT_SPEED
    #define BOARD_DEVICE_RHPORT_SPEED   OPT_MODE_HIGH_SPEED
#endif
#if   BOARD_DEVICE_RHPORT_NUM == 0
  #define CFG_TUSB_RHPORT0_MODE     (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)
#elif BOARD_DEVICE_RHPORT_NUM == 1
  #define CFG_TUSB_RHPORT1_MODE     (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)
#else
  #error "Incorrect RHPort configuration"
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS                 OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
/* Can be set during compilation i.e.: make LOG=<value for CFG_TUSB_DEBUG> BOARD=<bsp> */
/* Keep in mind that enabling logs when data is streaming can disrupt data flow. */
/* It can be very helpful though when audio unit requests are tested/debugged. */
#define CFG_TUSB_DEBUG              0
#endif

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
#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))
#endif

/*---------------------------------------------------------------------*/
/*                     DEVICE CONFIGURATION                            */
/*---------------------------------------------------------------------*/
#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    64
#endif

/*------------- CLASS -------------*/
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_HID               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_AUDIO             1
#define CFG_TUD_VENDOR            0

/*---------------------------------------------------------------------*/
/*              AUDIO CLASS DRIVER CONFIGURATION                       */
/*---------------------------------------------------------------------*/
#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN                                TUD_AUDIO_HEADSET_STEREO_DESC_LEN

/* Audio format type I specifications */
#define CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE                         96000
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX                           2
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX                           2

/* 24bit in 32bit slots */
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX          2
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_TX                  16
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX                  24

/* EP and buffer size - for isochronous EP´s, the buffer and EP size are equal (different sizes would not make sense) */
#define CFG_TUD_AUDIO_ENABLE_EP_IN                                    1
#define CFG_TUD_AUDIO_EP_SZ_IN                                        16 * CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX * CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX                             CFG_TUD_AUDIO_EP_SZ_IN
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ                          CFG_TUD_AUDIO_EP_SZ_IN + 1
/* EP and buffer size - for isochronous EP´s, the buffer and EP size are equal (different sizes would not make sense) */
#define CFG_TUD_AUDIO_ENABLE_EP_OUT               1
#define CFG_TUD_AUDIO_EP_SZ_OUT                   96 * CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX * CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX        CFG_TUD_AUDIO_EP_SZ_OUT /* Maximum EP IN size for all AS alternate settings used */
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ     CFG_TUD_AUDIO_EP_SZ_OUT

/* Number of Standard AS Interface Descriptors (4.9.1) defined per audio function - this is required to be able to remember the current alternate settings of these interfaces - We restrict us here to have a constant number for all audio functions (which means this has to be the maximum number of AS interfaces an audio function has and a second audio function with less AS interfaces just wastes a few bytes) */
#define CFG_TUD_AUDIO_FUNC_1_N_AS_INT             2

/* Size of control request buffer */
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ          64

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
