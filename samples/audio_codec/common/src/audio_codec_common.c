/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include "board.h"

#include "hpm_clock_drv.h"
#include "hpm_l1c_drv.h"
#ifdef CONFIG_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_i2s_drv.h"
#include "hpm_wav_codec.h"
#include "ff.h"
#include "diskio.h"
#include "sd_fatfs_common.h"
#include "audio_codec_common.h"

/*****************************************************************************************************************
 *
 *  Definitions
 *
 *****************************************************************************************************************/
#if USING_CODEC
    #define CODEC_I2C BOARD_APP_I2C_BASE
    #define TARGET_I2S BOARD_APP_I2S_BASE
    #define TARGET_I2S_CLK_NAME BOARD_APP_I2S_CLK_NAME
    #define TARGET_I2S_DATA_LINE BOARD_APP_I2S_DATA_LINE
    #define TARGET_I2S_TX_DMAMUX_SRC HPM_DMA_SRC_I2S0_TX

    #if CONFIG_CODEC_WM8960
        #include "hpm_wm8960.h"
        wm8960_config_t wm8960_config = {
            .route       = wm8960_route_playback,
            .left_input  = wm8960_input_closed,
            .right_input = wm8960_input_closed,
            .play_source = wm8960_play_source_dac,
            .bus         = wm8960_bus_i2s,
            .format = {.mclk_hz = 0U, .sample_rate = 0U, .bit_width = 32U},
        };

        wm8960_control_t wm8960_control = {
            .ptr = CODEC_I2C,
            .slave_address = WM8960_I2C_ADDR, /* I2C address */
        };
    #elif CONFIG_CODEC_SGTL5000
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
#elif USING_DAO
    #define TARGET_I2S DAO_I2S
    #define TARGET_I2S_CLK_NAME clock_i2s1
    #define TARGET_I2S_DATA_LINE 0
    #define TARGET_I2S_TX_DMAMUX_SRC HPM_DMA_SRC_I2S1_TX

    #include "hpm_dao_drv.h"
    dao_config_t dao_config;
#else
    #error define USING_CODEC or USING_DAO
#endif

#ifndef TARGET_I2S_TX_DMA_CH
#define TARGET_I2S_TX_DMA_CH 2
#endif
#define TARGET_I2S_TX_DMAMUX_CH DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH)

#ifndef TARGET_I2S_TX_DMA_IRQ_PRIORITY
#define TARGET_I2S_TX_DMA_IRQ_PRIORITY 7
#endif

#define CODEC_BUFF_CNT 3
#define CODEC_BUFF_SIZE 20480

/*****************************************************************************************************************
 *
 *  Prototypes
 *
 *****************************************************************************************************************/
static void init_audio_player_no_printf(char *fname);
static hpm_stat_t init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
static void i2s_dma_start_transfer(uint32_t addr, uint32_t size);
static bool is_i2s_buff_full(void);
static bool is_i2s_buff_empty(void);

/*****************************************************************************************************************
 *
 *  Extern Function Decalarion
 *
 *****************************************************************************************************************/
extern bool is_music_playing(void);
extern char *_lv_demo_music_get_title(uint32_t track_id);

/*****************************************************************************************************************
 *
 *  Variables
 *
 *****************************************************************************************************************/
static FIL wav_file;
static hpm_wav_ctrl wav_ctrl;
static uint32_t i2s_mclk_hz;
ATTR_ALIGN(4) static uint8_t wav_header_buff[512];
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint8_t i2s_buff[CODEC_BUFF_CNT][CODEC_BUFF_SIZE];
static volatile uint32_t i2s_buff_fill_size[CODEC_BUFF_CNT];
static volatile uint8_t s_i2s_buff_front;
static volatile uint8_t s_i2s_buff_rear;
static bool s_i2s_buff_first_tranferred;
static bool s_playing_finished;
static bool s_switch_songs_req;
static bool s_init_audio_player_req;
static uint32_t s_track_id;

/*****************************************************************************************************************
 *
 *  Codes
 *
 *****************************************************************************************************************/
hpm_stat_t hpm_audiocodec_search_file(char *file_name, HPM_AUDIOCODEC_FILE *fil)
{
    FRESULT rsl = f_open(&wav_file, file_name, FA_READ);
    if (rsl == FR_OK) {
        *fil = (uint32_t)&wav_file;
        return status_success;
    } else {
        return status_audio_codec_none_file;
    }
}

hpm_stat_t hpm_audiocodec_read_file(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br)
{
    FRESULT rsl = f_read((FIL *)fil, data, num_bytes, (UINT *)br);
    if (rsl == FR_OK) {
        return status_success;
    } else {
        return status_fail;
    }
}

hpm_stat_t hpm_audiocodec_write_file(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br)
{
    FRESULT rsl = f_write((FIL *)fil, data, num_bytes, (UINT *)br);
    if (rsl == FR_OK) {
        return status_success;
    } else {
        return status_fail;
    }
}

hpm_stat_t hpm_audiocodec_close_file(HPM_AUDIOCODEC_FILE fil)
{
    FRESULT rsl = f_close((FIL *)fil);
    if (rsl == FR_OK) {
        return status_success;
    } else {
        return status_fail;
    }
}

void isr_dma(void)
{
    volatile hpm_stat_t stat;

    stat = dma_check_transfer_status(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH);

    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        if (!is_i2s_buff_empty()) {
            i2s_dma_start_transfer((uint32_t)i2s_buff[s_i2s_buff_front], i2s_buff_fill_size[s_i2s_buff_front]);
            s_i2s_buff_front++;
            if (s_i2s_buff_front >= CODEC_BUFF_CNT) {
                s_i2s_buff_front = 0u;
            }
        }
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, isr_dma)

#ifdef USING_CODEC
void init_codec(void)
{
    board_init_i2c(CODEC_I2C);

    init_i2s_pins(TARGET_I2S);
    board_init_i2s_clock(TARGET_I2S);
}
#endif

#ifdef USING_DAO
void init_dao(void)
{
    init_dao_pins();
    board_init_dao_clock();
}
#endif

void init_wav_ctrl_func(void)
{
    wav_ctrl.func.close_file = hpm_audiocodec_close_file;
    wav_ctrl.func.read_file = hpm_audiocodec_read_file;
    wav_ctrl.func.search_file = hpm_audiocodec_search_file;
    wav_ctrl.func.write_file = hpm_audiocodec_write_file;
}

void lv_audio_codec_task(void)
{
    static uint8_t s_ctrl_state;

    if (s_init_audio_player_req) {
        init_audio_player_no_printf((char *)_lv_demo_music_get_title(s_track_id));
        s_init_audio_player_req = false;
    }

    if (is_music_playing()) {
        hpm_playback_wav();
    }

    switch (s_ctrl_state) {
    case 1:
        if (!is_music_playing()) {
            dma_disable_channel(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH);
            if ((I2S_STA_TX_DN_GET(i2s_get_irq_status(TARGET_I2S)) & (0x01 << TARGET_I2S_DATA_LINE)) != 0u) {
#ifdef USING_DAO
                dao_stop(HPM_DAO);
#endif
                s_ctrl_state = 0;
            }
        }
        break;

    case 0:
    default:
        if (is_music_playing()) {
#ifdef USING_DAO
            dao_start(HPM_DAO);
#endif
            dma_enable_channel(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH);
            s_ctrl_state = 1;
        }
        break;
    }

    if (s_playing_finished && is_i2s_buff_empty() && ((I2S_STA_TX_DN_GET(i2s_get_irq_status(TARGET_I2S)) & (0x01 << TARGET_I2S_DATA_LINE)) != 0u)) {
        set_switch_songs_req(true);
    }
}

void init_audio_player(char *fname)
{
    hpm_stat_t res;

    dma_abort_channel(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH);
    if (wav_ctrl.func.file != 0) {
        wav_ctrl.func.close_file(wav_ctrl.func.file);
    }
    s_i2s_buff_rear = 0;
    s_i2s_buff_front = 0;
    res = hpm_wav_decode_init(fname, &wav_ctrl, &wav_header_buff);
    if (res == status_success) {
        printf("wav_name:%s.\r\n", fname);
        printf("wav samperate: %d\n", wav_ctrl.wav_head.fmt_chunk.samplerate);
        printf("wav sampebits: %d\n", wav_ctrl.wav_head.fmt_chunk.bitspersample);
        printf("wav channels: %d\n", wav_ctrl.wav_head.fmt_chunk.channels);
        printf("music playing time:%d seconds ...\n", wav_ctrl.sec_total);
        f_lseek(&wav_file, wav_ctrl.data_pos);
        if (status_success != init_i2s_playback(wav_ctrl.wav_head.fmt_chunk.samplerate,
                                wav_ctrl.wav_head.fmt_chunk.bitspersample, wav_ctrl.wav_head.fmt_chunk.channels)) {
            printf("config i2s transfer failed.\n");
        }
        s_i2s_buff_first_tranferred = false;
        s_playing_finished = false;
    } else {
        printf("music file error.\r\n");
    }
}

void hpm_playback_wav(void)
{
    uint8_t rear;

    if (!is_i2s_buff_full() && !s_playing_finished) {
        rear = s_i2s_buff_rear;
        s_i2s_buff_rear++;
        if (s_i2s_buff_rear >= CODEC_BUFF_CNT) {
            s_i2s_buff_rear = 0u;
        }
        i2s_buff_fill_size[rear] = hpm_wav_decode(&wav_ctrl, i2s_buff[rear], CODEC_BUFF_SIZE);
        if (l1c_dc_is_enabled()) {
            l1c_dc_flush(HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)i2s_buff[rear]),
                HPM_L1C_CACHELINE_ALIGN_UP((CODEC_BUFF_SIZE + i2s_buff[rear])) - HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)i2s_buff[rear]));
        }

        if (!s_i2s_buff_first_tranferred) {
#ifdef USING_DAO
            dao_start(HPM_DAO);
#endif
            i2s_dma_start_transfer((uint32_t)i2s_buff[s_i2s_buff_front], i2s_buff_fill_size[s_i2s_buff_front]);
            s_i2s_buff_front++;
            if (s_i2s_buff_front >= CODEC_BUFF_CNT) {
                s_i2s_buff_front = 0u;
            }
            s_i2s_buff_first_tranferred = true;
        }

        if (i2s_buff_fill_size[rear] < CODEC_BUFF_SIZE) {
            s_playing_finished = true;
        }
    }
}

void caculate_music_time(uint32_t *data)
{
    hpm_stat_t res;
    uint8_t cnt;

    cnt = sd_get_search_file_cnt();
    for (uint8_t i = 0u; i < cnt; i++) {
        res = hpm_wav_decode_init(sd_get_search_file_buff_ptr(i), &wav_ctrl, &wav_header_buff);
        if (res == status_success) {
            data[i] = wav_ctrl.sec_total;
            wav_ctrl.func.close_file(wav_ctrl.func.file);
        }
    }
}

void set_s_playing_finished(bool set)
{
    s_playing_finished = set;
}

bool is_audio_playing_finished(void)
{
    return s_playing_finished;
}

void set_switch_songs_req(bool set)
{
    s_switch_songs_req = set;
}

bool has_switch_songs_req(void)
{
    return s_switch_songs_req;
}

void set_init_audio_player_req(uint32_t id)
{
    s_init_audio_player_req = true;
    s_track_id = id;
}

/*****************************************************************************************************************
 *
 *  Static Codes
 *
 *****************************************************************************************************************/
static void init_audio_player_no_printf(char *fname)
{
    hpm_stat_t res;

    dma_abort_channel(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH);
    if (wav_ctrl.func.file != 0) {
        wav_ctrl.func.close_file(wav_ctrl.func.file);
    }
    s_i2s_buff_rear = 0;
    s_i2s_buff_front = 0;
    res = hpm_wav_decode_init(fname, &wav_ctrl, &wav_header_buff);
    if (res == status_success) {
        f_lseek(&wav_file, wav_ctrl.data_pos);
        if (status_success != init_i2s_playback(wav_ctrl.wav_head.fmt_chunk.samplerate,
                                wav_ctrl.wav_head.fmt_chunk.bitspersample, wav_ctrl.wav_head.fmt_chunk.channels)) {
            while (1) {
                ;
            }
        }
        s_i2s_buff_first_tranferred = false;
        s_playing_finished = false;
    } else {
        while (1) {
            ;
        }
    }
}

static hpm_stat_t init_i2s_playback(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    if (channel_num > 2) {
        return status_invalid_argument; /* Currently not support TDM mode */
    }

    i2s_get_default_config(TARGET_I2S, &i2s_config);
#ifdef USING_CODEC
    i2s_config.enable_mclk_out = true;
#endif
    i2s_init(TARGET_I2S, &i2s_config);

    i2s_enable_tx_dma_request(TARGET_I2S);
    dmamux_config(BOARD_APP_DMAMUX, TARGET_I2S_TX_DMAMUX_CH, TARGET_I2S_TX_DMAMUX_SRC, 1);
    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, TARGET_I2S_TX_DMA_IRQ_PRIORITY);

    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.data_line = TARGET_I2S_DATA_LINE;
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.channel_num_per_frame = 2; /* non TDM mode, channel num fix to 2. */
    transfer.channel_slot_mask = 0x3; /* data from hpm_wav_decode API is 2 channels */
#if CONFIG_CODEC_WM8960
    transfer.protocol = I2S_PROTOCOL_I2S_PHILIPS;
#endif

    i2s_mclk_hz = board_config_i2s_clock(TARGET_I2S, sample_rate);

    if (status_success != i2s_config_tx(TARGET_I2S, i2s_mclk_hz, &transfer)) {
        return status_fail;
    }
    i2s_start(TARGET_I2S);

#ifdef USING_CODEC
    #if CONFIG_CODEC_WM8960
        wm8960_config.route = wm8960_route_playback;
        wm8960_config.format.sample_rate = sample_rate;
        wm8960_config.format.bit_width = audio_depth;
        wm8960_config.format.mclk_hz = i2s_mclk_hz;
        if (wm8960_init(&wm8960_control, &wm8960_config) != status_success) {
            printf("Init Audio Codec failed\n");
        }
        wm8960_set_volume(&wm8960_control, wm8960_module_dac, 200);
    #elif CONFIG_CODEC_SGTL5000
        sgtl5000_config.route = sgtl_route_playback;
        sgtl5000_config.format.sample_rate = sample_rate;
        sgtl5000_config.format.bit_width = audio_depth;
        sgtl5000_config.format.mclk_hz = i2s_mclk_hz;
        if (sgtl_init(&sgtl5000_context, &sgtl5000_config) != status_success) {
            printf("Init Audio Codec failed\n");
        }
        sgtl_set_volume(&sgtl5000_context, sgtl_module_dac, SGTL5000_DAC_MIN_VOLUME_VALUE);
    #endif
#elif USING_DAO
    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.enable_mono_output = true;
    dao_init(HPM_DAO, &dao_config);
#endif

    return status_success;
}

static void i2s_dma_start_transfer(uint32_t addr, uint32_t size)
{
    dma_channel_config_t ch_config = { 0 };

    dma_default_channel_config(BOARD_APP_HDMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, addr);
    ch_config.dst_addr = (uint32_t)&TARGET_I2S->TXD[TARGET_I2S_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = size;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = 0;

    if (status_success != dma_setup_channel(BOARD_APP_HDMA, TARGET_I2S_TX_DMA_CH, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
}

static bool is_i2s_buff_full(void)
{
    bool full = false;
    uint8_t rear = s_i2s_buff_rear + 1u;

    if (rear >= CODEC_BUFF_CNT) {
        rear = 0u;
    }

    if (rear == s_i2s_buff_front) {
        full = true;
    }

    return full;
}

static bool is_i2s_buff_empty(void)
{
    bool empty = false;

    if (s_i2s_buff_front == s_i2s_buff_rear) {
        empty = true;
    }

    return empty;
}
