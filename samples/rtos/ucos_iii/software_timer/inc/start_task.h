/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef START_TASK_H_
#define START_TASK_H_

#include "includes.h"
#include "float_task.h"

#define START_TASK_PRIO		3u
#define START_STK_SIZE 		512u
extern OS_TCB StartTaskTCB;
extern CPU_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *p_arg);

#endif
