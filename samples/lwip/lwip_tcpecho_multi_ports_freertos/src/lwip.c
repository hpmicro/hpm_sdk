/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*  HPM example includes. */
#include "common.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "tcp_echo.h"

/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO      (tskIDLE_PRIORITY + 5)
#define DHCP_TASK_PRIO      (tskIDLE_PRIORITY + 4)
#define NETIF_STA_TASK_PRIO (tskIDLE_PRIORITY + 4)

void Main_task(void *pvParameters);

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    printf("This is an ethernet demo: TCP Echo With Multi Ports On FreeRTOS\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

   /* Initialize Enet Clock */
    board_init_multiple_enet_clock();
}

int main(void)
{
    /* Initialize bsp */
    bsp_init();

    xTaskCreate(Main_task, "Main", 4096, NULL, MAIN_TASK_PRIO, NULL);

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
    TaskFunction_t pxTaskCode[] = {netif0_update_link_status, netif1_update_link_status};
    char task_name[30] = {0};

    /* Initialize MAC and DMA */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        if (enet_init(i) == status_success) {
            printf("Enet%d init passed!\n", i);
        } else {
            printf("Enet%d init failed!\n", i);
        }
    }

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);

    /* Initialize network setting, services and apps */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        netif_config(&gnetif[i], i);
        enet_services(&gnetif[i]);
        tcp_echo_init(&gnetif[i]);
    }

    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        sprintf(task_name, "Netif%d Link Status Update", i);
        xTaskCreate(pxTaskCode[i], task_name, configMINIMAL_STACK_SIZE * 2, &gnetif[i], NETIF_STA_TASK_PRIO, NULL);
    }

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