/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "hpm_serial_nor_host.h"

#if defined(USE_FREERTOS) && USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#ifndef HPM_SERIAL_NOR_SPI_RETRY_COUNT
#define HPM_SERIAL_NOR_SPI_RETRY_COUNT         (0xFFFFFF)
#endif


void serial_nor_host_delay(uint32_t ms)
{
#if defined(USE_FREERTOS) && USE_FREERTOS
    vTaskDelay(pdMS_TO_TICKS(ms));
#else
    board_delay_ms(ms);
#endif
}