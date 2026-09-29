/*
 * Copyright (c) 2022-2026 HPMicro
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
#ifdef HPMSOC_HAS_HPMSDK_PDMLITE
#include "hpm_pdmlite_drv.h"
#else
#include "hpm_pdm_drv.h"
#endif
#include "hpm_dao_drv.h"
#include "audio_v2_mic_speaker.h"


#define EP_INTERVAL_HS 0x02
#define EP_INTERVAL_FS 0x01

/* bInterval -> packets/sec: HS 8000 / 2^(bInterval-1), FS 1000 / 2^(bInterval-1) */
#define MIC_SPEAKER_HS_MICROFRAMES_PER_PACKET (1U << (EP_INTERVAL_HS - 1U))
#define MIC_SPEAKER_HS_PACKETS_PER_SEC        (8000U / MIC_SPEAKER_HS_MICROFRAMES_PER_PACKET)
#define MIC_SPEAKER_FS_FRAMES_PER_PACKET      (1U << (EP_INTERVAL_FS - 1U))
#define MIC_SPEAKER_FS_PACKETS_PER_SEC        (1000U / MIC_SPEAKER_FS_FRAMES_PER_PACKET)

#define AUDIO_VERSION 0x0200

#define AUDIO_OUT_EP 0x02
#define AUDIO_IN_EP  0x81

#define AUDIO_OUT_CLOCK_ID 0x01
#define AUDIO_OUT_FU_ID    0x03
#define AUDIO_IN_CLOCK_ID  0x05
#define AUDIO_IN_FU_ID     0x07

#define SPEAKER_MAX_SAMPLE_FREQ_HS 384000
#define SPEAKER_MAX_SAMPLE_FREQ_FS 96000
#define SPEAKER_SLOT_BYTE_SIZE 4
#define SPEAKER_AUDIO_DEPTH    24

#define MIC_SAMPLE_FREQ    16000
#define MIC_SLOT_BYTE_SIZE 4
#define MIC_AUDIO_DEPTH    24

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
#define AUDIO_IN_FRAME_SIZE   (IN_CHANNEL_NUM * MIC_SLOT_BYTE_SIZE)
#define AUDIO_OUT_FRAME_SIZE  (OUT_CHANNEL_NUM * SPEAKER_SLOT_BYTE_SIZE)

/* I2S DMA slot sizes: 1 audio frame less than descriptor packet */
#define AUDIO_IN_SLOT_HS      ((uint32_t)((MIC_SAMPLE_FREQ * AUDIO_IN_FRAME_SIZE) / MIC_SPEAKER_HS_PACKETS_PER_SEC))
#define AUDIO_IN_SLOT_FS      ((uint32_t)((MIC_SAMPLE_FREQ * AUDIO_IN_FRAME_SIZE) / MIC_SPEAKER_FS_PACKETS_PER_SEC))
#define AUDIO_IN_SLOT_MAX     ((AUDIO_IN_SLOT_HS) > (AUDIO_IN_SLOT_FS) ? (AUDIO_IN_SLOT_HS) : (AUDIO_IN_SLOT_FS))

#define AUDIO_OUT_SLOT_HS     ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ_HS * AUDIO_OUT_FRAME_SIZE) / MIC_SPEAKER_HS_PACKETS_PER_SEC))
#define AUDIO_OUT_SLOT_FS     ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ_FS * AUDIO_OUT_FRAME_SIZE) / MIC_SPEAKER_FS_PACKETS_PER_SEC))
#define AUDIO_OUT_SLOT_MAX    ((AUDIO_OUT_SLOT_HS) > (AUDIO_OUT_SLOT_FS) ? (AUDIO_OUT_SLOT_HS) : (AUDIO_OUT_SLOT_FS))

/* USB descriptor packet sizes: slot + 1 extra audio frame */
#define AUDIO_IN_PACKET_HS    (AUDIO_IN_SLOT_HS + AUDIO_IN_FRAME_SIZE)
#define AUDIO_IN_PACKET_FS    (AUDIO_IN_SLOT_FS + AUDIO_IN_FRAME_SIZE)
#define AUDIO_OUT_PACKET_HS   (AUDIO_OUT_SLOT_HS + AUDIO_OUT_FRAME_SIZE)
#define AUDIO_OUT_PACKET_FS   (AUDIO_OUT_SLOT_FS + AUDIO_OUT_FRAME_SIZE)

/* Speaker OUT buffer: 2D slot-based, each slot sized to max USB packet */
#define AUDIO_OUT_PACKET_MAX  ((AUDIO_OUT_PACKET_HS) > (AUDIO_OUT_PACKET_FS) ? (AUDIO_OUT_PACKET_HS) : (AUDIO_OUT_PACKET_FS))
#define AUDIO_IN_PACKET_MAX   ((AUDIO_IN_PACKET_HS) > (AUDIO_IN_PACKET_FS) ? (AUDIO_IN_PACKET_HS) : (AUDIO_IN_PACKET_FS))

#define USB_AUDIO_CONFIG_DESC_SIZ (9 +                                                     \
                                   AUDIO_V2_AC_DESCRIPTOR_LEN +                            \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM) +  \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_AS_DESCRIPTOR_LEN +                            \
                                   AUDIO_V2_AS_DESCRIPTOR_LEN)

#define AUDIO_AC_SIZ (AUDIO_V2_SIZEOF_AC_HEADER_DESC +                        \
                      AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                      AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM) +  \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x06, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x08, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_HS, EP_INTERVAL_HS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_HS, EP_INTERVAL_HS),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x06, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x08, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_FS, EP_INTERVAL_FS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x06, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x08, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_FS, EP_INTERVAL_FS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x06, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x08, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_HS, EP_INTERVAL_HS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_HS, EP_INTERVAL_HS),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro UAC V2 DEMO",        /* Product */
    "2026090201",                 /* Serial Number */
};

static uint32_t s_audio_out_packet_size;
static uint32_t s_audio_in_packet_size;

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

static const uint8_t speaker_default_sampling_freq_table_fs[] = {
    AUDIO_SAMPLE_FREQ_NUM(3),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
};

static const uint8_t speaker_default_sampling_freq_table_hs[] = {
    AUDIO_SAMPLE_FREQ_NUM(5),
    AUDIO_SAMPLE_FREQ_4B(384000),
    AUDIO_SAMPLE_FREQ_4B(384000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(192000),
    AUDIO_SAMPLE_FREQ_4B(192000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
};

static const uint8_t mic_default_sampling_freq_table[] = {
    AUDIO_SAMPLE_FREQ_NUM(1),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00)
};

/* static variable definition */
#define SPEAKER_DMA_CHANNEL 1U
#define MIC_DMA_CHANNEL     2U
#define SPEAKER_DMAMUX_CHANNEL    DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL)
#define MIC_DMAMUX_CHANNEL        DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, MIC_DMA_CHANNEL)

#define MIC_I2S               BOARD_MIC_I2S
#define MIC_I2S_CLK_NAME      BOARD_MIC_I2S_CLK_NAME
#define MIC_I2S_DATA_LINE     BOARD_MIC_I2S_DATA_LINE
#define MIC_I2S_RX_DMAMUX_SRC BOARD_MIC_I2S_RX_DMAMUX_SRC

#define SPEAKER_I2S               BOARD_SPEAKER_I2S
#define SPEAKER_I2S_CLK_NAME      BOARD_SPEAKER_I2S_CLK_NAME
#define SPEAKER_I2S_DATA_LINE     BOARD_SPEAKER_I2S_DATA_LINE
#define SPEAKER_I2S_TX_DMAMUX_SRC BOARD_SPEAKER_I2S_TX_DMAMUX_SRC

/* Static Variables */
#define AUDIO_BUFFER_COUNT 32
#define AUDIO_PRIME_PACKET_COUNT (AUDIO_BUFFER_COUNT / 2U)
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_out_buffer[AUDIO_BUFFER_COUNT][AUDIO_OUT_PACKET_MAX];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_mic_in_buffer[AUDIO_BUFFER_COUNT][AUDIO_IN_PACKET_MAX];
static volatile uint32_t s_speaker_out_buffer_size[AUDIO_BUFFER_COUNT];
static volatile uint32_t s_speaker_i2s_mclk_hz;
static volatile bool s_speaker_rx_flag;
static volatile bool s_speaker_dma_transfer_req;
static volatile bool s_speaker_dma_priming;
static volatile uint8_t s_speaker_out_buffer_front;
static volatile uint8_t s_speaker_out_buffer_rear;
static volatile uint32_t s_speaker_sample_rate;
static volatile int32_t s_speaker_volume_db;
static volatile bool s_speaker_mute;
static volatile bool s_mic_tx_flag;
static volatile bool s_mic_usb_transfer_req;
/* Hold USB IN until the capture ring has enough startup data to absorb
 * initial USB/I2S timing jitter. This is only used when opening the stream. */
static volatile bool s_mic_usb_priming;
static volatile uint8_t s_mic_in_buffer_front;
static volatile uint8_t s_mic_in_buffer_rear;
static volatile uint32_t s_mic_sample_rate;
static volatile int32_t s_mic_volume_db;
static volatile bool s_mic_mute;
static volatile bool s_speaker_dma_error;
static volatile bool s_mic_dma_error;
static volatile bool s_mic_usb_error;

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static struct usbd_interface intf2;

static uint8_t s_busid;

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

static struct audio_entity_info audio_entity_table[] = {
    {
        .bEntityId = AUDIO_OUT_CLOCK_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_CLOCK_SOURCE,
        .ep = AUDIO_OUT_EP
    },
    {
        .bEntityId = AUDIO_OUT_FU_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
        .ep = AUDIO_OUT_EP
    },
    {
        .bEntityId = AUDIO_IN_CLOCK_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_CLOCK_SOURCE,
        .ep = AUDIO_IN_EP
    },
    {
        .bEntityId = AUDIO_IN_FU_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
        .ep = AUDIO_IN_EP
    },
};

/* Static Functions Declaration */
static hpm_stat_t speaker_config_i2s_playback(uint32_t sample_rate);
static hpm_stat_t mic_config_i2s_recording(uint32_t sample_rate);
static hpm_stat_t speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static hpm_stat_t mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool speaker_out_buff_is_empty(void);
static uint32_t speaker_out_buff_get_used(void);
static bool mic_in_buff_is_empty(void);
static uint32_t mic_in_buff_get_used(void);
static uint8_t audio_ring_next(uint8_t index);

/* Extern Functions Definition */
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
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void audio_v2_init(uint8_t busid, uint32_t reg_base)
{
    s_busid = busid;

    usbd_desc_register(busid, &audio_v2_descriptor);
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf0, AUDIO_VERSION, audio_entity_table, 4));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf1, AUDIO_VERSION, audio_entity_table, 4));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf2, AUDIO_VERSION, audio_entity_table, 4));
    usbd_add_endpoint(busid, &audio_in_ep);
    usbd_add_endpoint(busid, &audio_out_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void speaker_init_i2s_dao(void)
{
    i2s_config_t i2s_config;
    dao_config_t dao_config;

    s_speaker_sample_rate = 16000;
    i2s_get_default_config(SPEAKER_I2S, &i2s_config);
    if (i2s_init(SPEAKER_I2S, &i2s_config) != status_success) {
        USB_LOG_ERR("speaker i2s_init failed\r\n");
        while (1) {
        }
    }
    s_speaker_i2s_mclk_hz = clock_get_frequency(SPEAKER_I2S_CLK_NAME);

    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);
}

void mic_init_i2s_pdm(void)
{
    i2s_config_t i2s_config;
    pdm_config_t pdm_config;

    i2s_get_default_config(MIC_I2S, &i2s_config);
    if (i2s_init(MIC_I2S, &i2s_config) != status_success) {
        USB_LOG_ERR("i2s_init failed\r\n");
        while (1) {
        }
    }

    s_mic_sample_rate = MIC_SAMPLE_FREQ;

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);
}

void i2s_enable_dma_irq_with_priority(int32_t priority)
{
    i2s_enable_tx_dma_request(SPEAKER_I2S);
    i2s_enable_rx_dma_request(MIC_I2S);
    dmamux_config(BOARD_APP_DMAMUX, SPEAKER_DMAMUX_CHANNEL, SPEAKER_I2S_TX_DMAMUX_SRC, true);
    dmamux_config(BOARD_APP_DMAMUX, MIC_DMAMUX_CHANNEL, MIC_I2S_RX_DMAMUX_SRC, true);

    intc_m_enable_irq_with_priority(BOARD_APP_DMA1_IRQ, priority);
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_DMA1_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL);
    mic_status = dma_check_transfer_status(BOARD_APP_DMA1, MIC_DMA_CHANNEL);

    if ((speaker_status & DMA_CHANNEL_STATUS_TC) != 0U) {
        if (s_speaker_rx_flag) {
            if (!speaker_out_buff_is_empty()) {
                uint8_t front = audio_ring_next(s_speaker_out_buffer_front);
                s_speaker_out_buffer_front = front;
                if (speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[front][0],
                                                   s_speaker_out_buffer_size[front]) != status_success) {
                    s_speaker_dma_transfer_req = true;
                    s_speaker_dma_error = true;
                }
            } else {
                s_speaker_dma_transfer_req = true;
            }
        }
    }

    if (s_mic_tx_flag && ((mic_status & DMA_CHANNEL_STATUS_TC) != 0U)) {
        uint8_t rear = s_mic_in_buffer_rear;
        uint8_t next_rear = audio_ring_next(rear);

        if (next_rear != s_mic_in_buffer_front) {
            rear = next_rear;
            s_mic_in_buffer_rear = rear;
        }

        if (mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[rear][0],
                                       s_audio_in_packet_size) != status_success) {
            s_mic_dma_error = true;
        }

        /* During startup, wait for a half-ring; after startup, send whenever
         * USB is waiting and at least one capture packet is ready. */
        if ((!s_mic_usb_priming || (mic_in_buff_get_used() >= AUDIO_PRIME_PACKET_COUNT))
            && s_mic_usb_transfer_req && !mic_in_buff_is_empty()) {
            s_mic_usb_priming = false;
            s_mic_usb_transfer_req = false;
            if (usbd_ep_start_write(s_busid, AUDIO_IN_EP,
                                    &s_mic_in_buffer[s_mic_in_buffer_front][0],
                                    s_audio_in_packet_size) != 0) {
                s_mic_usb_transfer_req = true;
                s_mic_usb_error = true;
            }
        }
    }
}

void audio_v2_task(uint8_t busid)
{
    (void)busid;

    if (s_speaker_dma_error) {
        s_speaker_dma_error = false;
        USB_LOG_ERR("speaker DMA setup failed\r\n");
    }
    if (s_mic_dma_error) {
        s_mic_dma_error = false;
        USB_LOG_ERR("mic DMA setup failed\r\n");
    }
    if (s_mic_usb_error) {
        s_mic_usb_error = false;
        USB_LOG_ERR("mic USB IN start failed\r\n");
    }
}

/* All function started with "USB_Audio_" needs attention to whether the running
 * environment is in the thread or interrupt environment */
void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    uint32_t packets_per_sec;
    hpm_stat_t state;

    /* Determine packets/sec from USB speed */
    if (usbd_get_port_speed(busid) == USB_SPEED_HIGH) {
        packets_per_sec = MIC_SPEAKER_HS_PACKETS_PER_SEC;
    } else {
        packets_per_sec = MIC_SPEAKER_FS_PACKETS_PER_SEC;
    }

    if (intf == 1) {
        /* OUT: use max buffer size; USB stack reports actual received bytes via nbytes */
        s_audio_out_packet_size = (packets_per_sec == MIC_SPEAKER_HS_PACKETS_PER_SEC) ? AUDIO_OUT_PACKET_HS : AUDIO_OUT_PACKET_FS;
        /* Start each playback session with an empty packet ring. */
        s_speaker_out_buffer_front = 0;
        s_speaker_out_buffer_rear = 0;
        s_speaker_dma_transfer_req = true;
        s_speaker_dma_priming = true;
        s_speaker_dma_error = false;
        i2s_reset_tx(SPEAKER_I2S);
        state = speaker_config_i2s_playback(s_speaker_sample_rate);
        if (state != status_success) {
            USB_LOG_ERR("SPEAKER I2S config failed\r\n");
            return;
        }
        s_speaker_rx_flag = true;
        usbd_ep_start_read(busid, AUDIO_OUT_EP,
                           &s_speaker_out_buffer[s_speaker_out_buffer_rear][0],
                           s_audio_out_packet_size);
        if (s_speaker_mute) {
            dao_stop(HPM_DAO);
        } else {
            dao_start(HPM_DAO);
        }
        i2s_start(SPEAKER_I2S);
        USB_LOG_RAW("OPEN SPEAKER, sample rate: %lu Hz\r\n", (unsigned long)s_speaker_sample_rate);
    } else {
        /* IN: compute exact size from actual mic sample rate and USB speed */
        s_audio_in_packet_size = (s_mic_sample_rate * AUDIO_IN_FRAME_SIZE) / packets_per_sec;
        i2s_reset_rx(MIC_I2S);
        state = mic_config_i2s_recording(s_mic_sample_rate);
        if (state != status_success) {
            USB_LOG_ERR("MIC I2S config failed\r\n");
            return;
        }
        s_mic_tx_flag = true;
        s_mic_usb_transfer_req = true;
        s_mic_usb_priming = true;
        s_mic_dma_error = false;
        s_mic_usb_error = false;
        s_mic_in_buffer_front = 0;
        s_mic_in_buffer_rear = 0;
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
        if (mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[0][0],
                                       s_audio_in_packet_size) != status_success) {
            s_mic_dma_error = true;
        }
        i2s_start(MIC_I2S);
        USB_LOG_RAW("OPEN MIC, sample rate: %lu Hz\r\n", (unsigned long)s_mic_sample_rate);
    }
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

    if (intf == 1) {
        s_speaker_rx_flag = false;
        s_speaker_dma_transfer_req = false;
        dma_abort_channel(BOARD_APP_DMA1, 1u << SPEAKER_DMA_CHANNEL);
        i2s_reset_tx(SPEAKER_I2S);
        i2s_stop(SPEAKER_I2S);
        dao_stop(HPM_DAO);
        USB_LOG_RAW("CLOSE SPEAKER\r\n");
    } else {
        s_mic_tx_flag = false;
        s_mic_usb_transfer_req = false;
        dma_abort_channel(BOARD_APP_DMA1, 1u << MIC_DMA_CHANNEL);
        i2s_reset_rx(MIC_I2S);
        i2s_stop(MIC_I2S);
        pdm_stop(HPM_PDM);
        USB_LOG_RAW("CLOSE MIC\r\n");
    }
}

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_volume_db = volume;
        /* Do Nothing */
    } else if (ep == AUDIO_IN_EP) {
        s_mic_volume_db = volume;
        /* Do Nothing */
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
        if (s_speaker_mute) {
            dao_stop(HPM_DAO);
        } else {
            dao_start(HPM_DAO);
        }
    } else if (ep == AUDIO_IN_EP) {
        s_mic_mute = mute;
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
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

    if (ep == AUDIO_OUT_EP) {
        s_speaker_sample_rate = sampling_freq;
    } else if (ep == AUDIO_IN_EP) {
        s_mic_sample_rate = sampling_freq;
    } else {
        ;
    }
}

uint32_t usbd_audio_get_sampling_freq(uint8_t busid, uint8_t ep)
{
    (void)busid;

    uint32_t freq = 0;

    if (ep == AUDIO_OUT_EP) {
        freq = s_speaker_sample_rate;
    } else if (ep == AUDIO_IN_EP) {
        freq = s_mic_sample_rate;
    } else {
        ;
    }

    return freq;
}

void usbd_audio_get_sampling_freq_table(uint8_t busid, uint8_t ep, uint8_t **sampling_freq_table)
{
    (void)busid;

    if (ep == AUDIO_OUT_EP) {
        if (usbd_get_port_speed(busid) == USB_SPEED_HIGH) {
            *sampling_freq_table = (uint8_t *)speaker_default_sampling_freq_table_hs;
        } else {
            *sampling_freq_table = (uint8_t *)speaker_default_sampling_freq_table_fs;
        }
    } else if (ep == AUDIO_IN_EP) {
        *sampling_freq_table = (uint8_t *)mic_default_sampling_freq_table;
    } else {
        ;
    }
}

/* Static Function Definition */
static void usbd_audio_iso_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if (s_speaker_rx_flag) {
        uint8_t rear = s_speaker_out_buffer_rear;
        uint8_t next_rear = audio_ring_next(rear);

        /* Keep one slot unused to distinguish full from empty. If full, drop
         * the newest packet; never move the consumer pointer from USB context. */
        if ((nbytes > 0U) && (nbytes <= AUDIO_OUT_PACKET_MAX)
            && ((nbytes % AUDIO_OUT_FRAME_SIZE) == 0U)
            && (next_rear != s_speaker_out_buffer_front)) {
            s_speaker_out_buffer_size[rear] = nbytes;
            s_speaker_out_buffer_rear = next_rear;

            uint32_t start_threshold = s_speaker_dma_priming ? AUDIO_PRIME_PACKET_COUNT : 1U;
            if (s_speaker_dma_transfer_req && (speaker_out_buff_get_used() >= start_threshold)) {
                uint8_t front = s_speaker_out_buffer_front;
                s_speaker_dma_transfer_req = false;
                if (speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[front][0],
                                                   s_speaker_out_buffer_size[front]) != status_success) {
                    s_speaker_dma_transfer_req = true;
                    s_speaker_dma_error = true;
                } else {
                    s_speaker_dma_priming = false;
                }
            }
        }
        /* A full ring reuses the just-completed slot and drops new packets
         * until DMA advances front. */
        usbd_ep_start_read(busid, ep,
                           &s_speaker_out_buffer[s_speaker_out_buffer_rear][0],
                           s_audio_out_packet_size);
    }
}

static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;

    if (s_mic_tx_flag) {
        s_mic_in_buffer_front = audio_ring_next(s_mic_in_buffer_front);
        if (!mic_in_buff_is_empty()) {
            if (usbd_ep_start_write(busid, AUDIO_IN_EP,
                                    &s_mic_in_buffer[s_mic_in_buffer_front][0],
                                    s_audio_in_packet_size) != 0) {
                s_mic_usb_transfer_req = true;
                s_mic_usb_error = true;
            }
        } else {
            /* FIFO is empty, defer USB IN kick-off to DMA ISR. */
            s_mic_usb_transfer_req = true;
        }
    }
}

static hpm_stat_t speaker_config_i2s_playback(uint32_t sample_rate)
{
    i2s_transfer_config_t transfer;

    if (OUT_CHANNEL_NUM > 2) {
        return status_invalid_argument; /* Currently not support TDM mode */
    }

    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.data_line = SPEAKER_I2S_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = SPEAKER_AUDIO_DEPTH;
    transfer.channel_num_per_frame = 2; /* non TDM mode, channel num fix to 2. */
    transfer.channel_slot_mask = 0x3;   /* 2 channels */

    s_speaker_i2s_mclk_hz = clock_get_frequency(SPEAKER_I2S_CLK_NAME);

    if (status_success != i2s_config_tx(SPEAKER_I2S, s_speaker_i2s_mclk_hz, &transfer)) {
        return status_fail;
    }
    return status_success;
}

static hpm_stat_t mic_config_i2s_recording(uint32_t sample_rate)
{
    i2s_transfer_config_t transfer;
    uint32_t i2s_mclk_hz;

    i2s_get_default_transfer_config_for_pdm(&transfer);
    transfer.data_line = MIC_I2S_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.channel_slot_mask = BOARD_PDM_DUAL_CHANNEL_MASK;
    i2s_mclk_hz = clock_get_frequency(MIC_I2S_CLK_NAME);

    return i2s_config_rx(MIC_I2S, i2s_mclk_hz, &transfer);
}

static hpm_stat_t speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };
    hpm_stat_t status;

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&SPEAKER_I2S->TXD[SPEAKER_I2S_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    status = dma_setup_channel(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL, &ch_config, true);
    if (status == status_success) {
        i2s_enable_tx(SPEAKER_I2S, 1U << SPEAKER_I2S_DATA_LINE);
    }
    return status;
}

static hpm_stat_t mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };
    hpm_stat_t status;

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = (uint32_t)(&MIC_I2S->RXD[MIC_I2S_DATA_LINE]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    status = dma_setup_channel(BOARD_APP_DMA1, MIC_DMA_CHANNEL, &ch_config, true);
    if (status == status_success) {
        i2s_enable_rx(MIC_I2S, 1U << MIC_I2S_DATA_LINE);
    }
    return status;
}

static bool speaker_out_buff_is_empty(void)
{
    bool empty = false;
    uint32_t front = s_speaker_out_buffer_front;  /* defined the order of volatile accesses */

    if (front == s_speaker_out_buffer_rear) {
        empty = true;
    }

    return empty;
}

static uint32_t speaker_out_buff_get_used(void)
{
    uint32_t front = s_speaker_out_buffer_front;
    uint32_t rear = s_speaker_out_buffer_rear;

    if (rear >= front) {
        return rear - front;
    }

    return AUDIO_BUFFER_COUNT + rear - front;
}

static bool mic_in_buff_is_empty(void)
{
    bool empty = false;
    uint32_t front = s_mic_in_buffer_front;  /* defined the order of volatile accesses */

    if (front == s_mic_in_buffer_rear) {
        empty = true;
    }

    return empty;
}

static uint32_t mic_in_buff_get_used(void)
{
    uint32_t front = s_mic_in_buffer_front;
    uint32_t rear = s_mic_in_buffer_rear;

    return (rear >= front) ? (rear - front) : (AUDIO_BUFFER_COUNT + rear - front);
}

static uint8_t audio_ring_next(uint8_t index)
{
    index++;
    return (index < AUDIO_BUFFER_COUNT) ? index : 0U;
}
