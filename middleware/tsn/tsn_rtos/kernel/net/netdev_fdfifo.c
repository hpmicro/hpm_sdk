/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_fdfifo.c
 * \author fra11385
 * \date 07.04.2021
 * \brief <insert file brief description>
 *
 */

#include <kernel/net/netdev_fdfifo.h>
#include "tsn_rtos_config.h"

#if USE_TSN_RTOS_NET_FDFIFO

#include <kernel/base/kernel.h>
#include <kernel/net/netdev.h>
#include <kernel/driver/tsn_ipms/ep/tsn_ctrl_drv.h>
#include <tsn_switch_rx_fdfifo.h>

static void _fdfifo_sf(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)netdev->prv;

    switch (access) {
    case NETDEV_ACCESS_SET:
        rx_fdfifo_set_store_forward(prv->baseaddr, *((uint16_t *)data));
        break;
    case NETDEV_ACCESS_GET:
        *((uint16_t *)data) = rx_fdfifo_get_store_forward(prv->baseaddr);
        break;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_ADD:
    case NETDEV_ACCESS_DEL:
    case NETDEV_ACCESS_LIST:
    default:
        break;
    }
}

static void _fdfifo_pm(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)netdev->prv;

    switch (access) {
    case NETDEV_ACCESS_SET:
        rx_fdfifo_set_port_mask(prv->baseaddr, *((uint16_t *)data));
        break;
    case NETDEV_ACCESS_GET:
        *((uint16_t *)data) = rx_fdfifo_get_port_mask(prv->baseaddr);
        break;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_ADD:
    case NETDEV_ACCESS_DEL:
    case NETDEV_ACCESS_LIST:
    default:
        break;
    }
}

int32_t _netdev_fdfifo_unsupported(struct netdev_s *netdev, enum netdev_access access, enum fdfifo_command cmd, void *data)
{
    switch (cmd) {
    case FDFIFO_SF:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: fdfifo (store&forward) not supported\n", netdev->name);
        break;
    case FDFIFO_PM:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: fdfifo (portmask) not supported\n", netdev->name);
        break;
    default:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: fdfifo (command '%d') not supported\n", netdev->name, cmd);
        break;
    }

    return NETDEV_ERR_UNSUPPORTED;
}

int32_t _netdev_fdfifo(struct netdev_s *netdev, enum netdev_access access, enum fdfifo_command cmd, void *data)
{
    switch (cmd) {
    case FDFIFO_SF:
        _fdfifo_sf(netdev, access, data);
        return NETDEV_RESULT_OK;
    case FDFIFO_PM:
        _fdfifo_pm(netdev, access, data);
        return NETDEV_RESULT_OK;
    default:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: fdfifo (command '%d') not supported\n", netdev->name, cmd);
        break;
    }

    return NETDEV_ERR_GENERAL;
}

#endif