/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/***********************************************************************************************************************
 *
 * This example demonstrate the usage of the
 *  - PDGO turn-off
 *  - PDGO wake-up
 *  - PDGO retention mode
 *
 *  The PDGO can turn off the System power (except the PDGO module) in specified counter\
 *  The PDGO support wake up the System via the Wake-up counter
 *  The PDGO support keep the GPR register values in DGO turn-off mode
 *
 *
 **********************************************************************************************************************/

#include <stdio.h>
#include "board.h"
#include "hpm_pdgo_drv.h"

#define TURNOFF_IN_US_DEFAULT   (1000000UL) /* Default Turn-off time: 1 second */
#define WAKEUP_IN_US_DEFAULT    (2000000UL) /* Default Wake-up time: 2 seconds */

#define TEST_GPR_VALUES  { 0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc }

/* Turn off Power (VPMCCAP, VDD_SOC) */
void dgo_turn_off(uint32_t turnoff_in_us);

/* Turn off Power and Wake-up by system only once */
void dgo_turn_off_sys_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us);

/* Turn off Power and Wake-up by system automatically */
void dgo_turn_off_sys_auto_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us);

/* Turn off Power and keep GPR register values then wake-up */
void dgo_gpr_retention_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us);

void show_menu(void);


int main(void)
{
    board_init();

    if (pdgo_is_pin_wakeup(HPM_PDGO)) {
        printf("The System was waken up by Wakeup pin\n");
    }

    if (pdgo_is_retention_mode_enabled(HPM_PDGO)) {
        static const uint32_t expected_gpr_values[] = TEST_GPR_VALUES;
        bool equal = true;
        for (uint32_t i = 0; i < DGO_GPR_WORD_COUNT; i++) {
            if (pdgo_read_gpr(HPM_PDGO, i) != expected_gpr_values[i]) {
                equal = false;
                break;
            }
        }
        printf("DGO GPR register values are %s\n", equal ? "as expected" : "not as expected");
    }
    show_menu();

    while (1) {
        char ch = getchar();
        putchar(ch);
        putchar('\n');
        switch (ch) {
        case 'a':
            /* Turn-off the DGO power */
            dgo_turn_off(TURNOFF_IN_US_DEFAULT);
            break;
        case 'b':
            dgo_turn_off_sys_oneshot_wakeup(TURNOFF_IN_US_DEFAULT, WAKEUP_IN_US_DEFAULT);
            break;
        case 'c':
            dgo_turn_off_sys_auto_wakeup(TURNOFF_IN_US_DEFAULT, WAKEUP_IN_US_DEFAULT);
            break;
        case 'd':
            dgo_gpr_retention_oneshot_wakeup(TURNOFF_IN_US_DEFAULT, WAKEUP_IN_US_DEFAULT);
            break;
        default:
            show_menu();
            break;
        }
    }
    return 0;
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
    uint32_t gpr_value_list[4] = TEST_GPR_VALUES;
    pdgo_enable_retention_mode(HPM_PDGO);

    for (uint32_t i = 0; i < ARRAY_SIZE(gpr_value_list); i++) {
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


void show_menu(void)
{
    static const char menu_str[] =
        "\n"
        "***********************************************************************\n"
        "*                                                                     *\n"
        "*     DGO Turn-off and Wake-up test                                   *\n"
        "*                                                                     *\n"
        "*     a - DGO Turn off                                                *\n"
        "*     b - DGO Turn off, System one-shot wakeup                        *\n"
        "*     c - DGO Turn off, System auto wakeup                            *\n"
        "*         Note: After wake-up, use option 'a' to test auto wake-up    *\n"
        "*     d - DGO GPR retention, System one-shot wakeup                   *\n"
        "*                                                                     *\n"
        "***********************************************************************\n";
    printf("%s", menu_str);
}