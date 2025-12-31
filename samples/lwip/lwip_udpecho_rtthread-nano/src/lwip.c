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
#include "udp_echo.h"


#define TIMER_TIMEOUT_CNT    1000

/*--------------- Tasks Priority -------------*/
#define IDLE_TASK_PRIO      (RT_THREAD_PRIORITY_MAX - 1)
#define MAIN_TASK_PRIO      (IDLE_TASK_PRIO - 1)
#define DHCP_TASK_PRIO      (IDLE_TASK_PRIO - 4)
#define NETIF_STA_TASK_PRIO (IDLE_TASK_PRIO - 4)

void Main_task(void *pvParameters);
void thread_entry(void *arg);

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    printf("This is an ethernet demo: UDP Echo On RTThread-Nano\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);
}

void rt_hw_board_init(void)
{
    /* Initialize bsp */
    bsp_init();

    rtt_base_init();
}

int rtthread_init(void)
{
    rt_thread_t main_thread = rt_thread_create("main", thread_entry, NULL, 2048, MAIN_TASK_PRIO, 10);
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

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    enet_init(ENET);

    /* Initialize lwIP stack */
    tcpip_init(NULL, NULL);
    netif_config(&gnetif);

    /* Start services */
    enet_services(&gnetif);

    udp_echo_init();

    rt_thread_t netif_update_link_status_thread = rt_thread_create("netif update status", netif_update_link_status, &gnetif, 1024, NETIF_STA_TASK_PRIO, 10);
    rt_thread_startup(netif_update_link_status_thread);

    timer = rt_timer_create("timer", timer_callback, RT_NULL, TIMER_TIMEOUT_CNT, RT_TIMER_FLAG_PERIODIC);
    rt_timer_start(timer);
}
