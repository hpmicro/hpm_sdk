/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_bridge.c
 * \author fra11385
 * \date 07.04.2021
 * \brief <insert file brief description>
 *
 */

#include "netdev_bridge.h"
#include "tsn_rtos_config.h"

#if (USE_TSN_RTOS_NET_BRIDGE == 1)

#include <kernel/base/kernel.h>
#include <kernel/net/netdev.h>
#include <kernel/driver/tsn_ipms/sx/tsn_ctrl_switch_drv.h>
#include <tsn_switch_lookup.h>

void _bridge_hitmem(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_sx_prv_t *prv = (tsn_sx_prv_t *)netdev->prv;

    switch (access) {
    case NETDEV_ACCESS_SET:
        lookup_clear_hitmem(prv->baseAddresses.base_central);
        break;
    case NETDEV_ACCESS_LIST:
        lookup_get_hitmem_list(prv->baseAddresses.base_central, (list_data_t *)data);
        break;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_GET:
    case NETDEV_ACCESS_ADD:
    case NETDEV_ACCESS_DEL:
    default:
        break;
    }
}

void _bridge_vlan(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_sx_prv_t *prv = (tsn_sx_prv_t *)netdev->prv;
    vlan_entry_data *_data = (vlan_entry_data *)data;

    switch (access) {
    case NETDEV_ACCESS_SET:
        lookup_set_vlan(prv->baseAddresses.base_central, _data);
        break;
    case NETDEV_ACCESS_LIST:
        lookup_get_vlan_list(prv->baseAddresses.base_central, (list_data_t *)data);
        break;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_GET:
    case NETDEV_ACCESS_ADD:
    case NETDEV_ACCESS_DEL:
    default:
        break;
    }
}

void _bridge_fdb(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_sx_prv_t *prv = (tsn_sx_prv_t *)netdev->prv;
    route_entry_data *_data = (route_entry_data *)data;

    switch (access) {
    case NETDEV_ACCESS_SET:
        lookup_set_entry(prv->baseAddresses.base_central, _data);
        break;
    case NETDEV_ACCESS_GET:
        lookup_get_entry(prv->baseAddresses.base_central, _data, NULL);
        break;
    case NETDEV_ACCESS_LIST:
        lookup_get_entry_list(prv->baseAddresses.base_central, (list_data_t *)data);
        break;
    case NETDEV_ACCESS_DEL:
        lookup_clear_entry_ext(prv->baseAddresses.base_central, _data);
        break;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_ADD:
    default:
        break;
    }
}

void _bridge_fw(struct netdev_s *netdev, enum netdev_access access, void *data)
{
    tsn_sx_prv_t *prv = (tsn_sx_prv_t *)netdev->prv;
    lookup_action_list_data *_data = (lookup_action_list_data *)data;
    uint8_t ext = (prv->coreInfo->nports >= LOOKUP_EXTEND_PORT_NUM);

    switch (access) {
    case NETDEV_ACCESS_SET:
        lookup_set_action_list(prv->baseAddresses.base_central, _data, ext);
        break;
    case NETDEV_ACCESS_GET:
        lookup_get_action_list(prv->baseAddresses.base_central, _data, ext);
        break;

    /* TODO error message ?! access not supported  */
    case NETDEV_ACCESS_LIST:
    case NETDEV_ACCESS_ADD:
    case NETDEV_ACCESS_DEL:
    default:
        break;
    }
}

//------------------------------------------------------------------------------------------------------------------

int32_t _netdev_bridge_unsupported(struct netdev_s *netdev, enum netdev_access access, enum bridge_command cmd, void *data)
{
    switch (cmd) {
    case BRIDGE_FDB:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: bridge (fdb) not supported\n", netdev->name);
        break;
    case BRIDGE_VLAN:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: bridge (vlan) not supported\n", netdev->name);
        break;
    case BRIDGE_FW:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: bridge (forward) not supported\n", netdev->name);
        break;
    default:
        logk(LOG_ALWAYS, NETDEV_CONTEXT, "%s: bridge (command '%d') not supported\n", netdev->name, cmd);
        break;
    }

    return NETDEV_ERR_UNSUPPORTED;
}

int32_t _netdev_bridge(struct netdev_s *netdev, enum netdev_access access, enum bridge_command cmd, void *data)
{
    switch (cmd) {
    case BRIDGE_HITMEM:
        _bridge_hitmem(netdev, access, data);
        return NETDEV_RESULT_OK;
    case BRIDGE_FDB:
        _bridge_fdb(netdev, access, data);
        return NETDEV_RESULT_OK;
    case BRIDGE_VLAN:
        _bridge_vlan(netdev, access, data);
        return NETDEV_RESULT_OK;
    case BRIDGE_FW:
        _bridge_fw(netdev, access, data);
        return NETDEV_RESULT_OK;
    default: /* TODO error message ?! */
        break;
    }

    return NETDEV_ERR_GENERAL;
}

#endif
