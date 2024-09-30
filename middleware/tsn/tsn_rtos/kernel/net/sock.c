/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   sock.c
 * \author zimmerli
 * \date   2020-01-21
 * \brief  Ethernet Socket
 *
 */

#include "sock.h"
#include <kernel/base/klibc.h>
#include <kernel/net/ether.h>
#include <kernel/net/netdev.h>
#include <kernel/lwip/lwip_port.h>

#include <tsn_rtos_config.h>

#include <string.h>

#if USE_TSN_RTOS_LWIP
#include <lwip/netif.h>
#endif

// ----------------------------------------------------------------
#define SOCKSRX_MAX 16
#define SOCKSTX_MAX 16
#define DEFAULT_QSZ 16

//#define DEBUG_PRINT_RX

static struct sock_ethrx_s *_socks_rx[SOCKSRX_MAX] = { 0 };
static struct sock_ethtx_s *_socks_tx[SOCKSTX_MAX] = { 0 };

//ERSTMAL
#ifdef DEBUG_PRINT_RX
extern void __internal_unset_indent();
extern void __internal_set_indent();
#endif

static const char *SOCKET_CONTEXT = "SOCKET";

void display_frame(char *ctx, uint8_t *data, uint8_t len)
{
    logs(LOG_ALWAYS, SOCKET_CONTEXT, " frame (%s, %u): ", ctx, len);

    for (uint8_t i = 0; i < len; i++)
        logc(LOG_ALWAYS, "%02X ", (data[i]));

    loge(LOG_ALWAYS, "\n");
}

// ----------------------------------------------------------------
/**
 * \brief Received data, called by network device driver
 *
 * @param netdev    pointer to netdev handling the received frame
 * @param netb      pointer to received network buffer
 */
uint32_t cnt = 0;
void socket_rx(struct netdev_s *netdev, struct netb_s *netb)
{
    uint32_t idx, tmp;
    uint32_t protosz;
    uint16_t ethtype_hbo;

    cnt++;

    /*
     * get ethertype
     * pull ethernet protocol
     * support for vlan
     */
    protosz = 14;
    ethtype_hbo = MEM16(netb->data + 12);
    ethtype_hbo = klibc_ntohs(ethtype_hbo);
    if (ethtype_hbo == ETH_TYPE_VLAN) {
        protosz += 4;
        ethtype_hbo = MEM16(netb->data + 16);
        ethtype_hbo = klibc_ntohs(ethtype_hbo);
    }

    /* find rx socket */
    for (idx = 0; idx < SOCKSRX_MAX; ++idx) {
        if (_socks_rx[idx] && (_socks_rx[idx]->ethtype_hbo == ethtype_hbo)) {
            if (!_socks_rx[idx]->netdev || (_socks_rx[idx]->netdev == netb->netdev) || _socks_rx[idx]->netdev == netdev) {
                netb_pull(netb, protosz);

                /* add to socket queue */
                //            	logk(LOG_DEBUG, SOCKET_CONTEXT, "put netbuf in sock rx queue %d (ethtype: 0x%04X)\n", idx, ethtype_hbo);
                tmp = xQueueSend(_socks_rx[idx]->queue_rx, &netb, 0);
                if (tmp != pdPASS) {
                    _socks_rx[idx]->netdev->stats.rx_dropped++;
                    kfree_netb(netb);
                }

                _socks_rx[idx]->netdev->stats.rx_packets++;
                return;
            }
        }
    }

    /*
     * to lwIP
     */
#if (USE_TSN_RTOS_NET_LWIP)
    lwip_recv_skb(netdev, netb);
#else
    /* free buffer */
    kfree_netb(netb);
#endif
}

/**
 * \brief Create MAC-layer RX socket for specified ether type
 *
 * @param ethtype_hbo   ether type, host-byte-order
 * @return              pointer to socket or NULL on error
 */
struct sock_ethrx_s *socket_ethrx(uint16_t ethtype_hbo)
{
    uint32_t idx;

    /* search empty rx sock ptr */
    for (idx = 0; idx < SOCKSRX_MAX; ++idx) {
        if (!_socks_rx[idx])
            break;
    }
    if (idx >= SOCKSRX_MAX)
        return NULL;

    /* alloc / init */
    _socks_rx[idx] = kallocz(sizeof(struct sock_ethrx_s));
    if (_socks_rx[idx]) {
        _socks_rx[idx]->ethtype_hbo = ethtype_hbo;
        _socks_rx[idx]->queue_rx = xQueueCreate(DEFAULT_QSZ, sizeof(struct netb_s *));
    }

    return _socks_rx[idx];
}

/**
 * \brief Bind socket to device
 *
 * Will only receive from device or from any device (device==NULL)
 *
 * @param sock      pointer to socket
 * @param device    pointer to netdev device, set NULL for any device
 */
void socket_ethrx_bind(struct sock_ethrx_s *sock, struct netdev_s *device)
{
    if (!sock)
        return;
    sock->netdev = device;
}

// ----------------------------------------------------------------

/**
 * \brief Create MAC-layer TX socket for specified ether type
 *
 * @param ethtype_hbo   ether type, host-byte-order
 * @return              pointer to socket, NULL on error
 */
struct sock_ethtx_s *socket_ethtx(uint16_t ethtype_hbo)
{
    uint32_t idx;

    /* search empty tx sock ptr */
    for (idx = 0; idx < SOCKSTX_MAX; ++idx) {
        if (!_socks_tx[idx])
            break;
    }
    if (idx >= SOCKSTX_MAX)
        return NULL;

    /* alloc / init */
    _socks_tx[idx] = kallocz(sizeof(struct sock_ethtx_s));
    if (_socks_tx[idx]) {
        _socks_tx[idx]->ethtype_hbo = ethtype_hbo;
    }

    return _socks_tx[idx];
}

/**
 * \brief Bind socket to device
 *
 * @param sock      pointer to socket
 * @param device    pointer to netdev device
 */
void socket_ethtx_bind(struct sock_ethtx_s *sock, struct netdev_s *device)
{
    if (!sock)
        return;
    sock->netdev = device;
}

static struct netb_s *_socktx_alloc_netb(uint32_t resv_sz, uint32_t timeout)
{
    struct netb_s *netb;

    netb = kalloc_netb(timeout);
    if (netb) {
        netb_reserve(netb, resv_sz);
    }

    return netb;
}

static int32_t _socktx_xmit(struct sock_ethtx_s *sock, struct netb_s *netb)
{
    netb->priority = sock->traffic_prio;
    netb->tstamp_id = sock->tstamp_id;
    return netdev_xmit(sock->netdev, netb);
}

static void _socktx_mac_hdr(struct netb_s *netb, uint8_t *src, uint8_t *dst, uint16_t ethtype, uint16_t vlantag)
{
    uint32_t proto_sz;

    /* calc mac header size && push to front */
    proto_sz = 14;
    if (vlantag)
        proto_sz += 4;
    netb_push(netb, proto_sz);

    /* set header, conversion to network byte order */
    memcpy((void *)(netb->data + 0), dst, MAC_ADDR_LEN);
    memcpy((void *)(netb->data + 6), src, MAC_ADDR_LEN);
    if (vlantag) {
        MEM16(netb->data + 12) = klibc_htons(ETH_TYPE_VLAN);
        MEM16(netb->data + 14) = klibc_htons(vlantag);
    }
    MEM16(netb->data + proto_sz - 2) = klibc_htons(ethtype);
}

static void _socktx_udp_hdr(struct netb_s *netb, struct udp_proto_s *udp)
{
    register uint16_t tmp;
    uint16_t payload_sz;
    uint32_t csum;
    uint32_t src_ip_nbo, dst_ip_nbo;
    uint32_t idx;

    // ip conversion (needed for pseudo header)
    src_ip_nbo = klibc_htonl(udp->src_ip_hbo);
    dst_ip_nbo = klibc_htonl(udp->dst_ip_hbo);
    payload_sz = netb_length(netb);

    /* UDP */
    netb_push(netb, 8);
    MEM16(netb->data + 0) = klibc_htons(udp->src_port_hbo); // src
    MEM16(netb->data + 2) = klibc_htons(udp->dst_port_hbo); // dst
    tmp = netb_length(netb);
    MEM16(netb->data + 4) = klibc_htons(tmp); // len (incl udp header)
    MEM16(netb->data + 6) = 0;                // csum
    /* UDP checksum */
    csum = 0;
    // pseudo header
    csum += src_ip_nbo & 0xFFFF;
    csum += src_ip_nbo >> 16;
    csum += dst_ip_nbo & 0xFFFF;
    csum += dst_ip_nbo >> 16;
    csum += klibc_htons(0x0011); // protocol
    csum += MEM16(netb->data + 4);
    csum += MEM16(netb->data + 0);
    csum += MEM16(netb->data + 2);
    csum += MEM16(netb->data + 4); // length included twice!
    // payload csum
    // payload odd byte=0
    if (payload_sz & 0x01)
        MEM8(netb->tail) = 0x00;
    for (idx = 0; idx < payload_sz; idx += 2) {
        csum += MEM16(netb->data + 8 + idx);
    }
    // set
    while (csum > 0xFFFF) {
        csum = (csum >> 16) + (csum & 0xFFFF);
    }
    csum = ~csum; // invert
    if (!csum)
        csum = ~0; // never 0
    MEM16(netb->data + 6) = csum;

    /* IPv4 */
    netb_push(netb, 20);
    MEM16(netb->data + 0) = klibc_htons(0x4500); // diffserv
    tmp = netb_length(netb);
    MEM16(netb->data + 2) = klibc_htons(tmp);          // total length
    MEM16(netb->data + 4) = klibc_htons(0);            // id
    MEM16(netb->data + 6) = klibc_htons(0);            // fragm, flags
    MEM16(netb->data + 8) = klibc_htons(0x8611);       // ttl, proto udp=0x11
    MEM16(netb->data + 10) = klibc_htons(0);           // hdr checksum, 0 for calculation
    __put_unaligned_le32(src_ip_nbo, netb->data + 12); // src ip addr
    __put_unaligned_le32(dst_ip_nbo, netb->data + 16); // dst ip addr
    // IP header checksum
    csum = 0;
    for (tmp = 0; tmp < 20; tmp += 2) {
        csum += MEM16(netb->data + tmp);
    }
    while (csum > 0xFFFF) {
        csum = (csum >> 16) + (csum & 0xFFFF);
    }
    // update checksum
    MEM16(netb->data + 10) = ~csum; // set hdr checksum
}

/**
 * \brief Send data over socket
 *
 * @param sock      pointer to socket
 * @param ptr       pointer to payload
 * @param length    length of payload in bytes
 * @param dstmac    destination mac address
 * @return          error code, NETDEV_RESULT_OK on success
 */
int32_t socket_ethtx_send(struct sock_ethtx_s *sock, void *ptr, uint32_t length, uint8_t *dstmac)
{
    struct netb_s *netb;
    uint32_t space;
    int32_t retval;

    if (!sock || !sock->netdev)
        return NETDEV_ERR_GENERAL;

    /* calc needed head space && allocate buffer */
    space = sock->netdev->needed_head + 14;
    if (sock->vlantag_hbo)
        space += 4;

    netb = _socktx_alloc_netb(space, 10);
    if (!netb)
        return NETDEV_ERR_GENERAL;

    /* put payload data to buffer*/
    memcpy((void *)netb->tail, ptr, length);
    netb_put(netb, length);

    /* set eth protocol header */
    _socktx_mac_hdr(netb, sock->netdev->dev_addr, dstmac, sock->ethtype_hbo, sock->vlantag_hbo);

    /* send to device */
    retval = _socktx_xmit(sock, netb);

    /* always free allocated buffer */
    kfree_netb(netb);

    return retval;
}

/**
 * \brief Send UDP data over socket
 *
 * @param sock      pointer to socket
 * @param udp       pointer to UDP settings
 * @param ptr       pointer to payload
 * @param length    length of payload in bytes
 * @return          error code, NETDEV_RESULT_OK on success
 */
int32_t socket_ethtx_send_udp(struct sock_ethtx_s *sock, struct udp_proto_s *udp, void *ptr, uint32_t length)
{
    uint8_t dstmac[6] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x01 };
    struct netb_s *netb;
    uint32_t space;
    int32_t retval;

    if (!sock || !sock->netdev)
        return NETDEV_ERR_GENERAL;

    /* calc needed head space && allocate buffer */
    space = sock->netdev->needed_head + 14 + 28;
    if (sock->vlantag_hbo)
        space += 4;
    netb = _socktx_alloc_netb(space, 10);
    if (!netb)
        return NETDEV_ERR_GENERAL;

    /* put payload data to buffer*/
    memcpy((void *)netb->tail, ptr, length);
    netb_put(netb, length);

    /* UDP protocol */
    _socktx_udp_hdr(netb, udp);
    if ((udp->dst_ip_hbo & 0xF0000000) == 0xE0000000) {
        // multicast IP -> set dst mac
        dstmac[3] = (udp->dst_ip_hbo >> 16) & 0x7F;
        dstmac[4] = (udp->dst_ip_hbo >> 8) & 0xFF;
        dstmac[5] = udp->dst_ip_hbo & 0xFF;
    }

    /* set eth protocol header */
    _socktx_mac_hdr(netb, sock->netdev->dev_addr, dstmac, ETH_TYPE_IP, sock->vlantag_hbo);

    /* send to device */
    retval = _socktx_xmit(sock, netb);
    /* always free allocated buffer */
    kfree_netb(netb);

    return retval;
}
