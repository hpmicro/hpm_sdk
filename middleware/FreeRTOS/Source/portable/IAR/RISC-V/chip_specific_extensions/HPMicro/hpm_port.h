/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef HPM_PORT_H
#define HPM_PORT_H

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#if ( configUSE_TICKLESS_IDLE != 0 )
#include "low_power_inf.h"
#endif

#if( configMTIME_BASE_ADDRESS == 0 ) || ( configMTIMECMP_BASE_ADDRESS == 0 )
    #include "hpm_gptmr_drv.h"
    #include "hpm_clock_drv.h"

    #if defined(CONFIG_FREERTOS_TICKLESS_USE_STOP_MODE) && CONFIG_FREERTOS_TICKLESS_USE_STOP_MODE
        #ifndef FREERTOS_TIMER_RESOURCE
            #define FREERTOS_TIMER_RESOURCE      BOARD_FREERTOS_LOWPOWER_TIMER
        #endif

        #ifndef FREERTOS_TIMER_CH
            #define FREERTOS_TIMER_CH            BOARD_FREERTOS_LOWPOWER_TIMER_CHANNEL
        #endif

        #ifndef FREERTOS_TIMER_IRQ
            #define FREERTOS_TIMER_IRQ           BOARD_FREERTOS_LOWPOWER_TIMER_IRQ
        #endif

        #ifndef FREERTOS_TIMER_CLOCK
            #define FREERTOS_TIMER_CLOCK         BOARD_FREERTOS_LOWPOWER_TIMER_CLK_NAME
        #endif
    #else
        #ifndef FREERTOS_TIMER_RESOURCE
            #define FREERTOS_TIMER_RESOURCE      BOARD_FREERTOS_TIMER
        #endif

        #ifndef FREERTOS_TIMER_CH
            #define FREERTOS_TIMER_CH            BOARD_FREERTOS_TIMER_CHANNEL
        #endif

        #ifndef FREERTOS_TIMER_IRQ
            #define FREERTOS_TIMER_IRQ           BOARD_FREERTOS_TIMER_IRQ
        #endif

        #ifndef FREERTOS_TIMER_CLOCK
            #define FREERTOS_TIMER_CLOCK         BOARD_FREERTOS_TIMER_CLK_NAME
        #endif
    #endif
#endif

#endif /* HPM_PORT_H */