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

File    : SEGGER_SYSVIEW_Config_embOS_RISCV.c
Purpose : Sample setup configuration of SystemView with embOS.
Revision: $Rev: 26222 $

Additional information:
  SEGGER_SYSVIEW_TickCnt must be incremented in the SysTick
  handler before any SYSVIEW event is generated.

  Example in embOS RTOSInit.c:

  void SysTick_Handler(void) {
  #if (OS_PROFILE != 0)
    SEGGER_SYSVIEW_TickCnt++;  // Increment SEGGER_SYSVIEW_TickCnt before calling OS_EnterNestableInterrupt().
  #endif
    OS_EnterNestableInterrupt();
    OS_TICK_Handle();
    OS_LeaveNestableInterrupt();
  }
*/
#include "SEGGER_SYSVIEW.h"
#include "hpm_csr_drv.h"
#include "hpm_clock_drv.h"

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
  SEGGER_SYSVIEW_SendSysDesc("N=" SEGGER_SYSVIEW_APP_NAME ",O=" SEGGER_SYSVIEW_OS_NAME ",D=" SEGGER_SYSVIEW_DEVICE_NAME );
  SEGGER_SYSVIEW_SendSysDesc(SEGGER_SYSVIEW_SYSDESC_MTIMER);
  SEGGER_SYSVIEW_SendSysDesc(SEGGER_SYSVIEW_SYSDESC_SOFTWARE);
  SEGGER_SYSVIEW_SendSysDesc(SEGGER_SYSVIEW_SYSDESC_ECALL);
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       SEGGER_SYSVIEW_Conf()
*
* Function description
*   Configure and initialize SystemView and register it with embOS.
*
* Additional information
*   If enabled, SEGGER_SYSVIEW_Conf() will also immediately start
*   recording events with SystemView.
*/
void SEGGER_SYSVIEW_Conf(void) {
  SEGGER_SYSVIEW_Init(SEGGER_SYSVIEW_TIMESTAMP_FREQ, SEGGER_SYSVIEW_CPU_FREQ,
                      NULL, _cbSendSystemDesc);
#if SEGGER_SYSVIEW_START_ON_INIT
  SEGGER_SYSVIEW_Start();                    // Start recording to catch system initialization.
#endif
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_X_GetTimestamp()
*
*  Function description
*    Returns the current timestamp in ticks.
*
*  Return value
*    The current timestamp.
*
*  Additional information
*    SEGGER_SYSVIEW_X_GetTimestamp is always called when interrupts are
*    disabled. Therefore locking here is not required.
*/
#if SEGGER_SYSVIEW_TIMESTAMP_FREQ_UINT_US

U32 SEGGER_SYSVIEW_X_GetTimestamp(void) {
  U32 MCycle;
  static U32 core_tick_us = 0;
  while (core_tick_us == 0) {
      core_tick_us = clock_get_core_clock_ticks_per_us();
  }
  MCycle = (U32)(hpm_csr_get_core_cycle() / core_tick_us);
  return MCycle;
}

#else

U32 SEGGER_SYSVIEW_X_GetTimestamp(void) {
  U32 cycle = read_csr(CSR_CYCLE);
  return cycle;
}

#endif

/*********************************************************************
*
*       SEGGER_SYSVIEW_X_GetInterruptId()
*
*  Function description
*    Return the currently active IRQ interrupt number
*    from the INTC_SIR_IRQ.
*/
U32 SEGGER_SYSVIEW_X_GetInterruptId(void) {
  return SEGGER_SYSVIEW_InterruptId;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_X_SetInterruptId()
*
*  Function description
*    Set the currently active IRQ interrupt number
*    to be returned by SEGGER_SYSVIEW_X_GetInterruptId.
*/
void SEGGER_SYSVIEW_X_SetInterruptId(uint32_t id) {
  SEGGER_SYSVIEW_InterruptId = id;
}

/*************************** End of file ****************************/
