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
    (void)p_arg;

    OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);

    printf("This thread will print 1 without mutex protection, and will print 2 within mutex protection.\n");

    while (1) {
        OSTimeDlyHMSM(0, 0, 0, 400, OS_OPT_TIME_HMSM_STRICT, &err);
        printf("\r\n11111111111111111111111111111\r\n");
        OSMutexPend(&Mutex_Sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
        printf("\r\n22222222222222222222222222222\r\n");
        OSMutexPost(&Mutex_Sem, OS_OPT_POST_NO_SCHED, &err);
        OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}