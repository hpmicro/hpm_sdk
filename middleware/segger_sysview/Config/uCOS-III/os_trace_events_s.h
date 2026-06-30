/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2024 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : os_trace_events_s.h
Purpose : Interface header for Micrium uC/OS-III and SystemView.
Revision: $Rev: 9599 $
*/

/*
************************************************************************************************************************
*                                          uC/OS-III Trace Kernel-Related Macros
************************************************************************************************************************
*/
#ifndef OS_TRACE_EVENTS_S_H
#define OS_TRACE_EVENTS_S_H

#define  OS_TRACE_ISR_SETID_ASM                     SEGGER_SYSVIEW_X_SetInterruptId
#define  OS_TRACE_ISR_ENTER_ASM                     SEGGER_SYSVIEW_RecordEnterISR
#define  OS_TRACE_ISR_EXIT_ASM                      SEGGER_SYSVIEW_RecordExitISR
#define  OS_TRACE_TASK_SWITCHED_IN_ASM              SYSVIEW_TaskSwitchedIn
#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()                        SEGGER_SYSVIEW_RecordExitISR()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()           SEGGER_SYSVIEW_RecordExitISRToScheduler()

#endif
