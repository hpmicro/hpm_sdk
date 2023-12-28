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
    (void)p_arg;
    CPU_SR_ALLOC();
    static float float_num = 0.01;
    while (1) {
        float_num += 0.01f;
        CPU_CRITICAL_ENTER();
        printf("float_num: %.4f\r\n", float_num);
        CPU_CRITICAL_EXIT();
        if (float_num > 0.101f && float_num < 0.119f) {
            OSTaskDel((OS_TCB *)&Led0TaskTCB, &err);
            printf("delete led task\r\n");
        }
        if (float_num > 0.291f && float_num < 0.301f) {
            CPU_CRITICAL_ENTER();
            OSTaskCreate((OS_TCB *)&Led0TaskTCB,
                (CPU_CHAR *)"led0 task",
                (OS_TASK_PTR)led0_task,
                (void *)0,
                (OS_PRIO)LED0_TASK_PRIO,
                (CPU_STK *)&LED0_TASK_STK[0],
                (CPU_STK_SIZE)LED0_STK_SIZE / 10,
                (CPU_STK_SIZE)LED0_STK_SIZE,
                (OS_MSG_QTY)0,
                (OS_TICK)0,
                (void *)0,
                (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                (OS_ERR *)&err);
            printf("create led task\r\n");
            CPU_CRITICAL_EXIT();
            float_num = 0.0f;
        }
        OSTimeDlyHMSM(0, 0, 0, 300, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}