/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "usb_osal.h"
#include "event_groups.h"
#include "assert.h"

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

#define SPEAKER_SET_SAMPLING_FREQ_REQ   0x01
#define SPEAKER_SET_MUTE_ENABLE_REQ     0x02
#define SPEAKER_SET_MUTE_DISABLE_REQ    0x03
#define SPEAKER_SET_VOLUME_REQ          0x04
#define SPEAKER_START_REQ               0x05
#define SPEAKER_STOP_REQ                0x06

#define MIC_SET_ENABLE_REQ     0x01
#define MIC_SET_DISABLE_REQ    0x02
#define MIC_SET_MUTE_ENABLE_REQ     0x03
#define MIC_SET_MUTE_DISABLE_REQ    0x04

#define EP_INTERVAL_HS 0x02
#define EP_INTERVAL_FS 0x01

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
#define MIC_SLOT_BYTE_SIZE 2
#define MIC_AUDIO_DEPTH    16

#define BMCONTROL (AUDIO_V2_FU_CONTROL_MUTE | AUDIO_V2_FU_CONTROL_VOLUME)

#define IN_CHANNEL_NUM 2
#if IN_CHANNEL_NUM == 1
#define INPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define INPUT_CH_ENABLE 0x00000000
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

/* AudioFreq * DataSize * NumChannels */
#define AUDIO_OUT_PACKET_HS ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ_HS * SPEAKER_SLOT_BYTE_SIZE * OUT_CHANNEL_NUM) / 4000))
#define AUDIO_OUT_PACKET_FS ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ_FS * SPEAKER_SLOT_BYTE_SIZE * OUT_CHANNEL_NUM) / 1000))
#define AUDIO_IN_PACKET_HS  ((uint32_t)((MIC_SAMPLE_FREQ * MIC_SLOT_BYTE_SIZE * IN_CHANNEL_NUM) / 4000))
#define AUDIO_IN_PACKET_FS  ((uint32_t)((MIC_SAMPLE_FREQ * MIC_SLOT_BYTE_SIZE * IN_CHANNEL_NUM) / 1000))

#define USB_AUDIO_CONFIG_DESC_SIZ (9 +                                                     \
                                   AUDIO_V2_AC_DESCRIPTOR_INIT_LEN +                       \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM) +  \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_AS_DESCRIPTOR_INIT_LEN +                       \
                                   AUDIO_V2_AS_DESCRIPTOR_INIT_LEN)

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
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, (AUDIO_OUT_PACKET_HS + 4), EP_INTERVAL_HS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, (AUDIO_IN_PACKET_HS + 4), EP_INTERVAL_HS),
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
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, (AUDIO_OUT_PACKET_FS + 4), EP_INTERVAL_FS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, (AUDIO_IN_PACKET_FS + 4), EP_INTERVAL_FS),
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
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, (AUDIO_OUT_PACKET_FS + 4), EP_INTERVAL_FS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, (AUDIO_IN_PACKET_FS + 4), EP_INTERVAL_FS),
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
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, (AUDIO_OUT_PACKET_HS + 4), EP_INTERVAL_HS),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, (AUDIO_IN_PACKET_HS + 4), EP_INTERVAL_HS),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro UAC V2 DEMO",        /* Product */
    "2024061703",                 /* Serial Number */
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
        s_audio_out_packet_size = AUDIO_OUT_PACKET_HS;
        s_audio_in_packet_size = AUDIO_IN_PACKET_HS;
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        s_audio_out_packet_size = AUDIO_OUT_PACKET_FS;
        s_audio_in_packet_size = AUDIO_IN_PACKET_FS;
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
#define SPEAKER_DMAMUX_CHANNEL    DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL)
#define MIC_DMAMUX_CHANNEL        DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_XDMA, MIC_DMA_CHANNEL)

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
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_out_buffer[AUDIO_BUFFER_COUNT][AUDIO_OUT_PACKET_FS];    /* buffer size: max(AUDIO_OUT_PACKET_HS, AUDIO_OUT_PACKET_FS) */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_mic_in_buffer[AUDIO_BUFFER_COUNT][AUDIO_IN_PACKET_FS];
static uint32_t s_speaker_out_buffer_size[AUDIO_BUFFER_COUNT];
static volatile uint32_t s_speaker_i2s_mclk_hz;
static volatile bool s_speaker_rx_flag;
static volatile uint8_t s_speaker_out_buffer_rd;
static volatile uint8_t s_speaker_out_buffer_wr;
static volatile uint32_t s_speaker_sample_rate;
static volatile bool s_speaker_mute;
static volatile int32_t s_speaker_volume_percent;
static volatile uint8_t s_mic_in_buffer_rd;
static volatile uint8_t s_mic_in_buffer_wr;
static volatile uint32_t s_mic_sample_rate;
static volatile int32_t s_mic_volume_percent;
static volatile bool s_mic_mute;
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

/* These signals are provided by all rtos, so the OSAL interface is used */
usb_osal_sem_t binsema_dma_speaker_done; /* When this semaphore is released, it means that the speaker data can be sent to the speaker. */
usb_osal_sem_t binsema_tx_resource; /* When this semaphore is released, it means that the mic data can be sent to the host*/
usb_osal_thread_t task_handle_speaker_play;
usb_osal_thread_t task_handle_mic_play;

/* We want to define a counting semaphore to achieve synchronization
 * between producers and consumers. There are not suitable interfaces
 * in cherryusb's OSAL interface, so we use freeRTOS's interface here.
 * Change them if you are porting the sample into another rtos.
 * These count semaphore represents the size of the data in the buffer */
SemaphoreHandle_t countsema_speaker_data, countsema_mic_data;
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define EVENT_BIT_SPEAKER_ENABLE    BIT_0
#define EVENT_BIT_MIC_ENABLE        BIT_1
#define EVENT_BIT_PDM_STOP          BIT_2

/* The event group is used to simplify event handling, This can be replaced by semaphores, but many semaphores need to be defined.
 * You can also use global flag variables instead as long as you like. */
EventGroupHandle_t event_group;

/* Static Functions Declaration */
static hpm_stat_t speaker_init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool speaker_out_buff_is_empty(void);
static bool mic_in_buff_is_empty(void);

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
    binsema_dma_speaker_done = usb_osal_sem_create(0);
    assert(binsema_dma_speaker_done != NULL);
    binsema_tx_resource = usb_osal_sem_create(0);
    assert(binsema_tx_resource);
    event_group = xEventGroupCreate();
    assert(event_group != NULL);

    countsema_speaker_data = xSemaphoreCreateCounting(10, 0);
    assert(countsema_speaker_data != NULL);
    countsema_mic_data = xSemaphoreCreateCounting(10, 0);
    assert(countsema_mic_data != NULL);

    s_busid = busid;

    usbd_desc_register(busid, &audio_v2_descriptor);
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf0, AUDIO_VERSION, audio_entity_table, 4));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf1, AUDIO_VERSION, audio_entity_table, 4));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf2, AUDIO_VERSION, audio_entity_table, 4));
    usbd_add_endpoint(busid, &audio_in_ep);
    usbd_add_endpoint(busid, &audio_out_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void speaker_init_i2s_dao_codec(void)
{
    dao_config_t dao_config;

    s_speaker_sample_rate = 16000;
    (void)speaker_init_i2s_playback(s_speaker_sample_rate, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);

    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);
}

void mic_init_i2s_pdm(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    pdm_config_t pdm_config;
    uint32_t i2s_mclk_hz;

    i2s_get_default_config(MIC_I2S, &i2s_config);
    i2s_init(MIC_I2S, &i2s_config);

    i2s_get_default_transfer_config_for_pdm(&transfer);
    transfer.data_line = MIC_I2S_DATA_LINE;
    transfer.sample_rate = MIC_SAMPLE_FREQ;
    transfer.channel_slot_mask = BOARD_PDM_DUAL_CHANNEL_MASK; /* 2 channels */

    s_mic_sample_rate = transfer.sample_rate;

    i2s_mclk_hz = clock_get_frequency(MIC_I2S_CLK_NAME);

    if (status_success != i2s_config_rx(MIC_I2S, i2s_mclk_hz, &transfer)) {
        USB_LOG_ERR("MIC I2S1 config failed\r\n");
        while (1) {
            ;
        }
    }
    i2s_start(MIC_I2S);

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);
}

void i2s_enable_dma_irq_with_priority(int32_t priority)
{
    i2s_enable_tx_dma_request(SPEAKER_I2S);
    i2s_enable_rx_dma_request(MIC_I2S);
    dmamux_config(BOARD_APP_DMAMUX, SPEAKER_DMAMUX_CHANNEL, SPEAKER_I2S_TX_DMAMUX_SRC, true);
    dmamux_config(BOARD_APP_DMAMUX, MIC_DMAMUX_CHANNEL, MIC_I2S_RX_DMAMUX_SRC, true);

    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, priority);
}

static inline void increase_buffer_write_pointer(volatile uint8_t *p_wr, volatile uint8_t *p_rd, uint8_t max)
{
    (*p_wr)++;
    (*p_wr) %= max;
    if (*p_wr == *p_rd) {
        (*p_rd)++;
        (*p_rd) %= max;
    }
}

static inline void increase_buffer_read_pointer(volatile uint8_t *p_rd, volatile uint8_t max)
{
    (*p_rd)++;
    (*p_rd) %= max;
}

void check_and_restart_mic_dma_from_isr(EventBits_t event)
{
    increase_buffer_write_pointer(&s_mic_in_buffer_wr, &s_mic_in_buffer_rd, AUDIO_BUFFER_COUNT);
    /* PDM stop event is not set, restart dma transfer */
    if ((event & EVENT_BIT_PDM_STOP) == 0) {
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_wr][0], s_audio_in_packet_size);
    } else {
        xEventGroupClearBitsFromISR(event_group, EVENT_BIT_PDM_STOP);
    }
}

bool check_and_restart_speaker_dma_from_isr(EventBits_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* stop event is not set, try take resource and restart dma */
    if ((event & EVENT_BIT_SPEAKER_ENABLE) != 0) {
        if (xSemaphoreTakeFromISR(countsema_speaker_data, &xHigherPriorityTaskWoken) == pdTRUE) {
            speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[s_speaker_out_buffer_rd][0],
                s_speaker_out_buffer_size[s_speaker_out_buffer_rd]);
            increase_buffer_read_pointer(&s_speaker_out_buffer_rd, AUDIO_BUFFER_COUNT);
            return true;
        }
    }
    return false;
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;
    speaker_status = dma_check_transfer_status(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL);
    mic_status = dma_check_transfer_status(BOARD_APP_XDMA, MIC_DMA_CHANNEL);

    EventBits_t event = 0;
    event = xEventGroupGetBitsFromISR(event_group);
    if (0 != (speaker_status & DMA_CHANNEL_STATUS_TC)) {
        /* true: restarted  false: not restarted, tell task to loop for data from host */
        if (!check_and_restart_speaker_dma_from_isr(event)) {
            usb_osal_sem_give(binsema_dma_speaker_done);
        }
    }

    if (0 != (mic_status & DMA_CHANNEL_STATUS_TC)) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        check_and_restart_mic_dma_from_isr(event);
        int ret = xSemaphoreGiveFromISR(countsema_mic_data, &xHigherPriorityTaskWoken);
        if (ret == pdPASS) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void request_handler_speaker_set_sampling_freq(void)
{
    hpm_stat_t state;
    state = speaker_init_i2s_playback(s_speaker_sample_rate, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);
    if (state == status_success) {
        USB_LOG_INFO("Init I2S Clock Ok! Sample Rate: %d, speaker_i2s_mclk_hz: %d\r\n", s_speaker_sample_rate, s_speaker_i2s_mclk_hz);
    } else {
        USB_LOG_INFO("Init I2S Clock Fail!\r\n");
    }
    s_speaker_out_buffer_rd = s_speaker_out_buffer_wr;

}

void task_speaker_play(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t event = 0;
    while (1) {
        /* wait host open speaker */
        while (1) {
            event = xEventGroupWaitBits(event_group, EVENT_BIT_SPEAKER_ENABLE, pdFALSE, 0, portMAX_DELAY);
            if ((event & EVENT_BIT_SPEAKER_ENABLE) != 0) {
                if (s_speaker_mute) {
                    dao_stop(HPM_DAO);
                } else {
                    dao_start(HPM_DAO);
                }
                USB_LOG_INFO("%s get start cmd\r\n", __func__);
                break;
            }
        }
        /* check speaker data is valid and start i2s dma transfer */
        while (1) {
            /* data valid!!! */
            if (xSemaphoreTake(countsema_speaker_data, portMAX_DELAY) != pdTRUE) {
                USB_LOG_ERR("error while take speaker resources %d\r\n", __LINE__);
                goto check_stop_cmd1;
            }
            while (1) {
                if (!speaker_out_buff_is_empty()) {
                    speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[s_speaker_out_buffer_rd][0],
                                                s_speaker_out_buffer_size[s_speaker_out_buffer_rd]);
                    increase_buffer_read_pointer(&s_speaker_out_buffer_rd, AUDIO_BUFFER_COUNT);
                    break;
                }
                vTaskDelay(1);
            }
            /* Wait dma done. If there is data valid when dma isr occur, dma transfer will restart in isr handler.
             * If there is a stop cmd from host or there is no data valid, sem will be given and we can take it here. */
            if (usb_osal_sem_take(binsema_dma_speaker_done, portMAX_DELAY) != 0) {
                assert(0);
            }
check_stop_cmd1:
            event = xEventGroupGetBits(event_group);
            if ((event & EVENT_BIT_SPEAKER_ENABLE) == 0) {
                dao_stop(HPM_DAO);
                break;
            }
        }
    }
}

void task_mic_play(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t event = 0;
    while (1) {
        /* wait host open mic */
        while (1) {
            event = xEventGroupWaitBits(event_group, EVENT_BIT_MIC_ENABLE, pdFALSE, 0, portMAX_DELAY);
            if ((event & EVENT_BIT_MIC_ENABLE) != 0) {
                if (s_mic_mute) {
                    pdm_stop(HPM_PDM);
                } else {
                    pdm_start(HPM_PDM);
                }
                mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_wr][0], s_audio_in_packet_size);
                USB_LOG_INFO("%s mic get start cmd\n", __func__);
                break;
            }
        }
        /* check weather data is valid and start usb ep in transfer */
        while (1) {
            /* data valid!!! */
            if (xSemaphoreTake(countsema_mic_data, 100) != pdTRUE) {
                USB_LOG_ERR("error while take mic data %d\n", __LINE__);
                goto check_stop_cmd2;
            }
            while (1) {
                if (!mic_in_buff_is_empty()) {
                    if (usb_osal_sem_take(binsema_tx_resource, 100) == 0) {
                        usbd_ep_start_write(s_busid, AUDIO_IN_EP, &s_mic_in_buffer[s_mic_in_buffer_rd][0], s_audio_in_packet_size);
                        increase_buffer_read_pointer(&s_mic_in_buffer_rd, AUDIO_BUFFER_COUNT);
                        break;
                    }
                }
                vTaskDelay(1);
            }
            /* stop if mic is disabled */
check_stop_cmd2:
            event = xEventGroupGetBits(event_group);
            if ((event & EVENT_BIT_MIC_ENABLE) == 0) {
                xEventGroupSetBits(event_group, EVENT_BIT_PDM_STOP);
                /* wait current dma operation done */
                while (xEventGroupGetBits(event_group) & EVENT_BIT_PDM_STOP) {
                    vTaskDelay(10);
                }
                pdm_stop(HPM_PDM);
                break;
            }
        }

    }
}

/* All function started with "USB_Audio_" needs attention to whether the running
 * environment is in the thread or interrupt environment */
void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    if (intf == 1) {
        s_speaker_rx_flag = 1;
        usbd_ep_start_read(busid, AUDIO_OUT_EP, (uint8_t *)&s_speaker_out_buffer[s_speaker_out_buffer_wr][0], s_audio_out_packet_size);

        if (xPortIsInsideInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            uint32_t ret = xEventGroupSetBitsFromISR(event_group, EVENT_BIT_SPEAKER_ENABLE, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } else {
            xEventGroupSetBits(event_group, EVENT_BIT_SPEAKER_ENABLE);
        }
        USB_LOG_INFO("OPEN SPEAKER\r\n");
    } else {
        usb_osal_sem_give(binsema_tx_resource);
        USB_LOG_INFO("OPEN MIC\r\n");
        if (xPortIsInsideInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            uint32_t ret = xEventGroupSetBitsFromISR(event_group, EVENT_BIT_MIC_ENABLE, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } else {
            xEventGroupSetBits(event_group, EVENT_BIT_MIC_ENABLE);
        }
    }
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

    if (intf == 1) {
        s_speaker_rx_flag = 0;
        if (xPortIsInsideInterrupt()) {
            xEventGroupClearBitsFromISR(event_group, EVENT_BIT_SPEAKER_ENABLE);
        } else {
            xEventGroupClearBits(event_group, EVENT_BIT_SPEAKER_ENABLE);
        }

        USB_LOG_INFO("CLOSE SPEAKER\r\n");
    } else {
        if (xPortIsInsideInterrupt()) {
            xEventGroupClearBitsFromISR(event_group, EVENT_BIT_MIC_ENABLE);
        } else {
            xEventGroupClearBits(event_group, EVENT_BIT_MIC_ENABLE);
        }

        USB_LOG_INFO("CLOSE MIC\r\n");
    }
}

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_volume_percent = volume;
    } else if (ep == AUDIO_IN_EP) {
        s_mic_volume_percent = volume;
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
        volume = s_speaker_volume_percent;
    } else if (ep == AUDIO_IN_EP) {
        volume = s_mic_volume_percent;
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
        if (!s_mic_mute) {
            if (xPortIsInsideInterrupt()) {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                uint32_t ret = xEventGroupSetBitsFromISR(event_group, EVENT_BIT_MIC_ENABLE, &xHigherPriorityTaskWoken);
                if (ret == pdPASS) {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }

            } else {
                xEventGroupSetBits(event_group, EVENT_BIT_MIC_ENABLE);
            }
        } else {
            if (xPortIsInsideInterrupt()) {
                xEventGroupClearBitsFromISR(event_group, EVENT_BIT_MIC_ENABLE);
            } else {
                xEventGroupClearBits(event_group, EVENT_BIT_MIC_ENABLE);
            }
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
        request_handler_speaker_set_sampling_freq();
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
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    s_speaker_out_buffer_size[s_speaker_out_buffer_wr] = nbytes;
    increase_buffer_write_pointer(&s_speaker_out_buffer_wr, &s_speaker_out_buffer_rd, AUDIO_BUFFER_COUNT);
    /* Prepare buffer to receive speaker data only when audio is open */
    if (s_speaker_rx_flag) {
        usbd_ep_start_read(busid, ep, &s_speaker_out_buffer[s_speaker_out_buffer_wr][0], s_audio_out_packet_size);
    }
    if (xPortIsInsideInterrupt()) {
        int ret = xSemaphoreGiveFromISR(countsema_speaker_data, &xHigherPriorityTaskWoken);
        if (ret == pdPASS) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        (void)xSemaphoreGive(countsema_speaker_data);
    }
}

static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    usb_osal_sem_give(binsema_tx_resource);
}

static hpm_stat_t speaker_init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    if (channel_num > 2) {
        return status_invalid_argument; /* Currently not support TDM mode */
    }

    i2s_get_default_config(SPEAKER_I2S, &i2s_config);
    i2s_init(SPEAKER_I2S, &i2s_config);

    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.data_line = SPEAKER_I2S_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.channel_num_per_frame = 2; /* non TDM mode, channel num fix to 2. */
    transfer.channel_slot_mask = 0x3;   /* 2 channels */

    s_speaker_i2s_mclk_hz = clock_get_frequency(SPEAKER_I2S_CLK_NAME);

    if (status_success != i2s_config_tx(SPEAKER_I2S, s_speaker_i2s_mclk_hz, &transfer)) {
        return status_fail;
    }
    i2s_start(SPEAKER_I2S);

    return status_success;
}

static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_XDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&SPEAKER_I2S->TXD[SPEAKER_I2S_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = 0;

    if (status_success != dma_setup_channel(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
    }
}

static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_XDMA, &ch_config);
    ch_config.src_addr = (uint32_t)(&MIC_I2S->RXD[MIC_I2S_DATA_LINE]) + 2u;
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = DMA_ALIGN_HALF_WORD(size);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

    if (status_success != dma_setup_channel(BOARD_APP_XDMA, MIC_DMA_CHANNEL, &ch_config, true)) {
        printf(" pdm dma setup channel failed\n");
    }
}

static bool speaker_out_buff_is_empty(void)
{
    bool empty = false;

    if (s_speaker_out_buffer_rd == s_speaker_out_buffer_wr) {
        empty = true;
    }

    return empty;
}

static bool mic_in_buff_is_empty(void)
{
    bool empty = false;

    if (s_mic_in_buffer_rd == s_mic_in_buffer_wr) {
        empty = true;
    }

    return empty;
}
