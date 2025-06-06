/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** NetX Component                                                        */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */ 
/*                                                                        */ 
/*    nx_port.h                                           Linux/GNU       */ 
/*                                                          6.1           */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file contains data type definitions that make the NetX         */ 
/*    real-time TCP/IP function identically on a variety of different     */ 
/*    processor architectures.                                            */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Yuxin Zhou               Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/

#ifndef NX_PORT_H
#define NX_PORT_H


#ifdef NX_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in nx_user.h. The defines in this file may 
   alternately be defined on the command line.  */

#include "nx_user.h"
#endif


/* Default to little endian, since this is what most ARM targets are.  */

#define NX_LITTLE_ENDIAN    1


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* Define various constants for the port.  */ 

#ifndef NX_IP_PERIODIC_RATE
#ifdef TX_TIMER_TICKS_PER_SECOND
#define NX_IP_PERIODIC_RATE         TX_TIMER_TICKS_PER_SECOND
#else
#define NX_IP_PERIODIC_RATE         100
#endif
#endif


/* Define macros that swap the endian for little endian ports.  */
#ifdef NX_LITTLE_ENDIAN
#define NX_CHANGE_ULONG_ENDIAN(arg)                         \
    {                                                       \
        ULONG _i;                                           \
        ULONG _tmp;                                         \
        _i = (UINT)arg;                                     \
        /* _i = A, B, C, D */                               \
        _tmp = _i ^ (((_i) >> 16) | (_i << 16));            \
        /* tmp = _i ^ (_i ROR 16) = A^C, B^D, C^A, D^B*/    \
        _tmp &= 0xff00ffff;                                 \
        /* tmp = A^C, 0, C^A, D^B */                        \
        _i = ((_i) >> 8) | (_i<<24);                        \
        /* _i = D, A, B, C */                               \
        _i = _i ^ ((_tmp) >> 8);                            \
        /* _i = D, C, B, A */                               \
        arg = _i;                                           \
    }
#define NX_CHANGE_USHORT_ENDIAN(a)      a = ((USHORT)((a >> 8) | (a << 8)) & 0xFFFF)


#define __SWAP32__(val) ((((val) & 0xFF000000) >> 24 ) | (((val) & 0x00FF0000) >> 8) \
        | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24))

#define __SWAP16__(val) (USHORT)((((val) & 0xFF00) >> 8) | (((val) & 0x00FF) << 8))


#ifndef htonl
#define htonl(val)  __SWAP32__(val)
#endif /* htonl */
#ifndef ntohl
#define ntohl(val)  __SWAP32__(val)
#endif /* htonl */

#ifndef htons
#define htons(val)  __SWAP16__(val)
#endif /*htons */

#ifndef ntohs
#define ntohs(val)  __SWAP16__(val)
#endif /*htons */


#else
#define NX_CHANGE_ULONG_ENDIAN(a)
#define NX_CHANGE_USHORT_ENDIAN(a)

#ifndef htons
#define htons(val) (val)
#endif /* htons */

#ifndef ntohs
#define ntohs(val) (val)
#endif /* ntohs */

#ifndef ntohl
#define ntohl(val) (val)
#endif

#ifndef htonl
#define htonl(val) (val)
#endif /* htonl */
#endif


/* Define several macros for the error checking shell in NetX.  */

#ifndef TX_TIMER_PROCESS_IN_ISR

#define NX_CALLER_CHECKING_EXTERNS          extern  TX_THREAD           *_tx_thread_current_ptr; \
                                            extern  TX_THREAD           _tx_timer_thread; \
                                            extern  volatile ULONG      _tx_thread_system_state;

#define NX_THREADS_ONLY_CALLER_CHECKING     if ((_tx_thread_system_state) || \
                                                (_tx_thread_current_ptr == TX_NULL) || \
                                                (_tx_thread_current_ptr == &_tx_timer_thread)) \
                                                return(NX_CALLER_ERROR);

#define NX_INIT_AND_THREADS_CALLER_CHECKING if (((_tx_thread_system_state) && (_tx_thread_system_state < ((ULONG) 0xF0F0F0F0))) || \
                                                (_tx_thread_current_ptr == &_tx_timer_thread)) \
                                                return(NX_CALLER_ERROR);


#define NX_NOT_ISR_CALLER_CHECKING          if ((_tx_thread_system_state) && (_tx_thread_system_state < ((ULONG) 0xF0F0F0F0))) \
                                                return(NX_CALLER_ERROR);

#define NX_THREAD_WAIT_CALLER_CHECKING      if ((wait_option) && \
                                               ((_tx_thread_current_ptr == NX_NULL) || (_tx_thread_system_state) || (_tx_thread_current_ptr == &_tx_timer_thread))) \
                                            return(NX_CALLER_ERROR);


#else



#define NX_CALLER_CHECKING_EXTERNS          extern  TX_THREAD           *_tx_thread_current_ptr; \
                                            extern  volatile ULONG      _tx_thread_system_state;

#define NX_THREADS_ONLY_CALLER_CHECKING     if ((_tx_thread_system_state) || \
                                                (_tx_thread_current_ptr == TX_NULL)) \
                                                return(NX_CALLER_ERROR);

#define NX_INIT_AND_THREADS_CALLER_CHECKING if (((_tx_thread_system_state) && (_tx_thread_system_state < ((ULONG) 0xF0F0F0F0)))) \
                                                return(NX_CALLER_ERROR);

#define NX_NOT_ISR_CALLER_CHECKING          if ((_tx_thread_system_state) && (_tx_thread_system_state < ((ULONG) 0xF0F0F0F0))) \
                                                return(NX_CALLER_ERROR);

#define NX_THREAD_WAIT_CALLER_CHECKING      if ((wait_option) && \
                                               ((_tx_thread_current_ptr == NX_NULL) || (_tx_thread_system_state))) \
                                            return(NX_CALLER_ERROR);

#endif


/* Define the version ID of NetX.  This may be utilized by the application.  */

#ifdef NX_SYSTEM_INIT
CHAR                            _nx_version_id[] = 
                                    "Copyright (c) Microsoft Corporation. All rights reserved.  *  NetX Duo Linux/GNU Version 6.2.1 *";
#else
extern  CHAR                    _nx_version_id[];
#endif

#endif

