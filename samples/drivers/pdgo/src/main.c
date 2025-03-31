/*
 * Copyright (c) 2023-2024 HPMicro
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
        "*                                                                     *\n"
        "***********************************************************************\n";
    printf("%s", menu_str);
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
        default:
            show_menu();
            break;
        }
    }
    return 0;
}
