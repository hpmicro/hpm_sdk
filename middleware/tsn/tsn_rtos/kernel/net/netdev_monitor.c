/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_monitor.c
 * \author fra11385
 * \date 13.04.2021
 * \brief <insert file brief description>
 *
 */

#include "tsn_rtos_config.h"

#if (USE_TSN_RTOS_NET_MONITOR == 1)
int a = 1 | 1;
#include "netdev_bridge.h"
#include <kernel/base/kernel.h>
#include <kernel/net/netdev.h>
#include <kernel/driver/tsn_ipms/ep/tsn_ctrl_drv.h>
#include <tsn_switch_monitor.h>

int32_t _netdev_monitor(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_ep_prv_t *prv =
        (tsn_ep_prv_t *)netdev
            ->prv; //TODO ACHTUNG im switch ist prv -> tsn_ctrl_prv_switch_t; hat aber als erstes word die baseaddress von central, somit passt der offset (MONITOR_OFS) wieder !!!
    monitor_data *_data = (monitor_data *)data;
    uint32_t baseaddr = prv->baseaddr + MONITOR_OFS; //TSN_SWITCH_OFS_MONITOR(netdev->port);

    switch (access) {
    case NETDEV_ACCESS_GET:
        monitor_get(baseaddr, _data);
        return NETDEV_RESULT_OK;
    case NETDEV_ACCESS_SET:
        monitor_set(baseaddr, _data);
        return NETDEV_RESULT_OK;
    case NETDEV_ACCESS_LIST:
        monitor_list(baseaddr, (list_data_t *)data);
        return NETDEV_RESULT_OK;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_ADD:
    case NETDEV_ACCESS_DEL:
    default:
        break;
    }

    return NETDEV_ERR_GENERAL;
}

#endif
