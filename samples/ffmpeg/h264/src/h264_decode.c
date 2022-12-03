/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "convert.h"
#include "hpm_display_common.h"
#include "hpm_gptmr_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_pdma_drv.h"
#include "measure.h"
#include "mp4.h"
#include "sd_fatfs_common.h"
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/url.h"

#define DISPLAY
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 100
#define AVCODEC_MAX_VIDEO_FRAME_SIZE 4096

extern void init_heap5_mem(void);
extern void init_lcd(void);
extern uint16_t framebuffer[2][BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT];
static uint64_t time_start = 0, time_end;
static FIL inputFil;
extern AVCodec ff_h264_decoder;
extern AVInputFormat ff_mov_demuxer;
extern URLProtocol ff_file_protocol;
extern video_lcd_cfg_t g_videoLcdCfg;

ff_measure_context_t s_ffMeasureContext;
ff_measure_sum_t s_ffMeasureSum;

AVPacket packet;
AVFrame *frame;
AVFrame *frameyuv;
AVFormatContext *pInFmtCtx;

static void h264_video_decode(const char *infilename)
{
    printf("Decode file '%s'\n", infilename);
    static float sum_decode_time = 0.0f;
    static uint16_t f_cnt = 0;
    /* Check input/output media file */
    FRESULT c;
    c = f_open(&inputFil, infilename, FA_READ | FA_OPEN_EXISTING);
    if (c != FR_OK) {
        printf("Could not open '%s', erro = %d\n", infilename, c);
        return;
    }

    /* Register several FFmpeg own var */
    av_register_input_format(&ff_mov_demuxer);
    ffurl_register_protocol(&ff_file_protocol);
    avcodec_register(&ff_h264_decoder);
    /* Init user format conext var Open input media file (mp4) from SD Card */
    int error = avformat_open_input(&pInFmtCtx, infilename, NULL, NULL);
    if (error != 0) {
        printf("Couldn't open file: error :%d\n", error);
        return;
    }
    /* Get steam info from input media file */
    error = avformat_find_stream_info(pInFmtCtx, NULL);
    if (error < 0) {
        printf("Couldn't find stream information error :%d\n", error);
        return;
    }
    int videoStream = -1;
    for (unsigned int j = 0; j < pInFmtCtx->nb_streams; j++) {
        /* Find video steam index */
        if (pInFmtCtx->streams[j]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = j;
        }
    }
    printf("video stream num: %d\n", videoStream);

    AVCodecContext *pInCodecCtx_video = pInFmtCtx->streams[videoStream]->codec;
    AVCodec *pInCodec_video = avcodec_find_decoder(AV_CODEC_ID_H264);
    int tmp = avcodec_open2(pInCodecCtx_video, pInCodec_video, NULL);
    if (tmp < 0) {
        printf("error avcodec_open failed video %d.\n", tmp);
        return;
    }

    ff_time_measure_utility(kFfTimeType_Start);

    while (av_read_frame(pInFmtCtx, &packet) >= 0) {
        ff_time_measure_utility(kFfTimeType_ReadFrame);
        if (packet.stream_index == videoStream) {
            uint8_t *pktdatayuv = packet.data;
            int pktsizeyuv = packet.size;
            int out_sizeyuv;
            int lenyuv;
            while (pktsizeyuv > 0) {
                out_sizeyuv = AVCODEC_MAX_VIDEO_FRAME_SIZE * 100;
                ff_time_measure_utility(kFfTimeType_Start);
                lenyuv = avcodec_decode_video2(pInCodecCtx_video, frameyuv, &out_sizeyuv, &packet);
                ff_time_measure_utility(kFfTimeType_DecodeVideo);
                float time_tmp = 1.0f * s_ffMeasureContext.decodeVideo_ns / 1E6;
                sum_decode_time += time_tmp;
                if (lenyuv < 0) {
                    printf("Error while decoding video.\n");
                    break;
                }
                if (out_sizeyuv > 0) {
                    f_cnt += out_sizeyuv;
                    printf("%d ", f_cnt);
#ifdef DISPLAY
                    ff_time_measure_utility(kFfTimeType_TransStart);
                    AVFrame2Img(frameyuv, &framebuffer[1]);
                    ff_time_measure_utility(kFfTimeType_TransEnd);

                    uint32_t status;
                    pdma_scale(BOARD_PDMA_BASE,
                        (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&framebuffer[0]),
                        800,
                        (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&framebuffer[1]),
                        frameyuv->width,
                        0, 0, frameyuv->width, frameyuv->height,
                        800, 480,
                        0xFF, display_pixel_format_rgb565, true, &status);
                    ff_time_measure_utility(kFfTimeType_Pdma);
                    float time_tmp1 = 1.0f * (s_ffMeasureContext.decodeVideo_TransEnd - s_ffMeasureContext.decodeVideo_TransStart) / 1E6;
                    float time_tmp2 = 1.0f * (s_ffMeasureContext.decodeVideo_Pdma - s_ffMeasureContext.decodeVideo_TransEnd) / 1E6;

                    printf("decode use %f ms trans %f pdma %f\r\n", time_tmp, time_tmp1, time_tmp2);
#else
                    printf("decode use %f ms \r\n", time_tmp);
#endif
                }
                pktsizeyuv -= lenyuv;
                pktdatayuv += lenyuv;
            }
        }
        av_packet_unref(&packet);

        ff_time_measure_utility(kFfTimeType_Start);
    }

    time_end = mchtmr_get_count(HPM_MCHTMR);
    printf("average fps is %f\n", f_cnt / (1.0f * (time_end - time_start) / 1E6 / 24));
    printf("----------------------------------\n");
    printf("avg decode time is %f\n", sum_decode_time / f_cnt);

  #ifdef DISPLAY
    printf("avg trans %f\n", 1.0f * s_ffMeasureSum.decodeVideo_TransEnd / f_cnt / 1E6);
    printf("avg pdma %f\n", 1.0f * s_ffMeasureSum.decodeVideo_Pdma / f_cnt / 1E6);
  #endif

    f_close(&inputFil);
    avcodec_close(pInCodecCtx_video);
    avformat_close_input(&pInFmtCtx);
    printf("Decode Done\n");
}


int main(void)
{
    FRESULT ff_result;
    init_heap5_mem();
    board_init();
    board_init_lcd();
    init_lcd();
    mchtmr_init_counter(HPM_MCHTMR, 0);

    ff_result = sd_mount_fs();
    while (FR_OK != ff_result) {

    }

    char *filepath_in = "1.mp4";

    frame = av_frame_alloc();
    frameyuv = av_frame_alloc();
    pInFmtCtx = avformat_alloc_context();

    time_start = mchtmr_get_count(HPM_MCHTMR);
    h264_video_decode(filepath_in);

    return 0;
}