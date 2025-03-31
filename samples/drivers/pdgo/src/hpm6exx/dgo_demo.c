/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_pdgo_drv.h"

static const uint32_t gpr_value_list[DGO_GPR_WORD_COUNT] = { 0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc };

void dgo_enable_wakeup_src(void)
{
    pdgo_enable_wkup_software_wakeup(HPM_PDGO);
}

void dgo_check_wakeup_src(void)
{
    if (pdgo_is_pin_wakeup(HPM_PDGO)) {
        printf("The System was waken up by Wakeup pin\n");
    }
}

void dgo_check_gpr_retention(void)
{
    if (pdgo_is_retention_mode_enabled(HPM_PDGO)) {
        bool equal = true;
        for (uint32_t i = 0; i < DGO_GPR_WORD_COUNT; i++) {
            if (pdgo_read_gpr(HPM_PDGO, i) != gpr_value_list[i]) {
                equal = false;
                break;
            }
        }
        printf("DGO GPR register values are %s\n", equal ? "as expected" : "not as expected");
    }
}

void dgo_turn_off(uint32_t turnoff_in_us)
{
    uint32_t turnoff_counter = pdgo_get_turnoff_counter_from_us(turnoff_in_us);
    pdgo_set_turnoff_counter(HPM_PDGO, turnoff_counter);
    printf("Actual Turn-off time is %dus, counter is %d\n", pdgo_get_us_from_turnoff_counter(turnoff_counter),
           turnoff_counter);
    printf("Wait until DGO turns off\n");
    while (true) {
    }
}

void dgo_turn_off_sys_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    uint32_t wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter),
           wakeup_counter);
    bool sys_auto_wakeup_enable = pdgo_is_auto_wakeup_enabled(HPM_PDGO);
    if (sys_auto_wakeup_enable) {
        pdgo_enable_oneshot_wakeup(HPM_PDGO);
        /* Note: After switching from auto-wakeup to oneshot wake-up, the IP internal logic requires a while to sync the
         *       wake-up counter logic, Software should wait 3 32K clock cycles before updating the wake-up counter
         */
        clock_cpu_delay_us(32 * 3);
    }
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_sys_auto_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    uint32_t wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter),
           wakeup_counter);
    pdgo_enable_auto_wakeup(HPM_PDGO);
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}

void dgo_gpr_retention_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    pdgo_enable_retention_mode(HPM_PDGO);

    for (uint32_t i = 0; i < DGO_GPR_WORD_COUNT; i++) {
        pdgo_write_gpr(HPM_PDGO, i, gpr_value_list[i]);
    }

    uint32_t wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter),
           wakeup_counter);
    bool sys_auto_wakeup_enable = pdgo_is_auto_wakeup_enabled(HPM_PDGO);
    if (sys_auto_wakeup_enable) {
        pdgo_enable_oneshot_wakeup(HPM_PDGO);
        /* Note: After switching from auto-wakeup to oneshot wake-up, the IP internal logic requires a while to sync the
         *       wake-up counter logic, Software should wait 3 32K clock cycles before updating the wake-up counter
         */
        clock_cpu_delay_us(32 * 3);
    }
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}
