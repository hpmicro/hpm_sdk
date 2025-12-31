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
#include "utils.h"
#include "netconf.h"
#include "sys_arch.h"
#include "ethernetif.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/apps/lwiperf.h"
#include "hpm_uart_drv.h"

#ifndef IPERF_UDP_CLIENT_RATE_RGMII
#define IPERF_UDP_CLIENT_RATE_RGMII (1000 * 1024 * 1024)
#endif

#ifndef IPERF_UDP_CLIENT_RATE_RMII
#define IPERF_UDP_CLIENT_RATE_RMII (100 * 1024 * 1024)
#endif

#ifndef IPERF_CLIENT_AMOUNT
#define IPERF_CLIENT_AMOUNT (-1000) /* 10 seconds */
#endif

static void
lwiperf_report(void *arg, enum lwiperf_report_type report_type,
              const ip_addr_t *local_addr, u16_t local_port, const ip_addr_t *remote_addr, u16_t remote_port,
              u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(local_addr);
    LWIP_UNUSED_ARG(local_port);

    LWIP_PLATFORM_DIAG(("iperf report: type=%d, remote: %s:%d, total bytes: %"U32_F", duration in ms: %"U32_F", kbits/s: %"U32_F"\n",
    (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec));

}

static bool select_netif(uint8_t *netif_num)
{
    char code = 0;

    if (*netif_num == 0) {
        printf("====================================================================\n");
        printf("1: RGMII(ENET0)\n");
        printf("2: RMII(ENET1)\n");
        printf("Please enter one of network interface cards above (e.g. 1 or 2): ");
    }

    code = console_try_receive_byte();
    if (code) {
        printf("%c\n", code);
    }

    if (code == '1' || code == '2') {
        code -= '0';
    } else {
        code = 0xff;
    }

    *netif_num = code;

    return code == 0xff ? false : true;
}

static uint8_t select_mode(struct netif *netif, bool *server_mode, bool *tcp, enum lwiperf_client_type *client_type)
{
    char code;

    if (!netif_is_link_up(netif)) {
        printf("Warning: Please make sure that the specified network port is a link-up state!\n");
        return false;
    }

    printf("\n");
    printf("================ Network Interface %d ================\n", netif->num);
    printf("1: TCP Server Mode\n");
    printf("2: TCP Client Mode\n");
    printf("3: UDP Server Mode\n");
    printf("4: UDP Client Mode\n");
    printf("Please enter one of modes above (e.g. 1 or 2 ...): ");
    code = getchar();
    printf("%c\n", code);

    switch (code) {
    case '1':
        *server_mode = true;
        *tcp         = true;
        *client_type = LWIPERF_CLIENT;
        break;

    case '2':
        *server_mode = false;
        *tcp         = true;
        *client_type = LWIPERF_CLIENT;
        break;

    case '3':
        *server_mode = true;
        *tcp         = false;
        *client_type = LWIPERF_CLIENT;
        break;

    case '4':
        *server_mode = false;
        *tcp         = false;
        *client_type = LWIPERF_CLIENT;
        break;

    default:
        return 0;
    }

    return code;
}

void *start_iperf(struct netif *netif)
{
    bool server = false;
    bool tcp = false;
    uint32_t udp_rate;
    ip_addr_t remote_addr;
    void *session;
    enum lwiperf_client_type client_type;
    uint8_t cmd_str_buff[20];

    if (!select_mode(netif, &server, &tcp, &client_type)) {
        return NULL;
    }

    if (server) {
        if (tcp) {
            session = lwiperf_start_tcp_server(netif_ip_addr4(netif), LWIPERF_TCP_PORT_DEFAULT, lwiperf_report, NULL);
        } else {
            session = lwiperf_start_udp_server(netif_ip_addr4(netif), LWIPERF_UDP_PORT_DEFAULT, lwiperf_report, NULL);
        }
    } else {
        while (!fetch_ip_addr_from_serial_terminal(netif->num, cmd_str_buff, sizeof(cmd_str_buff))) {

        }

        ip4addr_aton((char *)cmd_str_buff, &remote_addr);

        if (tcp) {
            session = lwiperf_start_tcp_client(&remote_addr, LWIPERF_TCP_PORT_DEFAULT, LWIPERF_CLIENT, lwiperf_report, netif);
        } else {
            udp_rate = netif->num  == 0 ? IPERF_UDP_CLIENT_RATE_RGMII : IPERF_UDP_CLIENT_RATE_RMII;
            session = lwiperf_start_udp_client(netif_ip_addr4(netif), LWIPERF_UDP_PORT_DEFAULT,
                                                              &remote_addr, LWIPERF_UDP_PORT_DEFAULT, client_type,
                                                              IPERF_CLIENT_AMOUNT, udp_rate, 0,
                                                              lwiperf_report, NULL);
        }
    }

    return session;
}

void iperf(struct netif *netif)
{
    static void *session = 0;
    static uint8_t netif_num = 0;;

    lwiperf_poll_udp_client();

    /* abort */
    if (session != NULL) {
        if (console_try_receive_byte() == ' ') {
            lwiperf_abort(session);
            session = NULL;
            netif_num = 0;
        } else {
            return;
        }
    }

    /* select a network card */
    if (!select_netif(&netif_num)) {
        return;
    }

    /* start iperf */
    session = start_iperf(&netif[netif_num - 1]);
    if (session == NULL) {
        netif_num = 0;
    }
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: Iperf On Multiple Ports\n");
    #else
    printf("This is an ethernet demo: Iperf On Multiple Ports\n");
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

    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    /* Initialize lwIP stack */
    lwip_init();

    /* Initialize network setting, services and apps */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        netif_config(&gnetif[i], i);
        enet_services(&gnetif[i]);
    }

    board_delay_ms(8000);

    while (1) {
        for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
            enet_common_handler(&gnetif[i]);
            iperf(gnetif);
        }
    }

    return 0;
}
