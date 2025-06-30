/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_l1c_drv.h"
#include "wav_ctrl_common.h"
#include "audio_codec_common.h"
#include "sd_fatfs_common.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_i2s_drv.h"
#if defined(USING_DAO) && USING_DAO
#include "hpm_dao_drv.h"
#endif

#define CODEC_BUFF_CNT 3
#define CODEC_BUFF_SIZE 20480

static hpm_wav_ctrl *wav_ctrl_ptr;
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint8_t wav_header_buff[0x400];
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint8_t i2s_buff[CODEC_BUFF_CNT][CODEC_BUFF_SIZE];
static volatile uint32_t i2s_buff_fill_size[CODEC_BUFF_CNT];
static volatile uint8_t s_i2s_buff_front;
static volatile uint8_t s_i2s_buff_rear;
static bool s_i2s_buff_first_tranferred;
static bool s_playing_finished;

/* Function Declaration */
static void init_audio_player(char *fname);
static void hpm_playback_wav(void);
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
            i2s_dma_start_transfer((uint32_t)i2s_buff[s_i2s_buff_front], i2s_buff_fill_size[s_i2s_buff_front]);
            s_i2s_buff_front++;
            if (s_i2s_buff_front >= CODEC_BUFF_CNT) {
                s_i2s_buff_front = 0u;
            }
        }
    }
}

int main(void)
{
    char file_name[256];
    FRESULT rsl;

    board_init();
#if defined(USING_CODEC) && USING_CODEC
    init_codec();
#elif defined(USING_DAO) && USING_DAO
    init_dao();
#endif

    printf("audio codec example\n");
    rsl = sd_mount_fs();
    if (rsl == FR_OK) {
        wav_ctrl_ptr = init_wav_ctrl_func();
        s_playing_finished = true;
        while (1) {
            if (s_playing_finished) {
                i2s_stop(TARGET_I2S);
                #if defined(USING_DAO) && USING_DAO
                dao_stop(HPM_DAO);
                #endif
                rsl = sd_choose_music(".wav", file_name);
                if (rsl == FR_OK) {
                    init_audio_player(file_name);
                }
            }
            hpm_playback_wav();
        }
    }

    return 0;
}

static void init_audio_player(char *fname)
{
    hpm_stat_t res;

    dma_abort_channel(BOARD_APP_XDMA, TARGET_I2S_TX_DMA_CH);
    if (wav_ctrl_ptr->func.file != 0) {
        wav_ctrl_ptr->func.close_file(wav_ctrl_ptr->func.file);
    }
    s_i2s_buff_rear = 0;
    s_i2s_buff_front = 0;
    res = hpm_wav_decode_init(fname, wav_ctrl_ptr, &wav_header_buff);
    if (res == status_success) {
        printf("wav_name:%s.\r\n", fname);
        printf("wav samperate: %d\n", wav_ctrl_ptr->wav_head.fmt_chunk.samplerate);
        printf("wav sampebits: %d\n", wav_ctrl_ptr->wav_head.fmt_chunk.bitspersample);
        printf("wav channels: %d\n", wav_ctrl_ptr->wav_head.fmt_chunk.channels);
        printf("music playing time:%d seconds ...\n", wav_ctrl_ptr->sec_total);
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
}

static void hpm_playback_wav(void)
{
    uint8_t rear;

    if (!is_i2s_buff_full() && !s_playing_finished) {
        rear = s_i2s_buff_rear;
        s_i2s_buff_rear++;
        if (s_i2s_buff_rear >= CODEC_BUFF_CNT) {
            s_i2s_buff_rear = 0u;
        }
        i2s_buff_fill_size[rear] = hpm_wav_decode(wav_ctrl_ptr, i2s_buff[rear], CODEC_BUFF_SIZE);
        if (l1c_dc_is_enabled()) {
            l1c_dc_writeback((uint32_t)i2s_buff[rear], HPM_L1C_CACHELINE_ALIGN_UP(i2s_buff_fill_size[rear]));
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

        if (i2s_buff_fill_size[rear] < CODEC_BUFF_SIZE) {
            s_playing_finished = true;
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
