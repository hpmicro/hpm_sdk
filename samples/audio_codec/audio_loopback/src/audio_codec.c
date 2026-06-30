/*
 * Copyright (c) 2021,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_i2s_drv.h"
#include "hpm_codec_common.h"

#ifndef BOARD_CODEC_I2C_BASE
#define CODEC_I2C            BOARD_APP_I2C_BASE
#else
#define CODEC_I2C            BOARD_CODEC_I2C_BASE
#endif
#define CODEC_I2S            BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME   BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_TX_DATA_LINE  BOARD_APP_I2S_TX_DATA_LINE
#define CODEC_I2S_RX_DATA_LINE  BOARD_APP_I2S_RX_DATA_LINE

#define CODEC_SAMPLE_RATE_HZ 48000U
#define CODEC_BIT_WIDTH      32U

/**
 * @brief Initialize audio codec with default configuration
 *
 * This function initializes the audio codec using default configuration.
 * The codec type is determined by compile-time configuration (CONFIG_CODEC_*).
 *
 * @param i2s_mclk_hz I2S master clock frequency in Hz
 * @param i2s_config Pointer to I2S configuration structure
 * @return Initialization status
 */
hpm_stat_t audio_codec_init(uint32_t i2s_mclk_hz, i2s_config_t *i2s_config, i2s_multiline_transfer_config_t *transfer)
{
    codec_control_t codec_control = {
        .ptr = CODEC_I2C,
        .slave_address = BOARD_AUDIO_CODEC_I2C_ADDR,
    };

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

void test_codec_playback_record(void)
{
    uint32_t data;
    i2s_config_t i2s_config;
    i2s_multiline_transfer_config_t transfer;
    uint32_t i2s_mclk_hz;

    /* Config I2S interface to CODEC */ 
    i2s_get_default_config(CODEC_I2S, &i2s_config);
    i2s_config.enable_mclk_out = true;
    i2s_init(CODEC_I2S, &i2s_config);

    i2s_get_default_multiline_transfer_config(&transfer);
    transfer.sample_rate = CODEC_SAMPLE_RATE_HZ;
    transfer.master_mode = true;
    transfer.rx_data_line_en[CODEC_I2S_RX_DATA_LINE] = true;
    transfer.tx_data_line_en[CODEC_I2S_TX_DATA_LINE] = true;
    transfer.rx_channel_slot_mask[CODEC_I2S_RX_DATA_LINE] = 0x03;
    transfer.tx_channel_slot_mask[CODEC_I2S_TX_DATA_LINE] = 0x03;
    i2s_mclk_hz = clock_get_frequency(CODEC_I2S_CLK_NAME);
    /* configure I2S RX and TX */
    if (status_success != i2s_config_multiline_transfer(CODEC_I2S, i2s_mclk_hz, &transfer))
    {
        printf("I2S config failed for CODEC\n");
        while(1);
    }

    /* Initialize audio codec */
    if (audio_codec_init(i2s_mclk_hz, &i2s_config, &transfer) != status_success) {
        printf("Init Audio Codec failed\n");
        while (1) {
        }
    }

    printf("Test Codec playback and record\n");
    i2s_start(CODEC_I2S);
    while (1) {
        /* record from codec and play by codec */
        while (i2s_get_rx_line_fifo_level(CODEC_I2S, CODEC_I2S_RX_DATA_LINE) == 0) {
        }
        i2s_receive_data(CODEC_I2S, CODEC_I2S_RX_DATA_LINE, &data);
        i2s_send_data(CODEC_I2S, CODEC_I2S_TX_DATA_LINE, data);
    }
}

int main(void)
{
    board_init();
    board_init_i2c(CODEC_I2C);

    init_i2s_pins(CODEC_I2S);
    board_config_i2s_clock(CODEC_I2S, CODEC_SAMPLE_RATE_HZ);

    printf("audio codec example\n");
    test_codec_playback_record();

    while (1) {
    }
    return 0;
}
