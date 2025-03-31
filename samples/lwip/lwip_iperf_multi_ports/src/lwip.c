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
#include "ethernetif.h"
#include "lwip.h"
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

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag[BOARD_ENET_COUNT];
#endif

typedef struct {
    enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT];
    enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT];
    uint8_t        rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE];
    uint8_t        tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE];
} enet_desc_init_t;

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
enet_desc_init_t desc_init[BOARD_ENET_COUNT];
enet_desc_t desc[BOARD_ENET_COUNT];
uint8_t mac[BOARD_ENET_COUNT][ENET_MAC];

struct netif gnetif[BOARD_ENET_COUNT];


/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
static void enet_desc_init(enet_desc_t *pdesc, enet_desc_init_t *pdesc_init)
{
    pdesc->tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_tx_desc_tab);
    pdesc->rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_rx_desc_tab);

    pdesc->tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->tx_buff);
    pdesc->tx_buff_cfg.count  = ENET_TX_BUFF_COUNT;
    pdesc->tx_buff_cfg.size   = ENET_TX_BUFF_SIZE;

    pdesc->rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->rx_buff);
    pdesc->rx_buff_cfg.count  = ENET_RX_BUFF_COUNT;
    pdesc->rx_buff_cfg.size   = ENET_RX_BUFF_SIZE;
}

static hpm_stat_t enet_init(uint8_t idx)
{
    enet_mac_config_t        enet_config;
    enet_tx_control_config_t enet_tx_control_config;
    enet_int_config_t        int_config = {0};
    enet_base_t              *base;
    enet_inf_type_t          itf;

    if (idx > BOARD_ENET_COUNT) {
        return status_invalid_argument;
    }

    base = board_get_enet_base(idx);
    itf  = board_get_enet_phy_itf(idx);

    /* Initialize td, rd and the corresponding buffers */
    enet_desc_init(&desc[idx], &desc_init[idx]);

    /* Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(base, &enet_tx_control_config);

    /* Set the control config for tx descriptor */
    memcpy(&desc[idx].tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get a default MAC address */
    enet_get_mac_address(idx, mac[idx]);

    /* Set MAC0 address */
    enet_set_mac_address(&enet_config, mac[idx]);

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(base);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Enable Enet IRQ */
    board_enable_enet_irq(base);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);
    #endif

    /* Initialize Enet MAC */
    if (enet_controller_init(base, itf, &desc[idx], &enet_config, &int_config) != status_success) {
        printf("Enet%d MAC init failed!\n", idx);
        return status_fail;
    }

    /* Initialize Enet PHY */
    if (board_init_enet_phy(base) != status_success) {
        printf("Enet%d PHY init failed!\n", idx);
        return status_fail;
    }

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(base);
    #endif

    return status_success;
}

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
        if (netif->num == 0) {
            ip4addr_aton(HPM_STRINGIFY(REMOTE_IP0_CONFIG), &remote_addr);
        } else {
            ip4addr_aton(HPM_STRINGIFY(REMOTE_IP1_CONFIG), &remote_addr);
        }

        if (tcp) {
            session = lwiperf_start_tcp_client(&remote_addr, LWIPERF_TCP_PORT_DEFAULT, LWIPERF_CLIENT, lwiperf_report, NULL);
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

    /* Initialize the Lwip stack */
    lwip_init();

    /* Initialize network setting, services and apps */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        netif_config(&gnetif[i], i);
        enet_services(&gnetif[i]);
    }

    board_delay_ms(8000);

    while (1) {
        for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
            ethernetif_input(&gnetif[i]);
            sys_check_timeouts();
            iperf(gnetif);
        }
    }

    return 0;
}
