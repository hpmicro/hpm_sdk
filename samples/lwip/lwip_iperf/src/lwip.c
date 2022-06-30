/*
 * Copyright (c) 2021-2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "board.h"
#include "hpm_enet_drv.h"
#include "hpm_enet_soc_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_clock_drv.h"
#include "netconf.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "lwip.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/timeouts.h"

#if RGMII
    #include "hpm_dp83867.h"
    #include "hpm_dp83867_regs.h"
#else
    #if defined __USE_DP83864
        #include "hpm_dp83848.h"
        #include "hpm_dp83848_regs.h"
    #elif defined  __USE_RTL8201
        #include "hpm_rtl8201.h"
        #include "hpm_rtl8201_regs.h"
    #else
        #error no specified Ethernet PHY !!!
    #endif
#endif

#define LWIP_APP_DELAY_INTERVAL (1U)  /* 1 ms */

#ifndef IPERF_UDP_CLIENT_RATE
#if RGMII
    #define IPERF_UDP_CLIENT_RATE (1000 * 1024 * 1024)
#else
    #define IPERF_UDP_CLIENT_RATE (100 * 1024 * 1024)
#endif
#endif

#ifndef IPERF_CLIENT_AMOUNT
#define IPERF_CLIENT_AMOUNT (-1000) /* 10 seconds */
#endif

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT] ; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT] ; /* Ethernet Tx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;


/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t enet_init(ENET_Type *ptr)
{
    uint32_t intr = 0;
    enet_mac_config_t enet_config;
    #if RGMII == 1
        dp83867_config_t phy_config;
    #else
        #if __USE_DP83864
        dp83848_config_t phy_config;
        #else
        rtl8201_config_t phy_config;
        #endif
    #endif

    /* Initialize td, rd and the corresponding buffers */
    memset((uint8_t *)dma_tx_desc_tab, 0x00, sizeof(dma_tx_desc_tab));
    memset((uint8_t *)dma_rx_desc_tab, 0x00, sizeof(dma_rx_desc_tab));
    memset((uint8_t *)rx_buff, 0x00, sizeof(rx_buff));
    memset((uint8_t *)tx_buff, 0x00, sizeof(tx_buff));

    desc.tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_tx_desc_tab);
    desc.rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_rx_desc_tab);

    desc.tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tx_buff);
    desc.tx_buff_cfg.count = ENET_TX_BUFF_COUNT;
    desc.tx_buff_cfg.size = ENET_TX_BUFF_SIZE;

    desc.rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rx_buff);
    desc.rx_buff_cfg.count = ENET_RX_BUFF_COUNT;
    desc.rx_buff_cfg.size = ENET_RX_BUFF_SIZE;

    /* Set mac0 address */
    enet_config.mac_addr_high[0] = MAC_ADDR5 << 8 | MAC_ADDR4;
    enet_config.mac_addr_low[0]  = MAC_ADDR3 << 24 | MAC_ADDR2 << 16 | MAC_ADDR1 << 8 | MAC_ADDR0;
#if 0
    enet_config.mac_addr_high[1] = ENET_MAC_ADDR_HIGH_AE_MASK | MAC_ADDR5 << 8 | MAC_ADDR4;
    enet_config.mac_addr_low[1] = MAC_ADDR3 << 24 | MAC_ADDR2 << 16 | MAC_ADDR1 << 8 | MAC_ADDR0;

    enet_config.mac_addr_high[2] = ENET_MAC_ADDR_HIGH_AE_MASK | 0xff << 8 | 0xff;
    enet_config.mac_addr_low[2] = 0xff << 24 | 0xff << 16 | 0xff << 8 | 0xff;

    enet_config.valid_max_count  = 3;
#else
    enet_config.valid_max_count = 1;
#endif
    /* Set RGMII or RMII clock */
    #if RGMII
        enet_rgmii_enable_clock(ptr);
    #else
        board_init_enet_rmii_reference_clock(ptr, BOARD_ENET_RMII_INT_REF_CLK);
        enet_rmii_enable_clock(ptr, BOARD_ENET_RMII_INT_REF_CLK);
    #endif

    /* Set RGMII clock delay */
    #if RGMII
        enet_rgmii_set_clock_delay(ptr, BOARD_ENET_RGMII_TX_DLY, BOARD_ENET_RGMII_RX_DLY);
    #endif

    /* Initialize enet controller */
    enet_controller_init(ptr, ENET_INF_TYPE, &desc, &enet_config, intr);

    /* Initialize phy */
    #if RGMII
        dp83867_reset(ptr);
        dp83867_basic_mode_default_config(ptr, &phy_config);
        if (dp83867_basic_mode_init(ptr, &phy_config) == true) {
    #else
        #if __USE_DP83864
        dp83848_reset(ptr);
        dp83848_basic_mode_default_config(ptr, &phy_config);
        if (dp83848_basic_mode_init(ptr, &phy_config) == true) {
        #else
        rtl8201_reset(ptr);
        rtl8201_basic_mode_default_config(ptr, &phy_config);
        if (rtl8201_basic_mode_init(ptr, &phy_config) == true) {
        #endif
    #endif
            printf("Enet phy init passes !\n");
            return status_success;
        } else {
            printf("Enet phy init fails !\n");
            return status_fail;
        }
}

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

static void select_mode(bool *server_mode, bool *tcp, enum lwiperf_client_type *client_type)
{
    char code;

    printf("\n");
    printf("1: TCP Server Mode\n");
    printf("2: TCP Client Mode\n");
    printf("3: UDP Server Mode\n");
    printf("4: UDP Client Mode\n");
    printf("Please enter the test mode code: ");
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
            break;
    }
}

void *start_iperf(void)
{
    bool server = false;
    bool tcp = false;
    enum lwiperf_client_type client_type;
    void *session;
    ip_addr_t remote_addr;

    select_mode(&server, &tcp, &client_type);

    if (server) {
        if (tcp) {
            session = lwiperf_start_tcp_server_default(lwiperf_report, NULL);
        } else {
            session = lwiperf_start_udp_server(netif_ip_addr4(netif_default), LWIPERF_UDP_PORT_DEFAULT,
                                               lwiperf_report, NULL);
        }
    } else {
        IP_ADDR4(&remote_addr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
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

    if (session == NULL) {
        session = start_iperf();
    }

    lwiperf_poll_udp_client();
}


void lwip_timer_callback(void);
/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize Clocks */
    board_init();

    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    printf("This is an ethernet demo: Iperf\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);
    printf("Local IP: %d.%d.%d.%d\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    printf("Speed Rate:%s\n", RGMII == 1 ? "1000Mbps" : "100Mbps");

    #if RGMII == 0
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
    #endif

    /* Initialize MAC and DMA */
    if (enet_init(ENET) == 0) {
         /* Initialize the Lwip stack */
        lwip_init();
        board_timer_create(LWIP_APP_DELAY_INTERVAL, lwip_timer_callback);
        netif_config();
        user_notification(&gnetif);
        
        while (1) {
            ethernetif_input(&gnetif);
            sys_check_timeouts();
            iperf();
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1);
    }

    return 0;
}
