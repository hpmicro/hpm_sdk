/*
 * Copyright (c) 2024 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "rtt_port.h"
#include <rtthread.h>
#include "rvbacktrace.h"

#define THREAD_PRIORITY      20
#define THREAD_STACK_SIZE    1024
#define THREAD_TIMESLICE     5
#define EVENT_FLAG          (1 << 4)

static struct rt_event event;
static rt_thread_t tid0 = RT_NULL;
static rt_thread_t tid1 = RT_NULL;

void rt_hw_board_init(void)
{
    board_init();
    rtt_base_init();
}

static void thread0_entry(void *param)
{
    (void)param;
    rt_base_t status;
    rt_uint32_t e;

    for (;;) {
        status = rt_event_recv(&event, EVENT_FLAG,
                               RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                               RT_WAITING_FOREVER, &e);
        if ((status != RT_EOK) || (e != EVENT_FLAG))
            break;
    }
}

static void thread1_entry(void *param)
{
    (void)param;

    for (;;) {
        rt_event_send(&event, EVENT_FLAG);
        rt_thread_mdelay(50);
    }
}

/* run in main thread */
int main(void)
{
    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
    tid0 = rt_thread_create("event_rec", thread0_entry, RT_NULL,
                    THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(tid0);
    tid1 = rt_thread_create("event_send", thread1_entry, RT_NULL,
                    THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(tid1);
    rvbacktrace();
    return 0;
}
