/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

#include "usbh_core.h"
#include "usbh_cdc_ecm.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"

struct netif g_cdc_ecm_netif;

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
    usbh_cdc_ecm_lwip_thread_init(&g_cdc_ecm_netif);
}

void usbh_cdc_ecm_stop(struct usbh_cdc_ecm *cdc_ecm_class)
{
    struct netif *netif = &g_cdc_ecm_netif;
    netif_set_down(netif);
    netif_remove(netif);
}
