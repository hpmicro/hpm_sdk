/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LED_TASK_H_
#define LED_TASK_H_

#include "includes.h"

#define LED0_TASK_PRIO		4u
#define LED0_STK_SIZE 		4096u
extern OS_TCB Led0TaskTCB;
extern CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *p_arg);

#endif
