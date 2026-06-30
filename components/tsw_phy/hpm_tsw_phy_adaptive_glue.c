/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_tsw_phy_adaptive_glue.h"
#include "hpm_tsw_phy_adaptive.h"
#include "hpm_tsw_phy_adaptive_util.h"
#include "hpm_tsw_phy_common.h"

static void tsw_adaptive_glue_on_link_down(const tsw_phy_status_t *status, void *ctx)
{
    const tsw_phy_adaptive_binding_t *binding = (const tsw_phy_adaptive_binding_t *)ctx;

    (void)status;

    if (binding->print_port_banner) {
        tsw_phy_adaptive_print_port_banner(binding->tsw_port);
    }
    tsw_phy_adaptive_print_link_down(binding->log_prefix);
    tsw_phy_adaptive_clear_port_on_down(binding->tsw_base, binding->frame_dst_port);

    if (binding->notify_netif && (binding->notify_link != NULL)) {
        binding->notify_link(binding, false);
    }
}

static void tsw_adaptive_glue_on_mac_apply(const tsw_phy_status_t *status, void *ctx)
{
    const tsw_phy_adaptive_binding_t *binding = (const tsw_phy_adaptive_binding_t *)ctx;

    if (binding->print_port_banner) {
        tsw_phy_adaptive_print_port_banner(binding->tsw_port);
    }
    tsw_phy_adaptive_print_link_up(binding->log_prefix, status);

    if (!tsw_phy_adaptive_apply_port_on_up(binding->tsw_base, binding->tsw_port,
                                           binding->frame_dst_port, status)) {
        return;
    }

    if (binding->notify_netif && (binding->notify_link != NULL)) {
        binding->notify_link(binding, true);
    }
}

void tsw_phy_adaptive_binding_poll(const tsw_phy_adaptive_binding_t *binding,
                                   const tsw_phy_status_t *status)
{
    tsw_phy_adaptive_port_t port;

    if ((binding == NULL) || (status == NULL)) {
        return;
    }

    port.last = binding->last;
    port.on_link_down = tsw_adaptive_glue_on_link_down;
    port.on_mac_apply = tsw_adaptive_glue_on_mac_apply;
    port.ctx = (void *)binding;

    tsw_phy_adaptive_poll(status, &port);
}
