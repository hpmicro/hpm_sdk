/*
 * Trace Recorder for Tracealyzer v4.9.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * For bare-metal use of Tracealyzer (no RTOS)
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <trcDefines.h>
#include "trcKernelPortHook.h"
#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER (1) /* Allows for disabling the recorder */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

#define STRING_CAST(x) x
#define TraceKernelPortTaskHandle_t rt_thread_t
#define TraceKernelPortTickType_t rt_tick_t
#define TraceKernelPortQueueHandle_t rt_mailbox_t

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void);

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_QUEUE_CREATE								0x11
#define PSF_EVENT_MUTEX_CREATE								0x13
#define PSF_EVENT_TIMER_CREATE								0x14
#define PSF_EVENT_EVENT_CREATE							0x15
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE					0x16
#define PSF_EVENT_STREAMBUFFER_CREATE						0x18
#define PSF_EVENT_MAIL_BOX_CREATE						0x19

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_TIMER_DELETE								0x24
#define PSF_EVENT_EVENT_DELETE							0x25
#define PSF_EVENT_STREAMBUFFER_DELETE						0x28
#define PSF_EVENT_MAIL_BOX_DELETE						0x29

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED			0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_TIMER_CREATE_FAILED						0x44
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x45
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED			0x46
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE_FAILED				0x47
#define PSF_EVENT_STREAMBUFFER_CREATE_FAILED				0x49
#define PSF_EVENT_MAIL_BOX_CREATE_FAILED				0x4A

#define PSF_EVENT_TIMER_DELETE_FAILED						0x48

#define PSF_EVENT_QUEUE_SEND								0x50
#define PSF_EVENT_SEMAPHORE_GIVE							0x51
#define PSF_EVENT_MUTEX_GIVE								0x52

#define PSF_EVENT_QUEUE_SEND_FAILED							0x53
#define PSF_EVENT_SEMAPHORE_GIVE_FAILED						0x54
#define PSF_EVENT_MUTEX_GIVE_FAILED							0x55

#define PSF_EVENT_QUEUE_SEND_BLOCK							0x56
#define PSF_EVENT_SEMAPHORE_GIVE_BLOCK						0x57
#define PSF_EVENT_MUTEX_GIVE_BLOCK							0x58

#define PSF_EVENT_QUEUE_SEND_FROMISR						0x59
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR					0x5A

#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x5C
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED				0x5D

#define PSF_EVENT_QUEUE_RECEIVE								0x60
#define PSF_EVENT_SEMAPHORE_TAKE							0x61
#define PSF_EVENT_MUTEX_TAKE								0x62

#define PSF_EVENT_QUEUE_RECEIVE_FAILED						0x63
#define PSF_EVENT_SEMAPHORE_TAKE_FAILED						0x64
#define PSF_EVENT_MUTEX_TAKE_FAILED							0x65

#define PSF_EVENT_QUEUE_RECEIVE_BLOCK						0x66
#define PSF_EVENT_SEMAPHORE_TAKE_BLOCK						0x67
#define PSF_EVENT_MUTEX_TAKE_BLOCK							0x68

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x69
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR					0x6A

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x6C
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED				0x6D

#define PSF_EVENT_QUEUE_PEEK								0x70
#define PSF_EVENT_SEMAPHORE_PEEK							0x71
#define PSF_EVENT_MUTEX_PEEK								0x72

#define PSF_EVENT_QUEUE_PEEK_FAILED							0x73
#define PSF_EVENT_SEMAPHORE_PEEK_FAILED						0x74	
#define PSF_EVENT_MUTEX_PEEK_FAILED							0x75

#define PSF_EVENT_QUEUE_PEEK_BLOCK							0x76
#define PSF_EVENT_SEMAPHORE_PEEK_BLOCK						0x77
#define PSF_EVENT_MUTEX_PEEK_BLOCK							0x78

#define PSF_EVENT_TASK_DELAY_UNTIL							0x79
#define PSF_EVENT_TASK_DELAY								0x7A
#define PSF_EVENT_TASK_SUSPEND								0x36
#define PSF_EVENT_TASK_RESUME								0x7C
#define PSF_EVENT_TASK_RESUME_FROMISR						0x7D

#define PSF_EVENT_TIMER_PENDFUNCCALL						0x80
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR				0x81
#define PSF_EVENT_TIMER_PENDFUNCCALL_FAILED					0x82
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED			0x83

/* We reserve 0x08 slots for this */
#define PSF_EVENT_USER_EVENT								0x90

/* We reserve 0x08 slots for this */
#define PSF_EVENT_USER_EVENT_FIXED							0x98

#define PSF_EVENT_TIMER_START								0xA0
#define PSF_EVENT_TIMER_RESET								0xA1
#define PSF_EVENT_TIMER_STOP								0xA2
#define PSF_EVENT_TIMER_CHANGEPERIOD						0xA3
#define PSF_EVENT_TIMER_START_FROMISR						0xA4
#define PSF_EVENT_TIMER_RESET_FROMISR						0xA5
#define PSF_EVENT_TIMER_STOP_FROMISR						0xA6
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0xA7
#define PSF_EVENT_TIMER_START_FAILED						0xA8
#define PSF_EVENT_TIMER_RESET_FAILED						0xA9
#define PSF_EVENT_TIMER_STOP_FAILED							0xAA
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0xAB
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0xAC
#define PSF_EVENT_TIMER_RESET_FROMISR_FAILED				0xAD
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0xAE
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0xAF

#define PSF_EVENT_EVENT_WAIT						0xB1
#define PSF_EVENT_EVENT_SET						0xB4

#define PSF_EVENT_QUEUE_SEND_FRONT							0xC0
#define PSF_EVENT_QUEUE_SEND_FRONT_FAILED					0xC1
#define PSF_EVENT_QUEUE_SEND_FRONT_BLOCK					0xC2
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR					0xC3
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED			0xC4
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE						0xC5
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED				0xC6
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE						0xC7
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED				0xC8

#define PSF_EVENT_TASK_NOTIFY								0xC9
#define PSF_EVENT_TASK_NOTIFY_WAIT							0xCA
#define PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK					0xCB
#define PSF_EVENT_TASK_NOTIFY_WAIT_FAILED					0xCC
#define PSF_EVENT_TASK_NOTIFY_FROM_ISR						0xCD

#define PSF_EVENT_TIMER_EXPIRED								0xD2

#define PSF_EVENT_STREAMBUFFER_SEND							0xD3
#define PSF_EVENT_STREAMBUFFER_SEND_BLOCK					0xD4
#define PSF_EVENT_STREAMBUFFER_SEND_FAILED					0xD5
#define PSF_EVENT_STREAMBUFFER_RECEIVE						0xD6
#define PSF_EVENT_STREAMBUFFER_RECEIVE_BLOCK				0xD7
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FAILED				0xD8
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR				0xD9
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR_FAILED			0xDA
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR				0xDB
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED		0xDC
#define PSF_EVENT_STREAMBUFFER_RESET						0xDD

#define PSF_EVENT_MAIL_BOX_SEND						0xDE
#define PSF_EVENT_MAIL_BOX_SEND_BLOCK					0xDF
#define PSF_EVENT_MAIL_BOX_SEND_FAILED					0xE0
#define PSF_EVENT_MAIL_BOX_RECEIVE						0xE1
#define PSF_EVENT_MAIL_BOX_RECEIVE_BLOCK				0xE2
#define PSF_EVENT_MAIL_BOX_RECEIVE_FAILED				0xE3
#define PSF_EVENT_MAIL_BOX_SEND_FROM_ISR				0xE4
#define PSF_EVENT_MAIL_BOX_SEND_FROM_ISR_FAILED		0xE5
#define PSF_EVENT_MAIL_BOX_RECEIVE_FROM_ISR			0xE6
#define PSF_EVENT_MAIL_BOX_RECEIVE_FROM_ISR_FAILED		0xE7
#define PSF_EVENT_MAIL_BOX_RESET						0xE8

#define PSF_EVENT_MALLOC_FAILED								0xE9
#define PSF_EVENT_FREE_FAILED								0xEA

#define PSF_EVENT_UNUSED_STACK								0xEB

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0xEC
#define PSF_EVENT_STATEMACHINE_CREATE						0xED
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0xEE

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0xEF
#define PSF_EVENT_INTERVAL_START							0xF0

#define PSF_EVENT_EXTENSION_CREATE							0xF1

#define PSF_EVENT_HEAP_CREATE								0xF2

#define PSF_EVENT_COUNTER_CREATE							0xF3
#define PSF_EVENT_COUNTER_CHANGE							0xF4
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0xF5

#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_BLOCK				0xF6

#define PSF_EVENT_INTERVAL_STOP								0xF7
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0xF8

#define PSF_EVENT_RUNNABLE_REGISTER							0xF9
#define PSF_EVENT_RUNNABLE_START							0xFA
#define PSF_EVENT_RUNNABLE_STOP								0xFB

#define PSF_EVENT_DEPENDENCY_REGISTER						0xFC

#define TRC_EVENT_LAST_ID									(PSF_EVENT_DEPENDENCY_REGISTER)
#endif
/* The object classes */
#define TRACE_NCLASSES 9
#define TRACE_CLASS_QUEUE ((traceObjectClass)0)
#define TRACE_CLASS_SEMAPHORE ((traceObjectClass)1)
#define TRACE_CLASS_MUTEX ((traceObjectClass)2)
#define TRACE_CLASS_TASK ((traceObjectClass)3)
#define TRACE_CLASS_ISR ((traceObjectClass)4)
#define TRACE_CLASS_TIMER ((traceObjectClass)5)
#define TRACE_CLASS_EVENTGROUP ((traceObjectClass)6)
#define TRACE_CLASS_STREAMBUFFER ((traceObjectClass)7)
#define TRACE_CLASS_MAIL_BOX ((traceObjectClass)8)
#undef TRC_CFG_ENABLE_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR 0
/* Definitions for Object Table */
#define TRACE_KERNEL_OBJECT_COUNT                                                                                     \
  ((TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NTASK) + (TRC_CFG_NISR) + (TRC_CFG_NTIMER) + \
   (TRC_CFG_NEVENTGROUP) + (TRC_CFG_NSTREAMBUFFER) + (TRC_CFG_NMAIL_BOX))

/* Queue properties (except name):	current number of message in queue */
#define PropertyTableSizeQueue ((TRC_CFG_NAME_LEN_QUEUE) + 1)

/* Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeSemaphore ((TRC_CFG_NAME_LEN_SEMAPHORE) + 1)

/* Mutex properties (except name):	owner (task handle, 0 = free) */
#define PropertyTableSizeMutex ((TRC_CFG_NAME_LEN_MUTEX) + 1)

/* Task properties (except name):	Byte 0: Current priority
                                                                        Byte 1: state (if already active)
                                                                        Byte 2: legacy, not used
                                                                        Byte 3: legacy, not used */
#define PropertyTableSizeTask ((TRC_CFG_NAME_LEN_TASK) + 4)

/* ISR properties:					Byte 0: priority
                                                                        Byte 1: state (if already active) */
#define PropertyTableSizeISR ((TRC_CFG_NAME_LEN_ISR) + 2)

/* TRC_CFG_NTIMER properties:				Byte 0: state (unused for now) */
#define PropertyTableSizeTimer ((TRC_CFG_NAME_LEN_TIMER) + 1)

/* TRC_CFG_NEVENTGROUP properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeEventGroup ((TRC_CFG_NAME_LEN_EVENTGROUP) + 4)

/* TRC_CFG_NSTREAMBUFFER properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeStreamBuffer ((TRC_CFG_NAME_LEN_STREAMBUFFER) + 4)

/* TRC_CFG_NMAIL_BOX properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeMessageBuffer ((TRC_CFG_NAME_LEN_MAIL_BOX) + 4)

/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue (0)
#define StartIndexSemaphore (StartIndexQueue + (TRC_CFG_NQUEUE) * PropertyTableSizeQueue)
#define StartIndexMutex (StartIndexSemaphore + (TRC_CFG_NSEMAPHORE) * PropertyTableSizeSemaphore)
#define StartIndexTask (StartIndexMutex + (TRC_CFG_NMUTEX) * PropertyTableSizeMutex)
#define StartIndexISR (StartIndexTask + (TRC_CFG_NTASK) * PropertyTableSizeTask)
#define StartIndexTimer (StartIndexISR + (TRC_CFG_NISR) * PropertyTableSizeISR)
#define StartIndexEventGroup (StartIndexTimer + (TRC_CFG_NTIMER) * PropertyTableSizeTimer)
#define StartIndexStreamBuffer (StartIndexEventGroup + (TRC_CFG_NEVENTGROUP) * PropertyTableSizeEventGroup)
#define StartIndexMessageBuffer (StartIndexStreamBuffer + (TRC_CFG_NSTREAMBUFFER) * PropertyTableSizeStreamBuffer)

/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE (StartIndexMessageBuffer + (TRC_CFG_NMAIL_BOX) * PropertyTableSizeMessageBuffer)

/*** Don't change the below definitions, unless you know what you are doing! ***/

#define TRACE_KERNEL_VERSION 0x1FF1

/* Dummy definitions, since no RTOS */
#define TRC_TICK_RATE_HZ 1 /* Must not be 0. */

/**
 * @def TRACE_CPU_CLOCK_HZ
 * @brief Trace CPU clock speed in Hz.
 */
#define TRACE_CPU_CLOCK_HZ TRC_CFG_CPU_CLOCK_HZ

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
#include <stdlib.h> /* Include malloc() */

/**
 * @internal Kernel port specific heap initialization
 */
#define TRC_KERNEL_PORT_HEAP_INIT(size)

/**
 * @internal Kernel port specific heap malloc definition
 */
#define TRC_KERNEL_PORT_HEAP_MALLOC(size) malloc(size)
#endif

/**
 * @internal Kernel port specific platform configuration. Maximum name length is 8!
 */
#define TRC_PLATFORM_CFG "RTThread"
#define TRC_PLATFORM_CFG_MAJOR 1
#define TRC_PLATFORM_CFG_MINOR 0
#define TRC_PLATFORM_CFG_PATCH 0

#ifndef TRACE_ENTER_CRITICAL_SECTION
#error \
    "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceTaskHandle_t) * (TRC_CFG_CORE_COUNT))
#else
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceUnsignedBaseType_t) * (TRC_CFG_CORE_COUNT))
#endif

/**
 * @internal The kernel port data buffer
 */
typedef struct TraceKernelPortDataBuffer /* Aligned */
{
  uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

/**
 * @internal Initializes the kernel port
 *
 * @param[in] pxBuffer Kernel port data buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* const pxBuffer);

/**
 * @internal Enables the kernel port
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortEnable(void);

/**
 * @internal Not used for BareMetal
 *
 * @param[in] uiTicks Tick count to delay
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceKernelPortDelay(uiTicks) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)uiTicks, TRC_SUCCESS)

/**
 * @internal Query if scheduler is suspended. For Bare Metal systems this will always be false.
 *
 * @retval 1 Scheduler suspended
 * @retval 0 Scheduler not suspended
 */
#define xTraceKernelPortIsSchedulerSuspended() (0U)

/******************************************************************************/
/*** Definitions for Snapshot mode ********************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

#endif

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/


#endif

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
