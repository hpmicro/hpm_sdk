/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_tsw_drv.h"
#include "hpm_tsw_phy_adaptive_glue.h"
#include "hpm_tsw_phy_adaptive_util.h"

#ifndef DEST_MAC0
#define DEST_MAC0 (0x982cbcb19f17)
#endif

#ifndef DEST_MAC1
#define DEST_MAC1 (0x381428149078)
#endif

#ifndef DEST_MAC2
#define DEST_MAC2 (0x381428149079)
#endif

#ifndef DEST_MAC3
#define DEST_MAC3 (0x38142814907a)
#endif

#define APP_TSW BOARD_TSW
#define APP_TIMER_INTERVAL (1) /* 1 ms*/

uint8_t mac1[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
uint8_t mac2[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x18};
uint8_t mac3[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x19};

static uint32_t sys_tick = 0;
static tsw_phy_status_t last_status[] = {
    {.tsw_phy_link = tsw_phy_link_unknown},
    {.tsw_phy_link = tsw_phy_link_unknown},
    {.tsw_phy_link = tsw_phy_link_unknown},
};

static const uint8_t dest_port[] = {tsw_dst_port_1, tsw_dst_port_2, tsw_dst_port_3};

static void print_mac_addr(const uint8_t *mac)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void print_lookup_mac(uint64_t mac)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           (unsigned int)((mac >> 40) & 0xffU), (unsigned int)((mac >> 32) & 0xffU),
           (unsigned int)((mac >> 24) & 0xffU), (unsigned int)((mac >> 16) & 0xffU),
           (unsigned int)((mac >> 8) & 0xffU), (unsigned int)(mac & 0xffU));
}

static void print_tsn_switch_port_info(void)
{
    static const uint8_t * const port_mac[] = {mac1, mac2, mac3};
    static const uint64_t lookup_mac[] = {DEST_MAC1, DEST_MAC2, DEST_MAC3};
    static const char * const port_itf[] = {"MII", "MII", "RGMII"};
    static const char * const port_speed[] = {"100Mbps", "100Mbps", "1000Mbps"};

    for (uint8_t i = 0; i < BOARD_TSW_PORT_NUM; i++) {
        printf("Port%u: %s %s, Switch MAC ", i + 1U, port_itf[i], port_speed[i]);
        print_mac_addr(port_mac[i]);
        printf(", Lookup dest MAC ");
        print_lookup_mac(lookup_mac[i]);
        printf("\n");
    }
}

void tsw_self_adaptive_port_speed(void)
{
    for (uint8_t i = 0; i < BOARD_TSW_PORT_NUM; i++) {
        tsw_phy_adaptive_binding_t binding = {
            .last = &last_status[i],
            .tsw_base = BOARD_TSW,
            .tsw_port = i,
            .frame_dst_port = dest_port[i],
            .log_prefix = NULL,
            .print_port_banner = true,
            .notify_netif = false,
        };
        enet_phy_status_t enet_status = {0};
        tsw_phy_status_t status = {0};

        if (board_get_tsw_phy_status(i, &enet_status) != status_success) {
            continue;
        }

        tsw_phy_adaptive_enet_to_tsw_status(&enet_status, &status);
        tsw_phy_adaptive_binding_poll(&binding, &status);
    }
}

void sys_timer_callback(void)
{
    sys_tick++;

    if (sys_tick % (2000 * APP_TIMER_INTERVAL) == 0) {
        tsw_self_adaptive_port_speed();
    }
}

int main(void)
{
    board_init();

    board_init_tsw_pins(APP_TSW);

    board_init_tsw_smi();

    board_reset_tsw_phy(APP_TSW, TSW_TSNPORT_PORT1);
    board_reset_tsw_phy(APP_TSW, TSW_TSNPORT_PORT2);
    board_reset_tsw_phy(APP_TSW, TSW_TSNPORT_PORT3);

    printf("This is a TSN switch demo\n");

    /* Disable All MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(APP_TSW, tsw_mac_type_emac);

    /* Set Mac Address */
    tsw_ep_set_mac_addr(APP_TSW, TSW_TSNPORT_PORT1, mac1, true);
    tsw_ep_set_mac_addr(APP_TSW, TSW_TSNPORT_PORT2, mac2, true);
    tsw_ep_set_mac_addr(APP_TSW, TSW_TSNPORT_PORT3, mac3, true);

    /* Set MAC Mode */
    tsw_ep_set_mac_mode(APP_TSW, TSW_TSNPORT_PORT1, tsw_mac_mode_mii);
    tsw_ep_set_mac_mode(APP_TSW, TSW_TSNPORT_PORT2, tsw_mac_mode_mii);
    tsw_ep_set_mac_mode(APP_TSW, TSW_TSNPORT_PORT3, tsw_mac_mode_gmii);

    /* Set port speed and PHY interface */
    tsw_port_gpr(APP_TSW, TSW_TSNPORT_PORT1, tsw_port_speed_100mbps,  tsw_port_phy_itf_mii, 0, 0);
    tsw_port_gpr(APP_TSW, TSW_TSNPORT_PORT2, tsw_port_speed_100mbps,  tsw_port_phy_itf_mii, 0, 0);
    tsw_port_gpr(APP_TSW, TSW_TSNPORT_PORT3, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0, 0);

    /* Enable All MACs(TX/RX) */
    tsw_ep_enable_all_mac_ctrl(APP_TSW, tsw_mac_type_emac);

    /* Clear CAM */
    tsw_clear_cam(APP_TSW);

    /* Wait for cam clearing completion */
    board_delay_ms(10);

    /* Enable VLAN-ID 1 at all ports */
    tsw_set_cam_vlan_port(APP_TSW);

    /* Set unknown frames to no ports */
    tsw_set_unknown_frame_action(APP_TSW, tsw_dst_port_null);

    /* Set broadcast frames to no ports */
    tsw_set_broadcast_frame_action(APP_TSW, tsw_dst_port_null);

    /* Set a lookup table */
    tsw_set_lookup_table(APP_TSW, 0, tsw_dst_port_1,  DEST_MAC1);
    tsw_set_lookup_table(APP_TSW, 1, tsw_dst_port_2,  DEST_MAC2);
    tsw_set_lookup_table(APP_TSW, 2, tsw_dst_port_3,  DEST_MAC3);

    /* Set all ports to Store & Forward mode */
    tsw_enable_store_forward_mode(APP_TSW, TSW_TSNPORT_PORT1);
    tsw_enable_store_forward_mode(APP_TSW, TSW_TSNPORT_PORT2);
    tsw_enable_store_forward_mode(APP_TSW, TSW_TSNPORT_PORT3);

    if (board_init_tsw_phy(BOARD_TSW_PHY_SMI) == status_success) {
        printf("TSW PHY init passed !\n");
    } else {
        printf("TSW PHY init failed !\n");
    }

    /* Start a board timer */
    board_timer_create(APP_TIMER_INTERVAL, sys_timer_callback);

    printf("\n===========================================================Tips=======================================================================\n");
    printf("Here are three TSN switch ports\n");
    printf("1. Network devices can be connected to TSN switch ports.\n");
    printf("2. If no specified MAC address is defined, frames will be forwarded to all ports.\n");
    printf("3. If one or more specified MAC addresses are defined, frames will be forwarded to the matched destination port(s).\n");
    print_tsn_switch_port_info();
    printf("\n======================================================================================================================================\n");

    tsw_self_adaptive_port_speed();

    while (1) {

    }

    return 0;
}
