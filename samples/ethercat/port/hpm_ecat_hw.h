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

#define ESC_EEPROM_EMULATION_SUCCESS        (0)
#define ESC_EEPROM_EMULATION_ACK_ERROR      (1)
#define ESC_EEPROM_EMULATION_CHECKSUM_ERROR (2)

#define ESC_EEPROM_PRODUCT_CODE_WORD_INDEX (0xA)
#define ESC_EEPROM_REVISION_NUM_WORD_INDEX (0xC)


#define ECAT_TIMER_INC_P_MS    1
extern MEM_ADDR ESCMEM * pEsc;         /* pointer to the ESC */


#define     HW_GetALEventRegister()                     ((((volatile UINT16 ESCMEM *) pEsc)[((ESC_AL_EVENT_OFFSET) >> 1)]))
#define     HW_GetALEventRegister_Isr                   HW_GetALEventRegister

#define     HW_EscReadDWord(DWordValue, Address)        ((DWordValue) = (((volatile UINT32 *) pEsc)[(Address >> 2)]))
#define     HW_EscReadDWordIsr(DWordValue, Address)     HW_EscReadDWord(DWordValue, Address)
#define     HW_EscReadWord(WordValue, Address)          ((WordValue) = (((volatile UINT16 *) pEsc)[((Address) >> 1)]))
#define     HW_EscReadWordIsr(WordValue, Address)       HW_EscReadWord(WordValue, Address)
#define     HW_EscReadByte(ByteValue, Address)          ((ByteValue) = (((volatile UINT8 *) pEsc)[(Address)]))
#define     HW_EscReadByteIsr(ByteValue, Address)       HW_EscReadByte(ByteValue, Address)

#if defined(HPM_IP_FEATURE_ESC_BYTE_READ) && HPM_IP_FEATURE_ESC_BYTE_READ
/* Read operation supports byte by byte */
#define     HW_EscRead(pData, Address, Len)             ESCMEMCPY((UINT8 *) (pData), &((UINT8 ESCMEM *) pEsc)[(Address)], (Len))
#define     HW_EscReadIsr                               HW_EscRead
#define     HW_EscReadMbxMem(pData, Address, Len)       ESCMEMCPY((UINT8 *) (pData), &((UINT8 ESCMEM *) pEsc)[(Address)], (Len))
#else
/* Read operation requires 4-byte alignment and reads 4 bytes at a time */
#define     HW_EscRead(pData, Address, Len)             hw_esc_read_4bytes((UINT32 *) (pData), &((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (Len))
#define     HW_EscReadIsr                               HW_EscRead
#define     HW_EscReadMbxMem(pData, Address, Len)       hw_esc_read_4bytes((UINT32 *) (pData), &((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (Len))
#endif

/* Write Function */
#define     HW_EscWrite(pData, Address, Len)            ESCMEMCPY(&((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (UINT32 *) (pData), (Len))
#define     HW_EscWriteIsr                              HW_EscWrite
#define     HW_EscWriteDWord(DWordValue, Address)       ((((volatile UINT32 *) pEsc)[(Address >> 2)]) = (DWordValue))
#define     HW_EscWriteDWordIsr(DWordValue, Address)    HW_EscWriteDWord(DWordValue, Address)
#define     HW_EscWriteMbxMem(pData, Address, Len)      ESCMBXMEMCPY(&((UINT32 ESCMEM *) pEsc)[((Address) >> 2)], (UINT32 *) (pData), (Len))
#define     HW_EscWriteWord(WordValue, Address)         ((((volatile UINT16 *) pEsc)[((Address) >> 1)]) = (WordValue))
#define     HW_EscWriteWordIsr(WordValue, Address)      HW_EscWriteWord(WordValue, Address)
#define     HW_EscWriteByte(ByteValue, Address)         ((((volatile UINT8 *) pEsc)[(Address)]) = (ByteValue))
#define     HW_EscWriteByteIsr(ByteValue, Address)      HW_EscWriteByte(ByteValue, Address)

#ifdef __cplusplus
extern "C" {
#endif

UINT32 HW_GetTimer(void);
void HW_ClearTimer(void);
UINT16 HW_Init(void);
void HW_Release(void);
void DISABLE_ESC_INT(void);
void ENABLE_ESC_INT(void);
#if defined(UC_SET_ECAT_LED) && UC_SET_ECAT_LED
void HW_SetLed(UINT8 RunLed, UINT8 ErrLed);
#endif

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
