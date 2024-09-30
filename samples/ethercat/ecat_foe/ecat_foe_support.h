/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* this file for eeprom emulation component */

#ifndef FOE_SUPPORT_H
#define FOE_SUPPORT_H

#include "board.h"
#include "ecat_def.h"
#include "ecat_config.h"

#define FOE_FILE_MAX_SIZE   FLASH_FILE_SIZE
#define FOE_FILE_ADDR       (FLASH_ADDR_BASE + FLASH_FILE_ADDR)
#define FOE_FILE_TOTAL_SIZE (FLASH_ADDR_BASE + FLASH_FILE_ADDR + FLASH_FILE_SIZE + 4) /* last 4 byte store file size */

#ifdef __cplusplus
extern "C" {
#endif

int8_t foe_support_init(void);

void foe_support_soc_reset(void);


#ifdef __cplusplus
}
#endif

#endif /* FOE_SUPPORT_H */
