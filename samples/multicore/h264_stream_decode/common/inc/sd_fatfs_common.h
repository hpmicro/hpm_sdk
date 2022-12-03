/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SD_FATFS_H
#define SD_FATFS_H

#include "ff.h"

FRESULT sd_mount_fs(void);
FRESULT sd_choose_music(char *target_filetype, char *filename);
FRESULT sd_search_music_name(char *target_filetype);
uint8_t sd_get_search_file_cnt(void);
char *sd_get_search_file_buff_ptr(uint8_t index);

#endif /* SD_FATFS_H */