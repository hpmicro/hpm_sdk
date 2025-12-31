/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef ESP_HOSTED_WIFI_LWIP_H
#define ESP_HOSTED_WIFI_LWIP_H
#include "esp_hosted.h"

/* MAC Address */
#ifndef ESP_HOSTED_WIFI_AP_MAC_CONFIG
#define ESP_HOSTED_WIFI_AP_MAC_CONFIG 02:12:34:56:78:9A
#endif

#ifndef ESP_HOSTED_WIFI_STA_MAC_CONFIG
#define ESP_HOSTED_WIFI_STA_MAC_CONFIG 84:0D:8E:EF:10:50
#endif

void hw_esp_wlan_init(void *parameter);
#endif