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

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER (TRC_CFG_USE_TRACEALYZER_RECORDER) /* Allows for disabling the recorder */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00UL

#define PSF_EVENT_TRACE_START								0x01UL
#define PSF_EVENT_TS_CONFIG									0x02UL
#define PSF_EVENT_OBJ_NAME									0x03UL
#define PSF_EVENT_TASK_PRIORITY								0x04UL
#define PSF_EVENT_DEFINE_ISR								0x05UL

#define PSF_EVENT_IFE_NEXT									0x08UL
#define PSF_EVENT_IFE_DIRECT								0x09UL

#define PSF_EVENT_TASK_CREATE								0x10UL
#define PSF_EVENT_TASK_DELETE								0x11UL

#define PSF_EVENT_TASK_READY								0x20UL
#define PSF_EVENT_ISR_BEGIN									0x21UL
#define PSF_EVENT_ISR_RESUME								0x22UL
#define PSF_EVENT_TS_BEGIN									0x23UL
#define PSF_EVENT_TS_RESUME									0x24UL
#define PSF_EVENT_TASK_ACTIVATE								0x25UL

#define PSF_EVENT_MALLOC									0x30UL
#define PSF_EVENT_FREE										0x31UL
#define PSF_EVENT_MALLOC_FAILED								0x32UL
#define PSF_EVENT_FREE_FAILED								0x33UL

#define PSF_EVENT_LOWPOWER_BEGIN							0x38UL
#define PSF_EVENT_LOWPOWER_END								0x39UL

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0x40UL
#define PSF_EVENT_STATEMACHINE_CREATE						0x41UL
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0x42UL

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0x43UL
#define PSF_EVENT_INTERVAL_START							0x44UL
#define PSF_EVENT_INTERVAL_STOP								0x45UL
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0x46UL

#define PSF_EVENT_EXTENSION_CREATE							0x47UL

#define PSF_EVENT_HEAP_CREATE								0x48UL

#define PSF_EVENT_COUNTER_CREATE							0x49UL
#define PSF_EVENT_COUNTER_CHANGE							0x4AUL
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0x4BUL

#define PSF_EVENT_DEPENDENCY_REGISTER						0x4CUL

#define PSF_EVENT_RUNNABLE_REGISTER							0x4DUL
#define PSF_EVENT_RUNNABLE_START							0x4EUL
#define PSF_EVENT_RUNNABLE_STOP								0x4FUL

#define PSF_EVENT_USER_EVENT								0x50UL

#define PSF_EVENT_USER_EVENT_FIXED							0x58UL


#define PSF_EVENT_MUTEX_CREATE								0x60
#define PSF_EVENT_MUTEX_LOCK								0x61
#define PSF_EVENT_MUTEX_UNLOCK								0x62

#define TRC_EVENT_LAST_ID									(PSF_EVENT_DEPENDENCY_REGISTER)
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
#define TRACE_CLASS_MESSAGEBUFFER ((traceObjectClass)8)
#undef TRC_CFG_ENABLE_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR 0
/* Definitions for Object Table */
#define TRACE_KERNEL_OBJECT_COUNT ((TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NTASK) + (TRC_CFG_NISR) + (TRC_CFG_NTIMER) + (TRC_CFG_NEVENTGROUP) + (TRC_CFG_NSTREAMBUFFER) + (TRC_CFG_NMESSAGEBUFFER))

/* Queue properties (except name):	current number of message in queue */
#define PropertyTableSizeQueue		((TRC_CFG_NAME_LEN_QUEUE) + 1)

/* Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeSemaphore	((TRC_CFG_NAME_LEN_SEMAPHORE) + 1)

/* Mutex properties (except name):	owner (task handle, 0 = free) */
#define PropertyTableSizeMutex		((TRC_CFG_NAME_LEN_MUTEX) + 1)

/* Task properties (except name):	Byte 0: Current priority
									Byte 1: state (if already active)
									Byte 2: legacy, not used
									Byte 3: legacy, not used */
#define PropertyTableSizeTask		((TRC_CFG_NAME_LEN_TASK) + 4)

/* ISR properties:					Byte 0: priority
									Byte 1: state (if already active) */
#define PropertyTableSizeISR		((TRC_CFG_NAME_LEN_ISR) + 2)

/* TRC_CFG_NTIMER properties:				Byte 0: state (unused for now) */
#define PropertyTableSizeTimer		((TRC_CFG_NAME_LEN_TIMER) + 1)

/* TRC_CFG_NEVENTGROUP properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeEventGroup	((TRC_CFG_NAME_LEN_EVENTGROUP) + 4)

/* TRC_CFG_NSTREAMBUFFER properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeStreamBuffer	((TRC_CFG_NAME_LEN_STREAMBUFFER) + 4)

/* TRC_CFG_NMESSAGEBUFFER properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeMessageBuffer	((TRC_CFG_NAME_LEN_MESSAGEBUFFER) + 4)


/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue			(0)
#define StartIndexSemaphore		(StartIndexQueue		+ (TRC_CFG_NQUEUE)			* PropertyTableSizeQueue)
#define StartIndexMutex			(StartIndexSemaphore 	+ (TRC_CFG_NSEMAPHORE)		* PropertyTableSizeSemaphore)
#define StartIndexTask			(StartIndexMutex		+ (TRC_CFG_NMUTEX)			* PropertyTableSizeMutex)
#define StartIndexISR			(StartIndexTask			+ (TRC_CFG_NTASK)			* PropertyTableSizeTask)
#define StartIndexTimer			(StartIndexISR			+ (TRC_CFG_NISR)			* PropertyTableSizeISR)
#define StartIndexEventGroup	(StartIndexTimer		+ (TRC_CFG_NTIMER)			* PropertyTableSizeTimer)
#define StartIndexStreamBuffer	(StartIndexEventGroup	+ (TRC_CFG_NEVENTGROUP)		* PropertyTableSizeEventGroup)
#define StartIndexMessageBuffer	(StartIndexStreamBuffer	+ (TRC_CFG_NSTREAMBUFFER)	* PropertyTableSizeStreamBuffer)

/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE	(StartIndexMessageBuffer + (TRC_CFG_NMESSAGEBUFFER) * PropertyTableSizeMessageBuffer)

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
#define TRC_PLATFORM_CFG "generic"
#define TRC_PLATFORM_CFG_MAJOR 1
#define TRC_PLATFORM_CFG_MINOR 0
#define TRC_PLATFORM_CFG_PATCH 0

#ifndef TRACE_ENTER_CRITICAL_SECTION
	#error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceTaskHandle_t) * (TRC_CFG_CORE_COUNT))
#else
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceUnsignedBaseType_t) * (TRC_CFG_CORE_COUNT))
#endif

/**
 * @internal The kernel port data buffer
 */
typedef struct TraceKernelPortDataBuffer	/* Aligned */
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
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00UL

#define PSF_EVENT_TRACE_START								0x01UL
#define PSF_EVENT_TS_CONFIG									0x02UL
#define PSF_EVENT_OBJ_NAME									0x03UL
#define PSF_EVENT_TASK_PRIORITY								0x04UL
#define PSF_EVENT_DEFINE_ISR								0x05UL

#define PSF_EVENT_IFE_NEXT									0x08UL
#define PSF_EVENT_IFE_DIRECT								0x09UL

#define PSF_EVENT_TASK_CREATE								0x10UL
#define PSF_EVENT_TASK_DELETE								0x11UL
#define PSF_EVENT_PROCESS_CREATE							0x12UL
#define PSF_EVENT_PROCESS_DELETE							0x13UL
#define PSF_EVENT_THREAD_CREATE								0x14UL
#define PSF_EVENT_THREAD_DELETE								0x15UL

#define PSF_EVENT_TASK_READY								0x20UL
#define PSF_EVENT_ISR_BEGIN									0x21UL
#define PSF_EVENT_ISR_RESUME								0x22UL
#define PSF_EVENT_TS_BEGIN									0x23UL
#define PSF_EVENT_TS_RESUME									0x24UL
#define PSF_EVENT_TASK_ACTIVATE								0x25UL

#define PSF_EVENT_MALLOC									0x30UL
#define PSF_EVENT_FREE										0x31UL
#define PSF_EVENT_MALLOC_FAILED								0x32UL
#define PSF_EVENT_FREE_FAILED								0x33UL

#define PSF_EVENT_LOWPOWER_BEGIN							0x38UL
#define PSF_EVENT_LOWPOWER_END								0x39UL

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0x40UL
#define PSF_EVENT_STATEMACHINE_CREATE						0x41UL
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0x42UL

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0x43UL
#define PSF_EVENT_INTERVAL_START							0x44UL
#define PSF_EVENT_INTERVAL_STOP								0x45UL
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0x46UL

#define PSF_EVENT_EXTENSION_CREATE							0x47UL

#define PSF_EVENT_HEAP_CREATE								0x48UL

#define PSF_EVENT_COUNTER_CREATE							0x49UL
#define PSF_EVENT_COUNTER_CHANGE							0x4AUL
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0x4BUL

#define PSF_EVENT_DEPENDENCY_REGISTER						0x4CUL

#define PSF_EVENT_RUNNABLE_REGISTER							0x4DUL
#define PSF_EVENT_RUNNABLE_START							0x4EUL
#define PSF_EVENT_RUNNABLE_STOP								0x4FUL

#define PSF_EVENT_USER_EVENT								0x50UL

#define PSF_EVENT_USER_EVENT_FIXED							0x58UL

#define TRC_EVENT_LAST_ID									(PSF_EVENT_DEPENDENCY_REGISTER)

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
