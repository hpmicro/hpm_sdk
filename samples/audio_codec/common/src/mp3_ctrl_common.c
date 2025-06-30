/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ff.h"

#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint8_t s_mp3_file_buf[0x800000];
static FIL s_mp3_fs;
/*****************************************************************************************************************
 *
 *  Codes
 *
 *****************************************************************************************************************/

uint8_t *mp3_preload(char *file_name, uint32_t *data_size)
{
    FRESULT rsl;
    uint32_t read_bytes;

    rsl = f_open(&s_mp3_fs, file_name, FA_READ);
    if (rsl != FR_OK) {
        printf("error: open file failure!\n");
        return 0;
    }

    *data_size = f_size(&s_mp3_fs);
    if (*data_size > 0x800000) {
        printf("error: not enough memory!\n");
        return 0;
    }

    rsl = f_read(&s_mp3_fs, s_mp3_file_buf, *data_size, (UINT *)&read_bytes);
    if ((rsl != FR_OK) || (*data_size != read_bytes)) {
        printf("error: read file failure!\n");
        return 0;
    }

    f_close(&s_mp3_fs);

    return s_mp3_file_buf;
}

uint32_t mp3_convert_data_format(int32_t *dst, int16_t *src, int32_t samples, uint8_t channels)
{
    int32_t data_cnt = 0;

    if (channels == 2) {
        for (int32_t n = 0; n < (samples * channels); n++) {
            dst[n] = (int32_t)src[n] << 16;
        }
        data_cnt = samples * 2;
    } else if (channels == 1) {
        for (int32_t n = 0; n < samples; n++) {
            dst[2 * n] = (int32_t)src[n] << 16;
            dst[2 * n + 1] = dst[2 * n];
        }
        data_cnt = samples * 2;
    } else {
        ;
    }

    return data_cnt * sizeof(int32_t);
}

int32_t mp3_calc_total_time_second(char *file_name)
{
    uint8_t *buf;
    uint32_t buf_size;
    int samples;
    uint64_t detected_samples = 0;
    int to_skip = 0;
    mp3dec_frame_info_t frame_info = { 0 };

    buf = mp3_preload(file_name, &buf_size);

    if (buf == NULL) {
        return -1;
    }

    mp3dec_skip_id3((const uint8_t **)&buf, (size_t *)&buf_size);
    if (!buf_size) {
        return -2;
    }

    /* try to make allocation size assumption by first frame or vbr tag */
    do {
        uint32_t frames;
        int i, delay, padding, free_format_bytes = 0, frame_size = 0;
        const uint8_t *hdr;

        i = mp3d_find_frame(buf, buf_size, &free_format_bytes, &frame_size);
        buf += i;
        buf_size -= i;
        hdr = buf;

        if (i && !frame_size) {
            continue;
        }
        if (!frame_size) {
            return -3;
        }
        frame_info.channels = HDR_IS_MONO(hdr) ? 1 : 2;
        frame_info.hz = hdr_sample_rate_hz(hdr);
        frame_info.layer = 4 - HDR_GET_LAYER(hdr);
        frame_info.bitrate_kbps = hdr_bitrate_kbps(hdr);
        frame_info.frame_bytes = frame_size;
        samples = hdr_frame_samples(hdr) * frame_info.channels;
        if (3 != frame_info.layer) {
            break;
        }
        int ret = mp3dec_check_vbrtag(hdr, frame_size, &frames, &delay, &padding);
        if (ret > 0) {
            padding *= frame_info.channels;
            to_skip = delay * frame_info.channels;
            detected_samples = samples * (uint64_t)frames;
            if (detected_samples >= (uint64_t)to_skip) {
                detected_samples -= to_skip;
            }
            if (padding > 0 && detected_samples >= (uint64_t)padding) {
                detected_samples -= padding;
            }
            if (!detected_samples) {
                return -4;
            }
        }
        if (ret) {
            buf += frame_size;
            buf_size -= frame_size;
        }
        break;
    } while (1);

    size_t total_samples = MINIMP3_MAX_SAMPLES_PER_FRAME;
    size_t total_sec;
    if (detected_samples) {
        total_samples += detected_samples;
    } else {
        total_samples += (buf_size / frame_info.frame_bytes) * samples;
    }
    total_sec = total_samples / (frame_info.hz * frame_info.channels);

    return total_sec;
}
