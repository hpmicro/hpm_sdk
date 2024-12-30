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
    CPU_INT08U *buf;
    (void)p_arg;
    while (1) {
        buf = OSMemGet((OS_MEM *)&my_mem,
            (OS_ERR *)&err);
        if (err == OS_ERR_NONE) {
            printf("led0 task get mem\r\n");
        }

        if (err == OS_ERR_MEM_NO_FREE_BLKS) {
            printf("No Free Mem\r\n");
        }

        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        OSMemPut((OS_MEM *)&my_mem,
            (void *)buf,
            (OS_ERR *)&err);
        printf("buf:%#x\r\n", (uint32_t)(buf));
        OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}