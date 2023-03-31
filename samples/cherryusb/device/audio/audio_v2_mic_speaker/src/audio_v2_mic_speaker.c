/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_audio.h"
#include "board.h"
#include "hpm_i2s_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_pdm_drv.h"
#include "hpm_dao_drv.h"
#include "audio_v2_mic_speaker.h"


#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_OUT_EP 0x02
#define AUDIO_IN_EP  0x81

#define SPEAKER_SAMPLE_FREQ    96000
#define SPEAKER_SLOT_BYTE_SIZE 4
#define SPEAKER_AUDIO_DEPTH    24

#define MIC_SAMPLE_FREQ    16000
#define MIC_SLOT_BYTE_SIZE 2
#define MIC_AUDIO_DEPTH    16

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

#define AUDIO_BUFFER_COUNT 32
/* AudioFreq * DataSize * NumChannels */
#define AUDIO_OUT_PACKET ((uint32_t)((SPEAKER_SAMPLE_FREQ * SPEAKER_SLOT_BYTE_SIZE * OUT_CHANNEL_NUM) / 1000))
#define AUDIO_IN_PACKET  ((uint32_t)((MIC_SAMPLE_FREQ * MIC_SLOT_BYTE_SIZE * IN_CHANNEL_NUM) / 1000))

#define BMCONTROL (AUDIO_V2_FU_CONTROL_MUTE | AUDIO_V2_FU_CONTROL_VOLUME)

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

uint8_t audio_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, AUDIO_CATEGORY_HEADSET, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(0x01, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, 0x01, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x03, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, 0x03, 0x01, 0x0000),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(0x05, 0x03, 0x07),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_INTERM_MIC, 0x05, IN_CHANNEL_NUM, INPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x07, 0x06, INPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x08, AUDIO_TERMINAL_STREAMING, 0x07, 0x05, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, AUDIO_OUT_PACKET, EP_INTERVAL),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, (AUDIO_IN_PACKET + 4), EP_INTERVAL),
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
    '5', 0x00,                  /* wcChar9 */
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

static const uint8_t speaker_default_sampling_freq_table[] = {
    AUDIO_SAMPLE_FREQ_NUM(5),
    AUDIO_SAMPLE_FREQ_4B(8000),
    AUDIO_SAMPLE_FREQ_4B(8000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(32000),
    AUDIO_SAMPLE_FREQ_4B(32000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(0x00)
};

static const uint8_t mic_default_sampling_freq_table[] = {
    AUDIO_SAMPLE_FREQ_NUM(1),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00)
};

/* Static Functions Declaration */
static void usbd_audio_iso_out_callback(uint8_t ep, uint32_t nbytes);
static void usbd_audio_iso_in_callback(uint8_t ep, uint32_t nbytes);
static hpm_stat_t speaker_init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool speaker_out_buff_is_empty(void);
static bool mic_in_buff_is_empty(void);

/* static variable definition */
#define DMA_MUX_CHANNEL_SPEAKER 1U
#define DMA_MUX_CHANNEL_MIC     2U

#define MIC_I2S               HPM_I2S0
#define MIC_I2S_CLK_NAME      clock_i2s0
#define MIC_I2S_DATA_LINE     I2S_DATA_LINE_0
#define MIC_I2S_RX_DMAMUX_SRC HPM_DMA_SRC_I2S0_RX

#define SPEAKER_I2S               HPM_I2S1
#define SPEAKER_I2S_CLK_NAME      clock_i2s1
#define SPEAKER_I2S_DATA_LINE     I2S_DATA_LINE_0
#define SPEAKER_I2S_TX_DMAMUX_SRC HPM_DMA_SRC_I2S1_TX

/* Static Variables */
#define AUDIO_BUFFER_COUNT 32
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_speaker_out_buffer[AUDIO_BUFFER_COUNT][AUDIO_OUT_PACKET];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t s_mic_in_buffer[AUDIO_BUFFER_COUNT][AUDIO_IN_PACKET];
static uint32_t s_speaker_out_buffer_size[AUDIO_BUFFER_COUNT];
static volatile uint32_t s_speaker_i2s_mclk_hz;
static volatile bool s_speaker_rx_flag;
static volatile uint8_t s_speaker_out_buffer_front;
static volatile uint8_t s_speaker_out_buffer_rear;
static volatile bool s_speaker_dma_transfer_req;
static volatile bool s_speaker_dma_transfer_done;
static volatile bool s_speaker_mute;
static volatile bool s_set_speaker_mute_req;
static volatile float s_speaker_volume_percent;
static volatile bool s_set_speaker_volume_req;
static volatile uint32_t s_speaker_sampling_freq;
static volatile bool s_set_speaker_sampling_freq_req;
static volatile bool s_mic_tx_flag;
static volatile bool s_mic_ep_tx_busy_flag;
static volatile uint8_t s_mic_in_buffer_front;
static volatile uint8_t s_mic_in_buffer_rear;
static volatile bool s_mic_dma_transfer_done;
static volatile bool s_mic_mute;
static volatile bool s_set_mic_mute_req;
static struct usbd_interface intf0;
static struct usbd_interface intf1;
static struct usbd_interface intf2;
static struct usbd_endpoint audio_in_ep = {
    .ep_cb = usbd_audio_iso_in_callback,
    .ep_addr = AUDIO_IN_EP
};
static struct usbd_endpoint audio_out_ep = {
    .ep_cb = usbd_audio_iso_out_callback,
    .ep_addr = AUDIO_OUT_EP
};

/* Extern Functions Definition */
void cherryusb_audio_v2_init(void)
{
    usbd_desc_register(audio_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&intf0));
    usbd_add_interface(usbd_audio_init_intf(&intf1));
    usbd_add_interface(usbd_audio_init_intf(&intf2));
    usbd_add_endpoint(&audio_in_ep);
    usbd_add_endpoint(&audio_out_ep);

    usbd_audio_add_entity(0x01, AUDIO_CONTROL_CLOCK_SOURCE);
    usbd_audio_add_entity(0x03, AUDIO_CONTROL_FEATURE_UNIT);
    usbd_audio_add_entity(0x05, AUDIO_CONTROL_CLOCK_SOURCE);
    usbd_audio_add_entity(0x07, AUDIO_CONTROL_FEATURE_UNIT);

    usbd_initialize();
}

void speaker_init_i2s_dao_codec(void)
{
    dao_config_t dao_config;

    (void)speaker_init_i2s_playback(SPEAKER_SAMPLE_FREQ, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);

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
    transfer.channel_slot_mask = 0x03; /* 2 channels */

    clock_set_i2s_source(MIC_I2S_CLK_NAME, clk_i2s_src_aud0);
    i2s_mclk_hz = clock_get_frequency(MIC_I2S_CLK_NAME);

    if (status_success != i2s_config_rx(MIC_I2S, i2s_mclk_hz, &transfer)) {
        printf("MIC I2S1 config failed\n");
        while (1) {
            ;
        }
    }

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);
}

void i2s_enable_dma_irq_with_priority(int32_t priority)
{
    i2s_enable_tx_dma_request(SPEAKER_I2S);
    i2s_enable_rx_dma_request(MIC_I2S);
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL_SPEAKER, SPEAKER_I2S_TX_DMAMUX_SRC, true);
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL_MIC, MIC_I2S_RX_DMAMUX_SRC, true);

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, priority);
}

void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_HDMA, DMA_MUX_CHANNEL_SPEAKER);
    mic_status = dma_check_transfer_status(BOARD_APP_HDMA, DMA_MUX_CHANNEL_MIC);

    if (0 != (speaker_status & DMA_CHANNEL_STATUS_TC)) {
        s_speaker_dma_transfer_done = true;
    }

    if (0 != (mic_status & DMA_CHANNEL_STATUS_TC)) {
        s_mic_dma_transfer_done = true;
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_dma)

void cherryusb_audio_v2_main_task(void)
{
    hpm_stat_t state;

    if (s_set_speaker_sampling_freq_req) {
        state = speaker_init_i2s_playback(s_speaker_sampling_freq, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);
        if (state == status_success) {
            USB_LOG_RAW("Init I2S Clock Ok! Sample Rate: %d, speaker_i2s_mclk_hz: %d\r\n", s_speaker_sampling_freq, s_speaker_i2s_mclk_hz);
        } else {
            USB_LOG_RAW("Init I2S Clock Fail!\r\n");
        }
        s_speaker_out_buffer_front = s_speaker_out_buffer_rear;
        s_speaker_dma_transfer_req = true;
        s_set_speaker_sampling_freq_req = false;
    }

    if (s_set_speaker_mute_req) {
        if (s_speaker_mute) {
            dao_stop(HPM_DAO);
        } else {
            dao_start(HPM_DAO);
        }
        s_set_speaker_mute_req = false;
    }

    if (s_set_speaker_volume_req) {
        /* DAO Do Nothing */
        s_set_speaker_volume_req = false;
    }

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

    if (s_set_mic_mute_req) {
        if (s_mic_mute) {
            pdm_stop(HPM_PDM);
        } else {
            pdm_start(HPM_PDM);
        }
        s_set_mic_mute_req = false;
    }

    if (s_mic_tx_flag) {
        if (s_mic_dma_transfer_done) {
            s_mic_dma_transfer_done = false;
            s_mic_in_buffer_rear++;
            if (s_mic_in_buffer_rear >= AUDIO_BUFFER_COUNT) {
                s_mic_in_buffer_rear = 0;
            }
            mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_rear][0], AUDIO_IN_PACKET);
        }

        if (!mic_in_buff_is_empty()) {
            if (!s_mic_ep_tx_busy_flag) {
                s_mic_ep_tx_busy_flag = true;
                usbd_ep_start_write(AUDIO_IN_EP, &s_mic_in_buffer[s_mic_in_buffer_front][0], AUDIO_IN_PACKET);
                s_mic_in_buffer_front++;
                if (s_mic_in_buffer_front >= AUDIO_BUFFER_COUNT) {
                    s_mic_in_buffer_front = 0;
                }
            }
        }
    }
}

void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        s_speaker_rx_flag = 1;
        s_speaker_out_buffer_front = 0;
        s_speaker_out_buffer_rear = 0;
        s_speaker_dma_transfer_req = true;
        usbd_ep_start_read(AUDIO_OUT_EP, (uint8_t *)&s_speaker_out_buffer[s_speaker_out_buffer_rear][0], AUDIO_OUT_PACKET);
        dao_start(HPM_DAO);
        USB_LOG_RAW("OPEN SPEAKER\r\n");
    } else {
        s_mic_tx_flag = 1;
        s_mic_ep_tx_busy_flag = false;
        s_mic_in_buffer_front = 0;
        s_mic_in_buffer_rear = 0;
        s_mic_dma_transfer_done = false;
        pdm_start(HPM_PDM);
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_rear][0], AUDIO_IN_PACKET);
        USB_LOG_RAW("OPEN MIC\r\n");
    }
}

void usbd_audio_close(uint8_t intf)
{
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

void usbd_configure_done_callback(void)
{
}

void usbd_audio_set_volume(uint8_t entity_id, uint8_t ch, float dB)
{
    if (entity_id == 0x03) {
        s_speaker_volume_percent = dB;
        s_set_speaker_volume_req = true;
    }
}

void usbd_audio_set_mute(uint8_t entity_id, uint8_t ch, uint8_t enable)
{
    if (entity_id == 0x03) {
        s_speaker_mute = enable;
        s_set_speaker_mute_req = true;
    }

    if (entity_id == 0x07) {
        s_mic_mute = enable;
        s_set_mic_mute_req = true;
    }
}

void usbd_audio_set_sampling_freq(uint8_t entity_id, uint8_t ep_ch, uint32_t sampling_freq)
{
    if (entity_id == 1) {
        s_speaker_sampling_freq = sampling_freq;
        s_set_speaker_sampling_freq_req = true;
    } else if (entity_id == 5) {
    } else {
    }
}

void usbd_audio_get_sampling_freq_table(uint8_t entity_id, uint8_t **sampling_freq_table)
{
    if (entity_id == 0x01) {
        *sampling_freq_table = (uint8_t *)speaker_default_sampling_freq_table;
    } else if (entity_id == 0x05) {
        *sampling_freq_table = (uint8_t *)mic_default_sampling_freq_table;
    } else {
    }
}

/* Static Function Definition */
static void usbd_audio_iso_out_callback(uint8_t ep, uint32_t nbytes)
{
    if (s_speaker_rx_flag) {
        s_speaker_out_buffer_size[s_speaker_out_buffer_rear] = nbytes;
        s_speaker_out_buffer_rear++;
        if (s_speaker_out_buffer_rear >= AUDIO_BUFFER_COUNT) {
            s_speaker_out_buffer_rear = 0;
        }
        usbd_ep_start_read(ep, &s_speaker_out_buffer[s_speaker_out_buffer_rear][0], AUDIO_OUT_PACKET);
    }
}

static void usbd_audio_iso_in_callback(uint8_t ep, uint32_t nbytes)
{
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
    transfer.channel_slot_mask = 0x3;   /* data from hpm_wav_decode API is 2 channels */

    if ((sample_rate % 44100) == 0) {
        /* clock_aud1 has been configured for 44100*n sample rate*/
        clock_set_i2s_source(SPEAKER_I2S_CLK_NAME, clk_i2s_src_aud1);
    } else {
        clock_set_i2s_source(SPEAKER_I2S_CLK_NAME, clk_i2s_src_aud0);
    }

    s_speaker_i2s_mclk_hz = clock_get_frequency(SPEAKER_I2S_CLK_NAME);

    if (status_success != i2s_config_tx(SPEAKER_I2S, s_speaker_i2s_mclk_hz, &transfer)) {
        return status_fail;
    }

    return status_success;
}

static void speaker_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&SPEAKER_I2S->TXD[SPEAKER_I2S_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = size;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = 0;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, DMA_MUX_CHANNEL_SPEAKER, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
}

static void mic_i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);
    ch_config.src_addr = (uint32_t)(&MIC_I2S->RXD[MIC_I2S_DATA_LINE]) + 2u;
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = size;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, DMA_MUX_CHANNEL_MIC, &ch_config, true)) {
        printf(" pdm dma setup channel failed\n");
        return;
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
