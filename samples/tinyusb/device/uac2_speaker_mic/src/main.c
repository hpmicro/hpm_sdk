/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jerzy Kasenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "hpm_i2s_drv.h"
#include "hpm_dao_drv.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_PDMLITE
#include "hpm_pdmlite_drv.h"
#else
#include "hpm_pdm_drv.h"
#endif

/*
 * MACRO CONSTANT TYPEDEF PROTOTYPES
 */
enum {
    VOLUME_CTRL_0_DB = 0,
    VOLUME_CTRL_10_DB = 2560,
    VOLUME_CTRL_20_DB = 5120,
    VOLUME_CTRL_30_DB = 7680,
    VOLUME_CTRL_40_DB = 10240,
    VOLUME_CTRL_50_DB = 12800,
    VOLUME_CTRL_60_DB = 15360,
    VOLUME_CTRL_70_DB = 17920,
    VOLUME_CTRL_80_DB = 20480,
    VOLUME_CTRL_90_DB = 23040,
    VOLUME_CTRL_100_DB = 25600,
    VOLUME_CTRL_SILENCE = 0x8000,
};

/* Macro Const Declaration */
#define I2S_MCLK_FREQ_IN_HZ (24576000UL)

#define SPEAKER_DMA_CHANNEL 1U
#define MIC_DMA_CHANNEL     2U
#define SPEAKER_DMAMUX_CHANNEL    DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL)
#define MIC_DMAMUX_CHANNEL        DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_XDMA, MIC_DMA_CHANNEL)

#define PDM_I2S_CLK_NAME      BOARD_MIC_I2S_CLK_NAME
#define PDM_I2S_DATA_LINE     BOARD_MIC_I2S_DATA_LINE
#define PDM_I2S_RX_DMAMUX_SRC BOARD_MIC_I2S_RX_DMAMUX_SRC

#define DAO_I2S_CLK_NAME      BOARD_SPEAKER_I2S_CLK_NAME
#define DAO_I2S_DATA_LINE     BOARD_SPEAKER_I2S_DATA_LINE
#define DAO_I2S_TX_DMAMUX_SRC BOARD_SPEAKER_I2S_TX_DMAMUX_SRC

#define N_SAMPLE_RATES  TU_ARRAY_SIZE(sample_rates)

/* Variable Definition */
volatile uint32_t current_sample_rate;

/* List of supported sample rates */
const uint32_t sample_rates[] = {16000};

/* Audio controls */
/* Current states */
int8_t spk_mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];       /* +1 for master channel 0 */
int16_t spk_volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];    /* +1 for master channel 0 */
int8_t mic_mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1];       /* +1 for master channel 0 */
int16_t mic_volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1];    /* +1 for master channel 0 */

/* Buffer for speaker data */
#define AUDIO_BUFFER_COUNT 32
ATTR_PLACE_AT_NONCACHEABLE uint8_t spk_buf[AUDIO_BUFFER_COUNT][CFG_TUD_AUDIO_EP_SZ_OUT];
ATTR_PLACE_AT_NONCACHEABLE uint32_t spk_buf_size[AUDIO_BUFFER_COUNT];
ATTR_PLACE_AT_NONCACHEABLE uint8_t mic_buf[AUDIO_BUFFER_COUNT][CFG_TUD_AUDIO_EP_SZ_IN];
static volatile bool s_spk_rx_flag;
static volatile uint8_t s_spk_buf_front;
static volatile uint8_t s_spk_buf_rear;
static volatile bool s_spk_dma_transfer_req;
static volatile bool s_spk_dma_transfer_done;
static volatile bool s_mic_tx_flag;
static volatile uint8_t s_mic_buf_front;
static volatile uint8_t s_mic_buf_rear;
static volatile bool s_mic_dma_transfer_done;


void i2s_pdm_dma_cfg(uint32_t size, uint32_t *ptr);
void reinit_dao_i2s_cfg(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
void audio_task(void);
bool speaker_out_buff_is_empty(void);
bool mic_in_buff_is_empty(void);

/* PDM */
void pdm_config(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    pdm_config_t pdm_config;

    i2s_get_default_config(PDM_I2S, &i2s_config);
    i2s_init(PDM_I2S, &i2s_config);

    /*
     * config transfer for PDM
     */
    i2s_get_default_transfer_config_for_pdm(&transfer);
    /*
     * enable mic0 @ RXD0
     */
    transfer.data_line = PDM_I2S_DATA_LINE;
    transfer.channel_slot_mask = BOARD_PDM_DUAL_CHANNEL_MASK; /* 2 channels */
    if (status_success != i2s_config_rx(PDM_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer)) {
        printf("I2S config failed for PDM\n");
        while (1) {
            ;
        }
    }
    i2s_start(PDM_I2S);

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);

    i2s_enable_rx_dma_request(PDM_I2S);
    dmamux_config(BOARD_APP_DMAMUX, MIC_DMAMUX_CHANNEL, PDM_I2S_RX_DMAMUX_SRC, true);
}

void dao_config(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;

    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_init(DAO_I2S, &i2s_config);
    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = 16000;
    transfer.audio_depth = CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX;
    transfer.channel_slot_mask = 0x3;
    if (status_success != i2s_config_tx(DAO_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer)) {
        printf("I2S config failed for DAO\n");
        while (1) {
            ;
        }
    }
    i2s_start(DAO_I2S);

    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);

    i2s_enable_tx_dma_request(DAO_I2S);
    dmamux_config(BOARD_APP_DMAMUX, SPEAKER_DMAMUX_CHANNEL, DAO_I2S_TX_DMAMUX_SRC, true);
}

/*------------- MAIN -------------*/
int main(void)
{
    board_init();

    board_init_dao_clock();
    board_init_pdm_clock();

    init_dao_pins();
    init_pdm_pins();

    board_init_led_pins();
    if (BOARD_DEVICE_RHPORT_NUM == 0) {
        board_init_usb(HPM_USB0);
#ifdef HPM_USB1
    } else if (BOARD_DEVICE_RHPORT_NUM == 1) {
        board_init_usb(HPM_USB1);
#endif
    } else {
        printf("Don't support HPM_USB%d!\n", BOARD_DEVICE_RHPORT_NUM);
        while (1) {
            ;
        }
    }

    pdm_config();
    dao_config();

    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, 1);
    intc_set_irq_priority(IRQn_USB0, 2);

    printf("USB%d Device - UAC2 Speaker Demo\r\n", BOARD_DEVICE_RHPORT_NUM);

    tusb_init();

    while (1) {
        tud_task();
        audio_task();
    }

    return 0;
}

/*
 * Device callbacks
 */

/* Invoked when device is mounted */
void tud_mount_cb(void)
{
}

/* Invoked when device is unmounted */
void tud_umount_cb(void)
{
}

/* Invoked when usb bus is suspended */
/* remote_wakeup_en : if host allow us  to perform remote wakeup */
/* Within 7ms, device must draw an average of current less than 2.5 mA from bus */
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
}

/* Invoked when usb bus is resumed */
void tud_resume_cb(void)
{
}

/* Helper for clock get requests */
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    TU_ASSERT((request->bEntityID == UAC2_ENTITY_SPK_CLOCK) || (request->bEntityID == UAC2_ENTITY_MIC_CLOCK));

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ) {
        if (request->bRequest == AUDIO_CS_REQ_CUR) {
            TU_LOG1("Clock get current freq %u\r\n", current_sample_rate);

            audio_control_cur_4_t curf = { tu_htole32(current_sample_rate) };
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
        } else if (request->bRequest == AUDIO_CS_REQ_RANGE) {
            audio_control_range_4_n_t(N_SAMPLE_RATES) rangef = {
                .wNumSubRanges = tu_htole16(N_SAMPLE_RATES)
            };
            TU_LOG1("Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
            for (uint8_t i = 0; i < N_SAMPLE_RATES; i++) {
                rangef.subrange[i].bMin = sample_rates[i];
                rangef.subrange[i].bMax = sample_rates[i];
                rangef.subrange[i].bRes = 0;
                TU_LOG1("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
            }

            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
            }
    } else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
        request->bRequest == AUDIO_CS_REQ_CUR) {
        audio_control_cur_1_t cur_valid = { .bCur = 1 };
        TU_LOG1("Clock get is valid %u\r\n", cur_valid.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
    }
    TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
        request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

/* Helper for clock set requests */
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    TU_ASSERT((request->bEntityID == UAC2_ENTITY_SPK_CLOCK) || (request->bEntityID == UAC2_ENTITY_MIC_CLOCK));
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ) {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

        current_sample_rate = ((audio_control_cur_4_t const *)buf)->bCur;

        TU_LOG1("Clock set current freq: %d\r\n", current_sample_rate);

        if (request->bEntityID == UAC2_ENTITY_SPK_CLOCK) {
            reinit_dao_i2s_cfg(current_sample_rate, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX);
        }
        return true;
    } else {
        TU_LOG1("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

/* Helper for feature unit get requests */
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) {
        if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR) {
            audio_control_cur_1_t mute1 = { .bCur = spk_mute[request->bChannelNumber] };
            TU_LOG1("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
        } else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
            if (request->bRequest == AUDIO_CS_REQ_RANGE) {
                audio_control_range_2_n_t(1) range_vol = {
                    .wNumSubRanges = tu_htole16(1),
                    .subrange[0] = { .bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256) }
                };
                TU_LOG1("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
                        range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
                return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
            } else if (request->bRequest == AUDIO_CS_REQ_CUR) {
                audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(spk_volume[request->bChannelNumber]) };

                TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
                return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
            }
        }
    }

    if (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) {
        if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR) {
            audio_control_cur_1_t mute1 = { .bCur = mic_mute[request->bChannelNumber] };
            TU_LOG1("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
        } else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
            if (request->bRequest == AUDIO_CS_REQ_RANGE) {
                audio_control_range_2_n_t(1) range_vol = {
                    .wNumSubRanges = tu_htole16(1),
                    .subrange[0] = { .bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256) }
                };
                TU_LOG1("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
                        range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
                return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
            } else if (request->bRequest == AUDIO_CS_REQ_CUR) {
                audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(mic_volume[request->bChannelNumber]) };

                TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
                return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
            }
        }
    }

    TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

/* Helper for feature unit set requests */
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE) {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

        if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) {
            spk_mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

            TU_LOG1("Set spk channel %d Mute: %d\r\n", request->bChannelNumber, spk_mute[request->bChannelNumber]);

            if (spk_mute[request->bChannelNumber]) {
                dao_stop(HPM_DAO);
            } else {
                dao_start(HPM_DAO);
            }
        } else if (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) {
            mic_mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

            TU_LOG1("Set mic channel %d Mute: %d\r\n", request->bChannelNumber, mic_mute[request->bChannelNumber]);

            if (mic_mute[request->bChannelNumber]) {
                pdm_stop(HPM_PDM);
            } else {
                pdm_start(HPM_PDM);
            }
        } else {
            ;
        }

        return true;
    } else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

        if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) {
            spk_volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

            TU_LOG1("Set spk channel %d volume: %d dB\r\n", request->bChannelNumber, spk_volume[request->bChannelNumber] / 256);
        } else if (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) {
            mic_volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

            TU_LOG1("Set mic channel %d volume: %d dB\r\n", request->bChannelNumber, mic_volume[request->bChannelNumber] / 256);
        } else {
            ;
        }

        return true;
    } else {
        TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

/*---------------------------------------------------------------------*/
/*          Application Callback API Implementations                   */
/*---------------------------------------------------------------------*/

/* Invoked when audio class specific get request received for an entity */
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if ((request->bEntityID == UAC2_ENTITY_SPK_CLOCK) || (request->bEntityID == UAC2_ENTITY_MIC_CLOCK)) {
        return tud_audio_clock_get_request(rhport, request);
    }

    if ((request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) || (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT)) {
        return tud_audio_feature_unit_get_request(rhport, request);
    } else {
        TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
    }
    return false;
}

/* Invoked when audio class specific set request received for an entity */
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if ((request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) || (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT)) {
        return tud_audio_feature_unit_set_request(rhport, request, buf);
    }

    if ((request->bEntityID == UAC2_ENTITY_SPK_CLOCK) || (request->bEntityID == UAC2_ENTITY_MIC_CLOCK)) {
        return tud_audio_clock_set_request(rhport, request, buf);
    }

    TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt == 0) {
        dao_stop(HPM_DAO);
        s_spk_rx_flag = false;
    } else if (ITF_NUM_AUDIO_STREAMING_MIC == itf && alt == 0) {
        pdm_stop(HPM_PDM);
        s_mic_tx_flag = false;
    }

    return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    TU_LOG2("Set interface %d alt %d\r\n", itf, alt);
    if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt != 0) {
        dao_start(HPM_DAO);
        s_spk_rx_flag = true;
        s_spk_buf_front = 0;
        s_spk_buf_rear = 0;
        s_spk_dma_transfer_req = true;
        s_spk_dma_transfer_done = false;
    } else if (ITF_NUM_AUDIO_STREAMING_MIC == itf && alt != 0) {
        pdm_start(HPM_PDM);
        s_mic_tx_flag = true;
        s_mic_buf_front = 0;
        s_mic_buf_rear = 0;
        s_mic_dma_transfer_done = false;
        i2s_pdm_dma_cfg(CFG_TUD_AUDIO_EP_SZ_IN, (uint32_t *)&mic_buf[s_mic_buf_rear][0]);
    }

    return true;
}

bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    if (s_spk_rx_flag) {
        tud_audio_read((uint8_t *)&spk_buf[s_spk_buf_rear][0], n_bytes_received);
        spk_buf_size[s_spk_buf_rear] = n_bytes_received;
        s_spk_buf_rear++;
        if (s_spk_buf_rear >= AUDIO_BUFFER_COUNT) {
            s_spk_buf_rear = 0;
        }
    }

    return true;
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)itf;
    (void)ep_in;
    (void)cur_alt_setting;

    if (s_mic_tx_flag) {
        if (!mic_in_buff_is_empty()) {
            tud_audio_write((uint8_t *)&mic_buf[s_mic_buf_front][0], CFG_TUD_AUDIO_EP_SZ_IN);
            s_mic_buf_front++;
            if (s_mic_buf_front >= AUDIO_BUFFER_COUNT) {
                s_mic_buf_front = 0;
            }
        }
    }

    return true;
}

/*---------------------------------------------------------------------*/
/*                  audio playback and DMA isr handling                */
/*---------------------------------------------------------------------*/
SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL);
    mic_status = dma_check_transfer_status(BOARD_APP_XDMA, MIC_DMA_CHANNEL);

    if (0 != (speaker_status & DMA_CHANNEL_STATUS_TC)) {
        s_spk_dma_transfer_done = true;
    } else if (0 != (mic_status & DMA_CHANNEL_STATUS_TC)) {
        s_mic_dma_transfer_done = true;
    } else {
        ;
    }
}

void i2s_speaker_dma_cfg(uint32_t size, volatile uint32_t *ptr)
{
    dma_channel_config_t ch_config = {0};

    dma_default_channel_config(BOARD_APP_XDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.dst_addr = (uint32_t)&DAO_I2S->TXD[DAO_I2S_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

    if (status_success != dma_setup_channel(BOARD_APP_XDMA, SPEAKER_DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
    }
}

void i2s_pdm_dma_cfg(uint32_t size, uint32_t *ptr)
{
    dma_channel_config_t ch_config = {0};

    dma_default_channel_config(BOARD_APP_XDMA, &ch_config);

    ch_config.src_addr = (uint32_t)(&PDM_I2S->RXD[PDM_I2S_DATA_LINE]) + 2u;
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = DMA_ALIGN_HALF_WORD(size);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

    if (status_success != dma_setup_channel(BOARD_APP_XDMA, MIC_DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
    }
}

void reinit_dao_i2s_cfg(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    (void)channel_num;
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_init(DAO_I2S, &i2s_config);

    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.channel_slot_mask = 0x3;
    if (status_success != i2s_config_tx(DAO_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer)) {
        printf("I2S config failed for DAO\n");
        while (1) {
            ;
        }
    }
    i2s_start(DAO_I2S);
}

/*---------------------------------------------------------------------*/
/*                          AUDIO Task                                 */
/*---------------------------------------------------------------------*/
void audio_task(void)
{
    if (s_spk_rx_flag) {
        if (!speaker_out_buff_is_empty()) {
            if (s_spk_dma_transfer_req) {
                s_spk_dma_transfer_req = false;
                i2s_speaker_dma_cfg(spk_buf_size[s_spk_buf_front], (uint32_t *)&spk_buf[s_spk_buf_front][0]);
                s_spk_buf_front++;
                if (s_spk_buf_front >= AUDIO_BUFFER_COUNT) {
                    s_spk_buf_front = 0;
                }
            } else if (s_spk_dma_transfer_done) {
                s_spk_dma_transfer_done = false;
                i2s_speaker_dma_cfg(spk_buf_size[s_spk_buf_front], (uint32_t *)&spk_buf[s_spk_buf_front][0]);
                s_spk_buf_front++;
                if (s_spk_buf_front >= AUDIO_BUFFER_COUNT) {
                    s_spk_buf_front = 0;
                }
            } else {
                ;    /* Do Nothing */
            }
        }
    }

    if (s_mic_tx_flag) {
        if (s_mic_dma_transfer_done) {
            s_mic_dma_transfer_done = false;
            s_mic_buf_rear++;
            if (s_mic_buf_rear >= AUDIO_BUFFER_COUNT) {
                s_mic_buf_rear = 0;
            }
            i2s_pdm_dma_cfg(CFG_TUD_AUDIO_EP_SZ_IN, (uint32_t *)&mic_buf[s_mic_buf_rear][0]);
        }
    }
}

bool speaker_out_buff_is_empty(void)
{
    bool empty = false;

    if (s_spk_buf_front == s_spk_buf_rear) {
        empty = true;
    }

    return empty;
}

bool mic_in_buff_is_empty(void)
{
    bool empty = false;

    if (s_mic_buf_front == s_mic_buf_rear) {
        empty = true;
    }

    return empty;
}
