/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_i2s_drv.h"
#include "hpm_pdm_drv.h"
#include "hpm_dao_drv.h"
#include "audio_data.h"

#define I2S_MCLK_FREQ_IN_HZ (24576000UL)
#define DAO_I2S_CLK_NAME (clock_dao)

#ifndef USING_SMALL_BUFF
#define DATA_BUFF_SIZE 50960
#else
#define DATA_BUFF_SIZE 20480
#endif
uint32_t pdm_in[DATA_BUFF_SIZE];

struct audio_info {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t audio_depth;
};

struct audio_info wav_info;

const uint32_t sin_1khz_48khz[] = {
  0x00000000, 0x0D5DAA00, 0x1A80C900, 0x272FD100, 0x33333300, 0x3E565100, 0x48686100, 0x513D4800,
  0x58AE5600, 0x5E9AF300, 0x62E92A00, 0x65862200, 0x66666600, 0x65862200, 0x62E92B00, 0x5E9AF300,
  0x58AE5600, 0x513D4800, 0x48686100, 0x3E565100, 0x33333300, 0x272FD100, 0x1A80CA00, 0x0D5DAB00,
  0x00000000, 0xF2A25600, 0xE57F3700, 0xD8D02F00, 0xCCCCCD00, 0xC1A9B000, 0xB7979F00, 0xAEC2B800,
  0xA751AA00, 0xA1650E00, 0x9D16D600, 0x9A79DE00, 0x99999A00, 0x9A79DE00, 0x9D16D500, 0xA1650D00,
  0xA751AA00, 0xAEC2B700, 0xB7979E00, 0xC1A9AF00, 0xCCCCCC00, 0xD8D02E00, 0xE57F3600, 0xF2A25500,
};

void test_dao(const uint8_t *audio,
              uint32_t length,
              struct audio_info *info,
              uint8_t volume_shift,
              uint32_t loop_count)
{
    uint32_t i, j, oversample, data;
    uint32_t *ptr;

    if (info->sample_rate < DAO_SOC_SAMPLE_RATE_IN_HZ) {
        oversample = DAO_SOC_SAMPLE_RATE_IN_HZ / info->sample_rate;
    } else {
        oversample = 1;
    }
    /*
     * FIXME: oversample calculation needs to be checked
     */
    oversample *= 1;

    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;

    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_init(DAO_I2S, &i2s_config);
    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    if (status_success != i2s_config_tx(DAO_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer))
    {
        printf("I2S config failed for DAO\n");
        while(1);
    }

    dao_get_default_config(HPM_DAO, &dao_config);
    if (info->channel_num == 1) {
        dao_config.enable_mono_output = true;
    }
    dao_init(HPM_DAO, &dao_config);

    dao_start(HPM_DAO);

    for (uint8_t k = 0; k < loop_count; k++) {
        i = 0;
        while(i < length) {
            ptr = (uint32_t *)&audio[i];
            data = *(ptr) & (((uint64_t)1 << info->audio_depth) - 1);
            data <<= volume_shift; /* Volume up */
            if (i2s_get_tx_line_fifo_level(DAO_I2S, 0) <= 2){
                for (j = 0; j < oversample; j++) {
                    while(i2s_get_tx_line_fifo_level(DAO_I2S, 0) >= 1)  {}
                    i2s_send_data(DAO_I2S, 0, &data, 1);
                    i2s_send_data(DAO_I2S, 0, &data, 1);
                }
                i += info->audio_depth / 8;
            }
        }
    }

}

void test_pdm_to_dao(uint8_t volume_shift)
{
    uint32_t i, j, data;
    bool recording;

    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    pdm_config_t pdm_config;
    dao_config_t dao_config;

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
    if (status_success != i2s_config_rx(PDM_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer))
    {
        printf("I2S config failed for PDM\n");
        while(1);
    }

    pdm_get_default_config(HPM_PDM, &pdm_config);
    pdm_init(HPM_PDM, &pdm_config);

    i2s_init(DAO_I2S, &i2s_config);
    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    if (status_success != i2s_config_tx(DAO_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer))
    {
        printf("I2S config failed for DAO\n");
        while(1);
    }

    dao_get_default_config(HPM_DAO, &dao_config);
    dao_init(HPM_DAO, &dao_config);

    pdm_start(HPM_PDM);
    dao_start(HPM_DAO);

    recording = true;
    for (uint8_t k = 0; k < 3; k++) {
        i = 0;
        if (recording) {
            printf("Recording...\nPlease make some sound to mic0\n");
            while(i < ARRAY_SIZE(pdm_in)) {
                if (i2s_get_rx_line_fifo_level(PDM_I2S, 0)){
                    i2s_receive_data(PDM_I2S, 0, &pdm_in[i], 1);
                    i++;
                }
            }
        } else {
            i = 0;
            printf("Playing...\n");
            while(i < (ARRAY_SIZE(pdm_in))) {
                if (i2s_get_tx_line_fifo_level(DAO_I2S, 0) <= 2){
                    data = *(pdm_in + i);
                    data <<= volume_shift;
                    for (j = 0; j < DAO_SOC_PDM_SAMPLE_RATE_RATIO; j++) {
                        i2s_send_data(DAO_I2S, 0, &data, 1); /* Left channel */
                        i2s_send_data(DAO_I2S, 0, &data, 1); /* Right channel */
                    }
                    i += 1;
                }
            }
        }
        if (!(i < ARRAY_SIZE(pdm_in))) {
            recording = !recording;
        }
    }
}

int main(void)
{
    board_init();

    board_init_dao_clock();
    board_init_pdm_clock();

    init_dao_pins();
    init_pdm_pins();

    printf("i2s example\n");

    printf("testing pdm to dao\n");
    test_pdm_to_dao(2);

    printf("testing wav playback\n");
    wav_info.sample_rate = 8000;
    wav_info.channel_num = 1;
    wav_info.audio_depth = 16;
    test_dao(wav, sizeof(wav), &wav_info, 17, 1);

    printf("testing sine wave playback\n");
    wav_info.sample_rate = 48000;
    wav_info.channel_num = 1;
    wav_info.audio_depth = 32;
    test_dao((uint8_t *)sin_1khz_48khz, sizeof(sin_1khz_48khz), &wav_info, 0, 200);

    while(1);
    return 0;
}
