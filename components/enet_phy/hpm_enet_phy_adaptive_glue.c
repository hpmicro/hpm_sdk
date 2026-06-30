/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_enet_phy_adaptive_glue.h"
#include "hpm_enet_phy_adaptive.h"
#include "hpm_enet_phy_adaptive_util.h"
#include "hpm_enet_phy_common.h"

static void enet_adaptive_glue_on_link_down(const enet_phy_status_t *status, void *ctx)
{
    const enet_phy_adaptive_binding_t *binding = (const enet_phy_adaptive_binding_t *)ctx;

    (void)status;

    if (binding->print_port_banner) {
        enet_phy_adaptive_print_port_banner(binding->netif_idx);
    }
    enet_phy_adaptive_print_link_down(binding->log_prefix);

    if (binding->notify_netif && (binding->notify_link != NULL)) {
        binding->notify_link(binding, false);
    }
}

static void enet_adaptive_glue_on_mac_apply(const enet_phy_status_t *status, void *ctx)
{
    const enet_phy_adaptive_binding_t *binding = (const enet_phy_adaptive_binding_t *)ctx;

    if (binding->print_port_banner) {
        enet_phy_adaptive_print_port_banner(binding->netif_idx);
    }
    enet_phy_adaptive_print_link_up(binding->log_prefix, status);
    enet_phy_adaptive_apply_mac(binding->enet_base, status);

    if (binding->notify_netif && (binding->notify_link != NULL)) {
        binding->notify_link(binding, true);
    }
}

void enet_phy_adaptive_binding_poll(const enet_phy_adaptive_binding_t *binding,
                                    const enet_phy_status_t *status)
{
    enet_phy_adaptive_port_t port;

    if ((binding == NULL) || (status == NULL)) {
        return;
    }

    port.last = binding->last;
    port.on_link_down = enet_adaptive_glue_on_link_down;
    port.on_mac_apply = enet_adaptive_glue_on_mac_apply;
    port.ctx = (void *)binding;

    enet_phy_adaptive_poll(status, &port);
}
