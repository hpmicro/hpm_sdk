/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "string.h"
#include "hpm_wav_codec.h"

hpm_stat_t hpm_wav_decode_init(char *fname, hpm_wav_ctrl *wav_ctrl, uint8_t (*pbuf)[512])
{
    hpm_stat_t res = status_success;
    uint32_t br = 0;
    uint16_t pos;
    res = wav_ctrl->func.search_file(fname, &wav_ctrl->func.file);
    if (res == status_success) {
        wav_ctrl->func.read_file(wav_ctrl->func.file, 512, (uint8_t *)pbuf, &br);
        for (pos = 0; pos < 512 - 4; pos++) {
            switch ((*pbuf)[pos]) {
            case 'R':
                if (((*pbuf)[pos + 1] == 'I') && ((*pbuf)[pos + 2] == 'F') && ((*pbuf)[pos + 3] == 'F')) {
                    memcpy((uint8_t *)&wav_ctrl->wav_head.riff_chunk, (uint8_t *)&((*pbuf)[pos]), sizeof(wav_riff));
                }
            break;
            case 'f':
                if (((*pbuf)[pos + 1] == 'm') && ((*pbuf)[pos + 2] == 't')) {
                    memcpy((uint8_t *)&wav_ctrl->wav_head.fmt_chunk, (uint8_t *)&((*pbuf)[pos]), sizeof(wav_formatchunk));
                }
            break;
            case 'd':
                if (((*pbuf)[pos + 1] == 'a') && ((*pbuf)[pos + 2] == 't') && ((*pbuf)[pos + 3] == 'a')) {
                    memcpy((uint8_t *)&wav_ctrl->wav_head.data_chunk, (uint8_t *)&((*pbuf)[pos]), sizeof(wav_data));
                }
            break;
            default:
            break;
            }
        }
        if (wav_ctrl->wav_head.riff_chunk.riff_type==0X45564157) { /** wav */
            if(wav_ctrl->wav_head.data_chunk.id == 0X61746164) { /** data */
                wav_ctrl->data_pos = 28 + wav_ctrl->wav_head.fmt_chunk.size;
                wav_ctrl->remaining_data = wav_ctrl->wav_head.data_chunk.size;
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

uint32_t hpm_wav_decode(hpm_wav_ctrl *wav_ctrl, uint8_t *buf, uint32_t size)
{
    uint16_t readlen=0;
    uint32_t read;
    uint32_t n;
    uint32_t *pbuf;
    uint8_t *p8;
    uint32_t word_size;
    uint32_t buf_len;

    pbuf = (uint32_t *)buf;
    if (wav_ctrl->wav_head.fmt_chunk.bitspersample == 32) {
        if (wav_ctrl->wav_head.fmt_chunk.channels == 2) {
            word_size = size >> 2;
            readlen = word_size << 2;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]);
                p8 += 4;
            }
            buf_len = read;
        } else if (wav_ctrl->wav_head.fmt_chunk.channels == 1) {
            word_size = size >> 2;
            readlen = word_size << 1;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n] = (*(uint32_t *)&p8[0]);
                pbuf[n + 1] = pbuf[n];
                n += 2;
                p8 += 4;
            }
            buf_len = read << 1;
        }
    } else if (wav_ctrl->wav_head.fmt_chunk.bitspersample == 24) {
        if (wav_ctrl->wav_head.fmt_chunk.channels == 2) {
            word_size = size >> 2;
            readlen = word_size * 3;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]) << 8;
                p8 += 3;
            }
            buf_len = (read << 2) / 3;
        } else if (wav_ctrl->wav_head.fmt_chunk.channels == 1) {
            word_size = size >> 2;
            readlen = (word_size * 3) >> 3;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]) << 8;
                p8 += 3;
            }
            buf_len = (read << 3) / 3;
        }
    } else if (wav_ctrl->wav_head.fmt_chunk.bitspersample == 16) {
        if (wav_ctrl->wav_head.fmt_chunk.channels == 2) {
            word_size = size >> 2;
            readlen = size >> 1;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]) << 16;
                p8 += 2;
            }
            buf_len = read << 1;
        } else if (wav_ctrl->wav_head.fmt_chunk.channels == 1) {
            word_size = size >> 2;
            readlen = word_size;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]) << 16;
                pbuf[n] = pbuf[n-1];
                n += 1;
                p8 += 2;
            }
            buf_len = read << 2;
        }
    } else if (wav_ctrl->wav_head.fmt_chunk.bitspersample == 8) {
        if (wav_ctrl->wav_head.fmt_chunk.channels == 2) {
            word_size = size >> 2;
            readlen = word_size;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]) << 24;
                p8 += 1;
            }
            buf_len = read << 2;
        } else if (wav_ctrl->wav_head.fmt_chunk.channels == 1) {
            word_size = size >> 2;
            readlen = word_size >> 1;
            if (wav_ctrl->remaining_data >= readlen) {
                wav_ctrl->remaining_data -= readlen;
            } else {
                readlen = wav_ctrl->remaining_data;
                wav_ctrl->remaining_data = 0;
            }
            wav_ctrl->func.read_file(wav_ctrl->func.file, readlen, &buf[size - readlen], &read);
            p8 = &buf[size - readlen];
            for (n = 0; n < word_size;) {
                pbuf[n++] = (*(uint32_t *)&p8[0]) << 24;
                pbuf[n + 1] = pbuf[n];
                n += 2;
                p8 += 1;
            }
            buf_len = read << 3;
        }
    }
    if (buf_len < size) {
        for (n = buf_len; n < size; n++) {
            buf[n] = 0;
        }
        buf_len = size - 8;
        wav_ctrl->func.close_file(wav_ctrl->func.file);
    }
    return buf_len;
}
