/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _FORMAT_CONVERT_H
#define _FORMAT_CONVERT_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void convert_rgb565_to_rgb888(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst);
void convert_rgb565_to_bgr888(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst);
void convert_rgb888_to_rgb565(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst);
void convert_bgr888_to_rgb565(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst);
void convert_abgr8888_to_argb8888(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif  /* _FORMAT_CONVERT_H */
