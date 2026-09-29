/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "cmsis_os.h"

/*  HPM example includes. */
#include "common.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "opener.h"
#include "bldc_foc.h"

/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO      osPriorityLow
#define DHCP_TASK_PRIO      osPriorityBelowNormal
#define NETIF_STA_TASK_PRIO osPriorityBelowNormal
#define APP_TASK_PRIO       (osPriorityAboveNormal + 5U) /* higher than OpENer thread (osPriorityAboveNormal=32) */

void Main_task(void *pvParameters);

static void netif_link_callback(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
        opener_init(netif);
    }
}

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    board_init_gpio_pins();

    board_init_led_pins();

    printf("This is an Ethernet/IP demo.\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);
}

int main(void)
{
    /* Initialize bsp */
    bsp_init();

    osKernelInitialize();
    {
        const osThreadAttr_t main_attr = {
            .name = "Main",
            .stack_size = configMINIMAL_STACK_SIZE * 2U * 4U,
            .priority = MAIN_TASK_PRIO,
        };
        (void)osThreadNew(Main_task, NULL, &main_attr);
    }

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    for ( ;; ) {

    }
}

void Main_task(void *pvParameters)
{
    (void)pvParameters;

    osTimerId_t timer_handle;
    setup_moter();

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    enet_init(ENET);

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);
    netif_config(&gnetif);

    /* Override the link callback to directly call opener_init on link up */
    netif_set_link_callback(&gnetif, netif_link_callback);

    /* Start services */
    enet_services(&gnetif);

#if defined(LWIP_DHCP) && LWIP_DHCP
    /* Start DHCP Client */
    {
        const osThreadAttr_t dhcp_attr = {
            .name = "DHCP",
            .stack_size = configMINIMAL_STACK_SIZE * 2U * 4U,
            .priority = DHCP_TASK_PRIO,
        };
        (void)osThreadNew(LwIP_DHCP_task, &gnetif, &dhcp_attr);
    }
#endif

    {
        const osThreadAttr_t netif_attr = {
            .name = "netif update status",
            .stack_size = configMINIMAL_STACK_SIZE * 2U * 4U,
            .priority = NETIF_STA_TASK_PRIO,
        };
        (void)osThreadNew(netif_update_link_status, &gnetif, &netif_attr);
    }

    timer_handle = osTimerNew((osTimerFunc_t)timer_callback, osTimerPeriodic, (void *)1, NULL);
    if (NULL != timer_handle) {
        osTimerStart(timer_handle, 1000U);
    }

    osThreadExit();
}
