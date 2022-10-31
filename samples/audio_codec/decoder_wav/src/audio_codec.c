/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "audio_codec_common.h"
#include "sd_fatfs_common.h"

int main(void)
{
    char file_name[256];
    FRESULT rsl;

    board_init();
#if USING_CODEC_SGTL5000
    init_codec();
#elif USING_DAO
    init_dao();
#endif

    printf("audio codec example\n");
    rsl = sd_mount_fs();
    if (rsl == FR_OK) {
        init_wav_ctrl_func();
        set_s_playing_finished(true);
        while (1) {
            if (is_audio_playing_finished()) {
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
