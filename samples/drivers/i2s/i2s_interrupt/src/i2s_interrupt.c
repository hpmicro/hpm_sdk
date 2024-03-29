/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_i2s_drv.h"
#include "hpm_dmamux_drv.h"
#include "audio_data.h"

#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"

#ifdef CONFIG_CODEC_WM8960
    #include "hpm_wm8960.h"
#elif defined(CONFIG_CODEC_SGTL5000)
    #include "hpm_sgtl5000.h"
#else
    #error no specified Audio Codec!!!
#endif

#ifndef BOARD_CODEC_I2C_BASE
#define CODEC_I2C            BOARD_APP_I2C_BASE
#else
#define CODEC_I2C            BOARD_CODEC_I2C_BASE
#endif
#define CODEC_I2S            BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME   BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_DATA_LINE  BOARD_APP_I2S_DATA_LINE
#define CODEC_I2S_IRQ        BOARD_APP_I2S_IRQ

#define TEST_I2S_TX_FIFO_THRESHOLD (2U)

typedef struct {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t audio_depth;
    const uint8_t *data;
    uint32_t length;
} audio_data_t;

volatile uint32_t t_count;
volatile uint32_t audio_play_count;
volatile bool audio_play_finished;

void isr_i2s(void)
{
    int32_t data;
    volatile uint32_t stat = i2s_check_data_line_status(CODEC_I2S, CODEC_I2S_DATA_LINE);

    if ((stat & i2s_data_line_tx_fifo_avail) != 0) {
        for (uint8_t i = 0; i < I2S_SOC_MAX_TX_FIFO_DEPTH - TEST_I2S_TX_FIFO_THRESHOLD; i++) {
            /* move valid 16bit audio data to high postion in 32bit value */
            data = *((uint16_t *)&sound_array[t_count]) << (32 - audio_depth);
            i2s_send_data(CODEC_I2S, CODEC_I2S_DATA_LINE, data);
            t_count += audio_depth / 8;
            if (t_count >= sizeof(sound_array)) {
                audio_play_count++;
                t_count = 0;
            }
            /* */
            if (audio_play_count >= 10) {
                audio_play_finished = true;
                i2s_disable_irq(CODEC_I2S, i2s_tx_fifo_threshold_irq_mask);
            }
        }
    }
}
SDK_DECLARE_EXT_ISR_M(CODEC_I2S_IRQ, isr_i2s)


hpm_stat_t board_i2s_init(audio_data_t *audio_data, uint32_t mclk_freq)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    hpm_stat_t stat;

    assert(mclk_freq > 0);

    /* Config I2S interface to CODEC */
    i2s_get_default_config(CODEC_I2S, &i2s_config);
    i2s_config.tx_fifo_threshold = TEST_I2S_TX_FIFO_THRESHOLD; /* tx fifo data <= threshold will generate irq */
    i2s_config.rx_fifo_threshold = TEST_I2S_TX_FIFO_THRESHOLD; /* rx fifo data >= threshold will generate irq */
    i2s_config.enable_mclk_out = true;
    i2s_init(CODEC_I2S, &i2s_config);

    i2s_get_default_transfer_config(&transfer);
    transfer.sample_rate = audio_data->sample_rate;
    transfer.audio_depth = audio_data->audio_depth;
    /* 1 chanel - channel slot mask 0x1; 2 channel - channel solt mask 0x3 */
    transfer.channel_slot_mask = (1 << audio_data->channel_num) - 1;
    transfer.data_line = CODEC_I2S_DATA_LINE;
    transfer.master_mode = true;

    stat = i2s_config_tx(CODEC_I2S, mclk_freq, &transfer);
    if (stat != status_success) {
        return status_fail;
    }

    i2s_enable_irq(CODEC_I2S, i2s_tx_fifo_threshold_irq_mask);
    intc_m_enable_irq_with_priority(CODEC_I2S_IRQ, 1);

    return status_success;
}

hpm_stat_t board_codec_init(audio_data_t *audio_data, uint32_t mclk_freq)
{
    assert(mclk_freq > 0);

    uint32_t sample_rate = audio_data->sample_rate;
    uint8_t audio_depth = audio_data->audio_depth;

    board_init_i2c(CODEC_I2C);

#ifdef CONFIG_CODEC_WM8960
    wm8960_config_t wm8960_config = {
        .route       = wm8960_route_playback,
        .left_input  = wm8960_input_closed,
        .right_input = wm8960_input_closed,
        .play_source = wm8960_play_source_dac,
        .bus         = wm8960_bus_left_justified,
        .format = {.mclk_hz = mclk_freq, .sample_rate = sample_rate, .bit_width = audio_depth},
    };

    wm8960_control_t wm8960_control = {
        .ptr = CODEC_I2C,
        .slave_address = WM8960_I2C_ADDR, /* I2C address */
    };

    if (wm8960_init(&wm8960_control, &wm8960_config) != status_success) {
        printf("Init Audio Codec failed\n");
        return status_fail;
    }

#elif defined(CONFIG_CODEC_SGTL5000)
    sgtl_config_t sgtl5000_config = {
        .route = sgtl_route_playback,    /*!< Audio data route.*/
        .bus = sgtl_bus_left_justified,  /*!< Audio transfer protocol */
        .master = false,                 /*!< Master or slave. True means master, false means slave. */
        .format = {
            .mclk_hz = mclk_freq,
            .sample_rate = sample_rate,
            .bit_width = audio_depth,
            .sclk_edge = sgtl_sclk_valid_edge_rising
        },
    };

    sgtl_context_t sgtl5000_context = {
        .ptr = CODEC_I2C,
        .slave_address = SGTL5000_I2C_ADDR, /* I2C address */
    };

    if (sgtl_init(&sgtl5000_context, &sgtl5000_config) != status_success) {
        return status_fail;
    }
#endif

    return status_success;
}

int main(void)
{
    uint32_t i2s_mclk_freq;
    hpm_stat_t stat;

    board_init();
    printf("I2S Interrupt example\n");

    audio_data_t wave_data;
    wave_data.sample_rate = sample_rate;
    wave_data.audio_depth = audio_depth;
    wave_data.channel_num = channel_num;
    wave_data.data = sound_array;
    wave_data.length = sizeof(sound_array);
    init_i2s_pins(CODEC_I2S);
    i2s_mclk_freq = board_config_i2s_clock(CODEC_I2S, wave_data.sample_rate); /* according the samplerate to init I2S clock */

    /* config I2S peripehral */
    stat = board_i2s_init(&wave_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init I2S failed\n");
        return 1;
    }

    /* config codec on baord */
    stat = board_codec_init(&wave_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init Audio Codec failed\n");
        return 1;
    }

    i2s_enable(CODEC_I2S); /* start */
    while ((!audio_play_finished)) {
        __asm("nop");
    }
    i2s_disable(CODEC_I2S); /* stop */
    printf("I2S interrupt play finished\n");

    return 0;
}
