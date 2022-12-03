
/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SEC_CORE_IMG_H
#define SEC_CORE_IMG_H
#include <stdint.h>


#define SEC_CORE_IMG_START 0x1090000


extern const uint32_t sec_core_img_size;
extern const uint8_t sec_core_img[];

#endif /* SEC_CORE_IMG_H */