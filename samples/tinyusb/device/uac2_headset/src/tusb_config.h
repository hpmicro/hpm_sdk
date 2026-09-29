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

/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TUSB_CONFIG_H_
#define TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_descriptors.h"

/*--------------------------------------------------------------------+*/
/* Board Specific Configuration                                       */
/*--------------------------------------------------------------------+*/

/* RHPort number used for device can be defined by board.mk, default to port 0 */
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

/* RHPort max operational speed can defined by board.mk */
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif

/*--------------------------------------------------------------------*/
/* Common Configuration                                               */
/*--------------------------------------------------------------------*/

/* defined by compiler flags for flexibility */
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        1
#endif

/* Enable Device stack */
#define CFG_TUD_ENABLED       1

/* Default is max speed that hardware controller could support with on-chip PHY */
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

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

/*--------------------------------------------------------------------*/
/* DEVICE CONFIGURATION                                               */
/*--------------------------------------------------------------------*/

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

/*--------------------------------------------------------------------*/
/* AUDIO CLASS DRIVER CONFIGURATION                                   */
/*--------------------------------------------------------------------*/

/* Allow volume controlled by on-baord button */
#define CFG_TUD_AUDIO_ENABLE_INTERRUPT_EP                            1

/* How many formats are used, need to adjust USB descriptor if changed */
#define CFG_TUD_AUDIO_FUNC_1_N_FORMATS                               1

/* Audio format type I specifications */
/* 24bit in 32bit slots */
#define CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE                         48000
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX                           2
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX                           2

#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_TX                  24
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX                  24

/* Use 32-bit resolution for Windows UAC1 compatibility with 4-byte subframes. */
#define CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_RESOLUTION_TX                32
#define CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_RESOLUTION_RX                32

#define CFG_TUD_AUDIO_FUNC_1_FRAME_SIZE_TX                           (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX * CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX)
#define CFG_TUD_AUDIO_FUNC_1_FRAME_SIZE_RX                           (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX * CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX)

/* bInterval -> packets/sec: HS 8000 / 2^(bInterval-1), FS 1000 / 2^(bInterval-1) */
#define CFG_TUD_AUDIO_EP_INTERVAL_HS                                 1U
#define CFG_TUD_AUDIO_EP_INTERVAL_FS                                 1U
#define CFG_TUD_AUDIO_HS_MICROFRAMES_PER_PACKET                      (1U << (CFG_TUD_AUDIO_EP_INTERVAL_HS - 1U))
#define CFG_TUD_AUDIO_HS_PACKETS_PER_SEC                             (8000U / CFG_TUD_AUDIO_HS_MICROFRAMES_PER_PACKET)
#define CFG_TUD_AUDIO_FS_FRAMES_PER_PACKET                           (1U << (CFG_TUD_AUDIO_EP_INTERVAL_FS - 1U))
#define CFG_TUD_AUDIO_FS_PACKETS_PER_SEC                             (1000U / CFG_TUD_AUDIO_FS_FRAMES_PER_PACKET)

/* EP and buffer size - for isochronous EP´s, the buffer and EP size are equal (different sizes would not make sense) */
#define CFG_TUD_AUDIO_ENABLE_EP_IN                1

/* UAC1 (Full-Speed) Endpoint size calculation */
#define CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_EP_SZ_IN   TUD_AUDIO_EP_SIZE(false, CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX)

/* UAC2 (High-Speed) Endpoint size calculation */
#define CFG_TUD_AUDIO20_FUNC_1_FORMAT_1_EP_SZ_IN   TUD_AUDIO_EP_SIZE(true, CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX)

/* Maximum EP IN size for all AS alternate settings used */
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX         TU_MAX(CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_EP_SZ_IN, CFG_TUD_AUDIO20_FUNC_1_FORMAT_1_EP_SZ_IN)

/* Tx flow control needs buffer size >= 4* EP size to work correctly */
/* Example write FIFO every 1ms (8 HS frames), so buffer size should be 8 times larger for HS device */
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ      TU_MAX(4 * CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_EP_SZ_IN, 32 * CFG_TUD_AUDIO20_FUNC_1_FORMAT_1_EP_SZ_IN)

/* EP and buffer size - for isochronous EP´s, the buffer and EP size are equal (different sizes would not make sense) */
#define CFG_TUD_AUDIO_ENABLE_EP_OUT               1

/* UAC1 (Full-Speed) Endpoint size calculation */
#define CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_EP_SZ_OUT  TUD_AUDIO_EP_SIZE(false, CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX)

/* UAC2 (High-Speed) Endpoint size calculation */
#define CFG_TUD_AUDIO20_FUNC_1_FORMAT_1_EP_SZ_OUT  TUD_AUDIO_EP_SIZE(true, CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX)

/* Maximum EP OUT size for all AS alternate settings used */
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX        TU_MAX(CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_EP_SZ_OUT, CFG_TUD_AUDIO20_FUNC_1_FORMAT_1_EP_SZ_OUT)

/* Rx flow control needs buffer size >= 4* EP size to work correctly */
/* Example read FIFO every 1ms (8 HS frames), so buffer size should be 8 times larger for HS device */
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ     TU_MAX(4 * CFG_TUD_AUDIO10_FUNC_1_FORMAT_1_EP_SZ_OUT, 32 * CFG_TUD_AUDIO20_FUNC_1_FORMAT_1_EP_SZ_OUT)

#ifdef __cplusplus
}
#endif

#endif /* TUSB_CONFIG_H_ */
