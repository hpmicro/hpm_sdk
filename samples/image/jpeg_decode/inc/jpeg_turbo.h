/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef JPEG_TURBO_H
#define JPEG_TURBO_H
/*---------------------------------------------------------------------*
 * Define function
 *---------------------------------------------------------------------
 */
void jpeg_convert_data(uint8_t *filebuffs, int32_t fileLen, int32_t *width, int32_t *height, uint8_t *rgb565buff);
bool jpeg_sw_decode(uint8_t *file_buf, uint32_t file_len, uint32_t *width, uint32_t *height, uint8_t *components, uint16_t *out_buf);
#endif  /* JPEG_TURBO_H */