/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_audio.h"
#include "usb_audio.h"
#include "hpm_i2s_drv.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_pdm_drv.h"
#include "board.h"


#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_VERSION 0x0100

#define AUDIO_IN_EP 0x81

#define AUDIO_IN_FU_ID 0x02

/* AUDIO Class Config */
#define AUDIO_FREQ 16000U

#define IN_CHANNEL_NUM 1

#if IN_CHANNEL_NUM == 1
#define INPUT_CTRL      0x03, 0x03
#define INPUT_CH_ENABLE 0x0000
#elif IN_CHANNEL_NUM == 2
#define INPUT_CTRL      0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x0003
#elif IN_CHANNEL_NUM == 3
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x0007
#elif IN_CHANNEL_NUM == 4
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x000f
#elif IN_CHANNEL_NUM == 5
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x001f
#elif IN_CHANNEL_NUM == 6
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x003F
#elif IN_CHANNEL_NUM == 7
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x007f
#elif IN_CHANNEL_NUM == 8
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x00ff
#endif

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 1) */
/* 16bit(2 Bytes) One Channel(Mono:1) */
#define AUDIO_IN_PACKET ((uint32_t)((AUDIO_FREQ * 2 * IN_CHANNEL_NUM) / 1000))

#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +                                                    \
                                                  AUDIO_AC_DESCRIPTOR_INIT_LEN(1) +                      \
                                                  AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +                  \
                                                  AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM, 1) + \
                                                  AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +                 \
                                                  AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

#define AUDIO_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(1) +                       \
                      AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +                  \
                      AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(IN_CHANNEL_NUM, 1) + \
                      AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

const uint8_t audio_v1_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, 0x00, 0x01),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, IN_CHANNEL_NUM, INPUT_CH_ENABLE),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x01, 0x01, INPUT_CTRL),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID),
    AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x03, IN_CHANNEL_NUM, 2, 16, AUDIO_IN_EP, 0x05, AUDIO_IN_PACKET, EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(AUDIO_FREQ)),
    /*
     * string0 descriptor
     */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /*
     * string1 descriptor
     */
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    /*
     * string2 descriptor
     */
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'U', 0x00,                  /* wcChar10 */
    'A', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    /*
     * string3 descriptor
     */
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '0' + IN_CHANNEL_NUM, 0x00, /* wcChar9 */
#ifdef CONFIG_USB_HS
    /*
     * device qualifier descriptor
     */
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

#define MIC_DMA_CHANNEL       1U
#define MIC_DMAMUX_CHANNEL    DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_HDMA, MIC_DMA_CHANNEL)
#define AUDIO_BUFFER_COUNT    32
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_in_buffer[AUDIO_BUFFER_COUNT][AUDIO_IN_PACKET];
static volatile uint8_t s_in_buffer_front;
static volatile uint8_t s_in_buffer_rear;
static volatile bool s_dma_transfer_done;
static volatile bool tx_flag;
static volatile bool ep_tx_busy_flag;
static volatile uint32_t s_mic_sample_rate;
static volatile int32_t s_mic_volume_percent;
static volatile bool s_mic_mute;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static void usbd_audio_iso_callback(uint8_t ep, uint32_t nbytes);
static struct usbd_endpoint audio_in_ep = {
    .ep_cb = usbd_audio_iso_callback,
    .ep_addr = AUDIO_IN_EP
};

static struct audio_entity_info audio_entity_table[] = {
    {
        .bEntityId = AUDIO_IN_FU_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
        .ep = AUDIO_IN_EP
    },
};

/* static function declaration */
static void i2s_pdm_dma_start_transfer(uint32_t addr, uint32_t size);
static bool in_buff_is_full(void);
static bool in_buff_is_empty(void);

/* extern function definition */
void usbd_event_handler(uint8_t event)
{
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

void usbd_audio_set_volume(uint8_t ep, uint8_t ch, int volume)
{
    (void)ch;
    if (ep == AUDIO_IN_EP) {
        s_mic_volume_percent = volume;
        /* Do Nothing */
    }
}

int usbd_audio_get_volume(uint8_t ep, uint8_t ch)
{
    (void)ch;
    int volume = 0;

    if (ep == AUDIO_IN_EP) {
        volume = s_mic_volume_percent;
    }

    return volume;
}

void usbd_audio_set_mute(uint8_t ep, uint8_t ch, bool mute)
{
    (void)ch;
    if (ep == AUDIO_IN_EP) {
        s_mic_mute = mute;
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
    }
}

bool usbd_audio_get_mute(uint8_t ep, uint8_t ch)
{
    (void)ch;
    bool mute = false;

    if (ep == AUDIO_IN_EP) {
        mute = s_mic_mute;
    }

    return mute;
}

void usbd_audio_set_sampling_freq(uint8_t ep, uint32_t sampling_freq)
{
    if (ep == AUDIO_IN_EP) {
        s_mic_sample_rate = sampling_freq;
    }
}

uint32_t usbd_audio_get_sampling_freq(uint8_t ep)
{
    uint32_t freq = 0;

    if (ep == AUDIO_IN_EP) {
        freq = s_mic_sample_rate;
    }

    return freq;
}

void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        tx_flag = 1;
        ep_tx_busy_flag = false;
        s_in_buffer_front = 0;
        s_in_buffer_rear = 0;
        s_dma_transfer_done = false;
        pdm_start(HPM_PDM);
        i2s_pdm_dma_start_transfer((uint32_t)&s_in_buffer[s_in_buffer_rear][0], AUDIO_IN_PACKET);

        USB_LOG_RAW("OPEN\r\n");
    }
}

void usbd_audio_close(uint8_t intf)
{
    if (intf == 1) {
        tx_flag = 0;
        pdm_stop(HPM_PDM);
        USB_LOG_RAW("CLOSE\r\n");
    }
}

void audio_init(void)
{
    usbd_desc_register(audio_v1_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&intf0, AUDIO_VERSION, audio_entity_table, 1));
    usbd_add_interface(usbd_audio_init_intf(&intf1, AUDIO_VERSION, audio_entity_table, 1));
    usbd_add_endpoint(&audio_in_ep);

    usbd_initialize();
}

void audio_test(void)
{
    if (tx_flag) {
        if (!in_buff_is_full()) {
            if (s_dma_transfer_done) {
                s_dma_transfer_done = false;
                s_in_buffer_rear++;
                if (s_in_buffer_rear >= AUDIO_BUFFER_COUNT) {
                    s_in_buffer_rear = 0;
                }
                i2s_pdm_dma_start_transfer((uint32_t)&s_in_buffer[s_in_buffer_rear][0], AUDIO_IN_PACKET);
            }
        }

        if (!in_buff_is_empty()) {
            if (!ep_tx_busy_flag) {
                ep_tx_busy_flag = true;
                usbd_ep_start_write(AUDIO_IN_EP, &s_in_buffer[s_in_buffer_front][0], AUDIO_IN_PACKET);
                s_in_buffer_front++;
                if (s_in_buffer_front >= AUDIO_BUFFER_COUNT) {
                    s_in_buffer_front = 0;
                }
            }
        }
    }
}

void i2s_enable_dma_irq_with_priority(int32_t priority)
{
    i2s_enable_rx_dma_request(PDM_I2S);
    dmamux_config(BOARD_APP_DMAMUX, MIC_DMAMUX_CHANNEL, HPM_DMA_SRC_I2S0_RX, true);
    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, priority);
}

void init_mic_i2s_pdm(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    pdm_config_t pdm_config;
    uint32_t i2s0_mclk_hz;

    i2s0_mclk_hz = clock_get_frequency(clock_i2s0);

    i2s_get_default_config(PDM_I2S, &i2s_config);
    i2s_init(PDM_I2S, &i2s_config);

    i2s_get_default_transfer_config_for_pdm(&transfer);
    transfer.data_line = I2S_DATA_LINE_0;
    transfer.sample_rate = AUDIO_FREQ;
    transfer.channel_slot_mask = BOARD_PDM_SINGLE_CHANNEL_MASK;

    s_mic_sample_rate = transfer.sample_rate;

    if (status_success != i2s_config_rx(PDM_I2S, i2s0_mclk_hz, &transfer)) {
        printf("I2S0 config failed for PDM\n");
        while (1)
            ;
    }
    i2s_start(PDM_I2S);

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);
}

void isr_dma(void)
{
    volatile hpm_stat_t stat;

    stat = dma_check_transfer_status(BOARD_APP_HDMA, MIC_DMA_CHANNEL);

    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        s_dma_transfer_done = true;
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_dma)

/* static function definition */
static void usbd_audio_iso_callback(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;
    ep_tx_busy_flag = false;
}

static void i2s_pdm_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);
    ch_config.src_addr = (uint32_t)(&PDM_I2S->RXD[I2S_DATA_LINE_0]) + 2u;
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = DMA_ALIGN_HALF_WORD(size);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, MIC_DMA_CHANNEL, &ch_config, true)) {
        printf(" pdm dma setup channel failed\n");
    }
}

static bool in_buff_is_full(void)
{
    bool full = false;
    uint8_t rear = s_in_buffer_rear + 1u;

    if (rear >= AUDIO_BUFFER_COUNT) {
        rear = 0u;
    }

    if (rear == s_in_buffer_front) {
        full = true;
    }

    return full;
}

static bool in_buff_is_empty(void)
{
    bool empty = false;

    if (s_in_buffer_front == s_in_buffer_rear) {
        empty = true;
    }

    return empty;
}
