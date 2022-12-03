/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MP4_H_
#define _MP4_H_
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stdbool.h"

#define READ_BUF_SIZE   (512 * 1024)

/*******************************************************************************
 * Definitions
 ******************************************************************************/

// Set to save decoded audio data into .wav file
#define MP4_WAV_ENABLE      0

typedef enum _ff_time_type
{
    kFfTimeType_Start       = 0U,
    kFfTimeType_ReadFrame   = 1U,
    kFfTimeType_DecodeAudio = 2U,
    kFfTimeType_DecodeVideo = 3U,
    kFfTimeType_TransStart,
    kFfTimeType_TransEnd,
    kFfTimeType_Pdma,
    kFfTimeType_flush,

} ff_time_type_t;
typedef struct _ff_measure_context
{
    uint64_t readFrame_ns;
    uint64_t decodeAudio_ns;
    uint64_t decodeVideo_ns;
    uint64_t decodeVideo_TransStart;
    uint64_t decodeVideo_TransEnd;
    uint64_t decodeVideo_Pdma;
    uint64_t decodeVideo_flush;
    bool isAudioStream;
} ff_measure_context_t;


typedef struct _ff_measure_sum
{
    uint64_t readFrame_ns;
    uint64_t decodeAudio_ns;
    uint64_t decodeVideo_ns;
    uint64_t decodeVideo_TransStart;
    uint64_t decodeVideo_TransEnd;
    uint64_t decodeVideo_Pdma;
    uint64_t decodeVideo_flush;
} ff_measure_sum_t;

// Set to measure FFmpeg decode time for each frame
#define MP4_FF_TIME_ENABLE  1

typedef struct _sai_measure_context
{
    uint32_t transIndex;
    uint32_t costBytes;
    uint64_t costTimeSai_ns;
} sai_measure_context_t;

// Set to measure SAI transfer time for cached frames
// !!!Note: it can NOT be enabled along with MP4_FF_TIME_ENABLE
#define MP4_SAI_TIME_ENABLE 0
// The detected error time in ns for one frame
#define AUDIO_FRAME_ERR_NS  1000000

typedef struct _lcd_measure_context
{
    uint64_t costTimePxp_ns;
    uint64_t costTimeLcd_ns;
} lcd_measure_context_t;

// Set to measure LCD display time for each frame
// !!!Note: it can be enabled along with MP4_FF_TIME_ENABLE
#define MP4_LCD_TIME_ENABLE 0
#define MP4_LCD_DISP_OFF    0

////////////////////////////////////////////////////////////////////////////////

// Set audio frame size according to source media
#define AUDIO_FRAME_SIZE   0x400
// Set audio cache frames, it is important for FFmpeg decode
#define AUDIO_CACHE_FRAMES 3
// Set audio buffer queue, it is important for SAI DMA transfer
#define AUDIO_BUFFER_QUEUE 3

// Set SAI configurations for audio
#define AUDIO_CONV_WIDTH   kSAI_WordWidth16bits

typedef struct _audio_sai_cfg
{
    bool isSaiConfigured;
    uint32_t sampleRate_Hz;
    uint32_t sampleWidth_bit;
    uint32_t sampleChannel;
} audio_sai_cfg_t;

////////////////////////////////////////////////////////////////////////////////

// Set PXP conversation method for video
#define VIDEO_PXP_CONV_BLOCKING       1
#define VIDEO_PXP_CONV_WAITING        1
// Set LCD display method for video
#define VIDEO_LCD_DISP_BLOCKING       1
#define VIDEO_LCD_DISP_WAITING        0

typedef struct _video_lcd_cfg
{
    bool isLcdConfigured;
    uint32_t srcWidth;
    uint32_t srcHeight;
} video_lcd_cfg_t;

// Set LCD resolution
#define VIDEO_LCD_RESOLUTION_HVGA272  1  // For 480*272 LCD
#define VIDEO_LCD_RESOLUTION_SVGA600  0  // For 800*600 LCD
#define VIDEO_LCD_RESOLUTION_WXGA800  0  // For 1280*800 LCD

// Set PXP converted pixel format
#define VIDEO_PIXEL_FMT_RGB888        0
#define VIDEO_PIXEL_FMT_RGB565        1

// Set LCD refresh frequency
#define VIDEO_LCD_REFRESH_FREG_60Hz   0
#define VIDEO_LCD_REFRESH_FREG_30Hz   0
#define VIDEO_LCD_REFRESH_FREG_25Hz   1

#endif