/*
 * Copyright (c) 2023 HPMicro
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

extern OS_TCB Led0TaskTCB;
extern CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

extern OS_TCB PrintTaskTCB;
extern CPU_STK PRINT_TASK_STK[PRINT_STK_SIZE];

void led0_task(void *p_arg);

void print_task(void *p_arg);

#endif