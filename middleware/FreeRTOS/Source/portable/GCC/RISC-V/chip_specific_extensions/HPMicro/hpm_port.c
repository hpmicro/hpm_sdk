/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
/* Used to program the machine timer compare register. */
extern uint64_t ullNextTime;
extern const size_t uxTimerIncrementsForOneTick; /* Assumes increment won't go over 32-bits. */
extern volatile uint64_t * pullMachineTimerCompareRegister;

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

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
#else
#include "hpm_gptmr_drv.h"
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
void vPortSysTimerIsr(void)
{
    BaseType_t need_switch;
    if (gptmr_check_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH))) {
        gptmr_clear_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH));
        need_switch = xTaskIncrementTick();
        if (pdTRUE == need_switch)
            vTaskSwitchContext();
    }
}
SDK_DECLARE_EXT_ISR_M(FREERTOS_TIMER_IRQ, vPortSysTimerIsr);

void vPortSetupTimerInterrupt( void )
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(FREERTOS_TIMER_RESOURCE, &config);

    gptmr_freq = clock_get_frequency(FREERTOS_TIMER_CLOCK);
    config.reload = gptmr_freq / configTICK_RATE_HZ;
    gptmr_channel_config(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, &config, false);
    gptmr_start_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);

    gptmr_enable_irq(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_IRQ_MASK(FREERTOS_TIMER_CH));
    intc_m_enable_irq_with_priority(FREERTOS_TIMER_IRQ, 1);
}

#endif