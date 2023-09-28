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
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile int _Cnt;

int main(void)
{
    int r;
    int CancelOp;
    board_init_pmp();
    board_init_clock();
    board_init_led_pins();
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    do {
        _Cnt = 0;

        SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
        SEGGER_RTT_WriteString(0, "Press <1> to continue in blocking mode (Application waits if necessary, no data lost)\r\n");
        SEGGER_RTT_WriteString(0, "Press <2> to continue in non-blocking mode (Application does not wait, data lost if fifo full)\r\n");
        do {
            r = SEGGER_RTT_WaitKey();
        } while ((r != '1') && (r != '2'));
        if (r == '1') {
            SEGGER_RTT_WriteString(0, "\r\nSelected <1>. Configuring RTT and starting...\r\n");
            SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
        } else {
            SEGGER_RTT_WriteString(0, "\r\nSelected <2>. Configuring RTT and starting...\r\n");
            SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        }
        CancelOp = 0;
        do {
            SEGGER_RTT_printf(0, "Count: %d. Press <Space> to get back to menu.\r\n", _Cnt++);
            r = SEGGER_RTT_HasKey();
            if (r) {
                CancelOp = (SEGGER_RTT_GetKey() == ' ') ? 1 : 0;
            }
            if (CancelOp) {
                SEGGER_RTT_WriteString(0, "Operation cancelled, going back to menu...\r\n");
                break;
            }
        } while (1);
        SEGGER_RTT_GetKey();
        SEGGER_RTT_WriteString(0, "\r\n");
    } while (1);
    return 0;
}

