/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "eoe_ethernetif.h"

#include "applInterface.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

struct netif eoe_netif;

#define EOE_RX_BUFF_SIZE (ETHERNET_MAX_FRAMEBUF_LEN)
uint8_t eoe_rx_buff[EOE_RX_BUFF_SIZE]; /* EoE Receive Buffer */

#if defined(STATIC_ETHERNET_BUFFER) && STATIC_ETHERNET_BUFFER
#define EOE_TX_BUFF_SIZE (ETHERNET_MAX_FRAMEBUF_LEN)
uint8_t eoe_tx_buff[EOE_TX_BUFF_SIZE]; /* EoE Transmit Buffer */
#endif

static err_t eoe_send(struct netif *netif, struct pbuf *p)
{
    uint8_t *pdata = NULL;
    if (netif == NULL || p == NULL) {
        return ERR_VAL;
    }

    if ((!bEoESendFramePending && (nAlStatus != STATE_INIT)) && (pEoeSendStored == NULL)) {
#if defined(STATIC_ETHERNET_BUFFER) && STATIC_ETHERNET_BUFFER
        if (p->tot_len > EOE_TX_BUFF_SIZE) {
            return ERR_MEM;
        }
        pdata = eoe_tx_buff;
#else
        pdata = (uint8_t *)malloc(p->tot_len);
        if (pdata == NULL) {
            return ERR_MEM;
        }
#endif

        struct pbuf *q;
        uint16_t copied = 0;
        for (q = p; q != NULL; q = q->next) {
            if ((copied + q->len) > p->tot_len) {
                LWIP_DEBUGF(NETIF_DEBUG, ("pbuf chain length mismatch!\n"));
                #if defined(STATIC_ETHERNET_BUFFER) && !STATIC_ETHERNET_BUFFER
                free(pdata);
                #endif
                return ERR_VAL;
            }
            memcpy((pdata + copied), q->payload, q->len);
            copied += q->len;
        }
        EOE_SendFrameRequest((uint16_t *)pdata, copied);
    } else {
        return ERR_MEM;
    }

    return ERR_OK;
}

void eoe_receive(uint16_t *pdata, uint16_t length)
{
    assert(length < EOE_RX_BUFF_SIZE);

    struct pbuf *p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
    if (p  == NULL) {
        printf("pbuf alloc failed\n");
        return;
    }

    memcpy(eoe_rx_buff, pdata, length);

    p->payload = eoe_rx_buff;
    p->len = p->tot_len = length;

    if (eoe_netif.input(p, &eoe_netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("Failed to send frame to ethernet_input\n"));
        pbuf_free(p);
    }
}

void eoe_setip(uint16_t *mac_addr, uint16_t *ip_addr, uint16_t *subnet_mask, uint16_t *default_gateway, uint16_t *dns_ip)
{
    (void) dns_ip;

    uint8_t macaddr[6];
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gateway;

    MEMCPY(&macaddr, mac_addr, 6);
    MEMCPY(&ip.addr, ip_addr, 4);
    MEMCPY(&netmask.addr, subnet_mask, 4);
    MEMCPY(&gateway.addr, default_gateway, 4);

    /* Set netif down */
    if (netif_is_up(&eoe_netif)) {
        netif_set_link_down(&eoe_netif);
        netif_set_down(&eoe_netif);
    }

    /* Set netif up */
    eoe_netif.hwaddr_len = 6;
    memcpy(&eoe_netif.hwaddr, macaddr, 6);
    netif_set_addr(&eoe_netif, &ip, &netmask, &gateway);
    netif_set_default(&eoe_netif);
    netif_set_link_up(&eoe_netif);
    netif_set_up(&eoe_netif);
    printf("EoE IP address setting:\n");
    printf("IPv4 Address: %s\n", ipaddr_ntoa(&eoe_netif.ip_addr));
    printf("IPv4 Netmask: %s\n", ipaddr_ntoa(&eoe_netif.netmask));
    printf("IPv4 Gateway: %s\n", ipaddr_ntoa(&eoe_netif.gw));
    printf("MAC         : %02X:%02X:%02X:%02X:%02X:%02X\n",
       macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
}

static err_t eoe_netif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "eoe_lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    netif->output = etharp_output;
    netif->linkoutput = eoe_send;

    netif->mtu = 1500;

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    netif->hwaddr_len = ETH_HWADDR_LEN;

    etharp_init();

    return ERR_OK;
}

void eoe_netif_config(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);
    ip4_addr_set_zero(&gw);

    netif_add(netif, &ipaddr, &netmask, &gw, NULL, &eoe_netif_init, &ethernet_input);
}

