/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _FLOAT_TASK_H_
#define _FLOAT_TASK_H_

#include "includes.h"

#define FLOAT_TASK_PRIO 6
#define FLOAT_STK_SIZE 4096
extern OS_TCB FloatTaskTCB;
extern CPU_STK FLOAT_TASK_STK[FLOAT_STK_SIZE];

void float_task(void *p_arg);
#endif