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
    uint8_t test[20];
    uint8_t times = 0;
    CPU_SR_ALLOC();
    uint8_t temp;
    (void)p_arg;
    while (1) {
        CPU_CRITICAL_ENTER();
        temp = isr_que.MsgQ.NbrEntriesSize - isr_que.MsgQ.NbrEntries;
        printf("que remain %d.\r\n", temp);
        CPU_CRITICAL_EXIT();
        sprintf((char *)test, "led_send_que %d", times++);
        printf("led0 task send que.\r\n");
        OSQPost((OS_Q *)&isr_que,
            (void *)test,
            (OS_MSG_SIZE)20,
            (OS_OPT)OS_OPT_POST_FIFO,
            (OS_ERR *)&err);

        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}