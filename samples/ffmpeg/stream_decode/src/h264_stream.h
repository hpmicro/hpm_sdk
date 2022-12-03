#ifndef _H264_STREAM_h_
#define _H264_STREAM_h_
#include "board.h"
#include "convert.h"
#include "disp.h"
#include "hpm_display_common.h"
#include "hpm_gptmr_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_pdma_drv.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/url.h"
#include "mp4.h"
#include "osd.h"
#include "sd_fatfs_common.h"
#include <stdio.h>

void stream_decode(void);
int h264_decoder_init(void);
int h264_decoder_release(void);


#endif