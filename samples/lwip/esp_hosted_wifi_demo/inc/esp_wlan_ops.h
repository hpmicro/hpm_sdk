/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef ESP_WLAN_OPS_H
#define ESP_WLAN_OPS_H
#include "esp_hosted.h"
#include "esp_wifi_types.h"
#include "esp_wifi_crypto_types.h"
#include "esp_wifi_types_generic.h"
#include "wifi.h"

#ifndef WLAN_BSSID_MAX_LENGTH
#define WLAN_BSSID_MAX_LENGTH (6)    /* BSSID MAX LEN (default is 6) */
#endif

#ifndef WLAN_SSID_MAX_LENGTH
#define WLAN_SSID_MAX_LENGTH (31)
#endif

#define SHARED_ENABLED  0x00008000
#define WPA_SECURITY    0x00200000
#define WPA2_SECURITY   0x00400000
#define WPS_ENABLED     0x10000000
#define WEP_ENABLED     0x0001
#define TKIP_ENABLED    0x0002
#define AES_ENABLED     0x0004
#define WSEC_SWFLAG     0x0008

/**
 * Enumeration of Wi-Fi security modes
 */
typedef enum {
    SECURITY_OPEN           = 0,                                                /* Open security                           */
    SECURITY_WEP_PSK        = WEP_ENABLED,                                      /* WEP Security with open authentication   */
    SECURITY_WEP_SHARED     = (WEP_ENABLED | SHARED_ENABLED),                   /* WEP Security with shared authentication */
    SECURITY_WPA_TKIP_PSK   = (WPA_SECURITY  | TKIP_ENABLED),                   /* WPA Security with TKIP                  */
    SECURITY_WPA_AES_PSK    = (WPA_SECURITY  | AES_ENABLED),                    /* WPA Security with AES                   */
    SECURITY_WPA2_AES_PSK   = (WPA2_SECURITY | AES_ENABLED),                    /* WPA2 Security with AES                  */
    SECURITY_WPA2_TKIP_PSK  = (WPA2_SECURITY | TKIP_ENABLED),                   /* WPA2 Security with TKIP                 */
    SECURITY_WPA2_MIXED_PSK = (WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED),     /* WPA2 Security with AES & TKIP           */
    SECURITY_WPS_OPEN       = WPS_ENABLED,                                      /* WPS with open security                  */
    SECURITY_WPS_SECURE     = (WPS_ENABLED | AES_ENABLED),                      /* WPS with AES security                   */
    SECURITY_UNKNOWN        = -1,                                               /* May be returned by scan function if security is unknown.
                                                                                    Do not pass this to the join function! */
} hpm_esp_hosted_wifi_security_t;

typedef enum {
    RT_802_11_BAND_5GHZ  =  0,             /* Denotes 5GHz radio band   */
    RT_802_11_BAND_2_4GHZ =  1,            /* Denotes 2.4GHz radio band */
    RT_802_11_BAND_UNKNOWN = 0x7fffffff,   /* unknown */
} hpm_esp_hosted_wifi_802_11_band_t;


typedef enum {
    hosted_wifi_evt_init_done = 0,
    hosted_wifi_evt_connect,
    hosted_wifi_evt_connect_fail,
    hosted_wifi_evt_disconnect,
    hosted_wifi_evt_ap_start,
    hosted_wifi_evt_ap_stop,
    hosted_wifi_evt_ap_associated,
    hosted_wifi_evt_ap_disassociated,
    hosted_wifi_evt_ap_associate_failed,
    hosted_wifi_evt_scan_report,
    hosted_wifi_evt_scan_done,
    hosted_wifi_evt_max,
} hpm_esp_hosted_wifi_event_t;

typedef struct {
    uint8_t len;
    uint8_t val[WLAN_SSID_MAX_LENGTH + 1];
} hpm_esp_hosted_wifi_ssid_t;


typedef struct {
    /* security type */
    hpm_esp_hosted_wifi_security_t security;
    /* 2.4G/5G */
    hpm_esp_hosted_wifi_802_11_band_t band;
    /* maximal data rate */
    uint32_t datarate;
    /* radio channel */
    int16_t channel;
    /* signal strength */
    int16_t  rssi;
    /* ssid */
    hpm_esp_hosted_wifi_ssid_t ssid;
    /* hwaddr */
    uint8_t bssid[WLAN_BSSID_MAX_LENGTH];
    uint8_t hidden;
} hpm_esp_hosted_wifi_info_t;

typedef void (*hpm_esp_hosted_wifi_event_callback)(hpm_esp_hosted_wifi_event_t event, void *data, uint32_t data_len);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Wi-Fi subsystem
 *
 * @param mode     Wi-Fi operation mode (STA, AP, or APSTA)
 * @param storage  Wi-Fi storage type (RAM/Flash)
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_init(wifi_mode_t mode, wifi_storage_t storage);

/**
 * @brief Start Wi-Fi
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_start(void);

/**
 * @brief Stop Wi-Fi
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_stop(void);

/**
 * @brief Restore Wi-Fi configuration to default
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_restore(void);

/**
 * @brief Register Wi-Fi RX callbacks for AP and STA interfaces
 *
 * @param ap_fn   Callback for AP interface RX
 * @param sta_fn  Callback for STA interface RX
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_internal_reg_rxcb(wifi_rxcb_t ap_fn, wifi_rxcb_t sta_fn);

/**
 * @brief Start Wi-Fi scan.
 *
 * @return hpm_stat_t   status_success on success, status_fail on failure.
 */
hpm_stat_t hpm_esp_hosted_wifi_scan(void);

/**
 * @brief Stop Wi-Fi scan.
 *
 * @return hpm_stat_t   status_success on success, status_fail on failure.
 */
hpm_stat_t hpm_esp_hosted_wifi_scan_stop(void);

/**
 * @brief Join a Wi-Fi network with specified SSID and password.
 *
 * @param ssid          Pointer to SSID.
 * @param password      Pointer to password.
 * @param ssid_len      Length of SSID.
 * @param password_len  Length of password.
 * @return hpm_stat_t   status_success on success, status_fail on failure.
 */
hpm_stat_t hpm_esp_hosted_wifi_join(uint8_t *ssid, uint8_t *password, uint8_t ssid_len, uint8_t password_len);

/**
 * @brief Disconnect from the current Wi-Fi network.
 *
 * @return hpm_stat_t   status_success on success, status_fail on failure.
 */
hpm_stat_t hpm_esp_hosted_wifi_disconnect(void);

/**
 * @brief Stop Wi-Fi AP mode.
 *
 * @return hpm_stat_t   status_success on success, status_fail on failure.
 */
hpm_stat_t hpm_esp_hosted_wifi_ap_stop(void);

/**
 * @brief Deauthenticate a STA connected to the AP
 *
 * @param mac MAC address of the STA
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_ap_deauth_sta(uint8_t *mac);

/**
 * @brief Configure Wi-Fi SoftAP parameters
 *
 * @param ap_config Pointer to AP configuration structure
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_set_softap(wifi_ap_config_t *ap_config);

/**
 * @brief Get current Wi-Fi RSSI
 *
 * @param rssi Pointer to store RSSI value
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_get_rssi(int *rssi);

/**
 * @brief Get information of connected AP
 *
 * @param ap_info Pointer to store AP record
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_get_info(wifi_ap_record_t *ap_info);

/**
 * @brief Set Wi-Fi power save mode
 *
 * @param level Power save mode
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_set_powersave(wifi_ps_type_t level);

/**
 * @brief Get Wi-Fi power save mode
 *
 * @param level Pointer to store power save mode
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_get_powersave(wifi_ps_type_t *level);

/**
 * @brief Set Wi-Fi channel
 *
 * @param channel Channel number
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_set_channel(uint8_t channel);

/**
 * @brief Get Wi-Fi channel
 *
 * @param channel Pointer to store channel number
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_get_channel(uint8_t *channel);

/**
 * @brief Set Wi-Fi country code
 *
 * @param country     Two-letter country code
 * @param ieee80211d  Enable/disable 802.11d
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_set_country(const char *country, bool ieee80211d);

/**
 * @brief Get Wi-Fi country code
 *
 * @param country Pointer to store country code
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_get_country(char *country);

/**
 * @brief Set MAC address for Wi-Fi interface
 *
 * @param ifx Wi-Fi interface (STA/AP)
 * @param mac Pointer to MAC address (6 bytes)
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_set_mac(wifi_interface_t ifx, uint8_t *mac);

/**
 * @brief Get MAC address of Wi-Fi interface
 *
 * @param ifx Wi-Fi interface (STA/AP)
 * @param mac Pointer to store MAC address (6 bytes)
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_get_mac(wifi_interface_t ifx, uint8_t *mac);

/**
 * @brief Register Wi-Fi event callbacks
 *
 * @param cbs Pointer to structure containing event callbacks
 */
void wifi_event_register_callbacks(hpm_esp_hosted_wifi_event_callback cb);;

/**
 * @brief Connect to a Wi-Fi AP with given SSID and password (for shell command)
 *
 * @param ssid      SSID of the AP
 * @param password  Password of the AP
 *
 * @return status_success on success, status_fail on failure
 */
hpm_stat_t hpm_esp_hosted_wifi_ap(const char *ssid, const char *password);

/**
 * @brief Wi-Fi command handler for shell interface
 * @param argc Argument count
 * @param argv Argument vector
 */
void hpm_esp_hosted_wifi_commands(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif