/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_lvgl.h"
#include "lv_demo_music.h"
#include "hpm_gptmr_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_i2s_drv.h"
#if defined(USING_DAO) && USING_DAO
#include "hpm_dao_drv.h"
#endif
#include "wav_ctrl_common.h"
#include "mp3_ctrl_common.h"
#include "audio_codec_common.h"
#include "sd_fatfs_common.h"

#include "minimp3.h"

#define CODEC_BUFF_CNT 3
#define CODEC_BUFF_SIZE HPM_L1C_CACHELINE_ALIGN_UP(1024 * 20)

#define WAV_FILE_TYPE 1
#define MP3_FILE_TYPE 2

static hpm_wav_ctrl *wav_ctrl_ptr;
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint8_t wav_header_buff[0x400];
static mp3dec_t s_mp3d;
static mp3dec_frame_info_t s_mp3dec_info;
static uint32_t s_file_size;
static uint8_t *s_file_data_ptr;
static int32_t s_frame_bytes;
static int32_t s_samples;
static int16_t s_out_pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

static uint8_t s_file_type;

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
 *  Static Function Decalarion
 *
 *****************************************************************************************************************/
static void lv_audio_codec_task(void);
static void init_audio_player(char *fname);
static void hpm_playback_wav_mp3(void);
static bool is_i2s_buff_full(void);
static bool is_i2s_buff_empty(void);

/*****************************************************************************************************************
 *
 *  Extern Function Decalarion
 *
 *****************************************************************************************************************/
extern bool is_music_playing(void);
extern char *_lv_demo_music_get_title(uint32_t track_id);
extern void _lv_demo_music_album_next(bool next);

/*****************************************************************************************************************
 *
 *  Function Definition
 *
 *****************************************************************************************************************/
static void init_gptmr1(uint32_t ms)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(clock_gptmr1, 0);
    gptmr_channel_get_default_config(HPM_GPTMR1, &config);
    gptmr_freq = clock_get_frequency(clock_gptmr1);
    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(HPM_GPTMR1, 0, &config, false);
    gptmr_enable_irq(HPM_GPTMR1, GPTMR_CH_RLD_IRQ_MASK(0));
    intc_m_enable_irq_with_priority(IRQn_GPTMR1, 2);

    gptmr_start_counter(HPM_GPTMR1, 0);
}

SDK_DECLARE_EXT_ISR_M(IRQn_GPTMR1, lv_audio_codec_task_isr)
void lv_audio_codec_task_isr(void)
{
    if (gptmr_check_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(0))) {
        gptmr_clear_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(0));
        lv_audio_codec_task();
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile hpm_stat_t stat;

    stat = dma_check_transfer_status(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);

    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        if (!is_i2s_buff_empty()) {
            i2s_dma_start_transfer((uint32_t)i2s_buff[s_i2s_buff_front], i2s_buff_fill_size[s_i2s_buff_front]);
            i2s_buff_fill_size[s_i2s_buff_front] = 0;
            s_i2s_buff_front++;
            if (s_i2s_buff_front >= CODEC_BUFF_CNT) {
                s_i2s_buff_front = 0u;
            }
        }
    }
}

int main(void)
{
    FRESULT ff_result;

    board_init();
    board_init_cap_touch();
    board_init_lcd();

#if defined(USING_CODEC) && USING_CODEC
    init_codec();
#elif defined(USING_DAO) && USING_DAO
    init_dao();
#endif

    hpm_lvgl_init();

    printf("lvgl music\n");
    printf("display freq: %d\n", clock_get_frequency(clock_display));

    mp3dec_init(&s_mp3d);

    wav_ctrl_ptr = init_wav_ctrl_func();
    ff_result = sd_mount_fs();

    if (ff_result != FR_OK) {
        lv_disp_no_sd_card_info();
    } else {
        ff_result = lv_generate_music_info_table();
        if (ff_result == FR_OK) {
            lv_demo_music();
        }
    }

    init_gptmr1(20U);

    while (1) {
        lv_timer_periodic_handler();
        if (s_switch_songs_req) {
            _lv_demo_music_album_next(true);
            s_switch_songs_req = false;
        }
    }

    return 0;
}

void set_init_audio_player_req(uint32_t id)
{
    s_init_audio_player_req = true;
    s_track_id = id;
}

hpm_stat_t calculate_music_time(char *file_name, uint32_t *tm_value)
{
    hpm_stat_t res = status_fail;
    int32_t total_time;

    if (strstr(file_name, ".wav") != NULL) {
        res = hpm_wav_decode_init(file_name, wav_ctrl_ptr, &wav_header_buff);
        if (res == status_success) {
            *tm_value = wav_ctrl_ptr->sec_total;
            wav_ctrl_ptr->func.close_file(wav_ctrl_ptr->func.file);
        }
    } else if (strstr(file_name, ".mp3") != NULL) {
        total_time = mp3_calc_total_time_second(file_name);
        if (total_time > 0) {
            *tm_value = (uint32_t)total_time;
            res = status_success;
        }
    } else {
        ;
    }

    return res;
}

static void lv_audio_codec_task(void)
{
    static uint8_t s_ctrl_state;

    if (s_init_audio_player_req) {
        init_audio_player((char *)_lv_demo_music_get_title(s_track_id));
        s_init_audio_player_req = false;
    }

    if (is_music_playing()) {
        hpm_playback_wav_mp3();
    }

    switch (s_ctrl_state) {
    case 1:
        if (!is_music_playing()) {
            dma_disable_channel(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);
            if ((I2S_STA_TX_DN_GET(i2s_get_irq_status(TARGET_I2S)) & (0x01 << TARGET_I2S_TX_DATA_LINE)) != 0u) {
#if defined(USING_DAO) && USING_DAO
                dao_stop(HPM_DAO);
#endif
                s_ctrl_state = 0;
            }
        }
        break;

    case 0:
    default:
        if (is_music_playing()) {
#if defined(USING_DAO) && USING_DAO
            dao_start(HPM_DAO);
#endif
            dma_enable_channel(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);
            s_ctrl_state = 1;
        }
        break;
    }

    if (s_playing_finished && is_i2s_buff_empty() && ((I2S_STA_TX_DN_GET(i2s_get_irq_status(TARGET_I2S)) & (0x01 << TARGET_I2S_TX_DATA_LINE)) != 0u)) {
        s_switch_songs_req = true;
    }
}

static void init_audio_player(char *fname)
{
    hpm_stat_t res;

    dma_abort_channel(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);
    dma_disable_channel(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);
    dma_clear_transfer_status(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);
    i2s_stop(TARGET_I2S);
#if defined(USING_DAO) && USING_DAO
    dao_stop(HPM_DAO);
#endif

    if (wav_ctrl_ptr->func.file != 0) {
        wav_ctrl_ptr->func.close_file(wav_ctrl_ptr->func.file);
    }
    s_i2s_buff_rear = 0;
    s_i2s_buff_front = 0;

    if (strstr(fname, ".wav") != NULL) {
        res = hpm_wav_decode_init(fname, wav_ctrl_ptr, &wav_header_buff);
        if (res == status_success) {
            s_file_type = WAV_FILE_TYPE;
            f_lseek((FIL *)(&wav_ctrl_ptr->func.file), wav_ctrl_ptr->data_pos);
            if (status_success != init_i2s_playback(wav_ctrl_ptr->wav_head.fmt_chunk.samplerate,
                                    wav_ctrl_ptr->wav_head.fmt_chunk.bitspersample, wav_ctrl_ptr->wav_head.fmt_chunk.channels)) {
                printf("config i2s transfer failed.\n");
            }
            s_i2s_buff_first_tranferred = false;
            s_playing_finished = false;
        } else {
            printf("music file error.\r\n");
        }
    } else if (strstr(fname, ".mp3") != NULL) {
        s_file_data_ptr = mp3_preload(fname, &s_file_size);
        if (s_file_data_ptr != NULL) {
            s_frame_bytes = 0;
            s_samples = mp3dec_decode_frame(&s_mp3d, s_file_data_ptr, s_file_size, s_out_pcm, &s_mp3dec_info);
            if (s_samples > 0) {
                s_file_type = MP3_FILE_TYPE;
                i2s_buff_fill_size[s_i2s_buff_rear] = mp3_convert_data_format((int32_t *)&i2s_buff[s_i2s_buff_rear][0], s_out_pcm, s_samples, s_mp3dec_info.channels);
                if (l1c_dc_is_enabled()) {
                    l1c_dc_writeback((uint32_t)i2s_buff[s_i2s_buff_rear], HPM_L1C_CACHELINE_ALIGN_UP(i2s_buff_fill_size[s_i2s_buff_rear]));
                }
                s_i2s_buff_rear++;
                if (s_i2s_buff_rear >= CODEC_BUFF_CNT) {
                    s_i2s_buff_rear = 0u;
                }

                if (status_success != init_i2s_playback(s_mp3dec_info.hz, i2s_audio_depth_16_bits, s_mp3dec_info.channels)) {
                    printf("config i2s transfer failed.\n");
                }
                s_i2s_buff_first_tranferred = false;
                s_playing_finished = false;
            } else {
                printf("is not a mp3 file.\r\n");
            }
        } else {
            printf("load music file failure.\r\n");
        }
    } else {
        ;
    }
}

static void hpm_playback_wav_mp3(void)
{
    uint8_t rear;

    if (!is_i2s_buff_full() && !s_playing_finished) {
        rear = s_i2s_buff_rear;
        s_i2s_buff_rear++;
        if (s_i2s_buff_rear >= CODEC_BUFF_CNT) {
            s_i2s_buff_rear = 0u;
        }
        if (s_file_type == WAV_FILE_TYPE) {
            i2s_buff_fill_size[rear] = hpm_wav_decode(wav_ctrl_ptr, i2s_buff[rear], CODEC_BUFF_SIZE);
            if (l1c_dc_is_enabled()) {
                l1c_dc_writeback((uint32_t)i2s_buff[rear], HPM_L1C_CACHELINE_ALIGN_UP(i2s_buff_fill_size[rear]));
            }
            if (i2s_buff_fill_size[rear] < CODEC_BUFF_SIZE) {
                s_playing_finished = true;
            }
        } else if (s_file_type == MP3_FILE_TYPE) {
            s_frame_bytes += s_mp3dec_info.frame_bytes;
            s_samples = mp3dec_decode_frame(&s_mp3d, s_file_data_ptr + s_frame_bytes, s_file_size - s_frame_bytes, s_out_pcm, &s_mp3dec_info);
            if (s_samples > 0) {
                i2s_buff_fill_size[rear] = mp3_convert_data_format((int32_t *)&i2s_buff[rear][0], s_out_pcm, s_samples, s_mp3dec_info.channels);
                if (l1c_dc_is_enabled()) {
                    l1c_dc_writeback((uint32_t)i2s_buff[rear], HPM_L1C_CACHELINE_ALIGN_UP(i2s_buff_fill_size[rear]));
                }
            } else {
                s_playing_finished = true;    /* finish */
            }
        } else {
            ;
        }

        if (!s_i2s_buff_first_tranferred) {
            i2s_dma_start_transfer((uint32_t)i2s_buff[s_i2s_buff_front], i2s_buff_fill_size[s_i2s_buff_front]);
#if defined(USING_DAO) && USING_DAO
            dao_start(HPM_DAO);
#endif
            i2s_start(TARGET_I2S);
            s_i2s_buff_front++;
            if (s_i2s_buff_front >= CODEC_BUFF_CNT) {
                s_i2s_buff_front = 0u;
            }
            s_i2s_buff_first_tranferred = true;
        }
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
