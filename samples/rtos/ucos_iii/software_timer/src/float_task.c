/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "float_task.h"
#include "led_task.h"
#include "stdint.h"

OS_TCB FloatTaskTCB;
CPU_STK FLOAT_TASK_STK[FLOAT_STK_SIZE];

void float_task(void *p_arg)
{
    OS_ERR err;
    uint32_t temp = 0;
    uint8_t i = 0;
    (void)p_arg;
    CPU_SR_ALLOC();
    static float float_num = 0.01;
    while (1) {
        float_num += 0.01f;
        CPU_CRITICAL_ENTER();
        printf("float run %d times\r\n", temp++);
        CPU_CRITICAL_EXIT();
        for (i = 0; i < 5; i++)
            printf("float : %f\r\n", float_num);
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}