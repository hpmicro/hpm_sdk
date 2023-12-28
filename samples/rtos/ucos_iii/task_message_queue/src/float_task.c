/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "float_task.h"
#include "led_task.h"
#include "task.h"

OS_TCB FloatTaskTCB;
CPU_STK FLOAT_TASK_STK[FLOAT_STK_SIZE];

void float_task(void *p_arg)
{
    OS_ERR err;
    OS_MSG_SIZE size;
    char *p;
    (void)p_arg;
    while (1) {
        p = OSTaskQPend((OS_TICK)0,
            (OS_OPT)OS_OPT_PEND_BLOCKING,
            (OS_MSG_SIZE *)&size,
            (CPU_TS *)0,
            (OS_ERR *)&err);

        printf("float task get que.\r\n");
        printf("que: %s.\r\n", p);

        OSTimeDlyHMSM(0, 0, 0, 800, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}