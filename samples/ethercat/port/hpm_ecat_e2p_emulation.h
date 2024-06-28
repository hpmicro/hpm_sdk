/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ECAT_EEPROM_EMULATION_H
#define HPM_ECAT_EEPROM_EMULATION_H

#include "ecat_def.h"
#include "user_config.h"
#include "eeprom_emulation.h"
#include "ecat_config.h"


#ifdef __cplusplus
extern "C" {
#endif


hpm_stat_t ecat_flash_eeprom_init(void);
hpm_stat_t ecat_flash_eeprom_write(uint32_t addr, uint8_t *data);
hpm_stat_t ecat_flash_eeprom_read(uint32_t addr, uint8_t *data);
void ecat_flash_eeprom_format(void);
void ecat_flash_eeprom_flush(void);
void ecat_flash_eeprom_show_info(void);

#ifdef __cplusplus
}
#endif


#endif /* HPM_ECAT_EEPROM_EMULATION_H */
