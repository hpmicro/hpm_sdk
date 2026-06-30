/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "hpm_tsw_phy_adaptive.h"

void tsw_phy_adaptive_poll(const tsw_phy_status_t *status, tsw_phy_adaptive_port_t *port)
{
    if ((status == NULL) || (port == NULL) || (port->last == NULL)) {
        return;
    }

    if (status->tsw_phy_link == tsw_phy_link_down) {
        if (tsw_phy_link_down_entered(port->last, status)) {
            tsw_phy_status_note_link_down(port->last);
            if (port->on_link_down != NULL) {
                port->on_link_down(status, port->ctx);
            }
        }
        return;
    }

    if (!tsw_phy_link_up_with_valid_speed(status)) {
        return;
    }

    if (tsw_phy_status_changed(port->last, status)) {
        memcpy(port->last, status, sizeof(tsw_phy_status_t));
        if (port->on_mac_apply != NULL) {
            port->on_mac_apply(status, port->ctx);
        }
    }
}
