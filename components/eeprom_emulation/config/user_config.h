/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define E2P_DEBUG_LEVEL        (1)
#define E2P_CRITICAL_ENTER()   do { disable_global_irq(CSR_MSTATUS_MIE_MASK); } while(0)
#define E2P_CRITICAL_EXIT()    do { enable_global_irq(CSR_MSTATUS_MIE_MASK); } while(0)

#define EEPROM_MAX_VAR_CNT     (100)

#ifdef __cplusplus
}
#endif

#endif