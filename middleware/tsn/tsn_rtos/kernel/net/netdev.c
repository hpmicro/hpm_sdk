/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   netdev.c
 * \author zimmerli
 * \date   2020-01-16
 * \brief  Generic Network Device
 *
 */

#include "netdev.h"
#include <string.h>
#include <kernel/base/kernel.h>
#include <kernel/net/sock.h>

// ----------------------------------------------------------------

static uint32_t _net_dev_cnt = 0;

// TODO implementation might need a list -> currently avoid many malloc
#define GETBYID_MAX 8
static struct netdev_s *_device_ptr[GETBYID_MAX] = { 0 };
const char *NETDEV_CONTEXT = "NETDEV";

//TODO unknown device (name: "unknown", callbacks: unsupported..., ... !?
const struct netdev_s UNKNOWN_DEVICE = { 0 };

// ----------------------------------------------------------------

/**
 * \brief Allocate and register network device driver
 *
 * @return      pointer to structure, NULL on error
 */
struct netdev_s *netdev_register(void)
{
    struct netdev_s *p;

    // alloc struct
    p = (struct netdev_s *)kallocz(sizeof(struct netdev_s));
    if (!p)
        return p;

    // set fields
    memset(p->name, 0, IFNAMSIZ);
    p->id = _net_dev_cnt;
    _net_dev_cnt++;

    // register
    if (p->id < GETBYID_MAX) {
        _device_ptr[p->id] = p;
    }

    return p;
}

/**
 * \brief Get registered network device driver by interface name
 *
 * @param name    interface name
 * @return      pointer to device driver, NULL if not found
 */
struct netdev_s *netdev_getbyname(char *name)
{
    struct netdev_s *ret = NULL;

    for (int id = 0; id < GETBYID_MAX; id++) {
        ret = _device_ptr[id];
        if (ret && strcmp(ret->name, name) == 0)
            return ret;
    }

    return NULL;
}

/**
 * \brief Get registered network device driver by id
 *
 * Maximum supported number of devices -> GETBYID_MAX = 8
 *
 * @param id    id
 * @return      pointer to device driver, NULL if not found
 */
struct netdev_s *netdev_getbyid(uint32_t id)
{
    if (id >= GETBYID_MAX)
        return NULL;
    return _device_ptr[id];
}

struct netdev_s *netdev_port2netdev(uint32_t port)
{
    struct netdev_s *ret = NULL;

    for (int id = 0; id < GETBYID_MAX; id++) {
        ret = _device_ptr[id];
        if (ret && ret->tsn_port == port)
            return ret;
    }

    logk(LOG_ALWAYS, NETDEV_CONTEXT, "device not found for tsn port '%d'\n", port);

    return NULL;
}

void netdev_set_name(struct netdev_s *netdev, const char *prefix, uint32_t index)
{
    char str[5];
    _itoa(index, str, 10);
    strcpy(netdev->name, prefix);
    strcat(netdev->name, str);
}

//---------------------------------------------------------------------------------------------------------------------
//TODO sind das driver function? wo geh�ren die hin

/**
 * \brief Network device transmit
 *
 * @param netdev        pointer to device driver
 * @param netb          pointer to network buffer
 * @return              NETDEV_RESULT_OK on success
 */
int32_t netdev_xmit(struct netdev_s *netdev, struct netb_s *netb)
{
    int32_t retval = NETDEV_ERR_UNSUPPORTED;

    if (netdev && netdev->netdev_xmit) {
        netb->netdev = netdev;
        retval = netdev->netdev_xmit(netdev, netb);
    } else {
        netdev->stats.tx_error++;
    }

    return retval;
}

/**
 * \brief Network device update link speed
 *
 * @param netdev        pointer to device driver
 * @param speed         speed in mbps (10/100/1000)
 */
void netdev_adjust_link(struct netdev_s *netdev, int speed)
{
    if (!netdev)
        return;
    netdev->speed = speed;
    if (netdev->netdev_link)
        netdev->netdev_link(netdev);
}

/**
 * \brief Network device set MAC address, promiscous mode
 * @param netdev        pointer to device driver
 * @param addr          pointer to MAC address
 * @param rx_promisc    promiscous mode flag
 */
void netdev_set_addr(struct netdev_s *netdev, uint8_t *addr, uint8_t rx_promisc)
{
    uint32_t idx;

    if (!netdev)
        return;
    for (idx = 0; idx < MAC_ADDR_LEN; ++idx)
        netdev->dev_addr[idx] = addr[idx];
    netdev->promisc = rx_promisc;
    if (netdev->netdev_addr)
        netdev->netdev_addr(netdev);
}

/**
 * \brief Network device, traffic control settings
 *
 * @param netdev        pointer to device driver
 * @param access        access type (get/set)
 * @param type          traffic control type (cbs, tas)
 * @param data          pointer to settings structure
 * @return
 */
int32_t netdev_tc(struct netdev_s *netdev, enum netdev_access access, enum tc_setup_type type, void *data)
{
    if (!netdev)
        return NETDEV_ERR_GENERAL;
    if (!netdev->netdev_tc)
        return NETDEV_ERR_UNSUPPORTED;
    return netdev->netdev_tc(netdev, access, type, data);
}

/**
 * \brief Network device, frame preemption settings
 *
 * @param netdev        pointer to device driver
 * @param access        access type (get/set)
 * @param data          pointer to settings structure
 * @return
 */
int32_t netdev_fpe(struct netdev_s *netdev, enum netdev_access access, struct netdev_fpe *data)
{
    if (!netdev)
        return NETDEV_ERR_GENERAL;
    if (!netdev->netdev_fpe)
        return NETDEV_ERR_UNSUPPORTED;
    return netdev->netdev_fpe(netdev, access, data);
}

/**
 * \brief Network device, ethtool settings
 *
 * @param netdev        pointer to device driver
 * @param access        access type (get/set)
 * @param data          pointer to settings structure
 * @return
 */
int32_t netdev_ethtool(struct netdev_s *netdev, enum netdev_access access, enum ethtool_command cmd, void *data)
{
    if (!netdev || !netdev->netdev_ethtool)
        return NETDEV_ERR_GENERAL;
    return netdev->netdev_ethtool(netdev, access, cmd, data);
}

/**
 * \brief Network device, monitor settings
 *
 * @param netdev        pointer to device driver
 * @param access        access type (get/set)
 * @param data          pointer to settings structure
 * @return
 */
int32_t netdev_monitor(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    if (!netdev || !netdev->netdev_monitor)
        return NETDEV_ERR_GENERAL;
    return netdev->netdev_monitor(netdev, access, data);
}

/**
 * \brief Network device, bridge settings
 *
 * @param netdev        pointer to device driver
 * @param access        access type (get/set)
 * @param data          pointer to settings structure
 * @return
 */
int32_t netdev_bridge(struct netdev_s *netdev, enum netdev_access access, enum bridge_command cmd, void *data)
{
    if (!netdev || !netdev->netdev_bridge)
        return NETDEV_ERR_GENERAL;
    return netdev->netdev_bridge(netdev, access, cmd, data);
}

/**
 * \brief Network device, fdfifo settings
 *
 * @param netdev        pointer to device driver
 * @param access        access type (get/set)
 * @param data          pointer to settings structure
 * @return
 */
int32_t netdev_fdfifo(struct netdev_s *netdev, enum netdev_access access, enum fdfifo_command cmd, void *data)
{
    if (!netdev || !netdev->netdev_fdfifo)
        return NETDEV_ERR_GENERAL;
    return netdev->netdev_fdfifo(netdev, access, cmd, data);
}

/**
 * \brief Network device, general receive funtion
 *
 * @param netdev        pointer to network device
 * @param netb          pointer to received network buffer
 */
void netdev_rx(struct netdev_s *netdev, struct netb_s *netb)
{
    socket_rx(netdev, netb);

    //TODO see socket_rx()
    //sp�ter (+ alte applikationen (lni, linktest, rtping, ...) gehen �ber RAW socket)
    //netb --> pbuf
    //netb --> netdev --> netif
    //netif->input(pbuf, netif)
}

/**
 * \brief Network device, register tx-timestamping channel
 *
 * @param netdev        pointer to device driver
 * @param qsize         size for queue holding timestamps
 * @return              channel id, 0 otherwise
 */
uint8_t netdev_tstamp_register(struct netdev_s *netdev, uint32_t qsize)
{
    if (!netdev || !netdev->netdev_tstamp_register)
        return 0;
    return netdev->netdev_tstamp_register(netdev, qsize);
}

/**
 * \brief Network device, unregister tx-timestamping channel
 *
 * @param netdev        pointer to device driver
 * @param id            channel id, previously registered
 */
void netdev_tstamp_unregister(struct netdev_s *netdev, uint8_t id)
{
    if (!netdev || !netdev->netdev_tstamp_unregister)
        return;
    netdev->netdev_tstamp_unregister(netdev, id);
}

/**
 * \brief Get tx-timestamp from channel
 *
 * Wait for reception until timeout. Use timeout=0 for immediate return.
 *
 * @param netdev        pointer to device driver
 * @param id            channel id, previously registered
 * @param timeout       timeout in system OS ticks (likely milliseconds)
 * @return              >0 timestamp; <= 0 on error
 */
int64_t netdev_tstamp_get(struct netdev_s *netdev, uint8_t id, uint32_t timeout)
{
    if (!netdev || !netdev->netdev_tstamp_get)
        return -1;
    return netdev->netdev_tstamp_get(netdev, id, timeout);
}
