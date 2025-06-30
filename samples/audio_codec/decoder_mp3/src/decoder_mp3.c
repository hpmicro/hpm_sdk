/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_dao_drv.h"
#include "hpm_i2s_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "sd_fatfs_common.h"
#include "audio_codec_common.h"
#include "mp3_ctrl_common.h"

#include "minimp3.h"

#define RING_BUFF_CNT 3

static mp3dec_t s_mp3d;
static mp3dec_frame_info_t s_mp3dec_info;
static int16_t s_out_pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
static ATTR_PLACE_AT_NONCACHEABLE int32_t s_i2s_buff[RING_BUFF_CNT][MINIMP3_MAX_SAMPLES_PER_FRAME];
static volatile int32_t s_i2s_data_cnt[RING_BUFF_CNT];
static volatile uint8_t s_i2s_buff_rear;
static volatile uint8_t s_i2s_buff_front;

/* Function Decalartion */
static bool is_i2s_buff_full(void);
static bool is_i2s_buff_empty(void);

/* Function Definition */
SDK_DECLARE_EXT_ISR_M(BOARD_APP_XDMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile hpm_stat_t stat;

    stat = dma_check_transfer_status(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);

    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        if (!is_i2s_buff_empty()) {
            i2s_dma_start_transfer((uint32_t)s_i2s_buff[s_i2s_buff_front], s_i2s_data_cnt[s_i2s_buff_front]);
            s_i2s_buff_front++;
            if (s_i2s_buff_front >= RING_BUFF_CNT) {
                s_i2s_buff_front = 0u;
            }
        } else {
            printf("I2S buffer is empty!\n");
        }
    }
}

int main(void)
{
    char file_name[256];
    uint32_t file_size;
    uint8_t *file_data_ptr;
    FRESULT rsl;
    int32_t frame_bytes;
    int32_t samples;
    int32_t total_time;

    board_init();
#if defined(USING_CODEC) && USING_CODEC
    init_codec();
#elif defined(USING_DAO) && USING_DAO
    init_dao();
#endif

    printf("audio codec mp3 example\n");

    rsl = sd_mount_fs();
    if (rsl == FR_OK) {
        mp3dec_init(&s_mp3d);
        while (1) {
            rsl = sd_choose_music(".mp3", file_name);
            if (rsl == FR_OK) {
                total_time = mp3_calc_total_time_second(file_name);
                if (total_time < 0) {
                    printf("calcate total time failure.\r\n");
                    while (1) {
                        ;
                    }
                }
                file_data_ptr = mp3_preload(file_name, &file_size);

                if (file_data_ptr != 0) {
                    frame_bytes = 0;
                    samples = mp3dec_decode_frame(&s_mp3d, file_data_ptr, file_size, s_out_pcm, &s_mp3dec_info);
                    if (samples > 0) {
                        printf("mp3 name: %s.\r\n", file_name);
                        printf("mp3 samplerate: %d Hz\n", s_mp3dec_info.hz);
                        printf("mp3 bitrate: %d kbps\n", s_mp3dec_info.bitrate_kbps);
                        printf("mp3 channels: %d\n", s_mp3dec_info.channels);
                        printf("music playing time: %d seconds ...\n", total_time);
                        s_i2s_data_cnt[s_i2s_buff_rear] = mp3_convert_data_format(s_i2s_buff[s_i2s_buff_rear], s_out_pcm, samples, s_mp3dec_info.channels);
                        s_i2s_buff_rear++;
                        if (s_i2s_buff_rear >= RING_BUFF_CNT) {
                            s_i2s_buff_rear = 0;
                        }

                        init_i2s_playback(s_mp3dec_info.hz, i2s_audio_depth_16_bits, s_mp3dec_info.channels);
                        i2s_dma_start_transfer((uint32_t)s_i2s_buff[s_i2s_buff_front], s_i2s_data_cnt[s_i2s_buff_front]);
                        s_i2s_buff_front++;
                        if (s_i2s_buff_front >= RING_BUFF_CNT) {
                            s_i2s_buff_front = 0u;
                        }
                        dao_start(HPM_DAO);
                        i2s_start(TARGET_I2S);
                    }

                    while (1) {
                        if (!is_i2s_buff_full()) {
                            frame_bytes += s_mp3dec_info.frame_bytes;
                            samples = mp3dec_decode_frame(&s_mp3d, file_data_ptr + frame_bytes, file_size - frame_bytes, s_out_pcm, &s_mp3dec_info);
                            if (samples > 0) {
                                s_i2s_data_cnt[s_i2s_buff_rear] = mp3_convert_data_format(s_i2s_buff[s_i2s_buff_rear], s_out_pcm, samples, s_mp3dec_info.channels);
                                s_i2s_buff_rear++;
                                if (s_i2s_buff_rear >= RING_BUFF_CNT) {
                                    s_i2s_buff_rear = 0;
                                }
                            } else {
                                break;    /* finish */
                            }
                        }
                    }

                    /* wait transfer complete */
                    while (!is_i2s_buff_empty()) {
                        ;
                    }

                    i2s_stop(TARGET_I2S);
                    #if defined(USING_DAO) && USING_DAO
                    dao_stop(HPM_DAO);
                    #endif
                }
            }
        }
    }

    return 0;
}

static bool is_i2s_buff_full(void)
{
    bool full = false;
    uint8_t rear = s_i2s_buff_rear + 1u;

    if (rear >= RING_BUFF_CNT) {
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
