/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _TASK_H_
#define _TASK_H_

#include "start_task.h"
#include "stdint.h"

#define MY_MEM_NUM  5
#define MY_MEMBLOCK_SIZE  100
extern OS_SEM SYNC_SEM;
extern OS_Q isr_que;
extern OS_MEM  my_mem;
#endif
