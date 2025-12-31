/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SLAVE_WIFI_CONFIG_H__
#define __SLAVE_WIFI_CONFIG_H__

#include "esp_idf_version.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 1)
#error ESP-IDF version used is not supported
#endif

/* ESP-IDF 5.5.0: renamed reserved fields to reserved1/reserved2 */
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
#define H_WIFI_NEW_RESERVED_FIELD_NAMES 1
#define H_PRESENT_IN_ESP_IDF_5_5_0      1
#define H_IF_AVAILABLE_SPI_SLAVE_ENABLE_DISABLE 1
#else
#define H_WIFI_NEW_RESERVED_FIELD_NAMES 0
#define H_PRESENT_IN_ESP_IDF_5_5_0      0
#define H_IF_AVAILABLE_SPI_SLAVE_ENABLE_DISABLE 0
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
#define H_PRESENT_IN_ESP_IDF_5_4_0      1
#else
#define H_PRESENT_IN_ESP_IDF_5_4_0      0
#endif

/*
 * wifi_twt_config_t::twt_enable_keep_alive only found in
 * IDF v5.3.2 and above
 */
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(5, 3, 1)
#define H_GOT_TWT_ENABLE_KEEP_ALIVE 1
#else
#define H_GOT_TWT_ENABLE_KEEP_ALIVE 0
#endif

/* wifi_ap_config_t::transition_disable only found in
 * IDF v5.3.3 and above, or
 * IDF v5.4.1 and above
 */
#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 3)) || (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 4, 1))
#define H_GOT_AP_CONFIG_PARAM_TRANSITION_DISABLE 0
#else
#define H_GOT_AP_CONFIG_PARAM_TRANSITION_DISABLE 1
#endif

/* WIFI_ENABLE_CACHE_TX_BUFFER only found in
 * IDF 5.3.3 and above
 * reused config for WIFI_ENABLE_SPIRAM
 */
#ifndef WIFI_ENABLE_CACHE_TX_BUFFER
#define WIFI_ENABLE_CACHE_TX_BUFFER WIFI_ENABLE_SPIRAM
#endif

#ifdef CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT
  #define H_WIFI_ENTERPRISE_SUPPORT 1
#else
  #define H_WIFI_ENTERPRISE_SUPPORT 0
#endif

#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 4)) || (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 4, 3)) || (H_PRESENT_IN_ESP_IDF_5_5_0)
#define H_GOT_SET_EAP_METHODS_API 0
#else
#define H_GOT_SET_EAP_METHODS_API 1
#endif

/**
 * Wi-Fi Easy Connect (DPP) events is returned to user via
 * Supplicant Callback or Wi-Fi DPP events,
 * depending on IDF version
 */
// Supplicant Callback DPP Events: still available, but deprecated
#if CONFIG_ESP_WIFI_DPP_SUPPORT
#define H_SUPP_DPP_SUPPORT 1
#else
#define H_SUPP_DPP_SUPPORT 0
#endif

// Wi-Fi DPP Events: only in IDF v5.5 and above
#if CONFIG_ESP_WIFI_DPP_SUPPORT && (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0))
#define H_WIFI_DPP_SUPPORT 1
#else
#define H_WIFI_DPP_SUPPORT 0
#endif

// for generic DPP support
#if H_SUPP_DPP_SUPPORT || H_WIFI_DPP_SUPPORT
#define H_DPP_SUPPORT 1
#else
#define H_DPP_SUPPORT 0
#endif

#endif
