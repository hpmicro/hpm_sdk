/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_tsw_drv.h"

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

uint8_t mac1[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
uint8_t mac2[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x18};
uint8_t mac3[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x19};

int main(void)
{
    board_init();

    board_init_tsw_pins(APP_TSW);

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

    /* Set unknown frames to all ports */
    tsw_set_unknown_frame_action(APP_TSW, tsw_dst_port_3 | tsw_dst_port_2 | tsw_dst_port_1);

    /* Set broadcast frames to all ports */
    tsw_set_broadcast_frame_action(APP_TSW, tsw_dst_port_3 | tsw_dst_port_2 | tsw_dst_port_1);

    /* Set a lookup table */
    tsw_set_lookup_table(APP_TSW, 0, tsw_dst_port_1,  DEST_MAC1);
    tsw_set_lookup_table(APP_TSW, 1, tsw_dst_port_2,  DEST_MAC2);
    tsw_set_lookup_table(APP_TSW, 2, tsw_dst_port_3,  DEST_MAC3);

    /* Set all ports to Store & Forward mode */
    tsw_enable_store_forward_mode(APP_TSW, TSW_TSNPORT_PORT1);
    tsw_enable_store_forward_mode(APP_TSW, TSW_TSNPORT_PORT2);
    tsw_enable_store_forward_mode(APP_TSW, TSW_TSNPORT_PORT3);

    printf("\n===========================================================Tips=======================================================================\n");
    printf("Here are three TSN switch ports\n");
    printf("1. Network devices can be connected to TSN switch ports.\n");
    printf("2. If no specified MAC address is defined, frames will be forwarded to all ports.\n");
    printf("3. If one or more specified MAC addresses are defined, frames will be forwarded to the matched destination port(s).\n");
    printf("\n======================================================================================================================================\n");

    while (1) {

    }

    return 0;
}

