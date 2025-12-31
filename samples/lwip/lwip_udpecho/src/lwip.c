/*
 * Copyright (c) 2024-2025 HPMicro
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
#include "udp_echo.h"

/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: UDP Echo (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: UDP Echo (Polling Usage)\n");
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

        /* Initialize UDP echo */
        udp_echo_init();

        /* Start a board timer */
        board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

        while (1) {
            enet_common_handler(&gnetif);
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
