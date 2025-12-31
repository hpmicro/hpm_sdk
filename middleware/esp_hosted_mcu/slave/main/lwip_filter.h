/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __LWIP_FILTER_H__
#define __LWIP_FILTER_H__

#include <stdint.h>
#include <stdbool.h>
#include <sdkconfig.h>

typedef enum {
	SLAVE_LWIP_BRIDGE,
	HOST_LWIP_BRIDGE,
	BOTH_LWIP_BRIDGE,
	INVALID_BRIDGE,
} hosted_l2_bridge;

#if defined(CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED) && defined(CONFIG_LWIP_ENABLE)
#include "esp_hosted_lwip_src_port_hook.h"

hosted_l2_bridge filter_and_route_packet(void *frame_data, uint16_t frame_length);

int configure_host_static_port_forwarding_rules(const char *ports_str_tcp_src, const char *ports_str_tcp_dst,
                                                const char *ports_str_udp_src, const char *ports_str_udp_dst);
#endif

#endif
