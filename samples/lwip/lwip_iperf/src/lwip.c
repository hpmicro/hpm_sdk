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
#include "utils.h"
#include "netconf.h"
#include "sys_arch.h"
#include "ethernetif.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"

#ifndef IPERF_UDP_CLIENT_RATE
#if defined(RGMII) && RGMII
    #define IPERF_UDP_CLIENT_RATE (1000 * 1024 * 1024)
#else
    #define IPERF_UDP_CLIENT_RATE (100 * 1024 * 1024)
#endif
#endif

#ifndef IPERF_CLIENT_AMOUNT
#define IPERF_CLIENT_AMOUNT (-1000) /* 10 seconds */
#endif

static void
lwiperf_report(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t* local_addr, u16_t local_port, const ip_addr_t* remote_addr, u16_t remote_port,
  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(local_addr);
  LWIP_UNUSED_ARG(local_port);

  LWIP_PLATFORM_DIAG(("iperf report: type=%d, remote: %s:%d, total bytes: %"U32_F", duration in ms: %"U32_F", kbits/s: %"U32_F"\n",
    (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec));
}

static bool select_mode(struct netif *netif, bool *server_mode, bool *tcp, enum lwiperf_client_type *client_type)
{
    char code;

    if (!netif_is_link_up(netif)) {
        return false;
    }

    printf("\n");
    printf("1: TCP Server Mode\n");
    printf("2: TCP Client Mode\n");
    printf("3: UDP Server Mode\n");
    printf("4: UDP Client Mode\n");
    printf("Please enter one of modes above (e.g. 1 or 2 ...): ");
    code = getchar();
    printf("%c\n", code);

    switch (code)
    {
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
            return false;
    }

    return true;
}

void *start_iperf(void)
{
    bool server = false;
    bool tcp = false;
    enum lwiperf_client_type client_type;
    void *session;
    ip_addr_t remote_addr;
    uint8_t cmd_str_buff[20];

    if (!select_mode(&gnetif, &server, &tcp, &client_type)) {
        return NULL;
    }

    if (server) {
        if (tcp) {
            session = lwiperf_start_tcp_server_default(lwiperf_report, NULL);
        } else {
            session = lwiperf_start_udp_server(netif_ip_addr4(netif_default), LWIPERF_UDP_PORT_DEFAULT,
                                               lwiperf_report, NULL);
        }
    } else {
        while (!fetch_ip_addr_from_serial_terminal(0, cmd_str_buff, sizeof(cmd_str_buff))) {

        }

        ip4addr_aton((char *)cmd_str_buff, &remote_addr);

        if (tcp) {
            session = lwiperf_start_tcp_client_default(&remote_addr, lwiperf_report, NULL);
        } else {
            session = lwiperf_start_udp_client(netif_ip_addr4(netif_default), LWIPERF_UDP_PORT_DEFAULT,
                                               &remote_addr, LWIPERF_UDP_PORT_DEFAULT, client_type,
                                               IPERF_CLIENT_AMOUNT, IPERF_UDP_CLIENT_RATE, 0,
                                               lwiperf_report, NULL);
        }
    }

    return session;
}

void iperf(void)
{
    static void *session = NULL;

#if defined(LWIP_DHCP) && LWIP_DHCP
    if (netif_dhcp_data(&gnetif)->state == DHCP_STATE_BOUND) {
#endif
        if (session == NULL) {
            session = start_iperf();
        } else {
            if (console_try_receive_byte() == ' ') {
                lwiperf_abort(session);
                session = NULL;
            }
        }

        lwiperf_poll_udp_client();
#if defined(LWIP_DHCP) && LWIP_DHCP
    }
#endif
}

/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: Iperf (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: Iperf (Polling Usage)\n");
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

        /* Start a board timer */
        board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

        while (1) {
            enet_common_handler(&gnetif);
            iperf();
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
