/*
 * Copyright (c) 2022 hpmicro
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

#endif  /* JPEG_TURBO_H */