/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This example demonstrates I2S audio playback using DMA transfer.
 * It shows how to:
 * 1. Configure I2S interface for audio codec
 * 2. Set up DMA channel for audio data transfer
 * 3. Implement audio playback with codec control
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

/* Audio codec selection based on board hardware */
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

/* DMA configuration definitions */
#define TEST_DMA             BOARD_APP_XDMA
#define TEST_DMA_CHANNEL     (2U)
#define TEST_I2S_DMA_IRQ     BOARD_APP_XDMA_IRQ
#define TEST_DMA_SRC_REQ     BOARD_APP_I2S_TX_DMA_REQ
#define TEST_DMAMUX_CH       DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_DMA, TEST_DMA_CHANNEL)

/* Audio data configuration structure */
typedef struct {
    uint32_t sample_rate;    /* Sampling frequency in Hz */
    uint8_t channel_num;     /* Number of audio channels (1: mono, 2: stereo) */
    uint8_t audio_depth;     /* Bit depth of audio samples (16 or 32 bits) */
    const uint8_t *data;     /* Pointer to audio data buffer */
    uint32_t length;         /* Length of audio data in bytes */
} audio_data_t;

/* DMA transfer status flags */
volatile bool dma_transfer_done;    /* Set when DMA transfer completes */
volatile bool dma_transfer_error;   /* Set if DMA transfer encounters an error */

/*
 * DMA interrupt handler
 * Monitors DMA transfer status and updates completion flags
 */
SDK_DECLARE_EXT_ISR_M(TEST_I2S_DMA_IRQ, isr_dma)
void isr_dma(void)
{
    hpm_stat_t stat;

    stat = dma_check_transfer_status(TEST_DMA, TEST_DMA_CHANNEL);
    if (0 != (stat & DMA_CHANNEL_STATUS_ERROR)) {
        dma_transfer_error = true;
    }
    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        dma_transfer_done = true;
    }
}

/*
 * Configure DMA channel for I2S data transfer
 * Parameters:
 * - audio_data: Audio configuration and data buffer
 * Returns:
 * - status_success: Configuration successful
 * - Other: Configuration failed
 */
hpm_stat_t config_dma_transfer_i2s_data(audio_data_t *audio_data)
{
    hpm_stat_t stat;
    dma_channel_config_t ch_config;
    uint8_t data_width;
    uint8_t data_shift_byte;

    /* Verify audio depth is either 16 or 32 bits */
    assert((audio_data->audio_depth == 16U) || (audio_data->audio_depth == 32U));

    /* Configure DMA transfer width based on audio depth */
    if (audio_data->audio_depth == 16U) {
        data_width = DMA_TRANSFER_WIDTH_HALF_WORD;
        data_shift_byte = 2U;
    } else {
        data_width = DMA_TRANSFER_WIDTH_WORD;
        data_shift_byte = 0U;
    }

    /* Set up DMA channel configuration */
    dma_default_channel_config(TEST_DMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)audio_data->data);
    ch_config.dst_addr = (uint32_t)&CODEC_I2S->TXD[CODEC_I2S_TX_DATA_LINE] + data_shift_byte;
    ch_config.src_width = data_width;
    ch_config.dst_width = data_width;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = audio_data->length;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;

    /* Apply DMA channel configuration */
    stat = dma_setup_channel(TEST_DMA, TEST_DMA_CHANNEL, &ch_config, true);
    return stat;
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
    i2s_config.enable_mclk_out = true;
    i2s_init(CODEC_I2S, &i2s_config);

    /* Configure I2S transfer parameters */
    i2s_get_default_transfer_config(&transfer);
    transfer.sample_rate = audio_data->sample_rate;
    transfer.audio_depth = audio_data->audio_depth;
    /* 1 chanel - channel slot mask 0x1; 2 channel - channel solt mask 0x3 */
    transfer.channel_slot_mask = (1 << audio_data->channel_num) - 1;
    transfer.data_line = CODEC_I2S_TX_DATA_LINE;
    transfer.master_mode = true;

    /* Apply I2S tx configuration */
    stat = i2s_config_tx(CODEC_I2S, mclk_freq, &transfer);
    if (stat != status_success) {
        return status_fail;
    }
    i2s_enable_tx_dma_request(CODEC_I2S); /* enable I2S TX DMA request */

    return status_success;
}

/*
 * Test I2S DMA playback
 * Performs multiple playback cycles using DMA transfer
 * Parameters:
 * - audio_data: Audio data and configuration
 * Returns:
 * - status_success: Playback successful
 * - status_fail: Playback failed
 */
hpm_stat_t test_i2s_dma_play(audio_data_t *audio_data)
{
    hpm_stat_t stat;

    /* Configure DMAMUX and enable interrupt */
    dmamux_config(BOARD_APP_DMAMUX, TEST_DMAMUX_CH, TEST_DMA_SRC_REQ, true);
    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, 1);

    /* Perform multiple playback cycles */
    for (uint8_t i = 0; i < 10; i++) {
        dma_transfer_done = false;
        dma_transfer_error = false;

        /* Configure and start DMA transfer */
        stat = config_dma_transfer_i2s_data(audio_data);
        if (stat != status_success) {
            printf("DMA transfer config failed\n");
            return status_fail;
        }

        i2s_reset_tx(CODEC_I2S); /* reset I2S tx and clear tx fifo */
        /* fill tx dummy data to tx fifo to prevent underflow when TX starts */
        if (i2s_fill_tx_dummy_data(CODEC_I2S, CODEC_I2S_TX_DATA_LINE, audio_data->channel_num) != status_success) {
            return status_fail;
        }
        i2s_enable(CODEC_I2S); /* start I2S */

        /* Wait for transfer completion */
        while ((!dma_transfer_error) && (!dma_transfer_done)) {
            __asm("nop");
        }

        i2s_disable(CODEC_I2S); /* stop I2S */

        if (dma_transfer_error) {
            printf("dma transfer i2s data failed\n");
            return status_fail;
        }
    }

    return status_success;
}

/*
 * Main function
 * Initializes system and performs I2S DMA playback test
 */
int main(void)
{
    uint32_t i2s_mclk_freq;
    hpm_stat_t stat;

    /* Initialize board basic peripherals */
    board_init();
    printf("I2S DMA example\n");

    /* Configure audio parameters */
    audio_data_t wave_data;
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

    /* Perform I2S DMA playback test */
    stat = test_i2s_dma_play(&wave_data);
    if (stat == status_success) {
        printf("i2s dma play finished\n");
    } else {
        printf("i2s dma play failed\n");
    }

    return 0;
}
