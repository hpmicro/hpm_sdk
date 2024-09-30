/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file
 * \author zimmerli
 * \date   2020-01-16
 * \brief  Generic Network Device
 *
 */

#ifndef NETDEV_H_
#define NETDEV_H_

#include <stdint.h>

#include <kernel/net/netb.h>

//TODO change; include in netdev_xxx_data.h
#include <kernel/net/netdev_tc.h>
#include <kernel/net/netdev_fpe.h>
#include <kernel/net/netdev_bridge.h>
#include <kernel/net/netdev_fdfifo.h>
#include <kernel/net/netdev_monitor.h>
#include <kernel/net/netdev_ethtool.h>

#include <kernel/net/phydev.h>

//neu
#include <kernel/net/netdev_cb_data.h>
#include <kernel/net/netdev_psfp_data.h>

//ERSTMAL TEST
#include <kernel/net/netdev_template_data.h>

#include <tsn_rtos_config.h>

#define MAC_ADDR_LEN 6

#define NETDEV_RESULT_OK        0
#define NETDEV_ERR_GENERAL      -1
#define NETDEV_ERR_TIMEOUT      -2
#define NETDEV_ERR_QFULL        -3
#define NETDEV_ERR_UNSUPPORTED  -4
#define NETDEV_ERR_INVALID_DATA -5

#define IFNAMSIZ 16

/**
 * \brief Network device settings access
 */
enum netdev_access {
    NETDEV_ACCESS_SET, //!< set settings
    NETDEV_ACCESS_GET, //!< get settings
    NETDEV_ACCESS_ADD, //!< add settings
    NETDEV_ACCESS_DEL, //!< delete settings
    NETDEV_ACCESS_LIST //!< get list of settings
};

struct netdev_s;

/**
 * \brief Tsync functions
 */
struct tsync_dev_s {
    void (*cb_rx_frame)(struct netdev_s *netdev, void *rxdata, uint32_t rxlen, uint32_t localtm_ns, uint32_t localtm_sec);
    void (*cb_tx_done)(struct netdev_s *netdev, void *arg, uint32_t localtm_ns, uint32_t localtm_sec);
    void (*cb_timer_done)(struct netdev_s *netdev, uint32_t timer_id);
};

/**
 * \brief Network device statistics
 */
struct netdev_stats_s {
    uint32_t rx_packets;
    uint32_t rx_dropped;
    uint32_t rx_error;
    uint32_t tx_packets;
    uint32_t tx_dropped;
    uint32_t tx_error;
};

/**
 * \brief Network device driver
 */
struct netdev_s {
    char name[IFNAMSIZ];            //!< device name
    uint32_t tsn_port;              //!< tsn port number -1-cpu, 0-eth1, ...
    uint32_t id;                    //!< device driver id					//REMARK do not use it as port id, tsn port id etc. !!!
    void *prv;                      //!< pointer to private data
    void *parent;                   //!< pointer to parent member, e.g. lwip neif structure
    uint8_t dev_addr[MAC_ADDR_LEN]; //!< MAC address to be used
    uint8_t promisc;                //!< promiscous mode flag
    uint8_t hw_queues;              //!< number of implemented hardware queues
    uint8_t prio2tq[8];             //!< traffic class (priority) to queue mapping [0..7 --> 0..7]
    int speed;                      //!< current link speed in mbps
    uint32_t needed_head;           //!< needed head space by driver in network buffer befor data

    struct phydev_s *phy;        //!< pointer to child phy device
    struct netdev_stats_s stats; //!< netdev statistics

    /* functions */
    int32_t (*netdev_xmit)(struct netdev_s *netdev, struct netb_s *netb); //!< function ptr, transmit network buffer
    void (*netdev_link)(struct netdev_s *netdev);                         //!< function ptr, change link speed
    void (*netdev_addr)(struct netdev_s *netdev);                         //!< function ptr, set MAC address, promiscous mode

    int32_t (*netdev_tc)(struct netdev_s *netdev, enum netdev_access access, enum tc_setup_type type, void *data); //!< function ptr, traffic control settings
    int32_t (*netdev_fpe)(struct netdev_s *netdev, enum netdev_access access, struct netdev_fpe *data);            //!< function ptr, frame preemption settings
    int32_t (*netdev_bridge)(struct netdev_s *netdev, enum netdev_access access, enum bridge_command cmd, void *data);   //!< function ptr, bridge command
    int32_t (*netdev_fdfifo)(struct netdev_s *netdev, enum netdev_access access, enum fdfifo_command cmd, void *data);   //!< function ptr, fdfifo command
    int32_t (*netdev_monitor)(struct netdev_s *netdev, enum netdev_access access, void *data);                           //!< function ptr, monitor command
    int32_t (*netdev_ethtool)(struct netdev_s *netdev, enum netdev_access access, enum ethtool_command cmd, void *data); //!< function ptr, monitor command
    int32_t (*netdev_cb)(struct netdev_s *netdev, enum netdev_access access, enum cb_command cmd, void *data);           //!< function ptr, cb command
    int32_t (*netdev_psfp)(struct netdev_s *netdev, enum netdev_access access, enum psfp_command cmd, void *data);       //!< function ptr, psfp command
    int32_t (*netdev_template)(struct netdev_s *netdev, enum netdev_access access, enum template_command cmd,
                               void *data); //!< function ptr for example command (template)

    uint8_t (*netdev_tstamp_register)(struct netdev_s *netdev, uint32_t qsize);          //!< function ptr, register tx-timestamp channel
    void (*netdev_tstamp_unregister)(struct netdev_s *netdev, uint8_t id);               //!< function ptr, unregister tx-timestamp channel
    int64_t (*netdev_tstamp_get)(struct netdev_s *netdev, uint8_t id, uint32_t timeout); //!< function ptr, get tx timestamp

    struct tsync_dev_s tsync; //!< tsync part
};

struct netdev_s *netdev_register(void);

//TODO deprecated !?
struct netdev_s *netdev_getbyid(uint32_t id);
//ERSTMAL
struct netdev_s *netdev_port2netdev(uint32_t port);

struct netdev_s *netdev_getbyname(char *ifname);

void netdev_set_name(struct netdev_s *netdev, const char *prefix, uint32_t index);

//TODO verschieben in netdev_xxx.h !?
int32_t netdev_xmit(struct netdev_s *netdev, struct netb_s *netb);
void netdev_adjust_link(struct netdev_s *netdev, int speed);
void netdev_set_addr(struct netdev_s *netdev, uint8_t *addr, uint8_t rx_promisc);
int32_t netdev_tc(struct netdev_s *netdev, enum netdev_access access, enum tc_setup_type type, void *data);
int32_t netdev_fpe(struct netdev_s *netdev, enum netdev_access access, struct netdev_fpe *data);
int32_t netdev_bridge(struct netdev_s *netdev, enum netdev_access access, enum bridge_command cmd, void *data);
int32_t netdev_fdfifo(struct netdev_s *netdev, enum netdev_access access, enum fdfifo_command cmd, void *data);
int32_t netdev_monitor(struct netdev_s *netdev, enum netdev_access access, void *data);
int32_t netdev_ethtool(struct netdev_s *netdev, enum netdev_access access, enum ethtool_command cmd, void *data);

void netdev_rx(struct netdev_s *netdev, struct netb_s *netb);

uint8_t netdev_tstamp_register(struct netdev_s *netdev, uint32_t qsize);
void netdev_tstamp_unregister(struct netdev_s *netdev, uint8_t id);
int64_t netdev_tstamp_get(struct netdev_s *netdev, uint8_t id, uint32_t timeout);

int32_t netdev_tsync_xmit(struct netdev_s *netdev, struct netb_s *netb);
void netdev_tsync_rx(void);

extern const char *NETDEV_CONTEXT;

#endif /* NETDEV_H_ */
