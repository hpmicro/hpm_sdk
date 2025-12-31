/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*  HPM example includes. */
#include <stdio.h>
#include "common.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "ping_thread.h"


/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO   (tskIDLE_PRIORITY + 1)
#define NETIF_STA_TASK_PRIO (tskIDLE_PRIORITY + 4)

void Main_task(void *pvParameters);

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    printf("This is an ethernet demo: Ping With Socket API On FreeRTOS\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);
}

int main(void)
{
    /* Initialize bsp */
    bsp_init();

    xTaskCreate(Main_task, "Main", configMINIMAL_STACK_SIZE * 2, NULL, MAIN_TASK_PRIO, NULL);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    for ( ;; ) {

    }
}

void Main_task(void *pvParameters)
{
    (void)pvParameters;

    TimerHandle_t timer_handle;

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    enet_init(ENET);

    /* Initialize lwIP stack */
    tcpip_init(NULL, NULL);
    netif_config(&gnetif);

    /* Start services */
    enet_services(&gnetif);

    /* Initialize ping */
    ping_init(&gnetif);

    xTaskCreate(netif_update_link_status, "netif update status", configMINIMAL_STACK_SIZE * 2, &gnetif, NETIF_STA_TASK_PRIO, NULL);

    timer_handle = xTimerCreate((const char *)NULL,
                                (TickType_t)1000,
                                (UBaseType_t)pdTRUE,
                                (void * const)1,
                                (TimerCallbackFunction_t)timer_callback);

    if (NULL != timer_handle)  {
        xTimerStart(timer_handle, 0);
    }

    vTaskDelete(NULL);
}