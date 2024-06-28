/*
 * Copyright (c) 2023 HPMicro
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
#include "hpm_ecat_e2p_emulation.h"

#define ESC_MEM_ADDR                  UINT8
#define ESC_MEM_SHIFT                 0

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


#define ECAT_TIMER_INC_P_MS    1
extern MEM_ADDR ESCMEM * pEsc;         /* pointer to the ESC */


#define     HW_GetALEventRegister()                     ((((volatile UINT16 ESCMEM *) pEsc)[((ESC_AL_EVENT_OFFSET) >> 1)]))
#define     HW_GetALEventRegister_Isr                   HW_GetALEventRegister

#define     HW_EscRead(pData, Address, Len)             ESCMEMCPY((MEM_ADDR *) (pData),                             \
                                                                &((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >>  ESC_MEM_SHIFT)], (Len))
#define     HW_EscReadIsr                               HW_EscRead
#define     HW_EscReadDWord(DWordValue, Address)        ((DWordValue) = (((volatile UINT32 *) pEsc)[(Address >> 2)]))
#define     HW_EscReadDWordIsr(DWordValue, Address)     HW_EscReadDWord(DWordValue, Address)
/* the esc pointer is handled as a word pointer so the address must be divided by 2 */
#define     HW_EscReadWord(WordValue, Address)          ((WordValue) = (((volatile UINT16 *) pEsc)[((Address) >> 1)]))
/* the esc pointer is handled as a word pointer so the address must be divided by 2 */
#define     HW_EscReadWordIsr(WordValue, Address)       HW_EscReadWord(WordValue, Address)
#define     HW_EscReadByte(ByteValue, Address)          ((ByteValue) = (((volatile UINT8 *) pEsc)[(Address)]))
#define     HW_EscReadByteIsr(ByteValue, Address)       HW_EscReadByte(ByteValue, Address)

/* ECATCHANGE_START(V5.01) HW2*/
#define     HW_EscReadMbxMem(pData, Address, Len)       ESCMBXMEMCPY((MEM_ADDR *) (pData),                             \
                                                                    &((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >> ESC_MEM_SHIFT)], (Len))

#define     HW_EscWrite(pData, Address, Len)            ESCMEMCPY(&((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >> ESC_MEM_SHIFT)], \
                                                                  (MEM_ADDR *) (pData), (Len))
#define     HW_EscWriteIsr                              HW_EscWrite
#define     HW_EscWriteDWord(DWordValue, Address)       ((((volatile UINT32 *) pEsc)[(Address >> 2)]) = (DWordValue))
#define     HW_EscWriteDWordIsr(DWordValue, Address)    HW_EscWriteDWord(DWordValue, Address)
/* the esc pointer is handled as a word pointer so the address must be divided by 2 */
#define     HW_EscWriteWord(WordValue, Address)         ((((volatile UINT16 *) pEsc)[((Address) >> 1)]) = (WordValue))
/* the esc pointer is handled as a word pointer so the address must be divided by 2 */
#define     HW_EscWriteWordIsr(WordValue, Address)      HW_EscWriteWord(WordValue, Address)
#define     HW_EscWriteByte(ByteValue, Address)         ((((volatile UINT8 *) pEsc)[(Address)]) = (ByteValue))
#define     HW_EscWriteByteIsr(ByteValue, Address)      HW_EscWriteByte(ByteValue, Address)
#define     HW_EscWriteMbxMem(pData, Address, Len)      ESCMBXMEMCPY(&((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >> ESC_MEM_SHIFT)], \
                                                                    (MEM_ADDR *) (pData), (Len))

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

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
UINT16 HW_EepromReload(void);
UINT16 ecat_eeprom_emulation_read(UINT32 wordaddr);
UINT16 ecat_eeprom_emulation_write(UINT32 wordaddr);
UINT16 ecat_eeprom_emulation_reload(void);
void ecat_eeprom_emulation_store(void);

void ecat_eeprom_emulation_reload_response(void);
#endif /* ESC_EEPROM_EMULATION */

#ifdef __cplusplus
}
#endif

#endif /* HPM_ECAT_HW_H */
