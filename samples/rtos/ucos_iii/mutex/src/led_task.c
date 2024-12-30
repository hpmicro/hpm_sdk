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

    printf("This thread will print 3 without mutex protection, and will print 4 within mutex protection.\n");

    OSTimeDlyHMSM(0, 0, 3, 0, OS_OPT_TIME_HMSM_STRICT, &err);

    while (1) {
        printf("\r\n3333333333333333333333333333\r\n");
        OSMutexPend(&Mutex_Sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        printf("\r\n4444444444444444444444444444\r\n");
        OSMutexPost(&Mutex_Sem, OS_OPT_POST_NO_SCHED, &err);
    }
}