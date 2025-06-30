/*
 * Copyright (c) 2022-2025 HPMicro
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
    "2024061702",                 /* Serial Number */
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
static volatile uint8_t s_speaker_out_buffer_front;
static volatile uint8_t s_speaker_out_buffer_rear;
static volatile bool s_speaker_dma_transfer_req;
static volatile bool s_speaker_dma_transfer_done;
static volatile uint32_t s_speaker_sample_rate;
static volatile int32_t s_speaker_volume_percent;
static volatile bool s_speaker_mute;
static volatile bool s_mic_tx_flag;
static volatile bool s_mic_ep_tx_busy_flag;
static volatile uint8_t s_mic_in_buffer_front;
static volatile uint8_t s_mic_in_buffer_rear;
static volatile bool s_mic_dma_transfer_done;
static volatile uint32_t s_mic_sample_rate;
static volatile int32_t s_mic_volume_percent;
static volatile bool s_mic_mute;

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
        printf("MIC I2S1 config failed\n");
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

SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL);
    mic_status = dma_check_transfer_status(BOARD_APP_XDMA, MIC_DMA_CHANNEL);

    if (0 != (speaker_status & DMA_CHANNEL_STATUS_TC)) {
        s_speaker_dma_transfer_done = true;
    }

    if (0 != (mic_status & DMA_CHANNEL_STATUS_TC)) {
        s_mic_dma_transfer_done = true;
    }
}

void audio_v2_task(uint8_t busid)
{
    if (s_speaker_rx_flag) {
        if (!speaker_out_buff_is_empty()) {
            if (s_speaker_dma_transfer_req) {
                s_speaker_dma_transfer_req = false;
                speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[s_speaker_out_buffer_front][0],
                                               s_speaker_out_buffer_size[s_speaker_out_buffer_front]);

                s_speaker_out_buffer_front++;
                if (s_speaker_out_buffer_front >= AUDIO_BUFFER_COUNT) {
                    s_speaker_out_buffer_front = 0;
                }
            } else if (s_speaker_dma_transfer_done) {
                s_speaker_dma_transfer_done = false;
                speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[s_speaker_out_buffer_front][0],
                                               s_speaker_out_buffer_size[s_speaker_out_buffer_front]);
                s_speaker_out_buffer_front++;
                if (s_speaker_out_buffer_front >= AUDIO_BUFFER_COUNT) {
                    s_speaker_out_buffer_front = 0;
                }
            } else {
                ;    /* Do Nothing */
            }
        }
    }

    if (s_mic_tx_flag) {
        if (s_mic_dma_transfer_done) {
            s_mic_dma_transfer_done = false;
            s_mic_in_buffer_rear++;
            if (s_mic_in_buffer_rear >= AUDIO_BUFFER_COUNT) {
                s_mic_in_buffer_rear = 0;
            }
            mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_rear][0], s_audio_in_packet_size);
        }

        if (!mic_in_buff_is_empty()) {
            if (!s_mic_ep_tx_busy_flag) {
                s_mic_ep_tx_busy_flag = true;
                usbd_ep_start_write(busid, AUDIO_IN_EP, &s_mic_in_buffer[s_mic_in_buffer_front][0], s_audio_in_packet_size);
                s_mic_in_buffer_front++;
                if (s_mic_in_buffer_front >= AUDIO_BUFFER_COUNT) {
                    s_mic_in_buffer_front = 0;
                }
            }
        }
    }
}

void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    if (intf == 1) {
        s_speaker_rx_flag = 1;
        s_speaker_out_buffer_front = 0;
        s_speaker_out_buffer_rear = 0;
        s_speaker_dma_transfer_req = true;
        usbd_ep_start_read(busid, AUDIO_OUT_EP, (uint8_t *)&s_speaker_out_buffer[s_speaker_out_buffer_rear][0], s_audio_out_packet_size);
        if (s_speaker_mute) {
            dao_stop(HPM_DAO);
        } else {
            dao_start(HPM_DAO);
        }
        USB_LOG_RAW("OPEN SPEAKER\r\n");
    } else {
        s_mic_tx_flag = 1;
        s_mic_ep_tx_busy_flag = false;
        s_mic_in_buffer_front = 0;
        s_mic_in_buffer_rear = 0;
        s_mic_dma_transfer_done = false;
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_rear][0], s_audio_in_packet_size);
        USB_LOG_RAW("OPEN MIC\r\n");
    }
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

    if (intf == 1) {
        s_speaker_rx_flag = 0;
        dao_stop(HPM_DAO);
        USB_LOG_RAW("CLOSE SPEAKER\r\n");
    } else {
        s_mic_tx_flag = 0;
        pdm_stop(HPM_PDM);
        USB_LOG_RAW("CLOSE MIC\r\n");
    }
}

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_volume_percent = volume;
        /* Do Nothing */
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

    hpm_stat_t state;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_sample_rate = sampling_freq;
        state = speaker_init_i2s_playback(sampling_freq, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);
        if (state == status_success) {
            USB_LOG_RAW("Init I2S Clock Ok! Sample Rate: %d, speaker_i2s_mclk_hz: %d\r\n", sampling_freq, s_speaker_i2s_mclk_hz);
        } else {
            USB_LOG_RAW("Init I2S Clock Fail!\r\n");
        }
        s_speaker_out_buffer_front = s_speaker_out_buffer_rear;
        s_speaker_dma_transfer_req = true;
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
        s_speaker_out_buffer_size[s_speaker_out_buffer_rear] = nbytes;
        s_speaker_out_buffer_rear++;
        if (s_speaker_out_buffer_rear >= AUDIO_BUFFER_COUNT) {
            s_speaker_out_buffer_rear = 0;
        }
        usbd_ep_start_read(busid, ep, &s_speaker_out_buffer[s_speaker_out_buffer_rear][0], s_audio_out_packet_size);
    }
}

static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    s_mic_ep_tx_busy_flag = false;
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
        printf(" speaker dma setup channel failed\n");
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

    if (s_speaker_out_buffer_front == s_speaker_out_buffer_rear) {
        empty = true;
    }

    return empty;
}

static bool mic_in_buff_is_empty(void)
{
    bool empty = false;

    if (s_mic_in_buffer_front == s_mic_in_buffer_rear) {
        empty = true;
    }

    return empty;
}
