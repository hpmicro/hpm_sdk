/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef START_TASK_H_
#define START_TASK_H_

#include "includes.h"

#define START_TASK_PRIO 3u
#define START_STK_SIZE 512u

#if !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR)

extern OS_SEM gptmr_sem;

#endif /* !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR) */

extern OS_TCB StartTaskTCB;
extern CPU_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *p_arg);

#endif