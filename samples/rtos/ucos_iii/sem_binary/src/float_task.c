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
    while (1) {
        OSSemPend(&sem_share_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
        printf("float task use share_data, prio : %d\r\n", FLOAT_TASK_PRIO);

        sprintf((char *)share_data, "float task write.\r\n");
        OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);

        printf("float task : %s", share_data);
        OSSemPost(&sem_share_data, OS_OPT_POST_1, &err);
        OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}