/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_i2s_drv.h"
#include "audio_codec_common.h"

/*****************************************************************************************************************
 *
 *  Definitions
 *
 *****************************************************************************************************************/
#if defined(USING_CODEC) && USING_CODEC
    #if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
        #include "hpm_wm8960.h"
        wm8960_config_t wm8960_config = {
            .route       = wm8960_route_playback,
            .left_input  = wm8960_input_closed,
            .right_input = wm8960_input_closed,
            .play_source = wm8960_play_source_dac,
            .bus         = wm8960_bus_left_justified,
            .format = {.mclk_hz = 0U, .sample_rate = 0U, .bit_width = 32U},
        };

        wm8960_control_t wm8960_control = {
            .ptr = CODEC_I2C,
            .slave_address = WM8960_I2C_ADDR, /* I2C address */
        };
    #elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
        #include "hpm_sgtl5000.h"
        sgtl_config_t sgtl5000_config = {
            .route = sgtl_route_playback_record, /*!< Audio data route.*/
            .bus = sgtl_bus_left_justified, /*!< Audio transfer protocol */
            .master = false, /*!< Master or slave. True means master, false means slave. */
            .format = { .mclk_hz = 0,
                .sample_rate = 0,
                .bit_width = 32,
                .sclk_edge = sgtl_sclk_valid_edge_rising }, /*!< audio format */
        };

        sgtl_context_t sgtl5000_context = {
            .ptr = CODEC_I2C,
            .slave_address = SGTL5000_I2C_ADDR, /* I2C address */
        };
    #else
        #error no specified Audio Codec!!!
    #endif
#elif defined(USING_DAO) && USING_DAO
    #include "hpm_dao_drv.h"
    dao_config_t dao_config;
#else
    #error define USING_CODEC or USING_DAO
#endif

#if defined(USING_CODEC) && USING_CODEC
void init_codec(void)
{
    board_init_i2c(CODEC_I2C);

    init_i2s_pins(TARGET_I2S);
    board_config_i2s_clock(TARGET_I2S, 48000);
}
#endif

#if defined(USING_DAO) && USING_DAO
void init_dao(void)
{
    init_dao_pins();
    board_init_dao_clock();
}
#endif

hpm_stat_t init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    uint32_t i2s_mclk_hz;
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    if (channel_num > 2) {
        return status_invalid_argument; /* Currently not support TDM mode */
    }

    i2s_get_default_config(TARGET_I2S, &i2s_config);
#if defined(USING_CODEC) && USING_CODEC
    i2s_config.enable_mclk_out = true;
#endif
    i2s_init(TARGET_I2S, &i2s_config);

    i2s_enable_tx_dma_request(TARGET_I2S);
    dmamux_config(BOARD_APP_DMAMUX, TARGET_I2S_TX_DMAMUX_CH, TARGET_I2S_TX_DMAMUX_SRC, true);
    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, TARGET_I2S_TX_DMA_IRQ_PRIORITY);

    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.data_line = TARGET_I2S_TX_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.channel_num_per_frame = 2; /* non TDM mode, channel num fix to 2. */
    transfer.channel_slot_mask = 0x3;   /* data from hpm_wav_decode()/convert_data_format() API is 2 channels */

    i2s_mclk_hz = board_config_i2s_clock(TARGET_I2S, sample_rate);

    if (status_success != i2s_config_tx(TARGET_I2S, i2s_mclk_hz, &transfer)) {
        return status_fail;
    }

#if defined(USING_CODEC) && USING_CODEC
    #if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
        wm8960_config.route = wm8960_route_playback;
        wm8960_config.format.sample_rate = sample_rate;
        wm8960_config.format.bit_width = audio_depth;
        wm8960_config.format.mclk_hz = i2s_mclk_hz;
        if (wm8960_init(&wm8960_control, &wm8960_config) != status_success) {
            printf("Init Audio Codec failed\n");
        }
        wm8960_set_volume(&wm8960_control, wm8960_module_dac, 200);
    #elif defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
        sgtl5000_config.route = sgtl_route_playback;
        sgtl5000_config.format.sample_rate = sample_rate;
        sgtl5000_config.format.bit_width = audio_depth;
        sgtl5000_config.format.mclk_hz = i2s_mclk_hz;
        if (sgtl_init(&sgtl5000_context, &sgtl5000_config) != status_success) {
            printf("Init Audio Codec failed\n");
        }
        sgtl_set_volume(&sgtl5000_context, sgtl_module_dac, SGTL5000_DAC_MIN_VOLUME_VALUE);
    #endif
#elif defined(USING_DAO) && USING_DAO
    dao_get_default_config(HPM_DAO, &dao_config);
#if defined(DAO_SOC_SUPPORT_DATA_FORMAT_CONFIG) && (DAO_SOC_SUPPORT_DATA_FORMAT_CONFIG == 1)
    dao_config.audio_depth = audio_depth;
#endif
    dao_init(HPM_DAO, &dao_config);
#endif

    return status_success;
}

void i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_XDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&TARGET_I2S->TXD[TARGET_I2S_TX_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = size;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = 0;

    if (status_success != dma_setup_channel(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
}
