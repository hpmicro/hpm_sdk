/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_AUDIO_CODEC_H
#define HPM_AUDIO_CODEC_H
#include "stdint.h"
#include "hpm_common.h"
/**
 * @brief audio codec return type
 *
 */
enum hpm_audio_codec_status {
    status_audio_codec_format_err = MAKE_STATUS(status_group_audio_codec, 2),
    status_audio_codec_non_standard = MAKE_STATUS(status_group_audio_codec, 3),
    status_audio_codec_none_file = MAKE_STATUS(status_group_audio_codec, 4),
    status_audio_codec_none_dir = MAKE_STATUS(status_group_audio_codec, 5),
    status_audio_codec_end = MAKE_STATUS(status_group_audio_codec, 6),
};

typedef uint32_t HPM_AUDIOCODEC_FILE;

typedef struct {
    HPM_AUDIOCODEC_FILE  file;
    hpm_stat_t (*search_file)(char *file_name, HPM_AUDIOCODEC_FILE *fil);
    hpm_stat_t (*read_file)(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br);
    hpm_stat_t (*write_file)(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br);
    hpm_stat_t (*lseek_file)(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data);
    hpm_stat_t (*close_file)(HPM_AUDIOCODEC_FILE fil);
} hpm_audiocodec_callback;

/**
 * @brief file search callback
 *
 * @param[in] file_name file name string
 * @param[out] fil return the file handle
 * @return hpm_stat_t @ref hpm_audio_codec_status
 */
hpm_stat_t hpm_audiocodec_search_file(char *file_name, HPM_AUDIOCODEC_FILE *fil);

/**
 * @brief read file callback
 *
 * @param[in] fil file handle
 * @param[in] num_bytes number of bytes
 * @param[out] data read data
 * @param[out] br Actual number of bytes read
 * @return hpm_stat_t @ref hpm_audio_codec_status
 */
hpm_stat_t hpm_audiocodec_read_file(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br);

/**
 * @brief write file callback
 *
 * @param[in] fil file handle
 * @param[in] num_bytes number of bytes
 * @param[in] data read data
 * @param[out] br Actual number of bytes read
 * @return hpm_stat_t hpm_stat_t @ref hpm_audio_codec_status
 */
hpm_stat_t hpm_audiocodec_write_file(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br);

/**
 * @brief close file callback
 *
 * @param fil file handle
 * @return hpm_stat_t hpm_stat_t @ref hpm_audio_codec_status
 */
hpm_stat_t hpm_audiocodec_close_file(HPM_AUDIOCODEC_FILE fil);


#endif
