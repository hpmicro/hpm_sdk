/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef FILE_OP_H
#define FILE_OP_H

#include <stdbool.h>
#include "ff.h"

#define FILE_BUFFER_LEN (1024 * 1024)
#define DECODE_BUFFER_LEN (1920 * 1280 * 2)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define FILENAMENUM 10
#define FILENAMELENGTH 255

bool file_store(char *fname, uint8_t *buffer, uint32_t size);
bool file_restore(char *fname, uint8_t *buf, uint32_t size);
uint32_t file_get_size(char *fname);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif  /* FILE_OP_H */
