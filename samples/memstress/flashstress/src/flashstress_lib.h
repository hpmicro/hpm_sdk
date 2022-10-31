/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef FLASHSTRESS_LIB_H
#define FLASHSTRESS_LIB_H

#include <stdint.h>

struct flashstress_driver {
    uint32_t (*get_flash_chip_size_bytes)(void);
    uint32_t (*get_flash_block_size_bytes)(void);
    uint8_t (*get_flash_erase_value)(void);
    int (*erase_chip)(void);
    int (*erase_block)(uint32_t offset);
    int (*read)(uint32_t offset, void *buf, uint32_t size_bytes);
    int (*write)(uint32_t offset, const void *buf, uint32_t size_bytes);
};

struct flashstress_context;

struct flashstress_context *flashstress_create(struct flashstress_driver *drv, const char *name);
void flashstress_destroy(struct flashstress_context *ctx);
int flashstress_run(struct flashstress_context *ctx);

#endif
