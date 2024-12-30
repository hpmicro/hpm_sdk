/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "start_task.h"
#include "led_task.h"
#include "soft_timer.h"

OS_TCB StartTaskTCB;
CPU_STK START_TASK_STK[START_STK_SIZE];

void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    (void)p_arg;

    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN
    OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err);
#endif

    CPU_CRITICAL_ENTER();

    OSTmrCreate((OS_TMR *)&tmr1,
        (CPU_CHAR *)"tmr1",
        (OS_TICK)20,
        (OS_TICK)100,
        (OS_OPT)OS_OPT_TMR_PERIODIC,
        (OS_TMR_CALLBACK_PTR)tmr1_callback,
        (void *)0,
        (OS_ERR *)&err);

    OSTmrCreate((OS_TMR *)&tmr2,
        (CPU_CHAR *)"tmr2",
        (OS_TICK)200,
        (OS_TICK)0,
        (OS_OPT)OS_OPT_TMR_ONE_SHOT,
        (OS_TMR_CALLBACK_PTR)tmr2_callback,
        (void *)0,
        (OS_ERR *)&err);

    CPU_CRITICAL_EXIT();
    OSTmrStart(&tmr1, &err);
    OSTmrStart(&tmr2, &err);
}