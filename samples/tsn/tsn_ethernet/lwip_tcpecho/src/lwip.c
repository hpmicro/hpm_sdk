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
#include "lwip.h"
#include "lwip/init.h"
#include "tcp_echo.h"
#include "hpm_tsw_drv.h"

struct netif gnetif;
uint8_t mac[TSW_ENET_MAC];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t send_buff[TSW_SEND_DESC_COUNT][TSW_SEND_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t recv_buff[TSW_RECV_DESC_COUNT][TSW_RECV_BUFF_LEN];

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
volatile bool rx_flag;
tsw_frame_t frame[TSW_FRAME_BUFF_COUNT];
#endif

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t tsw_init(TSW_Type *ptr)
{
    rtl8211_config_t phy_config;
    tsw_dma_config_t config;

    /* Disable all MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Get Mac address */
    tsw_get_mac_address(mac);

    /* Set Mac Address */
    tsw_ep_set_mac_addr(ptr, BOARD_TSW_PORT, mac, true);

    /* Set MAC Mode: GMII, CLKSEL: refclk */
    tsw_ep_set_mac_mode(ptr, BOARD_TSW_PORT, BOARD_TSW_PORT_ITF ==  tsw_port_phy_itf_rgmii ? tsw_mac_mode_gmii : tsw_mac_mode_mii);

    /* Set port PHY interface */
    tsw_set_port_interface(ptr, BOARD_TSW_PORT, BOARD_TSW_PORT_ITF);

    /* Enable all MACs(TX/RX) */
    tsw_ep_enable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Clear CAM */
    tsw_clear_cam(ptr);

    /* Wait for cam clearing completion */
    board_delay_ms(10);

    /* Enable VLAN-ID 1 at all ports */
    tsw_set_cam_vlan_port(ptr);

    /* Set system cycle numbers */
    tsw_cb_frer_set_msec_cycles(ptr, 0);

    /* Get the default DMA config */
    tsw_get_default_dma_config(&config);

    /* Initialize DMA for sending */
    tsw_init_send(ptr, &config);

    for (uint8_t i = 0; i < TSW_SEND_DESC_COUNT; i++) {
        *send_buff[i] = BOARD_TSW_PORT + 1;
    }

    /* Initialize DMA for receiving */
#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
    config.irq = true;
#endif
    tsw_init_recv(ptr, &config);

    for (uint8_t i = 0; i < TSW_RECV_DESC_COUNT; i++) {
        tsw_commit_recv_desc(ptr, recv_buff[i], TSW_RECV_BUFF_LEN, i);
    }

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
    intc_m_enable_irq(IRQn_TSW_0);  /* Enable TSW CPU Port IRQ */
#endif

    /* Set broadcast frame and unknown frame actions */
    tsw_set_broadcast_frame_action(BOARD_TSW, tsw_dst_port_cpu);
    tsw_set_unknown_frame_action(BOARD_TSW, tsw_dst_port_cpu);

    /* Set MDC clock frequency to 2.5MHz */
    tsw_ep_set_mdio_config(BOARD_TSW, BOARD_TSW_PORT, 19);

    /* Initialize PHY */
    rtl8211_reset(ptr, BOARD_TSW_PORT);
    rtl8211_basic_mode_default_config(ptr, &phy_config);
    if (rtl8211_basic_mode_init(ptr, BOARD_TSW_PORT, &phy_config) == true) {
        printf("TSW phy init passed !\n");
        return status_success;
    } else {
        printf("TSW phy init failed !\n");
        return status_fail;
    }

    return status_success;
}

/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_tsw_pins(BOARD_TSW);

    /* Reset an TSW PHY */
    board_reset_tsw_phy(BOARD_TSW, BOARD_TSW_PORT);

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
    printf("This is a TSW demo: TCP Echo (Interrupt Usage)\n");
#else
    printf("This is a TSW demo: TCP Echo (Polling Usage)\n");
#endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    #if defined(RGMII) && RGMII
    board_init_tsw_rgmii_clock_delay(BOARD_TSW, BOARD_TSW_PORT);
    #endif

    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    /* Initialize MAC and DMA */
    if (tsw_init(BOARD_TSW) == 0) {
        /* Initialize the Lwip stack */
        lwip_init();
        netif_config(&gnetif);

        /* Start services */
        tsw_services(&gnetif);

        /* Initialize TCP echo */
        tcp_echo_init();

        while (1) {
            tsw_common_handler(&gnetif);
        }
    } else {
        printf("TSW initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
