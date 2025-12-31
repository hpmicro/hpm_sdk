/*
 * Copyright (c) 2024-2025-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_pcfg_drv.h"
#include "low_power_inf.h"
#include "hpm_pdgo_drv.h"
#include "FreeRTOS.h"
#include "task.h"

static void enable_wkup_pin_irq(void)
{
    pdgo_enable_irq0_by_bit_mask(HPM_PDGO, dgo_irq0_wkup_pin);
    intc_m_enable_irq_with_priority(IRQn_DGO_PAD_WAKEUP, 2);
}

SDK_DECLARE_EXT_ISR_M(IRQn_DGO_PAD_WAKEUP, wakeup_isr)
void wakeup_isr(void)
{
    if ((pdgo_get_irq0_status(HPM_PDGO) & dgo_irq0_wkup_pin) != 0) {
        pdgo_clear_irq0_status(HPM_PDGO, dgo_irq0_wkup_pin);
    }
}

void prepare_soc_low_power(void)
{
    pcfg_dcdc_set_lp_current_limit(HPM_PCFG, pcfg_dcdc_lp_current_limit_250ma, false);
    pcfg_dcdc_set_current_hys_range(HPM_PCFG, pcfg_dcdc_current_hys_25mv);
    enable_wkup_pin_irq();
}

ATTR_RAMFUNC void vPreSleepProcessing(unsigned long uxExpectedIdleTime)
{
    (void)(uxExpectedIdleTime);

#if (configMTIME_BASE_ADDRESS != 0) && (configMTIMECMP_BASE_ADDRESS != 0)
#error "Machine Timer not supported cpu stop mode yet! Please set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR in CMakeLists to select gptmr to generate system tick."
#else
    uint32_t retention = 0x40FUL;
    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_trigger_system_lp);
    sysctl_enable_cpu0_wakeup_source_with_irq(HPM_SYSCTL, BOARD_FREERTOS_LOWPOWER_TIMER_IRQ);
    sysctl_enable_cpu0_wakeup_source_with_irq(HPM_SYSCTL, IRQn_DGO_PAD_WAKEUP);
#endif /* #if (configMTIME_BASE_ADDRESS != 0) && (configMTIMECMP_BASE_ADDRESS != 0) */

    /* Call User Custom Processing Code */
    vPreSleepUserCustomProcessing(uxExpectedIdleTime);
}

/* In order to further reduce power consumption, turn off unnecessary peripherals here,
 * keep the required peripherals running, and turn on the wake-up interrupt of the
 * corresponding peripherals. */
ATTR_RAMFUNC void vPreSleepUserCustomProcessing(unsigned long uxExpectedIdleTime)
{
    (void)(uxExpectedIdleTime);

    /* Turn off the LED to indicate that we are in low power */
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, board_get_led_gpio_off_level());
    printf("Enter stop mode, tick %d\n", xTaskGetTickCount());
}

ATTR_RAMFUNC void vPostSleepUserCustomProcessing(unsigned long uxExpectedIdleTime)
{
    (void)(uxExpectedIdleTime);
    /* Turn on the LED to indicate that we are out of low power */
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, board_get_led_gpio_off_level() ? 0 : 1);
    printf("Exit stop mode\n");
}
