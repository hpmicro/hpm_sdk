/*
 * Copyright (c) 2023-2025 HPMicro
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
#include "tcp_echo.h"

/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: TCP Echo On Multiple Ports (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: TCP Echo On Multiple Ports (Polling Usage)\n");
    #endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize Enet Clock */
    board_init_multiple_enet_clock();

    /* Initialize MAC and DMA */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        if (enet_init(i) == status_success) {
            printf("Enet%d init passed!\n", i);
        } else {
            printf("Enet%d init failed!\n", i);
        }
    }

    /* Initialize lwIP stack */
    lwip_init();

    /* Initialize network setting, services and apps */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        netif_config(&gnetif[i], i);
        enet_services(&gnetif[i]);
        tcp_echo_init(&gnetif[i]);
    }

    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    while (1) {
        for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
            enet_common_handler(&gnetif[i]);
        }
    }

    return 0;
}
