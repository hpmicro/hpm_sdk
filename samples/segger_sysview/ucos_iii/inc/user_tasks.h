/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef USER_TASKS_H_
#define USER_TASKS_H_

#include "includes.h"

#define LED0_TASK_PRIO 4u
#define LED0_STK_SIZE 4096u

#define PRINT_TASK_PRIO 2u
#define PRINT_STK_SIZE 4096u

#if !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR)

#define GPTMR_TASK_PRIO 6u
#define GPTMR_STK_SIZE 4096u

#endif /* !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR) */

extern OS_TCB Led0TaskTCB;
extern CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

extern OS_TCB PrintTaskTCB;
extern CPU_STK PRINT_TASK_STK[PRINT_STK_SIZE];

#if !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR)

extern OS_TCB GptmrTaskTCB;
extern CPU_STK GPTMR_TASK_STK[GPTMR_STK_SIZE];

#endif /* !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR) */

void led0_task(void *p_arg);

void print_task(void *p_arg);

#if !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR)

void gptmr_task(void *p_arg);

#endif /* !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR) */

#endif