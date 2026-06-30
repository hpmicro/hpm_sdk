/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "common.h"
#include "hpm_common.h"
#include "hpm_otp_drv.h"
#include "ethernetif.h"
#include "netconf.h"
#include "netinfo.h"
#include "lwip.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "osal.h"
#include "tsw_phy_adaptive_lwip.h"

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
    char *strtok_result = NULL;
    char tmp[32] = "";
    uint32_t macl, mach;
    uint32_t uuid[OTP_SOC_UUID_LEN / sizeof(uint32_t)];
    uint8_t idx = 0;
    char *token;

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
    strcpy(tmp, HPM_STRINGIFY(MAC0_CONFIG));
    token = strtok_r(tmp, ":", &strtok_result);
    mac[idx] = strtol(token, NULL, 16);
    while (token != NULL && ++idx < TSW_ENET_MAC) {
        token = strtok_r(NULL, ":", &strtok_result);
        mac[idx] = strtol(token, NULL, 16);
    }

    if (idx < TSW_ENET_MAC) {
        return TSW_MAC_ADDR_PARA_ERROR;
    }

    return TSW_MAC_ADDR_FROM_MACRO;
}

bool tsw_get_link_status(void)
{
    return last_status.tsw_phy_link;
}

void tsw_self_adaptive_port_speed(void)
{
    lwip_tsw_phy_adaptive_binding_t binding = {
        .last = &last_status,
        .tsw_base = BOARD_TSW,
        .tsw_port = BOARD_TSW_PORT,
        .frame_dst_port = tsw_dst_port_cpu,
        .notify_netif = true,
        .netif_idx = LWIP_NETIF_IDX,
#if defined(NO_SYS) && !NO_SYS
        .status_mbox = (void *)&netif_status_mbox,
        .link_msg = &msg,
#endif
    };

    lwip_tsw_phy_adaptive_poll(&binding);
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
    rx_flag = true;
    intc_m_disable_irq(IRQn_TSW_0);
}
#endif

