/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "led_task.h"
#include "board.h"
#include "task.h"

OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    while (1) {
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        printf("led0 task task sem.\r\n");
        OSTaskSemPost(&FloatTaskTCB, OS_OPT_POST_NONE, &err);
    }
}