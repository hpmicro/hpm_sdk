/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "usbh_core.h"
#include "usbh_rndis.h"
#include "usbh_cdc_ecm.h"

#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"


struct netif g_cdc_ecm_netif;
struct netif g_rndis_netif;
TimerHandle_t timer_handle;

#if LWIP_DHCP
TimerHandle_t dhcp_handle1;
TimerHandle_t dhcp_handle2;

static void dhcp_timeout(TimerHandle_t xTimer)
{
    struct netif *netif = (struct netif *)pvTimerGetTimerID(xTimer);
    struct dhcp *dhcp;

    if (netif_is_up(netif)) {
        dhcp = netif_dhcp_data(netif);

        if (dhcp && (dhcp->state == DHCP_STATE_BOUND)) {
            printf("\r\n");
            printf(" IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
            printf(" IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
            printf(" IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));

            xTimerStop(xTimer, 0);
        }
    }
}
#endif

/***********************************************
 * cdc_ecm host
 ***********************************************/
static err_t usbh_cdc_ecm_if_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->output = etharp_output;
    netif->linkoutput = usbh_cdc_ecm_linkoutput;
    return ERR_OK;
}

void usbh_cdc_ecm_run(struct usbh_cdc_ecm *cdc_ecm_class)
{
    struct netif *netif = &g_cdc_ecm_netif;

    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, cdc_ecm_class->mac, 6);

    IP4_ADDR(&cdc_ecm_class->ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&cdc_ecm_class->netmask, 0, 0, 0, 0);
    IP4_ADDR(&cdc_ecm_class->gateway, 0, 0, 0, 0);

    netif = netif_add(netif, &cdc_ecm_class->ipaddr, &cdc_ecm_class->netmask, &cdc_ecm_class->gateway, NULL, usbh_cdc_ecm_if_init, tcpip_input);
    netif_set_default(netif);
    while (!netif_is_up(netif)) {
    }

    dhcp_handle1 = xTimerCreate((const char *)"dhcp1", (TickType_t)200, (UBaseType_t)pdTRUE, (void *const)netif, (TimerCallbackFunction_t)dhcp_timeout);
    if (dhcp_handle1 == NULL) {
        USB_LOG_ERR("timer creation failed! \r\n");
        while (1) {
        }
    }

    usb_osal_thread_create("usbh_cdc_ecm_rx", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_cdc_ecm_rx_thread, netif);
#if LWIP_DHCP
    dhcp_start(netif);
    xTimerStart(dhcp_handle1, 0);
#endif
}

void usbh_cdc_ecm_stop(struct usbh_cdc_ecm *cdc_ecm_class)
{
    (void)cdc_ecm_class;

    struct netif *netif = &g_cdc_ecm_netif;
#if LWIP_DHCP
    dhcp_stop(netif);
    dhcp_cleanup(netif);
    xTimerStop(dhcp_handle1, 0);
    xTimerDelete(dhcp_handle1, 0);
#endif
    netif_set_down(netif);
    netif_remove(netif);
}

/***********************************************
 * rndis host
 ***********************************************/
static void rndis_dev_keepalive_timeout(TimerHandle_t xTimer)
{
    struct usbh_rndis *rndis_class = (struct usbh_rndis *)pvTimerGetTimerID(xTimer);
    usbh_rndis_keepalive(rndis_class);
}

void timer_init(struct usbh_rndis *rndis_class)
{
    timer_handle = xTimerCreate((const char *)NULL, (TickType_t)5000, (UBaseType_t)pdTRUE, (void *const)rndis_class, (TimerCallbackFunction_t)rndis_dev_keepalive_timeout);
    if (NULL != timer_handle) {
        xTimerStart(timer_handle, 0);
    } else {
        USB_LOG_ERR("timer creation failed! \r\n");
        for (;;) {
            ;
        }
    }
}

static err_t usbh_rndis_if_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->output = etharp_output;
    netif->linkoutput = usbh_rndis_linkoutput;
    return ERR_OK;
}

void usbh_rndis_run(struct usbh_rndis *rndis_class)
{
    struct netif *netif = &g_rndis_netif;

    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, rndis_class->mac, 6);

    IP4_ADDR(&rndis_class->ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&rndis_class->netmask, 0, 0, 0, 0);
    IP4_ADDR(&rndis_class->gateway, 0, 0, 0, 0);

    netif = netif_add(netif, &rndis_class->ipaddr, &rndis_class->netmask, &rndis_class->gateway, NULL, usbh_rndis_if_init, tcpip_input);
    netif_set_default(netif);
    while (!netif_is_up(netif)) {
    }

    dhcp_handle2 = xTimerCreate((const char *)"dhcp2", (TickType_t)200, (UBaseType_t)pdTRUE, (void *const)netif, (TimerCallbackFunction_t)dhcp_timeout);
    if (dhcp_handle2 == NULL) {
        USB_LOG_ERR("timer creation failed! \r\n");
        while (1) {
        }
    }

    usb_osal_thread_create("usbh_rndis_rx", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_rndis_rx_thread, netif);
    timer_init(rndis_class);

#if LWIP_DHCP
    dhcp_start(netif);
    xTimerStart(dhcp_handle2, 0);
#endif
}

void usbh_rndis_stop(struct usbh_rndis *rndis_class)
{
    (void)rndis_class;

    struct netif *netif = &g_rndis_netif;
#if LWIP_DHCP
    dhcp_stop(netif);
    dhcp_cleanup(netif);
    xTimerStop(dhcp_handle2, 0);
    xTimerDelete(dhcp_handle2, 0);
#endif
    netif_set_down(netif);
    netif_remove(netif);
    xTimerStop(timer_handle, 0);
    xTimerDelete(timer_handle, 0);
}
