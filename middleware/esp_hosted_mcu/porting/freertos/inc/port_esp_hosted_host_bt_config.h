/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 * copyright (c) 2025, HPMicro
 * 
 */

#ifndef __PORT_ESP_HOSTED_HOST_BT_CONFIG_H__
#define __PORT_ESP_HOSTED_HOST_BT_CONFIG_H__

#include "esp_idf_version.h"

#ifndef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
#define CONFIG_BT_BLE_50_FEATURES_SUPPORTED 0
#endif
#ifndef CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT
#define CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT 0
#endif

#ifndef CONFIG_ESP_HOSTED_ENABLE_BT_NIMBLE
#define CONFIG_ESP_HOSTED_ENABLE_BT_NIMBLE 0
#endif

#ifndef CONFIG_ESP_HOSTED_NIMBLE_HCI_VHCI
#define CONFIG_ESP_HOSTED_NIMBLE_HCI_VHCI 0
#endif

#ifndef CONFIG_ESP_HOSTED_ENABLE_BT_BLUEDROID
#define CONFIG_ESP_HOSTED_ENABLE_BT_BLUEDROID 0
#endif

#ifndef CONFIG_ESP_HOSTED_BLUEDROID_HCI_VHCI
#define CONFIG_ESP_HOSTED_BLUEDROID_HCI_VHCI 1
#endif
// check: if co-processor SOC is ESP32, only BT BLE 4.2 is supported
#if H_SLAVE_TARGET_ESP32
#if CONFIG_BT_BLE_50_FEATURES_SUPPORTED || CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT
#error "ESP32 co-processor only supports BLE 4.2"
#endif
#endif

// Hosted BT defines for NimBLE
#define H_BT_HOST_ESP_NIMBLE CONFIG_ESP_HOSTED_ENABLE_BT_NIMBLE
#define H_BT_USE_VHCI CONFIG_ESP_HOSTED_NIMBLE_HCI_VHCI

// Hosted BT defines for BlueDroid
#define H_BT_HOST_ESP_BLUEDROID CONFIG_ESP_HOSTED_ENABLE_BT_BLUEDROID

#define H_BT_BLUEDROID_USE_VHCI CONFIG_ESP_HOSTED_BLUEDROID_HCI_VHCI

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
// ll_init required
#define H_BT_ENABLE_LL_INIT 1
#else
#define H_BT_ENABLE_LL_INIT 0
#endif

// check: only one BT host stack can be enabled at a time
#if H_BT_HOST_ESP_NIMBLE && H_BT_HOST_ESP_BLUEDROID
#error "Enable only NimBLE or BlueDroid, not both"
#endif

#endif
