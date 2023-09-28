/*********************************************************************
 *                    SEGGER Microcontroller GmbH                     *
 *       Solutions for real time microcontroller applications         *
 **********************************************************************
 *                                                                    *
 *            (c) 1995 - 2018 SEGGER Microcontroller GmbH             *
 *                                                                    *
 *       www.segger.com     Support: support@segger.com               *
 *                                                                    *
 **********************************************************************
 */
/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "SEGGER_RTT.h"

#define LED_FLASH_PERIOD_IN_MS 300
ATTR_PLACE_AT_NONCACHEABLE_BSS static char r;

int main(void)
{
    board_init_pmp();
    board_init_clock();
    board_init_led_pins();
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    do {
        r = SEGGER_RTT_WaitKey();
        SEGGER_RTT_Write(0, &r, 1);
        r++;
    } while (1);
    return 0;
}
