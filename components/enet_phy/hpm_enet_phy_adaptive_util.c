/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "hpm_enet_phy_adaptive_util.h"

void enet_phy_adaptive_print_link_down(const char *log_prefix)
{
    if (log_prefix != NULL) {
        printf("%s Link Status: Down\n", log_prefix);
    } else {
        printf("Link Status: Down\n");
    }
}

void enet_phy_adaptive_print_port_banner(uint8_t netif_idx)
{
    printf("================ Network Interface %u ================\n", netif_idx);
}

void enet_phy_adaptive_print_link_up(const char *log_prefix, const enet_phy_status_t *status)
{
    static const char * const speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    static const char * const duplex_str[] = {"Half duplex", "Full duplex"};

    if (log_prefix != NULL) {
        printf("%s Link Status: Up\n", log_prefix);
        printf("%s Link Speed:  %s\n", log_prefix, speed_str[status->enet_phy_speed]);
        printf("%s Link Duplex: %s\n", log_prefix, duplex_str[status->enet_phy_duplex]);
    } else {
        printf("Link Status: Up\n");
        printf("Link Speed:  %s\n", speed_str[status->enet_phy_speed]);
        printf("Link Duplex: %s\n", duplex_str[status->enet_phy_duplex]);
    }
}

void enet_phy_adaptive_apply_mac(ENET_Type *enet_base, const enet_phy_status_t *status)
{
    enet_line_speed_t line_speed[] = {
        enet_line_speed_10mbps,
        enet_line_speed_100mbps,
        enet_line_speed_1000mbps
    };

    enet_set_line_speed(enet_base, line_speed[status->enet_phy_speed]);
    enet_set_duplex_mode(enet_base, status->enet_phy_duplex);
}
