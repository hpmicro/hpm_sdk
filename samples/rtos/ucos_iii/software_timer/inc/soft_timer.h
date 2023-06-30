/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _MY_TIME_H
#define _MY_TIME_H


#include "includes.h"
#include "task.h"
#include "led_task.h"

extern OS_TMR tmr1;
extern OS_TMR tmr2;

void tmr1_callback(void *p_tmr, void *p_arg);
void tmr2_callback(void *p_tmr, void *p_arg);

#endif
