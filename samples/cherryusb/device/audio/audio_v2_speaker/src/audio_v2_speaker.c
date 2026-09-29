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
#include "audio_v2_speaker.h"

static bool i2s_invert_fclk_out;

#if defined(USING_CODEC) && USING_CODEC
#include "hpm_codec_common.h"
/* Static control structure for codec access across multiple functions */
static codec_control_t s_codec_control = {
    .ptr = CODEC_I2C,
    .slave_address = BOARD_AUDIO_CODEC_I2C_ADDR,
};
#elif defined(USING_DAO) && USING_DAO
#include "hpm_dao_drv.h"
dao_config_t dao_config;
#else
#error define USING_CODEC or USING_DAO
#endif

#define EP_INTERVAL_HS 0x04
#define EP_INTERVAL_FS 0x01

/* bInterval -> packets/sec: HS 8000 / 2^(bInterval-1), FS 1000 / 2^(bInterval-1) */
#define SPEAKER_HS_MICROFRAMES_PER_PACKET (1U << (EP_INTERVAL_HS - 1U))
#define SPEAKER_HS_PACKETS_PER_SEC        (8000U / SPEAKER_HS_MICROFRAMES_PER_PACKET)
#define SPEAKER_FS_FRAMES_PER_PACKET      (1U << (EP_INTERVAL_FS - 1U))
#define SPEAKER_FS_PACKETS_PER_SEC        (1000U / SPEAKER_FS_FRAMES_PER_PACKET)

#define AUDIO_OUT_EP 0x01

#define AUDIO_VERSION 0x0200

#define AUDIO_OUT_CLOCK_ID 0x01
#define AUDIO_OUT_FU_ID    0x03

#define SPEAKER_MAX_SAMPLE_FREQ    96000
#define SPEAKER_SLOT_BYTE_SIZE 4
#define SPEAKER_AUDIO_DEPTH    24

#define BMCONTROL (AUDIO_V2_CONTROL_MUTE | AUDIO_V2_CONTROL_VOLUME)

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

#define SPEAKER_DMA_CHANNEL 1U
#define SPEAKER_DMAMUX_CHANNEL    DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL)

/* Buffer geometry */
#define AUDIO_OUT_FRAME_SIZE    (OUT_CHANNEL_NUM * SPEAKER_SLOT_BYTE_SIZE)

/* USB service slot sizes at the maximum sample rate */
#define AUDIO_OUT_SLOT_HS       ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ * AUDIO_OUT_FRAME_SIZE) / SPEAKER_HS_PACKETS_PER_SEC))
#define AUDIO_OUT_SLOT_FS       ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ * AUDIO_OUT_FRAME_SIZE) / SPEAKER_FS_PACKETS_PER_SEC))

/* USB descriptor packet size = slot + 1 extra audio frame */
#define AUDIO_OUT_PACKET_HS     (AUDIO_OUT_SLOT_HS + AUDIO_OUT_FRAME_SIZE)
#define AUDIO_OUT_PACKET_FS     (AUDIO_OUT_SLOT_FS + AUDIO_OUT_FRAME_SIZE)
#define AUDIO_OUT_PACKET_MAX    ((AUDIO_OUT_PACKET_HS) > (AUDIO_OUT_PACKET_FS) ? (AUDIO_OUT_PACKET_HS) : (AUDIO_OUT_PACKET_FS))

#define AUDIO_BUFFER_COUNT      32

#define USB_AUDIO_CONFIG_DESC_SIZ (9 +                                                     \
                                   AUDIO_V2_AC_DESCRIPTOR_LEN +                            \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_AS_DESCRIPTOR_LEN)

#define AUDIO_AC_SIZ (AUDIO_V2_SIZEOF_AC_HEADER_DESC +                        \
                      AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_SPEAKER, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_HS, EP_INTERVAL_HS),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_SPEAKER, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_SPEAKER, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_FS, EP_INTERVAL_FS),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_SPEAKER, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, AUDIO_OUT_CLOCK_ID, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID, AUDIO_OUT_CLOCK_ID, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET_HS, EP_INTERVAL_HS),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro UAC V2 DEMO",        /* Product */
    "2026060502",                 /* Serial Number */
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

static const uint8_t default_sampling_freq_table[] = {
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

/* Static Variables */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_out_buffer[AUDIO_BUFFER_COUNT][AUDIO_OUT_PACKET_MAX];
static uint32_t s_speaker_out_buffer_size[AUDIO_BUFFER_COUNT];
static volatile uint32_t s_speaker_i2s_mclk_hz;
static volatile bool s_speaker_rx_flag;
static volatile uint8_t s_speaker_out_buffer_front;
static volatile uint8_t s_speaker_out_buffer_rear;
static volatile bool s_speaker_dma_transfer_req;
static volatile uint8_t s_speaker_priming_count;
static volatile uint32_t s_speaker_sample_rate;
static volatile int32_t s_speaker_volume_db;
static volatile bool s_speaker_mute;
static volatile bool s_speaker_codec_update_pending;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static void usbd_audio_iso_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes);
static struct usbd_endpoint audio_out_ep = {
    .ep_cb = usbd_audio_iso_out_callback,
    .ep_addr = AUDIO_OUT_EP
};

struct audio_entity_info audio_entity_table[] = {
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
};

/* Static Functions Declaration */
static hpm_stat_t speaker_config_i2s_playback(uint32_t sample_rate);
static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static void speaker_apply_codec_state(void);
static bool speaker_out_buff_is_empty(void);

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
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf0, AUDIO_VERSION, audio_entity_table, 2));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &intf1, AUDIO_VERSION, audio_entity_table, 2));
    usbd_add_endpoint(busid, &audio_out_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void speaker_init_i2s_dao_codec(void)
{
    i2s_config_t i2s_config;

    s_speaker_sample_rate = 16000;

    i2s_get_default_config(TARGET_I2S, &i2s_config);
#if defined(USING_CODEC) && USING_CODEC
    i2s_config.enable_mclk_out = true;
#endif
    if (i2s_init(TARGET_I2S, &i2s_config) != status_success) {
        printf("i2s_init failed\n");
        while (1) {
        }
    }
    i2s_invert_fclk_out = i2s_config.invert_fclk_out;
    s_speaker_i2s_mclk_hz = clock_get_frequency(TARGET_I2S_CLK_NAME);

#if defined(USING_CODEC) && USING_CODEC
#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    wm8960_config_t wm8960_config;
    wm8960_get_default_config(&wm8960_config);

    /* Override default route for playback only */
    wm8960_config.route = wm8960_route_playback;
    wm8960_config.format.sample_rate = s_speaker_sample_rate;
    wm8960_config.format.bit_width = SPEAKER_AUDIO_DEPTH;
    wm8960_config.format.mclk_hz = s_speaker_i2s_mclk_hz;
    wm8960_config.lrclk_polarity = (i2s_invert_fclk_out) ? wm8960_lrclk_polarity_high_for_left_channel : wm8960_lrclk_polarity_low_for_left_channel;
    if (wm8960_init(&s_codec_control, &wm8960_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }

#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_config_t sgtl5000_config;
    sgtl_get_default_config(&sgtl5000_config);

    /* Override default route for playback only */
    sgtl5000_config.route = sgtl_route_playback;
    sgtl5000_config.format.sample_rate = s_speaker_sample_rate;
    sgtl5000_config.format.bit_width = SPEAKER_AUDIO_DEPTH;
    sgtl5000_config.format.mclk_hz = s_speaker_i2s_mclk_hz;
    sgtl5000_config.lrclk_polarity = (i2s_invert_fclk_out) ? sgtl_lrclk_polarity_high_for_left_channel : sgtl_lrclk_polarity_low_for_left_channel;
    if (sgtl_init(&s_codec_control, &sgtl5000_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }

#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_config_t es8389_config;
    es8389_get_default_config(&es8389_config);

    es8389_config.sample_rate = s_speaker_sample_rate;
    es8389_config.data_width = SPEAKER_AUDIO_DEPTH;
    es8389_config.mclk_hz = s_speaker_i2s_mclk_hz;
    es8389_config.lrclk_polarity = (i2s_invert_fclk_out) ? es8389_lrclk_polarity_high_for_left_channel : es8389_lrclk_polarity_low_for_left_channel;
    if (es8389_init(&s_codec_control, &es8389_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }

#else
    #error no specified Audio Codec!!!
#endif
#elif defined(USING_DAO) && USING_DAO
    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);
#endif
}

void i2s_enable_dma_irq_with_priority(int32_t priority)
{
    i2s_enable_tx_dma_request(TARGET_I2S);
    dmamux_config(BOARD_APP_DMAMUX, SPEAKER_DMAMUX_CHANNEL, TARGET_I2S_TX_DMAMUX_SRC, true);

    intc_m_enable_irq_with_priority(BOARD_APP_DMA1_IRQ, priority);
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_DMA1_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL);
    if (0 != (speaker_status & DMA_CHANNEL_STATUS_TC)) {
        if (s_speaker_rx_flag) {
            if (!speaker_out_buff_is_empty()) {
                uint32_t front = s_speaker_out_buffer_front;
                speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[front][0],
                                               s_speaker_out_buffer_size[front]);
                front++;
                if (front >= AUDIO_BUFFER_COUNT) {
                    front = 0;
                }
                s_speaker_out_buffer_front = front;
            } else {
                /* Buffer empty, set flag to retry when next USB packet arrives */
                s_speaker_dma_transfer_req = true;
            }

        }
    }
}

void audio_v2_task(uint8_t busid)
{
    (void)busid;

    /* Apply deferred codec state update (I2C is slow, must not run in USB ISR context) */
    if (s_speaker_codec_update_pending) {
        s_speaker_codec_update_pending = false;
        speaker_apply_codec_state();
    }
}

void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    hpm_stat_t state;

    (void)intf;

    i2s_reset_tx(TARGET_I2S);
    state = speaker_config_i2s_playback(s_speaker_sample_rate);
    if (state != status_success) {
        USB_LOG_ERR("SPEAKER I2S config failed\r\n");
        return;
    }
#if defined(USING_CODEC) && USING_CODEC
    #if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    wm8960_set_data_format(&s_codec_control, s_speaker_i2s_mclk_hz, s_speaker_sample_rate, SPEAKER_AUDIO_DEPTH);
    #elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_config_data_format(&s_codec_control, s_speaker_i2s_mclk_hz, s_speaker_sample_rate, SPEAKER_AUDIO_DEPTH);
    #elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_set_data_format(&s_codec_control, s_speaker_i2s_mclk_hz, s_speaker_sample_rate, SPEAKER_AUDIO_DEPTH);
    #endif
#endif
    s_speaker_rx_flag = 1;
    s_speaker_out_buffer_front = 0;
    s_speaker_out_buffer_rear = 0;
    s_speaker_dma_transfer_req = true;
    s_speaker_priming_count = 0;
    /* setup first out ep read transfer */
    usbd_ep_start_read(busid, AUDIO_OUT_EP, (uint8_t *)&s_speaker_out_buffer[s_speaker_out_buffer_rear][0], AUDIO_OUT_PACKET_MAX);
#if defined(USING_DAO) && USING_DAO
    if (s_speaker_mute) {
        dao_stop(HPM_DAO);
    } else {
        dao_start(HPM_DAO);
    }
#endif
    i2s_start(TARGET_I2S);
    USB_LOG_RAW("OPEN SPEAKER, sample rate: %lu Hz\r\n", (unsigned long)s_speaker_sample_rate);
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;
    (void)intf;

    s_speaker_rx_flag = 0;
    /* Stop any ongoing DMA transfer to prevent stale TC after close */
    dma_abort_channel(BOARD_APP_DMA1, 1u << SPEAKER_DMA_CHANNEL);
    i2s_reset_tx(TARGET_I2S);
    i2s_stop(TARGET_I2S);
#if defined(USING_DAO) && USING_DAO
    dao_stop(HPM_DAO);
#endif
    USB_LOG_RAW("CLOSE SPEAKER\r\n");
}

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume_db)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        /* CherryUSB passes volume in dB units (e.g., -30 for -30dB, 0 for 0dB) */
        /* Defer I2C codec write to audio_v2_task to avoid blocking USB ISR */
        s_speaker_volume_db = volume_db;
        s_speaker_codec_update_pending = true;
    }
}

int usbd_audio_get_volume(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

    int volume = 0;

    if (ep == AUDIO_OUT_EP) {
        volume = s_speaker_volume_db;
    }

    return volume;
}

void usbd_audio_set_mute(uint8_t busid, uint8_t ep, uint8_t ch, bool mute)
{
    (void)busid;
    (void)ch;

    if (ep == AUDIO_OUT_EP) {
        /* Defer I2C codec write to audio_v2_task to avoid blocking USB ISR */
        s_speaker_mute = mute;
        s_speaker_codec_update_pending = true;
#if defined(USING_DAO) && USING_DAO
        if (s_speaker_mute) {
            dao_stop(HPM_DAO);
        } else {
            dao_start(HPM_DAO);
        }
#endif
    }
}

bool usbd_audio_get_mute(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

    bool mute = false;

    if (ep == AUDIO_OUT_EP) {
        mute = s_speaker_mute;
    }

    return mute;
}

static void speaker_apply_codec_state(void)
{
#if defined(USING_CODEC) && USING_CODEC
    #if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    /* WM8960 has no hardware mute bit, use the minimum dB value as mute */
    if (s_speaker_mute) {
        wm8960_mute(&s_codec_control, wm8960_module_dac);
    } else {
        /* Restore previous volume from stored dB value */
        float volume_db;
        wm8960_clamp_volume_db(wm8960_module_dac, (float)s_speaker_volume_db, &volume_db);
        wm8960_set_volume_db(&s_codec_control, wm8960_module_dac, volume_db);
    }
    #elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    /* SGTL5000 has hardware mute, apply mute and volume independently */
    if (sgtl_set_mute(&s_codec_control, sgtl_module_dac, s_speaker_mute) != status_success) {
        USB_LOG_RAW("set mute Fail!\r\n");
    }
    if (!s_speaker_mute) {
        float volume_db;
        sgtl_clamp_volume_db(sgtl_module_dac, (float)s_speaker_volume_db, &volume_db);
        if (sgtl_set_volume_db(&s_codec_control, sgtl_module_dac, volume_db) != status_success) {
            USB_LOG_RAW("set volume Fail!\r\n");
        }
    }
    #elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    /* ES8389 has hardware mute, apply mute and volume independently */
    if (es8389_mute(&s_codec_control, es8389_dac1, s_speaker_mute) != status_success) {
        USB_LOG_RAW("set mute Fail!\r\n");
    }
    if (es8389_mute(&s_codec_control, es8389_dac2, s_speaker_mute) != status_success) {
        USB_LOG_RAW("set mute Fail!\r\n");
    }
    if (!s_speaker_mute) {
        float volume_db;
        es8389_clamp_volume_db(es8389_dac1, (float)s_speaker_volume_db, &volume_db);
        if (es8389_set_volume_db(&s_codec_control, es8389_dac1, volume_db) != status_success) {
            USB_LOG_RAW("set volume Fail!\r\n");
        }
        if (es8389_set_volume_db(&s_codec_control, es8389_dac2, volume_db) != status_success) {
            USB_LOG_RAW("set volume Fail!\r\n");
        }
    }
    #endif
#endif
}

void usbd_audio_set_sampling_freq(uint8_t busid, uint8_t ep, uint32_t sampling_freq)
{
    (void)busid;

    if (ep == AUDIO_OUT_EP) {
        s_speaker_sample_rate = sampling_freq;
    }
}

uint32_t usbd_audio_get_sampling_freq(uint8_t busid, uint8_t ep)
{
    (void)busid;

    uint32_t freq = 0;

    if (ep == AUDIO_OUT_EP) {
        freq = s_speaker_sample_rate;
    }

    return freq;
}

void usbd_audio_get_sampling_freq_table(uint8_t busid, uint8_t ep, uint8_t **sampling_freq_table)
{
    (void)busid;

    if (ep == AUDIO_OUT_EP) {
        *sampling_freq_table = (uint8_t *)default_sampling_freq_table;
    }
}

/* Static Function Definition */
static void usbd_audio_iso_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if (s_speaker_rx_flag) {
        uint32_t rear = s_speaker_out_buffer_rear;

        s_speaker_out_buffer_size[rear] = nbytes;
        rear++;
        if (rear >= AUDIO_BUFFER_COUNT) {
            rear = 0;
        }
        s_speaker_out_buffer_rear = rear;

        /* Priming: on initial open, accumulate half the buffer before first DMA kickoff.
         * Once primed, s_speaker_priming_count stays at its final value and never changes,
         * so mid-stream underrun recovery kicks off immediately on the next packet.
         */
        if (s_speaker_dma_transfer_req) {
            if (s_speaker_priming_count < (AUDIO_BUFFER_COUNT / 2)) {
                s_speaker_priming_count++;
            } else {
                uint32_t front = s_speaker_out_buffer_front;
                s_speaker_dma_transfer_req = false;
                speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[front][0],
                                               s_speaker_out_buffer_size[front]);
                front++;
                if (front >= AUDIO_BUFFER_COUNT) {
                    front = 0;
                }
                s_speaker_out_buffer_front = front;
            }
        }

        usbd_ep_start_read(busid, ep, &s_speaker_out_buffer[s_speaker_out_buffer_rear][0], AUDIO_OUT_PACKET_MAX);
    }
}

static hpm_stat_t speaker_config_i2s_playback(uint32_t sample_rate)
{
    i2s_transfer_config_t transfer;

    if (OUT_CHANNEL_NUM > 2) {
        return status_invalid_argument; /* Currently not support TDM mode */
    }

    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = TARGET_I2S_TX_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = SPEAKER_AUDIO_DEPTH;
    transfer.channel_num_per_frame = 2; /* non TDM mode, channel num fix to 2. */
    transfer.channel_slot_mask = 0x3;   /* 2 channels */

    if (status_success != i2s_config_tx(TARGET_I2S, s_speaker_i2s_mclk_hz, &transfer)) {
        return status_fail;
    }

    return status_success;
}

static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };
    hpm_stat_t status;

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&TARGET_I2S->TXD[TARGET_I2S_TX_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    status = dma_setup_channel(BOARD_APP_DMA1, SPEAKER_DMA_CHANNEL, &ch_config, true);
    if (status != status_success) {
        printf(" dma setup channel failed\n");
    }
    if (status == status_success) {
        i2s_enable_tx(TARGET_I2S, 1U << TARGET_I2S_TX_DATA_LINE);
    }
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
