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
#include "lwip/init.h"
#include "ptpd.h"

uint32_t glocaltime;

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
static void enet_ptp_init(void)
{
    enet_ptp_config_t config;
    enet_ptp_ts_update_t timestamp;

    /* initialize ptp timer */
    enet_get_default_ptp_config(BOARD_ENET_PPS, clock_get_frequency(BOARD_ENET_PPS_PTP_CLOCK), &config);
    config.update_method = enet_ptp_time_fine_update;
    enet_init_ptp(ENET, &config);

    /* set the initial timestamp */
    timestamp.sec = 1651074120UL;
    timestamp.nsec = 0;
    enet_set_ptp_timestamp(ENET, &timestamp);
}

static void local_timer_callback(void)
{
    glocaltime += LWIP_APP_TIMER_INTERVAL;
}

void timer_callback(void)
{
    local_timer_callback();
    sys_timer_callback();
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize PTP clock */
    board_init_enet_ptp_clock(ENET);

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: PTP V1 Master (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: PTP V1 Master (Polling Usage)\n");
    #endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    if (enet_init(ENET) == 0) {
        /* Initialize PTP */
        enet_ptp_init();

        #if defined(ENET_PPS_PINOUT) && ENET_PPS_PINOUT
        enet_set_pps0_control_output(BOARD_ENET_PPS, enet_pps_ctrl_pps);
        #endif

        /* Initialize lwIP stack */
        lwip_init();
        netif_config(&gnetif);

        /* Start services */
        enet_services(&gnetif);

        /* Initialize ptpd */
        ptpd_Init();

        /* Initialize a board timer */
        board_timer_create(LWIP_APP_TIMER_INTERVAL, timer_callback);

        while (1) {
            enet_common_handler(&gnetif);
            if (netif_is_link_up(&gnetif)) {
                ptpd_periodic_handle(glocaltime);
            }
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
