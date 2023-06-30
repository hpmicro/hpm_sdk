/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "assert.h"

#include "usbd_core.h"
#include "usbd_audio.h"
#include "board.h"
#include "hpm_i2s_drv.h"
#ifdef CONFIG_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_pdm_drv.h"
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

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_OUT_EP 0x02
#define AUDIO_IN_EP  0x81

#define SPEAKER_MAX_SAMPLE_FREQ    96000
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

#define AUDIO_BUFFER_COUNT 32
/* AudioFreq * DataSize * NumChannels */
#define AUDIO_OUT_PACKET ((uint32_t)((SPEAKER_MAX_SAMPLE_FREQ * SPEAKER_SLOT_BYTE_SIZE * OUT_CHANNEL_NUM) / 1000))
#define AUDIO_IN_PACKET  ((uint32_t)((MIC_SAMPLE_FREQ * MIC_SLOT_BYTE_SIZE * IN_CHANNEL_NUM) / 1000))

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
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, SPEAKER_SLOT_BYTE_SIZE, SPEAKER_AUDIO_DEPTH, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET, EP_INTERVAL),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x02, 0x08, IN_CHANNEL_NUM, INPUT_CH_ENABLE, MIC_SLOT_BYTE_SIZE, MIC_AUDIO_DEPTH, AUDIO_IN_EP, 0x05, (AUDIO_IN_PACKET + 4), EP_INTERVAL),
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
static volatile float s_speaker_volume_percent;
static volatile uint32_t s_speaker_sampling_freq;
static volatile uint8_t s_mic_in_buffer_front;
static volatile uint8_t s_mic_in_buffer_rear;
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

QueueHandle_t queue_speaker_ctl, queue_mic_ctl;
SemaphoreHandle_t binsema_dma_speaker_done, binsema_dma_mic_done;
SemaphoreHandle_t countsema_speaker_data, countsema_mic_data;
SemaphoreHandle_t binsema_dma_speaker_transfer_req;
SemaphoreHandle_t binsema_tx_resource;
TaskHandle_t task_handle_speaker_play;
TaskHandle_t task_handle_mic_play;

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define EVENT_BIT_SPEAKER_ENABLE    BIT_0
#define EVENT_BIT_SPEAKER_DISABLE   BIT_1
#define EVENT_BIT_MIC_ENABLE        BIT_2
#define EVENT_BIT_MIC_DISABLE       BIT_3

EventGroupHandle_t event_group;
/* Extern Functions Definition */
void cherryusb_audio_v2_init(void)
{
    queue_speaker_ctl = xQueueCreate(10, 1);
    assert(queue_speaker_ctl != NULL);
    queue_mic_ctl = xQueueCreate(10, 1);
    assert(queue_mic_ctl != NULL);
    binsema_dma_speaker_done = xSemaphoreCreateBinary();
    assert(binsema_dma_speaker_done != NULL);
    binsema_dma_mic_done = xSemaphoreCreateBinary();
    assert(binsema_dma_mic_done != NULL);
    binsema_dma_speaker_transfer_req = xSemaphoreCreateBinary();
    assert(binsema_dma_speaker_transfer_req != NULL);
    binsema_tx_resource = xSemaphoreCreateBinary();
    assert(binsema_tx_resource);
    event_group = xEventGroupCreate();
    assert(event_group != NULL);
    countsema_speaker_data = xSemaphoreCreateCounting(10, 0);
    assert(countsema_speaker_data != NULL);
    countsema_mic_data = xSemaphoreCreateCounting(10, 0);
    assert(countsema_mic_data != NULL);

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

    (void)speaker_init_i2s_playback(16000, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);

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
        USB_LOG_ERR("MIC I2S1 config failed\r\n");
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

void check_and_restart_mic_dma(EventBits_t event)
{
    /* stop event is not set, restart dma transfer */
    if ((event & (2 << 8)) == 0) {
        s_mic_in_buffer_rear++;
        if (s_mic_in_buffer_rear >= AUDIO_BUFFER_COUNT) {
            s_mic_in_buffer_rear = 0;
        }
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_rear][0], AUDIO_IN_PACKET);
    }
}

bool check_and_restart_speaker_dma(EventBits_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* stop event is not set, try take resource and restart dma */
    if ((event & (2)) == 0) {
        if (xSemaphoreTakeFromISR(countsema_speaker_data, &xHigherPriorityTaskWoken) == pdTRUE) {
            speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[s_speaker_out_buffer_front][0],
                s_speaker_out_buffer_size[s_speaker_out_buffer_front]);
            s_speaker_out_buffer_front++;
            if (s_speaker_out_buffer_front >= AUDIO_BUFFER_COUNT) {
                s_speaker_out_buffer_front = 0;
            }
            return true;
        }
    }
    return false;
}

void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;
    BaseType_t xHigherPriorityTaskWoken1 = pdFALSE, xHigherPriorityTaskWoken2 = pdFALSE;
    int ret1 = pdFALSE, ret2 = pdFALSE;
    speaker_status = dma_check_transfer_status(BOARD_APP_HDMA, DMA_MUX_CHANNEL_SPEAKER);
    mic_status = dma_check_transfer_status(BOARD_APP_HDMA, DMA_MUX_CHANNEL_MIC);

    EventBits_t event = 0;
    event = xEventGroupGetBitsFromISR(event_group);
    if (0 != (speaker_status & DMA_CHANNEL_STATUS_TC)) {
        /* true: restarted  false: not restarted, tell task to loop for data from host */
        if (!check_and_restart_speaker_dma(event)) {
            ret1 = xSemaphoreGiveFromISR(binsema_dma_speaker_done, &xHigherPriorityTaskWoken1);
        }
    }

    if (0 != (mic_status & DMA_CHANNEL_STATUS_TC)) {
        check_and_restart_mic_dma(event);
        ret2 = xSemaphoreGiveFromISR(countsema_mic_data, &xHigherPriorityTaskWoken2);
    }
    if (ret1 == pdPASS) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken1);
    }
    if (ret2 == pdPASS) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken2);
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_dma)

void request_handler_speaker_set_sampling_freq(void)
{
    hpm_stat_t state;
    state = speaker_init_i2s_playback(s_speaker_sampling_freq, SPEAKER_AUDIO_DEPTH, OUT_CHANNEL_NUM);
    if (state == status_success) {
        USB_LOG_RAW("Init I2S Clock Ok! Sample Rate: %d, speaker_i2s_mclk_hz: %d\r\n", s_speaker_sampling_freq, s_speaker_i2s_mclk_hz);
    } else {
        USB_LOG_RAW("Init I2S Clock Fail!\r\n");
    }
    s_speaker_out_buffer_front = s_speaker_out_buffer_rear;

}

void task_speaker_control(void *pvParameters)
{
    uint8_t cmd = 0;
    while (1) {
        if (xQueueReceive(queue_speaker_ctl, &cmd, portMAX_DELAY) != pdTRUE)
            continue;
        switch (cmd) {
        case SPEAKER_SET_SAMPLING_FREQ_REQ:
            request_handler_speaker_set_sampling_freq();
        break;
        case SPEAKER_SET_MUTE_DISABLE_REQ:
            dao_start(HPM_DAO);
        break;
        case SPEAKER_SET_MUTE_ENABLE_REQ:
            dao_stop(HPM_DAO);
        break;
        case SPEAKER_START_REQ:
            xEventGroupClearBits(event_group, EVENT_BIT_SPEAKER_DISABLE);
            xEventGroupSetBits(event_group, EVENT_BIT_SPEAKER_ENABLE);
        break;
        case SPEAKER_STOP_REQ:
            xEventGroupClearBits(event_group, EVENT_BIT_SPEAKER_ENABLE);
            xEventGroupSetBits(event_group, EVENT_BIT_SPEAKER_DISABLE);
        break;
        case SPEAKER_SET_VOLUME_REQ:
        /* do nothing */
        break;
        default:
        break;
        }
    }
}

void task_speaker_play(void *pvParameters)
{
    EventBits_t event = 0;
    while (1) {
        /* wait host open speaker */
        event = xEventGroupWaitBits(event_group, EVENT_BIT_SPEAKER_ENABLE, pdTRUE, 0, portMAX_DELAY);
        if ((event & EVENT_BIT_SPEAKER_ENABLE) != 0) {
            USB_LOG_RAW("%s get start cmd\r\n", __func__);
        }

        while (1) {
            /* data valid!!! */
            if (xSemaphoreTake(countsema_speaker_data, portMAX_DELAY) != pdTRUE) {
                USB_LOG_ERR("error while take speaker resources %d\r\n", __LINE__);
                goto check_stop_cmd1;
            }
            if (!speaker_out_buff_is_empty()) {
                speaker_i2s_dma_start_transfer((uint32_t)&s_speaker_out_buffer[s_speaker_out_buffer_front][0],
                                               s_speaker_out_buffer_size[s_speaker_out_buffer_front]);
                s_speaker_out_buffer_front++;
                if (s_speaker_out_buffer_front >= AUDIO_BUFFER_COUNT) {
                    s_speaker_out_buffer_front = 0;
                }
            }
            /* wait dma done. While in dma isr there is more data from host, restarted transfer in isr, not
               return to this code */
            if (xSemaphoreTake(binsema_dma_speaker_done, portMAX_DELAY) == pdTRUE) {
            }
check_stop_cmd1:
            event = xEventGroupGetBits(event_group);
            if ((event & EVENT_BIT_SPEAKER_DISABLE) != 0) {
                xEventGroupClearBits(event_group, EVENT_BIT_SPEAKER_DISABLE);
                break;
            }
        }
    }
}

void task_mic_play(void *pvParameters)
{
    EventBits_t event = 0;
    while (1) {
        /* wait host open mic */
        event = xEventGroupWaitBits(event_group, EVENT_BIT_MIC_ENABLE, pdTRUE, 0, portMAX_DELAY);
        if ((event & EVENT_BIT_MIC_ENABLE) != 0) {
            USB_LOG_RAW("%s mic get start cmd\n", __func__);
        }
        while (1) {
            /* data valid!!! */
            if (xSemaphoreTake(countsema_mic_data, portMAX_DELAY) != pdTRUE) {
                USB_LOG_ERR("error while take mic resources %d\n", __LINE__);
                goto check_stop_cmd2;
            }
            if (!mic_in_buff_is_empty()) {
                if (xSemaphoreTake(binsema_tx_resource, portMAX_DELAY) == pdTRUE) {
                    usbd_ep_start_write(AUDIO_IN_EP, &s_mic_in_buffer[s_mic_in_buffer_front][0], AUDIO_IN_PACKET);
                    s_mic_in_buffer_front++;
                    if (s_mic_in_buffer_front >= AUDIO_BUFFER_COUNT) {
                        s_mic_in_buffer_front = 0;
                    }
                }
            }

check_stop_cmd2:
            event = xEventGroupGetBits(event_group);
            if ((event & EVENT_BIT_MIC_DISABLE) != 0) {
                xEventGroupClearBits(event_group, EVENT_BIT_MIC_DISABLE);
                break;
            }
        }

    }

}

void task_mic_control(void *pvParameters)
{
    uint8_t cmd = 0;

    while (1) {
        if (xQueueReceive(queue_mic_ctl, &cmd, portMAX_DELAY) != pdTRUE)
            continue;
        switch (cmd) {
        case MIC_SET_ENABLE_REQ:
            xEventGroupClearBits(event_group, EVENT_BIT_MIC_DISABLE);
            xEventGroupSetBits(event_group, EVENT_BIT_MIC_ENABLE);
        break;
        case MIC_SET_DISABLE_REQ:
            xEventGroupClearBits(event_group, EVENT_BIT_MIC_ENABLE);
            xEventGroupSetBits(event_group, EVENT_BIT_MIC_DISABLE);
        break;
        case MIC_SET_MUTE_DISABLE_REQ:
            pdm_start(HPM_PDM);
            xEventGroupClearBits(event_group, EVENT_BIT_MIC_ENABLE);
            xEventGroupSetBits(event_group, EVENT_BIT_MIC_DISABLE);
        break;
        case MIC_SET_MUTE_ENABLE_REQ:
            pdm_stop(HPM_PDM);
            xEventGroupClearBits(event_group, EVENT_BIT_MIC_DISABLE);
            xEventGroupSetBits(event_group, EVENT_BIT_MIC_ENABLE);
        break;
        default:
        break;
        }
    }
}

void usbd_audio_open(uint8_t intf)
{
    uint8_t cmd = 0;
    if (intf == 1) {
        s_speaker_rx_flag = 1;
        s_speaker_out_buffer_front = 0;
        s_speaker_out_buffer_rear = 0;
        usbd_ep_start_read(AUDIO_OUT_EP, (uint8_t *)&s_speaker_out_buffer[s_speaker_out_buffer_rear][0], AUDIO_OUT_PACKET);
        dao_start(HPM_DAO);
        cmd = SPEAKER_START_REQ;
        xQueueSend(queue_speaker_ctl, &cmd, 0);
        USB_LOG_RAW("OPEN SPEAKER\r\n");
    } else {
        xSemaphoreGive(binsema_tx_resource);
        s_mic_in_buffer_front = 0;
        s_mic_in_buffer_rear = 0;
        pdm_start(HPM_PDM);
        mic_i2s_dma_start_transfer((uint32_t)&s_mic_in_buffer[s_mic_in_buffer_rear][0], AUDIO_IN_PACKET);
        USB_LOG_RAW("OPEN MIC\r\n");
        cmd = MIC_SET_ENABLE_REQ;
        xQueueSend(queue_mic_ctl, &cmd, 0);
    }
}

void usbd_audio_close(uint8_t intf)
{
    uint8_t cmd = 0;
    if (intf == 1) {
        s_speaker_rx_flag = 0;
        dao_stop(HPM_DAO);
        cmd = SPEAKER_STOP_REQ;
        xQueueSend(queue_speaker_ctl, &cmd, 0);
        USB_LOG_RAW("CLOSE SPEAKER\r\n");
    } else {
        pdm_stop(HPM_PDM);
        cmd = MIC_SET_DISABLE_REQ;
        xQueueSend(queue_mic_ctl, &cmd, 0);
        USB_LOG_RAW("CLOSE MIC\r\n");
    }
}

void usbd_configure_done_callback(void)
{
}

void usbd_audio_set_volume(uint8_t entity_id, uint8_t ch, float dB)
{
    uint8_t cmd = SPEAKER_SET_VOLUME_REQ;
    if (entity_id == 0x03) {
        s_speaker_volume_percent = dB;
        xQueueSend(queue_speaker_ctl, &cmd, 0);
    }
}

void usbd_audio_set_mute(uint8_t entity_id, uint8_t ch, uint8_t enable)
{
    uint8_t cmd = 0;
    if (entity_id == 0x03) {
        if (enable)
            cmd = SPEAKER_SET_MUTE_ENABLE_REQ;
        else
            cmd = SPEAKER_SET_MUTE_DISABLE_REQ;
        xQueueSend(queue_speaker_ctl, &cmd, 0);
    }

    if (entity_id == 0x07) {
        if (enable)
            cmd = MIC_SET_MUTE_ENABLE_REQ;
        else
            cmd = MIC_SET_MUTE_DISABLE_REQ;
        xQueueSend(queue_mic_ctl, &cmd, 0);
    }
}

void usbd_audio_set_sampling_freq(uint8_t entity_id, uint8_t ep_ch, uint32_t sampling_freq)
{
    uint8_t cmd = 0;
    if (entity_id == 1) {
        s_speaker_sampling_freq = sampling_freq;
        cmd = SPEAKER_SET_SAMPLING_FREQ_REQ;
        xQueueSend(queue_speaker_ctl, &cmd, 0);
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
        xSemaphoreGive(countsema_speaker_data);
    }
}

static void usbd_audio_iso_in_callback(uint8_t ep, uint32_t nbytes)
{
    xSemaphoreGive(binsema_tx_resource);
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