/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _MP3_CTRL_COMMON_H
#define _MP3_CTRL_COMMON_H


/*
 * Function Declaration
 */

uint8_t *mp3_preload(char *file_name, uint32_t *data_size);
uint32_t mp3_convert_data_format(int32_t *dst, int16_t *src, int32_t samples, uint8_t channels);
int32_t mp3_calc_total_time_second(char *file_name);

#endif
