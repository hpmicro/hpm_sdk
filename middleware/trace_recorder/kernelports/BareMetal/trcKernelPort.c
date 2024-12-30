/*
 * Trace Recorder for Tracealyzer v4.9.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * For bare-metal use of Tracealyzer (no RTOS)
 */

#include <stdint.h>
#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if(TRC_CFG_CPU_CLOCK_HZ == 0)
#error "TRC_CFG_CPU_CLOCK_HZ needs to be set to the CPU frequency."
#endif
	
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

typedef struct TraceKernelPortData
{
	TraceTaskHandle_t xTaskHandles[TRC_CFG_CORE_COUNT];
} TraceKernelPortData_t;

static TraceKernelPortData_t* pxKernelPortData TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* const pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceKernelPortData_t, TraceKernelPortDataBuffer_t);
	
	pxKernelPortData = (TraceKernelPortData_t*)pxBuffer; /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress conversion between pointer types checks*/
	
	return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
	uint32_t i;

	for (i = 0; i < (TRC_CFG_CORE_COUNT); i++)
	{
		(void)xTraceObjectRegister(PSF_EVENT_TASK_CREATE, (void*)0, "main", 1u, (TraceObjectHandle_t*)&pxKernelPortData->xTaskHandles[i]);
		(void)xTraceTaskSetCurrentOnCore(i, pxKernelPortData->xTaskHandles[i]);
	}
	
	return TRC_SUCCESS;
}

#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

#error "BareMetal requires using Streaming recorder mode (TRC_CFG_RECORDER_MODE) with RingBuffer streamport if snapshot functionality is needed."

#endif /* Snapshot mode */

#endif
