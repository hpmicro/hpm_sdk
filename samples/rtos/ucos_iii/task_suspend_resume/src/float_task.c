/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "float_task.h"
#include "led_task.h"

OS_TCB FloatTaskTCB;
CPU_STK FLOAT_TASK_STK[FLOAT_STK_SIZE];

void float_task(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    static float float_num = 0.01;
    (void)p_arg;
    while (1) {
        float_num += 0.01f;
        CPU_CRITICAL_ENTER();
        printf("float_num: %.4f\r\n", float_num);
        CPU_CRITICAL_EXIT();
        if (float_num > 0.101f && float_num < 0.119f) {
            OSTaskSuspend((OS_TCB *)&Led0TaskTCB, &err);
            printf("suspend task led\r\n");
        }
        if (float_num > 0.291f && float_num < 0.301f) {
            OSTaskResume((OS_TCB *)&Led0TaskTCB, &err);
            printf("resume task led\r\n");
            float_num = 0.0f;
        }
        OSTimeDlyHMSM(0, 0, 0, 300, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}