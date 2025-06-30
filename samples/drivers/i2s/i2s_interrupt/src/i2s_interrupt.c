/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This example demonstrates I2S audio playback using interrupt transfer.
 * It shows how to:
 * 1. Configure I2S interface for audio codec
 * 2. Set up I2S FIFO threshold interrupts
 * 3. Implement audio playback with interrupt-driven data transfer
 */

#include <stdio.h>
#include "board.h"
#include "hpm_i2s_drv.h"
#include "audio_data.h"

#ifdef CONFIG_CODEC_WM8960
    #include "hpm_wm8960.h"
#elif defined(CONFIG_CODEC_SGTL5000)
    #include "hpm_sgtl5000.h"
#else
    #error no specified Audio Codec!!!
#endif

/* I2C and I2S peripheral definitions */
#ifndef BOARD_CODEC_I2C_BASE
#define CODEC_I2C            BOARD_APP_I2C_BASE
#else
#define CODEC_I2C            BOARD_CODEC_I2C_BASE
#endif
#define CODEC_I2S            BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME   BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_TX_DATA_LINE  BOARD_APP_I2S_TX_DATA_LINE
#define CODEC_I2S_IRQ        BOARD_APP_I2S_IRQ

/* FIFO threshold for interrupt triggering */
#define TEST_I2S_TX_FIFO_THRESHOLD (2U)

/* Audio data configuration structure */
typedef struct {
    uint32_t sample_rate;    /* Sampling frequency in Hz */
    uint8_t channel_num;     /* Number of audio channels (1: mono, 2: stereo) */
    uint8_t audio_depth;     /* Bit depth of audio samples (16/24/32 bits) */
    const uint8_t *data;     /* Pointer to audio data buffer */
    uint32_t length;         /* Length of audio data in bytes */
} audio_data_t;

audio_data_t wave_data;      /* Global audio data instance */

/* Audio playback control variables */
volatile uint32_t t_count;              /* Current position in audio data buffer */
volatile uint32_t audio_play_count;     /* Number of complete playback cycles */
volatile bool audio_play_finished;      /* Flag indicating playback completion */

/*
 * I2S interrupt handler
 * Handles FIFO threshold interrupts and manages audio data transfer
 */
SDK_DECLARE_EXT_ISR_M(CODEC_I2S_IRQ, isr_i2s)
void isr_i2s(void)
{
    int32_t data;
    volatile uint32_t stat = i2s_check_data_line_status(CODEC_I2S, CODEC_I2S_TX_DATA_LINE);

    if ((stat & i2s_data_line_tx_fifo_avail) != 0) {
        /* Fill TX FIFO when space is available */
        for (uint8_t i = 0; i < I2S_SOC_MAX_TX_FIFO_DEPTH - TEST_I2S_TX_FIFO_THRESHOLD; i++) {
            /* Read data according to audio depth and align to MSB */
            data = (*((uint32_t *)&wave_data.data[t_count])) << (32 - wave_data.audio_depth);
            i2s_send_data(CODEC_I2S, CODEC_I2S_TX_DATA_LINE, data);
            t_count += wave_data.audio_depth / 8;  /* Increment by number of bytes per sample */

            /* Handle buffer wrap-around and playback count */
            if (t_count >= wave_data.length) {
                audio_play_count++;
                t_count = 0;
            }

            /* Stop after 10 playback cycles */
            if (audio_play_count >= 10) {
                audio_play_finished = true;
                i2s_disable_irq(CODEC_I2S, i2s_tx_fifo_threshold_irq_mask);
            }
        }
    }
}

/*
 * Initialize I2S interface
 * Parameters:
 * - audio_data: Audio configuration parameters
 * - mclk_freq: Master clock frequency for I2S
 * Returns:
 * - status_success: Initialization successful
 * - status_fail: Initialization failed
 */
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

    /* Configure I2S transfer parameters */
    i2s_get_default_transfer_config(&transfer);
    transfer.sample_rate = audio_data->sample_rate;
    transfer.audio_depth = audio_data->audio_depth;
    /* 1 channel - channel slot mask 0x1; 2 channel - channel slot mask 0x3 */
    transfer.channel_slot_mask = (1 << audio_data->channel_num) - 1;
    transfer.data_line = CODEC_I2S_TX_DATA_LINE;
    transfer.master_mode = true;

    /* Apply I2S tx configuration */
    stat = i2s_config_tx(CODEC_I2S, mclk_freq, &transfer);
    if (stat != status_success) {
        return status_fail;
    }

    /* Enable I2S interrupts */
    i2s_enable_irq(CODEC_I2S, i2s_tx_fifo_threshold_irq_mask);
    intc_m_enable_irq_with_priority(CODEC_I2S_IRQ, 1);

    return status_success;
}

/*
 * Initialize audio codec
 * Parameters:
 * - audio_data: Audio configuration parameters
 * - mclk_freq: Master clock frequency for codec
 * Returns:
 * - status_success: Initialization successful
 * - status_fail: Initialization failed
 */
hpm_stat_t board_codec_init(audio_data_t *audio_data, uint32_t mclk_freq)
{
    assert(mclk_freq > 0);

    uint32_t sample_rate = audio_data->sample_rate;
    uint8_t audio_depth = audio_data->audio_depth;

    /* Initialize I2C interface for codec control */
    board_init_i2c(CODEC_I2C);

#ifdef CONFIG_CODEC_WM8960
    /* Configure WM8960 codec */
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
    /* Configure SGTL5000 codec */
    sgtl_config_t sgtl5000_config = {
        .route = sgtl_route_playback,    /* Audio data route */
        .bus = sgtl_bus_left_justified,  /* Audio transfer protocol */
        .master = false,                 /* Master or slave mode selection */
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

/*
 * Main function
 * Initializes system and performs I2S interrupt-driven playback test
 */
int main(void)
{
    uint32_t i2s_mclk_freq;
    hpm_stat_t stat;

    /* Initialize board basic peripherals */
    board_init();
    printf("I2S Interrupt example\n");

    /* Configure audio parameters */
    wave_data.sample_rate = sample_rate;
    wave_data.audio_depth = audio_depth;
    wave_data.channel_num = channel_num;
    wave_data.data = sound_array;
    wave_data.length = sizeof(sound_array);

    /* Initialize I2S pins and clock */
    init_i2s_pins(CODEC_I2S);
    i2s_mclk_freq = board_config_i2s_clock(CODEC_I2S, wave_data.sample_rate); /* Configure I2S clock according to sample rate */

    /* Initialize I2S peripheral */
    stat = board_i2s_init(&wave_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init I2S failed\n");
        return 1;
    }

    /* Initialize audio codec */
    stat = board_codec_init(&wave_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init Audio Codec failed\n");
        return 1;
    }

    /* Start I2S transfer and wait for completion */
    i2s_enable(CODEC_I2S); /* start I2S */
    while ((!audio_play_finished)) {
        __asm("nop");
    }
    i2s_disable(CODEC_I2S); /* stop I2S */
    printf("I2S interrupt play finished\n");

    return 0;
}
