/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_i2s_drv.h"
#include "hpm_smix_drv.h"
#include "sound.h"
#include "bgm.h"

#ifdef CONFIG_CODEC_WM8960
    #include "hpm_wm8960.h"
#elif defined(CONFIG_CODEC_SGTL5000)
    #include "hpm_sgtl5000.h"
#else
    #error no specified Audio Codec!!!
#endif

/* codec related peripheral */
#ifndef BOARD_CODEC_I2C_BASE
#define CODEC_I2C            BOARD_APP_I2C_BASE
#else
#define CODEC_I2C            BOARD_CODEC_I2C_BASE
#endif
#define CODEC_I2S            BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME   BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_TX_DATA_LINE  BOARD_APP_I2S_TX_DATA_LINE

/* smix */
#define SMIX HPM_SMIX
#define SMIX_DMA_CH_FOR_SOURCE0 (0U)
#define SMIX_DMA_CH_FOR_SOURCE1 (1U)
#define SMIX_DMA_CH_FOR_DST     (2U)
#define SMIX_DMA_IRQ            IRQn_SMIX_DMA
#define SMIX_OUTPUT_SAMPLE_RATE (48000U)

volatile bool dma_transfer_done;

typedef struct {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t audio_depth;
    const uint8_t *data;
    uint32_t length;
} audio_data_t;

SDK_DECLARE_EXT_ISR_M(SMIX_DMA_IRQ, smix_dma_isr)
void smix_dma_isr(void)
{
    if (smix_dma_check_transfer_complete(SMIX, SMIX_DMA_CH_FOR_DST)) {
        dma_transfer_done = true;
    }
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

    i2s_get_default_transfer_config(&transfer);
    transfer.sample_rate = audio_data->sample_rate;
    transfer.audio_depth = audio_data->audio_depth;
    /* 1 chanel - channel slot mask 0x1; 2 channel - channel solt mask 0x3 */
    transfer.channel_slot_mask = (1 << audio_data->channel_num) - 1;
    transfer.data_line = CODEC_I2S_TX_DATA_LINE;
    transfer.master_mode = true;

    stat = i2s_config_tx(CODEC_I2S, mclk_freq, &transfer);
    if (stat != status_success) {
        return status_fail;
    }
    i2s_enable_tx_dma_request(CODEC_I2S); /* enable I2S TX DMA transfer */

    return status_success;
}

hpm_stat_t mixer_play_one_sound(audio_data_t *sound)
{
    smix_dma_ch_config_t source_dma, dst_dma;
    smix_mixer_source_config_t mixer_source;
    smix_mixer_dst_config_t mixer_dst;
    dma_transfer_done = false;

    assert((sound->audio_depth == 16) || (sound->audio_depth == 32));

    uint32_t ratio = SMIX_OUTPUT_SAMPLE_RATE / sound->sample_rate;
    uint32_t addr_shift = (32 - sound->audio_depth) / 8;
    uint32_t dma_width = sound->audio_depth / 8 - 1;
    uint32_t source_count = sound->length / (sound->audio_depth / 8);
    uint32_t dst_data_bytes = sound->length * ratio;

    printf("mixer play one sound\n");
    printf("input sound sample rate: %dHz\n", sound->sample_rate);
    printf("output sound sample rate: %dHz\n", SMIX_OUTPUT_SAMPLE_RATE);

    /* config source data DMA */
    smix_get_dma_default_ch_config(SMIX, &source_dma);
    source_dma.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)sound->data);
    source_dma.src_width = dma_width;
    source_dma.dst_addr = (uint32_t)&SMIX->SOURCE_CH[0].DATA + addr_shift; /* move valid data to high bit of register */
    source_dma.dst_width = dma_width;
    source_dma.dst_addr_ctrl = smix_dma_address_fixed;
    source_dma.dst_mode = smix_dma_mode_handshake;
    source_dma.dst_req_sel = smix_dma_req_mixer_src_ch0; /* align with dma smix source_dma channel */
    source_dma.trans_bytes = sound->length;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_SOURCE0, &source_dma, true);

    /* config dst data DMA */
    smix_get_dma_default_ch_config(SMIX, &dst_dma);
    dst_dma.src_addr = (uint32_t)&SMIX->DST_CH[0].DATA + addr_shift;
    dst_dma.src_width = dma_width;
    dst_dma.src_addr_ctrl = smix_dma_address_fixed;
    dst_dma.dst_addr = (uint32_t)&CODEC_I2S->TXD[CODEC_I2S_TX_DATA_LINE] + addr_shift;
    dst_dma.dst_width = dma_width;
    dst_dma.dst_addr_ctrl = smix_dma_address_fixed;
    dst_dma.dst_req_sel = smix_dma_req_i2s3_tx; /* align with I2s which use to play sound */
    dst_dma.dst_mode = smix_dma_mode_handshake;
    dst_dma.src_burst_size = smix_dma_transfer_burst_8t;
    dst_dma.trans_bytes = dst_data_bytes;
    dst_dma.complete_int_en = true;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_DST, &dst_dma, true);

    /* config mixer source */
    smix_get_mixer_source_ch_default_config(SMIX, &mixer_source);
    mixer_source.convert_rate = smix_mixer_upper_6x_sample;
    mixer_source.length = (source_count - 4) * ratio;
    smix_mixer_config_source_ch(SMIX, 0, &mixer_source);

    /* config mixer dst */
    smix_get_mixer_dst_ch_default_config(SMIX, &mixer_dst);
    mixer_dst.src_ch_mask = 0x1; /* enable source channel 0 as input */
    smix_mixer_config_dst_ch(SMIX, 0, &mixer_dst);

    i2s_reset_tx(CODEC_I2S);
    if (i2s_fill_tx_dummy_data(CODEC_I2S, CODEC_I2S_TX_DATA_LINE, sound->channel_num) != status_success) {
        printf("I2S error occurred during playing\n");
        return status_fail;
    }
    i2s_start(CODEC_I2S); /* start I2S */

    while (!dma_transfer_done) {
        __asm("nop");
    }

    /* disable source channel 0 to dst channel 0*/
    smix_mixer_dst_disable(SMIX);
    smix_mixer_dst_disable_source_channel(SMIX, 0, 1);
    i2s_stop(CODEC_I2S); /* stop I2S */

    if (smix_mixer_check_dst_cal_saturation_error(SMIX, 0)
        || smix_mixer_check_source_cal_saturation_error(SMIX, 0)
        || smix_mixer_check_dst_data_underflew(SMIX, 0)) {
        printf("SMIX error occurred during playing\n");
        return status_fail;
    }

    return status_success;
}

hpm_stat_t mixer_play_two_sound(audio_data_t *sound0, audio_data_t *sound1)
{
    smix_dma_ch_config_t sound0_dma, sound1_dma, dst_dma;
    smix_mixer_source_config_t mixer_source0, mixer_source1;
    smix_mixer_dst_config_t mixer_dst;
    dma_transfer_done = false;

    assert((sound0->audio_depth == 16) || (sound0->audio_depth == 32));
    assert((sound1->audio_depth == 16) || (sound1->audio_depth == 32));
    assert(sound0->channel_num == sound1->channel_num);

    uint32_t ratio0 = SMIX_OUTPUT_SAMPLE_RATE / sound0->sample_rate;
    uint32_t dma_width0 = sound0->audio_depth / 8 - 1;
    uint32_t addr_shift0 = (32 - sound0->audio_depth) / 8;
    uint32_t source0_count = sound0->length / (sound0->audio_depth / 8);

    uint32_t ratio1 = SMIX_OUTPUT_SAMPLE_RATE / sound1->sample_rate;
    uint32_t dma_width1 = sound1->audio_depth / 8 - 1;
    uint32_t addr_shift1 = (32 - sound1->audio_depth) / 8;
    uint32_t source1_count = sound1->length / (sound1->audio_depth / 8);

    uint32_t dst_audio_depth = MAX(sound0->audio_depth, sound1->audio_depth);
    uint32_t dst_dma_width = dst_audio_depth / 8 - 1;
    uint32_t dst_addr_shift = (32 - dst_audio_depth) / 8;
    uint32_t dst_count = MAX(source0_count * ratio0, source1_count * ratio1) * dst_audio_depth / 8;

    printf("mixer play two sound\n");
    printf("input sound0 sample rate: %dHz\n", sound0->sample_rate);
    printf("input sound1 sample rate: %dHz\n", sound1->sample_rate);
    printf("output sound sample rate: %dHz\n", SMIX_OUTPUT_SAMPLE_RATE);

    /* config source0 data DMA */
    smix_get_dma_default_ch_config(SMIX, &sound0_dma);
    sound0_dma.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)sound0->data);
    sound0_dma.src_width = dma_width0;
    sound0_dma.dst_addr = (uint32_t)&SMIX->SOURCE_CH[0].DATA + addr_shift0;
    sound0_dma.dst_width = dma_width0;
    sound0_dma.dst_addr_ctrl = smix_dma_address_fixed;
    sound0_dma.dst_mode = smix_dma_mode_handshake;
    sound0_dma.dst_req_sel = smix_dma_req_mixer_src_ch0;  /* align with dma smix source_dma channel */
    sound0_dma.trans_bytes = sound0->length;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_SOURCE0, &sound0_dma, true);

    /* config source1 data DMA */
    smix_get_dma_default_ch_config(SMIX, &sound1_dma);
    sound1_dma.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)sound1->data);
    sound1_dma.src_width = dma_width1;
    sound1_dma.dst_addr = (uint32_t)&SMIX->SOURCE_CH[1].DATA + addr_shift1;
    sound1_dma.dst_width = dma_width1;
    sound1_dma.dst_addr_ctrl = smix_dma_address_fixed;
    sound1_dma.dst_mode = smix_dma_mode_handshake;
    sound1_dma.dst_req_sel = smix_dma_req_mixer_src_ch1; /* align with dma smix source_dma channel */
    sound1_dma.trans_bytes = sound1->length;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_SOURCE1, &sound1_dma, true);

    smix_get_dma_default_ch_config(SMIX, &dst_dma);
    dst_dma.src_addr = (uint32_t)&SMIX->DST_CH[0].DATA + dst_addr_shift;
    dst_dma.src_width = dst_dma_width;
    dst_dma.src_addr_ctrl = smix_dma_address_fixed;
    dst_dma.dst_addr = (uint32_t)&CODEC_I2S->TXD[CODEC_I2S_TX_DATA_LINE] + dst_addr_shift;
    dst_dma.dst_width = dst_dma_width;
    dst_dma.dst_addr_ctrl = smix_dma_address_fixed;
    dst_dma.dst_req_sel = smix_dma_req_i2s3_tx;
    dst_dma.dst_mode = smix_dma_mode_handshake;
    dst_dma.src_burst_size = smix_dma_transfer_burst_8t;
    dst_dma.trans_bytes = dst_count * dst_audio_depth / 8;
    dst_dma.complete_int_en = true;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_DST, &dst_dma, true);

    /*  */
    smix_get_mixer_source_ch_default_config(SMIX, &mixer_source0);
    mixer_source0.convert_rate = smix_mixer_upper_6x_sample; /* adjust based on source sample rate and terget sample rate */
    mixer_source0.length = (source0_count - 4) * ratio0;
    smix_mixer_config_source_ch(SMIX, 0, &mixer_source0);

    smix_get_mixer_source_ch_default_config(SMIX, &mixer_source1);
    mixer_source1.convert_rate = smix_mixer_upper_6x_sample;
    mixer_source1.length = (source1_count - 4) * ratio1;
    smix_mixer_config_source_ch(SMIX, 1, &mixer_source1);

    smix_get_mixer_dst_ch_default_config(SMIX, &mixer_dst);
    mixer_dst.src_ch_mask = 0x3;
    smix_mixer_config_dst_ch(SMIX, 0, &mixer_dst);

    i2s_reset_tx(CODEC_I2S);
    if (i2s_fill_tx_dummy_data(CODEC_I2S, CODEC_I2S_TX_DATA_LINE, sound0->channel_num) != status_success) {
        printf("I2S error occurred during playing\n");
        return status_fail;
    }
    i2s_start(CODEC_I2S); /* start I2S */

    while (!dma_transfer_done) {
        __asm("nop");
    }

    /* disable source channel 0 and 1 to dst channel 0*/
    smix_mixer_dst_disable(SMIX);
    smix_mixer_dst_disable_source_channel(SMIX, 0, 3);
    i2s_stop(CODEC_I2S); /* stop I2S */

    if (smix_mixer_check_dst_cal_saturation_error(SMIX, 0)
        || smix_mixer_check_source_cal_saturation_error(SMIX, 0)
        || smix_mixer_check_dst_data_underflew(SMIX, 0)) {
        printf("SMIX error occurred during playing\n");
        return status_fail;
    }

    return status_success;
}

int main(void)
{
    uint32_t i2s_mclk_freq;
    hpm_stat_t stat;

    board_init();
    clock_add_to_group(clock_smix, 0);
    printf("Sound mixer example\n");

    audio_data_t source0_data;
    source0_data.sample_rate = sound_sample_rate;
    source0_data.audio_depth = sound_audio_depth;
    source0_data.channel_num = sound_channel_num;
    source0_data.data = sound;
    source0_data.length = sizeof(sound);

    audio_data_t source1_data;
    source1_data.sample_rate = bgm_sample_rate;
    source1_data.audio_depth = bgm_audio_depth;
    source1_data.channel_num = bgm_channel_num;
    source1_data.data = bgm;
    source1_data.length = sizeof(bgm);

    audio_data_t target_data;
    target_data.sample_rate = SMIX_OUTPUT_SAMPLE_RATE;
    target_data.audio_depth = source0_data.audio_depth;
    target_data.channel_num = source0_data.channel_num;

    init_i2s_pins(CODEC_I2S);
    i2s_mclk_freq = board_config_i2s_clock(CODEC_I2S, SMIX_OUTPUT_SAMPLE_RATE);

    /* config I2S peripehral */
    stat = board_i2s_init(&target_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init I2S failed\n");
        return 1;
    }

    /* config codec on baord */
    stat = board_codec_init(&target_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init Audio Codec failed\n");
        return 1;
    }

    intc_m_enable_irq_with_priority(IRQn_SMIX_DMA, 1);

    mixer_play_one_sound(&source0_data);

    mixer_play_one_sound(&source1_data);

    mixer_play_two_sound(&source0_data, &source1_data);

    while (1) {
    }

    return 0;
}
