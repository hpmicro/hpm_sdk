#ifndef TRC_KERNEL_PORT_HOOK_H
#define TRC_KERNEL_PORT_HOOK_H
#include "rtthread.h"
void xTraceBlockAllocateHook(uint32_t uiTxEventCode, rt_mp_t pxBlockPool,
				    unsigned long uiWaitOption,
				    uint32_t uiRemainingBlocks);
void xTraceBlockPoolCreateHook(uint32_t uiTxEventCode,
				      rt_mp_t pxBlockPool,
				      unsigned long uiBlockSize,
				      void *pvPoolStart,
				      unsigned long uiTotalBlocks);
void xTraceBlockPoolDeleteHook(uint32_t uiTxEventCode,
				      rt_mp_t pxBlockPool);
void xTraceBlockReleaseHook(uint32_t uiTxEventCode, rt_mp_t pvBlockPool);
#endif