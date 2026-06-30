/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "hpm_dfu_trigger.h"

int main(void)
{
    board_init();
    printf("DFU UART trigger demo\r\n");
    printf("Type 'dfu' on console UART to reboot into DFU mode\r\n");

    uint8_t state = 0;
    while (1) {
        int ch = getchar();
        if (ch < 0) {
            continue;
        }

        if (state == 0U) {
            state = (ch == 'd') ? 1U : 0U;
        } else if (state == 1U) {
            state = (ch == 'f') ? 2U : ((ch == 'd') ? 1U : 0U);
        } else {
            if (ch == 'u') {
                hpm_dfu_reboot_to_dfu();
            }
            state = (ch == 'd') ? 1U : 0U;
        }
    }
}
