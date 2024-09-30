/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   sock.h
 * \author zimmerli
 * \date   2020-01-21
 * \brief  Ethernet Socket
 *
 */

#ifndef SOCK_H_
#define SOCK_H_

#include <kernel/base/kernel.h>
#include <kernel/net/netb.h>
#include <kernel/net/netdev.h>

/**
 * \brief UDP protocol settings
 */
struct udp_proto_s {
    uint32_t src_ip_hbo;   //!< source ip address, host byte order
    uint32_t dst_ip_hbo;   //!< destination ip address, host byte order
    uint16_t src_port_hbo; //!< source port, host byte order
    uint16_t dst_port_hbo; //!< destination port, host byte order
};

/**
 * \brief Ethernet socket, rx only
 */
struct sock_ethrx_s {
    uint16_t ethtype_hbo;    //!< Ether type, host byte order
    QueueHandle_t queue_rx;  //!< rx queue
    struct netdev_s *netdev; //!< associated network device, NULL for any
};

/**
 * \brief Ethernet socket, tx only
 */
struct sock_ethtx_s {
    uint16_t ethtype_hbo;    //!< Ether type, host byte order
    uint16_t vlantag_hbo;    //!< VLAN tag, host byte order
    struct netdev_s *netdev; //!< associated network device
    uint8_t tstamp_id;       //!< tx-timestamping channel id, 0 for no timestamping
    uint8_t traffic_prio;    //!< traffic priority code
};

// layer functions
void socket_rx(struct netdev_s *netdev, struct netb_s *netb);

// socket functions
struct sock_ethrx_s *socket_ethrx(uint16_t ethtype_hbo);
void socket_ethrx_bind(struct sock_ethrx_s *sock, struct netdev_s *device);

struct sock_ethtx_s *socket_ethtx(uint16_t ethtype_hbo);
void socket_ethtx_bind(struct sock_ethtx_s *sock, struct netdev_s *device);
int32_t socket_ethtx_send(struct sock_ethtx_s *sock, void *ptr, uint32_t length, uint8_t *dstmac);
int32_t socket_ethtx_send_udp(struct sock_ethtx_s *sock, struct udp_proto_s *udp, void *ptr, uint32_t length);

#endif /* SOCK_H_ */
