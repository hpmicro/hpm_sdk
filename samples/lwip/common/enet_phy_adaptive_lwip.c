/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "enet_phy_adaptive_lwip.h"
#include "lwip/netif.h"
#include "board.h"

#if defined(NO_SYS) && !NO_SYS
#include "lwip/sys.h"
#include "hpm_enet_phy_common.h"
#endif

static void lwip_enet_adaptive_notify_link(const enet_phy_adaptive_binding_t *binding, bool link_up)
{
#if defined(NO_SYS) && !NO_SYS
    if ((binding->status_mbox == NULL) || (binding->link_msg == NULL)) {
        return;
    }

    *binding->link_msg = link_up ? enet_phy_link_up : enet_phy_link_down;
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

void lwip_enet_phy_adaptive_poll(const lwip_enet_phy_adaptive_binding_t *binding)
{
    enet_phy_adaptive_binding_t *mutable_binding = (enet_phy_adaptive_binding_t *)binding;

    if (binding == NULL) {
        return;
    }

    if (mutable_binding->notify_netif && (mutable_binding->notify_link == NULL)) {
        mutable_binding->notify_link = lwip_enet_adaptive_notify_link;
    }

    enet_phy_status_t status = {0};

    board_get_enet_phy_status(binding->phy_port, &status);
    enet_phy_adaptive_binding_poll(binding, &status);
}
