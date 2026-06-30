/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "hpm_tsw_phy_adaptive_util.h"

void tsw_phy_adaptive_print_link_down(const char *log_prefix)
{
    if (log_prefix != NULL) {
        printf("%s Link Status: Down\n", log_prefix);
    } else {
        printf("Link Status: Down\n");
    }
}

void tsw_phy_adaptive_print_port_banner(uint8_t tsw_port)
{
    printf("================ TSW PHY on Port%u ================\n", (unsigned int)(tsw_port + 1U));
}

void tsw_phy_adaptive_print_link_up(const char *log_prefix, const tsw_phy_status_t *status)
{
    static const char * const speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    static const char * const duplex_str[] = {"Half duplex", "Full duplex"};

    if (log_prefix != NULL) {
        printf("%s Link Status: Up\n", log_prefix);
        printf("%s Link Speed:  %s\n", log_prefix, speed_str[status->tsw_phy_speed]);
        printf("%s Link Duplex: %s\n", log_prefix, duplex_str[status->tsw_phy_duplex]);
    } else {
        printf("Link Status: Up\n");
        printf("Link Speed:  %s\n", speed_str[status->tsw_phy_speed]);
        printf("Link Duplex: %s\n", duplex_str[status->tsw_phy_duplex]);
    }
}

void tsw_phy_adaptive_clear_port_on_down(TSW_Type *tsw_base, uint8_t frame_dst_port)
{
    tsw_clear_unknown_frame_action(tsw_base, frame_dst_port);
    tsw_clear_broadcast_frame_action(tsw_base, frame_dst_port);
}

bool tsw_phy_adaptive_apply_port_on_up(TSW_Type *tsw_base, uint8_t tsw_port, uint8_t frame_dst_port,
                                       const tsw_phy_status_t *status)
{
    tsw_port_speed_t port_speed[] = {
        tsw_port_speed_10mbps,
        tsw_port_speed_100mbps,
        tsw_port_speed_1000mbps
    };

    if (!status->tsw_phy_duplex) {
        printf("Error: PHY is in half duplex now, but TSW MAC supports only full duplex mode!\n");
        return false;
    }

    tsw_set_port_speed(tsw_base, tsw_port, port_speed[status->tsw_phy_speed]);
    tsw_set_broadcast_frame_action(tsw_base, frame_dst_port);
    tsw_set_unknown_frame_action(tsw_base, frame_dst_port);

    return true;
}
