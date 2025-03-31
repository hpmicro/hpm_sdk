/*
 * Copyright (c) 2023-2025 HPMicro
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

static enet_phy_status_t last_status[BOARD_ENET_COUNT] = {{.enet_phy_link = enet_phy_link_unknown}, {.enet_phy_link = enet_phy_link_unknown}};

#if defined(NO_SYS) && !NO_SYS
uint32_t msg[BOARD_ENET_COUNT];
extern osSemaphoreId_t s_xSemaphore[];

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
    static uint8_t dhcp_last_state[BOARD_ENET_COUNT] = {DHCP_STATE_OFF};
    struct dhcp *dhcp = netif_dhcp_data(netif);

    if (netif_is_link_up(netif) == false) {
        return;
    }

    if (dhcp == NULL) {
        dhcp_last_state[netif->num] = DHCP_STATE_OFF;
    } else if (dhcp_last_state[netif->num] != dhcp->state) {
        printf("================ Network Interface %d ================\n", netif->num);
        dhcp_last_state[netif->num] = dhcp->state;

        printf("DHCP State: ");
        switch (dhcp_last_state[netif->num]) {
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
            printf("%u", dhcp_last_state[netif->num]);
            assert(0);
            break;
        }
        printf("\r\n");

        if (dhcp_last_state[netif->num] == DHCP_STATE_BOUND) {
            netif_user_notification(netif);
        }
    }
}
#endif

ATTR_WEAK uint8_t enet_get_mac_address(uint8_t i, uint8_t *mac)
{
    uint32_t macl, mach;
    uint8_t idx = 0;
    char *p = NULL;
    char *token;

    if (mac == NULL) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    /* load mac address from OTP MAC area */
    if (i == 0) {
        macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX);
        mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);

        mac[0] = (macl >>  0) & 0xff;
        mac[1] = (macl >>  8) & 0xff;
        mac[2] = (macl >> 16) & 0xff;
        mac[3] = (macl >> 24) & 0xff;
        mac[4] = (mach >>  0) & 0xff;
        mac[5] = (mach >>  8) & 0xff;
    } else {
        macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);
        mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 2);

        mac[0] = (macl >> 16) & 0xff;
        mac[1] = (macl >> 24) & 0xff;
        mac[2] = (mach >>  0) & 0xff;
        mac[3] = (mach >>  8) & 0xff;
        mac[4] = (mach >> 16) & 0xff;
        mac[5] = (mach >> 24) & 0xff;
    }

    if (!IS_MAC_INVALID(mac)) {
        return ENET_MAC_ADDR_FROM_OTP_MAC;
    }

    /* load MAC address from MACRO definitions */
    p = mac_init[i].mac_addr;
    token = strtok(p, ":");
    mac[idx] = strtol(token, NULL, 16);
    while (token != NULL && ++idx < ENET_MAC) {
        token = strtok(NULL, ":");
        mac[idx] = strtol(token, NULL, 16);
    }

    if (idx < ENET_MAC) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    return ENET_MAC_ADDR_FROM_MACRO;
}

ATTR_WEAK void enet_set_mac_address(void *config, uint8_t *mac)
{
    enet_mac_config_t *p = (enet_mac_config_t *)config;

    p->mac_addr_high[0] = mac[5] << 8 | mac[4];
    p->mac_addr_low[0]  = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    p->valid_max_count  = 1;
}

bool enet_get_link_status(uint8_t i)
{
    return last_status[i].enet_phy_link;
}

void enet_self_adaptive_port_speed(void)
{
    enet_base_t *base;
    enet_phy_status_t status = {0};
    enet_line_speed_t line_speed[] = {enet_line_speed_10mbps, enet_line_speed_100mbps, enet_line_speed_1000mbps};

    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};

    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        base = board_get_enet_base(netif_get_by_index(i+1)->num);
        board_get_enet_phy_status(i, &status);
        if (status.enet_phy_link || (status.enet_phy_link != last_status[i].enet_phy_link)) {
            if (memcmp(&last_status[i], &status, sizeof(enet_phy_status_t)) != 0) {
                memcpy(&last_status[i], &status, sizeof(enet_phy_status_t));
                printf("================ Network Interface %d ================\n", netif_get_by_index(i+1)->num);
                if (status.enet_phy_link) {
                    printf("Link Status: Up\n");
                    printf("Link Speed:  %s\n", speed_str[status.enet_phy_speed]);
                    printf("Link Duplex: %s\n", duplex_str[status.enet_phy_duplex]);

                    enet_set_line_speed(base, line_speed[status.enet_phy_speed]);
                    enet_set_duplex_mode(base, status.enet_phy_duplex);

                    #if defined(NO_SYS) && !NO_SYS
                    msg[i] = enet_phy_link_up;
                    sys_mbox_trypost_fromisr(&netif_status_mbox[i], &msg[i]);
                    #else
                    netif_set_link_up(netif_get_by_index(i+1));
                    #endif
                } else {
                    printf("Link Status: Down\n");
                    #if defined(NO_SYS) && !NO_SYS
                    msg[i] = enet_phy_link_down;
                    sys_mbox_trypost_fromisr(&netif_status_mbox[i], &msg[i]);
                    #else
                    netif_set_link_down(netif_get_by_index(i+1));
                    #endif
                }
            }
        }
    }
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

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
static void isr_enet(uint8_t idx)
{
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
#endif
#endif

    uint32_t status;
    uint32_t rxgbfrmis;
    uint32_t intr_status;
    enet_base_t *ptr = board_get_enet_base(idx);

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
        /* Give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(s_xSemaphore[idx], &xHigherPriorityTaskWoken);
        /* Switch tasks if necessary. */
        if (xHigherPriorityTaskWoken != pdFALSE) {
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        rt_sem_release(s_xSemaphore[idx]);
#endif
    #else
        rx_flag[idx] = true;
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
    isr_enet(0);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
void isr_enet1(void)
{
    isr_enet(1);
}
#endif

#endif
