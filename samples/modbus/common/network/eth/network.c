/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "common_lwip.h"
#include "netconf.h"
#include "sys_arch.h"
#include "lwip/init.h"
#include "lwipopts.h"
#include "board.h"

hpm_stat_t network_init(void)
{
    hpm_stat_t sta = status_success;

#if __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: modbus tcp(Interrupt Usage)\n");
#else
    printf("This is an ethernet demo: modbus tcp (Polling Usage)\n");
#endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    sta = enet_init(ENET);
    if (sta == status_success) {
        /* Initialize the Lwip stack */
        lwip_init();
        netif_config();
        user_notification(&gnetif);
        /* Start services */
        enet_services(&gnetif);
    }
    return sta;
}
