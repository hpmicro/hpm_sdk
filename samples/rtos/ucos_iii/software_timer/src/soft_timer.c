/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "soft_timer.h"
#include "stdint.h"

OS_TMR tmr1;
OS_TMR tmr2;

void tmr1_callback(void *p_tmr, void *p_arg)
{
    OS_ERR err;
    static uint8_t flag = 0;
    (void)p_tmr;
    CPU_SR_ALLOC();
    (void)p_arg;
    flag++;

    if ((flag % 2) == 0) {
        printf("tmr1 del float task\r\n");
        OSTaskDel((OS_TCB *)&FloatTaskTCB, &err);
        return;
    }
    printf("tmr1 create float task\r\n");
    CPU_CRITICAL_ENTER();

    OSTaskCreate((OS_TCB *)&FloatTaskTCB,
        (CPU_CHAR *)"float test task",
        (OS_TASK_PTR)float_task,
        (void *)0,
        (OS_PRIO)FLOAT_TASK_PRIO,
        (CPU_STK *)&FLOAT_TASK_STK[0],
        (CPU_STK_SIZE)FLOAT_STK_SIZE / 10,
        (CPU_STK_SIZE)FLOAT_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)0,
        (void *)0,
        (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
        (OS_ERR *)&err);
    CPU_CRITICAL_EXIT();
}

void tmr2_callback(void *p_tmr, void *p_arg)
{
    OS_ERR err;
    (void)p_tmr;
    (void)p_arg;
    CPU_SR_ALLOC();

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

    CPU_CRITICAL_EXIT();
    printf("tmr2 create led task\r\n");
}