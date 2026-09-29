/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_audio.h"
#include "board.h"
#include "hpm_i2s_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_codec_common.h"
#include "audio_v2_headset.h"

/* Codec I2C control handle, initialized once in headset_init_i2s_codec() */
static codec_control_t s_codec_control;
static bool s_i2s_invert_fclk_out;

#define EP_INTERVAL_HS 0x01
#define EP_INTERVAL_FS 0x01
#define FEEDBACK_ENDP_PACKET_SIZE_HS 0x04
#define FEEDBACK_ENDP_PACKET_SIZE_FS 0x03

/* bInterval → packets/sec: 8000 / 2^(bInterval-1) */
#define HEADSET_HS_MICROFRAMES_PER_PACKET (1U << (EP_INTERVAL_HS - 1U))
#define HEADSET_HS_PACKETS_PER_SEC        (8000U / HEADSET_HS_MICROFRAMES_PER_PACKET)
#define HEADSET_FS_FRAMES_PER_PACKET      (1U << (EP_INTERVAL_FS - 1U))
#define HEADSET_FS_PACKETS_PER_SEC        (1000U / HEADSET_FS_FRAMES_PER_PACKET)

#define AUDIO_VERSION 0x0200

#define AUDIO_OUT_EP          0x02
#define AUDIO_OUT_FEEDBACK_EP 0x82
#define AUDIO_IN_EP           0x81

/* Speaker and microphone share the codec clock source. */
#define SPK_IT_ID      0x01
#define SPK_FU_ID      0x02
#define SPK_OT_ID      0x03
#define AUDIO_CLOCK_ID 0x04
#define MIC_IT_ID      0x11
#define MIC_FU_ID      0x12
#define MIC_OT_ID      0x13

#define HEADSET_MAX_SAMPLE_FREQ_HS 192000
#define HEADSET_MAX_SAMPLE_FREQ_FS 96000
#define HEADSET_SLOT_BYTE_SIZE 4
#define HEADSET_AUDIO_DEPTH    24

#define BMCONTROL (AUDIO_V2_CONTROL_MUTE | AUDIO_V2_CONTROL_VOLUME)

#define IN_CHANNEL_NUM 2
#if IN_CHANNEL_NUM == 1
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x00000001
#elif IN_CHANNEL_NUM == 2
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x00000003
#elif IN_CHANNEL_NUM == 3
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x00000007
#elif IN_CHANNEL_NUM == 4
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x0000000f
#elif IN_CHANNEL_NUM == 5
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x0000001f
#elif IN_CHANNEL_NUM == 6
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x0000003F
#elif IN_CHANNEL_NUM == 7
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x0000007f
#elif IN_CHANNEL_NUM == 8
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x000000ff
#endif

#define OUT_CHANNEL_NUM 2
#if OUT_CHANNEL_NUM == 1
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x00000000
#elif OUT_CHANNEL_NUM == 2
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x00000003
#elif OUT_CHANNEL_NUM == 3
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x00000007
#elif OUT_CHANNEL_NUM == 4
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000000f
#elif OUT_CHANNEL_NUM == 5
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000001f
#elif OUT_CHANNEL_NUM == 6
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000003F
#elif OUT_CHANNEL_NUM == 7
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000007f
#elif OUT_CHANNEL_NUM == 8
#define OUTPUT_CTRL DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x000000ff
#endif

/* Buffer geometry */
#define AUDIO_IN_FRAME_SIZE   (IN_CHANNEL_NUM * HEADSET_SLOT_BYTE_SIZE)
#define AUDIO_OUT_FRAME_SIZE  (OUT_CHANNEL_NUM * HEADSET_SLOT_BYTE_SIZE)

/* I2S DMA slot sizes: 1 audio frame less than descriptor packet */
#define AUDIO_IN_SLOT_HS      ((uint32_t)((HEADSET_MAX_SAMPLE_FREQ_HS * AUDIO_IN_FRAME_SIZE) / HEADSET_HS_PACKETS_PER_SEC))
#define AUDIO_IN_SLOT_FS      ((uint32_t)((HEADSET_MAX_SAMPLE_FREQ_FS * AUDIO_IN_FRAME_SIZE) / HEADSET_FS_PACKETS_PER_SEC))
#define AUDIO_IN_SLOT_MAX     ((AUDIO_IN_SLOT_HS) > (AUDIO_IN_SLOT_FS) ? (AUDIO_IN_SLOT_HS) : (AUDIO_IN_SLOT_FS))

#define AUDIO_OUT_SLOT_HS     ((uint32_t)((HEADSET_MAX_SAMPLE_FREQ_HS * AUDIO_OUT_FRAME_SIZE) / HEADSET_HS_PACKETS_PER_SEC))
#define AUDIO_OUT_SLOT_FS     ((uint32_t)((HEADSET_MAX_SAMPLE_FREQ_FS * AUDIO_OUT_FRAME_SIZE) / HEADSET_FS_PACKETS_PER_SEC))
#define AUDIO_OUT_SLOT_MAX    ((AUDIO_OUT_SLOT_HS) > (AUDIO_OUT_SLOT_FS) ? (AUDIO_OUT_SLOT_HS) : (AUDIO_OUT_SLOT_FS))

/* USB descriptor packet sizes: slot + 1 extra audio frame */
#define AUDIO_IN_PACKET_HS    (AUDIO_IN_SLOT_HS + AUDIO_IN_FRAME_SIZE)
#define AUDIO_IN_PACKET_FS    (AUDIO_IN_SLOT_FS + AUDIO_IN_FRAME_SIZE)
#define AUDIO_OUT_PACKET_HS   (AUDIO_OUT_SLOT_HS + AUDIO_OUT_FRAME_SIZE)
#define AUDIO_OUT_PACKET_FS   (AUDIO_OUT_SLOT_FS + AUDIO_OUT_FRAME_SIZE)

/* Speaker OUT buffer: 2D slot-based, each slot sized to max USB packet */
#define AUDIO_OUT_PACKET_MAX  ((AUDIO_OUT_PACKET_HS) > (AUDIO_OUT_PACKET_FS) ? (AUDIO_OUT_PACKET_HS) : (AUDIO_OUT_PACKET_FS))

#define AUDIO_BUFFER_COUNT 32

/* Mic IN buffer: 1D byte-level circular buffer */
#define AUDIO_IN_BUF_TOTAL    (AUDIO_BUFFER_COUNT * AUDIO_IN_SLOT_MAX)
#define AUDIO_ADAPTIVE_FULL_THRESHOLD   (AUDIO_IN_BUF_TOTAL * 5 / 8)   /* >20/32: send one extra frame */
#define AUDIO_ADAPTIVE_EMPTY_THRESHOLD  (AUDIO_IN_BUF_TOTAL * 3 / 8)   /* <12/32: send one fewer frame */

/* Speaker OUT buffer: 1D byte-level circular buffer */
#define AUDIO_OUT_BUF_TOTAL   (AUDIO_BUFFER_COUNT * AUDIO_OUT_SLOT_MAX)
#define AUDIO_OUT_BUF_CENTER  (AUDIO_OUT_BUF_TOTAL / 2u)
#define AUDIO_FEEDBACK_ADJUST_MAX_STEP  8    /* Max ±8 steps per adjustment */
#define AUDIO_FEEDBACK_MARGIN  (AUDIO_OUT_SLOT_MAX)

/*
 * Audio Control (AC) descriptor size:
 *   Header + shared clock + SPK_IT + SPK_FU(2ch) + SPK_OT + MIC_IT + MIC_FU(2ch) + MIC_OT
 */
#define AUDIO_AC_SIZ (AUDIO_V2_SIZEOF_AC_HEADER_DESC +                        \
                      AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM) +  \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

/*
 * Total config descriptor size:
 *   Config(9) + IAD(26) + CS_AC(without header) + AS(spk,55) + AS(mic,55)
 *   Note: AUDIO_V2_AC_DESCRIPTOR_LEN already includes the CS AC header (9 bytes),
 *   so subtract it from AUDIO_AC_SIZ to avoid double-counting.
 */
#define USB_AUDIO_CONFIG_DESC_SIZ (9 +                                                          \
                                   AUDIO_V2_AC_DESCRIPTOR_LEN +                                 \
                                   (AUDIO_AC_SIZ - AUDIO_V2_SIZEOF_AC_HEADER_DESC) +            \
                                   AUDIO_V2_AS_FEEDBACK_DESCRIPTOR_LEN +                        \
                                   AUDIO_V2_AS_DESCRIPTOR_LEN)

/*
 * Descriptor arrays
 */
static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    /* Shared codec clock source for speaker and microphone. */
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_CLOCK_ID, 0x03, 0x07),
    /* Speaker path: IT(0x01) -> FU(0x02) -> OT(0x03, HEADPHONES) */
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(SPK_IT_ID, AUDIO_TERMINAL_STREAMING, AUDIO_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(SPK_FU_ID, SPK_IT_ID, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(SPK_OT_ID, AUDIO_OUTTERM_HEADPHONES, SPK_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    /* Microphone path: IT(0x11) -> FU(0x12) -> OT(0x13, USB_STREAMING) */
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(MIC_IT_ID, AUDIO_INTERM_MIC, AUDIO_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(MIC_FU_ID, MIC_IT_ID, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(MIC_OT_ID, AUDIO_TERMINAL_STREAMING, MIC_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    /* Audio Streaming: speaker (interface 1, feedback) and mic (interface 2) */
    AUDIO_V2_AS_FEEDBACK_DESCRIPTOR_INIT(0x01, SPK_IT_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_OUT_EP, AUDIO_OUT_PACKET_HS, EP_INTERVAL_HS, AUDIO_OUT_FEEDBACK_EP),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, MIC_OT_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_HS, EP_INTERVAL_HS),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    /* Shared codec clock source for speaker and microphone. */
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_CLOCK_ID, 0x03, 0x07),
    /* Speaker path: IT(0x01) -> FU(0x02) -> OT(0x03, HEADPHONES) */
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(SPK_IT_ID, AUDIO_TERMINAL_STREAMING, AUDIO_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(SPK_FU_ID, SPK_IT_ID, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(SPK_OT_ID, AUDIO_OUTTERM_HEADPHONES, SPK_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    /* Microphone path: IT(0x11) -> FU(0x12) -> OT(0x13, USB_STREAMING) */
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(MIC_IT_ID, AUDIO_INTERM_MIC, AUDIO_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(MIC_FU_ID, MIC_IT_ID, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(MIC_OT_ID, AUDIO_TERMINAL_STREAMING, MIC_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    /* Audio Streaming: speaker (interface 1, feedback) and mic (interface 2) */
    AUDIO_V2_AS_FEEDBACK_DESCRIPTOR_INIT(0x01, SPK_IT_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_OUT_EP, AUDIO_OUT_PACKET_FS, EP_INTERVAL_FS, AUDIO_OUT_FEEDBACK_EP),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, MIC_OT_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(SPK_IT_ID, AUDIO_TERMINAL_STREAMING, AUDIO_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(SPK_FU_ID, SPK_IT_ID, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(SPK_OT_ID, AUDIO_OUTTERM_HEADPHONES, SPK_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(MIC_IT_ID, AUDIO_INTERM_MIC, AUDIO_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(MIC_FU_ID, MIC_IT_ID, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(MIC_OT_ID, AUDIO_TERMINAL_STREAMING, MIC_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_FEEDBACK_DESCRIPTOR_INIT(0x01, SPK_IT_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_OUT_EP, AUDIO_OUT_PACKET_FS, EP_INTERVAL_FS, AUDIO_OUT_FEEDBACK_EP),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, MIC_OT_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(SPK_IT_ID, AUDIO_TERMINAL_STREAMING, AUDIO_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(SPK_FU_ID, SPK_IT_ID, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(SPK_OT_ID, AUDIO_OUTTERM_HEADPHONES, SPK_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(MIC_IT_ID, AUDIO_INTERM_MIC, AUDIO_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(MIC_FU_ID, MIC_IT_ID, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(MIC_OT_ID, AUDIO_TERMINAL_STREAMING, MIC_FU_ID, AUDIO_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_FEEDBACK_DESCRIPTOR_INIT(0x01, SPK_IT_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_OUT_EP, AUDIO_OUT_PACKET_HS, EP_INTERVAL_HS, AUDIO_OUT_FEEDBACK_EP),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, MIC_OT_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, HEADSET_SLOT_BYTE_SIZE, HEADSET_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_HS, EP_INTERVAL_HS),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro UAC V2 Headset",     /* Product */
    "2026082101",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor audio_v2_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

static const uint8_t default_sampling_freq_table_fs[] = {
    AUDIO_SAMPLE_FREQ_NUM(4),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(44100),
    AUDIO_SAMPLE_FREQ_4B(44100),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
};

static const uint8_t default_sampling_freq_table_hs[] = {
    AUDIO_SAMPLE_FREQ_NUM(6),
    AUDIO_SAMPLE_FREQ_4B(192000),
    AUDIO_SAMPLE_FREQ_4B(192000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(44100),
    AUDIO_SAMPLE_FREQ_4B(44100),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(32000),
    AUDIO_SAMPLE_FREQ_4B(32000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
};

/* Buffer allocation */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_packet_buffer[AUDIO_OUT_PACKET_MAX];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_out_buffer[AUDIO_OUT_BUF_TOTAL];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_feedback_buffer[4];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_mic_in_buffer[AUDIO_IN_BUF_TOTAL];

/* DMA channel definitions */
#define SPEAKER_DMA_CHANNEL     1U
#define MIC_DMA_CHANNEL         2U
#define SPEAKER_DMAMUX_CHANNEL  DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL)
#define MIC_DMAMUX_CHANNEL      DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, MIC_DMA_CHANNEL)

/* Runtime state */
static uint8_t s_usb_speed;
static volatile uint32_t s_i2s_mclk_hz;
static volatile bool s_speaker_rx_flag;
static volatile uint32_t s_speaker_out_buffer_front;
static volatile uint32_t s_speaker_out_buffer_rear;
static volatile bool s_speaker_dma_transfer_req;
static volatile uint32_t s_requested_sample_rate;
static volatile uint32_t s_active_sample_rate;
static volatile int32_t s_speaker_volume_db;
static volatile bool s_speaker_mute;
static volatile bool s_speaker_codec_update_pending;
static volatile uint32_t s_speaker_feedback_value;
static volatile uint32_t s_speaker_feedback_tm;
static volatile uint32_t s_speaker_feedback_cnt;
static volatile uint32_t s_speaker_feedback_interval;   /* DMA transfers per feedback adjust (~100ms), derived from packets_per_sec */
static volatile bool s_speaker_feedback_print_pending;
static volatile uint32_t s_speaker_usb_packet_min;
static volatile uint32_t s_speaker_usb_packet_max;
static volatile bool s_mic_tx_flag;
static volatile bool s_mic_usb_transfer_req;
static volatile uint32_t s_mic_in_buffer_front;
static volatile uint32_t s_mic_in_buffer_rear;
static volatile bool s_mic_priming;
static volatile bool s_mic_split_pending;
static volatile uint32_t s_mic_split_second_size;
static volatile int32_t s_mic_volume_db;
static volatile bool s_mic_mute;
static volatile bool s_mic_codec_update_pending;
static volatile bool s_mic_buffer_print_pending;
static volatile uint32_t s_mic_print_tm;
static volatile uint32_t s_mic_print_interval;

static volatile uint32_t s_audio_out_slot_size;
static volatile uint32_t s_audio_in_slot_size;

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static struct usbd_interface intf2;

static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes);
static struct usbd_endpoint audio_in_ep = {
    .ep_cb = usbd_audio_iso_in_callback,
    .ep_addr = AUDIO_IN_EP
};
static void usbd_audio_iso_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes);
static struct usbd_endpoint audio_out_ep = {
    .ep_cb = usbd_audio_iso_out_callback,
    .ep_addr = AUDIO_OUT_EP
};
static void usbd_audio_iso_out_feedback_callback(uint8_t busid, uint8_t ep, uint32_t nbytes);
static struct usbd_endpoint audio_out_feedback_ep = {
    .ep_cb = usbd_audio_iso_out_feedback_callback,
    .ep_addr = AUDIO_OUT_FEEDBACK_EP
};

/*
 * Audio entity table: shared clock + speaker FU + mic FU
 * Entity count = 3
 */
static struct audio_entity_info audio_entity_table[] = {
    {
        .bEntityId = AUDIO_CLOCK_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_CLOCK_SOURCE,
        .ep = AUDIO_OUT_EP
    },
    {
        .bEntityId = SPK_FU_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
        .ep = AUDIO_OUT_EP
    },
    {
        .bEntityId = MIC_FU_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
        .ep = AUDIO_IN_EP
    },
};

/* Static function declarations */
static void codec_init_i2s(uint32_t sample_rate, uint8_t audio_depth);
static void codec_reconfig_data_format(uint32_t sample_rate, uint8_t audio_depth);
static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool speaker_dma_slot_size_is_valid(uint32_t slot_size);
static uint32_t speaker_dma_slot_size_align(uint32_t slot_size);
static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool mic_dma_slot_size_is_valid(uint32_t slot_size);
static uint32_t mic_dma_slot_size_align(uint32_t slot_size);
static uint32_t speaker_out_buff_get_used(void);
static void speaker_calculate_feedback(void);
static uint32_t mic_in_buff_fill_count(void);
static uint32_t mic_in_buff_get_adaptive_transfer_size(void);
static void mic_in_buff_send_to_usb(uint8_t busid, uint32_t front, uint32_t transfer_size);
static void speaker_apply_codec_mute_volume(void);
static void mic_apply_codec_mute_volume(void);

/*--------------------------------------------------------------------*/
/* USB Event Handler */
/*--------------------------------------------------------------------*/
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid;

    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        s_usb_speed = usbd_get_port_speed(busid);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;
    default:
        break;
    }
}

/*--------------------------------------------------------------------*/
/* Audio Init */
/*--------------------------------------------------------------------*/
void audio_v2_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &audio_v2_descriptor);
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf0, AUDIO_VERSION, audio_entity_table, 3));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf1, AUDIO_VERSION, audio_entity_table, 3));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf2, AUDIO_VERSION, audio_entity_table, 3));
    usbd_add_endpoint(busid, &audio_in_ep);
    usbd_add_endpoint(busid, &audio_out_ep);
    usbd_add_endpoint(busid, &audio_out_feedback_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

/*--------------------------------------------------------------------*/
/* Codec Hardware Initialization */
/*--------------------------------------------------------------------*/
void audio_v2_init_i2s_codec(void)
{
    i2s_config_t i2s_config;
    uint32_t i2s_mclk_hz;

    s_requested_sample_rate = 48000;
    s_active_sample_rate = 48000;

    /* Initialize codec I2C handle */
    s_codec_control.ptr = CODEC_I2C;
    s_codec_control.slave_address = BOARD_AUDIO_CODEC_I2C_ADDR;

    /* Configure I2S basic settings */
    i2s_get_default_config(CODEC_I2S, &i2s_config);
    i2s_config.enable_mclk_out = true;
    if (i2s_init(CODEC_I2S, &i2s_config) != status_success) {
        printf("i2s_init failed\n");
        while (1) {
        }
    }

    s_i2s_invert_fclk_out = i2s_config.invert_fclk_out;

    i2s_mclk_hz = clock_get_frequency(CODEC_I2S_CLK_NAME);
    s_i2s_mclk_hz = i2s_mclk_hz;

    /* Configure I2S multiline transfer */
    codec_init_i2s(s_active_sample_rate, HEADSET_AUDIO_DEPTH);

    /* Configure DMA mux for both TX and RX */
    i2s_enable_tx_dma_request(CODEC_I2S);
    i2s_enable_rx_dma_request(CODEC_I2S);
    dmamux_config(BOARD_APP_DMAMUX, SPEAKER_DMAMUX_CHANNEL, CODEC_I2S_TX_DMAMUX_SRC, true);
    dmamux_config(BOARD_APP_DMAMUX, MIC_DMAMUX_CHANNEL, CODEC_I2S_RX_DMAMUX_SRC, true);

    /* Initialize codec chip */
#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    {
        wm8960_config_t wm8960_config;
        wm8960_get_default_config(&wm8960_config);
        wm8960_config.format.mclk_hz = i2s_mclk_hz;
        wm8960_config.format.sample_rate = s_active_sample_rate;
        wm8960_config.format.bit_width = HEADSET_AUDIO_DEPTH;
        wm8960_config.lrclk_polarity = (s_i2s_invert_fclk_out) ? wm8960_lrclk_polarity_high_for_left_channel : wm8960_lrclk_polarity_low_for_left_channel;
        if (wm8960_init(&s_codec_control, &wm8960_config) != status_success) {
            printf("Init Audio Codec WM8960 failed\n");
        }
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    {
        sgtl_config_t sgtl5000_config;
        sgtl_get_default_config(&sgtl5000_config);
        sgtl5000_config.format.mclk_hz = i2s_mclk_hz;
        sgtl5000_config.format.sample_rate = s_active_sample_rate;
        sgtl5000_config.format.bit_width = HEADSET_AUDIO_DEPTH;
        sgtl5000_config.lrclk_polarity = (s_i2s_invert_fclk_out) ? sgtl_lrclk_polarity_high_for_left_channel : sgtl_lrclk_polarity_low_for_left_channel;
        if (sgtl_init(&s_codec_control, &sgtl5000_config) != status_success) {
            printf("Init Audio Codec SGTL5000 failed\n");
        }
    }
#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    {
        es8389_config_t es8389_config;
        es8389_get_default_config(&es8389_config);
        es8389_config.mclk_hz = i2s_mclk_hz;
        es8389_config.sample_rate = s_active_sample_rate;
        es8389_config.data_width = HEADSET_AUDIO_DEPTH;
        es8389_config.lrclk_polarity = (s_i2s_invert_fclk_out) ? es8389_lrclk_polarity_high_for_left_channel : es8389_lrclk_polarity_low_for_left_channel;
        if (es8389_init(&s_codec_control, &es8389_config) != status_success) {
            printf("Init Audio Codec ES8389 failed\n");
        }
    }
#else
#error no specified Audio Codec!!!
#endif
}

/* Configure both directions together while the shared I2S is idle. */
static void codec_init_i2s(uint32_t sample_rate, uint8_t audio_depth)
{
    i2s_multiline_transfer_config_t transfer;

    i2s_get_default_multiline_transfer_config(&transfer);
    transfer.audio_depth = audio_depth;
    transfer.channel_length = HEADSET_SLOT_BYTE_SIZE * 8;
    transfer.sample_rate = sample_rate;
    transfer.master_mode = true;
    transfer.rx_data_line_en[CODEC_I2S_RX_DATA_LINE] = true;
    transfer.tx_data_line_en[CODEC_I2S_TX_DATA_LINE] = true;
    transfer.rx_channel_slot_mask[CODEC_I2S_RX_DATA_LINE] = 0x03;
    transfer.tx_channel_slot_mask[CODEC_I2S_TX_DATA_LINE] = 0x03;

    if (status_success != i2s_config_multiline_transfer(CODEC_I2S, s_i2s_mclk_hz, &transfer)) {
        printf("I2S config failed for CODEC\n");
        while (1) {
            ;
        }
    }
}

/* Reconfigure codec data format when sample rate changes */
static void codec_reconfig_data_format(uint32_t sample_rate, uint8_t audio_depth)
{
#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    wm8960_set_data_format(&s_codec_control, s_i2s_mclk_hz, sample_rate, audio_depth);
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_config_data_format(&s_codec_control, s_i2s_mclk_hz, sample_rate, audio_depth);
#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_set_data_format(&s_codec_control, s_i2s_mclk_hz, sample_rate, audio_depth);
#endif
}

/*--------------------------------------------------------------------*/
/* DMA / ISR */
/*--------------------------------------------------------------------*/
SDK_DECLARE_EXT_ISR_M(BOARD_APP_DMA1_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL);
    mic_status = dma_check_transfer_status(BOARD_APP_DMA1, MIC_DMA_CHANNEL);

    /* Speaker: DMA completed, kick next transfer or defer */
    if (s_speaker_rx_flag && (0 != (speaker_status & DMA_CHANNEL_STATUS_TC))) {
        speaker_calculate_feedback();
        uint32_t dma_size = s_audio_out_slot_size;
        if (speaker_out_buff_get_used() >= dma_size) {
            uint32_t front = s_speaker_out_buffer_front;
            speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[front], dma_size);
            front += dma_size;
            if (front >= AUDIO_OUT_BUF_TOTAL) {
                front -= AUDIO_OUT_BUF_TOTAL;
            }
            s_speaker_out_buffer_front = front;
        } else {
            /* Not enough data for one DMA transfer, defer to avoid reading garbage */
            s_speaker_dma_transfer_req = true;
        }
    }

    /* Mic: DMA completed, advance rear and start next DMA, then kick USB IN if waiting */
    if (s_mic_tx_flag && (0 != (mic_status & DMA_CHANNEL_STATUS_TC))) {
        uint32_t rear = s_mic_in_buffer_rear;
        rear += s_audio_in_slot_size;
        if (rear >= AUDIO_IN_BUF_TOTAL) {
            rear = 0;
        }
        s_mic_in_buffer_rear = rear;
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[rear], s_audio_in_slot_size);

        /* Priming: accumulate half the buffer before starting USB transfers */
        if (s_mic_priming) {
            if (mic_in_buff_fill_count() >= (AUDIO_IN_BUF_TOTAL / 2)) {
                s_mic_priming = false;
                s_mic_usb_transfer_req = true;
            }
        }

        /* If USB IN is waiting for data, kick it off now */
        if (s_mic_usb_transfer_req && !s_mic_split_pending) {
            uint32_t transfer_size = mic_in_buff_get_adaptive_transfer_size();
            if (mic_in_buff_fill_count() >= transfer_size) {
                uint32_t front = s_mic_in_buffer_front;
                s_mic_usb_transfer_req = false;
                mic_in_buff_send_to_usb(0, front, transfer_size);
                front += transfer_size;
                if (front >= AUDIO_IN_BUF_TOTAL) {
                    front -= AUDIO_IN_BUF_TOTAL;
                }
                s_mic_in_buffer_front = front;
            }
        }
        s_mic_print_tm++;
        if (s_mic_print_tm >= s_mic_print_interval) {
            s_mic_print_tm = 0;
            s_mic_buffer_print_pending = true;
        }
    }
}

/*--------------------------------------------------------------------*/
/* Main Audio Task (called from main loop) */
/*--------------------------------------------------------------------*/
void audio_v2_task(uint8_t busid)
{
    (void)busid;

    /* Apply deferred codec state updates (I2C is slow, must not run in ISR) */
    if (s_speaker_codec_update_pending) {
        s_speaker_codec_update_pending = false;
        speaker_apply_codec_mute_volume();
    }
    if (s_mic_codec_update_pending) {
        s_mic_codec_update_pending = false;
        mic_apply_codec_mute_volume();
    }

    /* Deferred feedback status print (printf is slow, must not run in ISR) */
    if (s_speaker_feedback_print_pending) {
        s_speaker_feedback_print_pending = false;
        uint32_t used = speaker_out_buff_get_used();
        uint32_t total = AUDIO_OUT_BUF_TOTAL;
        uint32_t percent = (used * 100u) / total;
        uint32_t fb_value = s_speaker_feedback_value;
        uint32_t freq_hz;
        if (s_usb_speed == USB_SPEED_HIGH) {
            freq_hz = AUDIO_FEEDBACK_TO_FREQ_HS(fb_value);
        } else {
            freq_hz = AUDIO_FEEDBACK_TO_FREQ_FS(fb_value);
        }
        uint32_t packet_min = s_speaker_usb_packet_min;
        uint32_t packet_max = s_speaker_usb_packet_max;
        s_speaker_usb_packet_min = AUDIO_OUT_PACKET_MAX;
        s_speaker_usb_packet_max = 0;
        printf("spk fb: used=%lu/%lu (%lu%%) val=%lu hz=%lu pkt=%lu..%lu\n",
               used, total, percent, fb_value, freq_hz, packet_min, packet_max);
    }

    /* Deferred mic buffer status print */
    if (s_mic_buffer_print_pending) {
        s_mic_buffer_print_pending = false;
        uint32_t used = mic_in_buff_fill_count();
        uint32_t total = AUDIO_IN_BUF_TOTAL;
        uint32_t percent = (used * 100u) / total;
        printf("mic buf: used=%lu/%lu (%lu%%)\n", used, total, percent);
    }
}

/*--------------------------------------------------------------------*/
/* CherryUSB Audio Callbacks */
/*--------------------------------------------------------------------*/
void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    uint32_t packets_per_sec;
    bool first_stream = !s_speaker_rx_flag && !s_mic_tx_flag;

    /* Compute actual packet size from current sample rate and USB speed */
    if (usbd_get_port_speed(busid) == USB_SPEED_HIGH) {
        packets_per_sec = HEADSET_HS_PACKETS_PER_SEC;
    } else {
        packets_per_sec = HEADSET_FS_PACKETS_PER_SEC;
    }

    if (first_stream) {
        i2s_reset_tx_rx(CODEC_I2S);

        if (s_active_sample_rate != s_requested_sample_rate) {
            /* Reconfigure the shared I2S source clock for the requested rate family
             * (e.g. 48kHz vs 44.1kHz), then re-read the resulting MCLK so the I2S and
             * codec are configured against the actual clock. */
            s_i2s_mclk_hz = board_config_i2s_clock(CODEC_I2S, s_requested_sample_rate);
            codec_init_i2s(s_requested_sample_rate, HEADSET_AUDIO_DEPTH);
            codec_reconfig_data_format(s_requested_sample_rate, HEADSET_AUDIO_DEPTH);
            s_active_sample_rate = s_requested_sample_rate;
            USB_LOG_RAW("Init I2S Clock Ok! Sample Rate: %d, mclk_hz: %d\r\n", s_active_sample_rate, s_i2s_mclk_hz);
        }

        /* Multiline configuration enables both directions; DMA setup enables the active one. */
        i2s_disable_tx(CODEC_I2S, 1U << CODEC_I2S_TX_DATA_LINE);
        i2s_disable_rx(CODEC_I2S, 1U << CODEC_I2S_RX_DATA_LINE);
    } else if (s_active_sample_rate != s_requested_sample_rate) {
        USB_LOG_ERR("Cannot open another stream while a shared I2S rate change is pending\r\n");
        return;
    }

    if (intf == 1) {
        /* Speaker streaming interface opened */
        if (!first_stream) {
            i2s_reset_tx(CODEC_I2S);
        }
        s_speaker_feedback_interval = packets_per_sec / 10u;
        s_speaker_rx_flag = true;
        s_speaker_out_buffer_front = 0;
        s_speaker_out_buffer_rear = 0;
        s_speaker_dma_transfer_req = true;
        s_speaker_feedback_tm = 0;
        s_speaker_usb_packet_min = AUDIO_OUT_PACKET_MAX;
        s_speaker_usb_packet_max = 0;
        /* DMA consumes one USB service interval per transfer (matches host packet cadence) */
        s_audio_out_slot_size = (s_active_sample_rate * AUDIO_OUT_FRAME_SIZE) / packets_per_sec;
        /* Snap to a valid slot for rates with a non-integer per-packet sample count (44.1kHz family) */
        s_audio_out_slot_size = speaker_dma_slot_size_align(s_audio_out_slot_size);
        if (!speaker_dma_slot_size_is_valid(s_audio_out_slot_size)) {
            s_speaker_rx_flag = false;
            s_speaker_dma_transfer_req = false;
            dma_abort_channel(BOARD_APP_DMA1, 1u << SPEAKER_DMA_CHANNEL);
            USB_LOG_RAW("Invalid speaker DMA slot size: %lu, buffer size: %lu\r\n", s_audio_out_slot_size, AUDIO_OUT_BUF_TOTAL);
            return;
        }
        usbd_ep_start_read(busid, AUDIO_OUT_EP, s_speaker_packet_buffer, AUDIO_OUT_PACKET_MAX);
        /* Send initial feedback sample rate */
        if (s_usb_speed == USB_SPEED_HIGH) {
            s_speaker_feedback_value = AUDIO_FREQ_TO_FEEDBACK_HS(s_active_sample_rate);
            AUDIO_FEEDBACK_TO_BUF_HS_INTERVAL(s_speaker_feedback_buffer, s_speaker_feedback_value, EP_INTERVAL_HS);
            usbd_ep_start_write(busid, AUDIO_OUT_FEEDBACK_EP, s_speaker_feedback_buffer, FEEDBACK_ENDP_PACKET_SIZE_HS);
        } else {
            s_speaker_feedback_value = AUDIO_FREQ_TO_FEEDBACK_FS(s_active_sample_rate);
            AUDIO_FEEDBACK_TO_BUF_FS_INTERVAL(s_speaker_feedback_buffer, s_speaker_feedback_value, EP_INTERVAL_FS);
            usbd_ep_start_write(busid, AUDIO_OUT_FEEDBACK_EP, s_speaker_feedback_buffer, FEEDBACK_ENDP_PACKET_SIZE_FS);
        }
        USB_LOG_RAW("OPEN SPEAKER, sample rate: %lu Hz\r\n", (unsigned long)s_active_sample_rate);
    } else {
        /* Mic streaming interface opened */
        /* IN: compute exact size from actual sample rate; device controls transfer size */
        s_audio_in_slot_size = (s_active_sample_rate * AUDIO_IN_FRAME_SIZE) / packets_per_sec;
        /* Snap to a valid slot for rates with a non-integer per-packet sample count (44.1kHz family) */
        s_audio_in_slot_size = mic_dma_slot_size_align(s_audio_in_slot_size);
        if (!mic_dma_slot_size_is_valid(s_audio_in_slot_size)) {
            s_mic_tx_flag = false;
            s_mic_usb_transfer_req = false;
            dma_abort_channel(BOARD_APP_DMA1, 1u << MIC_DMA_CHANNEL);
            USB_LOG_RAW("Invalid mic DMA slot size: %lu, buffer size: %lu\r\n", s_audio_in_slot_size, AUDIO_IN_BUF_TOTAL);
            return;
        }
        if (!first_stream) {
            i2s_reset_rx(CODEC_I2S);
        }
        s_mic_tx_flag = 1;
        s_mic_in_buffer_front = 0;
        s_mic_in_buffer_rear = 0;
        s_mic_usb_transfer_req = false;
        s_mic_priming = true;
        s_mic_split_pending = false;
        s_mic_print_interval = packets_per_sec / 10u;
        s_mic_print_tm = 0;
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[0], s_audio_in_slot_size);
        USB_LOG_RAW("OPEN MIC, sample rate: %lu Hz\r\n", (unsigned long)s_active_sample_rate);
    }

    if (first_stream) {
        i2s_start(CODEC_I2S);
    }
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

    if (intf == 1) {
        s_speaker_rx_flag = 0;
        /* Abort any ongoing DMA transfer to prevent stale TC after close */
        dma_abort_channel(BOARD_APP_DMA1, 1u << SPEAKER_DMA_CHANNEL);
        i2s_reset_tx(CODEC_I2S);
        USB_LOG_RAW("CLOSE SPEAKER\r\n");
    } else {
        s_mic_tx_flag = 0;
        dma_abort_channel(BOARD_APP_DMA1, 1u << MIC_DMA_CHANNEL);
        i2s_reset_rx(CODEC_I2S);
        USB_LOG_RAW("CLOSE MIC\r\n");
    }

    /* Stop I2S when both interfaces are inactive */
    if (!s_speaker_rx_flag && !s_mic_tx_flag) {
        i2s_stop(CODEC_I2S);
    }
}

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume_db)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_volume_db = volume_db;
        s_speaker_codec_update_pending = true;
    } else if (ep == AUDIO_IN_EP) {
        s_mic_volume_db = volume_db;
        s_mic_codec_update_pending = true;
    } else {
        ;
    }
}

int usbd_audio_get_volume(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

    int volume = 0;

    if (ep == AUDIO_OUT_EP) {
        volume = s_speaker_volume_db;
    } else if (ep == AUDIO_IN_EP) {
        volume = s_mic_volume_db;
    } else {
        ;
    }

    return volume;
}

void usbd_audio_set_mute(uint8_t busid, uint8_t ep, uint8_t ch, bool mute)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_mute = mute;
        s_speaker_codec_update_pending = true;
    } else if (ep == AUDIO_IN_EP) {
        s_mic_mute = mute;
        s_mic_codec_update_pending = true;
    } else {
        ;
    }
}

bool usbd_audio_get_mute(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

    bool mute = false;

    if (ep == AUDIO_OUT_EP) {
        mute = s_speaker_mute;
    } else if (ep == AUDIO_IN_EP) {
        mute = s_mic_mute;
    } else {
        ;
    }

    return mute;
}

void usbd_audio_set_sampling_freq(uint8_t busid, uint8_t ep, uint32_t sampling_freq)
{
    (void)busid;
    (void)ep;

    /* Apply the shared clock change when the next first stream opens. */
    s_requested_sample_rate = sampling_freq;
}

uint32_t usbd_audio_get_sampling_freq(uint8_t busid, uint8_t ep)
{
    (void)busid;
    (void)ep;

    return s_requested_sample_rate;
}

void usbd_audio_get_sampling_freq_table(uint8_t busid, uint8_t ep, uint8_t **sampling_freq_table)
{
    (void)busid;
    (void)ep;

    if (usbd_get_port_speed(busid) == USB_SPEED_HIGH) {
        *sampling_freq_table = (uint8_t *)default_sampling_freq_table_hs;
    } else {
        *sampling_freq_table = (uint8_t *)default_sampling_freq_table_fs;
    }
}

/*--------------------------------------------------------------------*/
/* Endpoint Callbacks */
/*--------------------------------------------------------------------*/
static void usbd_audio_iso_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if (s_speaker_rx_flag) {
        if (nbytes < s_speaker_usb_packet_min) {
            s_speaker_usb_packet_min = nbytes;
        }
        if (nbytes > s_speaker_usb_packet_max) {
            s_speaker_usb_packet_max = nbytes;
        }
        uint32_t rear = s_speaker_out_buffer_rear;

        if (rear + nbytes > AUDIO_OUT_BUF_TOTAL) {
            /* Circular buffer wrap: split into two parts */
            uint32_t first_part = AUDIO_OUT_BUF_TOTAL - rear;
            memcpy(&s_speaker_out_buffer[rear], s_speaker_packet_buffer, first_part);
            memcpy(&s_speaker_out_buffer[0], s_speaker_packet_buffer + first_part, nbytes - first_part);
            rear = nbytes - first_part;
        } else {
            memcpy(&s_speaker_out_buffer[rear], s_speaker_packet_buffer, nbytes);
            rear += nbytes;
            if (rear >= AUDIO_OUT_BUF_TOTAL) {
                rear -= AUDIO_OUT_BUF_TOTAL;
            }
        }
        s_speaker_out_buffer_rear = rear;

        /* Initial kickoff: buffer reached half-full, start first DMA transfer directly */
        uint32_t buffer_used = speaker_out_buff_get_used();
        if (s_speaker_dma_transfer_req && (buffer_used >= (AUDIO_OUT_BUF_TOTAL / 2u))) {
            s_speaker_dma_transfer_req = false;
            s_speaker_feedback_tm = 0;
            uint32_t front = s_speaker_out_buffer_front;
            uint32_t dma_size = s_audio_out_slot_size;
            speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[front], dma_size);
            front += dma_size;
            if (front >= AUDIO_OUT_BUF_TOTAL) {
                front -= AUDIO_OUT_BUF_TOTAL;
            }
            s_speaker_out_buffer_front = front;
        }
        usbd_ep_start_read(busid, ep, s_speaker_packet_buffer, AUDIO_OUT_PACKET_MAX);
    }
}

static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    if (s_mic_tx_flag) {
        if (s_mic_split_pending) {
            /* Complete a pending split transfer: send the second (head) part.
             * Front was already advanced by the full transfer_size when the split was initiated.
             */
            s_mic_split_pending = false;
            usbd_ep_start_write(busid, AUDIO_IN_EP, s_mic_in_buffer, s_mic_split_second_size);
        } else {
            uint32_t transfer_size = mic_in_buff_get_adaptive_transfer_size();
            if (mic_in_buff_fill_count() >= transfer_size) {
                uint32_t front = s_mic_in_buffer_front;
                mic_in_buff_send_to_usb(busid, front, transfer_size);
                front += transfer_size;
                if (front >= AUDIO_IN_BUF_TOTAL) {
                    front -= AUDIO_IN_BUF_TOTAL;
                }
                s_mic_in_buffer_front = front;
            } else {
                /* Not enough data, defer USB IN kick-off to DMA ISR */
                s_mic_usb_transfer_req = true;
            }
        }
    }
}

/*--------------------------------------------------------------------*/
/* DMA Transfer Helpers */
/*--------------------------------------------------------------------*/
static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };
    hpm_stat_t status;

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&CODEC_I2S->TXD[CODEC_I2S_TX_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    status = dma_setup_channel(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL, &ch_config, true);
    if (status != status_success) {
        printf(" speaker dma setup channel failed\n");
    }
    if (status == status_success) {
        i2s_enable_tx(CODEC_I2S, 1U << CODEC_I2S_TX_DATA_LINE);
    }
}

static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };
    hpm_stat_t status;

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = (uint32_t)(&CODEC_I2S->RXD[CODEC_I2S_RX_DATA_LINE]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    status = dma_setup_channel(BOARD_APP_DMA1, MIC_DMA_CHANNEL, &ch_config, true);
    if (status != status_success) {
        printf(" mic dma setup channel failed\n");
    }
    if (status == status_success) {
        i2s_enable_rx(CODEC_I2S, 1U << CODEC_I2S_RX_DATA_LINE);
    }
}

static uint32_t speaker_out_buff_get_used(void)
{
    uint32_t front = s_speaker_out_buffer_front;
    uint32_t rear = s_speaker_out_buffer_rear;

    if (rear >= front) {
        return rear - front;
    } else {
        return AUDIO_OUT_BUF_TOTAL + rear - front;
    }
}

static bool speaker_dma_slot_size_is_valid(uint32_t slot_size)
{
    return (slot_size != 0U)
        && ((slot_size % AUDIO_OUT_FRAME_SIZE) == 0U)
        && ((AUDIO_OUT_BUF_TOTAL % slot_size) == 0U);
}

static bool mic_dma_slot_size_is_valid(uint32_t slot_size)
{
    return (slot_size != 0U)
        && ((slot_size % AUDIO_IN_FRAME_SIZE) == 0U)
        && ((AUDIO_IN_BUF_TOTAL % slot_size) == 0U);
}

/*
 * The fixed-slot DMA design requires each slot to be an integer number of audio
 * frames and to divide the ring buffer evenly. Rates whose per-packet sample count
 * is non-integer (the 44.1kHz family: 44100/8000 = 5.5125, 44100/1000 = 44.1)
 * produce a truncated, invalid slot. Snap the ideal slot to the NEAREST frame-aligned
 * value that also divides the buffer; the small rate mismatch this introduces is
 * absorbed by the speaker feedback / mic adaptive-size mechanisms (the real audio
 * rate is set by the I2S clock, not by the DMA slot granularity). For rates that are
 * already valid (48k/96k/192k/16k/32k) this returns the value unchanged.
 */
static uint32_t dma_slot_size_align(uint32_t slot_size, uint32_t frame_size, uint32_t buf_total)
{
    if (slot_size == 0U) {
        return 0U;
    }

    /* Slot must divide the buffer evenly <=> its frame count must divide the buffer's
     * total frame count. Search outward from the ideal (rounded) frame count for the
     * nearest divisor. */
    uint32_t total_frames = buf_total / frame_size;
    uint32_t ideal = (slot_size + (frame_size / 2U)) / frame_size;
    if (ideal == 0U) {
        ideal = 1U;
    }

    for (uint32_t delta = 0U; delta <= total_frames; delta++) {
        uint32_t down = (ideal > delta) ? (ideal - delta) : 0U;
        uint32_t up = ideal + delta;

        if ((down >= 1U) && ((total_frames % down) == 0U)) {
            return down * frame_size;
        }
        if ((up <= total_frames) && ((total_frames % up) == 0U)) {
            return up * frame_size;
        }
    }

    return frame_size; /* Fallback: 1 frame always divides the buffer */
}

static uint32_t speaker_dma_slot_size_align(uint32_t slot_size)
{
    return dma_slot_size_align(slot_size, AUDIO_OUT_FRAME_SIZE, AUDIO_OUT_BUF_TOTAL);
}

static uint32_t mic_dma_slot_size_align(uint32_t slot_size)
{
    return dma_slot_size_align(slot_size, AUDIO_IN_FRAME_SIZE, AUDIO_IN_BUF_TOTAL);
}

static void speaker_calculate_feedback(void)
{
    int32_t error;
    int32_t step;

    s_speaker_feedback_tm++;
    if (s_speaker_feedback_tm >= s_speaker_feedback_interval) {
        s_speaker_feedback_tm = 0;

        uint32_t buffer_used = speaker_out_buff_get_used();

        /* Proportional control with half-margin rounding: |error| >= AUDIO_FEEDBACK_MARGIN/2
         * yields |step| >= 1, while |error| < AUDIO_FEEDBACK_MARGIN/2 is the center deadband
         * (step == 0), which is handled below by snapping feedback to nominal. */
        error = (int32_t)buffer_used - (int32_t)AUDIO_OUT_BUF_CENTER;
        if (error > 0) {
            step = (error + (int32_t)(AUDIO_FEEDBACK_MARGIN / 2u)) / (int32_t)AUDIO_FEEDBACK_MARGIN;
        } else {
            step = (error - (int32_t)(AUDIO_FEEDBACK_MARGIN / 2u)) / (int32_t)AUDIO_FEEDBACK_MARGIN;
        }

        if (step > AUDIO_FEEDBACK_ADJUST_MAX_STEP) {
            step = AUDIO_FEEDBACK_ADJUST_MAX_STEP;
        } else if (step < -AUDIO_FEEDBACK_ADJUST_MAX_STEP) {
            step = -AUDIO_FEEDBACK_ADJUST_MAX_STEP;
        }

        /* Inside the center deadband (|error| < half-margin ⇒ step == 0), force feedback to
         * nominal. This is level-triggered: no matter how the center crossing happens to be
         * sampled, the value can never stay stuck below/above nominal while the buffer hovers
         * near center. */
        if (step == 0) {
            if (s_usb_speed == USB_SPEED_HIGH) {
                s_speaker_feedback_value = AUDIO_FREQ_TO_FEEDBACK_HS(s_active_sample_rate);
            } else {
                s_speaker_feedback_value = AUDIO_FREQ_TO_FEEDBACK_FS(s_active_sample_rate);
            }
        } else {
            /* Positive step → buffer too full → reduce feedback (tell host to send less).
             * Negative step → buffer too empty → increase feedback; uint32_t wrap-around makes
             * this single subtraction cover both directions. */
            s_speaker_feedback_value -= (uint32_t)step;
        }

        if (s_usb_speed == USB_SPEED_HIGH) {
            AUDIO_FEEDBACK_TO_BUF_HS_INTERVAL(s_speaker_feedback_buffer, s_speaker_feedback_value, EP_INTERVAL_HS);
        } else {
            AUDIO_FEEDBACK_TO_BUF_FS_INTERVAL(s_speaker_feedback_buffer, s_speaker_feedback_value, EP_INTERVAL_FS);
        }

        s_speaker_feedback_print_pending = true;
    }
}

static void usbd_audio_iso_out_feedback_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)nbytes;
    if (s_speaker_rx_flag) {
        s_speaker_feedback_cnt++;
        if (s_usb_speed == USB_SPEED_HIGH) {
            usbd_ep_start_write(busid, ep, s_speaker_feedback_buffer, FEEDBACK_ENDP_PACKET_SIZE_HS);
        } else {
            usbd_ep_start_write(busid, ep, s_speaker_feedback_buffer, FEEDBACK_ENDP_PACKET_SIZE_FS);
        }
    }
}

static void mic_in_buff_send_to_usb(uint8_t busid, uint32_t front, uint32_t transfer_size)
{
    if (front + transfer_size > AUDIO_IN_BUF_TOTAL) {
        /* Circular buffer wrap: split into two USB writes.
         * First part (tail) sent now; second part (head) deferred to callback via s_mic_split_pending.
         * Front is advanced by the full transfer_size at once to keep state consistent for ISR.
         */
        uint32_t first_part = AUDIO_IN_BUF_TOTAL - front;
        s_mic_split_second_size = transfer_size - first_part;
        s_mic_split_pending = true;
        usbd_ep_start_write(busid, AUDIO_IN_EP, &s_mic_in_buffer[front], first_part);
    } else {
        usbd_ep_start_write(busid, AUDIO_IN_EP, &s_mic_in_buffer[front], transfer_size);
    }
}

static uint32_t mic_in_buff_fill_count(void)
{
    int32_t count = (int32_t)s_mic_in_buffer_rear - (int32_t)s_mic_in_buffer_front;
    if (count < 0) {
        count += AUDIO_IN_BUF_TOTAL;
    }
    return (uint32_t)count;
}

static uint32_t mic_in_buff_get_adaptive_transfer_size(void)
{
    uint32_t transfer_size = s_audio_in_slot_size;
    uint32_t fill = mic_in_buff_fill_count();

    if (fill > AUDIO_ADAPTIVE_FULL_THRESHOLD) {
        /* Buffer nearly full → send 1 extra audio frame to drain faster */
        transfer_size = s_audio_in_slot_size + AUDIO_IN_FRAME_SIZE;
    } else if (fill < AUDIO_ADAPTIVE_EMPTY_THRESHOLD) {
        /* Buffer nearly empty → send 1 fewer audio frame to let buffer refill */
        transfer_size = s_audio_in_slot_size - AUDIO_IN_FRAME_SIZE;
    }

    return transfer_size;
}

/*--------------------------------------------------------------------*/
/* Codec Mute/Volume Apply (deferred from ISR context) */
/*--------------------------------------------------------------------*/
static void speaker_apply_codec_mute_volume(void)
{
#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    if (s_speaker_mute) {
        wm8960_mute(&s_codec_control, wm8960_module_dac);
    } else {
        float volume_db;
        wm8960_clamp_volume_db(wm8960_module_dac, (float)s_speaker_volume_db, &volume_db);
        wm8960_set_volume_db(&s_codec_control, wm8960_module_dac, volume_db);
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_set_mute(&s_codec_control, sgtl_module_dac, s_speaker_mute);
    if (!s_speaker_mute) {
        float volume_db;
        sgtl_clamp_volume_db(sgtl_module_dac, (float)s_speaker_volume_db, &volume_db);
        sgtl_set_volume_db(&s_codec_control, sgtl_module_dac, volume_db);
    }
#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_mute(&s_codec_control, es8389_dac1, s_speaker_mute);
    es8389_mute(&s_codec_control, es8389_dac2, s_speaker_mute);
    if (!s_speaker_mute) {
        float volume_db;
        es8389_clamp_volume_db(es8389_dac1, (float)s_speaker_volume_db, &volume_db);
        es8389_set_volume_db(&s_codec_control, es8389_dac1, volume_db);
        es8389_set_volume_db(&s_codec_control, es8389_dac2, volume_db);
    }
#endif
}

static void mic_apply_codec_mute_volume(void)
{
#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    if (s_mic_mute) {
        wm8960_mute(&s_codec_control, wm8960_module_adc);
    } else {
        float volume_db;
        wm8960_clamp_volume_db(wm8960_module_adc, (float)s_mic_volume_db, &volume_db);
        wm8960_set_volume_db(&s_codec_control, wm8960_module_adc, volume_db);
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_set_mute(&s_codec_control, sgtl_module_adc, s_mic_mute);
    if (!s_mic_mute) {
        float volume_db;
        sgtl_clamp_volume_db(sgtl_module_adc, (float)s_mic_volume_db, &volume_db);
        sgtl_set_volume_db(&s_codec_control, sgtl_module_adc, volume_db);
    }
#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_mute(&s_codec_control, es8389_adc1, s_mic_mute);
    es8389_mute(&s_codec_control, es8389_adc2, s_mic_mute);
    if (!s_mic_mute) {
        float volume_db;
        es8389_clamp_volume_db(es8389_adc1, (float)s_mic_volume_db, &volume_db);
        es8389_set_volume_db(&s_codec_control, es8389_adc1, volume_db);
        es8389_set_volume_db(&s_codec_control, es8389_adc2, volume_db);
    }
#endif
}
