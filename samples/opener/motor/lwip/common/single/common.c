/*
 * Copyright (c) 2022-2026 HPMicro
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
#include "enet_phy_adaptive_lwip.h"

static enet_phy_status_t last_status;

#if defined(NO_SYS) && !NO_SYS
uint32_t msg;
extern osSemaphoreId_t s_xSemaphore;

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
void timer_callback(TimerHandle_t xTimer)
{
   (void)xTimer;

   enet_self_adaptive_port_speed();
}
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
void timer_callback(void *parameter)
{
   (void)parameter;

   enet_self_adaptive_port_speed();
}
#endif
#endif

#if defined(LWIP_DHCP) && LWIP_DHCP
void enet_update_dhcp_state(struct netif *netif)
{
    static uint8_t dhcp_last_state = DHCP_STATE_OFF;
    struct dhcp *dhcp = netif_dhcp_data(netif);

    if (netif_is_link_up(netif) == false) {
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

ATTR_WEAK uint8_t enet_get_mac_address(uint8_t *mac)
{
    uint32_t macl, mach;
    uint32_t uuid[OTP_SOC_UUID_LEN / sizeof(uint32_t)];

    if (mac == NULL) {
        return ENET_MAC_ADDR_PARA_ERROR;
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
        return ENET_MAC_ADDR_FROM_OTP_MAC;
    }

    /* load MAC address from OTP UUID area */
    for (uint32_t i = 0; i < ARRAY_SIZE(uuid); i++) {
        uuid[i] = otp_read_from_shadow(OTP_SOC_UUID_IDX + i);
    }

    if (!IS_UUID_INVALID(uuid)) {
        uuid[0] &= 0xfc;
        memcpy(mac, &uuid, ENET_MAC_SIZE);
        return ENET_MAC_ADDR_FROM_OTP_UUID;
    }

    /* load MAC address from MACRO definitions */
    mac[0] = MAC_ADDR0;
    mac[1] = MAC_ADDR1;
    mac[2] = MAC_ADDR2;
    mac[3] = MAC_ADDR3;
    mac[4] = MAC_ADDR4;
    mac[5] = MAC_ADDR5;

    return ENET_MAC_ADDR_FROM_MACRO;
}

bool enet_get_link_status(void)
{
    return last_status.enet_phy_link;
}

void enet_self_adaptive_port_speed(void)
{
    lwip_enet_phy_adaptive_binding_t binding = {
        .last = &last_status,
        .enet_base = ENET,
        .phy_port = 0,
        .log_prefix = NULL,
        .print_port_banner = false,
        .notify_netif = true,
        .netif_idx = LWIP_NETIF_IDX,
#if defined(NO_SYS) && !NO_SYS
        .status_mbox = (void *)&netif_status_mbox,
        .link_msg = &msg,
#endif
    };

    lwip_enet_phy_adaptive_poll(&binding);
}

void enet_services(struct netif *netif)
{
    (void)netif;

    #if defined(LWIP_DHCP) && LWIP_DHCP
    dhcp_start(netif);
    #endif
}

#if defined(NO_SYS) && NO_SYS
void enet_common_handler(struct netif *netif)
{
    ethernetif_input(netif);

    /* Handle all system timeouts for all core protocols */
    #if defined(LWIP_TIMERS) && LWIP_TIMERS
    sys_check_timeouts();
    #endif

    /* update DHCP progress */
    #if defined(LWIP_DHCP) && LWIP_DHCP
    enet_update_dhcp_state(netif);
    #endif
}
#endif
volatile int sem_give_cnt = 0;
#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
void isr_enet(ENET_Type *ptr)
{
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
#endif
#endif

    uint32_t status;
    uint32_t rxgbfrmis;
    uint32_t intr_status;

    status = ptr->DMA_STATUS;
    rxgbfrmis = ptr->MMC_INTR_RX;
    intr_status = ptr->INTR_STATUS;

    if (ENET_DMA_STATUS_GLPII_GET(status)) {
        /* read LPI_CSR to clear interrupt status */
        ptr->LPI_CSR;
    }

    if (ENET_INTR_STATUS_RGSMIIIS_GET(intr_status)) {
        /* read XMII_CSR to clear interrupt status */
        ptr->XMII_CSR;
    }

    if (ENET_DMA_STATUS_RI_GET(status)) {
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
        sem_give_cnt++;
        /* Give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(s_xSemaphore, &xHigherPriorityTaskWoken);
        /* Switch tasks if necessary. */
        if (xHigherPriorityTaskWoken != pdFALSE) {
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        rt_sem_release(s_xSemaphore);
#endif
#else
        rx_flag = true;
#endif
        ptr->DMA_STATUS |= ENET_DMA_STATUS_RI_MASK;
    }

    if (ENET_MMC_INTR_RX_RXCTRLFIS_GET(rxgbfrmis)) {
        ptr->RXFRAMECOUNT_GB;
    }
}

#ifdef HPM_ENET0_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET0, isr_enet0)
void isr_enet0(void)
{
    isr_enet(ENET);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
void isr_enet1(void)
{
    isr_enet(ENET);
}
#endif

#endif
