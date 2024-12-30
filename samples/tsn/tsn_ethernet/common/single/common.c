/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "common.h"
#include "hpm_common.h"
#include "hpm_otp_drv.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "osal.h"

static tsw_phy_status_t last_status = {.tsw_phy_link = tsw_phy_link_unknown};

#if defined(NO_SYS) && !NO_SYS
uint32_t msg;
extern osSemaphoreId_t s_xSemaphore;

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
void timer_callback(TimerHandle_t xTimer)
{
   (void)xTimer;

   tsw_self_adaptive_port_speed();
}
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
void timer_callback(void *parameter)
{
   (void)parameter;

   tsw_self_adaptive_port_speed();
}
#endif
#endif

#if defined(LWIP_DHCP) && LWIP_DHCP
void tsw_update_dhcp_state(struct netif *netif)
{
    static uint8_t dhcp_last_state = DHCP_STATE_OFF;
    struct dhcp *dhcp = netif_dhcp_data(netif);

    if (tsw_get_link_status() == false) {
        return;
    }

    if (dhcp == NULL) {
        dhcp_last_state = DHCP_STATE_OFF;
    } else if (dhcp_last_state != dhcp->state) {
        dhcp_last_state = dhcp->state;

        printf("DHCP State: ");
        switch (dhcp_last_state) {
        case DHCP_STATE_OFF:
            printf("OFF");
            break;
        case DHCP_STATE_REQUESTING:
            printf("REQUESTING");
            break;
        case DHCP_STATE_INIT:
            printf("INIT");
            break;
        case DHCP_STATE_REBOOTING:
            printf("REBOOTING");
            break;
        case DHCP_STATE_REBINDING:
            printf("REBINDING");
            break;
        case DHCP_STATE_RENEWING:
            printf("RENEWING");
            break;
        case DHCP_STATE_SELECTING:
            printf("SELECTING");
            break;
        case DHCP_STATE_INFORMING:
            printf("INFORMING");
            break;
        case DHCP_STATE_CHECKING:
            printf("CHECKING");
            break;
        case DHCP_STATE_BOUND:
            printf("BOUND");
            break;
        case DHCP_STATE_BACKING_OFF:
            printf("BACKING_OFF");
            break;
        default:
            printf("%u", dhcp_last_state);
            assert(0);
            break;
        }
        printf("\r\n");

        if (dhcp_last_state == DHCP_STATE_BOUND) {
            netif_user_notification(netif);
        }
    }
}
#endif

ATTR_WEAK uint8_t tsw_get_mac_address(uint8_t *mac)
{
    uint32_t macl, mach;
    uint32_t uuid[OTP_SOC_UUID_LEN / sizeof(uint32_t)];

    if (mac == NULL) {
        return TSW_MAC_ADDR_PARA_ERROR;
    }

    /* load mac address from OTP MAC area */
    macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX);
    mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);

    mac[0] = (macl >>  0) & 0xff;
    mac[1] = (macl >>  8) & 0xff;
    mac[2] = (macl >> 16) & 0xff;
    mac[3] = (macl >> 24) & 0xff;
    mac[4] = (mach >>  0) & 0xff;
    mac[5] = (mach >>  8) & 0xff;

    if (!IS_MAC_INVALID(mac)) {
        return TSW_MAC_ADDR_FROM_OTP_MAC;
    }

    /* load MAC address from OTP UUID area */
    for (uint32_t i = 0; i < ARRAY_SIZE(uuid); i++) {
        uuid[i] = otp_read_from_shadow(OTP_SOC_UUID_IDX + i);
    }

    if (!IS_UUID_INVALID(uuid)) {
        uuid[0] &= 0xfc;
        memcpy(mac, &uuid, TSW_MAC_LEN);
        return TSW_MAC_ADDR_FROM_OTP_UUID;
    }

    /* load MAC address from MACRO definitions */
    mac[0] = MAC_ADDR0;
    mac[1] = MAC_ADDR1;
    mac[2] = MAC_ADDR2;
    mac[3] = MAC_ADDR3;
    mac[4] = MAC_ADDR4;
    mac[5] = MAC_ADDR5;

    return TSW_MAC_ADDR_FROM_MACRO;
}

bool tsw_get_link_status(void)
{
    return last_status.tsw_phy_link;
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

                #if defined(NO_SYS) && !NO_SYS
                msg = tsw_phy_link_up;
                sys_mbox_trypost_fromisr(&netif_status_mbox, &msg);
                #else
                netif_set_link_up(netif_get_by_index(LWIP_NETIF_IDX));
                #endif
            } else {
                printf("Link Status: Down\n");
                #if defined(NO_SYS) && !NO_SYS
                msg = tsw_phy_link_down;
                sys_mbox_trypost_fromisr(&netif_status_mbox, &msg);
                #else
                netif_set_link_down(netif_get_by_index(LWIP_NETIF_IDX));
                #endif
            }
        }
    }
}

void tsw_services(struct netif *netif)
{
    (void)netif;

    #if defined(LWIP_DHCP) && LWIP_DHCP
    dhcp_start(netif);
    #endif
}

void tsw_common_handler(struct netif *netif)
{
    ethernetif_input(netif);

    /* Handle all system timeouts for all core protocols */
    #if defined(LWIP_TIMERS) && LWIP_TIMERS
    sys_check_timeouts();
    #endif

    /* update DHCP progress */
    #if defined(LWIP_DHCP) && LWIP_DHCP
    tsw_update_dhcp_state(netif);
    #endif
}

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
SDK_DECLARE_EXT_ISR_M(IRQn_TSW_0, isr_tsw_port_cpu)  /* Declare TSW CPU Port IRQ */
void isr_tsw_port_cpu(void)
{
    static int idx = 0;

    tsw_recv_frame(BOARD_TSW, &frame[idx]);
    tsw_commit_recv_desc(BOARD_TSW, recv_buff[idx], TSW_RECV_BUFF_LEN, idx);
    idx++;
    idx %= TSW_FRAME_BUFF_COUNT;
    rx_flag = true;
}
#endif
