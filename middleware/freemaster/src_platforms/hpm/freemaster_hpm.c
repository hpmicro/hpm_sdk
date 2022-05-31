/*******************************************************************************
*
* Copyright 2004-2014 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale FreeMASTER License
* distributed with this Material.
* See the LICENSE file distributed for more details.
* 
****************************************************************************/
/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "freemaster.h"
#include "freemaster_private.h"
#include "freemaster_hpm.h"

#if !(FMSTR_DISABLE)
#if defined(FMSTR_ISR_CALLBACK)
  extern void FMSTR_ISR_CALLBACK(void);
#endif 

/*******************************************************************************
*
* @brief    API: UART irq handler
*
* This Interrupt Service Routine handles the UART interrupts for the FreeMASTER 
* driver.*
*
* In poll-driven mode (FMSTR_POLL_DRIVEN) this function does nothing.
*
*******************************************************************************/

void FMSTR_Isr(void)
{
#if (FMSTR_LONG_INTR) || (FMSTR_SHORT_INTR)

    /* process serial interface */
#if (FMSTR_USE_UART)
    FMSTR_ProcessSCI();
#endif /* FMSTR_USE_UART */

    /* process application callback */
#if defined(FMSTR_ISR_CALLBACK)
    if((FMSTR_ISR_CALLBACK) != NULL)
        FMSTR_ISR_CALLBACK();
#endif

#endif
}

/**************************************************************************//*!
*
* @brief    The "memcpy" used internally in FreeMASTER driver
*
* @param    nDestAddr - destination memory address
* @param    nSrcAddr  - source memory address
* @param    nSize     - memory size (always in bytes)
*
******************************************************************************/

void FMSTR_CopyMemory(FMSTR_ADDR nDestAddr, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) nSrcAddr;
    FMSTR_U8* pd = (FMSTR_U8*) nDestAddr;

    while(nSize--) 
    {
        *pd++ = *ps++;
    }
}

/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

FMSTR_BPTR FMSTR_CopyToBuffer(FMSTR_BPTR pDestBuff, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) nSrcAddr;
    FMSTR_U8* pd = (FMSTR_U8*) pDestBuff;

    while(nSize--)
    {
        *pd++ = *ps++;
    }

    return (FMSTR_BPTR) pd;
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

FMSTR_BPTR FMSTR_CopyFromBuffer(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) pSrcBuff;
    FMSTR_U8* pd = (FMSTR_U8*) nDestAddr;

    while(nSize--)
    {
        *pd++ = *ps++;
    }

    return (FMSTR_BPTR) ps;
}

#if (FMSTR_BYTE_BUFFER_ACCESS)
FMSTR_BPTR FMSTR_ValueFromBuffer16(FMSTR_U16* pDest, FMSTR_BPTR pSrc)
{
    return FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pDest, pSrc, 2U);
}

FMSTR_BPTR FMSTR_ValueFromBuffer32(FMSTR_U32* pDest, FMSTR_BPTR pSrc)
{
    return FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pDest, pSrc, 4U);
}

FMSTR_BPTR FMSTR_ValueToBuffer16(FMSTR_BPTR pDest, FMSTR_U16 src)
{
    return FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)(FMSTR_U8*)&src, 2U);
}

FMSTR_BPTR FMSTR_ValueToBuffer32(FMSTR_BPTR pDest, FMSTR_U32 src)
{
    return FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)(FMSTR_U8*)&src, 4U);
}
#endif

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

void FMSTR_CopyFromBufferWithMask(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) pSrcBuff;
    FMSTR_U8* pd = (FMSTR_U8*) nDestAddr;
    FMSTR_U8* pm = ps + nSize;
    FMSTR_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;

        /* perform AND-masking */
        stmp = (FMSTR_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
    }
}

/******************************************************************************/

/* mixed EX and no-EX commands? */
#if (FMSTR_USE_EX_CMDS) && (FMSTR_USE_NOEX_CMDS) || (FMSTR_BYTE_BUFFER_ACCESS)

/**************************************************************************//*!
*
* @brief  When mixed EX and no-EX command may occur, this variable is
*         here to remember what command is just being handled.
*
******************************************************************************/

static FMSTR_BOOL pcm_bNextAddrIsEx;

void FMSTR_SetExAddr(FMSTR_BOOL bNextAddrIsEx)
{
    pcm_bNextAddrIsEx = bNextAddrIsEx;
}

/**************************************************************************//*!
*
* @brief  Store address to communication buffer. The address may be
*         32 or 16 bit wide (based on previous call to FMSTR_SetExAddr)
*
******************************************************************************/

FMSTR_BPTR FMSTR_AddressToBuffer(FMSTR_BPTR pDest, FMSTR_ADDR nAddr)
{
    if(pcm_bNextAddrIsEx)
    {
        /* fill in the 32bit address */
#if FMSTR_BYTE_BUFFER_ACCESS
        pDest = FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)&nAddr, 4U);
#else
        *(FMSTR_U32*) pDest = ((FMSTR_U32)nAddr);
        pDest += 4;
#endif
    }
    else
    {
        /* fill in the 16bit address (never used) */
#if FMSTR_BYTE_BUFFER_ACCESS
        pDest = FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)&nAddr, 2U);
#else
        *(FMSTR_U16*) pDest = ((FMSTR_U16)nAddr);
        pDest += 2;
#endif
    }

    return pDest;
}

/**************************************************************************//*!
*
* @brief  Fetch address from communication buffer
*
******************************************************************************/

FMSTR_BPTR FMSTR_AddressFromBuffer(FMSTR_ADDR* pAddr, FMSTR_BPTR pSrc)
{
    if(pcm_bNextAddrIsEx)
    {
#if FMSTR_BYTE_BUFFER_ACCESS
        pSrc = FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pAddr, pSrc, 4U);
#else
        *pAddr = (FMSTR_ADDR) *((FMSTR_U32*) pSrc);
        pSrc += 4;
#endif
    }
    else
    {
#if FMSTR_BYTE_BUFFER_ACCESS
        pSrc = FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pAddr, pSrc, 2U);
#else
        *pAddr = (FMSTR_ADDR) *((FMSTR_U16*) pSrc);
        pSrc += 2;
#endif
    }

    return pSrc;
}

#endif /* mixed EX and no-EX commands */

#endif /* !(FMSTR_DISABLE) */
