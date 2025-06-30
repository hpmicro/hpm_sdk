/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This example demonstrates the usage of I2S with PDM and DAO peripherals
 * for audio recording and playback functionality.
 */

#include "board.h"
#include <stdio.h>
#include "hpm_i2s_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_PDMLITE
#include "hpm_pdmlite_drv.h"
#else
#include "hpm_pdm_drv.h"
#endif
#include "hpm_dao_drv.h"
#include "audio_data.h"

/*
 * Configure buffer size for PDM recording
 * Normal mode: 48K (1024 * 48)
 * Small buffer mode: 20K (20480)
 */
#ifndef BOARD_PDM_USING_SMALL_BUFF
#define DATA_BUFF_SIZE (1024 * 48)
#else
#define DATA_BUFF_SIZE 20480
#endif
uint32_t pdm_in[DATA_BUFF_SIZE]; /* Buffer for PDM recorded data */

/*
 * Structure to hold audio configuration parameters
 * - sample_rate: Audio sampling frequency in Hz
 * - channel_num: Number of audio channels (1 for mono, 2 for stereo)
 * - audio_depth: Bit depth of audio samples (16/24/32 bits)
 */
struct audio_info {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t audio_depth;
};

struct audio_info wav_info;

/*
 * Pre-defined 1kHz sine wave data sampled at 48kHz
 * - 32-bit samples
 */
const uint32_t sin_1khz_48khz[] = {
  0x00000000, 0x0D5DAA00, 0x1A80C900, 0x272FD100, 0x33333300, 0x3E565100, 0x48686100, 0x513D4800,
  0x58AE5600, 0x5E9AF300, 0x62E92A00, 0x65862200, 0x66666600, 0x65862200, 0x62E92B00, 0x5E9AF300,
  0x58AE5600, 0x513D4800, 0x48686100, 0x3E565100, 0x33333300, 0x272FD100, 0x1A80CA00, 0x0D5DAB00,
  0x00000000, 0xF2A25600, 0xE57F3700, 0xD8D02F00, 0xCCCCCD00, 0xC1A9B000, 0xB7979F00, 0xAEC2B800,
  0xA751AA00, 0xA1650E00, 0x9D16D600, 0x9A79DE00, 0x99999A00, 0x9A79DE00, 0x9D16D500, 0xA1650D00,
  0xA751AA00, 0xAEC2B700, 0xB7979E00, 0xC1A9AF00, 0xCCCCCC00, 0xD8D02E00, 0xE57F3600, 0xF2A25500,
};

/*
 * Function: test_dao
 * Description: Implements DAO playback functionality
 * Parameters:
 * - audio: Pointer to audio data buffer
 * - length: Length of audio data in bytes
 * - info: Audio configuration parameters
 * - volume_scale: Volume adjustment factor
 * - loop_count: Number of times to loop the audio playback
 *
 * Note: DAO operates at fixed 48KHz sample rate and stereo mode. Input audio will be
 * resampled if necessary using simple sample repetition.
 */
void test_dao(const uint8_t *audio,  uint32_t length, struct audio_info *info, float volume_scale, uint32_t loop_count)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;

    /* Calculate oversampling factor for sample rate conversion */
    uint32_t oversample;
    assert(DAO_SOC_SAMPLE_RATE_IN_HZ >= info->sample_rate);
    oversample = DAO_SOC_SAMPLE_RATE_IN_HZ / info->sample_rate;

    /* Initialize I2S with basic configuration */
    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_init(DAO_I2S, &i2s_config);

    /* Configure I2S transfer parameters for DAO */
    i2s_get_default_transfer_config_for_dao(&transfer);
    uint32_t mclk_freq = clock_get_frequency(clock_i2s1);
    if (status_success != i2s_config_tx(DAO_I2S, mclk_freq, &transfer)) {
        printf("I2S config failed for DAO\n");
        while (1) {
        }
    }

    /* Initialize DAO with default configuration */
    dao_get_default_config(HPM_DAO, &dao_config);
    dao_init(HPM_DAO, &dao_config);

    /* Start DAO and I2S transmission */
    i2s_start(DAO_I2S);
    dao_start(HPM_DAO);

    /* Main playback loop */
    uint32_t i = 0;
    uint32_t *ptr;
    int32_t data;
    for (uint8_t k = 0; k < loop_count; k++) {
        i = 0;
        while(i < length) {
            ptr = (uint32_t *)&audio[i];
            /* Align audio data to high bits of 32-bit register */
            data = *(ptr) << (32 - info->audio_depth);
            data *= volume_scale; /* Apply volume adjustment */

            /* Handle sample rate conversion through oversampling */
            for (uint32_t j = 0; j < oversample; j++) {
                /* Wait for FIFO space */
                while (i2s_get_tx_line_fifo_level(DAO_I2S, 0) >= I2S_SOC_MAX_TX_FIFO_DEPTH / 2) {
                }
                /* Send data to I2S TX FIFO */
                i2s_send_data(DAO_I2S, 0, data);
                /* For mono audio, duplicate data to both channels */
                if (info->channel_num == 1) {
                    i2s_send_data(DAO_I2S, 0, data);
                }
            }
            i += info->audio_depth / 8;
        }
    }

    /* Stop DAO and I2S transmission */
    i2s_stop(DAO_I2S);
    dao_stop(HPM_DAO);
}

/*
 * Function: test_pdm
 * Description: Implements PDM recording functionality
 * Parameters:
 * - audio: Buffer to store recorded audio data
 * - length: Length of buffer in bytes
 * - info: Audio configuration parameters
 *
 * Features:
 * - I2S interface is configured with 32-bit data width for PDM operation
 * - Actual audio sample depth is adjusted during RX FIFO data processing:
 *   - Data is read as 32-bit from RX FIFO
 *   - Bit shifting is applied based on desired audio_depth
 *   - Only the valid bits are stored in the output buffer
 * - Supports configurable sample rates within hardware limits
 * - Supports up to 8 channels (4 stereo pairs) in TDM mode
 */
void test_pdm(uint8_t *audio, uint32_t length, struct audio_info *info)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    pdm_config_t pdm_config;

    /* Initialize I2S for PDM operation */
    i2s_get_default_config(PDM_I2S, &i2s_config);
    i2s_init(PDM_I2S, &i2s_config);

    /* Configure I2S transfer parameters for PDM */
    i2s_get_default_transfer_config_for_pdm(&transfer);
    transfer.sample_rate = info->sample_rate;
    /* Configure channel mask based on mono/stereo selection */
    if (info->channel_num == 2) {
        transfer.channel_slot_mask = BOARD_PDM_DUAL_CHANNEL_MASK;
    } else if (info->channel_num == 1) {
        transfer.channel_slot_mask = BOARD_PDM_SINGLE_CHANNEL_MASK;
    } else {
        printf("More channels require hardware support on board\n");
        while (1) {
        }
    }

    /* Configure I2S receiver */
    uint32_t mclk_freq = clock_get_frequency(clock_i2s0);
    if (status_success != i2s_config_rx(PDM_I2S, mclk_freq, &transfer)) {
        printf("I2S config failed for PDM\n");
        while (1) {
        }
    }

    /* Initialize PDM with default configuration */
    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);

    /* PDM sample rate configuration, limits frequency can be configurated related mclk_freq and pdm_config setting */
    if (status_success != pdm_config_sample_rate(HPM_PDM, mclk_freq, info->sample_rate)) {
        printf("PDM config sample rate failed\n");
        while (1) {
        }
    }

    /* Start PDM and I2S */
    i2s_start(PDM_I2S);
    pdm_start(HPM_PDM);

    /* Record data processing loop */
    uint32_t i = 0;
    uint32_t data;
    uint8_t sample_bytes = info->audio_depth / 8;
    uint8_t bit_shift = 32 - info->audio_depth;
    while (i < length) {
        if (i2s_get_rx_line_fifo_level(PDM_I2S, 0) > 0) {
            i2s_receive_data(PDM_I2S, 0, &data);    /* Get sample from FIFO */
            data = data >> bit_shift;               /* Align to proper bit width */
            memcpy(&audio[i], &data, sample_bytes); /* Store in output buffer */
            i += sample_bytes;
        }
    }

    /* Stop PDM and I2S */
    i2s_stop(PDM_I2S);
    pdm_stop(HPM_PDM);
}

/*
 * Function: test_pdm_to_dao
 * Description: Demonstrates complete audio loop - record via PDM and playback via DAO
 * Parameters:
 * - volume_scale: Volume adjustment for playback
 */
void test_pdm_to_dao(float volume_scale)
{
    printf("Please enter any character to start recording\n");
    getchar();
    wav_info.sample_rate = PDM_SOC_SAMPLE_RATE_IN_HZ;
    wav_info.channel_num = 2;
    wav_info.audio_depth = 32;
    test_pdm((uint8_t *)pdm_in, sizeof(pdm_in), &wav_info);
    printf("Recording finish\n");

    printf("Please enter any character to start playing\n");
    getchar();
    wav_info.sample_rate = PDM_SOC_SAMPLE_RATE_IN_HZ;
    wav_info.channel_num = 2;
    wav_info.audio_depth = 32;
    test_dao((uint8_t *)pdm_in, sizeof(pdm_in), &wav_info, volume_scale, 1);
    printf("Playing finish\n");
}

/*
 * Function: main
 * Description: Main program entry point
 * Demonstrates three audio scenarios:
 * 1. WAV file playback (8kHz, mono, 16-bit)
 * 2. Sine wave playback (48kHz, mono, 32-bit)
 * 3. PDM recording and playback
 */
int main(void)
{
    /* Initialize board basic peripherals and DAO/PDM clocks */
    board_init();
    board_init_dao_clock();
    board_init_pdm_clock();

    /* Initialize I/O pins */
    init_dao_pins();
    init_pdm_pins();

    printf("DAO and PDM with I2S example\n");

    /* Test 1: Play pre-recorded WAV file */
    printf("1. Testing DAO wav playback\n");
    wav_info.sample_rate = 8000;
    wav_info.channel_num = 1;
    wav_info.audio_depth = 16;
    test_dao(wav, sizeof(wav), &wav_info, 0.6, 1);

    /* Test 2: Play generated sine wave */
    printf("2. Testing DAO sine wave playback\n");
    wav_info.sample_rate = 48000;
    wav_info.channel_num = 1;
    wav_info.audio_depth = 32;
    test_dao((uint8_t *)sin_1khz_48khz, sizeof(sin_1khz_48khz), &wav_info, 0.6, 200);

    /* Test 3: Record and playback through PDM */
    printf("3. Testing PDM record and DAO playback\n");
    test_pdm_to_dao(0.6);

    /* Infinite loop */
    while (1) {
    }

    return 0;
}
