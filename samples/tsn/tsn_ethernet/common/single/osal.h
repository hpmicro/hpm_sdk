/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef OSAL_H
#define OSAL_H

#if defined(__ENABLE_FREERTOS) && (__ENABLE_FREERTOS)
typedef SemaphoreHandle_t osSemaphoreId_t;
typedef TimerHandle_t     osTimerId_t;
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#include "rtthread.h"

typedef rt_sem_t   osSemaphoreId_t;
typedef rt_timer_t osTimerId_t;

#endif


#endif
