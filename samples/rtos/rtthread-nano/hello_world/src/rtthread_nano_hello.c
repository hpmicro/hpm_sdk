/*
 * Copyright (c) 2023 hpmicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "rtt_port.h"
#include <stdlib.h>
#include <rtthread.h>
#include <time.h>

#define THREAD_PRIORITY      20
#define THREAD_STACK_SIZE    1024
#define THREAD_TIMESLICE     5
#define EVENT_FLAG          (1 << 4)
#define BLOCK_COUNT          48

#define THREAD(x)            tid##x
#define COUNTER(x)           tid##x##_counter

#define TIMER_TIMEOUT_CNT    500

#define CMD_EVENT            0
#define CMD_EVENTKILL        1
#define CMD_MBOX             2
#define CMD_MBOXKILL         3
#define CMD_SEMPHORE         4
#define CMD_SEMKILL          5
#define CMD_MEMPOOL          6
#define CMD_POOLKILL         7

static const char mb_str[] = "Hello world";

static rt_timer_t timer = RT_NULL;
static struct rt_event event;
static struct rt_mailbox mb;
static char mb_pool[64];
static rt_sem_t sem = RT_NULL;
static rt_uint8_t *ptr[BLOCK_COUNT];
static rt_uint8_t mempool[4096];
static struct rt_mempool mp;

static rt_thread_t tid0 = RT_NULL;
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;
static rt_thread_t tid4 = RT_NULL;
static rt_thread_t tid5 = RT_NULL;
static rt_thread_t tid6 = RT_NULL;
static rt_thread_t tid7 = RT_NULL;

uint32_t tid0_counter;
uint32_t tid1_counter;
uint32_t tid2_counter;
uint32_t tid3_counter;
uint32_t tid4_counter;
uint32_t tid5_counter;
uint32_t tid6_counter;
uint32_t tid7_counter;


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
        tid0_counter++;
        if ((status != RT_EOK) || (e != EVENT_FLAG))
            break;
    }
}

static void thread1_entry(void *param)
{
    (void)param;

    for (;;) {
        rt_event_send(&event, EVENT_FLAG);
        tid1_counter++;
        rt_thread_mdelay(50);
    }
}

static void thread2_entry(void *param)
{
    (void)param;
    unsigned char *str;

    for (;;) {
        if (rt_mb_recv(&mb, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK) {
            tid2_counter++;
            rt_kprintf("thread2: get a mail from mailbox, the content:%s\n", str);
            rt_thread_delay(10);
        }
    }
}

static void thread3_entry(void *param)
{
    (void)param;

    for (;;) {
        tid3_counter++;
        rt_mb_send(&mb, (rt_uint32_t)&mb_str[0]);
        rt_thread_mdelay(1000);
    }
}

static void thread4_entry(void *param)
{
    (void)param;

    for (;;) {
        rt_sem_take(sem, RT_WAITING_FOREVER);
        tid4_counter++;
        rt_thread_mdelay(30);
        rt_sem_release(sem);
    }
}

static void thread5_entry(void *param)
{
    (void)param;

    for (;;) {
        rt_sem_take(sem, RT_WAITING_FOREVER);
        tid5_counter++;
        rt_thread_mdelay(100);
        rt_sem_release(sem);
    }
}

static void thread6_entry(void *param)
{
    (void)param;
    int i;
    for (i = 0; i < BLOCK_COUNT; i++) {
        if (ptr[i] == RT_NULL) {
            rt_kprintf("allocate No.%d\n", i);
            ptr[i] = rt_mp_alloc(&mp, RT_WAITING_FOREVER);
        }
    }

    for (;;) {
        rt_mp_alloc(&mp, RT_WAITING_FOREVER);
        tid6_counter++;
    }
}

static void thread7_entry(void *param)
{
    (void)param;
    int i;
    for (;;) {
        for (i = 0 ; i < BLOCK_COUNT; i++) {
            if (ptr[i] != RT_NULL) {
                rt_mp_free(ptr[i]);
                tid7_counter++;
            }
        }
        rt_thread_mdelay(100);
    }
}

static void event_case(void)
{
    if (tid0 != RT_NULL && tid1 != RT_NULL)
        return;

    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
    tid0 = rt_thread_create("event_rec", thread0_entry, RT_NULL,
                    THREAD_STACK_SIZE, 25, THREAD_TIMESLICE);
    rt_thread_startup(tid0);
    tid1 = rt_thread_create("event_send", thread1_entry, RT_NULL,
                    THREAD_STACK_SIZE, 25, THREAD_TIMESLICE);
    rt_thread_startup(tid1);
}

static void mbox_case(void)
{
    if (tid2 != RT_NULL && tid3 != RT_NULL)
        return;

    rt_mb_init(&mb, "mbt", &mb_pool[0],
               sizeof(mb_pool) / 4, RT_IPC_FLAG_FIFO);
    tid2 = rt_thread_create("mbox_rec", thread2_entry, RT_NULL,
                            THREAD_STACK_SIZE, 25, THREAD_TIMESLICE);
    rt_thread_startup(tid2);

    tid3 = rt_thread_create("mbox_send", thread3_entry, RT_NULL,
                            THREAD_STACK_SIZE, 25, THREAD_TIMESLICE);
    rt_thread_startup(tid3);
}

static void semphore_case(void)
{
    if (tid4 != RT_NULL && tid5 != RT_NULL)
        return;

    sem = rt_sem_create("sem", 1, RT_IPC_FLAG_FIFO);
    tid4 = rt_thread_create("sem1", thread4_entry, RT_NULL,
                           THREAD_STACK_SIZE, 26, THREAD_TIMESLICE);
    rt_thread_startup(tid4);

    tid5 = rt_thread_create("sem2", thread5_entry, RT_NULL,
                              THREAD_STACK_SIZE, 26, THREAD_TIMESLICE);
    rt_thread_startup(tid5);
}

static void mempool_case(void)
{
    if (tid6 != RT_NULL && tid7 != RT_NULL)
        return;

    int i;
    for (i = 0; i < BLOCK_COUNT; i++)
        ptr[i] = RT_NULL;

    rt_mp_init(&mp, "mp1", &mempool[0], sizeof(mempool), 80);
    tid6 = rt_thread_create("malloc", thread6_entry, RT_NULL,
                            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(tid6);

    tid7 = rt_thread_create("mfree", thread7_entry, RT_NULL,
                            THREAD_STACK_SIZE, THREAD_PRIORITY + 1, THREAD_TIMESLICE);
    rt_thread_startup(tid7);
}

static void event_case_kill(void)
{
    if (tid0 == RT_NULL || tid1 == RT_NULL)
        return;

    if (tid0 != RT_NULL && tid0->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid0);
        tid0 = RT_NULL;
        tid0_counter = 0;
    }

    if (tid1 != RT_NULL && tid1->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid1);
        tid1 = RT_NULL;
        tid1_counter = 0;
    }
    rt_event_detach(&event);
}

static void mbox_case_kill(void)
{
    if (tid2 == RT_NULL || tid3 == RT_NULL)
        return;

    if (tid2 != RT_NULL && tid2->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid2);
        tid2 = RT_NULL;
        tid2_counter = 0;
    }

    if (tid3 != RT_NULL && tid3->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid3);
        tid3 = RT_NULL;
        tid3_counter = 0;
    }
    rt_mb_detach(&mb);
}

static void semphore_case_kill(void)
{
    if (tid4 == RT_NULL || tid5 == RT_NULL)
        return;

    if (tid4 != RT_NULL && tid4->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid4);
        tid4 = RT_NULL;
        tid4_counter = 0;
    }

    if (tid5 != RT_NULL && tid5->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid5);
        tid5 = RT_NULL;
        tid5_counter = 0;
    }
    rt_sem_delete(sem);
}

static void mempool_case_kill(void)
{
    if (tid6 == RT_NULL || tid7 == RT_NULL)
        return;

    if (tid6 != RT_NULL && tid6->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid6);
        tid6 = RT_NULL;
        tid6_counter = 0;
    }

    if (tid7 != RT_NULL && tid7->stat != RT_THREAD_CLOSE) {
        rt_thread_delete(tid7);
        tid7 = RT_NULL;
        tid7_counter = 0;
    }
    rt_mp_detach(&mp);
}

static void timer_entry(void *param)
{
    (void)param;

    rt_kprintf("\n       rtthread basic function demo    \n----------------------------------------\n");
    rt_kprintf("    thread0 event receive:        %u\n", tid0_counter);
    rt_kprintf("    thread1 event send:           %u\n", tid1_counter);
    rt_kprintf("    thread2 mbox receive:         %u\n", tid2_counter);
    rt_kprintf("    thread3 mbox send:            %u\n", tid3_counter);
    rt_kprintf("    thread4 semphore task 1:      %u\n", tid4_counter);
    rt_kprintf("    thread5 semphore task 2:      %u\n", tid5_counter);
    rt_kprintf("    thread6 malloc:               %u\n", tid6_counter);
    rt_kprintf("    thread7 free:                 %u\n", tid7_counter);

    rt_kprintf("----------------------------------------\n\n");
}

int rtthread_init(void)
{
    timer = rt_timer_create("timer", timer_entry, RT_NULL,
        TIMER_TIMEOUT_CNT, RT_TIMER_FLAG_PERIODIC);
    rt_timer_start(timer);

    return 0;
}

/* run in main thread */
int main(void)
{
    rtthread_init();

    return 0;
}

MSH_CMD_EXPORT_ALIAS(event_case, event, start event test)
MSH_CMD_EXPORT_ALIAS(event_case_kill, eventkill, stop event test)
MSH_CMD_EXPORT_ALIAS(mbox_case, mbox, start mbox test)
MSH_CMD_EXPORT_ALIAS(mbox_case_kill, mboxkill, stop mbox test)
MSH_CMD_EXPORT_ALIAS(semphore_case, semphore, start semphore test)
MSH_CMD_EXPORT_ALIAS(semphore_case_kill, semkill, stop semphore test)
MSH_CMD_EXPORT_ALIAS(mempool_case, mempool, start mempool test)
MSH_CMD_EXPORT_ALIAS(mempool_case_kill, memkill, stop mempool test)
