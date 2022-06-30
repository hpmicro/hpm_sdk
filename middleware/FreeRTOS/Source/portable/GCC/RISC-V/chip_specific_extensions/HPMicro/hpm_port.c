/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Scheduler includes. */
#include "FreeRTOS.h"

/* Used to program the machine timer compare register. */
extern uint64_t ullNextTime;
extern const size_t uxTimerIncrementsForOneTick; /* Assumes increment won't go over 32-bits. */
extern volatile uint64_t * pullMachineTimerCompareRegister;

void vPortSetupTimerInterrupt( void )
{
    uint64_t ulCurrentTime;

    pullMachineTimerCompareRegister  = ( volatile uint64_t * const ) ( configMTIMECMP_BASE_ADDRESS );
    ulCurrentTime =  *( volatile uint64_t * const ) ( configMTIME_BASE_ADDRESS );

    ullNextTime = ulCurrentTime + ( uint64_t ) uxTimerIncrementsForOneTick;
    *pullMachineTimerCompareRegister = ullNextTime;

    /* Prepare the time to use after the next tick interrupt. */
    ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
}
