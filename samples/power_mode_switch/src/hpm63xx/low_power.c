/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "hpm_uart_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_pcfg_drv.h"
#include "hpm_bcfg_drv.h"
#include "hpm_bpor_drv.h"

#define CLOCK_ON (1)
#define CLOCK_OFF (2)
#define POWER_OFF (3)
#define POWER_DOWN_COUNT (0x100)

void prepare_soc_low_power(void)
{
    bcfg_vbg_enable_lp_mode(HPM_BCFG);
    bcfg_ldo_set_voltage(HPM_BCFG, 800);
    pcfg_dcdc_set_lp_current_limit(HPM_PCFG, pcfg_dcdc_lp_current_limit_250ma, false);
    pcfg_dcdc_set_current_hys_range(HPM_PCFG, pcfg_dcdc_current_hys_25mv);

    clock_remove_from_group(clock_eth0, 0);
    clock_remove_from_group(clock_ntmr0, 0);
}

static void show_power_status(uint32_t retention_mask)
{
    static const char * const domain_name[] = {
        "SOC Power & Mem", "SOC Periph & Registers",
        "CPU0 Power & Mem", "CPU0 Periph & Registers",
        "XTAL", "PLL0", "PLL1", "PLL2",
    };
    printf("---------------------------------------------\n");
    for (uint32_t i = 0; i < 8; i++) {
        if (retention_mask & (1 << i)) {
            printf("%s: ON\n", domain_name[i]);
        } else {
            printf("%s: OFF\n", domain_name[i]);
        }
    }
    printf("---------------------------------------------\n");
}


void enter_wait_mode(void)
{
    uint32_t retention = 0xFFUL;
    printf("Entering wait mode\n");
    show_power_status(retention);
    printf("Send 'w' to wakeup from the wait mode\n");

    /*
     * Keep PUART clock
     */
    pcfg_set_periph_clock_mode(HPM_PCFG, PCFG_PERIPH_KEEP_CLOCK_ON(pcfg_pmc_periph_uart));
    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    sysctl_clock_set_preset(HPM_SYSCTL, sysctl_preset_0);
    pcfg_disable_power_trap(HPM_PCFG);
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_gate_cpu_clock);
    WFI();
}

void enter_stop_mode(void)
{
    uint32_t retention = 0x1FUL;

    printf("Entering stop mode\n");
    show_power_status(retention);
    printf("Send 'w' to wakeup from the stop mode\n");
    /*
     * Keep PUART clock
     */
    pcfg_set_periph_clock_mode(HPM_PCFG, PCFG_PERIPH_KEEP_CLOCK_ON(pcfg_pmc_periph_uart) | PCFG_PERIPH_KEEP_CLOCK_ON(pcfg_pmc_periph_debug));
    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    sysctl_clear_cpu0_flags(HPM_SYSCTL, cpu_event_flag_mask_all);
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_trigger_system_lp);
    WFI();
}


void enter_standby_mode(void)
{
    uint32_t retention = 0;
    show_power_status(retention);
    printf("Send 'w' to wakeup from the standby mode\n");

    pcfg_enable_power_trap(HPM_PCFG);
    /*
     * Keep PUART clock
     */
    pcfg_set_periph_clock_mode(HPM_PCFG, PCFG_PERIPH_KEEP_CLOCK_ON(pcfg_pmc_periph_uart));
    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    sysctl_set_cpu0_lp_mode(HPM_SYSCTL, cpu_lp_mode_gate_cpu_clock);
    WFI();
}

void enter_shutdown_mode(void)
{
    uint32_t retention = 0;
    printf("Entering shutdown mode\n");
    show_power_status(retention);
    printf("Long press PBUTN or WBUTN to wake up from the shutdown mode\n");
    printf("\nEntering shutdown mode with voltage:%dmV\n", 800);

    bcfg_ldo_set_voltage(HPM_BCFG, 800);
    bpor_set_power_on_cause(HPM_BPOR, bpor_power_on_cause_wbutn);
    sysctl_set_cpu0_lp_retention(HPM_SYSCTL, retention);
    pcfg_set_periph_clock_mode(HPM_PCFG, 0);
    bpor_set_power_down_counter(HPM_BPOR, POWER_DOWN_COUNT);
    WFI();
}
