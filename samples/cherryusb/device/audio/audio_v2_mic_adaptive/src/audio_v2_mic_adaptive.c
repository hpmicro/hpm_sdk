/*
 * Copyright (c) 2022-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_audio.h"
#include "hpm_i2s_drv.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#if !defined(USING_CODEC) || !USING_CODEC
#ifdef HPMSOC_HAS_HPMSDK_PDMLITE
#include "hpm_pdmlite_drv.h"
#else
#include "hpm_pdm_drv.h"
#endif
#endif
#include "board.h"
#include "audio_v2_mic_adaptive.h"

#if defined(USING_CODEC) && USING_CODEC
#include "hpm_codec_common.h"
/* Static control structure for codec access across multiple functions */
static codec_control_t s_codec_control = {
    .ptr = CODEC_I2C,
    .slave_address = BOARD_AUDIO_CODEC_I2C_ADDR,
};
static volatile bool s_mic_codec_update_pending;
static volatile uint32_t s_mic_i2s_mclk_hz;
static bool s_i2s_invert_fclk_out;
#endif

#if defined(USING_CODEC) && USING_CODEC
#define EP_INTERVAL_HS 0x01  /* 125µs for low-latency 96kHz mic capture */
#else
#define EP_INTERVAL_HS 0x04  /* 1ms for 16kHz PDM */
#endif

#define EP_INTERVAL_FS 0x01

/* bInterval -> packets/sec: HS 8000 / 2^(bInterval-1), FS 1000 / 2^(bInterval-1) */
#define MIC_HS_MICROFRAMES_PER_PACKET (1U << (EP_INTERVAL_HS - 1U))
#define MIC_HS_PACKETS_PER_SEC        (8000U / MIC_HS_MICROFRAMES_PER_PACKET)
#define MIC_FS_FRAMES_PER_PACKET      (1U << (EP_INTERVAL_FS - 1U))
#define MIC_FS_PACKETS_PER_SEC        (1000U / MIC_FS_FRAMES_PER_PACKET)

#define AUDIO_VERSION 0x0200

#define AUDIO_IN_EP 0x81

#define AUDIO_IN_CLOCK_ID 0x01
#define AUDIO_IN_FU_ID    0x03

#if defined(USING_CODEC) && USING_CODEC
#define MIC_MAX_SAMPLE_FREQ 96000
#else
#define MIC_MAX_SAMPLE_FREQ 16000
#endif
#define MIC_SLOT_BYTE_SIZE 4
#define SAMPLE_BITS        24

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

/* Buffer geometry */
#define AUDIO_IN_FRAME_SIZE  (IN_CHANNEL_NUM * MIC_SLOT_BYTE_SIZE)

/* I2S DMA slot sizes: 1 audio frame less than descriptor packet */
#define AUDIO_IN_SLOT_HS     ((MIC_MAX_SAMPLE_FREQ * AUDIO_IN_FRAME_SIZE) / MIC_HS_PACKETS_PER_SEC)
#define AUDIO_IN_SLOT_FS     ((MIC_MAX_SAMPLE_FREQ * AUDIO_IN_FRAME_SIZE) / MIC_FS_PACKETS_PER_SEC)
#define AUDIO_IN_SLOT_MAX    ((AUDIO_IN_SLOT_HS) > (AUDIO_IN_SLOT_FS) ? (AUDIO_IN_SLOT_HS) : (AUDIO_IN_SLOT_FS))

/* USB descriptor packet sizes: slot + 1 extra audio frame */
#define AUDIO_IN_PACKET_HS   (AUDIO_IN_SLOT_HS + AUDIO_IN_FRAME_SIZE)
#define AUDIO_IN_PACKET_FS   (AUDIO_IN_SLOT_FS + AUDIO_IN_FRAME_SIZE)

#define AUDIO_BUFFER_COUNT   32
#define AUDIO_IN_BUF_TOTAL   (AUDIO_BUFFER_COUNT * AUDIO_IN_SLOT_MAX)

#define USB_AUDIO_CONFIG_DESC_SIZ (9 +                                                    \
                                   AUDIO_V2_AC_DESCRIPTOR_LEN +                           \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                 \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM) + \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +              \
                                   AUDIO_V2_AS_DESCRIPTOR_LEN)

#define AUDIO_AC_SIZ (AUDIO_V2_SIZEOF_AC_HEADER_DESC +                       \
                      AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                 \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +               \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM) + \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_MICROPHONE, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x03),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x02, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x04, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, SAMPLE_BITS, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_HS, EP_INTERVAL_HS),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_MICROPHONE, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x03),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x02, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x04, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, SAMPLE_BITS, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_MICROPHONE, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x03),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x02, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x04, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, SAMPLE_BITS, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_MICROPHONE, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_IN_CLOCK_ID, 0x03, 0x03),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_INTERM_MIC, AUDIO_IN_CLOCK_ID, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x02, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID, AUDIO_IN_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x04, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, SAMPLE_BITS, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET_HS, EP_INTERVAL_HS),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro UAC V2 DEMO",        /* Product */
#if defined(USING_CODEC) && USING_CODEC
    "2026070901",                 /* Serial Number (codec) */
#else
    "2026070902",                 /* Serial Number (PDM) */
#endif
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

#if defined(USING_CODEC) && USING_CODEC
static const uint8_t mic_default_sampling_freq_table[] = {
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
#else
static const uint8_t mic_default_sampling_freq_table[] = {
    AUDIO_SAMPLE_FREQ_NUM(1),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00)
};
#endif

/* Static Variables */
#define MIC_DMA_CHANNEL 1U
#define MIC_DMAMUX_CHANNEL        DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, MIC_DMA_CHANNEL)

#define AUDIO_ADAPTIVE_FULL_THRESHOLD   (AUDIO_IN_BUF_TOTAL * 5 / 8)   /* >20/32: send one extra frame to drain faster */
#define AUDIO_ADAPTIVE_EMPTY_THRESHOLD  (AUDIO_IN_BUF_TOTAL * 3 / 8)   /* <12/32: send one fewer frame to let buffer refill */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_in_buffer[AUDIO_IN_BUF_TOTAL];
static volatile uint32_t s_in_buffer_front;
static volatile uint32_t s_in_buffer_rear;
static volatile bool s_mic_priming;
static volatile bool s_split_pending;
static volatile uint32_t s_split_second_size;
static volatile uint32_t s_mic_sample_rate;
static volatile int32_t s_mic_volume_db;
static volatile bool s_mic_mute;
static volatile bool tx_flag;
static volatile bool s_mic_usb_transfer_req;
static volatile uint32_t s_mic_print_tm;
static volatile uint32_t s_mic_print_interval;
static volatile bool s_mic_buffer_print_pending;

static volatile uint32_t s_audio_in_slot_size;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes);
static struct usbd_endpoint audio_in_ep = {
    .ep_cb = usbd_audio_iso_in_callback,
    .ep_addr = AUDIO_IN_EP
};

static struct audio_entity_info audio_entity_table[] = {
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

/* Static Function Declaration */
static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool mic_dma_slot_size_is_valid(uint32_t slot_size);
static uint32_t in_buff_fill_count(void);
static uint32_t in_buff_get_adaptive_transfer_size(void);
static void in_buff_send_to_usb(uint8_t busid, uint32_t front, uint32_t transfer_size);
static hpm_stat_t mic_config_i2s_recording(uint32_t sample_rate);
#if defined(USING_CODEC) && USING_CODEC
static void mic_apply_codec_state(void);
#endif

/* Extern Function Definition */
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

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume_db)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_IN_EP) {
        /* Defer I2C codec write to audio_v2_task to avoid blocking USB ISR */
        s_mic_volume_db = volume_db;
#if defined(USING_CODEC) && USING_CODEC
        s_mic_codec_update_pending = true;
#endif
    }
}

int usbd_audio_get_volume(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

    int volume = 0;

    if (ep == AUDIO_IN_EP) {
        volume = s_mic_volume_db;
    }

    return volume;
}

void usbd_audio_set_mute(uint8_t busid, uint8_t ep, uint8_t ch, bool mute)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_IN_EP) {
        /* Defer I2C codec write to audio_v2_task to avoid blocking USB ISR */
        s_mic_mute = mute;
#if defined(USING_CODEC) && USING_CODEC
        s_mic_codec_update_pending = true;
#else
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
#endif
    }
}

bool usbd_audio_get_mute(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

    bool mute = false;

    if (ep == AUDIO_IN_EP) {
        mute = s_mic_mute;
    }

    return mute;
}

void usbd_audio_set_sampling_freq(uint8_t busid, uint8_t ep, uint32_t sampling_freq)
{
    (void)busid;

    if (ep == AUDIO_IN_EP) {
        s_mic_sample_rate = sampling_freq;
    }
}

uint32_t usbd_audio_get_sampling_freq(uint8_t busid, uint8_t ep)
{
    (void)busid;

    uint32_t freq = 0;

    if (ep == AUDIO_IN_EP) {
        freq = s_mic_sample_rate;
    }

    return freq;
}

void usbd_audio_get_sampling_freq_table(uint8_t busid, uint8_t ep, uint8_t **sampling_freq_table)
{
    (void)busid;

    if (ep == AUDIO_IN_EP) {
        *sampling_freq_table = (uint8_t *)mic_default_sampling_freq_table;
    }
}

void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    uint32_t packets_per_sec;
    hpm_stat_t state;

    if (intf == 1) {
        /* Compute actual packet size from current sample rate and USB speed */
        if (usbd_get_port_speed(busid) == USB_SPEED_HIGH) {
            packets_per_sec = MIC_HS_PACKETS_PER_SEC;
        } else {
            packets_per_sec = MIC_FS_PACKETS_PER_SEC;
        }
        s_audio_in_slot_size = (s_mic_sample_rate * AUDIO_IN_FRAME_SIZE) / packets_per_sec;
        if (!mic_dma_slot_size_is_valid(s_audio_in_slot_size)) {
            tx_flag = false;
            dma_abort_channel(BOARD_APP_DMA1, 1u << MIC_DMA_CHANNEL);
            USB_LOG_RAW("Invalid mic DMA slot size: %lu, buffer size: %lu\r\n", s_audio_in_slot_size, AUDIO_IN_BUF_TOTAL);
            return;
        }

        i2s_reset_rx(TARGET_I2S);
        state = mic_config_i2s_recording(s_mic_sample_rate);
        if (state != status_success) {
            USB_LOG_ERR("MIC I2S config failed\r\n");
            return;
        }
#if defined(USING_CODEC) && USING_CODEC
    #if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
        wm8960_set_data_format(&s_codec_control, s_mic_i2s_mclk_hz, s_mic_sample_rate, SAMPLE_BITS);
    #elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
        sgtl_config_data_format(&s_codec_control, s_mic_i2s_mclk_hz, s_mic_sample_rate, SAMPLE_BITS);
    #elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
        es8389_set_data_format(&s_codec_control, s_mic_i2s_mclk_hz, s_mic_sample_rate, SAMPLE_BITS);
    #endif
#endif
        tx_flag = 1;
        s_in_buffer_front = 0;
        s_in_buffer_rear = 0;
        s_mic_usb_transfer_req = false;
        s_mic_priming = true;
        s_split_pending = false;
        s_mic_print_interval = packets_per_sec / 10u;  /* ~100ms */
        s_mic_print_tm = 0;
#if !defined(USING_CODEC) || !USING_CODEC
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
#endif
        mic_i2s_dma_start_transfer((uint32_t)&s_in_buffer[0], s_audio_in_slot_size);
        i2s_start(TARGET_I2S);

        USB_LOG_RAW("OPEN, sample rate: %lu Hz\r\n", (unsigned long)s_mic_sample_rate);
    }
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

    if (intf == 1) {
        tx_flag = 0;
        /* Stop any ongoing DMA transfer to prevent stale TC after close */
        dma_abort_channel(BOARD_APP_DMA1, 1u << MIC_DMA_CHANNEL);
        i2s_reset_rx(TARGET_I2S);
        i2s_stop(TARGET_I2S);
#if !defined(USING_CODEC) || !USING_CODEC
        pdm_stop(HPM_PDM);
#endif
        USB_LOG_RAW("CLOSE\r\n");
    }
}

void audio_v2_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &audio_v2_descriptor);
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf0, AUDIO_VERSION, audio_entity_table, 2));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf1, AUDIO_VERSION, audio_entity_table, 2));
    usbd_add_endpoint(busid, &audio_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void init_mic_i2s_pdm_codec(void)
{
    i2s_config_t i2s_config;

    i2s_get_default_config(TARGET_I2S, &i2s_config);
#if defined(USING_CODEC) && USING_CODEC
    i2s_config.enable_mclk_out = true;
#endif
    if (i2s_init(TARGET_I2S, &i2s_config) != status_success) {
        printf("i2s_init failed\n");
        while (1) {
        }
    }

#if defined(USING_CODEC) && USING_CODEC
    s_mic_sample_rate = 48000;
    s_i2s_invert_fclk_out = i2s_config.invert_fclk_out;
    s_mic_i2s_mclk_hz = clock_get_frequency(TARGET_I2S_CLK_NAME);

#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    wm8960_config_t wm8960_config;
    wm8960_get_default_config(&wm8960_config);

    /* Override default route for recording only */
    wm8960_config.route = wm8960_route_record;
    wm8960_config.format.sample_rate = s_mic_sample_rate;
    wm8960_config.format.bit_width = SAMPLE_BITS;
    wm8960_config.format.mclk_hz = s_mic_i2s_mclk_hz;
    wm8960_config.lrclk_polarity = (s_i2s_invert_fclk_out) ? wm8960_lrclk_polarity_high_for_left_channel : wm8960_lrclk_polarity_low_for_left_channel;
    if (wm8960_init(&s_codec_control, &wm8960_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }

#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_config_t sgtl5000_config;
    sgtl_get_default_config(&sgtl5000_config);

    /* Override default route for recording only */
    sgtl5000_config.route = sgtl_route_record;
    sgtl5000_config.format.sample_rate = s_mic_sample_rate;
    sgtl5000_config.format.bit_width = SAMPLE_BITS;
    sgtl5000_config.format.mclk_hz = s_mic_i2s_mclk_hz;
    sgtl5000_config.lrclk_polarity = (s_i2s_invert_fclk_out) ? sgtl_lrclk_polarity_high_for_left_channel : sgtl_lrclk_polarity_low_for_left_channel;
    if (sgtl_init(&s_codec_control, &sgtl5000_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }

#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_config_t es8389_config;
    es8389_get_default_config(&es8389_config);

    es8389_config.sample_rate = s_mic_sample_rate;
    es8389_config.data_width = SAMPLE_BITS;
    es8389_config.mclk_hz = s_mic_i2s_mclk_hz;
    es8389_config.lrclk_polarity = (s_i2s_invert_fclk_out) ? es8389_lrclk_polarity_high_for_left_channel : es8389_lrclk_polarity_low_for_left_channel;
    if (es8389_init(&s_codec_control, &es8389_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }

#else
    #error no specified Audio Codec!!!
#endif

#else
    pdm_config_t pdm_config;

    s_mic_sample_rate = MIC_MAX_SAMPLE_FREQ;
    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);
#endif
}

void i2s_enable_dma_irq_with_priority(int32_t priority)
{
    i2s_enable_rx_dma_request(TARGET_I2S);
    dmamux_config(BOARD_APP_DMAMUX, MIC_DMAMUX_CHANNEL, TARGET_I2S_RX_DMAMUX_SRC, true);
    intc_m_enable_irq_with_priority(BOARD_APP_DMA1_IRQ, priority);
}

void audio_v2_task(uint8_t busid)
{
    (void)busid;

#if defined(USING_CODEC) && USING_CODEC
    /* Apply deferred codec state update (I2C is slow, must not run in USB ISR context) */
    if (s_mic_codec_update_pending) {
        s_mic_codec_update_pending = false;
        mic_apply_codec_state();
    }
#endif

    /* Deferred buffer status print (printf is slow, must not run in ISR) */
    if (s_mic_buffer_print_pending) {
        s_mic_buffer_print_pending = false;
        uint32_t used = in_buff_fill_count();
        uint32_t total = AUDIO_IN_BUF_TOTAL;
        uint32_t percent = (used * 100u) / total;
        printf("mic buf: used=%lu/%lu (%lu%%)\n", used, total, percent);
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_DMA1_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t stat;

    stat = dma_check_transfer_status(BOARD_APP_DMA1, MIC_DMA_CHANNEL);

    if (tx_flag && (0 != (stat & DMA_CHANNEL_STATUS_TC))) {
        uint32_t rear = s_in_buffer_rear;
        rear += s_audio_in_slot_size;
        if (rear >= AUDIO_IN_BUF_TOTAL) {
            rear = 0;
        }
        s_in_buffer_rear = rear;
        mic_i2s_dma_start_transfer((uint32_t)&s_in_buffer[rear], s_audio_in_slot_size);

        /* Priming: accumulate half the buffer before starting USB transfers */
        if (s_mic_priming) {
            if (in_buff_fill_count() >= (AUDIO_IN_BUF_TOTAL / 2)) {
                s_mic_priming = false;
                s_mic_usb_transfer_req = true;
            }
        }

        /* If USB IN is waiting for data, kick it off now */
        if (s_mic_usb_transfer_req && !s_split_pending) {
            uint32_t transfer_size = in_buff_get_adaptive_transfer_size();
            if (in_buff_fill_count() >= transfer_size) {
                uint32_t front = s_in_buffer_front;
                s_mic_usb_transfer_req = false;
                in_buff_send_to_usb(0, front, transfer_size);
                front += transfer_size;
                if (front >= AUDIO_IN_BUF_TOTAL) {
                    front -= AUDIO_IN_BUF_TOTAL;
                }
                s_in_buffer_front = front;
            }
        }

        s_mic_print_tm++;
        if (s_mic_print_tm >= s_mic_print_interval) {
            s_mic_print_tm = 0;
            s_mic_buffer_print_pending = true;
        }
    }
}

/* Static Function Definition */
static void usbd_audio_iso_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    if (tx_flag) {
        if (s_split_pending) {
            /* Complete a pending split transfer: send the second (head) part.
             * Front was already advanced by the full transfer_size when the split was initiated.
             */
            s_split_pending = false;
            usbd_ep_start_write(busid, AUDIO_IN_EP, s_in_buffer, s_split_second_size);
        } else {
            uint32_t transfer_size = in_buff_get_adaptive_transfer_size();
            if (in_buff_fill_count() >= transfer_size) {
                uint32_t front = s_in_buffer_front;
                in_buff_send_to_usb(busid, front, transfer_size);
                front += transfer_size;
                if (front >= AUDIO_IN_BUF_TOTAL) {
                    front -= AUDIO_IN_BUF_TOTAL;
                }
                s_in_buffer_front = front;
            } else {
                /* Not enough data, defer USB IN kick-off to DMA ISR */
                s_mic_usb_transfer_req = true;
            }
        }
    }
}

static hpm_stat_t mic_config_i2s_recording(uint32_t sample_rate)
{
    i2s_transfer_config_t transfer;
    uint32_t i2s_mclk_hz;

#if defined(USING_CODEC) && USING_CODEC
    if (IN_CHANNEL_NUM > 2) {
        return status_invalid_argument; /* Currently not support TDM mode */
    }

    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = TARGET_I2S_RX_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = SAMPLE_BITS;
    transfer.channel_num_per_frame = 2; /* non TDM mode, channel num fix to 2. */
    transfer.channel_slot_mask = 0x3;   /* 2 channels */

    i2s_mclk_hz = s_mic_i2s_mclk_hz;
#else
    i2s_get_default_transfer_config_for_pdm(&transfer);
    transfer.data_line = TARGET_I2S_RX_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.channel_slot_mask = BOARD_PDM_DUAL_CHANNEL_MASK;

    i2s_mclk_hz = clock_get_frequency(TARGET_I2S_CLK_NAME);
#endif

    return i2s_config_rx(TARGET_I2S, i2s_mclk_hz, &transfer);
}

#if defined(USING_CODEC) && USING_CODEC
static void mic_apply_codec_state(void)
{
#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    /* WM8960 has no hardware mute bit, use the minimum dB value as mute */
    if (s_mic_mute) {
        wm8960_mute(&s_codec_control, wm8960_module_adc);
    } else {
        /* Restore previous volume from stored dB value */
        float volume_db;
        wm8960_clamp_volume_db(wm8960_module_adc, (float)s_mic_volume_db, &volume_db);
        wm8960_set_volume_db(&s_codec_control, wm8960_module_adc, volume_db);
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    /* SGTL5000 has hardware mute, apply mute and volume independently */
    if (sgtl_set_mute(&s_codec_control, sgtl_module_adc, s_mic_mute) != status_success) {
        USB_LOG_RAW("set mute Fail!\r\n");
    }
    if (!s_mic_mute) {
        float volume_db;
        sgtl_clamp_volume_db(sgtl_module_adc, (float)s_mic_volume_db, &volume_db);
        if (sgtl_set_volume_db(&s_codec_control, sgtl_module_adc, volume_db) != status_success) {
            USB_LOG_RAW("set volume Fail!\r\n");
        }
    }
#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    /* ES8389 has hardware mute, apply mute and volume independently */
    if (es8389_mute(&s_codec_control, es8389_adc1, s_mic_mute) != status_success) {
        USB_LOG_RAW("set mute Fail!\r\n");
    }
    if (es8389_mute(&s_codec_control, es8389_adc2, s_mic_mute) != status_success) {
        USB_LOG_RAW("set mute Fail!\r\n");
    }
    if (!s_mic_mute) {
        float volume_db;
        es8389_clamp_volume_db(es8389_adc1, (float)s_mic_volume_db, &volume_db);
        if (es8389_set_volume_db(&s_codec_control, es8389_adc1, volume_db) != status_success) {
            USB_LOG_RAW("set volume Fail!\r\n");
        }
        if (es8389_set_volume_db(&s_codec_control, es8389_adc2, volume_db) != status_success) {
            USB_LOG_RAW("set volume Fail!\r\n");
        }
    }
#endif
}
#endif /* USING_CODEC */

static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };
    hpm_stat_t status;

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = (uint32_t)(&TARGET_I2S->RXD[TARGET_I2S_RX_DATA_LINE]);
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
        i2s_enable_rx(TARGET_I2S, 1U << TARGET_I2S_RX_DATA_LINE);
    }
}

static bool mic_dma_slot_size_is_valid(uint32_t slot_size)
{
    return (slot_size != 0U)
        && ((slot_size % AUDIO_IN_FRAME_SIZE) == 0U)
        && ((AUDIO_IN_BUF_TOTAL % slot_size) == 0U);
}

static void in_buff_send_to_usb(uint8_t busid, uint32_t front, uint32_t transfer_size)
{
    if (front + transfer_size > AUDIO_IN_BUF_TOTAL) {
        /* Circular buffer wrap: split into two USB writes.
         * First part (tail) sent now; second part (head) deferred to callback via s_split_pending.
         * Front is advanced by the full transfer_size at once to keep state consistent for ISR.
         */
        uint32_t first_part = AUDIO_IN_BUF_TOTAL - front;
        s_split_second_size = transfer_size - first_part;
        s_split_pending = true;
        usbd_ep_start_write(busid, AUDIO_IN_EP, &s_in_buffer[front], first_part);
    } else {
        usbd_ep_start_write(busid, AUDIO_IN_EP, &s_in_buffer[front], transfer_size);
    }
}

static uint32_t in_buff_fill_count(void)
{
    int32_t count = (int32_t)s_in_buffer_rear - (int32_t)s_in_buffer_front;
    if (count < 0) {
        count += AUDIO_IN_BUF_TOTAL;
    }
    return (uint32_t)count;
}

static uint32_t in_buff_get_adaptive_transfer_size(void)
{
    uint32_t transfer_size = s_audio_in_slot_size;
    uint32_t fill = in_buff_fill_count();

    if (fill > AUDIO_ADAPTIVE_FULL_THRESHOLD) {
        /* Buffer nearly full → send 1 extra audio frame to drain faster */
        transfer_size = s_audio_in_slot_size + AUDIO_IN_FRAME_SIZE;
    } else if (fill < AUDIO_ADAPTIVE_EMPTY_THRESHOLD) {
        /* Buffer nearly empty → send 1 fewer audio frame to let buffer refill */
        transfer_size = s_audio_in_slot_size - AUDIO_IN_FRAME_SIZE;
    }

    return transfer_size;
}
