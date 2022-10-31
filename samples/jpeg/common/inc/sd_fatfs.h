/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SD_FATFS_H
#define SD_FATFS_H

void init_disk(void);
bool disk_is_mounted(void);
bool check_disk(void);

#endif  /* SD_FATFS_H */