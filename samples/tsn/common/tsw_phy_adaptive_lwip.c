/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tsw_phy_adaptive_lwip.h"
#include "lwip/netif.h"
#include "board.h"
#include "hpm_tsw_phy_adaptive_util.h"

#if defined(CONFIG_ENET_PHY) && CONFIG_ENET_PHY
static bool lwip_tsw_fetch_phy_status(uint8_t tsw_port, tsw_phy_status_t *status)
{
    enet_phy_status_t enet_status = {0};

    if ((status == NULL) || (board_get_tsw_phy_status(tsw_port, &enet_status) != status_success)) {
        return false;
    }

    tsw_phy_adaptive_enet_to_tsw_status(&enet_status, status);
    return true;
}
#elif defined(CONFIG_TSW_PHY) && CONFIG_TSW_PHY
static bool lwip_tsw_fetch_phy_status(uint8_t tsw_port, tsw_phy_status_t *status)
{
    if (status == NULL) {
        return false;
    }

    return board_get_tsw_port_phy_status(tsw_port, status) == status_success;
}
#endif

#if defined(NO_SYS) && !NO_SYS
#include "lwip/sys.h"
#include "hpm_tsw_phy_common.h"
#endif

static void lwip_tsw_adaptive_notify_link(const tsw_phy_adaptive_binding_t *binding, bool link_up)
{
#if defined(NO_SYS) && !NO_SYS
    if ((binding->status_mbox == NULL) || (binding->link_msg == NULL)) {
        return;
    }

    *binding->link_msg = link_up ? tsw_phy_link_up : tsw_phy_link_down;
    sys_mbox_trypost_fromisr((sys_mbox_t *)binding->status_mbox, binding->link_msg);
#else
    struct netif *netif = netif_get_by_index(binding->netif_idx);

    if (netif == NULL) {
        return;
    }

    if (link_up) {
        netif_set_link_up(netif);
    } else {
        netif_set_link_down(netif);
    }
#endif
}

void lwip_tsw_phy_adaptive_poll(const lwip_tsw_phy_adaptive_binding_t *binding)
{
    tsw_phy_adaptive_binding_t *mutable_binding = (tsw_phy_adaptive_binding_t *)binding;

    if (binding == NULL) {
        return;
    }

    if (mutable_binding->notify_netif && (mutable_binding->notify_link == NULL)) {
        mutable_binding->notify_link = lwip_tsw_adaptive_notify_link;
    }

#if (defined(CONFIG_ENET_PHY) && CONFIG_ENET_PHY) || (defined(CONFIG_TSW_PHY) && CONFIG_TSW_PHY)
    tsw_phy_status_t status = {0};

    if (!lwip_tsw_fetch_phy_status(binding->tsw_port, &status)) {
        return;
    }

    tsw_phy_adaptive_binding_poll(binding, &status);
#endif
}
