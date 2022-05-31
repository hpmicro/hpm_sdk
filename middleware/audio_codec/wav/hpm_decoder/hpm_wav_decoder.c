/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "string.h"
#include "hpm_wav_codec.h"

hpm_stat_t hpm_wav_decode_init(char *fname, hpm_wav_ctrl *wav_ctrl)
{
    uint8_t buf[512];
    hpm_stat_t res = status_audio_codec_true;
    uint32_t br = 0;
    res = wav_ctrl->func.search_file(fname, &wav_ctrl->func.file);
    if (res == status_audio_codec_true) {
        wav_ctrl->func.read_file(wav_ctrl->func.file, 512, buf, &br);
        memcpy((uint8_t *)&wav_ctrl->wav_head, buf, 44);
        if (wav_ctrl->wav_head.riff_chunk.riff_type==0X45564157) { /** wav */
            if(wav_ctrl->wav_head.data_chunk.id == 0X61746164) { /** data */
                wav_ctrl->data_pos = 28 + wav_ctrl->wav_head.fmt_chunk.size;
                wav_ctrl->sec_total = wav_ctrl->wav_head.data_chunk.size / wav_ctrl->wav_head.fmt_chunk.byterate;
            }else {
                res = status_audio_codec_non_standard;
            }
        } else {
            res = status_audio_codec_format_err;
        }
    } else {
        res = status_audio_codec_none_file;
    }
    return res;
}

uint32_t hpm_wav_decode(hpm_wav_ctrl *wav_ctrl, uint8_t *buf, uint8_t * buf_transit, uint32_t size)
{
    uint16_t readlen=0;
    uint32_t read;
    uint32_t n;
    uint8_t *p;
    if (wav_ctrl->wav_head.fmt_chunk.bitspersample == 24) {
        readlen=(size/4)*3;
        wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, buf_transit, &read);
        p = buf_transit;
        for(n = 0; n < size;) {
            buf[n++] = 0;
            buf[n++] = p[0];
            buf[n++] = p[1];
            buf[n++] = p[2];
            p += 3;
        }
        read = (read * 4) / 3;
    } else {
        wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, buf_transit, &read);
        if(read < size) {
            for(n = read; n < size - read; n++) {
                buf[n] = 0;
            }
        }
    }
    if(read < size) {
        wav_ctrl->func.close_file(wav_ctrl->func.file);
    }
    return read;
}
