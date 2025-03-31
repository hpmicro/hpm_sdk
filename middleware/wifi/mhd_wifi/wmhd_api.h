/*
 * Copyright (c) 2024 Ampak
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef INCLUDED_WMHD_API_H
#define INCLUDED_WMHD_API_H

#include <stdint.h>

#define WMHD_MK_CNTRY(a, b)          (((unsigned char)(a)) + (((unsigned char)(b)) << 8))

//About GPIO Define
typedef enum {
    WL_REG_ON = 0,
    WL_HW_OOB = 1
} wmhd_gpio_type_t;

typedef enum {
    WMHD_802_11_BAND_2_4GHZ = 0, /**< Denotes 2.4GHz radio band  */
    WMHD_802_11_BAND_5GHZ, /**< Denotes 5GHz radio band    */
    WMHD_802_11_BAND_6GHZ, /**< Denotes 6GHz radio band    */
    WMHD_802_11_BAND_UNKNOWN, /**< Denotes unknown radio band */
} wmhd_802_11_band_t;

typedef enum {
    WMHD_SECURE_OPEN = 0, // OPEN
    WMHD_WPA_PSK_AES, // WPA-PSK AES
    WMHD_WPA2_PSK_AES, // WPA2-PSK AES
    WMHD_WEP_OPEN, // WEP+OPEN
    WMHD_WEP_SHARED, // WEP+SHARE
    WMHD_WPA_PSK_TKIP, // WPA-PSK TKIP
    WMHD_WPA_PSK_MIXED, // WPA-PSK AES & TKIP MIXED
    WMHD_WPA2_PSK_TKIP, // WPA2-PSK TKIP
    WMHD_WPA2_PSK_MIXED, // WPA2-PSK AES & TKIP MIXED
    WMHD_WPA2_WPA_PSK, // WPA2/WPA-PSK AES & TKIP MIXED
    WMHD_WPA2_PSK_SHA256, // WPA2-PSK-SHA256
    WMHD_WPA3_PSK_SAE, // WPA3-PSK SAE
    WMHD_WPA3_WPA2_PSK, // WPA3 SAE & WPA2-PSK AES
    WMHD_WPS_OPEN, // WPS OPEN, NOT supported
    WMHD_WPS_AES, // WPS AES, NOT supported
    WMHD_IBSS_OPEN, // ADHOC, NOT supported
    WMHD_WPA_ENT_AES, // WPA-ENT AES, NOT supported
    WMHD_WPA_ENT_TKIP, // WPA-ENT TKIP, NOT supported
    WMHD_WPA_ENT_MIXED, // WPA-ENT AES & TKIP MIXED, NOT supported
    WMHD_WPA2_ENT_AES, // WPA2-ENT AES, NOT supported
    WMHD_WPA2_ENT_TKIP, // WPA2-ENT TKIP, NOT supported
    WMHD_WPA2_ENT_MIXED, // WPA2-ENT AES & TKIP MIXED, NOT supported
    WMHD_STA_SECURITY_AUTO = 99, // MHD auto select security
    WMHD_SECURITY_MAX = 0x7f, // force host_security_t type to 8bits
    WMHD_SECURITY_UNKNOWN = -1 // Unknown security type
} wmhd_sta_security_t;

typedef enum {
    WMHD_AP_OPEN, // 0 OPEN
    WMHD_AP_WPA_AES_PSK, // 1 WPA-PSK AES
    WMHD_AP_WPA2_AES_PSK, // 2 WPA2-PSK AES
    WMHD_AP_WEP_OPEN, // 3 WEP+OPEN
    WMHD_AP_WEP_SHARED, // 4 WEP+SHARE
    WMHD_AP_WPA_TKIP_PSK, // 5 WPA-PSK TKIP
    WMHD_AP_WPA_MIXED_PSK, // 6 WPA-PSK AES & TKIP MIXED
    WMHD_AP_WPA2_TKIP_PSK, // 7 WPA2-PSK TKIP
    WMHD_AP_WPA2_MIXED_PSK, // 8 WPA2-PSK AES & TKIP MIXED
    WMHD_AP_WPA3_WPA2_PSK, // 9 WPA3 & WPA2 AES
    WMHD_AP_WPA3_SAE, //10 WPA3 SAE
    WMHD_AP_WPS_OPEN, //11 WPS OPEN, NOT supported
    WMHD_AP_WPS_AES, //12 WPS AES, NOT supported
    WMHD_AP_WPA2_WPA_AES_PSK, //13 WPA2/WPA-PSK AES
    WMHD_AP_WPA2_WPA_TKIP_PSK, //14 WPA2/WPA-PSK TKIP
    WMHD_AP_WPA2_WPA_MIXED_PSK, //15 WPA2/WPA-PSK AES & TKIP MIXED
} wmhd_ap_security_t;

typedef struct {
    uint8_t octet[6]; /* Unique 6-byte MAC address */
} wmhd_mac_t;

typedef struct {
    char ssid[32];
    char bssid[6];
    uint32_t channel;
    uint32_t security;
    uint32_t rssi;
    uint32_t data_rate;
    char ccode[4];
    uint8_t band;
    uint8_t mfp;
} wmhd_ap_info_t;

#ifdef __cplusplus
extern "C" {
#endif
extern void wmhd_gpio_config(uint8_t type, uint32_t port, uint32_t pin);

extern void wmhd_sdio_config(void *sdio_base[], uint8_t num);

extern void wmhd_sdio_set_slot_and_maxclk(uint8_t index, uint32_t clock);

extern void wmhd_thread_config(uint8_t type, uint32_t priority, uint32_t stack_size);

extern void wmhd_host_oob_isr(void);

extern void wmhd_host_sdio_isr(void);

extern void wmhd_set_country_code(uint32_t country);

extern void wmhd_get_country_code(uint32_t *country);

extern int wmhd_module_init(void);

extern int wmhd_module_exit(void);

extern int wmhd_get_mac_address(wmhd_mac_t *mac);

//extern int wmhd_set_mac_address( wmhd_mac_t *mac );

typedef void (*wmhd_scan_report_cb_t)(wmhd_ap_info_t *result);

extern int wmhd_scan_ap(wmhd_scan_report_cb_t scan_report_cb, wmhd_ap_info_t *scan_report_param);

extern int wmhd_scan_ap_ext(char *ssid_str,
                            char *mac_val,
                            uint16_t *channel_list,
                            wmhd_scan_report_cb_t scan_report_cb,
                            wmhd_ap_info_t *scan_report_param);

extern int wmhd_scan_aplist(wmhd_ap_info_t *results, int *num);

// station connects to ap. 0:success, others:failed
// security: 0-open, 1-wpa_psk_aes, 2-wpa2_psk_aes
extern int wmhd_sta_connect(char *ssid, char *bssid, uint8_t security, char *password, uint8_t channel);

extern int wmhd_sta_disconnect(void);

extern int wmhd_sta_get_connection(void);

extern int wmhd_join_ap(char *ssid, char *password);

extern int wmhd_leave_ap(void);

extern int wmhd_sta_network_up(uint32_t ip, uint32_t gateway, uint32_t netmask);

extern int wmhd_sta_network_down(void);

typedef void (*wmhd_link_cb_t)(void);

extern int wmhd_sta_register_link_callback(wmhd_link_cb_t link_up_cb, wmhd_link_cb_t link_down_cb);

extern int wmhd_sta_deregister_link_callback(wmhd_link_cb_t link_up_cb, wmhd_link_cb_t link_down_cb);

extern int wmhd_sta_get_rssi(void);

extern int wmhd_sta_get_rate(void);

extern int wmhd_sta_get_noise(void);

extern int wmhd_sta_get_bssid(char mac_addr[]);

extern int wmhd_sta_get_ssid(char ssid_data[]);

extern uint32_t wmhd_sta_ipv4_ipaddr(void);

extern uint32_t wmhd_sta_ipv4_gateway(void);

extern uint32_t wmhd_sta_ipv4_netmask(void);

extern int wmhd_sta_set_powersave(uint8_t mode, uint8_t time_ms);

extern int wmhd_sta_get_powersave(uint8_t *mode, uint8_t *time_ms);

extern int wmhd_sta_set_bcn_li_dtim(uint8_t dtim);

extern int wmhd_sta_get_bcn_li_dtim(void);

extern int wmhd_sta_set_dtim_interval(int dtim_interval_ms);

extern int wmhd_host_get_mac_address(wmhd_mac_t *mac);

extern void wmhd_host_set_mac_address(wmhd_mac_t mac);

extern void wmhd_sta_tput_patch(uint8_t enable);
extern void wmhd_ap_tput_patch(uint8_t enable);

// ssid:  less than 32 bytes
// password: less than 32 bytes
// security: 0-open, 1-wpa_psk_aes, 2-wpa2_psk_aes
// channel: 1~13
extern int wmhd_softap_start(char *ssid, char *password, uint8_t security, uint8_t channel);

extern int wmhd_softap_stop(void);

extern int wmhd_softap_set_hidden(uint8_t enable);

extern int wmhd_softap_get_hidden(void);

extern int wmhd_softap_start_dhcpd(uint32_t ip_address);

extern int wmhd_softap_stop_dhcpd(void);

extern int wmhd_softap_get_mac_list(wmhd_mac_t *mac_list, uint32_t *count);

extern int wmhd_softap_get_rssi(wmhd_mac_t *mac);

extern int wmhd_softap_deauth_assoc_sta(wmhd_mac_t *mac);

typedef void (*wmhd_client_cb_t)(wmhd_mac_t *);

extern int wmhd_softap_register_client_callback(wmhd_client_cb_t client_assoc_cb, wmhd_client_cb_t client_disassoc_cb);

extern int wmhd_wifi_get_channel(int interface, uint32_t *channel);

extern int wmhd_wifi_set_channel(int interface, uint32_t channel);

extern int wmhd_wifi_get_max_associations(uint32_t *max_assoc);

extern int wmhd_wifi_set_max_associations(uint32_t max_assoc);

extern int wmhd_enable_monitor_mode(void);

extern int wmhd_disable_monitor_mode(void);

typedef void (*wmhd_packet_cb_t)(void *buffer, int interface);

extern void wmhd_set_netif_rx_callback(wmhd_packet_cb_t function);

extern int wmhd_wifi_set_raw_packet_processor(wmhd_packet_cb_t function);

extern int wmhd_config_fwlog(uint8_t enable, uint32_t interval);

//WIFI Test Using
extern int wmhd_wifi_commands(uint32_t argc, char **argv);

extern int wmhd_wl_cmds(uint32_t argc, char **argv);

extern int wmhd_log_msg_level(uint32_t level);

extern int wmhd_lookup_hpm_sdio_index(void *sdio_base);

extern void  wmhd_config_hpm_sdio_instances(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef INCLUDED_MHD_API_H */