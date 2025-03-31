/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "tas.h"

static tsw_phy_status_t last_status = {.tsw_phy_link = tsw_phy_link_unknown};
static uint32_t last_op_gs = 0xff;
static uint32_t cur_op_gs = 0;
uint8_t mac[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t send_buff[TSW_SEND_DESC_COUNT][TSW_SEND_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t recv_buff[TSW_RECV_DESC_COUNT][TSW_RECV_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_INIT_WITH_ALIGNMENT(4)  uint8_t data_buff[] = {
0x00, 0x00, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
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

tsw_tas_controllist_entry_t entry[3];
/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t tsw_init(TSW_Type *ptr)
{
    rtl8211_config_t phy_config;
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

    for (uint8_t i = 0; i < TSW_SEND_DESC_COUNT; i++) {
        *send_buff[i] = BOARD_TSW_PORT + 1;
    }

    /* Initialize DMA for receiving */
    tsw_init_recv(ptr, &config);

    for (uint8_t i = 0; i < TSW_RECV_DESC_COUNT; i++) {
        tsw_commit_recv_desc(ptr, recv_buff[i], TSW_RECV_BUFF_LEN, i);
    }

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

void tsw_self_adaptive_port_speed(void)
{
    tsw_phy_status_t status = {0};
    tsw_port_speed_t port_speed[] = {tsw_port_speed_10mbps, tsw_port_speed_100mbps, tsw_port_speed_1000mbps};
    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};

    rtl8211_get_phy_status(BOARD_TSW, BOARD_TSW_PORT, &status);

    if (status.tsw_phy_link || (status.tsw_phy_link != last_status.tsw_phy_link)) {
        if (memcmp(&last_status, &status, sizeof(tsw_phy_status_t)) != 0) {
            memcpy(&last_status, &status, sizeof(tsw_phy_status_t));
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
    uint32_t sec, nsec;
    uint32_t crsr;
    tsw_tas_config_t config;

    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_tsw_pins(BOARD_TSW);

    /* Reset an TSW PHY */
    board_reset_tsw_phy(BOARD_TSW, BOARD_TSW_PORT);

    printf("This is a TSW demo: Time Aware Scheduing\n");

    #if defined(RGMII) && RGMII
    board_init_tsw_rgmii_clock_delay(BOARD_TSW, BOARD_TSW_PORT);
    #endif

     /* Start a board timer */
    board_timer_create(TSW_APP_TIMER_INTERVAL, tsw_self_adaptive_port_speed);

    /* Set RTC increment */
    tsw_set_rtc_time_increment(BOARD_TSW, (10 << 24));

    entry[TSW_SHACL_ENT0].interval = 100000000; /* 100ms */
    entry[TSW_SHACL_ENT0].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT0].state = tsw_shap_tas_aclist_state_open_queueu_0;

    entry[TSW_SHACL_ENT1].interval = 100000000;  /* 100ms */
    entry[TSW_SHACL_ENT1].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT1].state = tsw_shap_tas_aclist_state_open_queueu_1;

    entry[TSW_SHACL_ENT2].interval = 100000000;  /* 100ms */
    entry[TSW_SHACL_ENT2].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT2].state = tsw_shap_tas_aclist_state_open_queueu_2;

    config.entry = entry;
    config.entry_count = 3;
    config.cycle_time = 1000000000; /* 1000ms */

    config.base_time_sec = 3;
    config.base_time_ns = 0;

    /* Initialize MAC and DMA */
    if (tsw_init(BOARD_TSW) == 0) {
        tsw_shap_disable_tas(BOARD_TSW, BOARD_TSW_PORT);
        tsw_shap_set_tas_max_sdu_ticks(BOARD_TSW, BOARD_TSW_PORT, tsw_traffic_queue_0, 60);
        tsw_shap_set_tas_max_sdu_ticks(BOARD_TSW, BOARD_TSW_PORT, tsw_traffic_queue_1, 60);
        tsw_shap_set_tas_max_sdu_ticks(BOARD_TSW, BOARD_TSW_PORT, tsw_traffic_queue_2, 60);
        tsw_shap_set_tas(BOARD_TSW, BOARD_TSW_PORT, &config);
        tsw_shap_enable_tas(BOARD_TSW, BOARD_TSW_PORT);

        /* Set dest port */
        data_buff[0] = BOARD_TSW_PORT + 1;

        /* Prepare streams */
        memcpy(send_buff[0], data_buff, sizeof(data_buff));
        memcpy(send_buff[1], data_buff, sizeof(data_buff));
        memcpy(send_buff[2], data_buff, sizeof(data_buff));

        while (1) {
            tsw_shap_get_tas_crsr(BOARD_TSW, BOARD_TSW_PORT, &crsr);
            cur_op_gs = TSW_TSNPORT_TSN_SHAPER_TAS_CRSR_OPERGS_GET(crsr);
            if (last_op_gs != cur_op_gs) {
                last_op_gs = cur_op_gs;
                tsw_get_rtc_current_time(BOARD_TSW, &sec, &nsec);
                if (sec >= config.base_time_sec) {
                    if (cur_op_gs & BIT0_MASK) {
                        send_buff[0][2] = tsw_traffic_queue_0;
                        tsw_send_frame(BOARD_TSW, send_buff[0], sizeof(data_buff), 0);
                    }

                    if (cur_op_gs & BIT1_MASK) {
                        send_buff[1][2] = tsw_traffic_queue_1;
                        tsw_send_frame(BOARD_TSW, send_buff[1], sizeof(data_buff), 1);
                    }

                    if (cur_op_gs & BIT2_MASK) {
                        send_buff[2][2] = tsw_traffic_queue_2;
                        tsw_send_frame(BOARD_TSW, send_buff[2], sizeof(data_buff), 2);
                    }
                }
            }
        }
    } else {
        printf("TSW initialization fails !!!\n");

        while (1) {

        }
    }

    return 0;
}
