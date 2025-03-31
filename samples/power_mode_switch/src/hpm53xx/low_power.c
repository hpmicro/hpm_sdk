/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pcfg_drv.h"
#include "hpm_pdgo_drv.h"

#define POWER_DOWN_COUNT (0x100)

static void show_power_status(uint32_t retention_mask)
{
    static const char * const domain_name[] = {
        "SOC Power & Mem", "SOC Periph & Registers",
        "CPU0 Power & Mem", "CPU0 Periph & Registers",
        "XTAL", "PLL0", "PLL1",
    };
    printf("---------------------------------------------\n");
    for (uint32_t i = 0; i < ARRAY_SIZE(domain_name); i++) {
        if (retention_mask & (1 << i)) {
            printf("%s: ON\n", domain_name[i]);
        } else {
            printf("%s: OFF\n", domain_name[i]);
        }
    }
    printf("---------------------------------------------\n");
}

static void enable_wkup_pin_irq(void)
{
    pdgo_enable_wkup_software_wakeup(HPM_PDGO);
    intc_m_enable_irq_with_priority(IRQn_PAD_WAKEUP, 2);
}

SDK_DECLARE_EXT_ISR_M(IRQn_PAD_WAKEUP, wakeup_isr)
void wakeup_isr(void)
{
    ;
}

void prepare_soc_low_power(void)
{
    pcfg_dcdc_set_lp_current_limit(HPM_PCFG, pcfg_dcdc_lp_current_limit_250ma);
    pcfg_dcdc_set_current_hys_range(HPM_PCFG, pcfg_dcdc_current_hys_25mv);
    enable_wkup_pin_irq();
}


void enter_wait_mode(void)
{
    printf("Entering wait mode\n");
    printf("Press WKUP pin to wakeup from the wait mode\n");

    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_gate_cpu_clock);
    WFI();
}

void enter_stop_mode(void)
{
    uint32_t retention = 0x0FUL;

    printf("Entering stop mode\n");
    show_power_status(retention);
    printf("Press WKUP pin to wakeup from the stop mode\n");

    sysctl_enable_cpu0_wakeup_source_with_irq(HPM_SYSCTL, IRQn_PAD_WAKEUP);

    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_trigger_system_lp);
    WFI();
}


void enter_standby_mode(void)
{
    uint32_t retention = 0;

    printf("Entering standby mode\n");
    show_power_status(retention);
    printf("Press WKUP pin to wakeup from the standby mode\n");

    pcfg_enable_power_trap(HPM_PCFG);
    pcfg_disable_dcdc_retention(HPM_PCFG);

    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_trigger_system_lp);
    WFI();
}

void enter_shutdown_mode(void)
{
    printf("Entering shutdown mode\n");
    printf("Press WAKEUP or RESETN to wake up from the shutdown mode\n");

    pdgo_set_turnoff_counter(HPM_PDGO, POWER_DOWN_COUNT);

    while (1) {
        ;
    }
}
