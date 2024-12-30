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
        OSSemPend(&sem_share_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
        printf("led0 task use share_data, prio : %d\r\n", LED0_TASK_PRIO);
        sprintf((char *)share_data, "led0 task write.\r\n");
        OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);

        printf("led0 task : %s", share_data);
        OSSemPost(&sem_share_data, OS_OPT_POST_1, &err);
        OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}