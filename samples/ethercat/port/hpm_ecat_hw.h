/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ECAT_HW_H
#define HPM_ECAT_HW_H

#include <string.h>
#include "hpm_common.h"
#include "ecatappl.h"
#include "hpm_esc_drv.h"
#include "ecat_config.h"
#include "hpm_ecat_phy.h"
#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
#include "hpm_ecat_e2p_emulation.h"
#endif

#define ESC_REG_RUN_LED_OVERRIDE      (0x0138)
#define ESC_REG_ERROR_LED_OVERRIDE    (0x0139)
#define ESC_LED_OVERRIDE_OFF          (0x00)
#define ESC_LED_OFF                   (0x10)
#define ESC_LED_BLINKING              (0x1D)
#define ESC_LED_SINGLE_FLASH          (0x11)
#define ESC_LED_DOUBLE_FLASH          (0x12)
#define ESC_LED_FLICKERING            (0x1E)
#define ESC_LED_ON                    (0x1F)

#define ESC_EEPROM_EMULATION_SUCCESS        (0)
#define ESC_EEPROM_EMULATION_ACK_ERROR      (1)
#define ESC_EEPROM_EMULATION_CHECKSUM_ERROR (2)

#define ESC_EEPROM_PRODUCT_CODE_WORD_INDEX (0xA)
#define ESC_EEPROM_REVISION_NUM_WORD_INDEX (0xC)


#define ECAT_TIMER_INC_P_MS    1
extern MEM_ADDR ESCMEM * pEsc;         /* pointer to the ESC */


#define     HW_GetALEventRegister()                     ((((volatile UINT16 ESCMEM *) pEsc)[((ESC_AL_EVENT_OFFSET) >> 1)]))
#define     HW_GetALEventRegister_Isr                   HW_GetALEventRegister

#if defined(HPM_IP_FEATURE_ESC_BYTE_READ) && HPM_IP_FEATURE_ESC_BYTE_READ
/* Read operation requires 4-byte alignment and reads 4 bytes at a time */
#define     HW_EscRead(pData, Address, Len)             ESCMEMCPY((UINT32 *) (pData), &((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (Len))
#define     HW_EscReadIsr                               HW_EscRead
#define     HW_EscReadDWord(DWordValue, Address)        ((DWordValue) = (((volatile UINT32 *) pEsc)[(Address >> 2)]))
#define     HW_EscReadDWordIsr(DWordValue, Address)     HW_EscReadDWord(DWordValue, Address)
#define     HW_EscReadMbxMem(pData, Address, Len)       ESCMEMCPY((UINT32 *) (pData), &((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (Len))
#else
/* Read operation requires 4-byte alignment and reads 4 bytes at a time */
#define     HW_EscRead(pData, Address, Len)             hw_esc_read_4bytes((UINT32 *) (pData), &((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (Len))
#define     HW_EscReadIsr                               HW_EscRead
#define     HW_EscReadDWord(DWordValue, Address)        ((DWordValue) = (((volatile UINT32 *) pEsc)[(Address >> 2)]))
#define     HW_EscReadDWordIsr(DWordValue, Address)     HW_EscReadDWord(DWordValue, Address)
#define     HW_EscReadMbxMem(pData, Address, Len)       hw_esc_read_4bytes((UINT32 *) (pData), &((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (Len))
#endif

/* Write Function */
#define     HW_EscWrite(pData, Address, Len)            ESCMEMCPY(&((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (UINT32 *) (pData), (Len))
#define     HW_EscWriteIsr                              HW_EscWrite
#define     HW_EscWriteDWord(DWordValue, Address)       ((((volatile UINT32 *) pEsc)[(Address >> 2)]) = (DWordValue))
#define     HW_EscWriteDWordIsr(DWordValue, Address)    HW_EscWriteDWord(DWordValue, Address)
#define     HW_EscWriteMbxMem(pData, Address, Len)      ESCMBXMEMCPY(&((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (UINT32 *) (pData), (Len))

#ifdef __cplusplus
extern "C" {
#endif

UINT32 HW_GetTimer(void);
void HW_ClearTimer(void);
UINT16 HW_Init(void);
void HW_Release(void);
void DISABLE_ESC_INT(void);
void ENABLE_ESC_INT(void);
void HW_SetLed(BOOL RunLed, BOOL ErrLed);

hpm_stat_t ecat_hardware_init(ESC_Type *esc_ptr);

void hw_esc_read_4bytes(void *dest, const void *src, uint16_t size);

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
UINT16 HW_EepromReload(void);
UINT16 ecat_eeprom_emulation_read(UINT32 wordaddr);
UINT16 ecat_eeprom_emulation_write(UINT32 wordaddr);
UINT16 ecat_eeprom_emulation_reload(void);
void ecat_eeprom_emulation_store(void);

void ecat_eeprom_emulation_reload_response(void);
#endif /* ESC_EEPROM_EMULATION */

/* calculate ECAT EEPROM  Configuration Area checksum value */
uint8_t ecat_calculate_eeprom_config_data_checksum(const uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* HPM_ECAT_HW_H */
