#ifndef _H264_STREAM_h_
#define _H264_STREAM_h_
#include "board.h"
#include "hpm_gptmr_drv.h"
#include "hpm_mchtmr_drv.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/url.h"
#include "mp4.h"
#include "sd_fatfs_common.h"
#include <stdio.h>


int h264_decode_init(void);
int h264_decode_release(void);
int FFmpeg_H264Decode(unsigned char *outRGBBuf, unsigned int **outYUVBuf);
int FFmpeg_VideoDecoderInit(AVCodecContext *ctx);
void core1_decode_loop(void);


#endif