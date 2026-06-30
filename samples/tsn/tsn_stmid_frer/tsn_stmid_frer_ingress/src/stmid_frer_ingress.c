/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "stmid_frer_ingress.h"

static volatile tsw_phy_status_t last_status = {.tsw_phy_link = tsw_phy_link_unknown};
uint8_t mac[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
uint8_t mac_dst[] = {0x38, 0x14, 0x28, 0x14, 0x90, 0x78};

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t send_buff[TSW_SEND_DESC_COUNT][TSW_SEND_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t recv_buff[TSW_RECV_DESC_COUNT][TSW_RECV_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_INIT_WITH_ALIGNMENT(4) uint8_t data_buff[] = {
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x38, 0x14, 0x28, 0x14, 0x90, 0x78,
0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17,
0x08, 0x06,
0x00, 0x01,
0x08, 0x00,
0x06,
0x04,
0x00, 0x01,
0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17,
0xc0, 0xa8, 0x64, 0x0a,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xc0, 0xa8, 0x64, 0x05,
};

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t tsw_init(TSW_Type *ptr)
{
    tsw_dma_config_t config;

    /* Disable all MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_emac);

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

    /* Get the default DMA config */
    tsw_get_default_dma_config(&config);

    /* Initialize DMA for sending */
    tsw_init_send(ptr, &config);

    /* Initialize DMA for receiving */
    tsw_init_recv(ptr, &config);

    for (uint8_t i = 0; i < TSW_RECV_DESC_COUNT; i++) {
        tsw_commit_recv_desc(ptr, recv_buff[i], TSW_RECV_BUFF_LEN, i);
    }

    /* Set MDC clock frequency to 2.5MHz */
    tsw_ep_set_mdio_config(BOARD_TSW, BOARD_TSW_PORT, 19);

    /* Initialize PHY */
    if (board_init_tsw_port_phy(ptr) == status_success) {
        printf("TSW phy init passed !\n");
        return status_success;
    } else {
        printf("TSW phy init failed !\n");
        return status_fail;
    }

    return status_success;
}

bool tsw_get_link_status(void)
{
    return (last_status.tsw_phy_link == tsw_phy_link_up) ? true : false;
}

void tsw_self_adaptive_port_speed(void)
{
    tsw_phy_status_t status = {0};
    tsw_port_speed_t port_speed[] = {tsw_port_speed_10mbps, tsw_port_speed_100mbps, tsw_port_speed_1000mbps};
    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};

    board_get_tsw_port_phy_status(BOARD_TSW_PORT, &status);

    if (status.tsw_phy_link || (status.tsw_phy_link != last_status.tsw_phy_link)) {
        if (memcmp((uint8_t *)&last_status, &status, sizeof(tsw_phy_status_t)) != 0) {
            memcpy((uint8_t *)&last_status, &status, sizeof(tsw_phy_status_t));
            if (status.tsw_phy_link) {
                printf("Link Status: Up\n");
                printf("Link Speed:  %s\n", speed_str[status.tsw_phy_speed]);
                printf("Link Duplex: %s\n", duplex_str[status.tsw_phy_duplex]);

                tsw_set_port_speed(BOARD_TSW, BOARD_TSW_PORT, port_speed[status.tsw_phy_speed]);

                if (!status.tsw_phy_duplex) {
                    printf("Error: PHY is in half duplex now, but TSW MAC supports only full duplex mode!\n");
                    return;
                }
            } else {
                printf("Link Status: Down\n");
            }
        }
    }
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
    uint8_t stream_sid;
    tsw_cb_stmid_entry_t cb_stmid_entry;

    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_tsw_pins(BOARD_TSW);

    /* Reset an TSW PHY */
    board_reset_tsw_phy(BOARD_TSW, BOARD_TSW_PORT);

    printf("This is a TSW demo: CB Stmid Frer Ingress\n");

    #if defined(HPM_TSW_RGMII) && HPM_TSW_RGMII
    board_init_tsw_rgmii_clock_delay(BOARD_TSW, BOARD_TSW_PORT);
    #endif

    board_timer_create(TSW_APP_TIMER_INTERVAL, tsw_self_adaptive_port_speed);

    /* Set RTC increment */
    tsw_set_rtc_time_increment(BOARD_TSW, (10 << 24));

    /* Initialize MAC and DMA */
    if (tsw_init(BOARD_TSW) == 0) {
        memcpy(send_buff[0], data_buff, sizeof(data_buff));

        cb_stmid_entry.idx = 0;
        cb_stmid_entry.sid = 1;
        cb_stmid_entry.seqnum = 0;
        cb_stmid_entry.seqgen = true;
        cb_stmid_entry.actctl = tsw_stmid_actctl_disabled;
        cb_stmid_entry.smac = tsw_stmid_control_lookup_by_dest_mac;
        cb_stmid_entry.mode = tsw_stmid_lookup_mode_all;
        cb_stmid_entry.enable = true;
        cb_stmid_entry.lookup_mac.mach = MAC_HI(mac_dst);
        cb_stmid_entry.lookup_mac.macl = MAC_LO(mac_dst);
        tsw_cb_stmid_ingress_set_entry(BOARD_TSW, &cb_stmid_entry);
        tsw_cb_frer_ingress_enable_rtag(BOARD_TSW);
        stream_sid = cb_stmid_entry.sid;

        while (1) {
            if (tsw_get_link_status()) {
                tsw_set_tx_hdr_route((tx_hdr_desc_t *)send_buff[0], TSW_CPU_SEND_TO_PORT(BOARD_TSW_PORT), tsw_traffic_queue_0, 1);
                tsw_set_tx_hdr_stream((tx_hdr_desc_t *)send_buff[0], stream_sid, 0);
                tsw_send_frame(BOARD_TSW, send_buff[0], TSW_DMA_FRAME_LEN(data_buff), 0);
                board_delay_ms(500);

                memset(&cb_stmid_entry, 0x00, sizeof(cb_stmid_entry));
                cb_stmid_entry.idx = 0;
                tsw_cb_stmid_ingress_get_entry(BOARD_TSW, &cb_stmid_entry);
                printf("Stream Match Count: %d,  SeqNo: %d\n", cb_stmid_entry.match, cb_stmid_entry.seqnum);
            }
        }
    } else {
        printf("TSW initialization fails !!!\n");

        while (1) {

        }
    }

    return 0;
}
