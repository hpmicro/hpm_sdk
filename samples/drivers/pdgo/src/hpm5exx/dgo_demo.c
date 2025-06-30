/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_pdgo_drv.h"

static const uint32_t gpr_value_list[DGO_GPR_WORD_COUNT] = { 0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc, 0x12345678, 0x5AA555AA, 0x98765432, 0xFF0000FF };

void dgo_enable_wakeup_src(void)
{
    pdgo_enable_wakeup_by_bit_mask(HPM_PDGO, dgo_wakeup_wkup_pin | dgo_wakeup_software);
}

void dgo_check_wakeup_src(void)
{
    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_wkup_pin) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_wkup_pin);
        printf("The System was waken up by Wakeup pin\n");
    }

    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_software) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_software);
        printf("The System was waken up by software\n");
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
    pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_wkup_pin | dgo_wakeup_software);
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
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_sys_auto_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    uint32_t wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter),
           wakeup_counter);
    pdgo_set_cycle_wakeup_counter(HPM_PDGO, wakeup_counter);
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
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}
