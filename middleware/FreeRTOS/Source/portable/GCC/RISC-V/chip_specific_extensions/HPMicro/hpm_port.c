/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_port.h"
/* Used to program the machine timer compare register. */
extern uint64_t ullNextTime;
extern const size_t uxTimerIncrementsForOneTick; /* Assumes increment won't go over 32-bits. */
extern volatile uint64_t * pullMachineTimerCompareRegister;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if ( configUSE_TICKLESS_IDLE != 0 )
#if( configMTIME_BASE_ADDRESS == 0 ) || ( configMTIMECMP_BASE_ADDRESS == 0 )
#define portMAX_32_BIT_NUMBER                 ( 0xFFFFFFFFUL )

/* A fiddle factor to estimate the number of gptmr counts that would have
 * occurred while the gptmr counter is stopped during tickless idle
 * calculations. */
#ifndef portMISSED_GPTMR_COUNT
#define portMISSED_GPTMR_COUNT           (0)
#endif
static uint32_t xMaximumPossibleSuppressedTicks = 0;

#endif
#endif /* configUSE_TICKLESS_IDLE */

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
    /* Calculate the constants required to configure the tick interrupt. */
    #if ( configUSE_TICKLESS_IDLE != 0 )
        {
            soc_lowpower_init();
        }
    #endif /* configUSE_TICKLESS_IDLE */
}
#else
#if defined (portTIMER_SOURCE_GPTMR)
volatile uint32_t uxGptimerIncrementsForOneTick;
SDK_DECLARE_EXT_ISR_M(FREERTOS_TIMER_IRQ, vPortSysTimerIsr)
void vPortSysTimerIsr(void)
{
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
    UBaseType_t uxSavedInterruptStatus;
#endif
    BaseType_t need_switch;
    if (gptmr_check_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH))) {
        gptmr_clear_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH));
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
        uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        {
#endif
            /* Critical section start */
            need_switch = xTaskIncrementTick();
            if (pdTRUE == need_switch)
                vTaskSwitchContext();
            /* Critical section end */
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
        }
        portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
#endif
    }
}

void vPortSetupTimerInterrupt( void )
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(FREERTOS_TIMER_CLOCK, 0);
    gptmr_channel_get_default_config(FREERTOS_TIMER_RESOURCE, &config);
    gptmr_freq = clock_get_frequency(FREERTOS_TIMER_CLOCK);
    config.reload = gptmr_freq / configTICK_RATE_HZ;
    uxGptimerIncrementsForOneTick = gptmr_freq / configTICK_RATE_HZ;
    gptmr_channel_config(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, &config, false);
    gptmr_start_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);

    gptmr_enable_irq(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_IRQ_MASK(FREERTOS_TIMER_CH));

    /* Calculate the constants required to configure the tick interrupt. */
    #if ( configUSE_TICKLESS_IDLE != 0 )
        {
            soc_lowpower_init();
            xMaximumPossibleSuppressedTicks = portMAX_32_BIT_NUMBER / uxGptimerIncrementsForOneTick - 1;
        }
    #endif /* configUSE_TICKLESS_IDLE */
    intc_m_enable_irq_with_priority(FREERTOS_TIMER_IRQ, 1);
}
#elif defined (portTIMER_SOURCE_PWM)

SDK_DECLARE_EXT_ISR_M(FREERTOS_TIMER_IRQ, vPortSysTimerIsr)
void vPortSysTimerIsr(void)
{
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
    UBaseType_t uxSavedInterruptStatus;
#endif
    BaseType_t need_switch;
#ifdef HPMSOC_HAS_HPMSDK_PWM
    uint32_t irq_flag = pwm_get_irq_en(FREERTOS_TIMER_RESOURCE) & pwm_get_status(FREERTOS_TIMER_RESOURCE);
#else
    uint32_t irq_flag = pwmv2_get_reload_irq_status(FREERTOS_TIMER_RESOURCE);
#endif
    if (irq_flag) {
#ifdef HPMSOC_HAS_HPMSDK_PWM
        pwm_clear_status(FREERTOS_TIMER_RESOURCE, irq_flag);
        if (irq_flag & PWM_IRQ_RELOAD) {
#else
        pwmv2_clear_reload_irq_status(FREERTOS_TIMER_RESOURCE, irq_flag);
        if (irq_flag & ((1 << FREERTOS_TIMER_COUNTER) << PWMV2_IRQ_EN_RELOAD_IRQ_EN_RELOAD_SHIFT)) {
#endif
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
            uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
            {
#endif
                /* Critical section start */
                need_switch = xTaskIncrementTick();
                if (pdTRUE == need_switch)
                    vTaskSwitchContext();
                /* Critical section end */
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
            }
            portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
#endif                    
        }
    }
}

void vPortSetupTimerInterrupt( void )
{
    uint32_t pwm_freq;
    uint32_t reload;

    clock_add_to_group(FREERTOS_TIMER_CLOCK, 0);
    pwm_freq = clock_get_frequency(FREERTOS_TIMER_CLOCK);
    reload = pwm_freq / configTICK_RATE_HZ - 1;
#ifdef HPMSOC_HAS_HPMSDK_PWM
    pwm_stop_counter(FREERTOS_TIMER_RESOURCE);
    pwm_enable_reload_at_synci(FREERTOS_TIMER_RESOURCE);
    pwm_set_reload(FREERTOS_TIMER_RESOURCE, 0, reload);
    pwm_set_start_count(FREERTOS_TIMER_RESOURCE, 0, 0);
    pwm_issue_shadow_register_lock_event(FREERTOS_TIMER_RESOURCE);

    pwm_start_counter(FREERTOS_TIMER_RESOURCE);
    pwm_enable_irq(FREERTOS_TIMER_RESOURCE, PWM_IRQ_RELOAD);
#else
    pwmv2_deinit(FREERTOS_TIMER_RESOURCE);
    pwmv2_shadow_register_unlock(FREERTOS_TIMER_RESOURCE);
    pwmv2_set_shadow_val(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_COUNTER_SHADOW, reload, 0, false);
    pwmv2_counter_select_data_offset_from_shadow_value(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_COUNTER, FREERTOS_TIMER_COUNTER_SHADOW);
    pwmv2_enable_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_COUNTER);
    pwmv2_shadow_register_lock(FREERTOS_TIMER_RESOURCE);
    pwmv2_enable_reload_irq(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_COUNTER);

#endif
    intc_m_enable_irq_with_priority(FREERTOS_TIMER_IRQ, 1);
}
#endif
#endif

#if ( configUSE_TICKLESS_IDLE != 0 )
#pragma GCC push_options
#pragma GCC optimize ("O0")
#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

    ATTR_RAMFUNC __attribute__( ( weak ) ) void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
    {
        uint64_t ulCurrentTime;
        ulCurrentTime =  *( volatile uint64_t * const ) ( configMTIME_BASE_ADDRESS );
        pullMachineTimerCompareRegister  = ( volatile uint64_t * const ) ( configMTIMECMP_BASE_ADDRESS );
        uint64_t ullCmpValue, ulCompleteTickPeriods, ulCompletedGptTickDecrements;
        TickType_t xModifiableIdleTime;

        /* Calculate the new compare value required to wait xExpectedIdleTime
         * tick periods.  -1 is used because this code will execute part way
         * through one of the tick periods. */
        ullCmpValue = *pullMachineTimerCompareRegister + ( uxTimerIncrementsForOneTick * ( xExpectedIdleTime - 1UL ) );

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
         * method as that will mask interrupts that should exit sleep mode. */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
        portDISABLE_GLOBAL_INTERRUPTS();
#else
        portDISABLE_INTERRUPTS();
#endif

        /* If a context switch is pending or a task is waiting for the scheduler
         * to be unsuspended then abandon the low power entry. */
        if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        {
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portENABLE_GLOBAL_INTERRUPTS();
#else
            portENABLE_INTERRUPTS();
#endif
        }
        else
        {
            /* Set the new compare value. */
            *pullMachineTimerCompareRegister = ullCmpValue;
            ullNextTime = *pullMachineTimerCompareRegister + uxTimerIncrementsForOneTick;

            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
            xModifiableIdleTime = xExpectedIdleTime;
            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

            if( xModifiableIdleTime > 0 )
            {
                __asm volatile ( "wfi" );
            }

            configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

            /* Re-enable interrupts to allow the interrupt that brought the MCU
             * out of sleep mode to execute immediately. */
            disable_mchtmr_irq();
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portENABLE_GLOBAL_INTERRUPTS();
#else
            portENABLE_INTERRUPTS();
#endif

            /* Disable interrupts again because the clock is about to be stopped
             * and interrupts that execute while the clock is stopped will increase
             * any slippage between the time maintained by the RTOS and calendar
             * time. */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portDISABLE_GLOBAL_INTERRUPTS();
#else
            portDISABLE_INTERRUPTS();
#endif
            /* Determine if the mchtmr has already reach compare value.
             * Or if the mchtmr is yet reach the compare value (in which 
             * case an interrupt other than the mchtmr) must have brought 
             * the system out of sleep mode). */
            ulCurrentTime =  *( volatile uint64_t * const ) ( configMTIME_BASE_ADDRESS );
            if ( ulCurrentTime > *pullMachineTimerCompareRegister ) /* current value > compare value, exit sleep mode as we thought*/
            {
                
                *pullMachineTimerCompareRegister = *pullMachineTimerCompareRegister + uxTimerIncrementsForOneTick;
                ullNextTime = *pullMachineTimerCompareRegister + uxTimerIncrementsForOneTick;
                /*  Not same as ARM, the pending mchtr isr will not exe after enable interrupts,
                 *  so we have no need to decrease 1 to the expected idle time
                 */
                ulCompleteTickPeriods = xExpectedIdleTime;
            }
            else
            {
                /* Something other than the tick interrupt ended the sleep.
                 * Work out how long the sleep lasted rounded to complete tick
                 * periods (not the ulReload value which accounted for part
                 * ticks). */
                ulCurrentTime =  *( volatile uint64_t * const ) ( configMTIME_BASE_ADDRESS );
                ulCompletedGptTickDecrements = ( *pullMachineTimerCompareRegister ) - ulCurrentTime;

                /* How many complete tick periods passed while the processor
                 * was waiting? */
                ulCompleteTickPeriods = xExpectedIdleTime - ulCompletedGptTickDecrements / uxTimerIncrementsForOneTick - 1;
                *pullMachineTimerCompareRegister = (ulCurrentTime / uxTimerIncrementsForOneTick + 1) * uxTimerIncrementsForOneTick;
                ullNextTime = *pullMachineTimerCompareRegister + uxTimerIncrementsForOneTick;
            }

            /* Restart mchtmr. */
            vTaskStepTick( ulCompleteTickPeriods );
            enable_mchtmr_irq();

            /* Exit with interrupts enabled. */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portENABLE_GLOBAL_INTERRUPTS();
#else
            portENABLE_INTERRUPTS();
#endif
        
        }
    }
#elif defined (portTIMER_SOURCE_GPTMR)

    ATTR_RAMFUNC __attribute__( ( weak ) ) void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
    {
        uint32_t ulReloadValue, ulCompleteTickPeriods;
        uint32_t ulCntCurrent, ulCntWithinOneTick;
        TickType_t xModifiableIdleTime;

        /* Make sure the Gptmr reload value does not overflow the counter. */
        if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
        {
            xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
        }

        /* Stop the Gptmr momentarily.  The time the SysTick is stopped for
         * is accounted for as best it can be, but using the tickless mode will
         * inevitably result in some tiny drift of the time maintained by the
         * kernel with respect to calendar time. */
        gptmr_stop_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);
        /* Calculate the reload value required to wait xExpectedIdleTime
         * tick periods.  -1 is used because this code will execute part way
         * through one of the tick periods. */
        ulReloadValue = uxGptimerIncrementsForOneTick * xExpectedIdleTime;

        if( ulReloadValue > portMISSED_GPTMR_COUNT )
        {
            ulReloadValue -= portMISSED_GPTMR_COUNT;
        }

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
         * method as that will mask interrupts that should exit sleep mode. */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
        portDISABLE_GLOBAL_INTERRUPTS();
#else
        portDISABLE_INTERRUPTS();
#endif

        /* If a context switch is pending or a task is waiting for the scheduler
         * to be unsuspended then abandon the low power entry. */
        if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        {
            gptmr_start_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);

            /* Re-enable interrupts */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portENABLE_GLOBAL_INTERRUPTS();
#else
            portENABLE_INTERRUPTS();
#endif
        }
        else
        {
            /* Set the new reload value. */
            gptmr_channel_config_update_reload(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, ulReloadValue);

            /* Clear the reload flag and set the count value back to
             * zero. */
            if (gptmr_check_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH)))
            {
                gptmr_clear_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH));
            }

            /* Restart Gptmr. */
            gptmr_start_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);

            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
            xModifiableIdleTime = xExpectedIdleTime;
            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

            if( xModifiableIdleTime > 0 )
            {
                __asm volatile ( "wfi" );
            }

            configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

            /* Re-enable interrupts to allow the interrupt that brought the MCU
             * out of sleep mode to execute immediately.  see comments above
             * __disable_interrupt() call above. */
            intc_m_disable_irq(FREERTOS_TIMER_IRQ);
            /* Turn on MSTATUS.MIE so we will enter plic isr function */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portENABLE_GLOBAL_INTERRUPTS();
#else
            portENABLE_INTERRUPTS();
#endif

            /* Disable interrupts again because the clock is about to be stopped
             * and interrupts that execute while the clock is stopped will increase
             * any slippage between the time maintained by the RTOS and calendar
             * time. */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portDISABLE_GLOBAL_INTERRUPTS();
#else
            portDISABLE_INTERRUPTS();
#endif

            /* Stop the Gptmr. using the tickless mode will inevitably result in some tiny
             * drift of the time maintained by the kernel with respect to calendar
             * time*/
            gptmr_stop_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);

            /* Determine if the Gptmr has already counted to reload value and
             * been set back to the zero (the reload back being
             * correct for the entire expected idle time) or if the Gptmr is yet
             * to count to reload value (in which case an interrupt other than the Gptmr
             * must have brought the system out of sleep mode). */
            if (gptmr_check_status(FREERTOS_TIMER_RESOURCE, GPTMR_CH_RLD_STAT_MASK(FREERTOS_TIMER_CH)))
            {
                ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
                gptmr_channel_update_count(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, 0);
            }
            else
            {
                ulCntCurrent = gptmr_channel_get_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, gptmr_counter_type_normal);
                ulCompleteTickPeriods = ulCntCurrent / uxGptimerIncrementsForOneTick;
                ulCntWithinOneTick = ulCntCurrent % uxGptimerIncrementsForOneTick;
                /* Don't allow a tiny difference from reload value, or values that have somehow
                 * overflowed because the post sleep hook did something
                 * that took too long. */
                if ( ulCntWithinOneTick > uxGptimerIncrementsForOneTick - portMISSED_GPTMR_COUNT )
                {
                    ulCompleteTickPeriods += 1;
                    gptmr_channel_update_count(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, 0);
                }
                else
                {
                    gptmr_channel_update_count(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, ulCntWithinOneTick);
                }
            }

            /* Restart Gptmr */
            gptmr_channel_config_update_reload(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH, uxGptimerIncrementsForOneTick);
            gptmr_start_counter(FREERTOS_TIMER_RESOURCE, FREERTOS_TIMER_CH);
            vTaskStepTick( ulCompleteTickPeriods );
            intc_m_enable_irq(FREERTOS_TIMER_IRQ);
            /* Exit with interrupts enabled. */
#if defined(USE_SYSCALL_INTERRUPT_PRIORITY) && USE_SYSCALL_INTERRUPT_PRIORITY
            portENABLE_GLOBAL_INTERRUPTS();
#else
            portENABLE_INTERRUPTS();
#endif
        }
    }
#else
#error "Only Machine Timer or Gptmr is supported for tickless idle mode in HPMicro FreeRTOS port."
#endif
#pragma GCC pop_options
#endif /* #if configUSE_TICKLESS_IDLE */
