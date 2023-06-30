/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"

#include <rcl/rcl.h>
#include <rmw_microxrcedds_c/config.h>
#include <ucdr/microcdr.h>
#include <uxr/client/client.h>
#include <rmw_microros/rmw_microros.h>

#include "app.h"
#include "allocators.h"
#include "platform_uart.h"
#include "microros_transports.h"
#include "FreeRTOS.h"
#include "task.h"

#define DEFAULT_TASK_PRIORITY (configMAX_PRIORITIES - 3U)
#define MR_TASK_PRIORITY (configMAX_PRIORITIES - 5U)

static void default_task(void *pvParameters)
{
  bool transport_avail = false;

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM
  transport_avail = true;
  rmw_uros_set_custom_transport(
    true,
    (void *) &uart_microros_context,
    hpm_freertos_serial_open,
    hpm_freertos_serial_close,
    hpm_freertos_serial_write,
    hpm_freertos_serial_read);
#endif

  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
  freeRTOS_allocator.allocate = __freertos_allocate;
  freeRTOS_allocator.deallocate = __freertos_deallocate;
  freeRTOS_allocator.reallocate = __freertos_reallocate;
  freeRTOS_allocator.zero_allocate = __freertos_zero_allocate;

  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
    printf("Error on default allocators (line %d)\n", __LINE__);
  }

  if (xTaskCreate(appMain, "microros_app", 3 * 3000, NULL, MR_TASK_PRIORITY, NULL) != pdPASS) {
    printf("microros_app creation failed!.\n");

    for (;;) {
    }
  }
  vTaskDelay(500);

  TaskHandle_t xHandle;
  xHandle = xTaskGetHandle("microros_app");

  while (1) {
    if (eTaskGetState(xHandle) != eSuspended && transport_avail) {
        vTaskDelay(100);
    }
  }
}

int main(void)
{
    board_init();
    board_init_gpio_pins();
    board_init_uart(BOARD_MICROROS_UART_BASE);

    if (xTaskCreate(default_task, "default_task", 3 * 1500, NULL, DEFAULT_TASK_PRIORITY, NULL) != pdPASS) {
        printf("Task default failed!.\n");
        for (;;) {
            ;
        }
    }

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}