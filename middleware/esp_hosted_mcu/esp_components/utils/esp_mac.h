/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "sdkconfig.h"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_MAC_WIFI_STA,      /**< MAC for WiFi Station (6 bytes) */
    ESP_MAC_WIFI_SOFTAP,   /**< MAC for WiFi Soft-AP (6 bytes) */
    ESP_MAC_BT,            /**< MAC for Bluetooth (6 bytes) */
    ESP_MAC_ETH,           /**< MAC for Ethernet (6 bytes) */
    ESP_MAC_IEEE802154,    /**< if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC for IEEE802154 (8 bytes) */
    ESP_MAC_BASE,          /**< Base MAC for that used for other MAC types (6 bytes) */
    ESP_MAC_EFUSE_FACTORY, /**< MAC_FACTORY eFuse which was burned by Espressif in production (6 bytes) */
    ESP_MAC_EFUSE_CUSTOM,  /**< MAC_CUSTOM eFuse which was can be burned by customer (6 bytes) */
    ESP_MAC_EFUSE_EXT,     /**< if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC_EXT eFuse which is used as an extender for IEEE802154 MAC (2 bytes) */
} esp_mac_type_t;

/** @cond */
#define ONE_UNIVERSAL_MAC_ADDR 1
#define TWO_UNIVERSAL_MAC_ADDR 2
#define FOUR_UNIVERSAL_MAC_ADDR 4
#define UNIVERSAL_MAC_ADDR_NUM CONFIG_ESP_MAC_UNIVERSAL_MAC_ADDRESSES

#ifdef __cplusplus
}
#endif
