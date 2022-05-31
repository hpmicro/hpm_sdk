/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_butn_drv.h"
#include "hpm_bpor_drv.h"

#define TEST_BPOR HPM_BPOR
#define TEST_BPOR_POWER_ON_CAUSE bpor_power_on_cause_wbutn

int main(void)
{
    uint8_t u;
    board_init();
    init_butn_pins();

    bpor_select_power_on_cause(TEST_BPOR, TEST_BPOR_POWER_ON_CAUSE);

    printf("Please press PBUTN 16s to enter power down mode, then press PBUTN or WBUTN 0.5s to wake up from power down mode.\n");
    while(1)
    {
        u = console_receive_byte();
        printf("%c", u);
    }
    return 0;
}
