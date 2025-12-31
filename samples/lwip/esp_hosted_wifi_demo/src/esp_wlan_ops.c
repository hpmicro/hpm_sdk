/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

 #include "string.h"

#include "wifi.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_hosted.h"
#include "esp_wlan_ops.h"
#include "port_esp_hosted_host_os.h"

#include "hpm_common.h"

DEFINE_LOG_TAG(esp_wlan_ops);

extern bool g_sta_connected;
static const char *supported_countries[] = {
    "CN", "US", "JP", "EU"
};

static hpm_esp_hosted_wifi_event_callback g_wifi_event_cbs = NULL;

static int hosted_wifi_compute_max_rate(uint8_t protocol_bit_map, wifi_bandwidth_t bandwidth)
{
    int max_rate = 0;

    if (protocol_bit_map & (WIFI_PROTOCOL_11AC | WIFI_PROTOCOL_11AX | WIFI_PROTOCOL_11N)) {
        /* It's just a simple calculation
         * Assume minimum GI guard time and maximum Coding Rate
         * Since esp32 does not provide an API to obtain AP MIMO information,
         * and esp32 only does not support MIMO,
         * our evaluated rate is also based on 1x1.
         */
        int spatial_streams = 1;

        if (protocol_bit_map & WIFI_PROTOCOL_11AX) {
            switch (bandwidth) {
            case WIFI_BW20:
                max_rate = 143400;
                break;
            case WIFI_BW40:
                max_rate = 286800;
                break;
            case WIFI_BW80:
                max_rate = 600500;
                break;
            case WIFI_BW80_BW80:
            case WIFI_BW160:
                max_rate = 1201800;
                break;
            default:
                break;
            }
        } else if (protocol_bit_map & WIFI_PROTOCOL_11AC) {
            switch (bandwidth) {
            case WIFI_BW20:
                max_rate = 86700;
                break;
            case WIFI_BW40:
                max_rate = 200000;
                break;
            case WIFI_BW80:
                max_rate = 433300;
                break;
            case WIFI_BW80_BW80:
            case WIFI_BW160:
                max_rate = 866000;
                break;
            default:
                break;
            }
        } else if (protocol_bit_map & WIFI_PROTOCOL_11N) {
            switch (bandwidth) {
            case WIFI_BW20:
                max_rate = 72200;
                break;
            case WIFI_BW40:
                max_rate = 150000;
                break;
            default:
                break;
            }
        }
        max_rate *= spatial_streams;
    } else if (protocol_bit_map & (WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11G)) {
        max_rate = 54000;
    } else if (protocol_bit_map & WIFI_PROTOCOL_11B) {
        max_rate = 11000;
    } else if (protocol_bit_map & WIFI_PROTOCOL_LR) {
        max_rate = 2000;
    }

    return max_rate;
}

static int hosted_wifi_compute_max_rate_from_ap_record(wifi_ap_record_t *ap_info)
{
    uint8_t protocol_bit_map = 0;
    wifi_bandwidth_t bandwidth = ap_info->bandwidth;

    protocol_bit_map |= (ap_info->phy_11b) ? WIFI_PROTOCOL_11B : 0;
    protocol_bit_map |= (ap_info->phy_11g) ? WIFI_PROTOCOL_11G : 0;
    protocol_bit_map |= (ap_info->phy_11n) ? WIFI_PROTOCOL_11N : 0;
    protocol_bit_map |= (ap_info->phy_lr) ? WIFI_PROTOCOL_LR : 0;
    protocol_bit_map |= (ap_info->phy_11a) ? WIFI_PROTOCOL_11A : 0;
    protocol_bit_map |= (ap_info->phy_11ac) ? WIFI_PROTOCOL_11AC : 0;
    protocol_bit_map |= (ap_info->phy_11ax) ? WIFI_PROTOCOL_11AX : 0;

    return hosted_wifi_compute_max_rate(protocol_bit_map, bandwidth);
}

static hpm_esp_hosted_wifi_security_t hosted_wifi_ssecurity_esp_to_wlan (int encryption_mode)
{
    switch (encryption_mode) {
    case WIFI_AUTH_OPEN:
        return SECURITY_OPEN;
    case WIFI_AUTH_WEP:
        return SECURITY_WEP_PSK;
    case WIFI_AUTH_WPA_PSK:
        return SECURITY_WPA_AES_PSK;
    case WIFI_AUTH_WPA2_PSK:
        return SECURITY_WPA2_AES_PSK;
    case WIFI_AUTH_WPA_WPA2_PSK:
        return SECURITY_WPA2_AES_PSK;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return SECURITY_WPA2_AES_PSK;
    default:
        return SECURITY_UNKNOWN;
    }
}

static void hosted_wifi_send_ap_record_to_wlan(void const *parameter)
{
    wifi_event_sta_scan_done_t *info = (wifi_event_sta_scan_done_t *)parameter;
    uint16_t number = 0, i = 0;
    esp_err_t ret = ESP_OK;
    hpm_esp_hosted_wifi_info_t wlan_info = (hpm_esp_hosted_wifi_info_t){0};

    if (info) {
        number = info->number;
    }
    if (info) {
        g_h.funcs->_h_free(info);
        info = NULL;
    }

    if (number == 0) {
        (void)esp_wifi_scan_get_ap_num(&number);
    }
    if (number == 0) {
        ESP_LOGW(TAG, "No AP found");
        g_wifi_event_cbs(hosted_wifi_evt_scan_done, NULL, 0);
        vTaskDelete(NULL);
        return;
    }

    wifi_ap_record_t *ap_info = g_h.funcs->_h_calloc(number, sizeof(wifi_ap_record_t));
    if (ap_info == NULL) {
        ESP_LOGE(TAG, "No memory to alloc ap_record");
        vTaskDelete(NULL);
        return;
    }
    if (esp_wifi_scan_get_ap_records(&number, ap_info) == ESP_OK) {
        for (i = 0; i < number; i++) {
            g_h.funcs->_h_memcpy((void *)&wlan_info.ssid, (void *)ap_info[i].ssid, sizeof(wlan_info.bssid));
            wlan_info.ssid.len = sizeof(wlan_info.ssid.val);
            g_h.funcs->_h_memcpy(wlan_info.ssid.val, ap_info[i].ssid, wlan_info.ssid.len);
            if (wlan_info.ssid.len > 0) {
                wlan_info.hidden = 0;
            } else {
                wlan_info.hidden = 1;
            }
            wlan_info.channel = (int16_t)ap_info[i].primary;
            wlan_info.rssi = ap_info[i].rssi;
            wlan_info.datarate = hosted_wifi_compute_max_rate_from_ap_record(&ap_info[i]) * 1000;
            wlan_info.security = hosted_wifi_ssecurity_esp_to_wlan(ap_info[i].authmode);
            if (ap_info[i].phy_11b) {
                wlan_info.band |= RT_802_11_BAND_2_4GHZ;
            }
            if (ap_info[i].phy_11g) {
                wlan_info.band |= RT_802_11_BAND_2_4GHZ;
            }
            if (ap_info[i].phy_11n) {
                if (ap_info[i].second == 1) {
                    wlan_info.band |= RT_802_11_BAND_5GHZ;
                } else {
                    wlan_info.band |= RT_802_11_BAND_2_4GHZ;
                }
            }
            if (ap_info[i].phy_lr) {
                wlan_info.band |= RT_802_11_BAND_2_4GHZ;
            }
            g_wifi_event_cbs(hosted_wifi_evt_scan_report, (void *)&wlan_info, sizeof(wlan_info));
        }
    } else {
        ESP_LOGE(TAG, "Failed to get ap records: %d", ret);
    }
    g_h.funcs->_h_free(ap_info);
    g_wifi_event_cbs(hosted_wifi_evt_scan_done, NULL, 0);
    vTaskDelete(NULL);
}

int hosted_wifi_event_post(int32_t event_id, void *event_data, size_t event_data_size, uint32_t ticks_to_wait)
{
    hpm_esp_hosted_wifi_info_t wlan_info = {0};
    wifi_event_sta_disconnected_t *disconnect_info;
    wifi_event_sta_connected_t *connect_info;
    wifi_event_ap_staconnected_t *ap_sta_connected_info;
    wifi_event_ap_stadisconnected_t *ap_sta_disconnected_info;
    ESP_LOGD(TAG, "Event posted: %d", event_id);
    if (g_wifi_event_cbs == NULL) {
        ESP_LOGW(TAG, "No Wi-Fi event callback registered");
        return ESP_OK;
    }
    switch (event_id) {
    case WIFI_EVENT_STA_DISCONNECTED:
        disconnect_info = (wifi_event_sta_disconnected_t *)event_data;
        if (disconnect_info != NULL) {
            if (disconnect_info->reason >= WIFI_REASON_BEACON_TIMEOUT) {
                g_wifi_event_cbs(hosted_wifi_evt_connect_fail, event_data, event_data_size);
            }
            g_wifi_event_cbs(hosted_wifi_evt_disconnect, event_data, event_data_size);
            g_sta_connected = false;
        } else {
            ESP_LOGE(TAG, "WIFI_EVENT_STA_DISCONNECTED event data is NULL");
        }
        break;
    case WIFI_EVENT_STA_CONNECTED:
        connect_info = (wifi_event_sta_connected_t *)event_data;
        if (connect_info != NULL) {
            ESP_LOGI(TAG, "wifi connected SSID:%s \n", connect_info->ssid);
            g_wifi_event_cbs(hosted_wifi_evt_connect, NULL, 0);
            g_sta_connected = true;
        } else {
            ESP_LOGE(TAG, "WIFI_EVENT_STA_CONNECTED event data is NULL");
        }
        break;
    case WIFI_EVENT_AP_START:
        g_wifi_event_cbs(hosted_wifi_evt_ap_start, NULL, 0);
        break;
    case WIFI_EVENT_AP_STOP:
        g_wifi_event_cbs(hosted_wifi_evt_ap_stop, NULL, 0);
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        ap_sta_connected_info = (wifi_event_ap_staconnected_t *)event_data;
        memcpy(wlan_info.bssid, ap_sta_connected_info->mac, WLAN_BSSID_MAX_LENGTH);
        g_wifi_event_cbs(hosted_wifi_evt_ap_associated, (void *)&wlan_info, sizeof(wlan_info));
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        ap_sta_disconnected_info = (wifi_event_ap_stadisconnected_t *)event_data;
        memcpy(wlan_info.bssid, ap_sta_disconnected_info->mac, WLAN_BSSID_MAX_LENGTH);
        g_wifi_event_cbs(hosted_wifi_evt_ap_disassociated, (void *)&wlan_info, sizeof(wlan_info));
        break;
    case WIFI_EVENT_SCAN_DONE: {
        wifi_event_sta_scan_done_t *scan_done_info = (wifi_event_sta_scan_done_t *)event_data;
        if ((scan_done_info != NULL) && (scan_done_info->number > 0)) {
            wifi_event_sta_scan_done_t *copy = (wifi_event_sta_scan_done_t *)g_h.funcs->_h_malloc(sizeof(wifi_event_sta_scan_done_t));
            if (copy) {
                memcpy(copy, scan_done_info, sizeof(wifi_event_sta_scan_done_t));
                g_h.funcs->_h_thread_create("scan_ap_record",
                                            tskIDLE_PRIORITY + 1,
                                            1024,
                                            hosted_wifi_send_ap_record_to_wlan,
                                            (void *)copy);
            } else {
                ESP_LOGE(TAG, "malloc scan_done copy failed");
                g_wifi_event_cbs(hosted_wifi_evt_scan_done, NULL, 0);
            }
        } else {
            ESP_LOGW(TAG, "No AP found");
            g_wifi_event_cbs(hosted_wifi_evt_scan_done, NULL, 0);
        }
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

static hpm_stat_t hpm_esp_hosted_ensure_wifi_ap_mode(void)
{
    wifi_mode_t mode;
    if (esp_wifi_get_mode(&mode) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get Wi-Fi mode");
        return status_fail;
    }
    if (mode != WIFI_MODE_AP && mode != WIFI_MODE_APSTA) {
        if (mode == WIFI_MODE_STA) {
            ESP_LOGI(TAG, "Wi-Fi in STA mode, need set to APSTA mode to start softAP");
            if (esp_wifi_set_mode(WIFI_MODE_APSTA) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to set Wi-Fi mode to APSTA");
                return status_fail;
            }
        } else {
            ESP_LOGI(TAG, "Wi-Fi in not AP mode, need set to AP mode to start softAP");
            if (esp_wifi_set_mode(WIFI_MODE_AP) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to set Wi-Fi mode to AP");
                return status_fail;
            }
        }
    }
    return status_success;
}

void wifi_event_register_callbacks(hpm_esp_hosted_wifi_event_callback cbs)
{
    g_wifi_event_cbs = cbs;
}

hpm_stat_t hpm_esp_hosted_wifi_init(wifi_mode_t mode, wifi_storage_t storage)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    /* initialize the esp-hosted */
    if (esp_hosted_init() != ESP_OK) {
        ESP_LOGE(TAG, "esp-hosted init failed!");
        return status_fail;
    }

    /* initialize the wifi driver */
    if (esp_wifi_init(&cfg) != ESP_OK) {
        ESP_LOGE(TAG, "esp wifi init failed!");
        return status_fail;
    }
    /* set the wifi mode */
    if (esp_wifi_set_mode(mode) != ESP_OK) {
        ESP_LOGE(TAG, "esp wifi set mode failed!");
        return status_fail;
    }

    /* set the wifi storage mode */
    if (esp_wifi_set_storage(storage) != ESP_OK) {
        ESP_LOGE(TAG, "esp wifi set storage failed!");
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_start(void)
{
    esp_err_t ret;
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_stop(void)
{
    esp_err_t ret;
    ret = esp_wifi_stop();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop Wi-Fi: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_restore(void)
{
    esp_err_t ret;
    ret = esp_wifi_restore();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to restore Wi-Fi: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_internal_reg_rxcb(wifi_rxcb_t ap_fn, wifi_rxcb_t sta_fn)
{
    esp_err_t ret;
    if (ap_fn) {
        ret = esp_wifi_internal_reg_rxcb(WIFI_IF_AP, ap_fn);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to register AP RX callback: %d", ret);
            return status_fail;
        }
    }
    if (sta_fn) {
        ret = esp_wifi_internal_reg_rxcb(WIFI_IF_STA, sta_fn);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to register STA RX callback: %d", ret);
            return status_fail;
        }
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_scan(void)
{
    esp_err_t ret;
    if (hpm_esp_hosted_ensure_wifi_ap_mode() != status_success) {
        return status_fail;
    }
    ret = esp_wifi_scan_start(NULL, false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start scan: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_scan_stop(void)
{
    esp_err_t ret;
    ret = esp_wifi_scan_stop();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop scan: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_join(uint8_t *ssid, uint8_t *password, uint8_t ssid_len, uint8_t password_len)
{
    wifi_config_t wifi_config = {0};
    esp_err_t ret;

    if (ssid_len > sizeof(wifi_config.sta.ssid) || password_len > sizeof(wifi_config.sta.password)) {
        ESP_LOGE(TAG, "SSID or password length is too long");
        return status_fail;
    }
    if (hpm_esp_hosted_ensure_wifi_ap_mode() != status_success) {
        return status_fail;
    }

    memcpy(wifi_config.sta.ssid, ssid, ssid_len);
    memcpy(wifi_config.sta.password, password, password_len);
    wifi_config.sta.ssid[ssid_len] = '\0';
    wifi_config.sta.password[password_len] = '\0';
    wifi_config.sta.bssid_set = false;

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi config: %d", ret);
        return status_fail;
    }

    ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to AP: %d", ret);
        return status_fail;
    }

    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_disconnect(void)
{
    esp_err_t ret;
    ret = esp_wifi_disconnect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disconnect from AP: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_ap_stop(void)
{
    esp_err_t ret;
    wifi_mode_t mode;

    if (esp_wifi_get_mode(&mode) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get Wi-Fi mode");
        return status_fail;
    }

    if (mode == WIFI_MODE_AP) {
        mode = WIFI_MODE_NULL;
    } else if (mode == WIFI_MODE_APSTA) {
        mode = WIFI_MODE_STA;
    } else {
        ESP_LOGE(TAG, "Wi-Fi in not AP mode");
        return status_fail;
    }

    ret = esp_wifi_set_mode(mode);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode: %d", ret);
        return status_fail;
    }

    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_ap_deauth_sta(uint8_t *mac)
{
    uint16_t aid = 0;
    if (esp_wifi_ap_get_sta_aid(mac, &aid) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get sta aid");
        return status_fail;
    }
    if (aid == 0) {
        ESP_LOGE(TAG, "Station not found");
        return status_fail;
    }
    if (esp_wifi_deauth_sta(aid) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deauth sta");
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_set_softap(wifi_ap_config_t *ap_config)
{
    wifi_config_t wifi_config = {0};
    if (ap_config == NULL) {
        ESP_LOGE(TAG, "Invalid AP config");
        return status_fail;
    }
    if (hpm_esp_hosted_ensure_wifi_ap_mode() != status_success) {
        return status_fail;
    }
    memcpy(&wifi_config.ap, ap_config, sizeof(wifi_ap_config_t));
    if (esp_wifi_set_config(WIFI_IF_AP, &wifi_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set AP config");
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_get_rssi(int *rssi)
{
    if (rssi == NULL) {
        ESP_LOGE(TAG, "Invalid rssi pointer");
        return status_fail;
    }
    if (esp_wifi_sta_get_rssi(rssi) != ESP_OK) {
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_get_info(wifi_ap_record_t *ap_info)
{
    wifi_mode_t mode;
    if (ap_info == NULL) {
        ESP_LOGE(TAG, "Invalid ap_info pointer");
        return status_fail;
    }
    if (esp_wifi_get_mode(&mode) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get Wi-Fi mode");
        return status_fail;
    }
    if (mode != WIFI_MODE_STA && mode != WIFI_MODE_APSTA) {
        ESP_LOGE(TAG, "Wi-Fi in not STA mode");
        return status_fail;
    }
    if (esp_wifi_sta_get_ap_info(ap_info) != ESP_OK) {
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_set_powersave(wifi_ps_type_t level)
{
    esp_err_t ret;
    if (level >= WIFI_PS_MAX_MODEM) {
        ESP_LOGE(TAG, "Invalid power save level");
        return status_fail;
    }
    ret = esp_wifi_set_ps(level);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set power save mode: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_get_powersave(wifi_ps_type_t *level)
{
    esp_err_t ret;
    if (level == NULL) {
        ESP_LOGE(TAG, "Invalid power save level pointer");
        return status_fail;
    }
    ret = esp_wifi_get_ps(level);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get power save mode: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_set_channel(uint8_t channel)
{
    esp_err_t ret;
    ret = esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_ABOVE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set channel: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_get_channel(uint8_t *channel)
{
    esp_err_t ret;
    wifi_second_chan_t second;
    if (channel == NULL) {
        ESP_LOGE(TAG, "Invalid channel pointer");
        return status_fail;
    }
    ret = esp_wifi_get_channel(channel, &second);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get channel: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_set_country(const char *country, bool ieee80211d)
{
    uint32_t i;
    if (country == NULL) {
        ESP_LOGE(TAG, "Invalid country pointer");
        return status_fail;
    }
    if (strlen(country) != 2) {
        ESP_LOGE(TAG, "Country code must be 2 characters");
        return status_fail;
    }
    for (i = 0; i < sizeof(supported_countries)/sizeof(supported_countries[0]); i++) {
        if (strcmp(country, supported_countries[i]) == 0) {
            break;
        }
    }
    if (i == sizeof(supported_countries)/sizeof(supported_countries[0])) {
        ESP_LOGE(TAG, "Country code %s not supported", country);
        return status_fail;
    }
    if (esp_wifi_set_country_code(country, ieee80211d) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set country code");
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_get_country(char *country)
{
    if (country == NULL) {
        ESP_LOGE(TAG, "Invalid country pointer");
        return status_fail;
    }
    if (esp_wifi_get_country_code(country) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get country code");
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_set_mac(wifi_interface_t ifx, uint8_t *mac)
{
    esp_err_t ret;
    if (mac == NULL) {
        ESP_LOGE(TAG, "Invalid MAC pointer");
        return status_fail;
    }
    ret = esp_wifi_set_mac(ifx, mac);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set MAC address: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_get_mac(wifi_interface_t ifx, uint8_t *mac)
{
    esp_err_t ret;
    if (mac == NULL) {
        ESP_LOGE(TAG, "Invalid MAC pointer");
        return status_fail;
    }
    ret = esp_wifi_get_mac(ifx, mac);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get MAC address: %d", ret);
        return status_fail;
    }
    return status_success;
}

hpm_stat_t hpm_esp_hosted_wifi_ap(const char *ssid, const char *password)
{
    wifi_config_t wifi_config = {0};
    esp_err_t ret;

    if (!ssid || !password) {
        ESP_LOGE(TAG, "SSID or password is NULL");
        return status_fail;
    }
    if (strlen(ssid) > sizeof(wifi_config.ap.ssid) || strlen(password) > sizeof(wifi_config.ap.password)) {
        ESP_LOGE(TAG, "SSID or password length is too long");
        return status_fail;
    }
    strcpy((char *)wifi_config.ap.ssid, ssid);
    strcpy((char *)wifi_config.ap.password, password);
    wifi_config.ap.ssid_len = strlen(ssid);
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;

    if (hpm_esp_hosted_ensure_wifi_ap_mode() != status_success) {
        return status_fail;
    }

    ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set AP config: %d", ret);
        return status_fail;
    }

    ESP_LOGI(TAG, "AP started, SSID: %s", ssid);
    return status_success;
}

void hpm_esp_hosted_wifi_commands(int argc, char **argv)
{
    if (argc < 2) {
        ESP_LOGI(TAG, "Usage:");
        ESP_LOGI(TAG, "  wifi scan");
        ESP_LOGI(TAG, "  wifi join <SSID> <PASSWORD>");
        ESP_LOGI(TAG, "  wifi stop");
        return;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "scan") == 0) {
        if (hpm_esp_hosted_wifi_scan() != status_success) {
            ESP_LOGE(TAG, "Failed to start Wi-Fi scan");
        }
        return;
    }

    if (strcmp(cmd, "join") == 0) {
        if (argc < 4) {
            ESP_LOGI(TAG, "Usage: wifi join <SSID> <PASSWORD>");
            return;
        }
        const char *ssid = argv[2];
        const char *pwd  = argv[3];
        hpm_stat_t s = hpm_esp_hosted_wifi_join((uint8_t *)ssid, (uint8_t *)pwd,
                                                (uint8_t)strlen(ssid), (uint8_t)strlen(pwd));
        if (s == status_success) {
            ESP_LOGI(TAG, "Connecting to SSID:%s ...", ssid);
        } else {
            ESP_LOGE(TAG, "Failed to start STA connection");
        }
        return;
    }

    if (strcmp(cmd, "stop") == 0) {
        if (hpm_esp_hosted_wifi_disconnect() == status_success) {
            ESP_LOGI(TAG, "Disconnected from AP");
        } else {
            ESP_LOGE(TAG, "Failed to disconnect from AP");
        }
        return;
    }

    ESP_LOGI(TAG, "Unknown wifi command: %s", cmd);
    ESP_LOGI(TAG, "Try: wifi scan | wifi scanresults | wifi join <SSID> <PASSWORD> | wifi stop");
}