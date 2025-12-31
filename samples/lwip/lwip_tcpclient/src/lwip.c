/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "common.h"
#include "netconf.h"
#include "sys_arch.h"
#include "lwip/init.h"
#include "tcp_client.h"

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: TCP Client (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: TCP Client (Polling Usage)\n");
    #endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    if (enet_init(ENET) == 0) {
        /* Initialize lwIP stack */
        lwip_init();

        /* Initialize network interface */
        netif_config(&gnetif);

        /* Start services */
        enet_services(&gnetif);

        /* Initialize TCP client */
        tcp_client_init(&server_ip);

        /* Start a board timer */
        board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

        while (1) {
            tcp_client_reconnect(&gnetif, &server_ip);
            enet_common_handler(&gnetif);
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
