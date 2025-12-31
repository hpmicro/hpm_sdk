/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "rtt_port.h"
#include <rtthread.h>

#include "common.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "tcp_echo.h"

#define TIMER_TIMEOUT_CNT    1000

/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO      (IDLE_TASK_PRIO - 5)
#define NETIF_STA_TASK_PRIO (IDLE_TASK_PRIO - 4)

void Main_task(void *pvParameters);
void thread_entry(void *arg);

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    printf("This is an ethernet demo: TCP Echo With Multi Ports On RTThread-Nano\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

   /* Initialize Enet Clock */
    board_init_multiple_enet_clock();
}

void rt_hw_board_init(void)
{
    /* Initialize bsp */
    bsp_init();

    rtt_base_init();
}

int rtthread_init(void)
{
    rt_thread_t main_thread = rt_thread_create("main", thread_entry, NULL, 4096, MAIN_TASK_PRIO, 10);
    rt_thread_startup(main_thread);

    return 0;
}

int main(void)
{
    rtthread_init();

    return 0;
}

void thread_entry(void *arg)
{
    (void)arg;

    static rt_timer_t timer = RT_NULL;
    osTaskFunction_t pxTaskCode[] = {netif0_update_link_status, netif1_update_link_status};
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
        rt_thread_t netif_update_link_status_thread = rt_thread_create(task_name, pxTaskCode[i], &gnetif[i], 1024, NETIF_STA_TASK_PRIO, 10);
        rt_thread_startup(netif_update_link_status_thread);
    }

    timer = rt_timer_create("timer", timer_callback, RT_NULL, TIMER_TIMEOUT_CNT, RT_TIMER_FLAG_PERIODIC);
    rt_timer_start(timer);
}

