/*
 * Trace Recorder for Tracealyzer v4.10.1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * ThreadX specific hooks
 */

#include "rtt_port.h"
#include "rtthread.h"
#include <stdint.h>
#include <trcRecorder.h>

void xTraceBlockAllocateHook(uint32_t uiTxEventCode, rt_mp_t pxBlockPool,
                 unsigned long uiWaitOption,
                 uint32_t uiRemainingBlocks)
{
    (void)xTraceEventCreate3(uiTxEventCode,
                 (TraceUnsignedBaseType_t)pxBlockPool,
                 (TraceUnsignedBaseType_t)uiWaitOption,
                 (TraceUnsignedBaseType_t)uiRemainingBlocks);
}

void xTraceBlockPoolCreateHook(uint32_t uiTxEventCode, rt_mp_t pxBlockPool,
                   unsigned long uiBlockSize, void *pvPoolStart,
                   unsigned long uiTotalBlocks)
{
    if (pxBlockPool->parent.name[0] != '\0') {
        (void)xTraceObjectSetNameWithoutHandle(
            (void *)pxBlockPool, pxBlockPool->parent.name);
    }

    (void)xTraceEventCreate4(uiTxEventCode,
                 (TraceUnsignedBaseType_t)pxBlockPool,
                 (TraceUnsignedBaseType_t)uiBlockSize,
                 (TraceUnsignedBaseType_t)pvPoolStart,
                 (TraceUnsignedBaseType_t)uiTotalBlocks);
}

void xTraceBlockPoolDeleteHook(uint32_t uiTxEventCode, rt_mp_t pxBlockPool)
{
    (void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode,
                          (void *)pxBlockPool, 0);
}

void xTraceBlockReleaseHook(uint32_t uiTxEventCode, rt_mp_t pvBlockPool)
{
    (void)xTraceEventCreate1(uiTxEventCode,
                 (TraceUnsignedBaseType_t)pvBlockPool);
}
