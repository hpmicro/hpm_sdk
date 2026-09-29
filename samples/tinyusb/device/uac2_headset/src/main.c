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

/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "board.h"
#include "hpm_i2s_drv.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_codec_common.h"

/*--------------------------------------------------------------------*/
/* MACRO CONSTANT TYPEDEF PROTOTYPES */
/*--------------------------------------------------------------------*/

/* List of supported sample rates */
const uint32_t sample_rates[] = { CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE };

static volatile uint32_t s_requested_sample_rate = CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE;
static volatile uint32_t s_active_sample_rate = CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE;

#define N_SAMPLE_RATES TU_ARRAY_SIZE(sample_rates)

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
#ifndef BOARD_CODEC_I2C_BASE
#define CODEC_I2C BOARD_APP_I2C_BASE
#else
#define CODEC_I2C BOARD_CODEC_I2C_BASE
#endif

#define CODEC_I2S              BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME     BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_TX_DATA_LINE BOARD_APP_I2S_TX_DATA_LINE
#define CODEC_I2S_RX_DATA_LINE BOARD_APP_I2S_RX_DATA_LINE

#define CODEC_I2S_TX_DMA_CHANNEL    1U
#define CODEC_I2S_RX_DMA_CHANNEL    2U
#define CODEC_I2S_TX_DMAMUX_CHANNEL DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, CODEC_I2S_TX_DMA_CHANNEL)
#define CODEC_I2S_RX_DMAMUX_CHANNEL DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_DMA1, CODEC_I2S_RX_DMA_CHANNEL)
#define CODEC_I2S_TX_DMA_SRC        BOARD_APP_I2S_TX_DMA_REQ
#define CODEC_I2S_RX_DMA_SRC        BOARD_APP_I2S_RX_DMA_REQ

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

/* Audio controls */
/* Current states */
uint8_t spk_mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];   /* +1 for master channel 0 */
int16_t spk_volume_db[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1]; /* +1 for master channel 0 */
uint8_t mic_mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1];   /* +1 for master channel 0 */
int16_t mic_volume_db[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1]; /* +1 for master channel 0 */

/* Ring Buffer Size */
#define AUDIO_BUFFER_COUNT 8
ATTR_PLACE_AT_NONCACHEABLE int32_t mic_buf[AUDIO_BUFFER_COUNT][CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ / 4];
ATTR_PLACE_AT_NONCACHEABLE int32_t spk_buf[AUDIO_BUFFER_COUNT][CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 4];
/* Speaker data size received in the last frame */
static volatile uint32_t spk_data_size[AUDIO_BUFFER_COUNT];
static volatile uint32_t mic_data_size;

static codec_control_t codec_control;

static volatile bool s_spk_rx_flag;
static volatile uint8_t s_spk_buf_front;
static volatile uint8_t s_spk_buf_rear;
static volatile bool s_spk_dma_transfer_req;
static volatile uint8_t s_spk_priming_count;
static volatile bool s_mic_tx_flag;
static volatile uint8_t s_mic_buf_front;
static volatile uint8_t s_mic_buf_rear;
static volatile bool s_mic_usb_transfer_req;
static volatile bool s_spk_codec_update_pending;
static volatile bool s_mic_codec_update_pending;

static void codec_config(uint32_t sample_rate, uint32_t audio_depth, uint32_t channel_length);
static void i2s_speaker_dma_cfg(volatile uint32_t *ptr, uint32_t size);
static void i2s_mic_dma_cfg(uint32_t *ptr, uint32_t size);
static bool speaker_out_buff_is_empty(void);
static bool mic_in_buff_is_empty(void);

void led_blinking_task(void);
void audio_control_task(void);

/*------------- MAIN -------------*/
int main(void)
{
    board_init();

    board_init_i2c(CODEC_I2C);

    init_i2s_pins(CODEC_I2S);
    board_config_i2s_clock(CODEC_I2S, s_active_sample_rate);

    if (BOARD_TUD_RHPORT == 0) {
        board_init_usb(HPM_USB0);
        intc_set_irq_priority(IRQn_USB0, 1);
#ifdef HPM_USB1
    } else if (BOARD_TUD_RHPORT == 1) {
        board_init_usb(HPM_USB1);
        intc_set_irq_priority(IRQn_USB1, 1);
#endif
    } else {
        printf("Don't support HPM_USB%d!\n", BOARD_TUD_RHPORT);
        while (1) {
            ;
        }
    }

    codec_config(s_active_sample_rate, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX,
                 CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX * 8U);

    intc_m_enable_irq_with_priority(BOARD_APP_DMA1_IRQ, 2);

    /* init device stack on configured roothub port */
    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO,
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    board_init_after_tusb();

    printf("Headset running\r\n");

    while (1) {
        tud_task(); /* TinyUSB device task */
        audio_control_task();
        led_blinking_task();
    }
}

static hpm_stat_t audio_codec_init(uint32_t i2s_mclk_hz, i2s_config_t *i2s_config, i2s_multiline_transfer_config_t *transfer)
{
    codec_control.ptr = CODEC_I2C;
    codec_control.slave_address = BOARD_AUDIO_CODEC_I2C_ADDR;

#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    wm8960_config_t wm8960_config;
    wm8960_get_default_config(&wm8960_config);

    wm8960_config.format.mclk_hz = i2s_mclk_hz;
    wm8960_config.format.sample_rate = transfer->sample_rate;
    wm8960_config.format.bit_width = transfer->audio_depth;
    wm8960_config.lrclk_polarity = (i2s_config->invert_fclk_out) ? wm8960_lrclk_polarity_high_for_left_channel : wm8960_lrclk_polarity_low_for_left_channel;
    return wm8960_init(&codec_control, &wm8960_config);

#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000

    sgtl_config_t sgtl5000_config;
    sgtl_get_default_config(&sgtl5000_config);

    sgtl5000_config.format.mclk_hz = i2s_mclk_hz;
    sgtl5000_config.format.sample_rate = transfer->sample_rate;
    sgtl5000_config.format.bit_width = transfer->audio_depth;
    sgtl5000_config.lrclk_polarity = (i2s_config->invert_fclk_out) ? sgtl_lrclk_polarity_high_for_left_channel : sgtl_lrclk_polarity_low_for_left_channel;
    return sgtl_init(&codec_control, &sgtl5000_config);

#elif defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389

    es8389_config_t es8389_config;
    es8389_get_default_config(&es8389_config);

    es8389_config.mclk_hz = i2s_mclk_hz;
    es8389_config.sample_rate = transfer->sample_rate;
    es8389_config.data_width = transfer->audio_depth;
    es8389_config.lrclk_polarity = (i2s_config->invert_fclk_out) ? es8389_lrclk_polarity_high_for_left_channel : es8389_lrclk_polarity_low_for_left_channel;
    return es8389_init(&codec_control, &es8389_config);

#else
#error no specified Audio Codec!!!
    return status_fail;
#endif
}

static void codec_config(uint32_t sample_rate, uint32_t audio_depth, uint32_t channel_length)
{
    i2s_config_t i2s_config;
    i2s_multiline_transfer_config_t transfer;
    uint32_t i2s_mclk_hz;

    /* Config I2S interface to CODEC */
    i2s_get_default_config(CODEC_I2S, &i2s_config);
    i2s_config.enable_mclk_out = true;
    i2s_init(CODEC_I2S, &i2s_config);

    i2s_get_default_multiline_transfer_config(&transfer);
    transfer.audio_depth = audio_depth;
    transfer.channel_length = channel_length;
    transfer.sample_rate = sample_rate;
    transfer.master_mode = true;
    transfer.rx_data_line_en[CODEC_I2S_RX_DATA_LINE] = true;
    transfer.tx_data_line_en[CODEC_I2S_TX_DATA_LINE] = true;
    transfer.rx_channel_slot_mask[CODEC_I2S_RX_DATA_LINE] = 0x03;
    transfer.tx_channel_slot_mask[CODEC_I2S_TX_DATA_LINE] = 0x03;
    i2s_mclk_hz = clock_get_frequency(CODEC_I2S_CLK_NAME);
    /* configure I2S RX and TX */
    if (status_success != i2s_config_multiline_transfer(CODEC_I2S, i2s_mclk_hz, &transfer)) {
        printf("I2S config failed for CODEC\n");
        while (1)
            ;
    }

    i2s_enable_tx_dma_request(CODEC_I2S);
    i2s_enable_rx_dma_request(CODEC_I2S);
    dmamux_config(BOARD_APP_DMAMUX, CODEC_I2S_TX_DMAMUX_CHANNEL, CODEC_I2S_TX_DMA_SRC, true);
    dmamux_config(BOARD_APP_DMAMUX, CODEC_I2S_RX_DMAMUX_CHANNEL, CODEC_I2S_RX_DMA_SRC, true);

    /* Initialize audio codec */
    if (audio_codec_init(i2s_mclk_hz, &i2s_config, &transfer) != status_success) {
        printf("Init Audio Codec failed\n");
        while (1) {
        }
    }
}

/* Apply speaker mute and volume to codec hardware (combined, reads from master channel 0, values in dB) */
static void speaker_apply_codec_mute_volume(void)
{
#if defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_mute(&codec_control, es8389_dac1, spk_mute[0]);
    es8389_mute(&codec_control, es8389_dac2, spk_mute[0]);
    if (!spk_mute[0]) {
        float volume_db;
        es8389_clamp_volume_db(es8389_dac1, (float)spk_volume_db[0], &volume_db);
        es8389_set_volume_db(&codec_control, es8389_dac1, volume_db);
        es8389_set_volume_db(&codec_control, es8389_dac2, volume_db);
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_set_mute(&codec_control, sgtl_module_dac, spk_mute[0]);
    if (!spk_mute[0]) {
        float volume_db;
        sgtl_clamp_volume_db(sgtl_module_dac, (float)spk_volume_db[0], &volume_db);
        sgtl_set_volume_db(&codec_control, sgtl_module_dac, volume_db);
    }
#elif defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    /* WM8960 has no hardware mute bit, use the minimum dB value as mute */
    if (spk_mute[0]) {
        wm8960_mute(&codec_control, wm8960_module_dac);
    } else {
        float volume_db;
        wm8960_clamp_volume_db(wm8960_module_dac, (float)spk_volume_db[0], &volume_db);
        wm8960_set_volume_db(&codec_control, wm8960_module_dac, volume_db);
    }
#endif
}

/* Apply microphone mute and volume to codec hardware (combined, reads from master channel 0, values in dB) */
static void mic_apply_codec_mute_volume(void)
{
#if defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
    es8389_mute(&codec_control, es8389_adc1, mic_mute[0]);
    es8389_mute(&codec_control, es8389_adc2, mic_mute[0]);
    if (!mic_mute[0]) {
        float volume_db;
        es8389_clamp_volume_db(es8389_adc1, (float)mic_volume_db[0], &volume_db);
        es8389_set_volume_db(&codec_control, es8389_adc1, volume_db);
        es8389_set_volume_db(&codec_control, es8389_adc2, volume_db);
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl_set_mute(&codec_control, sgtl_module_adc, mic_mute[0]);
    if (!mic_mute[0]) {
        float volume_db;
        sgtl_clamp_volume_db(sgtl_module_adc, (float)mic_volume_db[0], &volume_db);
        sgtl_set_volume_db(&codec_control, sgtl_module_adc, volume_db);
    }
#elif defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    /* WM8960 has no hardware mute bit, use the minimum dB value as mute */
    if (mic_mute[0]) {
        wm8960_mute(&codec_control, wm8960_module_adc);
    } else {
        float volume_db;
        wm8960_clamp_volume_db(wm8960_module_adc, (float)mic_volume_db[0], &volume_db);
        wm8960_set_volume_db(&codec_control, wm8960_module_adc, volume_db);
    }
#endif
}

/*--------------------------------------------------------------------*/
/* Device callbacks */
/*--------------------------------------------------------------------*/

/* Invoked when device is mounted */
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

/* Invoked when device is unmounted */
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

/* Invoked when usb bus is suspended */
/* remote_wakeup_en : if host allow us  to perform remote wakeup */
/* Within 7ms, device must draw an average of current less than 2.5 mA from bus */
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

/* Invoked when usb bus is resumed */
void tud_resume_cb(void)
{
    blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

/*--------------------------------------------------------------------*/
/* Audio Callback Functions */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* UAC1 Helper Functions */
/*--------------------------------------------------------------------*/

static bool audio10_set_req_ep(tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);

    switch (ctrlSel) {
    case AUDIO10_EP_CTRL_SAMPLING_FREQ:
        if (p_request->bRequest == AUDIO10_CS_REQ_SET_CUR) {
            /* Request uses 3 bytes */
            TU_VERIFY(p_request->wLength == 3);

            s_requested_sample_rate = tu_unaligned_read32(pBuff) & 0x00FFFFFF;

            TU_LOG1("EP set current freq: %" PRIu32 "\r\n", s_requested_sample_rate);

            return true;
        }
        break;

    /* Unknown/Unsupported control */
    default:
        TU_BREAKPOINT();
        return false;
    }

    return false;
}

static bool audio10_get_req_ep(uint8_t rhport, tusb_control_request_t const *p_request)
{
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);

    switch (ctrlSel) {
    case AUDIO10_EP_CTRL_SAMPLING_FREQ:
        if (p_request->bRequest == AUDIO10_CS_REQ_GET_CUR) {
            TU_LOG1("EP get current freq\r\n");

            uint8_t freq[3];
            freq[0] = (uint8_t)(s_requested_sample_rate & 0xFF);
            freq[1] = (uint8_t)((s_requested_sample_rate >> 8) & 0xFF);
            freq[2] = (uint8_t)((s_requested_sample_rate >> 16) & 0xFF);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, freq, sizeof(freq));
        }
        break;

    /* Unknown/Unsupported control */
    default:
        TU_BREAKPOINT();
        return false;
    }

    return false;
}

static bool audio10_set_req_entity(tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

    uint8_t *mute_arr = NULL;
    int16_t *vol_arr = NULL;

    if (entityID == UAC1_ENTITY_SPK_FEATURE_UNIT) {
        mute_arr = spk_mute;
        vol_arr = spk_volume_db;
    } else if (entityID == UAC1_ENTITY_MIC_FEATURE_UNIT) {
        mute_arr = mic_mute;
        vol_arr = mic_volume_db;
    } else {
        return false;
    }

    switch (ctrlSel) {
    case AUDIO10_FU_CTRL_MUTE:
        switch (p_request->bRequest) {
        case AUDIO10_CS_REQ_SET_CUR:
            /* Only 1st form is supported */
            TU_VERIFY(p_request->wLength == 1);

            mute_arr[channelNum] = pBuff[0];
            /* Propagate to master channel so deferred codec apply reads the correct value */
            if (channelNum != 0) {
                mute_arr[0] = pBuff[0];
            }

            TU_LOG1("    Set Mute: %d of channel: %u\r\n", mute_arr[channelNum], channelNum);

            if (entityID == UAC1_ENTITY_MIC_FEATURE_UNIT) {
                s_mic_codec_update_pending = true;
            } else {
                s_spk_codec_update_pending = true;
            }
            return true;

        default:
            return false; /* not supported */
        }

    case AUDIO10_FU_CTRL_VOLUME:
        switch (p_request->bRequest) {
        case AUDIO10_CS_REQ_SET_CUR:
            /* Only 1st form is supported */
            TU_VERIFY(p_request->wLength == 2);

            vol_arr[channelNum] = (int16_t)tu_unaligned_read16(pBuff) / 256;
            /* Propagate to master channel so deferred codec apply reads the correct value */
            if (channelNum != 0) {
                vol_arr[0] = vol_arr[channelNum];
            }

            TU_LOG1("    Set Volume: %d dB of channel: %u\r\n", vol_arr[channelNum], channelNum);

            if (entityID == UAC1_ENTITY_MIC_FEATURE_UNIT) {
                s_mic_codec_update_pending = true;
            } else {
                s_spk_codec_update_pending = true;
            }
            return true;

        default:
            return false; /* not supported */
        }

        /* Unknown/Unsupported control */
    default:
        TU_BREAKPOINT();
        return false;
    }
}

static bool audio10_get_req_entity(uint8_t rhport, tusb_control_request_t const *p_request)
{
    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

    uint8_t *mute_arr = NULL;
    int16_t *vol_arr = NULL;

    if (entityID == UAC1_ENTITY_SPK_FEATURE_UNIT) {
        mute_arr = spk_mute;
        vol_arr = spk_volume_db;
    } else if (entityID == UAC1_ENTITY_MIC_FEATURE_UNIT) {
        mute_arr = mic_mute;
        vol_arr = mic_volume_db;
    } else {
        return false;
    }

    switch (ctrlSel) {
    case AUDIO10_FU_CTRL_MUTE:
        /* Audio control mute cur parameter block consists of only one byte - we thus can send it right away */
        /* There does not exist a range parameter block for mute */
        TU_LOG1("    Get Mute of channel: %u\r\n", channelNum);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &mute_arr[channelNum], 1);

    case AUDIO10_FU_CTRL_VOLUME:
        switch (p_request->bRequest) {
        case AUDIO10_CS_REQ_GET_CUR:
            TU_LOG1("    Get Volume of channel: %u\r\n", channelNum);
            {
                int16_t vol = (int16_t)vol_arr[channelNum];
                vol = vol * 256; /* convert to 1/256 dB units */
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &vol, sizeof(vol));
            }

        case AUDIO10_CS_REQ_GET_MIN:
            TU_LOG1("    Get Volume min of channel: %u\r\n", channelNum);
            {
                int16_t min = -100 * 256; /* -100 dB in 1/256 dB units */
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &min, sizeof(min));
            }

        case AUDIO10_CS_REQ_GET_MAX:
            TU_LOG1("    Get Volume max of channel: %u\r\n", channelNum);
            {
                int16_t max = 0; /* 0 dB in 1/256 dB units */
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &max, sizeof(max));
            }

        case AUDIO10_CS_REQ_GET_RES:
            TU_LOG1("    Get Volume res of channel: %u\r\n", channelNum);
            {
                int16_t res = 1; /* 1 dB */
                res = res * 256; /* convert to 1/256 dB units */
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &res, sizeof(res));
            }
            /* Unknown/Unsupported control */
        default:
            TU_BREAKPOINT();
            return false;
        }

        /* Unknown/Unsupported control */
    default:
        TU_BREAKPOINT();
        return false;
    }
}

/*--------------------------------------------------------------------*/
/* UAC2 Helper Functions */
/*--------------------------------------------------------------------*/

#if TUD_OPT_HIGH_SPEED

/* Helper for clock get requests */
static bool audio20_clock_get_request(uint8_t rhport, audio20_control_request_t const *request)
{
    TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

    if (request->bControlSelector == AUDIO20_CS_CTRL_SAM_FREQ) {
        if (request->bRequest == AUDIO20_CS_REQ_CUR) {
            TU_LOG1("Clock get current freq %" PRIu32 "\r\n", s_requested_sample_rate);

            audio20_control_cur_4_t curf = { (int32_t)tu_htole32(s_requested_sample_rate) };
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
        } else if (request->bRequest == AUDIO20_CS_REQ_RANGE) {
            audio20_control_range_4_n_t(N_SAMPLE_RATES) rangef = { .wNumSubRanges = tu_htole16(N_SAMPLE_RATES) };
            TU_LOG1("Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
            for (uint8_t i = 0; i < N_SAMPLE_RATES; i++) {
                rangef.subrange[i].bMin = (int32_t)sample_rates[i];
                rangef.subrange[i].bMax = (int32_t)sample_rates[i];
                rangef.subrange[i].bRes = 0;
                TU_LOG1("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
            }

            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
        }
    } else if (request->bControlSelector == AUDIO20_CS_CTRL_CLK_VALID && request->bRequest == AUDIO20_CS_REQ_CUR) {
        audio20_control_cur_1_t cur_valid = { .bCur = 1 };
        TU_LOG1("Clock get is valid %u\r\n", cur_valid.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
    }
    TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

/* Helper for clock set requests */
static bool audio20_clock_set_request(uint8_t rhport, audio20_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);
    TU_VERIFY(request->bRequest == AUDIO20_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO20_CS_CTRL_SAM_FREQ) {
        TU_VERIFY(request->wLength == sizeof(audio20_control_cur_4_t));

        s_requested_sample_rate = (uint32_t)((audio20_control_cur_4_t const *)buf)->bCur;

        TU_LOG1("Clock set current freq: %" PRIu32 "\r\n", s_requested_sample_rate);

        return true;
    } else {
        TU_LOG1("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector,
                request->bRequest);
        return false;
    }
}

/* Helper for feature unit get requests */
static bool audio20_feature_unit_get_request(uint8_t rhport, audio20_control_request_t const *request)
{
    TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT || request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT);

    uint8_t *mute_arr = (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) ? mic_mute : spk_mute;
    int16_t *vol_arr = (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) ? mic_volume_db : spk_volume_db;

    if (request->bControlSelector == AUDIO20_FU_CTRL_MUTE && request->bRequest == AUDIO20_CS_REQ_CUR) {
        audio20_control_cur_1_t mute1 = { .bCur = mute_arr[request->bChannelNumber] };
        TU_LOG1("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
    } else if (request->bControlSelector == AUDIO20_FU_CTRL_VOLUME) {
        if (request->bRequest == AUDIO20_CS_REQ_RANGE) {
            audio20_control_range_2_n_t(1)
                range_vol = { .wNumSubRanges = tu_htole16(1),
                              .subrange[0] = { .bMin = tu_htole16(-VOLUME_CTRL_100_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256) } };
            TU_LOG1("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber, range_vol.subrange[0].bMin / 256,
                    range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
        } else if (request->bRequest == AUDIO20_CS_REQ_CUR) {
            /* vol_arr stores dB, convert to 1/256 dB for USB response */
            int16_t vol_256db = (int16_t)vol_arr[request->bChannelNumber] * 256;
            audio20_control_cur_2_t cur_vol = { .bCur = tu_htole16(vol_256db) };
            TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, vol_256db / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
        }
    }
    TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector,
            request->bRequest);

    return false;
}

/* Helper for feature unit set requests */
static bool audio20_feature_unit_set_request(uint8_t rhport, audio20_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT || request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT);
    TU_VERIFY(request->bRequest == AUDIO20_CS_REQ_CUR);

    uint8_t *mute_arr = (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) ? mic_mute : spk_mute;
    int16_t *vol_arr = (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) ? mic_volume_db : spk_volume_db;

    if (request->bControlSelector == AUDIO20_FU_CTRL_MUTE) {
        TU_VERIFY(request->wLength == sizeof(audio20_control_cur_1_t));

        mute_arr[request->bChannelNumber] = ((audio20_control_cur_1_t const *)buf)->bCur;
        /* Propagate to master channel so deferred codec apply reads the correct value */
        if (request->bChannelNumber != 0) {
            mute_arr[0] = mute_arr[request->bChannelNumber];
        }

        TU_LOG1("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute_arr[request->bChannelNumber]);

        if (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) {
            s_mic_codec_update_pending = true;
        } else {
            s_spk_codec_update_pending = true;
        }
        return true;
    } else if (request->bControlSelector == AUDIO20_FU_CTRL_VOLUME) {
        TU_VERIFY(request->wLength == sizeof(audio20_control_cur_2_t));

        /* Store in dB (bCur is in 1/256 dB units) */
        vol_arr[request->bChannelNumber] = ((audio20_control_cur_2_t const *)buf)->bCur / 256;
        /* Propagate to master channel so deferred codec apply reads the correct value */
        if (request->bChannelNumber != 0) {
            vol_arr[0] = vol_arr[request->bChannelNumber];
        }

        TU_LOG1("Set channel %d volume: %d dB\r\n", request->bChannelNumber, vol_arr[request->bChannelNumber]);

        if (request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT) {
            s_mic_codec_update_pending = true;
        } else {
            s_spk_codec_update_pending = true;
        }
        return true;
    } else {
        TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector,
                request->bRequest);
        return false;
    }
}

static bool audio20_get_req_entity(uint8_t rhport, tusb_control_request_t const *p_request)
{
    audio20_control_request_t const *request = (audio20_control_request_t const *)p_request;

    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return audio20_clock_get_request(rhport, request);
    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT || request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT)
        return audio20_feature_unit_get_request(rhport, request);
    else {
        TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n", request->bEntityID, request->bControlSelector, request->bRequest);
    }
    return false;
}

static bool audio20_set_req_entity(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
    audio20_control_request_t const *request = (audio20_control_request_t const *)p_request;

    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT || request->bEntityID == UAC2_ENTITY_MIC_FEATURE_UNIT)
        return audio20_feature_unit_set_request(rhport, request, buf);
    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return audio20_clock_set_request(rhport, request, buf);
    TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n", request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

#endif /* TUD_OPT_HIGH_SPEED */

/* Invoked when audio class specific set request received for an EP */
bool tud_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    (void)rhport;
    (void)pBuff;

    if (tud_audio_version() == 1) {
        return audio10_set_req_ep(p_request, pBuff);
    } else if (tud_audio_version() == 2) {
        /* We do not support any requests here */
    }

    return false; /* Yet not implemented */
}

/* Invoked when audio class specific get request received for an EP */
bool tud_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    if (tud_audio_version() == 1) {
        return audio10_get_req_ep(rhport, p_request);
    } else if (tud_audio_version() == 2) {
        /* We do not support any requests here */
    }

    return false; /* Yet not implemented */
}

/* Invoked when audio class specific get request received for an entity */
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    if (tud_audio_version() == 1) {
        return audio10_get_req_entity(rhport, p_request);
#if TUD_OPT_HIGH_SPEED
    } else if (tud_audio_version() == 2) {
        return audio20_get_req_entity(rhport, p_request);
#endif
    }

    return false;
}

/* Invoked when audio class specific set request received for an entity */
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
    (void)rhport;

    if (tud_audio_version() == 1) {
        return audio10_set_req_entity(p_request, buf);
#if TUD_OPT_HIGH_SPEED
    } else if (tud_audio_version() == 2) {
        return audio20_set_req_entity(rhport, p_request, buf);
#endif
    }

    return false;
}

bool tud_audio_set_itf_close_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt == 0) {
        blink_interval_ms = BLINK_MOUNTED;
    }

    return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));
    bool const first_stream = !s_spk_rx_flag && !s_mic_tx_flag;

    TU_LOG1("Set interface %d alt %d\r\n", itf, alt);

    if (alt != 0 && (ITF_NUM_AUDIO_STREAMING_SPK == itf || ITF_NUM_AUDIO_STREAMING_MIC == itf)) {
        if (first_stream) {
            i2s_reset_tx_rx(CODEC_I2S);

            if (s_active_sample_rate != s_requested_sample_rate) {
                codec_config(s_requested_sample_rate, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX,
                             CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX * 8U);
                s_active_sample_rate = s_requested_sample_rate;
            }

            /* Multiline configuration enables both directions; DMA setup enables the active one. */
            i2s_disable_tx(CODEC_I2S, 1U << CODEC_I2S_TX_DATA_LINE);
            i2s_disable_rx(CODEC_I2S, 1U << CODEC_I2S_RX_DATA_LINE);
        } else if (s_active_sample_rate != s_requested_sample_rate) {
            TU_LOG1("Cannot open another stream while a shared I2S rate change is pending\r\n");
            return false;
        }
    }

    if (ITF_NUM_AUDIO_STREAMING_SPK == itf) {
        if (alt != 0) {
            blink_interval_ms = BLINK_STREAMING;
            if (!first_stream) {
                i2s_reset_tx(CODEC_I2S);
            }
            s_spk_rx_flag = true;
            s_spk_buf_front = 0;
            s_spk_buf_rear = 0;
            s_spk_dma_transfer_req = true;
            s_spk_priming_count = 0;
        } else {
            s_spk_rx_flag = false;
            dma_abort_channel(BOARD_APP_DMA1, 1u << CODEC_I2S_TX_DMA_CHANNEL);
            i2s_reset_tx(CODEC_I2S);
        }
    } else if (ITF_NUM_AUDIO_STREAMING_MIC == itf) {
        if (alt != 0) {
            /* Compute the exact mic packet size from the active rate and endpoint interval. */
            uint32_t packets_per_sec = (tud_speed_get() == TUSB_SPEED_HIGH) ?
                                           CFG_TUD_AUDIO_HS_PACKETS_PER_SEC :
                                           CFG_TUD_AUDIO_FS_PACKETS_PER_SEC;
            if (!first_stream) {
                i2s_reset_rx(CODEC_I2S);
            }
            mic_data_size = (s_active_sample_rate * CFG_TUD_AUDIO_FUNC_1_FRAME_SIZE_TX) / packets_per_sec;

            s_mic_tx_flag = true;
            s_mic_buf_front = 0;
            s_mic_buf_rear = 0;
            s_mic_usb_transfer_req = true;
            i2s_mic_dma_cfg((uint32_t *)&mic_buf[0][0], mic_data_size);
        } else {
            s_mic_tx_flag = false;
            dma_abort_channel(BOARD_APP_DMA1, 1u << CODEC_I2S_RX_DMA_CHANNEL);
            i2s_reset_rx(CODEC_I2S);
        }
    }

    if (first_stream && (s_spk_rx_flag || s_mic_tx_flag)) {
        i2s_start(CODEC_I2S);
    } else if (!s_spk_rx_flag && !s_mic_tx_flag) {
        i2s_stop(CODEC_I2S);
    }

    return true;
}

bool tud_audio_rx_done_isr(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    if (s_spk_rx_flag) {
        uint8_t rear = s_spk_buf_rear;
        tud_audio_read((uint8_t *)&spk_buf[rear][0], n_bytes_received);
        spk_data_size[rear] = n_bytes_received;
        rear++;
        if (rear >= AUDIO_BUFFER_COUNT) {
            rear = 0;
        }
        s_spk_buf_rear = rear;

        /* If DMA is waiting, accumulate buffers and kick off at half-full */
        if (s_spk_dma_transfer_req) {
            if (s_spk_priming_count >= (AUDIO_BUFFER_COUNT / 2) - 1) {
                uint8_t front = s_spk_buf_front;
                s_spk_dma_transfer_req = false;
                i2s_speaker_dma_cfg((uint32_t *)&spk_buf[front][0], spk_data_size[front]);
                front++;
                if (front >= AUDIO_BUFFER_COUNT) {
                    front = 0;
                }
                s_spk_buf_front = front;
            } else {
                s_spk_priming_count++;
            }
        }
    }

    return true;
}

bool tud_audio_tx_done_isr(uint8_t rhport, uint16_t n_bytes_sent, uint8_t func_id, uint8_t ep_in, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)n_bytes_sent;
    (void)func_id;
    (void)ep_in;
    (void)cur_alt_setting;

    if (s_mic_tx_flag) {
        if (!mic_in_buff_is_empty()) {
            uint8_t front = s_mic_buf_front;
            tud_audio_write((uint8_t *)&mic_buf[front][0], mic_data_size);
            front++;
            if (front >= AUDIO_BUFFER_COUNT) {
                front = 0;
            }
            s_mic_buf_front = front;
        } else {
            /* FIFO is empty, defer USB IN kick-off to DMA ISR */
            s_mic_usb_transfer_req = true;
        }
    }

    return true;
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_DMA1_IRQ, isr_dma)
void isr_dma(void)
{
    volatile uint32_t speaker_status;
    volatile uint32_t mic_status;

    speaker_status = dma_check_transfer_status(BOARD_APP_DMA1, CODEC_I2S_TX_DMA_CHANNEL);
    mic_status = dma_check_transfer_status(BOARD_APP_DMA1, CODEC_I2S_RX_DMA_CHANNEL);

    if (s_spk_rx_flag && (0 != (speaker_status & DMA_CHANNEL_STATUS_TC))) {
        if (!speaker_out_buff_is_empty()) {
            uint8_t front = s_spk_buf_front;
            i2s_speaker_dma_cfg((uint32_t *)&spk_buf[front][0], spk_data_size[front]);
            front++;
            if (front >= AUDIO_BUFFER_COUNT) {
                front = 0;
            }
            s_spk_buf_front = front;
        } else {
            /* FIFO is empty, defer DMA kick-off to tud_audio_rx_done_isr */
            s_spk_dma_transfer_req = true;
        }
    }

    if (s_mic_tx_flag && (0 != (mic_status & DMA_CHANNEL_STATUS_TC))) {
        uint8_t rear = s_mic_buf_rear;
        rear++;
        if (rear >= AUDIO_BUFFER_COUNT) {
            rear = 0;
        }
        s_mic_buf_rear = rear;
        i2s_mic_dma_cfg((uint32_t *)&mic_buf[rear][0], mic_data_size);

        /* If USB IN is waiting for data, kick it off now */
        if (s_mic_usb_transfer_req) {
            uint8_t front = s_mic_buf_front;
            s_mic_usb_transfer_req = false;
            tud_audio_write((uint8_t *)&mic_buf[front][0], mic_data_size);
            front++;
            if (front >= AUDIO_BUFFER_COUNT) {
                front = 0;
            }
            s_mic_buf_front = front;
        }
    }
}

static void i2s_speaker_dma_cfg(volatile uint32_t *ptr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.dst_addr = (uint32_t)&CODEC_I2S->TXD[CODEC_I2S_TX_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    if (status_success != dma_setup_channel(BOARD_APP_DMA1, CODEC_I2S_TX_DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
    } else {
        i2s_enable_tx(CODEC_I2S, 1U << CODEC_I2S_TX_DATA_LINE);
    }
}

static void i2s_mic_dma_cfg(uint32_t *ptr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_DMA1, &ch_config);

    ch_config.src_addr = (uint32_t)(&CODEC_I2S->RXD[CODEC_I2S_RX_DATA_LINE]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)ptr);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = DMA_ALIGN_WORD(size);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T; /* 2 transfers per burst: each burst fetches one stereo audio frame (2 channels) */

    if (status_success != dma_setup_channel(BOARD_APP_DMA1, CODEC_I2S_RX_DMA_CHANNEL, &ch_config, true)) {
        printf(" dma setup channel failed\n");
    } else {
        i2s_enable_rx(CODEC_I2S, 1U << CODEC_I2S_RX_DATA_LINE);
    }
}

static bool speaker_out_buff_is_empty(void)
{
    return (s_spk_buf_front == s_spk_buf_rear) ? true : false;
}

static bool mic_in_buff_is_empty(void)
{
    return (s_mic_buf_front == s_mic_buf_rear) ? true : false;
}

void audio_control_task(void)
{
    /* Poll every 50ms */
    const uint32_t interval_ms = 50;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms)
        return; /* not enough time */
    start_ms += interval_ms;

    /* Apply deferred codec state updates from USB host (I2C must not run in USB callback context) */
    if (s_spk_codec_update_pending) {
        s_spk_codec_update_pending = false;
        speaker_apply_codec_mute_volume();
    }
    if (s_mic_codec_update_pending) {
        s_mic_codec_update_pending = false;
        mic_apply_codec_mute_volume();
    }

    /* Press on-board button to control volume */
    /* Open host volume control, volume should switch between 10% and 100% */
    static uint32_t btn_prev = 0;
    uint32_t btn = board_button_read();

    /* Even UAC1 spec have status interrupt support like UAC2, most host do not support it */
    /* So you have to either use UAC2 or use old day HID volume control */
    TU_VERIFY((tud_audio_version() == 1),);

    if (!btn_prev && btn) {
        /* Adjust volume between 0dB (100%) and -30dB (10%) */
        for (int i = 0; i < CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1; i++) {
            spk_volume_db[i] = (spk_volume_db[i] == 0) ? -30 : 0;
        }

        /* Apply volume change to codec hardware (already in task context, no need to defer) */
        speaker_apply_codec_mute_volume();

        /* 6.1 Interrupt Data Message */
        const audio_interrupt_data_t data = { .v2 = {
                                                  .bInfo = 0,                                       /* Class-specific interrupt, originated from an interface */
                                                  .bAttribute = AUDIO20_CS_REQ_CUR,                 /* Caused by current settings */
                                                  .wValue_cn_or_mcn = 0,                            /* CH0: master volume */
                                                  .wValue_cs = AUDIO20_FU_CTRL_VOLUME,              /* Volume change */
                                                  .wIndex_ep_or_int = 0,                            /* From the interface itself */
                                                  .wIndex_entity_id = UAC2_ENTITY_SPK_FEATURE_UNIT, /* From feature unit */
                                              } };

        tud_audio_int_write(&data);
    }

    btn_prev = btn;
}

/*--------------------------------------------------------------------*/
/* BLINKING TASK */
/*--------------------------------------------------------------------*/
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    /* Blink every interval ms */
    if (board_millis() - start_ms < blink_interval_ms) {
        return;
    }
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state;
}
