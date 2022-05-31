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

#ifndef __FREEMASTER_HPM_H
#define __FREEMASTER_HPM_H
/******************************************************************************
 * platform-specific default configuration
 ******************************************************************************/

/* use 32-bit (EX) commands by default */
#ifndef FMSTR_USE_EX_CMDS
#define FMSTR_USE_EX_CMDS 1
#endif

/* do not use 16-bit (no-EX) commands by default */
#ifndef FMSTR_USE_NOEX_CMDS
#define FMSTR_USE_NOEX_CMDS 0
#endif

/* at least one of EX or no-EX command handling must be enabled */
#if !FMSTR_USE_EX_CMDS && !FMSTR_USE_NOEX_CMDS
    #error At least one of EX or no-EX command handling must be enabled (please set FMSTR_USE_EX_CMDS)
    #undef  FMSTR_USE_EX_CMDS
    #define FMSTR_USE_EX_CMDS 1
#endif

/*****************************************************************************
* Board configuration information 
******************************************************************************/

#define FMSTR_PROT_VER           3      /* protocol version 3 */
#define FMSTR_CFG_FLAGS          FMSTR_CFG_REC_LARGE_MODE      /* board info flags */
#define FMSTR_CFG_BUS_WIDTH      1      /* data bus width */
#define FMSTR_GLOB_VERSION_MAJOR 1      /* driver version */
#define FMSTR_GLOB_VERSION_MINOR 0
#define FMSTR_IDT_STRING "HPM FreeMASTER"
#define FMSTR_TSA_FLAGS          0

/******************************************************************************
* platform-specific types
******************************************************************************/

typedef unsigned char  FMSTR_U8;        /* smallest memory entity */
typedef unsigned short FMSTR_U16;       /* 16bit value */
typedef unsigned long  FMSTR_U32;       /* 32bit value */

typedef signed char    FMSTR_S8;        /* signed 8bit value */
typedef signed short   FMSTR_S16;       /* signed 16bit value */
typedef signed long    FMSTR_S32;       /* signed 32bit value */

#if FMSTR_REC_FLOAT_TRIG
typedef float          FMSTR_FLOAT;     /* float value */
#endif

typedef unsigned char  FMSTR_FLAGS;     /* type to be union-ed with flags (at least 8 bits) */
typedef unsigned char  FMSTR_SIZE8;     /* one-byte size value */
typedef signed short   FMSTR_INDEX;     /* general for-loop index (must be signed) */

typedef unsigned char  FMSTR_BCHR;      /* type of a single character in comm.buffer */
typedef unsigned char* FMSTR_BPTR;      /* pointer within a communication buffer */

typedef unsigned int FMSTR_SCISR;       /* data type to store SCI status register */

/******************************************************************************
* communication buffer access functions
******************************************************************************/

void FMSTR_CopyMemory(FMSTR_ADDR nDestAddr, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_CopyToBuffer(FMSTR_BPTR pDestBuff, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_CopyFromBuffer(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);
void FMSTR_CopyFromBufferWithMask(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);

/* mixed EX and non-EX commands may occur */
#if (FMSTR_USE_EX_CMDS) && (FMSTR_USE_NOEX_CMDS) || (FMSTR_BYTE_BUFFER_ACCESS)
void FMSTR_SetExAddr(FMSTR_BOOL bNextAddrIsEx);
#else
/* otherwise, we always know what addresses are used, (ignore FMSTR_SetExAddr) */
#define FMSTR_SetExAddr(bNextAddrIsEx) 
#endif

#if (FMSTR_BYTE_BUFFER_ACCESS)
FMSTR_BPTR FMSTR_ValueFromBuffer16(FMSTR_U16* pDest, FMSTR_BPTR pSrc);
FMSTR_BPTR FMSTR_ValueFromBuffer32(FMSTR_U32* pDest, FMSTR_BPTR pSrc);
FMSTR_BPTR FMSTR_ValueToBuffer16(FMSTR_BPTR pDest, FMSTR_U16 src);
FMSTR_BPTR FMSTR_ValueToBuffer32(FMSTR_BPTR pDest, FMSTR_U32 src);
#endif

/*********************************************************************************
* communication buffer access functions. Most of them are trivial simple on KXX
*********************************************************************************/

#define FMSTR_ValueFromBuffer8(pDest, pSrc) \
    ( (*((FMSTR_U8*)(pDest)) = *(FMSTR_U8*)(pSrc)), (((FMSTR_BPTR)(pSrc))+1) )

#if !(FMSTR_BYTE_BUFFER_ACCESS)
#define FMSTR_ValueFromBuffer16(pDest, pSrc) \
    ( (*((FMSTR_U16*)(pDest)) = *(FMSTR_U16*)(pSrc)), (((FMSTR_BPTR)(pSrc))+2) )

#define FMSTR_ValueFromBuffer32(pDest, pSrc) \
    ( (*((FMSTR_U32*)(pDest)) = *(FMSTR_U32*)(pSrc)), (((FMSTR_BPTR)(pSrc))+4) )
#endif

#define FMSTR_ValueToBuffer8(pDest, src) \
    ( (*((FMSTR_U8*)(pDest)) = (FMSTR_U8)(src)), (((FMSTR_BPTR)(pDest))+1) )

#if !(FMSTR_BYTE_BUFFER_ACCESS)
#define FMSTR_ValueToBuffer16(pDest, src) \
    ( (*((FMSTR_U16*)(pDest)) = (FMSTR_U16)(src)), (((FMSTR_BPTR)(pDest))+2) )

#define FMSTR_ValueToBuffer32(pDest, src) \
    ( (*((FMSTR_U32*)(pDest)) = (FMSTR_U32)(src)), (((FMSTR_BPTR)(pDest))+4) )
#endif

#define FMSTR_SkipInBuffer(pDest, nSize) \
    ( ((FMSTR_BPTR)(pDest)) + (nSize) )


#define FMSTR_ConstToBuffer8  FMSTR_ValueToBuffer8
#define FMSTR_ConstToBuffer16 FMSTR_ValueToBuffer16

/* EX address used only: fetching 32bit word */
#if (FMSTR_USE_EX_CMDS) && !(FMSTR_USE_NOEX_CMDS) && !(FMSTR_BYTE_BUFFER_ACCESS)
    #define FMSTR_AddressFromBuffer(pDest, pSrc) \
        FMSTR_ValueFromBuffer32(pDest, pSrc)
    #define FMSTR_AddressToBuffer(pDest, nAddr) \
        FMSTR_ValueToBuffer32(pDest, nAddr)

/* no-EX address used only: fetching 16bit word  */
#elif !(FMSTR_USE_EX_CMDS) && (FMSTR_USE_NOEX_CMDS) && !(FMSTR_BYTE_BUFFER_ACCESS)
    #define FMSTR_AddressFromBuffer(pDest, pSrc) \
        FMSTR_ValueFromBuffer16(pDest, pSrc)
    #define FMSTR_AddressToBuffer(pDest, nAddr) \
        FMSTR_ValueToBuffer16(pDest, nAddr)

/* mixed addresses used, need to process it programatically */
#else
    FMSTR_BPTR FMSTR_AddressFromBuffer(FMSTR_ADDR* pAddr, FMSTR_BPTR pSrc);
    FMSTR_BPTR FMSTR_AddressToBuffer(FMSTR_BPTR pDest, FMSTR_ADDR nAddr);
#endif

#define FMSTR_GetS8(addr)  ( *(FMSTR_S8*)(addr) )
#define FMSTR_GetU8(addr)  ( *(FMSTR_U8*)(addr) )
#define FMSTR_GetS16(addr) ( *(FMSTR_S16*)(addr) )
#define FMSTR_GetU16(addr) ( *(FMSTR_U16*)(addr) )
#define FMSTR_GetS32(addr) ( *(FMSTR_S32*)(addr) )
#define FMSTR_GetU32(addr) ( *(FMSTR_U32*)(addr) )

#if FMSTR_REC_FLOAT_TRIG
#define FMSTR_GetFloat(addr) ( *(FMSTR_FLOAT*)(addr) )
#endif

/****************************************************************************************
* Other helper macros
*****************************************************************************************/

/* This macro assigns C pointer to FMSTR_ADDR-typed variable */
#define FMSTR_PTR2ADDR(tmpAddr,ptr) ( tmpAddr = (FMSTR_ADDR) (FMSTR_U8*) ptr )
#define FMSTR_ARR2ADDR FMSTR_PTR2ADDR

/****************************************************************************************
* General peripheral space access macros
*****************************************************************************************/

#define FMSTR_SETREG8(base, offset, value)      (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG8(base, offset)             (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT8(base, offset, bit)        (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) |= bit)
#define FMSTR_CLRBIT8(base, offset, bit)        (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) &= (FMSTR_U8)~((FMSTR_U8)(bit)))
#define FMSTR_SETREG16(base, offset, value)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG16(base, offset)            (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT16(base, offset, bit)       (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) |= bit)
#define FMSTR_CLRBIT16(base, offset, bit)       (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) &= (FMSTR_U16)~((FMSTR_U16)(bit)))
#define FMSTR_TSTBIT16(base, offset, bit)       (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) & (bit))
#define FMSTR_SETREG32(base, offset, value)     (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG32(base, offset)            (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT32(base, offset, bit)       ((*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset))) |= bit)
#define FMSTR_CLRBIT32(base, offset, bit)       ((*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset))) &= ~(bit))
#define FMSTR_TSTBIT32(base, offset, bit)       (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)) & (bit))

/* UART module registers */
#define FMSTR_UART_INTR_EN_OFFSET (0x24U)
#define FMSTR_UART_TX_DATA_OFFSET (0x20U)
#define FMSTR_UART_RX_DATA_OFFSET (0x20U)
#define FMSTR_UART_LINE_STATUS_OFFSET (0x34U)

/* UART Interrupt Enable Register bits */
#define FMSTR_UART_INTR_EN_TX_SLOT_AVAILABLE (0x2U)
#define FMSTR_UART_INTR_EN_RX_DATA_AVAILABLE (0x1U)

/* UART Line Status Register bits */
#define FMSTR_UART_LINE_STATUS_TX_EMPTY (0x1U << 6)
#define FMSTR_UART_LINE_STATUS_RX_DATA_AVAILABLE (0x1U)

/*******************************************************************************************
* SCI access macros
*****************************************************************************************/
#if FMSTR_USE_UART

#define FMSTR_SCI_TXFULL(wSciSR)  (!((wSciSR) & FMSTR_UART_LINE_STATUS_TX_EMPTY))
#define FMSTR_SCI_TXEMPTY(wSciSR) ((wSciSR) & FMSTR_UART_LINE_STATUS_TX_EMPTY)
#define FMSTR_SCI_RXREADY(wSciSR) ((wSciSR) & (FMSTR_UART_LINE_STATUS_RX_DATA_AVAILABLE))

/* Transmitter-empty interrupt enable/disable */
#define FMSTR_SCI_TE() FMSTR_SETBIT32(FMSTR_UART_BASE, FMSTR_UART_INTR_EN_OFFSET, FMSTR_UART_INTR_EN_TX_SLOT_AVAILABLE)
#define FMSTR_SCI_TD() FMSTR_CLRBIT32(FMSTR_UART_BASE, FMSTR_UART_INTR_EN_OFFSET, FMSTR_UART_INTR_EN_TX_SLOT_AVAILABLE)

/* Receiver-full interrupt enable/disable */
#define FMSTR_SCI_RE() FMSTR_SETBIT32(FMSTR_UART_BASE, FMSTR_UART_INTR_EN_OFFSET, FMSTR_UART_INTR_EN_RX_DATA_AVAILABLE)
#define FMSTR_SCI_RD() FMSTR_CLRBIT32(FMSTR_UART_BASE, FMSTR_UART_INTR_EN_OFFSET, FMSTR_UART_INTR_EN_RX_DATA_AVAILABLE)

/* Tranmsit character */
#define FMSTR_SCI_PUTCHAR(ch) FMSTR_SETREG8(FMSTR_UART_BASE, FMSTR_UART_TX_DATA_OFFSET, ch)

/* Get received character */
#define FMSTR_SCI_GETCHAR() FMSTR_GETREG8(FMSTR_UART_BASE, FMSTR_UART_RX_DATA_OFFSET)

/* read status register */
#define FMSTR_SCI_GETSR()   FMSTR_GETREG32(FMSTR_UART_BASE, FMSTR_UART_LINE_STATUS_OFFSET)

/* read & clear status register */
#define FMSTR_SCI_RDCLRSR() (FMSTR_GETREG32(FMSTR_UART_BASE, FMSTR_UART_LINE_STATUS_OFFSET));\
  FMSTR_SETREG32(FMSTR_UART_BASE, FMSTR_UART_LINE_STATUS_OFFSET, FMSTR_GETREG32(FMSTR_UART_BASE, FMSTR_UART_LINE_STATUS_OFFSET));

#endif
#endif /* __FREEMASTER_HPM_H */

