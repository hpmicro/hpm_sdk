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
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_pllctl_drv.h"
#include "hpm_pdm_drv.h"

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
#define TARGET_I2S            DAO_I2S
#define TARGET_I2S_CLK_NAME   clock_dao
#define TARGET_I2S_DATA_LINE  0
#define TARGET_I2S_TX_DMAMUX_SRC HPM_DMA_SRC_I2S1_TX

#define DMA_MUX_CHANNEL_MIC     (1U)
#define DMA_MUX_CHANNEL_SPEAKER (2U)
#define DMA_MUX_SRC HPM_DMA_SRC_I2S0_RX
#define I2S_MCLK_FREQ_IN_HZ (24576000UL)

#define N_SAMPLE_RATES  TU_ARRAY_SIZE(sample_rates)

/* Variable Definition */
uint32_t current_sample_rate;

/* List of supported sample rates */
const uint32_t sample_rates[] = {16000};

/* Blink pattern
 * - 25 ms   : streaming data
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
  BLINK_STREAMING = 25,
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

struct audio_info {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t audio_depth;
};

/* Audio controls */
/* Current states */
int8_t mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];       /* +1 for master channel 0 */
int16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];    /* +1 for master channel 0 */

/* Buffer for speaker data */
ATTR_PLACE_AT_NONCACHEABLE_BSS int32_t spk_buf1[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 4];
ATTR_PLACE_AT_NONCACHEABLE_BSS int32_t spk_buf2[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 4];
ATTR_PLACE_AT_NONCACHEABLE_BSS uint16_t mic_buf[CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX/2];
int32_t *spk_buf[] = {spk_buf1, spk_buf2};

/* Resolution per format */
const uint8_t resolutions_per_format[CFG_TUD_AUDIO_FUNC_1_N_FORMATS] = {CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX,
                                                                        CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX};
/* Current resolution, update on format change */
uint8_t current_resolution;

/* buffer pointer */
uint32_t *strdata;

/* Buffer index */
int8_t idx;

/* Speaker data size received in the last frame */
int spk_data_size;

/* Recording flag */
volatile bool tx_flag;

/* DMA transfer flags */
ATTR_PLACE_AT_NONCACHEABLE_INIT volatile bool dma_speaker_transfer_done = true;
ATTR_PLACE_AT_NONCACHEABLE volatile bool dma_speaker_transfer_error;

ATTR_PLACE_AT_NONCACHEABLE_INIT volatile bool dma_mic_transfer_done = true;
ATTR_PLACE_AT_NONCACHEABLE volatile bool dma_mic_transfer_error;

void i2s_pdm_dma_cfg(uint32_t size, uint32_t *ptr);
void audio_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
void audio_task(void);

static hpm_stat_t get_audio_clock_div(uint16_t *div1, uint32_t sample_rate, uint8_t channel_length, uint8_t channel_num)
{
    uint32_t bclk_freq_in_hz;
    uint32_t audio_freq_in_hz;
    uint32_t prop;
    uint16_t i, j;

    bclk_freq_in_hz = sample_rate * channel_length * channel_num;
    audio_freq_in_hz = clock_get_frequency(BOARD_APP_AUDIO_CLK_SRC_NAME);
    prop = audio_freq_in_hz / bclk_freq_in_hz;

    for (i = 20; i <= 256; i++) {
        for (j = 512; j >= 10; j--) {
            if (((i * j) == prop) || ((i * j) == (prop + 1)) || ((i * j) == (prop - 1))) {
                *div1 = i;
                return status_success;
            }
        }
    }

    return status_fail;
}

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
    transfer.data_line = I2S_DATA_LINE_0;
    transfer.channel_slot_mask = 0x1;
    if (status_success != i2s_config_rx(PDM_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer)) {
        printf("I2S config failed for PDM\n");
        while (1) {

        }
    }

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);
    pdm_start(HPM_PDM);
    i2s_enable_rx_dma_request(PDM_I2S);
}

void dao_config(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;

    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_config.fifo_threshold = 2;
    i2s_init(DAO_I2S, &i2s_config);
    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    if (status_success != i2s_config_tx(DAO_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer)) {
        printf("I2S config failed for DAO\n");
        while (1) {

        }
    }

    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);

    dao_start(HPM_DAO);
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
    board_init_usb_pins();

    pdm_config();
    dao_config();

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
    TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

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

    TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ) {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

        current_sample_rate = ((audio_control_cur_4_t const *)buf)->bCur;

        TU_LOG1("Clock set current freq: %d\r\n", current_sample_rate);

        audio_playback(current_sample_rate, 32, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX);

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
    TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR) {
        audio_control_cur_1_t mute1 = { .bCur = mute[request->bChannelNumber] };
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
            audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(volume[request->bChannelNumber]) };

            TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
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

    TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE) {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

        mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

        TU_LOG1("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

        return true;
    } else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

        volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

        TU_LOG1("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

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

    if (request->bEntityID == UAC2_ENTITY_CLOCK) {
        return tud_audio_clock_get_request(rhport, request);
    }

    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) {
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

    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT) {
        return tud_audio_feature_unit_set_request(rhport, request, buf);
    }

    if (request->bEntityID == UAC2_ENTITY_CLOCK) {
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

    } else if (ITF_NUM_AUDIO_STREAMING_MIC == itf && alt == 0) {
        tx_flag = false;
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

    } else if (ITF_NUM_AUDIO_STREAMING_MIC == itf && alt != 0) {
        tx_flag = true;
    }

    /* Clear buffer when streaming format is changed */
    spk_data_size = 0;
    if (alt != 0) {
        current_resolution = resolutions_per_format[alt-1];
        printf("current resolution: %d\r\n", current_resolution);
    }

    return true;
}

bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    if (idx == 0) {
        spk_data_size = tud_audio_read(spk_buf1, n_bytes_received);
    } else {
        spk_data_size = tud_audio_read(spk_buf2, n_bytes_received);
    }

    return true;
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
  (void)rhport;
  (void)itf;
  (void)ep_in;
  (void)cur_alt_setting;

  if (tx_flag) {
      tud_audio_write((uint8_t *)mic_buf, CFG_TUD_AUDIO_EP_SZ_IN);
  }
  return true;
}

bool tud_audio_tx_done_post_load_cb(uint8_t rhport, uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
  (void) rhport;
  (void) n_bytes_copied;
  (void) itf;
  (void) ep_in;
  (void) cur_alt_setting;

    if (tx_flag) {
        while (dma_mic_transfer_done == false) {

        }
        dma_mic_transfer_done = false;
        i2s_pdm_dma_cfg(CFG_TUD_AUDIO_EP_SZ_IN, (uint32_t *)mic_buf);
    }

    return true;
}
/*---------------------------------------------------------------------*/
/*                  audio playback and DMA isr handling                */
/*---------------------------------------------------------------------*/
void isr_dma(void)
{
    volatile hpm_stat_t stat;

    stat = dma_get_irq_status(BOARD_APP_HDMA);
    dma_clear_irq_status(BOARD_APP_HDMA, stat);

    if ((4 << 16) == (stat & (4 << 16))) {
        dma_speaker_transfer_done = true;
    } else  if ((2 << 16) == (stat & (2 << 16))) {
        dma_mic_transfer_done = true;
    } else {
        dma_speaker_transfer_error = true;
        dma_mic_transfer_error = true;
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_dma)

void i2s_speaker_dma_cfg(uint32_t size, uint32_t *ptr)
{
    dma_channel_config_t ch_config = {0};

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, 1);

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.dst_addr = (uint32_t)&TARGET_I2S->TXD[TARGET_I2S_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = size;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = 2;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, DMA_MUX_CHANNEL_SPEAKER, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL_SPEAKER, TARGET_I2S_TX_DMAMUX_SRC, true);
}

void i2s_pdm_dma_cfg(uint32_t size, uint32_t *ptr)
{
    dma_channel_config_t ch_config = {0};

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, 1);

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);

    ch_config.src_addr = (uint32_t)((uint8_t *)(&PDM_I2S->RXD[I2S_DATA_LINE_0]) + sizeof(uint16_t));
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = size;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, DMA_MUX_CHANNEL_MIC, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL_MIC, DMA_MUX_SRC, true);
}

void audio_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    uint32_t i2s_mclk_hz;
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;
    uint16_t div1;
    hpm_stat_t rsl;

    i2s_get_default_config(TARGET_I2S, &i2s_config);
    i2s_config.fifo_threshold = 2;
    i2s_init(TARGET_I2S, &i2s_config);

    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;

    rsl = get_audio_clock_div(&div1, transfer.sample_rate, audio_depth, channel_num);
    if (rsl == status_fail) {
        printf("Clock selection error.\r\n");
        return;
        while (1) {

        }
    }
    sysctl_config_clock(HPM_SYSCTL, clock_node_aud0, BOARD_APP_AUDIO_CLK_SRC, div1);
    i2s_mclk_hz = clock_get_frequency(BOARD_APP_I2S_CLK_NAME);
    if (status_success != i2s_config_tx(TARGET_I2S, i2s_mclk_hz, &transfer)) {
        printf("I2S config failed for DAO\n");
        while (1) {

        }
    }

    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);

    dao_start(HPM_DAO);
    i2s_enable_tx_dma_request(TARGET_I2S);
}

/*---------------------------------------------------------------------*/
/*                          AUDIO Task                                 */
/*---------------------------------------------------------------------*/
void audio_task(void)
{
    if (spk_data_size) {
        if (idx == 0) {
            while (dma_speaker_transfer_done == false) {

            }
            strdata = (uint32_t *)&spk_buf1[0];
        } else {
            while (dma_speaker_transfer_done == false) {

            }
            strdata = (uint32_t *)&spk_buf2[0];
        }
        dma_speaker_transfer_done = false;
        i2s_speaker_dma_cfg(spk_data_size, strdata);

        if (idx == 0) {
            idx = 1;
        } else {
            idx = 0;
        }

        spk_data_size = 0;
    }
}
