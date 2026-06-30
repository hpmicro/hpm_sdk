/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "frame_preemption_egress.h"

static volatile tsw_phy_status_t last_status = {.tsw_phy_link = tsw_phy_link_unknown};
uint8_t mac[] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t send_buff[TSW_SEND_DESC_COUNT][TSW_SEND_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t recv_buff[TSW_RECV_DESC_COUNT][TSW_RECV_BUFF_LEN];

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t tsw_init(TSW_Type *ptr)
{
    tsw_dma_config_t config;

    /* Disable all MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_pmac);

    /* Set Mac Address */
    tsw_ep_set_mac_addr(ptr, BOARD_TSW_PORT, mac, true);

    /* Set MAC Mode: GMII, CLKSEL: refclk */
    tsw_ep_set_mac_mode(ptr, BOARD_TSW_PORT, BOARD_TSW_PORT_ITF ==  tsw_port_phy_itf_rgmii ? tsw_mac_mode_gmii : tsw_mac_mode_mii);

    /* Set port PHY interface */
    tsw_set_port_interface(ptr, BOARD_TSW_PORT, BOARD_TSW_PORT_ITF);

    /* Enable all MACs(TX/RX) */
    tsw_ep_enable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    tsw_ep_enable_all_mac_ctrl(ptr, tsw_mac_type_pmac);

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
    hpm_stat_t stat;
    tsw_frame_t frame;
    uint32_t rx_sec;
    uint32_t rx_nsec;
    uint8_t data[1536];
    uint32_t value = 0;

    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_tsw_pins(BOARD_TSW);

    /* Reset an TSW PHY */
    board_reset_tsw_phy(BOARD_TSW, BOARD_TSW_PORT);

    printf("This is a TSW demo: Frame Preemption Egress\n");

    #if defined(HPM_TSW_RGMII) && HPM_TSW_RGMII
    board_init_tsw_rgmii_clock_delay(BOARD_TSW, BOARD_TSW_PORT);
    #endif

    board_timer_create(TSW_APP_TIMER_INTERVAL, tsw_self_adaptive_port_speed);

    /* Set RTC increment */
    tsw_set_rtc_time_increment(BOARD_TSW, (10 << 24));

    /* Initialize MAC and DMA */
    if (tsw_init(BOARD_TSW) == 0) {

        while (1) {
            if (tsw_get_link_status()) {

                stat = tsw_recv_frame(BOARD_TSW, &frame);

                if (stat == status_success) {
                    if ((frame.length > TSW_SOC_SWITCH_HEADER_LEN)) {
                        frame.buffer = recv_buff[frame.id];
                        memcpy(data, &frame.buffer[TSW_SOC_ETH_PAYLOAD_OFFSET], TSW_FRAME_ETH_LEN(frame.length));
                        tsw_commit_recv_desc(BOARD_TSW, recv_buff[frame.id], TSW_RECV_BUFF_LEN, frame.id);

                        tsw_get_rx_hdr_timestamp(frame.buffer, &rx_sec, &rx_nsec);
                        printf("FPE: %d, RX-Time: %d.%09d\n", tsw_get_rx_hdr_fpe(frame.buffer), rx_sec, rx_nsec);
                        printf("Rx Frame Length: %d\n", TSW_FRAME_ETH_LEN(frame.length));

                        for (uint16_t i = 0; i < TSW_FRAME_ETH_LEN(frame.length); i++) {
                            printf("%02x ", data[i]);
                        }
                        printf("\n");

                        tsw_fpe_get_mms_statistics_counter(BOARD_TSW, BOARD_TSW_PORT, tsw_fpe_mms_frame_assembly_ok_counter, &value);
                        printf("FPE Assembly Ok Counter: %d\n", value);
                        printf("========================================================================================================");
                        printf("======================================================================================================\n");
                    }
                } else if (stat == status_fail) {
                    tsw_commit_recv_desc(BOARD_TSW, recv_buff[frame.id], TSW_RECV_BUFF_LEN, frame.id);
                } else {

                }
            }

        }
    } else {
        printf("TSW initialization failed !!!\n");

        while (1) {

        }
    }

    return 0;
}
