/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "led_task.h"
#include "board.h"

OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *p_arg)
{
    unsigned int task_loop = 0;
    OS_ERR err;
    (void)p_arg;
    while (1) {
        task_loop++;
        printf("%s loop for %d times\n", __func__, task_loop);
        board_led_toggle();
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}