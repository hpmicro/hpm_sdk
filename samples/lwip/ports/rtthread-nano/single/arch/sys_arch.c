/*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 *
 */

/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* lwIP includes. */
#include "common.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "sys_arch.h"

#include "rtthread.h"
#include "rthw.h"

void sys_init(void)
{
}

u32_t sys_now(void)
{
   return rt_tick_get() * (1000u / RT_TICK_PER_SECOND);
}

u32_t sys_jiffies(void)
{
  return rt_tick_get();
}

err_t sys_mutex_new(sys_mutex_t *mutex)
{
    static unsigned short counter = 0;
    char tname[RT_NAME_MAX];
    sys_mutex_t tmpmutex;

    RT_DEBUG_NOT_IN_INTERRUPT;

    rt_snprintf(tname, RT_NAME_MAX, "%s%d", SYS_LWIP_MUTEX_NAME, counter);
    counter++;;

    tmpmutex = rt_mutex_create(tname, RT_IPC_FLAG_PRIO);
    if (tmpmutex == RT_NULL) {
        return ERR_MEM;
    } else {
        *mutex = tmpmutex;
        return ERR_OK;
    }
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    RT_DEBUG_NOT_IN_INTERRUPT;
    rt_mutex_take(*mutex, RT_WAITING_FOREVER);
    return;
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    rt_mutex_release(*mutex);
}

err_t sys_sem_new(sys_sem_t *sem, u8_t initial_count)
{
    static unsigned short counter = 0;
    char tname[RT_NAME_MAX];
    sys_sem_t tmpsem;

    RT_DEBUG_NOT_IN_INTERRUPT;

    rt_snprintf(tname, RT_NAME_MAX, "%s%d", SYS_LWIP_SEM_NAME, counter);
    counter++;;

    tmpsem = rt_sem_create(tname, initial_count, RT_IPC_FLAG_FIFO);
    if (tmpsem == RT_NULL) {
        return ERR_MEM;
    } else {
        *sem = tmpsem;
        return ERR_OK;
    }
}

void sys_sem_signal(sys_sem_t *sem)
{
  rt_sem_release(*sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    rt_err_t ret;
    s32_t t;
    u32_t tick;

    RT_DEBUG_NOT_IN_INTERRUPT;

    /* get the begin tick */
    tick = rt_tick_get();
    if (timeout == 0) {
        t = RT_WAITING_FOREVER;
    } else {
        /* convert msecond to os tick */
        if (timeout < (1000 / RT_TICK_PER_SECOND))
            t = 1;
        else
            t = timeout / (1000 / RT_TICK_PER_SECOND);
    }

    ret = rt_sem_take(*sem, t);

    if (ret == -RT_ETIMEOUT) {
        return SYS_ARCH_TIMEOUT;
    }

    /* get elapse msecond */
    tick = rt_tick_get() - tick;

    /* convert tick to msecond */
    tick = tick * (1000 / RT_TICK_PER_SECOND);
    if (tick == 0)
        tick = 1;

    return tick;
}

int sys_sem_valid(sys_sem_t *sem)
{
    int ret = 0;

    if (*sem) {
        ret = 1;
    }

    return ret;
}

void sys_sem_free(sys_sem_t *sem)
{
    RT_DEBUG_NOT_IN_INTERRUPT;
    rt_sem_delete(*sem);
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = RT_NULL;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    static unsigned short counter = 0;
    char tname[RT_NAME_MAX];
    sys_mbox_t tmpmbox;

    RT_DEBUG_NOT_IN_INTERRUPT;

    rt_snprintf(tname, RT_NAME_MAX, "%s%d", SYS_LWIP_MBOX_NAME, counter);
    counter++;;

    tmpmbox = rt_mb_create(tname, size, RT_IPC_FLAG_FIFO);
    if (tmpmbox != RT_NULL) {
        *mbox = tmpmbox;
        return ERR_OK;
    }

    return ERR_MEM;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    RT_DEBUG_NOT_IN_INTERRUPT;
    rt_mb_send_wait(*mbox, (rt_ubase_t)msg, RT_WAITING_FOREVER);
    return;
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (rt_mb_send(*mbox, (rt_ubase_t)msg) == RT_EOK) {
        return ERR_OK;
    }

    return ERR_MEM;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
    return sys_mbox_trypost(mbox, msg);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    rt_err_t ret;
    s32_t t;
    u32_t tick;

    RT_DEBUG_NOT_IN_INTERRUPT;

    /* get the begin tick */
    tick = rt_tick_get();

    if (timeout == 0) {
        t = RT_WAITING_FOREVER;
    } else {
        /* convirt msecond to os tick */
        if (timeout < (1000 / RT_TICK_PER_SECOND))
            t = 1;
        else
            t = timeout / (1000 / RT_TICK_PER_SECOND);
    }

    ret = rt_mb_recv(*mbox, (rt_ubase_t *)msg, t);
    if (ret != RT_EOK) {
        return SYS_ARCH_TIMEOUT;
    }

    /* get elapse msecond */
    tick = rt_tick_get() - tick;

    /* convert tick to msecond */
    tick = tick * (1000 / RT_TICK_PER_SECOND);
    if (tick == 0)
        tick = 1;

    return tick;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    int ret;

    ret = rt_mb_recv(*mbox, (rt_ubase_t *)msg, 0);
    if (ret == -RT_ETIMEOUT) {
        return SYS_ARCH_TIMEOUT;
    } else {
        if (ret == RT_EOK)
            ret = 0;
    }

    return ret;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    RT_DEBUG_NOT_IN_INTERRUPT;
    rt_mb_delete(*mbox);
    return;
}

 void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = RT_NULL;
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    int ret = 0;

    if (*mbox) {
        ret = 1;
    }

    return ret;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    rt_thread_t t;

    RT_DEBUG_NOT_IN_INTERRUPT;

    /* create thread */
    t = rt_thread_create(name, thread, arg, stacksize, prio, 100);
    RT_ASSERT(t != RT_NULL);

    /* startup thread */
    rt_thread_startup(t);

    return t;
}

sys_prot_t sys_arch_protect(void)
{
    rt_base_t level;
    level = rt_hw_interrupt_disable();
    return level;
}

void sys_arch_unprotect(sys_prot_t pval)
{
    rt_hw_interrupt_enable(pval);
}
