/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_i2s_drv.h"

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

#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    #include "hpm_wm8960.h"
    wm8960_config_t wm8960_config = {
        .route       = wm8960_route_playback_and_record,
        .left_input  = wm8960_input_closed,
        .right_input = wm8960_input_differential_mic_input2,
        .play_source = wm8960_play_source_dac,
        .bus         = wm8960_bus_left_justified,
        .format = {.mclk_hz = 0U, .sample_rate = CODEC_SAMPLE_RATE_HZ, .bit_width = CODEC_BIT_WIDTH},
    };

    wm8960_control_t wm8960_control = {
        .ptr = CODEC_I2C,
        .slave_address = WM8960_I2C_ADDR, /* I2C address */
    };
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    #include "hpm_sgtl5000.h"
    sgtl_config_t sgtl5000_config = {
        .route = sgtl_route_playback_record,  /*!< Audio data route.*/
        .bus = sgtl_bus_left_justified,       /*!< Audio transfer protocol */
        .master = false,                      /*!< Master or slave. True means master, false means slave. */
        .format = {.mclk_hz = 0,
                .sample_rate = CODEC_SAMPLE_RATE_HZ,
                .bit_width = CODEC_BIT_WIDTH,
                .sclk_edge = sgtl_sclk_valid_edge_rising}, /*!< audio format */
    };

    sgtl_context_t sgtl5000_context = {
        .ptr = CODEC_I2C,
        .slave_address = SGTL5000_I2C_ADDR, /* I2C address */
    };
#else
    #error no specified Audio Codec!!!
#endif

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

#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
    wm8960_config.format.mclk_hz = i2s_mclk_hz;
    if (wm8960_init(&wm8960_control, &wm8960_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }
#elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
    sgtl5000_config.format.mclk_hz = i2s_mclk_hz;
    if (sgtl_init(&sgtl5000_context, &sgtl5000_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }
#endif

    printf("Test Codec playback and record\n");
    i2s_start(CODEC_I2S);
    while(1) {
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

    while(1);
    return 0;
}
