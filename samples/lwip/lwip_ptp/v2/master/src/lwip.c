/*
 * Copyright (c) 2023-2026 HPMicro
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
/** @brief Initialize PTP timer and PPS output */
static void ptp_init(void)
{
    enet_ptp_config_t cfg = {0};
    enet_ptp_ts_update_t timestamp = {0};

    /* Get default PTP config based on ENET PTP clock frequency */
    enet_get_default_ptp_config(BOARD_ENET_PPS, clock_get_frequency(BOARD_ENET_PPS_PTP_CLOCK), &cfg);
    /* Set update method and initial timestamp */
    cfg.update_method = enet_ptp_time_fine_update;
    timestamp.sec = 1651074120UL;
    timestamp.nsec = 0;
    /* Configure PPS output and set ptp_timestamp in config */
    cfg.mode = enet_pps_fixed_output;
    cfg.param.control_freq = enet_pps_ctrl_pps;
    cfg.pps_enable_output = true;
    cfg.ptp_timestamp = &timestamp;
    /* Initialize PTP timer and set initial time */
    enet_init_ptp(ENET, &cfg);
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

    #if defined(ENET_PPS_PINOUT) && ENET_PPS_PINOUT
    board_init_enet_pps_pins(BOARD_ENET_PPS);
    #endif

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: PTP V2 Master (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: PTP V2 Master (Polling Usage)\n");
    #endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    if (enet_init(ENET) == 0) {
        /* Initialize PTP */
        ptp_init();

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
