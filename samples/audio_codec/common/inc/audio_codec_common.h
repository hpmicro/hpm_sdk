/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_AUDIO_H
#define _HPM_AUDIO_H

#include "ff.h"

/*
 * Function Declaration
 */
void init_codec(void);
void init_dao(void);
void init_wav_ctrl_func(void);
void lv_audio_codec_task(void);
void init_audio_player(char *fname);
void hpm_playback_wav(void);
void caculate_music_time(uint32_t *data);
void set_s_playing_finished(bool set);
bool is_audio_playing_finished(void);
void set_switch_songs_req(bool set);
bool has_switch_songs_req(void);
void set_init_audio_player_req(uint32_t id);

#endif
