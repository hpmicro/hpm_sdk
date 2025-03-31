/*
 * Trace Recorder for Tracealyzer v4.9.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * For bare-metal use of Tracealyzer (no RTOS)
 */

#include "rtt_port.h"
#include "rtthread.h"
#include <stdint.h>
#include <trcRecorder.h>
static rt_thread_t tidle;
static TraceISRHandle_t global_isr;
#if (TRC_USE_TRACEALYZER_RECORDER == 1)

typedef struct TraceKernelPortData {
    TraceTaskHandle_t xTaskHandles[TRC_CFG_CORE_COUNT];
    TraceKernelPortTaskHandle_t xTzCtrlHandle;
} TraceKernelPortData_t;

#if (TRC_CFG_CPU_CLOCK_HZ == 0)
#error "TRC_CFG_CPU_CLOCK_HZ needs to be set to the CPU frequency."
#endif
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

static TraceHeapHandle_t ptrc_heap;
static TraceKernelPortData_t *pxKernelPortData TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static void trc_thread_suspend_hook(rt_thread_t thread)
{
    prvTraceStoreEvent_Param(PSF_EVENT_TASK_SUSPEND,
                 (TraceUnsignedBaseType_t)thread);
}

static void trc_thread_resume_hook(rt_thread_t thread)
{
    xTraceTaskReady((void *)thread);
}

static void trc_thread_inited_hook(rt_thread_t thread)
{
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_TASK_CREATE, (void *)thread,
                      thread->name, thread->current_priority);
}

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t *const pxBuffer)
{
    TRC_ASSERT_EQUAL_SIZE(TraceKernelPortData_t,
                  TraceKernelPortDataBuffer_t);

    pxKernelPortData = (TraceKernelPortData_t *)
        pxBuffer; /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress
             conversion between pointer types checks*/

    return TRC_SUCCESS;
}

static void trc_object_attach_hook(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static)) {
    case RT_Object_Class_Semaphore: {
        rt_sem_t sem = (rt_sem_t)object;
        xTraceObjectRegisterWithoutHandle(
            PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (void *)(object),
            object->name, sem->value);
    } break;
    case RT_Object_Class_Mutex:
        prvTraceStoreEvent_Param(PSF_EVENT_MUTEX_CREATE, object);
        break;
    case RT_Object_Class_Event:
        prvTraceStoreEvent_Param(PSF_EVENT_EVENT_CREATE, object);
        break;
    case RT_Object_Class_MailBox:
        xTraceObjectRegisterWithoutHandle(
            PSF_EVENT_MAIL_BOX_CREATE, (void *)(object), object->name,
            ((rt_mailbox_t)object)->size);
        break;
    case RT_Object_Class_MessageQueue:
        xTraceObjectRegisterWithoutHandle(
            PSF_EVENT_QUEUE_CREATE, (void *)(object), object->name,
            ((rt_mq_t)object)->max_msgs);
        break;
    case RT_Object_Class_Timer:
        xTraceObjectRegisterWithoutHandle(
            PSF_EVENT_TIMER_CREATE, (void *)(object), object->name,
            ((rt_timer_t)object)->timeout_tick);
        break;
    case RT_Object_Class_MemPool:
        xTraceBlockPoolCreateHook(1116, (rt_mp_t)object,
                      ((rt_mp_t)object)->block_size,
                      ((rt_mp_t)object)->start_address,
                      ((rt_mp_t)object)->block_total_count);
        break;
    default:
        break;
    }
}

static void trc_object_detach_hook(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static)) {
    case RT_Object_Class_Thread:
        prvTraceStoreEvent_ParamParam(PSF_EVENT_TASK_DELETE,
                          (void *)(object), object->name);
        break;
    case RT_Object_Class_Semaphore:
        prvTraceStoreEvent_Param(PSF_EVENT_SEMAPHORE_DELETE, object);
        break;
    case RT_Object_Class_Mutex:
        prvTraceStoreEvent_Param(PSF_EVENT_MUTEX_DELETE, object);
        break;
    case RT_Object_Class_Event:
        prvTraceStoreEvent_Param(PSF_EVENT_EVENT_DELETE, object);
        break;
    case RT_Object_Class_MailBox:
        prvTraceStoreEvent_ParamParam(PSF_EVENT_MAIL_BOX_DELETE,
                          (void *)(object), object->name);
        break;
    case RT_Object_Class_MessageQueue:
        prvTraceStoreEvent_Param(PSF_EVENT_QUEUE_DELETE, object);
        break;
    case RT_Object_Class_Timer:
        prvTraceStoreEvent_ParamParam(PSF_EVENT_TIMER_DELETE,
                          (void *)(object), object->name);
        break;
    case RT_Object_Class_MemPool:
        prvTraceStoreEvent_Param(1117, object);
        break;
    default:
        break;
    }
}

static void trc_object_take_hook(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static)) {
    case RT_Object_Class_Semaphore:
        prvTraceStoreEvent_HandleParamParam(PSF_EVENT_SEMAPHORE_TAKE,
                            object, 0,
                            ((rt_sem_t)object)->value);
        break;
    case RT_Object_Class_Mutex:
        prvTraceStoreEvent_Param(PSF_EVENT_MUTEX_TAKE, object);
        break;
    case RT_Object_Class_Event:
        prvTraceStoreEvent_HandleParam(PSF_EVENT_EVENT_WAIT, object,
                           ((rt_event_t)object)->set);
        break;
    case RT_Object_Class_MailBox:
        prvTraceStoreEvent_Param(PSF_EVENT_MAIL_BOX_RECEIVE, object);
        break;
    case RT_Object_Class_MessageQueue:
        prvTraceStoreEvent_Param(PSF_EVENT_QUEUE_RECEIVE, object);
        break;
#if RT_TRACE_TIMER_START_STOP
    case RT_Object_Class_Timer:
        prvTraceStoreEvent_Param(PSF_EVENT_TIMER_START, object);
        break;
#endif
    }
}

static void trc_object_put_hook(struct rt_object *object)
{
    switch (object->type & (~RT_Object_Class_Static)) {
    case RT_Object_Class_Semaphore:
        prvTraceStoreEvent_HandleParamParam(PSF_EVENT_SEMAPHORE_GIVE,
                            object, 0,
                            ((rt_sem_t)object)->value);
        break;
    case RT_Object_Class_Mutex:
        prvTraceStoreEvent_HandleParam(PSF_EVENT_MUTEX_GIVE, object, 1);
        break;
    case RT_Object_Class_Event:
        prvTraceStoreEvent_HandleParam(PSF_EVENT_EVENT_SET, object,
                           ((rt_event_t)object)->set);
        break;
    case RT_Object_Class_MailBox:
        prvTraceStoreEvent_Param(PSF_EVENT_MAIL_BOX_SEND, object);
        break;
    case RT_Object_Class_MessageQueue:
        prvTraceStoreEvent_HandleParam(PSF_EVENT_QUEUE_SEND, object, 1);
        break;
#if RT_TRACE_TIMER_START_STOP
    case RT_Object_Class_Timer:
        prvTraceStoreEvent_Param(PSF_EVENT_TIMER_STOP, object);
        break;
#endif
    }
}

static void trc_scheduler_hook(struct rt_thread *from, struct rt_thread *to)
{
    xTraceTaskSetCurrentOnCore(0, to);
    prvTraceStoreEvent_Param(PSF_EVENT_TASK_SUSPEND,
                 (TraceUnsignedBaseType_t)from);
    
    if (to == tidle) {
        prvTraceStoreEvent_Param(PSF_EVENT_TASK_ACTIVATE,
                     (TraceUnsignedBaseType_t)to);
        return;
    }
    if (rt_interrupt_get_nest()) {
        xTraceTaskReady((TraceUnsignedBaseType_t)to);
    } else {
        prvTraceStoreEvent_Param(PSF_EVENT_TASK_RESUME,
                     (TraceUnsignedBaseType_t)to);
    }
}

static void trc_timer_enter_hook(struct rt_timer *timer)
{
    prvTraceStoreEvent_HandleParam(PSF_EVENT_TIMER_EXPIRED, timer,
                       timer->timeout_tick);
}

static void trc_irq_enter_hook(void)
{
    xTraceISRBegin(global_isr);
}

static void trc_irq_leave_hook(void)
{
    xTraceISREnd(0);
}

static void trc_mp_alloc_hook(struct rt_mempool *mp, void *block)
{
    (void)block;
    xTraceBlockAllocateHook(1111, mp, 0, 1);
}

static void trc_mp_free_hook(struct rt_mempool *mp, void *block)
{
    (void)block;
    xTraceBlockReleaseHook(1127, mp);
}

static void TzCtrl(void *param)
{
    (void)param;

    while (1) {
        xTraceTzCtrl();

        rt_thread_mdelay(TRC_CFG_CTRL_TASK_DELAY);
    }
}

traceResult xTraceKernelPortEnable(void)
{
    tidle = rt_thread_idle_gethandler();

    rt_thread_resume_sethook(trc_thread_resume_hook);

    rt_thread_suspend_sethook(trc_thread_suspend_hook);

    rt_thread_inited_sethook(trc_thread_inited_hook);

    rt_object_attach_sethook(trc_object_attach_hook);

    rt_object_detach_sethook(trc_object_detach_hook);

    rt_object_take_sethook(trc_object_take_hook);

    rt_object_put_sethook(trc_object_put_hook);

    rt_scheduler_sethook(trc_scheduler_hook);

    rt_timer_enter_sethook(trc_timer_enter_hook);

    xTraceISRRegister("isr", 0, &global_isr);

    rt_interrupt_enter_sethook(trc_irq_enter_hook);

    rt_interrupt_leave_sethook(trc_irq_leave_hook);

    rt_mp_alloc_sethook(trc_mp_alloc_hook);

    rt_mp_free_sethook(trc_mp_free_hook);

    xTraceHeapCreate("SysHeap", 0, 0, RT_HEAP_SIZE, &ptrc_heap);

    if (pxKernelPortData->xTzCtrlHandle == 0) {
        /* Creates the TzCtrl task - receives trace commands (start,
         * stop, ...) */
#if defined(configSUPPORT_STATIC_ALLOCATION) &&                                \
    (configSUPPORT_STATIC_ALLOCATION == 1)
        pxKernelPortData->xTzCtrlHandle = xTaskCreateStatic(
            TzCtrl, STRING_CAST("TzCtrl"), TRC_CFG_CTRL_TASK_STACK_SIZE,
            0, TRC_CFG_CTRL_TASK_PRIORITY, stackTzCtrl, &tcbTzCtrl);
#else
        pxKernelPortData->xTzCtrlHandle =
            rt_thread_create(STRING_CAST("TzCtrl"), TzCtrl, RT_NULL,
                     TRC_CFG_CTRL_TASK_STACK_SIZE,
                     TRC_CFG_CTRL_TASK_PRIORITY, RT_NULL);
#endif

        if (pxKernelPortData->xTzCtrlHandle == 0) {
            xTraceError(TRC_ERROR_TZCTRLTASK_NOT_CREATED);

            return TRC_FAIL;
        }
    }
    return TRC_SUCCESS;
}

#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

#error                                                                         \
    "BareMetal requires using Streaming recorder mode (TRC_CFG_RECORDER_MODE) with RingBuffer streamport if snapshot functionality is needed."

#endif /* Snapshot mode */

#endif
