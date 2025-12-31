/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_tsw_drv.h"
#include "hpm_enet_drv.h"
#include "hpm_enet_phy_common.h"

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

typedef void (*enet_get_phy_status)(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status);

uint8_t mac1[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
uint8_t mac2[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x18};
uint8_t mac3[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x19};

static uint32_t sys_tick = 0;
static enet_phy_status_t last_status[] = {{.enet_phy_link = enet_phy_link_unknown}, {.enet_phy_link = enet_phy_link_unknown}, {.enet_phy_link = enet_phy_link_unknown}};

hpm_stat_t jl111_phy_init(ENET_Type *ptr)
{
    jl1111_config_t phy_config;
    hpm_stat_t stat1, stat2;

    /* Switch to smi group 0 */
    board_switch_tsw_smi_group(BOARD_TSW_PORT1_SMI_GROUP);

    /* Initialize PHY on TSW port 1 */
    jl1111_reset(ptr, BOARD_TSW_PORT1_PHY_ADDR);
    jl1111_basic_mode_default_config(ptr, &phy_config);
    if (jl1111_basic_mode_init(ptr, BOARD_TSW_PORT1_PHY_ADDR, &phy_config) == true) {
        printf("TSW PHY on Port1 init passed !\n");
       stat1 = status_success;
    } else {
        printf("TSW PHY on Port1 init failed !\n");
       stat1 = status_fail;
    }

    /* Initialize PHY on TSW port 2 */
    board_switch_tsw_smi_group(BOARD_TSW_PORT2_SMI_GROUP);
    jl1111_reset(ptr, BOARD_TSW_PORT2_PHY_ADDR);
    jl1111_disable_broadcast_response(ptr, BOARD_TSW_PORT2_PHY_ADDR);
    jl1111_basic_mode_default_config(ptr, &phy_config);
    if (jl1111_basic_mode_init(ptr, BOARD_TSW_PORT2_PHY_ADDR, &phy_config) == true) {
        printf("TSW PHY on Port2 init passed !\n");
        stat2 = status_success;
    } else {
        printf("TSW PHY on Port2 init failed !\n");
       stat2 = status_fail;
    }

    return stat1 || stat2;
}

hpm_stat_t rtl8211_phy_init(ENET_Type *ptr)
{
    rtl8211_config_t phy_config;

    /* Switch to smi group 3 */
    board_switch_tsw_smi_group(BOARD_TSW_PORT3_SMI_GROUP);

    /* Initialize PHY on TSW port 3 */
    rtl8211_reset(ptr, BOARD_TSW_PORT3_PHY_ADDR);
    rtl8211_basic_mode_default_config(ptr, &phy_config);
    if (rtl8211_basic_mode_init(ptr, BOARD_TSW_PORT3_PHY_ADDR, &phy_config) == true) {
        printf("TSW PHY on Port3 init passed !\n");
        return status_success;
    } else {
        printf("TSW PHY on Port3 init failed !\n");
        return status_fail;
    }
}

void enet_self_adaptive_port_speed(void)
{
    enet_phy_status_t status = {0};
    uint8_t smi_group[] = {BOARD_TSW_PORT1_SMI_GROUP, BOARD_TSW_PORT2_SMI_GROUP, BOARD_TSW_PORT3_SMI_GROUP};
    enet_line_speed_t port_speed[] = {enet_line_speed_10mbps, enet_line_speed_100mbps, enet_line_speed_1000mbps};
    enet_get_phy_status get_phy_status[] = {jl1111_get_phy_status, jl1111_get_phy_status, rtl8211_get_phy_status};
    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};
    uint8_t dest_port[] = {tsw_dst_port_1, tsw_dst_port_2, tsw_dst_port_3};

    for (uint8_t i = 0; i < BOARD_TSW_PORT_NUM; i++) {
        board_switch_tsw_smi_group(smi_group[i]);
        get_phy_status[i](BOARD_TSW_PHY_SMI, i, &status);
        if (status.enet_phy_link || (status.enet_phy_link != last_status[i].enet_phy_link)) {
            if (memcmp(&last_status[i], &status, sizeof(enet_phy_status_t)) != 0) {
                memcpy(&last_status[i], &status, sizeof(enet_phy_status_t));

                printf("================ TSW PHY on Port%d ================\n", i+1);
                if (status.enet_phy_link) {
                    printf("Link Status: Up\n");
                    printf("Link Speed:  %s\n", speed_str[status.enet_phy_speed]);
                    printf("Link Duplex: %s\n", duplex_str[status.enet_phy_duplex]);

                    tsw_set_port_speed(BOARD_TSW, i, port_speed[status.enet_phy_speed]);
                    tsw_set_unknown_frame_action(APP_TSW, dest_port[i]);
                    tsw_set_broadcast_frame_action(APP_TSW, dest_port[i]);

                    if (!status.enet_phy_duplex) {
                        printf("Error: PHY is in half duplex now, but TSW MAC supports only full duplex mode!\n");
                        return;
                    }
                } else {
                    printf("Link Status: Down\n");
                    tsw_clear_unknown_frame_action(APP_TSW, dest_port[i]);
                    tsw_clear_broadcast_frame_action(APP_TSW, dest_port[i]);
                }
            }
        }
    }
}

void sys_timer_callback(void)
{
    sys_tick++;

    if (sys_tick % (2000 * APP_TIMER_INTERVAL) == 0) {
        enet_self_adaptive_port_speed();
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

    /* JL1111 initialization */
    jl111_phy_init(BOARD_TSW_PHY_SMI);

    /* RTL8211 initialization */
    rtl8211_phy_init(BOARD_TSW_PHY_SMI);

    /* Start a board timer */
    board_timer_create(APP_TIMER_INTERVAL, sys_timer_callback);

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

