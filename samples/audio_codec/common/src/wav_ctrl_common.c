/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ff.h"
#include "wav_ctrl_common.h"

static FIL wav_file;
static hpm_wav_ctrl wav_ctrl;

/*****************************************************************************************************************
 *
 *  Codes
 *
 *****************************************************************************************************************/
hpm_stat_t hpm_audiocodec_search_file(char *file_name, HPM_AUDIOCODEC_FILE *fil)
{
    FRESULT rsl = f_open(&wav_file, file_name, FA_READ);
    if (rsl == FR_OK) {
        *fil = (uint32_t)&wav_file;
        return status_success;
    } else {
        return status_audio_codec_none_file;
    }
}

hpm_stat_t hpm_audiocodec_read_file(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br)
{
    FRESULT rsl = f_read((FIL *)fil, data, num_bytes, (UINT *)br);
    if (rsl == FR_OK) {
        return status_success;
    } else {
        return status_fail;
    }
}

hpm_stat_t hpm_audiocodec_write_file(HPM_AUDIOCODEC_FILE fil, uint32_t num_bytes, uint8_t *data, uint32_t *br)
{
    FRESULT rsl = f_write((FIL *)fil, data, num_bytes, (UINT *)br);
    if (rsl == FR_OK) {
        return status_success;
    } else {
        return status_fail;
    }
}

hpm_stat_t hpm_audiocodec_close_file(HPM_AUDIOCODEC_FILE fil)
{
    FRESULT rsl = f_close((FIL *)fil);
    if (rsl == FR_OK) {
        return status_success;
    } else {
        return status_fail;
    }
}

hpm_wav_ctrl *init_wav_ctrl_func(void)
{
    wav_ctrl.func.close_file = hpm_audiocodec_close_file;
    wav_ctrl.func.read_file = hpm_audiocodec_read_file;
    wav_ctrl.func.search_file = hpm_audiocodec_search_file;
    wav_ctrl.func.write_file = hpm_audiocodec_write_file;

    return &wav_ctrl;
}
