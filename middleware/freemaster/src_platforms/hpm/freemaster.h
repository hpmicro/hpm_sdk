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


#ifndef __FREEMASTER_H
#define __FREEMASTER_H

/* identify our current platform */
#define FMSTR_PLATFORM_HPM 1

/* FreeMASTER configuration */
#include "freemaster_defcfg.h"

/*****************************************************************************
* Global types
******************************************************************************/

typedef unsigned char *FMSTR_ADDR;              /* CPU address type (4bytes) */
typedef unsigned short FMSTR_SIZE;              /* general size type (at least 16 bits) */
typedef unsigned long  FMSTR_SIZE32;            /* general size type (at least size of address (typicaly 32bits)) */
typedef unsigned char FMSTR_BOOL;               /* general boolean type  */
#if FMSTR_REC_LARGE_MODE
typedef unsigned long FMSTR_SIZE_RECBUFF;
#else
typedef unsigned short FMSTR_SIZE_RECBUFF;
#endif

/* application command-specific types */
typedef unsigned char FMSTR_APPCMD_CODE;
typedef unsigned char FMSTR_APPCMD_DATA, *FMSTR_APPCMD_PDATA;
typedef unsigned char FMSTR_APPCMD_RESULT;

/* pointer to application command callback handler */
typedef FMSTR_APPCMD_RESULT (*FMSTR_PAPPCMDFUNC)(FMSTR_APPCMD_CODE code, FMSTR_APPCMD_PDATA pdata, FMSTR_SIZE size);

/* pipe-related types */
typedef void* FMSTR_HPIPE;                      /* pipe handle */
typedef unsigned short FMSTR_PIPE_PORT;         /* pipe port identifier (unsigned, 7 bits used) */
typedef unsigned short FMSTR_PIPE_SIZE;         /* pipe buffer size type (unsigned, at least 16 bits) */

/* pointer to pipe event handler */
typedef void (*FMSTR_PPIPEFUNC)(FMSTR_HPIPE);

/*****************************************************************************
* TSA-related user types and macros
******************************************************************************/

#include "freemaster_tsa.h"

/*****************************************************************************
* Constants
******************************************************************************/

/* application command status information  */
#define FMSTR_APPCMDRESULT_NOCMD      0xffU
#define FMSTR_APPCMDRESULT_RUNNING    0xfeU
#define MFSTR_APPCMDRESULT_LASTVALID  0xf7U  /* F8-FF are reserved  */

/* recorder time base declaration helpers */
#define FMSTR_REC_BASE_SECONDS(x)  ((x) & 0x3fffU)
#define FMSTR_REC_BASE_MILLISEC(x) (((x) & 0x3fffU) | 0x4000U)
#define FMSTR_REC_BASE_MICROSEC(x) (((x) & 0x3fffU) | 0x8000U)
#define FMSTR_REC_BASE_NANOSEC(x)  (((x) & 0x3fffU) | 0xc000U)

/******************************************************************************
* NULL needed
******************************************************************************/

#ifndef NULL
#ifdef __cplusplus
#define NULL (0)
#else
#define NULL ((void *) 0)
#endif
#endif

/*****************************************************************************
* Global functions
******************************************************************************/
#ifdef __cplusplus
  extern "C" {
#endif

/* Assigning FreeMASTER communication module base address */
void FMSTR_SetUartBaseAddress(FMSTR_ADDR address);

/* FreeMASTER serial communication API */
FMSTR_BOOL FMSTR_Init(void);    /* general initialization */
void FMSTR_Poll(void);          /* polling call, use in SHORT_INTR and POLL_DRIVEN modes */
void FMSTR_Isr(void);           /* SCI interrupt handler for LONG_INTR and SHORT_INTR modes */

/* Recorder API */
void FMSTR_Recorder(void);
void FMSTR_TriggerRec(void);
void FMSTR_SetUpRecBuff(FMSTR_ADDR nBuffAddr, FMSTR_SIZE_RECBUFF nBuffSize);

/* Application commands API */
FMSTR_APPCMD_CODE  FMSTR_GetAppCmd(void);
FMSTR_APPCMD_PDATA FMSTR_GetAppCmdData(FMSTR_SIZE* pDataLen);
FMSTR_BOOL         FMSTR_RegisterAppCmdCall(FMSTR_APPCMD_CODE nAppCmdCode, FMSTR_PAPPCMDFUNC pCallbackFunc);

void FMSTR_AppCmdAck(FMSTR_APPCMD_RESULT nResultCode);
void FMSTR_AppCmdSetResponseData(FMSTR_ADDR nResultDataAddr, FMSTR_SIZE nResultDataLen);

/* Dynamic TSA API */
FMSTR_BOOL FMSTR_SetUpTsaBuff(FMSTR_ADDR nBuffAddr, FMSTR_SIZE nBuffSize);
FMSTR_BOOL FMSTR_TsaAddVar(FMSTR_TSATBL_STRPTR pszName, FMSTR_TSATBL_STRPTR pszType, 
                           FMSTR_TSATBL_VOIDPTR nAddr, FMSTR_SIZE32 nSize, FMSTR_SIZE nFlags);

/* FreeMASTER LIN Transport Layer processing. User should call ld_receive_message() on his own. */
FMSTR_BOOL FMSTR_IsLinTLFrame(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize);
FMSTR_BOOL FMSTR_ProcessLinTLFrame(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize);

/* Loss-less Communication Pipes API */
FMSTR_HPIPE FMSTR_PipeOpen(FMSTR_PIPE_PORT nPort, FMSTR_PPIPEFUNC pCallback,
                           FMSTR_ADDR pRxBuff, FMSTR_PIPE_SIZE nRxSize,
                           FMSTR_ADDR pTxBuff, FMSTR_PIPE_SIZE nTxSize);
void FMSTR_PipeClose(FMSTR_HPIPE hpipe);
FMSTR_PIPE_SIZE FMSTR_PipeWrite(FMSTR_HPIPE hpipe, FMSTR_ADDR addr, FMSTR_PIPE_SIZE length, FMSTR_PIPE_SIZE granularity);
FMSTR_PIPE_SIZE FMSTR_PipeRead(FMSTR_HPIPE hpipe, FMSTR_ADDR addr, FMSTR_PIPE_SIZE length, FMSTR_PIPE_SIZE granularity);

/* Pipe printing and formatting */
FMSTR_BOOL FMSTR_PipePuts(FMSTR_HPIPE hpipe, const char* pszStr);
FMSTR_BOOL FMSTR_PipePrintf(FMSTR_HPIPE hpipe, const char* pszFmt, ...);
FMSTR_BOOL FMSTR_PipePrintfU8(FMSTR_HPIPE hpipe, const char* pszFmt, unsigned char arg);
FMSTR_BOOL FMSTR_PipePrintfS8(FMSTR_HPIPE hpipe, const char* pszFmt, signed char arg);
FMSTR_BOOL FMSTR_PipePrintfU16(FMSTR_HPIPE hpipe, const char* pszFmt, unsigned short arg);
FMSTR_BOOL FMSTR_PipePrintfS16(FMSTR_HPIPE hpipe, const char* pszFmt, signed short arg);
FMSTR_BOOL FMSTR_PipePrintfU32(FMSTR_HPIPE hpipe, const char* pszFmt, unsigned long arg);
FMSTR_BOOL FMSTR_PipePrintfS32(FMSTR_HPIPE hpipe, const char* pszFmt, signed long arg);

#ifdef __cplusplus
  }
#endif

#endif /* __FREEMASTER_H */

