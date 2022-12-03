/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "convert.h"
#include "disp.h"
#include "h264_stream.h"
#include "heap_5.h"
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
#include "uart.h"
#include <stdio.h>

int main(void)
{
    board_init();
    board_init_lcd();
    init_lcd();
    init_heap5_mem();
    mchtmr_init_counter(HPM_MCHTMR, 0);
    uart_dma_rx_init();

    printf("using sw yuv transform 420p\r\n");

    int ret = h264_decoder_init();
    if (ret) {
        printf("error while h264_decoder_init %d\r\n", ret);
        while (1)
            ;
    }
    while (1) {
        stream_decode();
    }
    return 0;
}