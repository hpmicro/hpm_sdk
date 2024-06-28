/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef JPEG_HW_H
#define JPEG_HW_H

#include "hpm_common.h"

#ifndef TEST_JPEG
#define TEST_JPEG HPM_JPEG
#endif

/* image information extracted from jpeg file */
typedef struct jpeg_image_info {
    uint32_t ecs_start;
    uint32_t ecs_length;
    uint16_t width;
    uint16_t height;
    uint8_t components;
    uint8_t sampling_format;
    uint8_t qt_count;
    uint32_t huffmin[16];
    uint16_t huffbase[64];
    uint16_t qdtable[256];
    uint16_t huffenc[384];
    uint8_t huffsymb[336];
} jpeg_image_info_t;

#ifdef __cplusplus
extern "C" {
#endif

bool jpeg_hw_decode(uint8_t *in_buf, int32_t len, uint8_t *out_buf, jpeg_image_info_t *info);

#ifdef __cplusplus
}
#endif
#endif  /* JPEG_HW_H */
