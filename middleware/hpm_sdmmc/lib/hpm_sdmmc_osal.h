/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_OSAL_H
#define HPM_SDMMC_OSAL_H

/**
 *
 * @brief HPM SDMMC OSAL APIs
 * @defgroup hpm_sdmmc HPM SDMMC stack
 * @ingroup hpm_sdmmc_interfaces
 * @{
 *
 */

#include <stdint.h>
#include "hpm_common.h"

#if defined(HPM_SDMMC_USE_CMSIS_OS2) && (HPM_SDMMC_USE_CMSIS_OS2 == 1)
#include "cmsis_os2.h"
typedef osEventFlagsId_t hpm_sdmmc_osal_event_t;
#define HPM_SDMMC_OSAL_WAIT_FOREVER osWaitForever
#else
typedef volatile uint32_t *hpm_sdmmc_osal_event_t;
#define HPM_SDMMC_OSAL_WAIT_FOREVER (0xFFFFFFFFUL)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Delay in milliseconds
 *
 * @param [in] ctx OSAL context
 * @param [in] ms Delay in milliseconds
 */
void hpm_sdmmc_osal_delay(void *ctx, uint32_t ms);

/**
 * @brief Create Event
 *
 * @param [in] ctx OSAL context
 *
 * @return The created event
 */
hpm_sdmmc_osal_event_t hpm_sdmmc_osal_event_create(void *ctx);

/**
 * @brief Delete an event
 *
 * @param [in] ctx OSAL context
 * @param [in] event  The event to be removed
 *
 * @return the Operation status
 */
hpm_stat_t hpm_sdmmc_osal_event_delete(void *ctx, hpm_sdmmc_osal_event_t event);

/**
 * @brief Wait specified event flags
 *
 * @param [in] ctx OSAL context
 * @param [in] event  The event
 * @param [in] flags  The flags to wait
 * @param [in] timeout  Timeout in milliseconds
 *
 * @return The operation status
 */
hpm_stat_t hpm_sdmmc_osal_event_wait(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags, uint32_t timeout);

/**
 * @brief Set specified event flags
 *
 * @param [in] ctx OSAL context
 * @param [in] event  The event
 * @param [in] flags  The flags to set
 */
void hpm_sdmmc_osal_event_set(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags);

/**
 * @brief Clear specified event flags
 *
 * @param [in] ctx OSAL context
 * @param [in] event  The event
 * @param [in] flags  The flags to clear
 */
void hpm_sdmmc_osal_event_clear(void *ctx, hpm_sdmmc_osal_event_t event, uint32_t flags);

/**
 * @brief Enter critical section
 *
 * @param [in] ctx OSAL context
 */
void hpm_sdmmc_osal_enter_critical(void *ctx);

/**
 * @brief Exit critical section
 *
 * @param [in] ctx OSAL context
 */
void hpm_sdmmc_osal_exit_critical(void *ctx);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* HPM_SDMMC_OSAL_H */
