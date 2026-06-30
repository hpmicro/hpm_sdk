/*
 * Copyright (c) 2023-2026 HPMicro
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
#include "dgo_demo.h"

#define TURNOFF_IN_US_DEFAULT   (1000000UL) /* Default Turn-off time: 1 second */
#define WAKEUP_IN_US_DEFAULT    (2000000UL) /* Default Wake-up time: 2 seconds */

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
        "*     e - DGO Turn off, WKUP pin wakeup                               *\n"
        "*     f - DGO Turn off, WUIO pins wakeup                              *\n"
        "*     g - DGO Turn off, PCAP wakeup                                   *\n"
        "*     h - DGO Turn off, PCNT wakeup                                   *\n"
        "*                                                                     *\n"
        "***********************************************************************\n";
    printf("%s", menu_str);
}

ATTR_WEAK void dgo_turn_off_wkup_pin_wakeup(uint32_t turnoff_in_us)
{
    (void)turnoff_in_us;
    printf("DGO Turn off by WKUP pin wakeup is not supported on this SoC\n");
}

ATTR_WEAK void dgo_turn_off_wuio_wakeup(uint32_t turnoff_in_us)
{
    (void)turnoff_in_us;
    printf("DGO Turn off by WUIO pins wakeup is not supported on this SoC\n");
}

ATTR_WEAK void dgo_turn_off_pcap_wakeup(uint32_t turnoff_in_us)
{
    (void)turnoff_in_us;
    printf("DGO Turn off by PCAP wakeup is not supported on this SoC\n");
}

ATTR_WEAK void dgo_turn_off_pcnt_wakeup(uint32_t turnoff_in_us)
{
    (void)turnoff_in_us;
    printf("DGO Turn off by PCNT wakeup is not supported on this SoC\n");
}

int main(void)
{
    board_init();

    dgo_enable_wakeup_src();

    dgo_check_wakeup_src();

    dgo_check_gpr_retention();

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
        case 'e':
            dgo_turn_off_wkup_pin_wakeup(TURNOFF_IN_US_DEFAULT);
            break;
        case 'f':
            dgo_turn_off_wuio_wakeup(TURNOFF_IN_US_DEFAULT);
            break;
        case 'g':
            dgo_turn_off_pcap_wakeup(TURNOFF_IN_US_DEFAULT);
            break;
        case 'h':
            dgo_turn_off_pcnt_wakeup(TURNOFF_IN_US_DEFAULT);
            break;
        default:
            show_menu();
            break;
        }
    }
    return 0;
}
