/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_sdmmc_osal.h"
#include "hpm_sdmmc_host.h"

#if defined(HPM_SDMMC_USE_CMSIS_OS2) && (HPM_SDMMC_USE_CMSIS_OS2 == 1)
#include "cmsis_os2.h"

ATTR_WEAK void hpm_sdmmc_osal_delay(void *ctx, uint32_t ms)
{
    (void) ctx;
    uint32_t ticks_per_sec = osKernelGetTickFreq();
    uint32_t delay_ticks = (ms * ticks_per_sec + 999UL) / 1000UL;
    osDelay(delay_ticks);
}

ATTR_WEAK hpm_sdmmc_osal_event_t hpm_sdmmc_osal_event_create(void *ctx)
{
    (void) ctx;

    hpm_sdmmc_osal_event_t event = osEventFlagsNew(NULL);

    return event;
}

ATTR_WEAK hpm_stat_t hpm_sdmmc_osal_event_delete(void *ctx, hpm_sdmmc_osal_event_t event)
{
    (void) ctx;
    uint32_t err = osEventFlagsDelete(event);
    return (err == osOK) ? status_success : status_fail;
}

ATTR_WEAK hpm_stat_t hpm_sdmmc_osal_event_wait(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags, uint32_t timeout)
{
    (void) ctx;

    uint32_t ticks_per_sec = osKernelGetTickFreq();
    uint32_t timeout_ticks = (timeout * ticks_per_sec + 999UL) / 1000UL;
    uint32_t err = osEventFlagsWait(event, flags, osFlagsWaitAny, timeout_ticks);
    if (err == (uint32_t)osErrorTimeout) {
        return status_timeout;
    }
    return status_success;
}

ATTR_WEAK void hpm_sdmmc_osal_event_set(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags)
{
    (void) ctx;

    osEventFlagsSet(event, flags);
}

ATTR_WEAK void hpm_sdmmc_osal_event_clear(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags)
{
    (void) ctx;

    osEventFlagsClear(event, flags);
}

ATTR_WEAK void hpm_sdmmc_osal_enter_critical(void *ctx)
{
    (void) ctx;
    osKernelLock();
}

ATTR_WEAK void hpm_sdmmc_osal_exit_critical(void *ctx)
{
    (void) ctx;
    osKernelUnlock();
}

#else
#include "hpm_clock_drv.h"
#include "hpm_interrupt.h"
#include "hpm_csr_drv.h"
#include <string.h>
#include <stdlib.h>

volatile uint32_t hpm_sdmmc_osal_level;

ATTR_WEAK void hpm_sdmmc_osal_delay(void *ctx, uint32_t ms)
{
    sdmmc_host_t *host = (sdmmc_host_t *) ctx;
    if ((host != NULL) && (host->host_param.delay_ms != NULL)) {
        host->host_param.delay_ms(ms);
    }
}

ATTR_WEAK hpm_sdmmc_osal_event_t hpm_sdmmc_osal_event_create(void *ctx)
{
    (void) ctx;
    hpm_sdmmc_osal_event_t event = (hpm_sdmmc_osal_event_t) malloc(sizeof(uint32_t));
    if (event != NULL) {
        *event = 0;
    }
    return event;
}

ATTR_WEAK hpm_stat_t hpm_sdmmc_osal_event_delete(void *ctx, hpm_sdmmc_osal_event_t event)
{
    (void) ctx;
    if (event != NULL) {
        free((void *)event);
    }
    return status_success;
}

ATTR_WEAK hpm_stat_t hpm_sdmmc_osal_event_wait(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags, uint32_t timeout)
{
    (void) ctx;
    uint32_t matched_flags = 0;
    bool wait_forever = (timeout == HPM_SDMMC_OSAL_WAIT_FOREVER);
    bool timeout_occurred = false;
    uint64_t start_ticks = hpm_csr_get_core_mcycle();
    uint32_t ticks_per_ms = clock_get_core_clock_ticks_per_ms();
    uint64_t timeout_ticks = start_ticks + timeout * ticks_per_ms;
    while ((*event & flags) == 0) {
        if (!wait_forever) {
            uint64_t current_ticks = hpm_csr_get_core_mcycle();
            if (current_ticks > timeout_ticks) {
                timeout_occurred = true;
                break;
            }
        }
    }
    matched_flags = *event & flags;
    if (!timeout_occurred) {
        *event &= ~matched_flags;
    }
    return timeout_occurred ? status_timeout : status_success;
}

ATTR_WEAK void hpm_sdmmc_osal_event_set(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags)
{
    (void) ctx;

    *event |= flags;
}

ATTR_WEAK void hpm_sdmmc_osal_event_clear(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags)
{
    (void) ctx;

    *event &= ~flags;
}

ATTR_WEAK void hpm_sdmmc_osal_enter_critical(void *ctx)
{
    (void) ctx;
    hpm_sdmmc_osal_level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

ATTR_WEAK void hpm_sdmmc_osal_exit_critical(void *ctx)
{
    (void) ctx;
    restore_global_irq(hpm_sdmmc_osal_level);
}
#endif
