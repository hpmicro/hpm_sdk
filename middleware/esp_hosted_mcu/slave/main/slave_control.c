/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "slave_control.h"
#include "esp_hosted_rpc.pb-c.h"
#include "esp_ota_ops.h"
#include "esp_mac.h"
#include "esp_hosted_rpc.h"
#include "esp_hosted_transport.h"
#include "esp_hosted_bitmasks.h"
#include "slave_wifi_config.h"
#include "esp_hosted_log.h"
#include "slave_bt.h"
#include "esp_hosted_coprocessor_fw_ver.h"

#if H_DPP_SUPPORT
#include "esp_dpp.h"
#endif

#if CONFIG_SOC_WIFI_HE_SUPPORT
#include "esp_wifi_he.h"
#endif
#if H_WIFI_ENTERPRISE_SUPPORT
#include "esp_eap_client.h"
#endif
/* Slave-side: Always support reserved field decoding for maximum compatibility
 * The host may or may not have CONFIG_ESP_HOSTED_DECODE_WIFI_RESERVED_FIELD enabled
 */
#define H_DECODE_WIFI_RESERVED_FIELD 1

#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
#include "esp_check.h"
#include "lwip/inet.h"
#include "host_power_save.h"
#include "mqtt_example.h"
#endif

#define MAC_STR_LEN                 17
#define MAC2STR(a)                  (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR                      "%02x:%02x:%02x:%02x:%02x:%02x"
#define SUCCESS                     0
#define FAILURE                     -1
#define MIN_TX_POWER                8
#define MAX_TX_POWER                84

#define IFACE_MAC_SIZE              8 // 6 for MAC-48, 8 for EIU-64, 2 for EFUSE_EXT

#define MAX_STA_CONNECT_ATTEMPTS    3

#define TIMEOUT_IN_MIN              (60*TIMEOUT_IN_SEC)
#define TIMEOUT_IN_HOUR             (60*TIMEOUT_IN_MIN)
#define RESTART_TIMEOUT             (5*TIMEOUT_IN_SEC)

#define MIN_HEARTBEAT_INTERVAL      (10)
#define MAX_HEARTBEAT_INTERVAL      (60*60)



static wifi_config_t new_wifi_config = {0};
static bool new_config_recvd = false;
static wifi_event_sta_connected_t lkg_sta_connected_event = {0};

#if H_WIFI_ENTERPRISE_SUPPORT
#define CLEAR_CERT(ptr, len) \
    do { \
        if (ptr) { \
            memset(ptr, 0, len); \
            free(ptr); \
            ptr = NULL; \
        } \
        len = 0; \
    } while (0)
#endif

typedef struct esp_rpc_cmd {
	int req_num;
	esp_err_t (*command_handler)(Rpc *req,
			Rpc *resp, void *priv_data);
} esp_rpc_req_t;


static const char* TAG = "slave_rpc";
static TimerHandle_t handle_heartbeat_task;
static uint32_t hb_num;

/* FreeRTOS event group to signal when we are connected*/
static esp_event_handler_instance_t instance_any_id;
#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
static esp_event_handler_instance_t instance_ip;
extern volatile uint8_t station_got_ip;
static rpc_dhcp_dns_status_t s2h_dhcp_dns = {0};

#endif

#if H_WIFI_ENTERPRISE_SUPPORT
unsigned char *g_ca_cert = NULL;
int g_ca_cert_len = 0;
unsigned char *g_client_cert = NULL;
int g_client_cert_len = 0;
unsigned char *g_private_key = NULL;
int g_private_key_len = 0;
unsigned char *g_private_key_password = NULL;
int g_private_key_passwd_len = 0;
#endif

static esp_ota_handle_t handle;
const esp_partition_t* update_partition = NULL;
static int ota_msg = 0;

extern esp_err_t wlan_sta_rx_callback(void *buffer, uint16_t len, void *eb);
extern esp_err_t wlan_ap_rx_callback(void *buffer, uint16_t len, void *eb);

extern volatile uint8_t station_connected;
extern volatile uint8_t softap_started;
static volatile bool station_connecting = false;
static volatile bool wifi_initialized = false;

static void send_wifi_event_data_to_host(int event, void *event_data, int event_size)
{
	send_event_data_to_host(event, event_data, event_size);
}

#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
/* DHCP/DNS event handling */
void send_dhcp_dns_info_to_host(uint8_t network_up, uint8_t send_wifi_connected)
{
	rpc_dhcp_dns_status_t s2h_dhcp_dns_DOWN = {0};
	rpc_dhcp_dns_status_t *evnt_to_send = &s2h_dhcp_dns_DOWN;

	if (network_up && s2h_dhcp_dns.dhcp_up && s2h_dhcp_dns.net_link_up && s2h_dhcp_dns.dns_up) {
		evnt_to_send = &s2h_dhcp_dns;
	}
	send_event_data_to_host(RPC_ID__Event_DhcpDnsStatus,
			evnt_to_send, sizeof(rpc_dhcp_dns_status_t));

	vTaskDelay(pdMS_TO_TICKS(10));
	if (send_wifi_connected && station_connected) {
		send_wifi_event_data_to_host(RPC_ID__Event_StaConnected,
				 &lkg_sta_connected_event, sizeof(wifi_event_sta_connected_t));
	}

	ESP_EARLY_LOGI(TAG, "Send DHCP-DNS status to Host: IP: %s, NM: %s, GW: %s, DNS IP: %s, Type: %"PRId32,
			(char *)evnt_to_send->dhcp_ip,
			(char *)evnt_to_send->dhcp_nm,
			(char *)evnt_to_send->dhcp_gw,
			(char *)evnt_to_send->dns_ip,
			evnt_to_send->dns_type);
}

/* Get DNS information */
esp_err_t get_slave_dns(wifi_interface_t iface, esp_netif_dns_info_t *dns)
{
	esp_netif_t *netif = NULL;

	if (!dns) {
		return ESP_ERR_INVALID_ARG;
	}

	if (iface == WIFI_IF_STA) {
		netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
	} else if (iface == WIFI_IF_AP) {
		netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
	}

	if (!netif) {
		return ESP_FAIL;
	}

	return esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, dns);
}

/* Set DNS information */
esp_err_t set_slave_dns(wifi_interface_t iface, char *ip, uint8_t type)
{
	esp_netif_t *netif = NULL;
	esp_netif_dns_info_t dns = {0};

	if (!ip) {
		return ESP_ERR_INVALID_ARG;
	}

	if (iface == WIFI_IF_STA) {
		netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
	} else if (iface == WIFI_IF_AP) {
		netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
	}

	if (!netif) {
		return ESP_FAIL;
	}

	/* Convert IP string to IP address */
	if (esp_netif_str_to_ip4(ip, &dns.ip.u_addr.ip4) != ESP_OK) {
		return ESP_FAIL;
	}

	dns.ip.type = type;
	return esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns);
}

/* Get static IP information */
esp_err_t get_slave_static_ip(wifi_interface_t iface, esp_netif_ip_info_t *ip_info, uint8_t *netlink_up)
{
	esp_netif_t *netif = NULL;

	if (!ip_info || !netlink_up) {
		return ESP_ERR_INVALID_ARG;
	}

	if (iface == WIFI_IF_STA) {
		netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
	} else if (iface == WIFI_IF_AP) {
		netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
	}

	if (!netif) {
		return ESP_FAIL;
	}

	/* Get network interface status */
	*netlink_up = esp_netif_is_netif_up(netif);

	/* Get IP info only if interface is up */
	if (*netlink_up) {
		return esp_netif_get_ip_info(netif, ip_info);
	}

	/* Clear IP info if interface is down */
	memset(ip_info, 0, sizeof(esp_netif_ip_info_t));
	return ESP_OK;
}
#endif /* CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED */


/* OTA end timer callback */
void vTimerCallback( TimerHandle_t xTimer )
{
	xTimerDelete(xTimer, 0);
	esp_unregister_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
	esp_restart();
}

#if H_WIFI_ENTERPRISE_SUPPORT
static void free_all_g_eap_cert_and_key(void)
{
	CLEAR_CERT(g_client_cert, g_client_cert_len);
	CLEAR_CERT(g_private_key, g_private_key_len);
	CLEAR_CERT(g_private_key_password, g_private_key_passwd_len);
}

static void free_g_ca_cert(void)
{
	CLEAR_CERT(g_ca_cert, g_ca_cert_len);
}
#endif

/* Function returns mac address of station/softap */
static esp_err_t req_wifi_get_mac(Rpc *req,
		Rpc *resp, void *priv_data)
{
	uint8_t mac[BSSID_BYTES_SIZE] = {0};

	RPC_TEMPLATE_SIMPLE(RpcRespGetMacAddress, resp_get_mac_address,
			RpcReqGetMacAddress, req_get_mac_address,
			rpc__resp__get_mac_address__init);

	RPC_RET_FAIL_IF(esp_wifi_get_mac(req->req_get_mac_address->mode, mac));

	ESP_LOGI(TAG,"mac [" MACSTR "]", MAC2STR(mac));

	RPC_RESP_COPY_BYTES_SRC_UNCHECKED(resp_payload->mac, mac, BSSID_BYTES_SIZE);

	ESP_LOGD(TAG, "resp mac [" MACSTR "]", MAC2STR(resp_payload->mac.data));

	return ESP_OK;
}

/* Function returns wifi mode */
static esp_err_t req_wifi_get_mode(Rpc *req,
		Rpc *resp, void *priv_data)
{
	wifi_mode_t mode = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespGetMode, resp_get_wifi_mode,
			RpcReqGetMode, req_get_wifi_mode,
			rpc__resp__get_mode__init);

	RPC_RET_FAIL_IF(esp_wifi_get_mode(&mode));

	resp_payload->mode = mode;

	return ESP_OK;
}

/* Function sets wifi mode */
static esp_err_t req_wifi_set_mode(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespSetMode, resp_set_wifi_mode,
			RpcReqSetMode, req_set_wifi_mode,
			rpc__resp__set_mode__init);

	RPC_RET_FAIL_IF(esp_wifi_set_mode(req_payload->mode));

	return ESP_OK;
}

/* Function sets MAC address for station/softap */
static esp_err_t req_wifi_set_mac(Rpc *req,
		Rpc *resp, void *priv_data)

{
	RPC_TEMPLATE(RpcRespSetMacAddress, resp_set_mac_address,
			RpcReqSetMacAddress, req_set_mac_address,
			rpc__resp__set_mac_address__init);

	if (!req_payload->mac.data || (req_payload->mac.len != BSSID_BYTES_SIZE)) {
		ESP_LOGE(TAG, "Invalid MAC address data or len: %d", req_payload->mac.len);
		resp_payload->resp = ESP_ERR_INVALID_ARG;
		return ESP_OK;
	}

	RPC_RET_FAIL_IF(esp_wifi_set_mac(req_payload->mode, req_payload->mac.data));

	return ESP_OK;
}

/* Function sets power save mode */
static esp_err_t req_wifi_set_ps(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespSetPs, resp_wifi_set_ps,
			RpcReqSetPs, req_wifi_set_ps,
			rpc__resp__set_ps__init);

	RPC_RET_FAIL_IF(esp_wifi_set_ps(req_payload->type));

	return ESP_OK;
}

/* Function returns current power save mode */
static esp_err_t req_wifi_get_ps(Rpc *req,
		Rpc *resp, void *priv_data)
{
	wifi_ps_type_t ps_type = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespGetPs, resp_wifi_get_ps,
			RpcReqGetPs, req_wifi_get_ps,
			rpc__resp__get_ps__init);
	RPC_RET_FAIL_IF(esp_wifi_get_ps(&ps_type));
	resp_payload->type = ps_type;
	return ESP_OK;
}

/* Function OTA begin */
static esp_err_t req_ota_begin_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespOTABegin *resp_payload = NULL;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}

	ESP_LOGI(TAG, "OTA update started");

	resp_payload = (RpcRespOTABegin *)
		calloc(1,sizeof(RpcRespOTABegin));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__resp__otabegin__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_OTA_BEGIN;
	resp->resp_ota_begin = resp_payload;

	/* Identify next OTA partition */
	update_partition = esp_ota_get_next_update_partition(NULL);
	if (update_partition == NULL) {
		ESP_LOGE(TAG, "Failed to get next update partition");
		ret = -1;
		goto err;
	}

	ESP_LOGI(TAG, "Prepare partition for OTA\n");
	ret = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &handle);
	if (ret) {
		ESP_LOGE(TAG, "OTA begin failed[%d]", ret);
		goto err;
	}

	ota_msg = 1;

	resp_payload->resp = SUCCESS;
	return ESP_OK;
err:
	resp_payload->resp = ret;
	return ESP_OK;

}

/* Function OTA write */
static esp_err_t req_ota_write_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespOTAWrite *resp_payload = NULL;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}

	resp_payload = (RpcRespOTAWrite *)calloc(1,sizeof(RpcRespOTAWrite));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}

	if (ota_msg) {
		ESP_LOGI(TAG, "Flashing image\n");
		ota_msg = 0;
	}
	rpc__resp__otawrite__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_OTA_WRITE;
	resp->resp_ota_write = resp_payload;

	ret = esp_ota_write( handle, (const void *)req->req_ota_write->ota_data.data,
			req->req_ota_write->ota_data.len);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "OTA write failed with return code 0x%x",ret);
		resp_payload->resp = ret;
		return ESP_OK;
	}
	resp_payload->resp = SUCCESS;
	return ESP_OK;
}

/* Function OTA end */
static esp_err_t req_ota_end_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespOTAEnd *resp_payload = NULL;
	TimerHandle_t xTimer = NULL;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}

	resp_payload = (RpcRespOTAEnd *)calloc(1,sizeof(RpcRespOTAEnd));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__resp__otaend__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_OTA_END;
	resp->resp_ota_end = resp_payload;

	ret = esp_ota_end(handle);
	if (ret != ESP_OK) {
		if (ret == ESP_ERR_OTA_VALIDATE_FAILED) {
			ESP_LOGE(TAG, "Image validation failed, image is corrupted");
		} else {
			ESP_LOGE(TAG, "OTA update failed in end (%s)!", esp_err_to_name(ret));
		}
		goto err;
	}

	/* set OTA partition for next boot */
	ret = esp_ota_set_boot_partition(update_partition);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(ret));
		goto err;
	}
	xTimer = xTimerCreate("Timer", RESTART_TIMEOUT , pdFALSE, 0, vTimerCallback);
	if (xTimer == NULL) {
		ESP_LOGE(TAG, "Failed to create timer to restart system");
		ret = -1;
		goto err;
	}
	ret = xTimerStart(xTimer, 0);
	if (ret != pdPASS) {
		ESP_LOGE(TAG, "Failed to start timer to restart system");
		ret = -2;
		goto err;
	}
	ESP_LOGE(TAG, "**** OTA updated successful, ESP32 will reboot in 5 sec ****");
	resp_payload->resp = SUCCESS;
	return ESP_OK;
err:
	resp_payload->resp = ret;
	return ESP_OK;
}

#if 0
/* Function vendor specific ie */
static esp_err_t req_set_softap_vender_specific_ie_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespSetSoftAPVendorSpecificIE *resp_payload = NULL;
	RpcReqSetSoftAPVendorSpecificIE *p_vsi = req->req_set_softap_vendor_specific_ie;
	RpcReqVendorIEData *p_vid = NULL;
	vendor_ie_data_t *v_data = NULL;

	if (!req || !resp || !p_vsi) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}
	p_vid = p_vsi->vendor_ie_data;

	if (!p_vsi->enable) {

		ESP_LOGI(TAG,"Disable softap vendor IE\n");

	} else {

		ESP_LOGI(TAG,"Enable softap vendor IE\n");

		if (!p_vid ||
			!p_vid->payload.len ||
			!p_vid->payload.data) {
			ESP_LOGE(TAG, "Invalid parameters");
			return ESP_FAIL;
		}

		v_data = (vendor_ie_data_t*)calloc(1,sizeof(vendor_ie_data_t)+p_vid->payload.len);
		if (!v_data) {
			ESP_LOGE(TAG, "Malloc failed at %s:%u\n", __func__, __LINE__);
			return ESP_FAIL;
		}

		v_data->length = p_vid->length;
		v_data->element_id = p_vid->element_id;
		v_data->vendor_oui_type = p_vid->vendor_oui_type;

		memcpy(v_data->vendor_oui, p_vid->vendor_oui.data, VENDOR_OUI_BUF);

		if (p_vid->payload.len && p_vid->payload.data) {
			memcpy(v_data->payload, p_vid->payload.data, p_vid->payload.len);
		}
	}


	resp_payload = (RpcRespSetSoftAPVendorSpecificIE *)
		calloc(1,sizeof(RpcRespSetSoftAPVendorSpecificIE));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		if (v_data)
			mem_free(v_data);
		return ESP_ERR_NO_MEM;
	}

	rpc__resp__set_soft_apvendor_specific_ie__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_SET_SOFTAP_VENDOR_SPECIFIC_IE;
	resp->resp_set_softap_vendor_specific_ie = resp_payload;


	ret = esp_wifi_set_vendor_ie(p_vsi->enable,
			p_vsi->type,
			p_vsi->idx,
			v_data);

	if (v_data)
		mem_free(v_data);

	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to set vendor information element %d \n", ret);
		resp_payload->resp = FAILURE;
		return ESP_OK;
	}
	resp_payload->resp = SUCCESS;
	return ESP_OK;
}
#endif

/* Function set wifi maximum TX power */
static esp_err_t req_wifi_set_max_tx_power(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetMaxTxPower, resp_set_wifi_max_tx_power,
			RpcReqWifiSetMaxTxPower, req_set_wifi_max_tx_power,
			rpc__resp__wifi_set_max_tx_power__init);
	RPC_RET_FAIL_IF(esp_wifi_set_max_tx_power(req_payload->power));
	return ESP_OK;
}

/* Function get wifi TX current power */
static esp_err_t req_wifi_get_max_tx_power(Rpc *req,
		Rpc *resp, void *priv_data)
{
	int8_t power = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetMaxTxPower, resp_get_wifi_max_tx_power,
			RpcReqWifiGetMaxTxPower, req_get_wifi_max_tx_power,
			rpc__resp__wifi_get_max_tx_power__init);
	RPC_RET_FAIL_IF(esp_wifi_get_max_tx_power(&power));
	resp_payload->power = power;
	return ESP_OK;
}

static void heartbeat_timer_cb(TimerHandle_t xTimer)
{
	send_event_to_host(RPC_ID__Event_Heartbeat);
	hb_num++;
}

static void stop_heartbeat(void)
{
	if (handle_heartbeat_task &&
		xTimerIsTimerActive(handle_heartbeat_task)) {
		ESP_LOGI(TAG, "Stopping HB timer");
		xTimerStop(handle_heartbeat_task, portMAX_DELAY);
		xTimerDelete(handle_heartbeat_task, portMAX_DELAY);
		handle_heartbeat_task = NULL;
	}
	hb_num = 0;
}

static esp_err_t start_heartbeat(int duration)
{
	esp_err_t ret = ESP_OK;

	handle_heartbeat_task = xTimerCreate("HB_Timer",
			duration*TIMEOUT_IN_SEC, pdTRUE, 0, heartbeat_timer_cb);
	if (handle_heartbeat_task == NULL) {
		ESP_LOGE(TAG, "Failed to Heartbeat");
		return ESP_FAIL;
	}

	ret = xTimerStart(handle_heartbeat_task, 0);
	if (ret != pdPASS) {
		ESP_LOGE(TAG, "Failed to start Heartbeat");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "HB timer started for %u sec\n", duration);

	return ESP_OK;
}

static esp_err_t configure_heartbeat(bool enable, int hb_duration)
{
	esp_err_t ret = ESP_OK;
	int duration = hb_duration ;

	if (!enable) {
		ESP_LOGI(TAG, "Stop Heatbeat");
		stop_heartbeat();

	} else {
		if (duration < MIN_HEARTBEAT_INTERVAL)
			duration = MIN_HEARTBEAT_INTERVAL;
		if (duration > MAX_HEARTBEAT_INTERVAL)
			duration = MAX_HEARTBEAT_INTERVAL;

		stop_heartbeat();

		ret = start_heartbeat(duration);
	}

	return ret;
}

/* Function to config heartbeat */
static esp_err_t req_config_heartbeat(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespConfigHeartbeat,
			resp_config_heartbeat,
			RpcReqConfigHeartbeat,
			req_config_heartbeat,
			rpc__resp__config_heartbeat__init);

	RPC_RET_FAIL_IF(configure_heartbeat(req_payload->enable, req_payload->duration));

	return ESP_OK;
}



#if CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
static void event_handler_ip(void* arg, esp_event_base_t event_base,
			int32_t event_id, void* event_data)
{
	char ip_s[16] = {0};
	char nm_s[16] = {0};
	char gw_s[16] = {0};
	char dns_ip_s[16] = {0};

	if (event_base == IP_EVENT) {
		switch (event_id) {

		case IP_EVENT_STA_GOT_IP: {
			ESP_LOGI(TAG, "Got IP");
			ip_event_got_ip_t* event = event_data;
			esp_netif_t *netif = event->esp_netif;
			esp_netif_dns_info_t dns = {0};

			//memcpy(&lkg_sta_got_ip_event, event_data, sizeof(ip_event_got_ip_t));
			ESP_ERROR_CHECK(esp_wifi_internal_set_sta_ip());
			ESP_ERROR_CHECK(esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns));

			esp_ip4addr_ntoa(&event->ip_info.ip, ip_s, sizeof(ip_s));
			esp_ip4addr_ntoa(&event->ip_info.netmask, nm_s, sizeof(nm_s));
			esp_ip4addr_ntoa(&event->ip_info.gw, gw_s, sizeof(gw_s));
			esp_ip4addr_ntoa(&dns.ip.u_addr.ip4, dns_ip_s, sizeof(dns_ip_s));

			ESP_LOGI(TAG, "Slave sta dhcp {IP[%s] NM[%s] GW[%s]} dns{type[%u] ip[%s]}",
					ip_s, nm_s, gw_s, dns.ip.type, dns_ip_s);

			s2h_dhcp_dns.net_link_up = 1;
			s2h_dhcp_dns.dhcp_up     = 1;
			s2h_dhcp_dns.dns_up      = 1;
			strlcpy((char*)s2h_dhcp_dns.dhcp_ip, ip_s, sizeof(s2h_dhcp_dns.dhcp_ip));
			strlcpy((char*)s2h_dhcp_dns.dhcp_nm, nm_s, sizeof(s2h_dhcp_dns.dhcp_nm));
			strlcpy((char*)s2h_dhcp_dns.dhcp_gw, gw_s, sizeof(s2h_dhcp_dns.dhcp_gw));
			strlcpy((char*)s2h_dhcp_dns.dns_ip, dns_ip_s, sizeof(s2h_dhcp_dns.dns_ip));
			s2h_dhcp_dns.dns_type = ESP_NETIF_DNS_MAIN;


			//send_dhcp_dns_info_to_host(1, 0);
			station_got_ip = 1;
#ifdef CONFIG_ESP_HOSTED_COPROCESSOR_EXAMPLE_MQTT
			example_mqtt_resume();
#endif
			break;
		} case IP_EVENT_STA_LOST_IP: {
#ifdef CONFIG_ESP_HOSTED_COPROCESSOR_EXAMPLE_MQTT
			example_mqtt_pause();
#endif
			ESP_LOGI(TAG, "Lost IP address");
			station_got_ip = 0;
			memset(&s2h_dhcp_dns, 0, sizeof(s2h_dhcp_dns));
			//send_dhcp_dns_info_to_host(0, 0);
			break;
		}

		}
	}
}


extern esp_netif_t *slave_sta_netif;

static esp_err_t set_slave_static_ip(wifi_interface_t iface, char *ip, char *nm, char *gw)
{

	esp_netif_ip_info_t ip_info = {0};

	ESP_RETURN_ON_FALSE(iface == WIFI_IF_STA, ESP_FAIL, TAG, "only sta iface supported yet");

	ip_info.ip.addr = ipaddr_addr(ip);
	ip_info.netmask.addr = ipaddr_addr(nm);
	ip_info.gw.addr = ipaddr_addr(gw);

	ESP_LOGI(TAG, "Set static IP addr ip:%s nm:%s gw:%s", ip, nm, gw);
	ESP_ERROR_CHECK(esp_netif_set_ip_info(slave_sta_netif, &ip_info));
	esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, (wifi_rxcb_t) wlan_sta_rx_callback);

	return ESP_OK;
}
#endif

#include "esp_wifi_netif.h"
static void event_handler_wifi(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT) {
		if (event_id == WIFI_EVENT_AP_STACONNECTED) {
			wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
			ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
					MAC2STR(event->mac), event->aid);
			send_event_data_to_host(RPC_ID__Event_AP_StaConnected,
					event_data, sizeof(wifi_event_ap_staconnected_t));
		} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
			wifi_event_ap_stadisconnected_t *event =
				(wifi_event_ap_stadisconnected_t *) event_data;
			ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
					MAC2STR(event->mac), event->aid);
			send_event_data_to_host(RPC_ID__Event_AP_StaDisconnected,
					event_data, sizeof(wifi_event_ap_stadisconnected_t));
		} else if (event_id == WIFI_EVENT_SCAN_DONE) {
			ESP_LOGI(TAG, "Wi-Fi sta scan done");
			// rpc event receiver expects Scan Done to have this ID
			send_event_data_to_host(RPC_ID__Event_StaScanDone,
					event_data, sizeof(wifi_event_sta_scan_done_t));
		} else if (event_id == WIFI_EVENT_STA_CONNECTED) {
			ESP_LOGI(TAG, "Sta mode connected");
			if (new_config_recvd) {
				ESP_LOGI(TAG, "New wifi config still unapplied, applying it");
				/* Still not applied new config, so apply it */
				int ret = esp_wifi_set_config(WIFI_IF_STA, &new_wifi_config);
				if (ret) {
					ESP_LOGE(TAG, "Error[0x%x] while setting the wifi config", ret);
				} else {
					new_config_recvd = 0;
				}
				esp_wifi_disconnect();
				return;
			}
			station_connecting = false;
			send_event_data_to_host(RPC_ID__Event_StaConnected,
				event_data, sizeof(wifi_event_sta_connected_t));
			memcpy(&lkg_sta_connected_event, event_data, sizeof(wifi_event_sta_connected_t));
			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, (wifi_rxcb_t) wlan_sta_rx_callback);
			station_connected = true;
		} else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
			station_connected = false;
			if (new_config_recvd) {
				ESP_LOGI(TAG, "New wifi config still unapplied, applying it");
				/* Still not applied new config, so apply it */
				int ret = esp_wifi_set_config(WIFI_IF_STA, &new_wifi_config);
				if (ret) {
					ESP_LOGE(TAG, "Error[0x%x] while setting the wifi config", ret);
				} else {
					new_config_recvd = 0;
				}
			}
			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
			ESP_LOGI(TAG, "Sta mode disconnect");
			station_connecting = false;
			send_event_data_to_host(RPC_ID__Event_StaDisconnected,
				event_data, sizeof(wifi_event_sta_disconnected_t));
#if CONFIG_SOC_WIFI_HE_SUPPORT
		} else if (event_id == WIFI_EVENT_ITWT_SETUP) {
			ESP_LOGI(TAG, "Itwt Setup");
			send_event_data_to_host(RPC_ID__Event_StaItwtSetup,
				event_data, sizeof(wifi_event_sta_itwt_setup_t));
		} else if (event_id == WIFI_EVENT_ITWT_TEARDOWN) {
			ESP_LOGI(TAG, "Itwt Teardown");
			send_event_data_to_host(RPC_ID__Event_StaItwtTeardown,
				event_data, sizeof(wifi_event_sta_itwt_teardown_t));
		} else if (event_id == WIFI_EVENT_ITWT_SUSPEND) {
			ESP_LOGI(TAG, "Itwt Suspend");
			send_event_data_to_host(RPC_ID__Event_StaItwtSuspend,
				event_data, sizeof(wifi_event_sta_itwt_suspend_t));
		} else if (event_id == WIFI_EVENT_ITWT_PROBE) {
			ESP_LOGI(TAG, "Itwt Probe");
			send_event_data_to_host(RPC_ID__Event_StaItwtProbe,
				event_data, sizeof(wifi_event_sta_itwt_probe_t));
#endif
#if H_WIFI_DPP_SUPPORT
		} else if (event_id == WIFI_EVENT_DPP_URI_READY) {
			ESP_LOGW(TAG, "DPP URI Ready");
			wifi_event_dpp_uri_ready_t *uri = (wifi_event_dpp_uri_ready_t *)event_data;
			int uri_len = uri->uri_data_len;
			// include the length of the uri when sending event
			send_event_data_to_host(RPC_ID__Event_WifiDppUriReady,
				event_data, sizeof(wifi_event_dpp_uri_ready_t) + uri_len);
		} else if (event_id == WIFI_EVENT_DPP_CFG_RECVD) {
			ESP_LOGI(TAG, "DPP CFG Ready");
			send_event_data_to_host(RPC_ID__Event_WifiDppCfgRecvd,
				event_data, sizeof(wifi_event_dpp_config_received_t));
		} else if (event_id == WIFI_EVENT_DPP_FAILED) {
			ESP_LOGI(TAG, "DPP Failed");
			send_event_data_to_host(RPC_ID__Event_WifiDppFail,
				event_data, sizeof(wifi_event_dpp_failed_t));
#endif
		} else {
			// ensure start events are only sent once during a state change
			if (event_id == WIFI_EVENT_AP_START) {
				if (!softap_started) {
					ESP_LOGI(TAG,"softap started");
					esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, (wifi_rxcb_t) wlan_ap_rx_callback);
					softap_started = 1;
					send_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
							&event_id, sizeof(event_id));
				}
			} else if (event_id == WIFI_EVENT_AP_STOP) {
				if (softap_started) {
					ESP_LOGI(TAG,"softap stopped");
					esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL);
					softap_started = 0;
					send_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
							&event_id, sizeof(event_id));
				}
			} else if (event_id == WIFI_EVENT_STA_START) {
				if (!station_connecting) {
					ESP_LOGI(TAG, "sta started");
					station_connecting = true;
					esp_wifi_connect();
					send_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
							&event_id, sizeof(event_id));
				}
			} else if (event_id == WIFI_EVENT_STA_STOP) {
				ESP_LOGI(TAG, "sta stopped");
				station_connecting = false;
				send_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
						&event_id, sizeof(event_id));
			} else {
				send_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
						&event_id, sizeof(event_id));
			}
		}
	}
}

// macros to format output
#define PRINT_HEADER() ESP_LOGI(TAG, "     Wifi Init Param | Default |    Host |  Actual");
#define PRINT_FOOTER() ESP_LOGI(TAG, " End Wifi Init Param |");

// need several ESP_LOGx formats due to different sizes of variables to be printed
// int (PRI16), int32_t (PRI32), bool (PRI16)
#define PRINT_USE_HOST_VALUE(param_str, default, host, final)        \
	ESP_LOGD(TAG, "%20s | %7"PRIu16" | %7"PRIi32" | %7"PRIi16, param_str, default, host, final);
#define PRINT_USE_DEFAULT_VALUE(param_str, default, host, final)     \
	ESP_LOGW(TAG, "%20s | %7"PRIu16" | %7"PRIi32" | %7"PRIi16, param_str, default, host, final);

#define PRINT_USE_HOST_VALUE_BOOL(param_str, default, host, final)		\
	ESP_LOGD(TAG, "%20s | %7"PRIu16" | %7"PRIi16" | %7"PRIi16, param_str, default, host, final);
#define PRINT_USE_DEFAULT_VALUE_BOOL(param_str, default, host, final)	\
	ESP_LOGI(TAG, "%20s | %7"PRIu16" | %7"PRIi16" | %7"PRIi16, param_str, default, host, final);

#define PRINT_HEX64_USE_HOST_VALUE(param_str, default, host, final)		\
	ESP_LOGD(TAG, "%20s | 0x%5"PRIx16" | 0x%5"PRIx64" | 0x%5"PRIx64, param_str, default, host, final);
#define PRINT_HEX64_USE_DEFAULT_VALUE(param_str, default, host, final) \
	ESP_LOGW(TAG, "%20s | %7"PRIx16" | %7"PRIx64" | %7"PRIx64, param_str, default, host, final);

// macros to copy host or default value
#define USE_HOST_VALUE(PARAM_STR, DEFAULT, PARAM) \
  do {                                            \
    dst_config->PARAM = src_config->PARAM;        \
    PRINT_USE_HOST_VALUE(PARAM_STR,               \
        DEFAULT,                                  \
        src_config->PARAM,                        \
        dst_config->PARAM);                       \
  } while(0);

#define USE_HOST_VALUE_BOOL(PARAM_STR, DEFAULT, PARAM) \
  do {                                                 \
    dst_config->PARAM = src_config->PARAM;             \
    PRINT_USE_HOST_VALUE_BOOL(PARAM_STR,               \
        DEFAULT,                                       \
        src_config->PARAM,                             \
        dst_config->PARAM);                            \
  } while(0);

#define USE_DEFAULT_VALUE(PARAM_STR, DEFAULT, PARAM) \
  do {                                               \
    dst_config->PARAM = DEFAULT;                     \
    PRINT_USE_DEFAULT_VALUE(PARAM_STR,               \
        DEFAULT,                                     \
        src_config->PARAM,                           \
        dst_config->PARAM);                          \
  } while(0);

#define USE_DEFAULT_VALUE_BOOL(PARAM_STR, DEFAULT, PARAM) \
  do {                                                    \
    dst_config->PARAM = DEFAULT;                          \
    PRINT_USE_DEFAULT_VALUE_BOOL(PARAM_STR,               \
        DEFAULT,                                          \
        src_config->PARAM,                                \
        dst_config->PARAM);                               \
  } while(0);

/** Returns the merged wifi init config
 * Compares the src config from the host with our Wi-Fi defaults
 * and adjust dst_config as necessary.
 *
 * Also displays the changed configs.
 */
static wifi_init_config_t * get_merged_init_config(wifi_init_config_t *dst_config, WifiInitConfig *src_config)
{
	/* always use value from host, except for
	 * - cache_tx_buf_num
	 * - feature_caps
	 */
	PRINT_HEADER();
	USE_HOST_VALUE("static_rx_buf", CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM, static_rx_buf_num);
	USE_HOST_VALUE("dynamic_rx_buf", CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM, dynamic_rx_buf_num);
	USE_HOST_VALUE("tx_buf_type", CONFIG_ESP_WIFI_TX_BUFFER_TYPE, tx_buf_type);
	USE_HOST_VALUE("static_tx_buf", WIFI_STATIC_TX_BUFFER_NUM, static_tx_buf_num);
	USE_HOST_VALUE("dynamic_tx_buf", WIFI_DYNAMIC_TX_BUFFER_NUM, dynamic_tx_buf_num);
	USE_HOST_VALUE("rx_mgmt_buf_type", CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF, rx_mgmt_buf_type);
	USE_HOST_VALUE("rx_mgmt_buf", WIFI_RX_MGMT_BUF_NUM_DEF, rx_mgmt_buf_num);

	if (WIFI_ENABLE_CACHE_TX_BUFFER) {
		// use setting from host
		USE_HOST_VALUE("cache_tx_buf", WIFI_CACHE_TX_BUFFER_NUM, cache_tx_buf_num);
		dst_config->feature_caps = src_config->feature_caps;
		PRINT_HEX64_USE_HOST_VALUE("feature_caps", WIFI_FEATURE_CAPS,
				src_config->feature_caps,
				dst_config->feature_caps);
	} else {
		if (WIFI_FEATURE_CAPS != src_config->feature_caps) {
			// don't use host setting, which may have enabled CACHE_TX_BUFFER
			USE_DEFAULT_VALUE("cache_tx_buf", WIFI_CACHE_TX_BUFFER_NUM, cache_tx_buf_num);
			dst_config->feature_caps = WIFI_FEATURE_CAPS;
			PRINT_HEX64_USE_DEFAULT_VALUE("feature_caps", WIFI_FEATURE_CAPS,
					src_config->feature_caps,
					dst_config->feature_caps);
		} else {
			USE_HOST_VALUE("cache_tx_buf", WIFI_CACHE_TX_BUFFER_NUM, cache_tx_buf_num);
			dst_config->feature_caps = src_config->feature_caps;
			PRINT_HEX64_USE_HOST_VALUE("feature_caps", WIFI_FEATURE_CAPS,
					src_config->feature_caps,
					dst_config->feature_caps);
		}
	}

	USE_HOST_VALUE("csi_enable", WIFI_CSI_ENABLED, csi_enable);
	USE_HOST_VALUE("ampdu_rx_enable", WIFI_AMPDU_RX_ENABLED, ampdu_rx_enable);
	USE_HOST_VALUE("ampdu_tx_enable", WIFI_AMPDU_TX_ENABLED, ampdu_tx_enable);
	USE_HOST_VALUE("amsdu_tx_enable", WIFI_AMSDU_TX_ENABLED, amsdu_tx_enable);
	USE_HOST_VALUE("nvs_enable", WIFI_NVS_ENABLED, nvs_enable);
	USE_HOST_VALUE("nano_enable", WIFI_NANO_FORMAT_ENABLED, nano_enable);
	USE_HOST_VALUE("rx_ba_win", WIFI_DEFAULT_RX_BA_WIN, rx_ba_win);
	USE_HOST_VALUE("wifi_task_core", WIFI_TASK_CORE_ID, wifi_task_core_id);
	USE_HOST_VALUE("beacon_max_len", WIFI_SOFTAP_BEACON_MAX_LEN, beacon_max_len);
	USE_HOST_VALUE("mgmt_sbuf_num", WIFI_MGMT_SBUF_NUM, mgmt_sbuf_num);
	USE_HOST_VALUE_BOOL("sta_disconnected_pm", WIFI_STA_DISCONNECTED_PM_ENABLED, sta_disconnected_pm);
	USE_HOST_VALUE("espnow_max_encrypt",CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM, espnow_max_encrypt_num);
	USE_HOST_VALUE("tx_hetb_queue", WIFI_TX_HETB_QUEUE_NUM, tx_hetb_queue_num);
	USE_HOST_VALUE("dump_hesigb_enable", WIFI_DUMP_HESIGB_ENABLED, dump_hesigb_enable);
	PRINT_FOOTER();

	dst_config->magic = src_config->magic;

	return dst_config;
}

esp_err_t esp_hosted_register_wifi_event_handlers(void)
{
	int ret1;

	esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &instance_any_id);

	ret1 = esp_event_handler_instance_register(WIFI_EVENT,
				ESP_EVENT_ANY_ID,
				&event_handler_wifi,
				NULL,
				&instance_any_id);

	if (ret1) {
		ESP_LOGW(TAG, "Failed to register WiFi events");
	}
#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
	int ret2, ret3;

	esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &instance_any_id);
	ret2 = esp_event_handler_instance_register(IP_EVENT,
				IP_EVENT_STA_GOT_IP,
				&event_handler_ip,
				NULL,
				&instance_ip);
	esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_LOST_IP, &instance_any_id);
	ret3 = esp_event_handler_instance_register(IP_EVENT,
				IP_EVENT_STA_LOST_IP,
				&event_handler_ip,
				NULL,
				&instance_ip);

	if (ret2 || ret3) {
		ESP_LOGW(TAG, "Failed to register IP events");
	}
#endif
	return ESP_OK;
}

/* Cached WiFi init configuration for comparison when reinitializing */
static wifi_init_config_t cached_wifi_init_config = {0};
static bool has_cached_config = false;

/* External declaration of the real esp_wifi_init function */
extern esp_err_t __real_esp_wifi_init(const wifi_init_config_t *config);

/**
 * @brief Compare only the relevant wifi init config fields that we care about
 *
 * This function compares only the specific fields that are set in req_wifi_init()
 * to determine if we need to reinitialize WiFi
 */
static bool wifi_init_config_changed(const wifi_init_config_t *new_cfg, const wifi_init_config_t *cached_cfg)
{
	if (!new_cfg || !cached_cfg) {
		ESP_LOGI(TAG, "WiFi init config comparison: One of the configs is NULL");
		return true;
	}

	/* Compare only the fields that are explicitly set in req_wifi_init */
	bool changed = false;

	if (new_cfg->static_rx_buf_num != cached_cfg->static_rx_buf_num) {
		ESP_LOGI(TAG, "WiFi init config: static_rx_buf_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->static_rx_buf_num, (unsigned long)new_cfg->static_rx_buf_num);
		changed = true;
	}

	if (new_cfg->dynamic_rx_buf_num != cached_cfg->dynamic_rx_buf_num) {
		ESP_LOGI(TAG, "WiFi init config: dynamic_rx_buf_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->dynamic_rx_buf_num, (unsigned long)new_cfg->dynamic_rx_buf_num);
		changed = true;
	}

	if (new_cfg->tx_buf_type != cached_cfg->tx_buf_type) {
		ESP_LOGI(TAG, "WiFi init config: tx_buf_type changed: %lu -> %lu",
				(unsigned long)cached_cfg->tx_buf_type, (unsigned long)new_cfg->tx_buf_type);
		changed = true;
	}

	if (new_cfg->static_tx_buf_num != cached_cfg->static_tx_buf_num) {
		ESP_LOGI(TAG, "WiFi init config: static_tx_buf_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->static_tx_buf_num, (unsigned long)new_cfg->static_tx_buf_num);
		changed = true;
	}

	if (new_cfg->dynamic_tx_buf_num != cached_cfg->dynamic_tx_buf_num) {
		ESP_LOGI(TAG, "WiFi init config: dynamic_tx_buf_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->dynamic_tx_buf_num, (unsigned long)new_cfg->dynamic_tx_buf_num);
		changed = true;
	}

	if (new_cfg->cache_tx_buf_num != cached_cfg->cache_tx_buf_num) {
		ESP_LOGI(TAG, "WiFi init config: cache_tx_buf_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->cache_tx_buf_num, (unsigned long)new_cfg->cache_tx_buf_num);
		changed = true;
	}

	if (new_cfg->csi_enable != cached_cfg->csi_enable) {
		ESP_LOGI(TAG, "WiFi init config: csi_enable changed: %lu -> %lu",
				(unsigned long)cached_cfg->csi_enable, (unsigned long)new_cfg->csi_enable);
		changed = true;
	}

	if (new_cfg->ampdu_rx_enable != cached_cfg->ampdu_rx_enable) {
		ESP_LOGI(TAG, "WiFi init config: ampdu_rx_enable changed: %lu -> %lu",
				(unsigned long)cached_cfg->ampdu_rx_enable, (unsigned long)new_cfg->ampdu_rx_enable);
		changed = true;
	}

	if (new_cfg->ampdu_tx_enable != cached_cfg->ampdu_tx_enable) {
		ESP_LOGI(TAG, "WiFi init config: ampdu_tx_enable changed: %lu -> %lu",
				(unsigned long)cached_cfg->ampdu_tx_enable, (unsigned long)new_cfg->ampdu_tx_enable);
		changed = true;
	}

	if (new_cfg->amsdu_tx_enable != cached_cfg->amsdu_tx_enable) {
		ESP_LOGI(TAG, "WiFi init config: amsdu_tx_enable changed: %lu -> %lu",
				(unsigned long)cached_cfg->amsdu_tx_enable, (unsigned long)new_cfg->amsdu_tx_enable);
		changed = true;
	}

	if (new_cfg->nvs_enable != cached_cfg->nvs_enable) {
		ESP_LOGI(TAG, "WiFi init config: nvs_enable changed: %lu -> %lu",
				(unsigned long)cached_cfg->nvs_enable, (unsigned long)new_cfg->nvs_enable);
		changed = true;
	}

	if (new_cfg->nano_enable != cached_cfg->nano_enable) {
		ESP_LOGI(TAG, "WiFi init config: nano_enable changed: %lu -> %lu",
				(unsigned long)cached_cfg->nano_enable, (unsigned long)new_cfg->nano_enable);
		changed = true;
	}

	if (new_cfg->rx_ba_win != cached_cfg->rx_ba_win) {
		ESP_LOGI(TAG, "WiFi init config: rx_ba_win changed: %lu -> %lu",
				(unsigned long)cached_cfg->rx_ba_win, (unsigned long)new_cfg->rx_ba_win);
		changed = true;
	}

	if (new_cfg->wifi_task_core_id != cached_cfg->wifi_task_core_id) {
		ESP_LOGI(TAG, "WiFi init config: wifi_task_core_id changed: %lu -> %lu",
				(unsigned long)cached_cfg->wifi_task_core_id, (unsigned long)new_cfg->wifi_task_core_id);
		changed = true;
	}

	if (new_cfg->beacon_max_len != cached_cfg->beacon_max_len) {
		ESP_LOGI(TAG, "WiFi init config: beacon_max_len changed: %lu -> %lu",
				(unsigned long)cached_cfg->beacon_max_len, (unsigned long)new_cfg->beacon_max_len);
		changed = true;
	}

	if (new_cfg->mgmt_sbuf_num != cached_cfg->mgmt_sbuf_num) {
		ESP_LOGI(TAG, "WiFi init config: mgmt_sbuf_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->mgmt_sbuf_num, (unsigned long)new_cfg->mgmt_sbuf_num);
		changed = true;
	}

	if (new_cfg->feature_caps != cached_cfg->feature_caps) {
		ESP_LOGI(TAG, "WiFi init config: feature_caps changed: %lu -> %lu",
				(unsigned long)cached_cfg->feature_caps, (unsigned long)new_cfg->feature_caps);
		changed = true;
	}

	if (new_cfg->sta_disconnected_pm != cached_cfg->sta_disconnected_pm) {
		ESP_LOGI(TAG, "WiFi init config: sta_disconnected_pm changed: %lu -> %lu",
				(unsigned long)cached_cfg->sta_disconnected_pm, (unsigned long)new_cfg->sta_disconnected_pm);
		changed = true;
	}

	if (new_cfg->espnow_max_encrypt_num != cached_cfg->espnow_max_encrypt_num) {
		ESP_LOGI(TAG, "WiFi init config: espnow_max_encrypt_num changed: %lu -> %lu",
				(unsigned long)cached_cfg->espnow_max_encrypt_num, (unsigned long)new_cfg->espnow_max_encrypt_num);
		changed = true;
	}

	if (new_cfg->magic != cached_cfg->magic) {
		ESP_LOGI(TAG, "WiFi init config: magic changed: %lu -> %lu",
				(unsigned long)cached_cfg->magic, (unsigned long)new_cfg->magic);
		changed = true;
	}

	return changed;
}

/**
 * @brief Wrapper function for esp_wifi_init that caches config and handles reinitialization
 *
 * This function intercepts calls to esp_wifi_init, caches the configuration,
 * and compares with previous config. If config has changed and WiFi is already
 * initialized, it will stop, deinit, and reinitialize with the new parameters.
 */
esp_err_t __wrap_esp_wifi_init(const wifi_init_config_t *config)
{
	esp_err_t ret;
	bool should_reinit = false;
	ESP_LOGI(TAG, "=== __wrap_esp_wifi_init called ===");

	if (wifi_initialized) {
		/* Compare with cached config */
		if (has_cached_config && wifi_init_config_changed(config, &cached_wifi_init_config)) {
			ESP_LOGI(TAG, "WiFi init config changed, reinitializing");
			esp_wifi_stop();
			esp_wifi_deinit();
			wifi_initialized = false;
			should_reinit = true;
		} else {
			ESP_LOGW(TAG, "WiFi already initialized with same parameters");
			return ESP_OK;
		}
	} else {
		ESP_LOGI(TAG, "First-time WiFi initialization");
	}

	/* Cache the config for future comparisons */
	if (config) {
		memcpy(&cached_wifi_init_config, config, sizeof(wifi_init_config_t));
		has_cached_config = true;
	}

	/* Call the real init function */
	ESP_LOGI(TAG, "Calling __real_esp_wifi_init...");
	ret = __real_esp_wifi_init(config);
	ESP_LOGI(TAG, "__real_esp_wifi_init returned: %d", ret);

	if (ret == ESP_OK && !should_reinit) {
		wifi_initialized = true;
	}

	return ret;
}

static esp_err_t req_wifi_init(Rpc *req, Rpc *resp, void *priv_data)
{
	int ret = 0;

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	RPC_TEMPLATE(RpcRespWifiInit, resp_wifi_init,
			RpcReqWifiInit, req_wifi_init,
			rpc__resp__wifi_init__init);

	RPC_RET_FAIL_IF(!req_payload->cfg);

	cfg.static_rx_buf_num       = req_payload->cfg->static_rx_buf_num      ;
	cfg.dynamic_rx_buf_num      = req_payload->cfg->dynamic_rx_buf_num     ;
	cfg.tx_buf_type             = req_payload->cfg->tx_buf_type            ;
	cfg.static_tx_buf_num       = req_payload->cfg->static_tx_buf_num      ;
	cfg.dynamic_tx_buf_num      = req_payload->cfg->dynamic_tx_buf_num     ;

#if CONFIG_IDF_TARGET_ESP32C2
	/* Hardcode: No static tx buffers for c2 due to low memory issues */
	if (!cfg.tx_buf_type) {
		cfg.tx_buf_type         = 1;

		if (!cfg.dynamic_tx_buf_num)
			cfg.dynamic_tx_buf_num = 16;
	}
#endif
	cfg.cache_tx_buf_num        = req_payload->cfg->cache_tx_buf_num       ;
	cfg.csi_enable              = req_payload->cfg->csi_enable             ;
	cfg.ampdu_rx_enable         = req_payload->cfg->ampdu_rx_enable        ;
	cfg.ampdu_tx_enable         = req_payload->cfg->ampdu_tx_enable        ;
	cfg.amsdu_tx_enable         = req_payload->cfg->amsdu_tx_enable        ;
	cfg.nvs_enable              = req_payload->cfg->nvs_enable             ;
	cfg.nano_enable             = req_payload->cfg->nano_enable            ;
	cfg.rx_ba_win               = req_payload->cfg->rx_ba_win              ;
	cfg.wifi_task_core_id       = req_payload->cfg->wifi_task_core_id      ;
	cfg.beacon_max_len          = req_payload->cfg->beacon_max_len         ;
	cfg.mgmt_sbuf_num           = req_payload->cfg->mgmt_sbuf_num          ;
	cfg.feature_caps            = req_payload->cfg->feature_caps           ;
	cfg.sta_disconnected_pm     = req_payload->cfg->sta_disconnected_pm    ;
	cfg.espnow_max_encrypt_num  = req_payload->cfg->espnow_max_encrypt_num ;
	cfg.magic                   = req_payload->cfg->magic                  ;

	ESP_LOGV(TAG, "Wifi-config: static_rx_buf_num[%lu] dynamic_rx_buf_num[%lu] tx_buf_type[%lu]", (unsigned long)cfg.static_rx_buf_num, (unsigned long)cfg.dynamic_rx_buf_num, (unsigned long)cfg.tx_buf_type);
	ESP_LOGV(TAG, "Wifi-config: static_tx_buf_num[%lu] dynamic_tx_buf_num[%lu] cache_tx_buf_num[%lu]", (unsigned long)cfg.static_tx_buf_num, (unsigned long)cfg.dynamic_tx_buf_num, (unsigned long)cfg.cache_tx_buf_num);
	ESP_LOGV(TAG, "Wifi-config: csi_enable[%lu] ampdu_rx_enable[%lu] ampdu_tx_enable[%lu] amsdu_tx_enable[%lu]", (unsigned long)cfg.csi_enable, (unsigned long)cfg.ampdu_rx_enable, (unsigned long)cfg.ampdu_tx_enable, (unsigned long)cfg.amsdu_tx_enable);
	ESP_LOGV(TAG, "Wifi-config: nvs_enable[%lu] nano_enable[%lu] rx_ba_win[%lu] wifi_task_core_id[%lu]", (unsigned long)cfg.nvs_enable, (unsigned long)cfg.nano_enable, (unsigned long)cfg.rx_ba_win, (unsigned long)cfg.wifi_task_core_id);
	ESP_LOGV(TAG, "Wifi-config: beacon_max_len[%lu] mgmt_sbuf_num[%lu] feature_caps[%lu] sta_disconnected_pm[%lu]", (unsigned long)cfg.beacon_max_len, (unsigned long)cfg.mgmt_sbuf_num, (unsigned long)cfg.feature_caps, (unsigned long)cfg.sta_disconnected_pm);
	ESP_LOGV(TAG, "Wifi-config: espnow_max_encrypt_num[%lu] magic[%lu]", (unsigned long)cfg.espnow_max_encrypt_num, (unsigned long)cfg.magic);

	/* Use our wrapper directly instead of esp_hosted_wifi_init */
	RPC_RET_FAIL_IF(esp_wifi_init(get_merged_init_config(&cfg, req_payload->cfg)));

	esp_hosted_register_wifi_event_handlers();

	return ret;
}

static esp_err_t req_wifi_deinit(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiDeinit, resp_wifi_deinit,
			RpcReqWifiDeinit, req_wifi_deinit,
			rpc__resp__wifi_deinit__init);

#if H_WIFI_ENTERPRISE_SUPPORT
	free_g_ca_cert();
	free_all_g_eap_cert_and_key();
#endif
	RPC_RET_FAIL_IF(esp_wifi_deinit());

	return ESP_OK;
}


static esp_err_t req_wifi_start(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStart, resp_wifi_start,
			RpcReqWifiStart, req_wifi_start,
			rpc__resp__wifi_start__init);

	RPC_RET_FAIL_IF(esp_wifi_start());

	/**
	 * check the current wifi mode and send the STA/AP start event(s)
	 * to handle the case where the host wakes up from deep sleep.
	 * In this case, the wifi was already started on the co-processor
	 * and does not generate the required start events
	 */
	wifi_mode_t mode;
	int event_id;
	esp_err_t res = esp_wifi_get_mode(&mode);
	if (res == ESP_OK) {
		if ((mode == WIFI_MODE_STA) || (mode == WIFI_MODE_APSTA)) {
			ESP_LOGI(TAG, "send WIFI_EVENT_STA_START");
			event_id = WIFI_EVENT_STA_START;
			send_wifi_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
					&event_id, sizeof(event_id));
		}
		if ((mode == WIFI_MODE_AP) || (mode == WIFI_MODE_APSTA)) {
			ESP_LOGI(TAG, "send WIFI_EVENT_AP_START");
			event_id = WIFI_EVENT_AP_START;
			send_wifi_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
					&event_id, sizeof(event_id));
		}
	}

	return ESP_OK;
}

static esp_err_t req_wifi_stop(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStop, resp_wifi_stop,
			RpcReqWifiStop, req_wifi_stop,
			rpc__resp__wifi_stop__init);

	RPC_RET_FAIL_IF(esp_wifi_stop());
	return ESP_OK;
}

static esp_err_t req_wifi_connect(Rpc *req, Rpc *resp, void *priv_data)
{
	int ret = ESP_OK;
	wifi_config_t wifi_cfg = {0};

	RPC_TEMPLATE_SIMPLE(RpcRespWifiConnect, resp_wifi_connect,
			RpcReqWifiConnect, req_wifi_connect,
			rpc__resp__wifi_connect__init);

	// Check if WiFi config has valid SSID before attempting connection
	if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
		ESP_LOGE(TAG, "Failed to get WiFi config");
		resp_payload->resp = ESP_ERR_WIFI_NOT_INIT;
		return ESP_OK;
	}

	if (strlen((char*)wifi_cfg.sta.ssid) == 0) {
		ESP_LOGE(TAG, "No SSID configured, cannot connect");
		resp_payload->resp = ESP_ERR_WIFI_SSID;
		return ESP_OK;
	}

	ESP_LOGI(TAG, "Attempting to connect to SSID: %s", wifi_cfg.sta.ssid);

	if (new_config_recvd || !station_connected) {
		ESP_LOGI(TAG, "************ connect ****************");
		//station_connecting = true;
		ret = esp_wifi_connect();
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "Failed to connect to WiFi: %d", ret);
			station_connecting = false;
		}
	} else {
		ESP_LOGI(TAG, "connect recvd, ack with connected event");


#if CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
		//send_dhcp_dns_info_to_host(1, 0);
#endif

		send_wifi_event_data_to_host(RPC_ID__Event_StaConnected,
				&lkg_sta_connected_event, sizeof(wifi_event_sta_connected_t));
	}

	if (ret != ESP_ERR_WIFI_CONN)
		resp_payload->resp = ret;

	return ESP_OK;
}

static esp_err_t req_wifi_disconnect(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiDisconnect, resp_wifi_disconnect,
			RpcReqWifiDisconnect, req_wifi_disconnect,
			rpc__resp__wifi_disconnect__init);

	RPC_RET_FAIL_IF(esp_wifi_disconnect());

	return ESP_OK;
}

static bool wifi_is_provisioned(wifi_config_t *wifi_cfg)
{
	if (!wifi_cfg) {
		ESP_LOGI(TAG, "NULL wifi cfg passed, ignore");
		return false;
	}

	if (esp_wifi_get_config(WIFI_IF_STA, wifi_cfg) != ESP_OK) {
		ESP_LOGI(TAG, "Wifi get config failed");
		return false;
	}

	ESP_LOGI(TAG, "SSID: %s", wifi_cfg->sta.ssid);

	if (strlen((const char *) wifi_cfg->sta.ssid)) {
		ESP_LOGI(TAG, "Wifi provisioned");
		return true;
	}
	ESP_LOGI(TAG, "Wifi not provisioned, Fallback to example config");

	return false;
}

/* Function to compare two WiFi configurations */
static bool is_wifi_config_equal(const wifi_config_t *cfg1, const wifi_config_t *cfg2)
{
	if (!cfg1 || !cfg2) {
		return false;
	}
	/* Compare SSID */
	if (strcmp((char *)cfg1->sta.ssid, (char *)cfg2->sta.ssid) != 0) {
		ESP_LOGD(TAG, "SSID different: '%s' vs '%s'", cfg1->sta.ssid, cfg2->sta.ssid);
		return false;
	}

	/* Compare password */
	if (strcmp((char *)cfg1->sta.password, (char *)cfg2->sta.password) != 0) {
		ESP_LOGD(TAG, "Password different");
		return false;
	}

	/* Compare BSSID if set */
	if (cfg1->sta.bssid_set && cfg2->sta.bssid_set) {
		if (memcmp(cfg1->sta.bssid, cfg2->sta.bssid, BSSID_BYTES_SIZE) != 0) {
			ESP_LOGD(TAG, "BSSID different");
			return false;
		}
	} else if (cfg1->sta.bssid_set != cfg2->sta.bssid_set) {
		ESP_LOGD(TAG, "BSSID set status different: %d vs %d",
			cfg1->sta.bssid_set, cfg2->sta.bssid_set);
		return false;
	}

	/* Compare channel if set */
	if (cfg1->sta.channel != 0 && cfg2->sta.channel != 0) {
		if (cfg1->sta.channel != cfg2->sta.channel) {
			ESP_LOGD(TAG, "Channel different: %d vs %d",
				cfg1->sta.channel, cfg2->sta.channel);
			return false;
		}
	}

	return true;
}


/* Function to handle WiFi configuration */
esp_err_t esp_hosted_set_sta_config(wifi_interface_t iface, wifi_config_t *cfg)
{
	wifi_config_t current_config = {0};
	if (!wifi_is_provisioned(&current_config)) {
		if (esp_wifi_set_config(WIFI_IF_STA, cfg) != ESP_OK) {
			ESP_LOGW(TAG, "not provisioned and failed to set wifi config");
		} else {
			ESP_LOGI(TAG, "Provisioned new Wi-Fi config");
			new_config_recvd = true;
			station_connecting = false;
		}
	}

	if (!is_wifi_config_equal(cfg, &current_config)) {
		new_config_recvd = true;
		station_connecting = false;
		ESP_LOGI(TAG, "Setting new WiFi config SSID: %s", cfg->sta.ssid);
		memcpy(&new_wifi_config, cfg, sizeof(wifi_config_t));
	} else {
		ESP_LOGI(TAG, "WiFi config unchanged, keeping current connection");
		new_config_recvd = false;
	}

	return ESP_OK;
}

static esp_err_t req_wifi_set_config(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_config_t cfg = {0};

	RPC_TEMPLATE(RpcRespWifiSetConfig, resp_wifi_set_config,
			RpcReqWifiSetConfig, req_wifi_set_config,
			rpc__resp__wifi_set_config__init);

	RPC_RET_FAIL_IF((req_payload->iface != WIFI_IF_STA) &&
			(req_payload->iface != WIFI_IF_AP));

	RPC_RET_FAIL_IF(!req_payload->cfg);

	if (req_payload->iface == WIFI_IF_STA) {

		wifi_sta_config_t * p_a_sta = &(cfg.sta);
		WifiStaConfig * p_c_sta = req_payload->cfg->sta;
		RPC_RET_FAIL_IF(!req_payload->cfg->sta);
		RPC_REQ_COPY_STR(p_a_sta->ssid, p_c_sta->ssid, SSID_LENGTH);
		if (strlen((char*)p_a_sta->ssid))
			ESP_LOGI(TAG, "STA set config: SSID:%s", p_a_sta->ssid);
		RPC_REQ_COPY_STR(p_a_sta->password, p_c_sta->password, PASSWORD_LENGTH);
		if (strlen((char*)p_a_sta->password))
			ESP_LOGD(TAG, "STA: password:xxxxxxxx");
		p_a_sta->scan_method = p_c_sta->scan_method;
		p_a_sta->bssid_set = p_c_sta->bssid_set;

		if (p_a_sta->bssid_set)
			RPC_REQ_COPY_BYTES(p_a_sta->bssid, p_c_sta->bssid, BSSID_BYTES_SIZE);

		p_a_sta->channel = p_c_sta->channel;
		p_a_sta->listen_interval = p_c_sta->listen_interval;
		p_a_sta->sort_method = p_c_sta->sort_method;
		if (p_c_sta->threshold) {
			p_a_sta->threshold.rssi = p_c_sta->threshold->rssi;
			p_a_sta->threshold.authmode = p_c_sta->threshold->authmode;
#if H_PRESENT_IN_ESP_IDF_5_4_0
			p_a_sta->threshold.rssi_5g_adjustment = p_c_sta->threshold->rssi_5g_adjustment;
#endif
		}
		//p_a_sta->ssid_hidden = p_c_sta->ssid_hidden;
		//p_a_sta->max_connections = p_c_sta->max_connections;
		if (p_c_sta->pmf_cfg) {
			p_a_sta->pmf_cfg.capable = p_c_sta->pmf_cfg->capable;
			p_a_sta->pmf_cfg.required = p_c_sta->pmf_cfg->required;
		}
		p_a_sta->rm_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_rm_enabled, p_c_sta->bitmask);
		p_a_sta->btm_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_btm_enabled, p_c_sta->bitmask);
		p_a_sta->mbo_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_mbo_enabled, p_c_sta->bitmask);
		p_a_sta->ft_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_ft_enabled, p_c_sta->bitmask);
		p_a_sta->owe_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_owe_enabled, p_c_sta->bitmask);
		p_a_sta->transition_disable = H_GET_BIT(WIFI_STA_CONFIG_1_transition_disable, p_c_sta->bitmask);
#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
		p_a_sta->reserved1 = WIFI_STA_CONFIG_1_GET_RESERVED_VAL(p_c_sta->bitmask);
#else
		p_a_sta->reserved = WIFI_STA_CONFIG_1_GET_RESERVED_VAL(p_c_sta->bitmask);
#endif
#endif

		p_a_sta->sae_pwe_h2e = p_c_sta->sae_pwe_h2e;
		p_a_sta->sae_pk_mode = p_c_sta->sae_pk_mode;
		p_a_sta->failure_retry_cnt = p_c_sta->failure_retry_cnt;

		p_a_sta->he_dcm_set = H_GET_BIT(WIFI_STA_CONFIG_2_he_dcm_set_BIT, p_c_sta->he_bitmask);
		/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx is two bits wide */
		p_a_sta->he_dcm_max_constellation_tx = (p_c_sta->he_bitmask >> WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx_BITS) & 0x03;
		/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx is two bits wide */
		p_a_sta->he_dcm_max_constellation_rx = (p_c_sta->he_bitmask >> WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx_BITS) & 0x03;

		p_a_sta->he_mcs9_enabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_mcs9_enabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_su_beamformee_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_su_beamformee_disabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_trig_su_bmforming_feedback_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_trig_su_bmforming_feedback_disabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_trig_mu_bmforming_partial_feedback_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_trig_mu_bmforming_partial_feedback_disabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_trig_cqi_feedback_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_trig_cqi_feedback_disabled_BIT, p_c_sta->he_bitmask);

#if H_PRESENT_IN_ESP_IDF_5_5_0
		p_a_sta->vht_su_beamformee_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_vht_su_beamformee_disabled, p_c_sta->he_bitmask);
		p_a_sta->vht_mu_beamformee_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_vht_mu_beamformee_disabled, p_c_sta->he_bitmask);
		p_a_sta->vht_mcs8_enabled = H_GET_BIT(WIFI_STA_CONFIG_2_vht_mcs8_enabled, p_c_sta->he_bitmask);
#endif

#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
		p_a_sta->reserved2 = WIFI_STA_CONFIG_2_GET_RESERVED_VAL(p_c_sta->he_bitmask);
#else
		p_a_sta->he_reserved = WIFI_STA_CONFIG_2_GET_RESERVED_VAL(p_c_sta->he_bitmask);
#endif
#endif

		/* Avoid using fast scan, which leads to faster SSID selection,
		 * but faces data throughput issues when same SSID broadcasted by weaker AP
		 */
		p_a_sta->scan_method = WIFI_ALL_CHANNEL_SCAN;
		p_a_sta->sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

		RPC_REQ_COPY_STR(p_a_sta->sae_h2e_identifier, p_c_sta->sae_h2e_identifier, SAE_H2E_IDENTIFIER_LEN);
		RPC_RET_FAIL_IF(esp_hosted_set_sta_config(req_payload->iface, &cfg));
	} else if (req_payload->iface == WIFI_IF_AP) {
		wifi_ap_config_t * p_a_ap = &(cfg.ap);
		WifiApConfig * p_c_ap = req_payload->cfg->ap;
		RPC_RET_FAIL_IF(!req_payload->cfg->ap);
		/* esp_wifi_types.h says SSID should be NULL terminated if ssid_len is 0 */
		RPC_REQ_COPY_STR(p_a_ap->ssid, p_c_ap->ssid, SSID_LENGTH);
		p_a_ap->ssid_len = p_c_ap->ssid_len;
		RPC_REQ_COPY_STR(p_a_ap->password, p_c_ap->password, PASSWORD_LENGTH);
		p_a_ap->channel = p_c_ap->channel;
		p_a_ap->authmode = p_c_ap->authmode;
		p_a_ap->ssid_hidden = p_c_ap->ssid_hidden;
		p_a_ap->max_connection = p_c_ap->max_connection;
		p_a_ap->beacon_interval = p_c_ap->beacon_interval;
		p_a_ap->csa_count = p_c_ap->csa_count;
		p_a_ap->dtim_period = p_c_ap->dtim_period;
		p_a_ap->pairwise_cipher = p_c_ap->pairwise_cipher;
		p_a_ap->ftm_responder = p_c_ap->ftm_responder;
		if (p_c_ap->pmf_cfg) {
			p_a_ap->pmf_cfg.capable = p_c_ap->pmf_cfg->capable;
			p_a_ap->pmf_cfg.required = p_c_ap->pmf_cfg->required;
		}
		p_a_ap->sae_pwe_h2e = p_c_ap->sae_pwe_h2e;
#if H_GOT_AP_CONFIG_PARAM_TRANSITION_DISABLE
		p_a_ap->transition_disable = p_c_ap->transition_disable;
#endif
#if H_PRESENT_IN_ESP_IDF_5_5_0
		p_a_ap->sae_ext = p_c_ap->sae_ext;
		if (p_c_ap->bss_max_idle_cfg) {
			p_a_ap->bss_max_idle_cfg.period = p_c_ap->bss_max_idle_cfg->period;
			p_a_ap->bss_max_idle_cfg.protected_keep_alive = p_c_ap->bss_max_idle_cfg->protected_keep_alive;
		}
		p_a_ap->gtk_rekey_interval = p_c_ap->gtk_rekey_interval;
#endif

		RPC_RET_FAIL_IF(esp_wifi_set_config(req_payload->iface, &cfg));
	}

	return ESP_OK;
}

typedef enum {
	PAYLOAD_TYPE_RPC_RESP_WIFI_GET_CONFIG,
	PAYLOAD_TYPE_RPC_EVENT_SUPP_DPP_GET_CONFIG,
#if H_WIFI_DPP_SUPPORT
	PAYLOAD_TYPE_RPC_EVENT_WIFI_DPP_GET_CONFIG,
#endif
} rpc_payload_type_t;

/*
 * Copies the station config from ESP-IDF struct to RPC response/event. This is used in:
 * - req_wifi_get_config
 * - rpc_evt_supp
 * - wifi_dpp_cfg_recvd
 *
 * `type` determines the type of payload the station config is copied to
 */
static esp_err_t copy_wifi_sta_cfg_to_rpc_struct(void *payload, rpc_payload_type_t type,
		wifi_sta_config_t *sta_cfg)
{
	wifi_sta_config_t * p_a_sta = sta_cfg;
	WifiStaConfig * p_c_sta = NULL;

	if (!payload) {
		ESP_LOGE(TAG, "%s called with NULL payload", __func__);
		return ESP_FAIL;
	}

	 /** macros used to do allocation and copying depend on
	 ** resp_payload or ntfy_payload being defined and used, so do
	 ** them all here
	 **/
	switch (type) {
	case PAYLOAD_TYPE_RPC_RESP_WIFI_GET_CONFIG: {
		RpcRespWifiGetConfig *resp_payload = (RpcRespWifiGetConfig *)payload;
		RPC_ALLOC_ELEMENT(WifiStaConfig, resp_payload->cfg->sta, wifi_sta_config__init);
		p_c_sta = resp_payload->cfg->sta;
		RPC_RESP_COPY_STR(p_c_sta->ssid, p_a_sta->ssid, SSID_LENGTH);
		RPC_RESP_COPY_STR(p_c_sta->password, p_a_sta->password, PASSWORD_LENGTH);
		RPC_RESP_COPY_BYTES(p_c_sta->bssid, p_a_sta->bssid, BSSID_BYTES_SIZE);
		RPC_ALLOC_ELEMENT(WifiScanThreshold, p_c_sta->threshold, wifi_scan_threshold__init);
		RPC_ALLOC_ELEMENT(WifiPmfConfig, p_c_sta->pmf_cfg, wifi_pmf_config__init);
		break;
		}
	case PAYLOAD_TYPE_RPC_EVENT_SUPP_DPP_GET_CONFIG: {
		RpcEventSuppDppCfgRecvd *ntfy_payload = (RpcEventSuppDppCfgRecvd *)payload;
		NTFY_ALLOC_ELEMENT(WifiStaConfig, ntfy_payload->cfg->sta, wifi_sta_config__init);
		p_c_sta = ntfy_payload->cfg->sta;
		NTFY_COPY_STR(p_c_sta->ssid, p_a_sta->ssid, SSID_LENGTH);
		NTFY_COPY_STR(p_c_sta->password, p_a_sta->password, PASSWORD_LENGTH);
		NTFY_COPY_BYTES(p_c_sta->bssid, p_a_sta->bssid, BSSID_BYTES_SIZE);
		NTFY_ALLOC_ELEMENT(WifiScanThreshold, p_c_sta->threshold, wifi_scan_threshold__init);
		NTFY_ALLOC_ELEMENT(WifiPmfConfig, p_c_sta->pmf_cfg, wifi_pmf_config__init);
		break;
		}
#if H_WIFI_DPP_SUPPORT
	case PAYLOAD_TYPE_RPC_EVENT_WIFI_DPP_GET_CONFIG: {
		RpcEventWifiDppCfgRecvd *ntfy_payload = (RpcEventWifiDppCfgRecvd *)payload;
		NTFY_ALLOC_ELEMENT(WifiStaConfig, ntfy_payload->cfg->sta, wifi_sta_config__init);
		p_c_sta = ntfy_payload->cfg->sta;
		NTFY_COPY_STR(p_c_sta->ssid, p_a_sta->ssid, SSID_LENGTH);
		NTFY_COPY_STR(p_c_sta->password, p_a_sta->password, PASSWORD_LENGTH);
		NTFY_COPY_BYTES(p_c_sta->bssid, p_a_sta->bssid, BSSID_BYTES_SIZE);
		NTFY_ALLOC_ELEMENT(WifiScanThreshold, p_c_sta->threshold, wifi_scan_threshold__init);
		NTFY_ALLOC_ELEMENT(WifiPmfConfig, p_c_sta->pmf_cfg, wifi_pmf_config__init);
		break;
		}
#endif
	}

	if (!p_c_sta) {
		ESP_LOGE(TAG, "%s: p_c_sta still NULL", __func__);
		return ESP_FAIL;
	}

	// generic copying of data done here using only p_c_sta
	p_c_sta->scan_method = p_a_sta->scan_method;
	p_c_sta->bssid_set = p_a_sta->bssid_set;
	p_c_sta->channel = p_a_sta->channel;
	p_c_sta->listen_interval = p_a_sta->listen_interval;
	p_c_sta->sort_method = p_a_sta->sort_method;
	p_c_sta->threshold->rssi = p_a_sta->threshold.rssi;
	p_c_sta->threshold->authmode = p_a_sta->threshold.authmode;
#if H_PRESENT_IN_ESP_IDF_5_4_0
	p_c_sta->threshold->rssi_5g_adjustment = p_a_sta->threshold.rssi_5g_adjustment;
#endif
	p_c_sta->pmf_cfg->capable = p_a_sta->pmf_cfg.capable;
	p_c_sta->pmf_cfg->required = p_a_sta->pmf_cfg.required;

	if (p_a_sta->rm_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_1_rm_enabled, p_c_sta->bitmask);

	if (p_a_sta->btm_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_1_btm_enabled, p_c_sta->bitmask);

	if (p_a_sta->mbo_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_1_mbo_enabled, p_c_sta->bitmask);

	if (p_a_sta->ft_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_1_ft_enabled, p_c_sta->bitmask);

	if (p_a_sta->owe_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_1_owe_enabled, p_c_sta->bitmask);

	if (p_a_sta->transition_disable)
		H_SET_BIT(WIFI_STA_CONFIG_1_transition_disable, p_c_sta->bitmask);

#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
	WIFI_STA_CONFIG_1_SET_RESERVED_VAL(p_a_sta->reserved1, p_c_sta->bitmask);
#else
	WIFI_STA_CONFIG_1_SET_RESERVED_VAL(p_a_sta->reserved, p_c_sta->bitmask);
#endif
#endif

	p_c_sta->sae_pwe_h2e = p_a_sta->sae_pwe_h2e;
	p_c_sta->sae_pk_mode = p_a_sta->sae_pk_mode;
	p_c_sta->failure_retry_cnt = p_a_sta->failure_retry_cnt;

	/* HE field handling */
	if (p_a_sta->he_dcm_set)
		H_SET_BIT(WIFI_STA_CONFIG_2_he_dcm_set_BIT, p_c_sta->he_bitmask);

	/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx is two bits wide */
	if (p_a_sta->he_dcm_max_constellation_tx & 0x03) {
		p_c_sta->he_bitmask |= (p_a_sta->he_dcm_max_constellation_tx & 0x03) << WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx_BITS;
	}
	/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx is two bits wide */
	if (p_a_sta->he_dcm_max_constellation_rx & 0x03) {
		p_c_sta->he_bitmask |= (p_a_sta->he_dcm_max_constellation_rx & 0x03) << WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx_BITS;
	}

	if (p_a_sta->he_mcs9_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_he_mcs9_enabled_BIT, p_c_sta->he_bitmask);

	if (p_a_sta->he_su_beamformee_disabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_he_su_beamformee_disabled_BIT, p_c_sta->he_bitmask);

	if (p_a_sta->he_trig_su_bmforming_feedback_disabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_he_trig_su_bmforming_feedback_disabled_BIT, p_c_sta->he_bitmask);

	if (p_a_sta->he_trig_mu_bmforming_partial_feedback_disabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_he_trig_mu_bmforming_partial_feedback_disabled_BIT, p_c_sta->he_bitmask);

	if (p_a_sta->he_trig_cqi_feedback_disabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_he_trig_cqi_feedback_disabled_BIT, p_c_sta->he_bitmask);

#if H_PRESENT_IN_ESP_IDF_5_5_0
	if (p_a_sta->vht_su_beamformee_disabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_vht_su_beamformee_disabled, p_c_sta->he_bitmask);

	if (p_a_sta->vht_mu_beamformee_disabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_vht_mu_beamformee_disabled, p_c_sta->he_bitmask);

	if (p_a_sta->vht_mcs8_enabled)
		H_SET_BIT(WIFI_STA_CONFIG_2_vht_mcs8_enabled, p_c_sta->he_bitmask);
#endif

#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
	WIFI_STA_CONFIG_2_SET_RESERVED_VAL(p_a_sta->reserved2, p_c_sta->he_bitmask);
#else
	WIFI_STA_CONFIG_2_SET_RESERVED_VAL(p_a_sta->he_reserved, p_c_sta->he_bitmask);
#endif
#endif

err:
	return ESP_OK;
}

static esp_err_t req_wifi_get_config(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_interface_t iface;
	wifi_config_t cfg = {0};

	RPC_TEMPLATE(RpcRespWifiGetConfig, resp_wifi_get_config,
			RpcReqWifiGetConfig, req_wifi_get_config,
			rpc__resp__wifi_get_config__init);

	iface = req_payload->iface;
	resp_payload->iface = iface;
	RPC_RET_FAIL_IF(iface > WIFI_IF_AP);
	RPC_RET_FAIL_IF(esp_wifi_get_config(iface, &cfg));

	RPC_ALLOC_ELEMENT(WifiConfig, resp_payload->cfg, wifi_config__init);
	switch (iface) {

	case WIFI_IF_STA: {
		resp_payload->cfg->u_case = WIFI_CONFIG__U_STA;
		esp_err_t res = copy_wifi_sta_cfg_to_rpc_struct(resp_payload,
				PAYLOAD_TYPE_RPC_RESP_WIFI_GET_CONFIG, &cfg.sta);
		if (res != ESP_OK) {
			ESP_LOGE(TAG, "RPC_RESP: copy_wifi_sta_cfg_to_rpc_struct() FAILED");
			return res;
		}
		break;
	}
	case WIFI_IF_AP: {
		wifi_ap_config_t * p_a_ap = &(cfg.ap);
		resp_payload->cfg->u_case = WIFI_CONFIG__U_AP;

		RPC_ALLOC_ELEMENT(WifiApConfig, resp_payload->cfg->ap, wifi_ap_config__init);
		WifiApConfig * p_c_ap = resp_payload->cfg->ap;
		RPC_RESP_COPY_STR(p_c_ap->password, p_a_ap->password, PASSWORD_LENGTH);
		p_c_ap->ssid_len = p_a_ap->ssid_len;
		if (p_c_ap->ssid_len)
			RPC_RESP_COPY_STR(p_c_ap->ssid, p_a_ap->ssid, SSID_LENGTH);
		p_c_ap->channel = p_a_ap->channel;
		p_c_ap->authmode = p_a_ap->authmode;
		p_c_ap->ssid_hidden = p_a_ap->ssid_hidden;
		p_c_ap->max_connection = p_a_ap->max_connection;
		p_c_ap->beacon_interval = p_a_ap->beacon_interval;
		p_c_ap->csa_count = p_a_ap->csa_count;
		p_c_ap->dtim_period = p_a_ap->dtim_period;
		p_c_ap->pairwise_cipher = p_a_ap->pairwise_cipher;
		p_c_ap->ftm_responder = p_a_ap->ftm_responder;
		RPC_ALLOC_ELEMENT(WifiPmfConfig, p_c_ap->pmf_cfg, wifi_pmf_config__init);
		p_c_ap->pmf_cfg->capable = p_a_ap->pmf_cfg.capable;
		p_c_ap->pmf_cfg->required = p_a_ap->pmf_cfg.required;
		p_c_ap->sae_pwe_h2e = p_a_ap->sae_pwe_h2e;
#if H_GOT_AP_CONFIG_PARAM_TRANSITION_DISABLE
		p_c_ap->transition_disable = p_a_ap->transition_disable;
#endif
#if H_PRESENT_IN_ESP_IDF_5_5_0
		p_c_ap->sae_ext = p_a_ap->sae_ext;
		RPC_ALLOC_ELEMENT(WifiBssMaxIdleConfig, p_c_ap->bss_max_idle_cfg, wifi_bss_max_idle_config__init);
		p_c_ap->bss_max_idle_cfg->period = p_a_ap->bss_max_idle_cfg.period;
		p_c_ap->bss_max_idle_cfg->protected_keep_alive = p_a_ap->bss_max_idle_cfg.protected_keep_alive;
		p_c_ap->gtk_rekey_interval = p_a_ap->gtk_rekey_interval;
#endif
		break;
	}
	default:
		ESP_LOGE(TAG, "Unsupported WiFi interface[%u]\n", iface);
	} //switch

err:
	return ESP_OK;
}

static esp_err_t req_wifi_scan_start(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_scan_config_t scan_conf = {0};
	WifiScanConfig *p_c = NULL;
	WifiScanTime *p_c_st = NULL;
	wifi_scan_config_t * p_a = &scan_conf;
	wifi_scan_time_t *p_a_st = &p_a->scan_time;

	RPC_TEMPLATE(RpcRespWifiScanStart, resp_wifi_scan_start,
			RpcReqWifiScanStart, req_wifi_scan_start,
			rpc__resp__wifi_scan_start__init);

	p_c = req_payload->config;

	if (!req_payload->config || !req_payload->config_set) {
		p_a = NULL;
	} else {
		//RPC_REQ_COPY_STR(p_a->ssid, p_c->ssid, SSID_LENGTH);
		//RPC_REQ_COPY_STR(p_a->bssid, p_c->ssid, MAC_SIZE_BYTES);

		/* Note these are only pointers, not allocating memory for that */
		if (p_c->ssid.len)
			p_a->ssid = p_c->ssid.data;
		if (p_c->bssid.len)
			p_a->bssid = p_c->bssid.data;

		p_a->channel = p_c->channel;
		p_a->show_hidden = p_c->show_hidden;
		p_a->scan_type = p_c->scan_type;

		p_c_st = p_c->scan_time;

		p_a_st->passive = p_c_st->passive;
		p_a_st->active.min = p_c_st->active->min ;
		p_a_st->active.max = p_c_st->active->max ;

		p_a->home_chan_dwell_time = p_c->home_chan_dwell_time;

		if (p_c->channel_bitmap) {
			p_a->channel_bitmap.ghz_2_channels = p_c->channel_bitmap->ghz_2_channels;
			p_a->channel_bitmap.ghz_5_channels = p_c->channel_bitmap->ghz_5_channels;
		}
	}

	RPC_RET_FAIL_IF(esp_wifi_scan_start(p_a, req_payload->block));

	return ESP_OK;
}



static esp_err_t req_wifi_set_protocol(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetProtocol, resp_wifi_set_protocol,
		RpcReqWifiSetProtocol, req_wifi_set_protocol,
		rpc__resp__wifi_set_protocol__init);

	RPC_RET_FAIL_IF(esp_wifi_set_protocol(req_payload->ifx,
			req_payload->protocol_bitmap));

	return ESP_OK;
}

static esp_err_t req_wifi_get_protocol(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetProtocol, resp_wifi_get_protocol,
			RpcReqWifiGetProtocol, req_wifi_get_protocol,
			rpc__resp__wifi_get_protocol__init);

	uint8_t protocol_bitmap = 0;
	RPC_RET_FAIL_IF(esp_wifi_get_protocol(req_payload->ifx, &protocol_bitmap));

	resp_payload->protocol_bitmap = protocol_bitmap;
	return ESP_OK;
}

static esp_err_t req_wifi_scan_stop(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanStop, resp_wifi_scan_stop,
			RpcReqWifiScanStop, req_wifi_scan_stop,
			rpc__resp__wifi_scan_stop__init);

	RPC_RET_FAIL_IF(esp_wifi_scan_stop());
	return ESP_OK;
}

static esp_err_t req_wifi_scan_get_ap_num(Rpc *req, Rpc *resp, void *priv_data)
{
	uint16_t number = 0;
	int ret = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanGetApNum, resp_wifi_scan_get_ap_num,
			RpcReqWifiScanGetApNum, req_wifi_scan_get_ap_num,
			rpc__resp__wifi_scan_get_ap_num__init);

	ret = esp_wifi_scan_get_ap_num(&number);
	RPC_RET_FAIL_IF(ret);

	resp_payload->number = number;

	return ESP_OK;
}

// function only copies data: any required memory in the rpc struct must be allocated already
static int copy_ap_record_to_rpc_struct(WifiApRecord *rpc, wifi_ap_record_t *scan)
{
	ESP_LOGD(TAG, "Ssid: %s, Bssid: " MACSTR, scan->ssid, MAC2STR(scan->bssid));
	ESP_LOGD(TAG, "Primary: %u Second: %u Rssi: %d Authmode: %u",
		scan->primary, scan->second,
		scan->rssi, scan->authmode
		);
	ESP_LOGD(TAG, "PairwiseCipher: %u Groupcipher: %u Ant: %u",
		scan->pairwise_cipher, scan->group_cipher,
		scan->ant
		);
	ESP_LOGD(TAG, "Bitmask: 11b:%u g:%u n:%u a:%u ac:%u ax:%u lr:%u wps:%u ftm_resp:%u ftm_ini:%u res: %u",
		scan->phy_11b, scan->phy_11g, scan->phy_11n,
		scan->phy_11a, scan->phy_11ac, scan->phy_11ax,
		scan->phy_lr,
		scan->wps, scan->ftm_responder,
		scan->ftm_initiator, scan->reserved
		);
	RPC_COPY_STR(rpc->ssid, scan->ssid, SSID_LENGTH);
	RPC_COPY_BYTES(rpc->bssid, scan->bssid, BSSID_BYTES_SIZE);
	rpc->primary         = scan->primary;
	rpc->second          = scan->second;
	rpc->rssi            = scan->rssi;
	rpc->authmode        = scan->authmode;
	rpc->pairwise_cipher = scan->pairwise_cipher;
	rpc->group_cipher    = scan->group_cipher;
	rpc->ant             = scan->ant;

	/*Bitmask*/
	if (scan->phy_11b)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11b_BIT,rpc->bitmask);

	if (scan->phy_11g)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11g_BIT,rpc->bitmask);

	if (scan->phy_11n)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11n_BIT,rpc->bitmask);

	if (scan->phy_lr)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_lr_BIT,rpc->bitmask);

	if (scan->phy_11a)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11a_BIT,rpc->bitmask);

	if (scan->phy_11ac)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11ac_BIT,rpc->bitmask);

	if (scan->phy_11ax)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11ax_BIT,rpc->bitmask);

	if (scan->wps)
		H_SET_BIT(WIFI_SCAN_AP_REC_wps_BIT,rpc->bitmask);

	if (scan->ftm_responder)
		H_SET_BIT(WIFI_SCAN_AP_REC_ftm_responder_BIT,rpc->bitmask);

	if (scan->ftm_initiator)
		H_SET_BIT(WIFI_SCAN_AP_REC_ftm_initiator_BIT,rpc->bitmask);

	WIFI_SCAN_AP_SET_RESERVED_VAL(scan->reserved, rpc->bitmask);

	/* country */
	RPC_COPY_BYTES(rpc->country->cc, scan->country.cc, sizeof(scan->country.cc));
	rpc->country->schan        = scan->country.schan;
	rpc->country->nchan        = scan->country.nchan;
	rpc->country->max_tx_power = scan->country.max_tx_power;
	rpc->country->policy       = scan->country.policy;

	ESP_LOGD(TAG, "Country cc:%c%c schan: %u nchan: %u max_tx_pow: %d policy: %u",
		scan->country.cc[0], scan->country.cc[1], scan->country.schan, scan->country.nchan,
		scan->country.max_tx_power, scan->country.policy);

	/* he_ap */
	WifiHeApInfo * p_c_he_ap = rpc->he_ap;
	wifi_he_ap_info_t * p_a_he_ap = &scan->he_ap;

	// bss_color uses six bits
	p_c_he_ap->bitmask = (p_a_he_ap->bss_color & WIFI_HE_AP_INFO_BSS_COLOR_BITS);

	if (p_a_he_ap->partial_bss_color)
		H_SET_BIT(WIFI_HE_AP_INFO_partial_bss_color_BIT,p_c_he_ap->bitmask);

	if (p_a_he_ap->bss_color_disabled)
		H_SET_BIT(WIFI_HE_AP_INFO_bss_color_disabled_BIT,p_c_he_ap->bitmask);

	p_c_he_ap->bssid_index = p_a_he_ap->bssid_index;

	ESP_LOGD(TAG, "HE_AP: bss_color %d, partial_bss_color %d, bss_color_disabled %d",
		p_a_he_ap->bss_color, p_a_he_ap->bss_color_disabled, p_a_he_ap->bss_color_disabled);

	rpc->bandwidth    = scan->bandwidth;
	rpc->vht_ch_freq1 = scan->vht_ch_freq1;
	rpc->vht_ch_freq2 = scan->vht_ch_freq2;

	return 0;
}

static esp_err_t req_wifi_scan_get_ap_record(Rpc *req, Rpc *resp, void *priv_data)
{
	int ret = 0;
	wifi_ap_record_t *p_a_ap = NULL;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanGetApRecord, resp_wifi_scan_get_ap_record,
			RpcReqWifiScanGetApRecord, req_wifi_scan_get_ap_record,
			rpc__resp__wifi_scan_get_ap_record__init);

	p_a_ap = (wifi_ap_record_t *)calloc(1, sizeof(wifi_ap_record_t));
	RPC_RET_FAIL_IF(!p_a_ap);

	ret = esp_wifi_scan_get_ap_record(p_a_ap);
	if (ret) {
		ESP_LOGE(TAG,"failed to get ap record");
		resp_payload->resp = ret;
		goto err;
	}

	RPC_ALLOC_ELEMENT(WifiApRecord, resp_payload->ap_record, wifi_ap_record__init);
	RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->ap_record->country, wifi_country__init);
	RPC_ALLOC_ELEMENT(WifiHeApInfo, resp_payload->ap_record->he_ap, wifi_he_ap_info__init);

	ret = copy_ap_record_to_rpc_struct(resp_payload->ap_record, p_a_ap);
	if (ret) {
		ESP_LOGE(TAG, "failed to copy ap record to rpc struct");
		resp_payload->resp = ret;
	}

err:
	mem_free(p_a_ap);
	return ESP_OK;
}

static esp_err_t req_wifi_scan_get_ap_records(Rpc *req, Rpc *resp, void *priv_data)
{
	uint16_t number = 0;
	uint16_t ap_count = 0;
	int ret = 0;
	uint16_t i;

	wifi_ap_record_t *p_a_ap_list = NULL;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanGetApRecords, resp_wifi_scan_get_ap_records,
			RpcReqWifiScanGetApRecords, req_wifi_scan_get_ap_records,
			rpc__resp__wifi_scan_get_ap_records__init);

	number = req->req_wifi_scan_get_ap_records->number;
	ESP_LOGD(TAG,"n_elem_scan_list predicted: %u\n", number);

	p_a_ap_list = (wifi_ap_record_t *)calloc(number, sizeof(wifi_ap_record_t));
	RPC_RET_FAIL_IF(!p_a_ap_list);

	ret = esp_wifi_scan_get_ap_num(&ap_count);
	if (ret || !ap_count) {
		ESP_LOGE(TAG,"esp_wifi_scan_get_ap_num: ret: %d num_ap_scanned:%u", ret, number);
		goto err;
	}
	if (number < ap_count) {
		ESP_LOGI(TAG,"n_elem_scan_list wants to return: %u Limit to %u\n", ap_count, number);
	}

	ret = esp_wifi_scan_get_ap_records(&number, p_a_ap_list);
	if(ret) {
		ESP_LOGE(TAG,"Failed to scan ap records");
		goto err;
	}

	resp_payload->number = number;
	resp_payload->ap_records = (WifiApRecord**)calloc(number, sizeof(WifiApRecord *));
	if (!resp_payload->ap_records) {
		ESP_LOGE(TAG,"resp: malloc failed for resp_payload->ap_records");
		resp_payload->resp = RPC_ERR_MEMORY_FAILURE;
		goto err;
	}

	for (i=0;i<number;i++) {
		ESP_LOGD(TAG, "ap_record[%u]:", i+1);
		RPC_ALLOC_ELEMENT(WifiApRecord, resp_payload->ap_records[i], wifi_ap_record__init);
		RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->ap_records[i]->country, wifi_country__init);
		RPC_ALLOC_ELEMENT(WifiHeApInfo, resp_payload->ap_records[i]->he_ap, wifi_he_ap_info__init);

		ret = copy_ap_record_to_rpc_struct(resp_payload->ap_records[i], &p_a_ap_list[i]);
		if (ret) {
			ESP_LOGE(TAG, "failed to copy ap record to rpc struct");
			resp_payload->resp = ret;
			goto err;
		}

		/* increment num of records in rpc msg */
		resp_payload->n_ap_records++;
	}

err:
	mem_free(p_a_ap_list);
	return ESP_OK;
}

static esp_err_t req_wifi_clear_ap_list(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiClearApList, resp_wifi_clear_ap_list,
			RpcReqWifiClearApList, req_wifi_clear_ap_list,
			rpc__resp__wifi_clear_ap_list__init);

	RPC_RET_FAIL_IF(esp_wifi_clear_ap_list());
	return ESP_OK;
}

static esp_err_t req_wifi_restore(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiRestore, resp_wifi_restore,
			RpcReqWifiRestore, req_wifi_restore,
			rpc__resp__wifi_restore__init);

	RPC_RET_FAIL_IF(esp_wifi_restore());
	return ESP_OK;
}

static esp_err_t req_wifi_clear_fast_connect(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiClearFastConnect, resp_wifi_clear_fast_connect,
			RpcReqWifiClearFastConnect, req_wifi_clear_fast_connect,
			rpc__resp__wifi_clear_fast_connect__init);

	RPC_RET_FAIL_IF(esp_wifi_clear_fast_connect());
	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_ap_info(Rpc *req, Rpc *resp, void *priv_data)
{
	int ret = 0;
	wifi_ap_record_t p_a_ap_info = {0};

	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetApInfo, resp_wifi_sta_get_ap_info,
			RpcReqWifiStaGetApInfo, req_wifi_sta_get_ap_info,
			rpc__resp__wifi_sta_get_ap_info__init);


	RPC_RET_FAIL_IF(esp_wifi_sta_get_ap_info(&p_a_ap_info));
	RPC_ALLOC_ELEMENT(WifiApRecord, resp_payload->ap_record, wifi_ap_record__init);
	RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->ap_record->country, wifi_country__init);
	RPC_ALLOC_ELEMENT(WifiHeApInfo, resp_payload->ap_record->he_ap, wifi_he_ap_info__init);

	ret = copy_ap_record_to_rpc_struct(resp_payload->ap_record, &p_a_ap_info);
	if (ret) {
		ESP_LOGE(TAG, "failed to copy ap info to rpc struct");
		resp_payload->resp = ret;
	}
err:
	return ESP_OK;
}


static esp_err_t req_wifi_deauth_sta(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiDeauthSta, resp_wifi_deauth_sta,
			RpcReqWifiDeauthSta, req_wifi_deauth_sta,
			rpc__resp__wifi_deauth_sta__init);

	RPC_RET_FAIL_IF(esp_wifi_deauth_sta(req_payload->aid));
	return ESP_OK;
}

static esp_err_t req_wifi_set_storage(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetStorage, resp_wifi_set_storage,
			RpcReqWifiSetStorage, req_wifi_set_storage,
			rpc__resp__wifi_set_storage__init);

	ESP_LOGI(TAG, "Setting wifi storage: %lu", req_payload->storage);

	RPC_RET_FAIL_IF(esp_wifi_set_storage(req_payload->storage));

	return ESP_OK;
}

static esp_err_t req_wifi_set_bandwidth(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetBandwidth, resp_wifi_set_bandwidth,
			RpcReqWifiSetBandwidth, req_wifi_set_bandwidth,
			rpc__resp__wifi_set_bandwidth__init);

	RPC_RET_FAIL_IF(esp_wifi_set_bandwidth(req_payload->ifx, req_payload->bw));

	return ESP_OK;
}

static esp_err_t req_wifi_get_bandwidth(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetBandwidth, resp_wifi_get_bandwidth,
			RpcReqWifiGetBandwidth, req_wifi_get_bandwidth,
			rpc__resp__wifi_get_bandwidth__init);

	wifi_bandwidth_t bw = 0;
	RPC_RET_FAIL_IF(esp_wifi_get_bandwidth(req_payload->ifx, &bw));

	resp_payload->bw = bw;
	return ESP_OK;
}

static esp_err_t req_wifi_set_channel(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetChannel, resp_wifi_set_channel,
			RpcReqWifiSetChannel, req_wifi_set_channel,
			rpc__resp__wifi_set_channel__init);

	RPC_RET_FAIL_IF(esp_wifi_set_channel(req_payload->primary, req_payload->second));

	return ESP_OK;
}

static esp_err_t req_wifi_get_channel(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetChannel, resp_wifi_get_channel,
			RpcReqWifiGetChannel, req_wifi_get_channel,
			rpc__resp__wifi_get_channel__init);

	uint8_t primary = 0;
	wifi_second_chan_t second = 0;
	RPC_RET_FAIL_IF(esp_wifi_get_channel(&primary, &second));

	resp_payload->primary = primary;
	resp_payload->second = second;
	return ESP_OK;
}

static esp_err_t req_wifi_set_country_code(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetCountryCode, resp_wifi_set_country_code,
			RpcReqWifiSetCountryCode, req_wifi_set_country_code,
			rpc__resp__wifi_set_country_code__init);

	char cc[3] = {0}; // country code
	RPC_RET_FAIL_IF(!req_payload->country.data);
	RPC_REQ_COPY_STR(&cc[0], req_payload->country, 2); // only copy the first two chars

	RPC_RET_FAIL_IF(esp_wifi_set_country_code(&cc[0],
			req_payload->ieee80211d_enabled));

	return ESP_OK;
}

static esp_err_t req_wifi_get_country_code(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetCountryCode, resp_wifi_get_country_code,
			RpcReqWifiGetCountryCode, req_wifi_get_country_code,
			rpc__resp__wifi_get_country_code__init);

	char cc[3] = {0}; // country code
	RPC_RET_FAIL_IF(esp_wifi_get_country_code(&cc[0]));

	RPC_RESP_COPY_STR(resp_payload->country, &cc[0], sizeof(cc));

	return ESP_OK;
}

static esp_err_t req_wifi_set_country(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetCountry, resp_wifi_set_country,
			RpcReqWifiSetCountry, req_wifi_set_country,
			rpc__resp__wifi_set_country__init);

	RPC_RET_FAIL_IF(!req_payload->country);

	wifi_country_t country = {0};
	WifiCountry * p_c_country = req_payload->country;
	RPC_REQ_COPY_BYTES(&country.cc[0], p_c_country->cc, sizeof(country.cc));
	country.schan        = p_c_country->schan;
	country.nchan        = p_c_country->nchan;
	country.max_tx_power = p_c_country->max_tx_power;
	country.policy       = p_c_country->policy;

	RPC_RET_FAIL_IF(esp_wifi_set_country(&country));

	return ESP_OK;
}

static esp_err_t req_wifi_get_country(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetCountry, resp_wifi_get_country,
			RpcReqWifiGetCountry, req_wifi_get_country,
			rpc__resp__wifi_get_country__init);

	wifi_country_t country = {0};
	RPC_RET_FAIL_IF(esp_wifi_get_country(&country));

	RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->country, wifi_country__init);
	WifiCountry * p_c_country = resp_payload->country;
	RPC_RESP_COPY_BYTES(p_c_country->cc, &country.cc[0], sizeof(country.cc));
	p_c_country->schan        = country.schan;
	p_c_country->nchan        = country.nchan;
	p_c_country->max_tx_power = country.max_tx_power;
	p_c_country->policy       = country.policy;

err:
	return ESP_OK;
}

static esp_err_t req_wifi_ap_get_sta_list(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiApGetStaList, resp_wifi_ap_get_sta_list,
			RpcReqWifiApGetStaList, req_wifi_ap_get_sta_list,
			rpc__resp__wifi_ap_get_sta_list__init);

	wifi_sta_list_t sta;
	RPC_RET_FAIL_IF(esp_wifi_ap_get_sta_list(&sta));

	RPC_ALLOC_ELEMENT(WifiStaList, resp_payload->sta_list, wifi_sta_list__init);
	WifiStaList * p_c_sta_list = resp_payload->sta_list;

	resp_payload->sta_list->sta = (WifiStaInfo**)calloc(ESP_WIFI_MAX_CONN_NUM, sizeof(WifiStaInfo *));
	if (!resp_payload->sta_list->sta) {
		ESP_LOGE(TAG,"resp: malloc failed for resp_payload->sta_list->sta");
		goto err;
	}

	for (int i = 0; i < ESP_WIFI_MAX_CONN_NUM; i++) {
		RPC_ALLOC_ELEMENT(WifiStaInfo, p_c_sta_list->sta[i], wifi_sta_info__init);
		WifiStaInfo * p_c_sta_info = p_c_sta_list->sta[i];

		RPC_RESP_COPY_BYTES(p_c_sta_info->mac, &sta.sta[i].mac[0], sizeof(sta.sta[i].mac));
		p_c_sta_info->rssi = sta.sta[i].rssi;

		if (sta.sta[i].phy_11b)
			H_SET_BIT(WIFI_STA_INFO_phy_11b_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_11g)
			H_SET_BIT(WIFI_STA_INFO_phy_11g_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_11n)
			H_SET_BIT(WIFI_STA_INFO_phy_11n_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_lr)
			H_SET_BIT(WIFI_STA_INFO_phy_lr_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_11ax)
			H_SET_BIT(WIFI_STA_INFO_phy_11ax_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].is_mesh_child)
			H_SET_BIT(WIFI_STA_INFO_is_mesh_child_BIT, p_c_sta_info->bitmask);

		WIFI_STA_INFO_SET_RESERVED_VAL(sta.sta[i].reserved, p_c_sta_info->bitmask);
	}
	// number of sta records in the list
	resp_payload->sta_list->n_sta = ESP_WIFI_MAX_CONN_NUM;

	p_c_sta_list->num = sta.num;

err:
	return ESP_OK;
}

static esp_err_t req_wifi_ap_get_sta_aid(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiApGetStaAid, resp_wifi_ap_get_sta_aid,
			RpcReqWifiApGetStaAid, req_wifi_ap_get_sta_aid,
			rpc__resp__wifi_ap_get_sta_aid__init);

	uint8_t mac[6];
	uint16_t aid;

	RPC_REQ_COPY_BYTES(mac, req_payload->mac, sizeof(mac));
	ESP_LOGI(TAG, "mac: " MACSTR, MAC2STR(mac));
	RPC_RET_FAIL_IF(esp_wifi_ap_get_sta_aid(mac, &aid));

	resp_payload->aid = aid;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_rssi(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetRssi, resp_wifi_sta_get_rssi,
			RpcReqWifiStaGetRssi, req_wifi_sta_get_rssi,
			rpc__resp__wifi_sta_get_rssi__init);

	int rssi;
	RPC_RET_FAIL_IF(esp_wifi_sta_get_rssi(&rssi));

	resp_payload->rssi = rssi;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_aid(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetAid, resp_wifi_sta_get_aid,
			RpcReqWifiStaGetAid, req_wifi_sta_get_aid,
			rpc__resp__wifi_sta_get_aid__init);

	uint16_t aid;
	RPC_RET_FAIL_IF(esp_wifi_sta_get_aid(&aid));

	resp_payload->aid = aid;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_negotiated_phymode(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetNegotiatedPhymode, resp_wifi_sta_get_negotiated_phymode,
			RpcReqWifiStaGetNegotiatedPhymode, req_wifi_sta_get_netogitated_phymode,
			rpc__resp__wifi_sta_get_negotiated_phymode__init);

	wifi_phy_mode_t phymode;
	RPC_RET_FAIL_IF(esp_wifi_sta_get_negotiated_phymode(&phymode));

	resp_payload->phymode = phymode;

	return ESP_OK;
}

#if H_PRESENT_IN_ESP_IDF_5_4_0
static esp_err_t req_wifi_set_protocols(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetProtocols, resp_wifi_set_protocols,
			RpcReqWifiSetProtocols, req_wifi_set_protocols,
			rpc__resp__wifi_set_protocols__init);

	RPC_RET_FAIL_IF(!req_payload->protocols);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_protocols_t protocols;
	protocols.ghz_2g = req_payload->protocols->ghz_2g;
	protocols.ghz_5g = req_payload->protocols->ghz_5g;

	ESP_LOGI(TAG, "set protocols: ghz_2g %d, ghz_5g %d", protocols.ghz_2g, protocols.ghz_5g);

	RPC_RET_FAIL_IF(esp_wifi_set_protocols(ifx, &protocols));

	return ESP_OK;
}

static esp_err_t req_wifi_get_protocols(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetProtocols, resp_wifi_get_protocols,
			RpcReqWifiGetProtocols, req_wifi_get_protocols,
			rpc__resp__wifi_get_protocols__init);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_protocols_t protocols;

	RPC_RET_FAIL_IF(esp_wifi_get_protocols(ifx, &protocols));

	RPC_ALLOC_ELEMENT(WifiProtocols, resp_payload->protocols, wifi_protocols__init);
	resp_payload->protocols->ghz_2g = protocols.ghz_2g;
	resp_payload->protocols->ghz_5g = protocols.ghz_5g;

	ESP_LOGI(TAG, "get protocols: ghz_2g %d, ghz_5g %d", protocols.ghz_2g, protocols.ghz_5g);
err:
	return ESP_OK;
}

static esp_err_t req_wifi_set_bandwidths(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetBandwidths, resp_wifi_set_bandwidths,
			RpcReqWifiSetBandwidths, req_wifi_set_bandwidths,
			rpc__resp__wifi_set_bandwidths__init);

	RPC_RET_FAIL_IF(!req_payload->bandwidths);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_bandwidths_t bw;

	bw.ghz_2g = req_payload->bandwidths->ghz_2g;
	bw.ghz_5g = req_payload->bandwidths->ghz_5g;

	ESP_LOGI(TAG, "set bandwidths: ghz_2g %d, ghz_5g %d", bw.ghz_2g, bw.ghz_5g);

	RPC_RET_FAIL_IF(esp_wifi_set_bandwidths(ifx, &bw));

	return ESP_OK;
}

static esp_err_t req_wifi_get_bandwidths(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetBandwidths, resp_wifi_get_bandwidths,
			RpcReqWifiGetBandwidths, req_wifi_get_bandwidths,
			rpc__resp__wifi_get_bandwidths__init);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_bandwidths_t bw;

	RPC_RET_FAIL_IF(esp_wifi_get_bandwidths(ifx, &bw));

	RPC_ALLOC_ELEMENT(WifiBandwidths, resp_payload->bandwidths, wifi_bandwidths__init);

	resp_payload->bandwidths->ghz_2g = bw.ghz_2g;
	resp_payload->bandwidths->ghz_5g = bw.ghz_5g;

	ESP_LOGI(TAG, "get bandwidths: ghz_2g %d, ghz_5g %d", bw.ghz_2g, bw.ghz_5g);
err:
	return ESP_OK;
}

static esp_err_t req_wifi_set_band(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetBand, resp_wifi_set_band,
			RpcReqWifiSetBand, req_wifi_set_band,
			rpc__resp__wifi_set_band__init);

	wifi_band_t band;
	band = req_payload->band;

	RPC_RET_FAIL_IF(esp_wifi_set_band(band));

	return ESP_OK;
}

static esp_err_t req_wifi_get_band(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetBand, resp_wifi_get_band,
			RpcReqWifiGetBand, req_wifi_get_band,
			rpc__resp__wifi_get_band__init);

	wifi_band_t band;
	RPC_RET_FAIL_IF(esp_wifi_get_band(&band));

	resp_payload->band = band;

	ESP_LOGW(TAG, "get band: %d", band);

	return ESP_OK;
}

static esp_err_t req_wifi_set_band_mode(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetBandMode, resp_wifi_set_bandmode,
			RpcReqWifiSetBandMode, req_wifi_set_bandmode,
			rpc__resp__wifi_set_band_mode__init);

	wifi_band_mode_t band_mode;
	band_mode = req_payload->bandmode;

	ESP_LOGW(TAG, "set band mode: %d", band_mode);

	RPC_RET_FAIL_IF(esp_wifi_set_band_mode(band_mode));

	return ESP_OK;
}

static esp_err_t req_wifi_get_band_mode(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetBandMode, resp_wifi_get_bandmode,
			RpcReqWifiGetBandMode, req_wifi_get_bandmode,
			rpc__resp__wifi_get_band_mode__init);

	wifi_band_mode_t band_mode;
	RPC_RET_FAIL_IF(esp_wifi_get_band_mode(&band_mode));

	resp_payload->bandmode = band_mode;

	ESP_LOGW(TAG, "get band_mode: %d", band_mode);

	return ESP_OK;
}
#endif // H_PRESENT_IN_ESP_IDF_5_4_0

/* Get DHCP/DNS status handler */
static esp_err_t req_get_dhcp_dns_status(Rpc *req, Rpc *resp, void *priv_data)
{
#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
	RPC_TEMPLATE(RpcRespGetDhcpDnsStatus, resp_get_dhcp_dns,
			RpcReqGetDhcpDnsStatus, req_get_dhcp_dns,
			rpc__resp__get_dhcp_dns_status__init);
#else
	RPC_TEMPLATE_SIMPLE(RpcRespGetDhcpDnsStatus, resp_get_dhcp_dns,
			RpcReqGetDhcpDnsStatus, req_get_dhcp_dns,
			rpc__resp__get_dhcp_dns_status__init);
#endif

#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
	int ret1, ret2;
	esp_netif_ip_info_t ip_info = {0};
	esp_netif_dns_info_t dns = {0};
	uint8_t netlink_up = 0;

	ret1 = get_slave_static_ip(req_payload->iface, &ip_info, &netlink_up);
	ret2 = get_slave_dns(req_payload->iface, &dns);

	if (ret1 || ret2) {
		ESP_LOGE(TAG, "Failed to get DHCP/DNS status");
		resp_payload->resp = ESP_FAIL;
		resp_payload->dhcp_up = 0;
		resp_payload->dns_up = 0;
		resp_payload->net_link_up = 0;
		return ESP_OK;
	}
	ESP_LOGI(TAG, "static_ip_ret: %d dns_ret: %d", ret1, ret2);

	resp_payload->net_link_up = netlink_up;
	resp_payload->dhcp_up = netlink_up;
	resp_payload->dns_up = netlink_up;
	resp_payload->dns_type = dns.ip.type;

	char sta_ip[64] = {0};
	char sta_nm[64] = {0};
	char sta_gw[64] = {0};
	char sta_dns_ip[64] = {0};

	if (esp_ip4addr_ntoa(&ip_info.ip, sta_ip, sizeof(sta_ip))) {
		strlcpy((char *)resp_payload->dhcp_ip.data, sta_ip, sizeof(resp_payload->dhcp_ip.data));
		resp_payload->dhcp_ip.len = strlen(sta_ip);
	}
	if (esp_ip4addr_ntoa(&ip_info.netmask, sta_nm, sizeof(sta_nm))) {
		strlcpy((char *)resp_payload->dhcp_nm.data, sta_nm, sizeof(resp_payload->dhcp_nm.data));
		resp_payload->dhcp_nm.len = strlen(sta_nm);
	}
	if (esp_ip4addr_ntoa(&ip_info.gw, sta_gw, sizeof(sta_gw))) {
		strlcpy((char *)resp_payload->dhcp_gw.data, sta_gw, sizeof(resp_payload->dhcp_gw.data));
		resp_payload->dhcp_gw.len = strlen(sta_gw);
	}
	if (esp_ip4addr_ntoa(&dns.ip.u_addr.ip4, sta_dns_ip, sizeof(sta_dns_ip))) {
		strlcpy((char *)resp_payload->dns_ip.data, sta_dns_ip, sizeof(resp_payload->dns_ip.data));
		resp_payload->dns_ip.len = strlen(sta_dns_ip);
	}

	ESP_LOGI(TAG, "Fetched IP: %s, NM: %s, GW: %s, DNS IP: %s, Type: %"PRId32,
			resp_payload->dhcp_ip.data,
			resp_payload->dhcp_nm.data,
			resp_payload->dhcp_gw.data,
			resp_payload->dns_ip.data,
			resp_payload->dns_type);

	resp_payload->resp = ESP_OK;
#else
	resp_payload->resp = ESP_FAIL;
#endif
	return ESP_OK;
}

static esp_err_t req_set_dhcp_dns_status(Rpc *req, Rpc *resp, void *priv_data)
{

#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
	RPC_TEMPLATE(RpcRespSetDhcpDnsStatus, resp_set_dhcp_dns,
			RpcReqSetDhcpDnsStatus, req_set_dhcp_dns,
			rpc__resp__set_dhcp_dns_status__init);
#else
	RPC_TEMPLATE_SIMPLE(RpcRespSetDhcpDnsStatus, resp_set_dhcp_dns,
			RpcReqSetDhcpDnsStatus, req_set_dhcp_dns,
			rpc__resp__set_dhcp_dns_status__init);
#endif

#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED

	uint8_t iface = req_payload->iface;
	uint8_t net_link_up = req_payload->net_link_up;
	uint8_t dhcp_up = req_payload->dhcp_up;
	uint8_t dns_up = req_payload->dns_up;
	uint8_t dns_type = req_payload->dns_type;

	char dhcp_ip[64] = {0};
	char dhcp_nm[64] = {0};
	char dhcp_gw[64] = {0};
	char dns_ip[64] = {0};

	ESP_LOGI(TAG, "iface: %u link_up:%u dhcp_up:%u dns_up:%u dns_type:%u",
			iface, net_link_up, dhcp_up, dns_up, dns_type);

	if (req_payload->dhcp_ip.len)
		ESP_LOGI(TAG, "dhcp ip: %s" , req_payload->dhcp_ip.data);
	if (req_payload->dhcp_nm.len)
		ESP_LOGI(TAG, "dhcp nm: %s" , req_payload->dhcp_nm.data);
	if (req_payload->dhcp_gw.len)
		ESP_LOGI(TAG, "dhcp gw: %s" , req_payload->dhcp_gw.data);
	if (req_payload->dns_ip.len)
		ESP_LOGI(TAG, "dns ip: %s" , req_payload->dns_ip.data);

	RPC_REQ_COPY_BYTES(dhcp_ip, req_payload->dhcp_ip, sizeof(dhcp_ip));
	RPC_REQ_COPY_BYTES(dhcp_nm, req_payload->dhcp_nm, sizeof(dhcp_nm));
	RPC_REQ_COPY_BYTES(dhcp_gw, req_payload->dhcp_gw, sizeof(dhcp_gw));
	RPC_REQ_COPY_BYTES(dns_ip, req_payload->dns_ip, sizeof(dns_ip));

	if (dhcp_up)
		set_slave_static_ip(iface, dhcp_ip, dhcp_nm, dhcp_gw);

	if (dns_up)
		set_slave_dns(iface, dns_ip, dns_type);
#else

	resp_payload->resp = ESP_FAIL;

#endif

	return ESP_OK;
}

static esp_err_t req_wifi_set_inactive_time(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetInactiveTime, resp_wifi_set_inactive_time,
			RpcReqWifiSetInactiveTime, req_wifi_set_inactive_time,
			rpc__resp__wifi_set_inactive_time__init);

	wifi_interface_t ifx = req_payload->ifx;
	uint16_t sec = req_payload->sec;

	RPC_RET_FAIL_IF(esp_wifi_set_inactive_time(ifx, sec));

	return ESP_OK;
}

static esp_err_t req_wifi_get_inactive_time(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetInactiveTime, resp_wifi_get_inactive_time,
			RpcReqWifiGetInactiveTime, req_wifi_get_inactive_time,
			rpc__resp__wifi_get_inactive_time__init);

	wifi_interface_t ifx = req_payload->ifx;
	uint16_t sec;

	RPC_RET_FAIL_IF(esp_wifi_get_inactive_time(ifx, &sec));

	resp_payload->sec = sec;

	return ESP_OK;
}

#if H_WIFI_ENTERPRISE_SUPPORT
static esp_err_t req_wifi_sta_enterprise_enable(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaEnterpriseEnable, resp_wifi_sta_enterprise_enable,
			RpcReqWifiStaEnterpriseEnable, req_wifi_sta_enterprise_enable,
			rpc__resp__wifi_sta_enterprise_enable__init);

	RPC_RET_FAIL_IF(esp_wifi_sta_enterprise_enable());

	return ESP_OK;
}

static esp_err_t req_wifi_sta_enterprise_disable(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaEnterpriseDisable, resp_wifi_sta_enterprise_disable,
			RpcReqWifiStaEnterpriseDisable, req_wifi_sta_enterprise_disable,
			rpc__resp__wifi_sta_enterprise_disable__init);

	free_g_ca_cert();
	free_all_g_eap_cert_and_key();
	RPC_RET_FAIL_IF(esp_wifi_sta_enterprise_disable());

	return ESP_OK;
}

static esp_err_t req_eap_set_identity(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetIdentity, resp_eap_set_identity,
				RpcReqEapSetIdentity, req_eap_set_identity,
				rpc__resp__eap_set_identity__init);

	RPC_RET_FAIL_IF(!req_payload->identity.data || req_payload->len <= 0);
	const unsigned char *identity = req_payload->identity.data;
	int len = req_payload->len;

	RPC_RET_FAIL_IF(esp_eap_client_set_identity(identity, len));
	return ESP_OK;
}

static esp_err_t req_eap_clear_identity(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespEapClearIdentity, resp_eap_clear_identity,
				RpcReqEapClearIdentity, req_eap_clear_identity,
				rpc__resp__eap_clear_identity__init);

	esp_eap_client_clear_identity();

	return ESP_OK;
}

static esp_err_t req_eap_set_username(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetUsername, resp_eap_set_username,
				RpcReqEapSetUsername, req_eap_set_username,
				rpc__resp__eap_set_username__init);

	RPC_RET_FAIL_IF(!req_payload->username.data || req_payload->len <= 0);
	const unsigned char *username = req_payload->username.data;
	int len = req_payload->len;

	RPC_RET_FAIL_IF(esp_eap_client_set_username(username, len));

	return ESP_OK;
}

static esp_err_t req_eap_clear_username(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespEapClearUsername, resp_eap_clear_username,
				RpcReqEapClearUsername, req_eap_clear_username,
				rpc__resp__eap_clear_username__init);

	esp_eap_client_clear_username();

	return ESP_OK;
}

static esp_err_t req_eap_set_password(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetPassword, resp_eap_set_password,
				RpcReqEapSetPassword, req_eap_set_password,
				rpc__resp__eap_set_password__init);

	RPC_RET_FAIL_IF(!req_payload->password.data || req_payload->len <= 0);
	const unsigned char *password = req_payload->password.data;
	int len = req_payload->len;

	RPC_RET_FAIL_IF(esp_eap_client_set_password(password, len));

	return ESP_OK;
}

static esp_err_t req_eap_clear_password(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespEapClearPassword, resp_eap_clear_password,
				RpcReqEapClearPassword, req_eap_clear_password,
				rpc__resp__eap_clear_password__init);

	esp_eap_client_clear_password();

	return ESP_OK;
}

static esp_err_t req_eap_set_new_password(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetNewPassword, resp_eap_set_new_password,
				RpcReqEapSetNewPassword, req_eap_set_new_password,
				rpc__resp__eap_set_new_password__init);

	RPC_RET_FAIL_IF(!req_payload->new_password.data || req_payload->len <= 0);

	RPC_RET_FAIL_IF(esp_eap_client_set_password(req_payload->new_password.data, req_payload->len));

	return ESP_OK;
}

static esp_err_t req_eap_clear_new_password(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespEapClearNewPassword, resp_eap_clear_new_password,
				RpcReqEapClearNewPassword, req_eap_clear_new_password,
				rpc__resp__eap_clear_new_password__init);

	esp_eap_client_clear_new_password();

	return ESP_OK;
}

static esp_err_t req_eap_set_ca_cert(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetCaCert, resp_eap_set_ca_cert,
				RpcReqEapSetCaCert, req_eap_set_ca_cert,
				rpc__resp__eap_set_ca_cert__init);

	RPC_RET_FAIL_IF(!req_payload->ca_cert.data || req_payload->ca_cert_len <= 0);
	free_g_ca_cert();
	g_ca_cert_len = req_payload->ca_cert_len;
	if (g_ca_cert_len) {
		g_ca_cert = (unsigned char *)malloc(g_ca_cert_len);
		if (g_ca_cert == NULL) {
			return ESP_ERR_NO_MEM;
		}
		memcpy(g_ca_cert, req_payload->ca_cert.data, g_ca_cert_len);
	}

	RPC_RET_FAIL_IF(esp_eap_client_set_ca_cert(g_ca_cert, g_ca_cert_len));

	return ESP_OK;
}

static esp_err_t req_eap_clear_ca_cert(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespEapClearCaCert, resp_eap_clear_ca_cert,
				RpcReqEapClearCaCert, req_eap_clear_ca_cert,
				rpc__resp__eap_clear_ca_cert__init);

	free_g_ca_cert();
	esp_eap_client_clear_ca_cert();

	return ESP_OK;
}

static esp_err_t req_eap_set_certificate_and_key(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetCertificateAndKey, resp_eap_set_certificate_and_key,
				RpcReqEapSetCertificateAndKey, req_eap_set_certificate_and_key,
				rpc__resp__eap_set_certificate_and_key__init);

	RPC_RET_FAIL_IF((req_payload->client_cert.data && req_payload->client_cert_len <= 0) ||
			(req_payload->client_cert_len > 0 && !req_payload->client_cert.data));
	RPC_RET_FAIL_IF((req_payload->private_key.data && req_payload->private_key_len <= 0) ||
			(req_payload->private_key_len > 0 && !req_payload->private_key.data));
	RPC_RET_FAIL_IF((req_payload->private_key_password.data && req_payload->private_key_passwd_len <= 0) ||
			(req_payload->private_key_passwd_len > 0 && !req_payload->private_key_password.data));

	free_all_g_eap_cert_and_key();
	g_client_cert_len = req_payload->client_cert_len;
	if (g_client_cert_len) {
		g_client_cert = (unsigned char *)malloc(g_client_cert_len);
		if (g_client_cert == NULL) {
			return ESP_ERR_NO_MEM;
		}
		memcpy(g_client_cert, req_payload->client_cert.data, g_client_cert_len);
	}

	g_private_key_len = req_payload->private_key_len;
	if (g_private_key_len) {
		g_private_key = (unsigned char *)malloc(g_private_key_len);
		if (g_private_key == NULL) {
			free_all_g_eap_cert_and_key();
			return ESP_ERR_NO_MEM;
		}
		memcpy(g_private_key, req_payload->private_key.data, g_private_key_len);
	}

	g_private_key_passwd_len = req_payload->private_key_passwd_len;
	if (g_private_key_passwd_len) {
		g_private_key_password = (unsigned char *)malloc(g_private_key_passwd_len);
		if (g_private_key_password == NULL) {
			free_all_g_eap_cert_and_key();
			return ESP_ERR_NO_MEM;
		}
		memcpy(g_private_key_password, req_payload->private_key_password.data, g_private_key_passwd_len);
	}

	RPC_RET_FAIL_IF(esp_eap_client_set_certificate_and_key(g_client_cert, g_client_cert_len,
													g_private_key, g_private_key_len,
													g_private_key_password, g_private_key_passwd_len));

	return ESP_OK;
}

static esp_err_t req_eap_clear_certificate_and_key(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespEapClearCertificateAndKey, resp_eap_clear_certificate_and_key,
				RpcReqEapClearCertificateAndKey, req_eap_clear_certificate_and_key,
				rpc__resp__eap_clear_certificate_and_key__init);

	free_all_g_eap_cert_and_key();
	esp_eap_client_clear_certificate_and_key();

	return ESP_OK;
}

static esp_err_t req_eap_set_disable_time_check(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetDisableTimeCheck, resp_eap_set_disable_time_check,
				RpcReqEapSetDisableTimeCheck, req_eap_set_disable_time_check,
				rpc__resp__eap_set_disable_time_check__init);

	RPC_RET_FAIL_IF(esp_eap_client_set_disable_time_check(req_payload->disable));

	return ESP_OK;
}

static esp_err_t req_eap_get_disable_time_check(Rpc *req, Rpc *resp, void *priv_data)
{
	bool disable = false;

	RPC_TEMPLATE_SIMPLE(RpcRespEapGetDisableTimeCheck, resp_eap_get_disable_time_check,
				RpcReqEapGetDisableTimeCheck, req_eap_get_disable_time_check,
				rpc__resp__eap_get_disable_time_check__init);

	RPC_RET_FAIL_IF(esp_eap_client_get_disable_time_check(&disable));

	resp_payload->disable = disable;

	return ESP_OK;
}

static esp_err_t req_eap_set_ttls_phase2_method(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetTtlsPhase2Method, resp_eap_set_ttls_phase2_method,
				RpcReqEapSetTtlsPhase2Method, req_eap_set_ttls_phase2_method,
				rpc__resp__eap_set_ttls_phase2_method__init);

	RPC_RET_FAIL_IF(esp_eap_client_set_ttls_phase2_method(req_payload->type));

	return ESP_OK;
}

static esp_err_t req_eap_set_suiteb_certification(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetSuiteb192bitCertification, resp_eap_set_suiteb_certification,
				RpcReqEapSetSuiteb192bitCertification, req_eap_set_suiteb_certification,
				rpc__resp__eap_set_suiteb192bit_certification__init);

	RPC_RET_FAIL_IF(esp_eap_client_set_suiteb_192bit_certification(req_payload->enable));

	return ESP_OK;
}

static esp_err_t req_eap_set_pac_file(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetPacFile, resp_eap_set_pac_file,
				RpcReqEapSetPacFile, req_eap_set_pac_file,
				rpc__resp__eap_set_pac_file__init);

	RPC_RET_FAIL_IF(!req_payload->pac_file.data || req_payload->pac_file_len <= 0);
	const unsigned char *pac_file = req_payload->pac_file.data;
	int pac_file_len = req_payload->pac_file_len;

	RPC_RET_FAIL_IF(esp_eap_client_set_pac_file(pac_file, pac_file_len));

	return ESP_OK;
}

static esp_err_t req_eap_set_fast_params(Rpc *req, Rpc *resp, void *priv_data)
{
	esp_eap_fast_config fast_config = {0};

	RPC_TEMPLATE(RpcRespEapSetFastParams, resp_eap_set_fast_params,
				RpcReqEapSetFastParams, req_eap_set_fast_params,
				rpc__resp__eap_set_fast_params__init);

	fast_config.fast_provisioning = req_payload->eap_fast_config->fast_provisioning;
	fast_config.fast_max_pac_list_len = req_payload->eap_fast_config->fast_max_pac_list_len;
	fast_config.fast_pac_format_binary = req_payload->eap_fast_config->fast_pac_format_binary;

	RPC_RET_FAIL_IF(esp_eap_client_set_fast_params(fast_config));

	return ESP_OK;
}

static esp_err_t req_eap_use_default_cert_bundle(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapUseDefaultCertBundle, resp_eap_use_default_cert_bundle,
				RpcReqEapUseDefaultCertBundle, req_eap_use_default_cert_bundle,
				rpc__resp__eap_use_default_cert_bundle__init);

	RPC_RET_FAIL_IF(esp_eap_client_use_default_cert_bundle(req_payload->use_default_bundle));

	return ESP_OK;
}

#if H_GOT_EAP_OKC_SUPPORT
static esp_err_t req_wifi_set_okc_support(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetOkcSupport, resp_wifi_set_okc_support,
				RpcReqWifiSetOkcSupport, req_wifi_set_okc_support,
				rpc__resp__wifi_set_okc_support__init);

	esp_wifi_set_okc_support(req_payload->enable);
	return ESP_OK;
}
#endif

#if H_GOT_EAP_SET_DOMAIN_NAME
static esp_err_t req_eap_set_domain_name(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetDomainName, resp_eap_set_domain_name,
				RpcReqEapSetDomainName, req_eap_set_domain_name,
				rpc__resp__eap_set_domain_name__init);

	const char *domain_name = (const char *)req_payload->domain_name.data;
	RPC_RET_FAIL_IF(!req_payload->domain_name.data);

	RPC_RET_FAIL_IF(esp_eap_client_set_domain_name(domain_name));
	return ESP_OK;
}
#endif

#if H_GOT_SET_EAP_METHODS_API
static esp_err_t req_eap_set_eap_methods(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespEapSetEapMethods, resp_eap_set_eap_methods,
				RpcReqEapSetEapMethods, req_eap_set_eap_methods,
				rpc__resp__eap_set_eap_methods__init);

    RPC_RET_FAIL_IF(esp_eap_client_set_eap_methods(req_payload->methods));

	return ESP_OK;
}
#endif
#endif // H_WIFI_ENTERPRISE_SUPPORT

static esp_err_t req_get_coprocessor_fw_version(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespGetCoprocessorFwVersion, resp_get_coprocessor_fwversion,
			RpcReqGetCoprocessorFwVersion, req_get_coprocessor_fwversion,
			rpc__resp__get_coprocessor_fw_version__init);

	resp_payload->major1 = PROJECT_VERSION_MAJOR_1;
	resp_payload->minor1 = PROJECT_VERSION_MINOR_1;
	resp_payload->patch1 = PROJECT_VERSION_PATCH_1;
	resp_payload->resp = ESP_OK;

	return ESP_OK;
}

static esp_err_t req_iface_mac_addr_len_get(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespIfaceMacAddrLenGet, resp_iface_mac_addr_len_get,
			RpcReqIfaceMacAddrLenGet, req_iface_mac_addr_len_get,
			rpc__resp__iface_mac_addr_len_get__init);

	size_t len = esp_mac_addr_len_get(req_payload->type);

	resp_payload->type = req_payload->type;
	resp_payload->len = len;

	return ESP_OK;
}

static esp_err_t req_iface_mac_addr_set_get(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespIfaceMacAddrSetGet, resp_iface_mac_addr_set_get,
			RpcReqIfaceMacAddrSetGet, req_iface_mac_addr_set_get,
			rpc__resp__iface_mac_addr_set_get__init);

	// copy the incoming request to the outgoing response
	resp_payload->set = req_payload->set;
	resp_payload->type = req_payload->type;

	// get the expected len based on the type
	size_t len = esp_mac_addr_len_get(req_payload->type);

	if (req_payload->set) {
		// set the interface mac address
		if (req_payload->mac.len) {
			if (req_payload->mac.len == len) {
				RPC_RET_FAIL_IF(esp_iface_mac_addr_set(req_payload->mac.data, req_payload->type));
				// copy the mac address that was set in the response
				RPC_RESP_COPY_BYTES_SRC_UNCHECKED(resp_payload->mac, req_payload->mac.data, len);
			} else {
				ESP_LOGE(TAG, "expected mac length %" PRIu16 ", but got %" PRIu16, len, req_payload->mac.len);
				resp_payload->resp = ESP_ERR_INVALID_ARG;
			}
		} else {
			// no mac data provided
			ESP_LOGE(TAG, "error: set iface mac address without mac data");
			resp_payload->resp = ESP_ERR_INVALID_ARG;
		}
	} else {
		// get the interface mac address
		uint8_t iface_mac[IFACE_MAC_SIZE] = {0};
		RPC_RET_FAIL_IF(esp_read_mac(iface_mac, req_payload->type));

		RPC_RESP_COPY_BYTES_SRC_UNCHECKED(resp_payload->mac, iface_mac, len);
	}

	return ESP_OK;
}

static esp_err_t req_feature_control(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespFeatureControl, resp_feature_control,
			RpcReqFeatureControl, req_feature_control,
			rpc__resp__feature_control__init);

	// copy the incoming request to the outgoing response
	resp_payload->feature = req_payload->feature;
	resp_payload->command = req_payload->command;
	resp_payload->option  = req_payload->option;

	if (req_payload->feature == RPC_FEATURE__Feature_Bluetooth) {
		// decode the requested Bluetooth control
		switch (req_payload->command) {
		case RPC_FEATURE_COMMAND__Feature_Command_BT_Init:
			RPC_RET_FAIL_IF(init_bluetooth());
			break;
		case RPC_FEATURE_COMMAND__Feature_Command_BT_Deinit:
			bool mem_release = false;
			if (req_payload->option == RPC_FEATURE_OPTION__Feature_Option_BT_Deinit_Release_Memory) {
				mem_release = true;
			}
			RPC_RET_FAIL_IF(deinit_bluetooth(mem_release));
			break;
		case RPC_FEATURE_COMMAND__Feature_Command_BT_Enable:
			RPC_RET_FAIL_IF(enable_bluetooth());
			break;
		case RPC_FEATURE_COMMAND__Feature_Command_BT_Disable:
			RPC_RET_FAIL_IF(disable_bluetooth());
			break;
		default:
			// invalid Bluetooth control feature
			ESP_LOGE(TAG, "error: invalid Bluetooth Feature Control");
			resp_payload->resp = ESP_ERR_INVALID_ARG;
			break;
		}
	} else {
		// invalid feature
		ESP_LOGE(TAG, "error: invalid Feature Control");
		resp_payload->resp = ESP_ERR_INVALID_ARG;
	}
	return ESP_OK;
}

#if CONFIG_SOC_WIFI_HE_SUPPORT
static esp_err_t req_wifi_sta_twt_config(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiStaTwtConfig, resp_wifi_sta_twt_config,
			RpcReqWifiStaTwtConfig, req_wifi_sta_twt_config,
			rpc__resp__wifi_sta_twt_config__init);

	wifi_twt_config_t wifi_twt_config;
	wifi_twt_config.post_wakeup_event = req_payload->config->post_wakeup_event;
#if H_GOT_TWT_ENABLE_KEEP_ALIVE
	wifi_twt_config.twt_enable_keep_alive = req_payload->config->twt_enable_keep_alive;
#endif

	RPC_RET_FAIL_IF(esp_wifi_sta_twt_config(&wifi_twt_config));

	return ESP_OK;
}

static esp_err_t req_wifi_sta_itwt_setup(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_itwt_setup_config_t cfg = {0};

	RPC_TEMPLATE(RpcRespWifiStaItwtSetup, resp_wifi_sta_itwt_setup,
			RpcReqWifiStaItwtSetup, req_wifi_sta_itwt_setup,
			rpc__resp__wifi_sta_itwt_setup__init);

	wifi_itwt_setup_config_t * p_a_cfg = &cfg;
	WifiItwtSetupConfig *p_c_cfg = req_payload->setup_config;

	p_a_cfg->setup_cmd = p_c_cfg->setup_cmd;
	p_a_cfg->trigger = H_GET_BIT(WIFI_ITWT_CONFIG_1_trigger_BIT, p_c_cfg->bitmask_1);
	p_a_cfg->flow_type = H_GET_BIT(WIFI_ITWT_CONFIG_1_flow_type_BIT, p_c_cfg->bitmask_1);
	/* WIFI_ITWT_CONFIG_1_flow_id_BIT is three bits wide */
	p_a_cfg->flow_id = (p_c_cfg->bitmask_1 >> WIFI_ITWT_CONFIG_1_flow_id_BIT) & 0x07;
	/* WIFI_ITWT_CONFIG_1_wake_invl_expn_BIT is five bits wide */
	p_a_cfg->wake_invl_expn = (p_c_cfg->bitmask_1 >> WIFI_ITWT_CONFIG_1_wake_invl_expn_BIT) & 0x1F;
	p_a_cfg->wake_duration_unit = H_GET_BIT(WIFI_ITWT_CONFIG_1_wake_duration_unit_BIT, p_c_cfg->bitmask_1);
#if H_DECODE_WIFI_RESERVED_FIELD
	p_a_cfg->reserved = WIFI_ITWT_CONFIG_1_GET_RESERVED_VAL(p_c_cfg->bitmask_1);
#endif
	p_a_cfg->min_wake_dura = p_c_cfg->min_wake_dura;
	p_a_cfg->wake_invl_mant = p_c_cfg->wake_invl_mant;
	p_a_cfg->twt_id = p_c_cfg->twt_id;
	p_a_cfg->timeout_time_ms = p_c_cfg->timeout_time_ms;

	RPC_RET_FAIL_IF(esp_wifi_sta_itwt_setup(&cfg));

	return ESP_OK;
}

static esp_err_t req_wifi_sta_itwt_teardown(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiStaItwtTeardown, resp_wifi_sta_itwt_teardown,
			RpcReqWifiStaItwtTeardown, req_wifi_sta_itwt_teardown,
			rpc__resp__wifi_sta_itwt_teardown__init);

	int flow_id = req_payload->flow_id;

	RPC_RET_FAIL_IF(esp_wifi_sta_itwt_teardown(flow_id));

	return ESP_OK;
}

static esp_err_t req_wifi_sta_itwt_suspend(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiStaItwtSuspend, resp_wifi_sta_itwt_suspend,
			RpcReqWifiStaItwtSuspend, req_wifi_sta_itwt_suspend,
			rpc__resp__wifi_sta_itwt_suspend__init);

	int flow_id = req_payload->flow_id;
	int suspend_time_ms = req_payload->suspend_time_ms;

	RPC_RET_FAIL_IF(esp_wifi_sta_itwt_suspend(flow_id, suspend_time_ms));

	return ESP_OK;
}

static esp_err_t req_wifi_sta_itwt_get_flow_id_status(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaItwtGetFlowIdStatus, resp_wifi_sta_itwt_get_flow_id_status,
			RpcReqWifiStaItwtGetFlowIdStatus, req_wifi_sta_itwt_get_flow_id_status,
			rpc__resp__wifi_sta_itwt_get_flow_id_status__init);

	int flow_id_bitmap;

	RPC_RET_FAIL_IF(esp_wifi_sta_itwt_get_flow_id_status(&flow_id_bitmap));

	resp_payload->flow_id_bitmap = flow_id_bitmap;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_itwt_send_probe_req(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiStaItwtSendProbeReq, resp_wifi_sta_itwt_send_probe_req,
			RpcReqWifiStaItwtSendProbeReq, req_wifi_sta_itwt_send_probe_req,
			rpc__resp__wifi_sta_itwt_send_probe_req__init);

	int timeout_ms = req_payload->timeout_ms;

	RPC_RET_FAIL_IF(esp_wifi_sta_itwt_send_probe_req(timeout_ms));

	return ESP_OK;
}

static esp_err_t req_wifi_sta_itwt_set_target_wake_time_offset(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiStaItwtSetTargetWakeTimeOffset, resp_wifi_sta_itwt_set_target_wake_time_offset,
			RpcReqWifiStaItwtSetTargetWakeTimeOffset, req_wifi_sta_itwt_set_target_wake_time_offset,
			rpc__resp__wifi_sta_itwt_set_target_wake_time_offset__init);

	int offset_us = req_payload->offset_us;

	RPC_RET_FAIL_IF(esp_wifi_sta_itwt_set_target_wake_time_offset(offset_us));

	return ESP_OK;
}
#endif // CONFIG_SOC_WIFI_HE_SUPPORT

#if H_DPP_SUPPORT
void dpp_enrollee_event_cb(esp_supp_dpp_event_t event, void *data);

static esp_err_t req_supp_dpp_init(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespSuppDppInit, resp_supp_dpp_init,
			RpcReqSuppDppInit, req_supp_dpp_init,
			rpc__resp__supp_dpp_init__init);

	if (req_payload->cb) {
		// init with callback
		ESP_LOGI(TAG, "dpp init with callback");
		RPC_RET_FAIL_IF(esp_supp_dpp_init(dpp_enrollee_event_cb));
	} else {
		// init without callback
		ESP_LOGI(TAG, "dpp init WITHOUT callback");
		RPC_RET_FAIL_IF(esp_supp_dpp_init(NULL));
	}
	return ESP_OK;
}

static esp_err_t req_supp_dpp_deinit(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespSuppDppDeinit, resp_supp_dpp_deinit,
			RpcReqSuppDppDeinit, req_supp_dpp_deinit,
			rpc__resp__supp_dpp_deinit__init);

	RPC_RET_FAIL_IF(esp_supp_dpp_deinit());

	return ESP_OK;
}

static esp_err_t req_supp_dpp_bootstrap_gen(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespSuppDppBootstrapGen, resp_supp_dpp_bootstrap_gen,
			RpcReqSuppDppBootstrapGen, req_supp_dpp_bootstrap_gen,
			rpc__resp__supp_dpp_bootstrap_gen__init);

	const char *chan_list = NULL;
	esp_supp_dpp_bootstrap_t type;
	const char *key = NULL;
	const char *info = NULL;

	chan_list = (const char *)req_payload->chan_list.data;
	type = req_payload->type;
	if (req_payload->key.len) {
		key = (const char *)req_payload->key.data;
	}
	if (req_payload->info.len) {
		info = (const char *)req_payload->info.data;
	}

	RPC_RET_FAIL_IF(esp_supp_dpp_bootstrap_gen(chan_list, type, key, info));

	return ESP_OK;
}

static esp_err_t req_supp_dpp_start_listen(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespSuppDppStartListen, resp_supp_dpp_start_listen,
			RpcReqSuppDppStartListen, req_supp_dpp_start_listen,
			rpc__resp__supp_dpp_start_listen__init);

	RPC_RET_FAIL_IF(esp_supp_dpp_start_listen());

	return ESP_OK;
}

static esp_err_t req_supp_dpp_stop_listen(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespSuppDppStopListen, resp_supp_dpp_stop_listen,
			RpcReqSuppDppStopListen, req_supp_dpp_stop_listen,
			rpc__resp__supp_dpp_stop_listen__init);

	RPC_RET_FAIL_IF(esp_supp_dpp_stop_listen());

	return ESP_OK;
}
#endif

static esp_rpc_req_t req_table[] = {
	{
		.req_num = RPC_ID__Req_GetMACAddress ,
		.command_handler = req_wifi_get_mac
	},
	{
		.req_num = RPC_ID__Req_GetWifiMode,
		.command_handler = req_wifi_get_mode
	},
	{
		.req_num = RPC_ID__Req_SetWifiMode,
		.command_handler = req_wifi_set_mode
	},
	{
		.req_num = RPC_ID__Req_SetMacAddress,
		.command_handler = req_wifi_set_mac
	},
	{
		.req_num = RPC_ID__Req_WifiSetPs,
		.command_handler = req_wifi_set_ps
	},
	{
		.req_num = RPC_ID__Req_WifiGetPs,
		.command_handler = req_wifi_get_ps
	},
	{
		.req_num = RPC_ID__Req_OTABegin,
		.command_handler = req_ota_begin_handler
	},
	{
		.req_num = RPC_ID__Req_OTAWrite,
		.command_handler = req_ota_write_handler
	},
	{
		.req_num = RPC_ID__Req_OTAEnd,
		.command_handler = req_ota_end_handler
	},
	{
		.req_num = RPC_ID__Req_WifiSetMaxTxPower,
		.command_handler = req_wifi_set_max_tx_power
	},
	{
		.req_num = RPC_ID__Req_WifiGetMaxTxPower,
		.command_handler = req_wifi_get_max_tx_power
	},
	{
		.req_num = RPC_ID__Req_ConfigHeartbeat,
		.command_handler = req_config_heartbeat
	},
	{
		.req_num = RPC_ID__Req_WifiInit,
		.command_handler = req_wifi_init
	},
	{
		.req_num = RPC_ID__Req_WifiDeinit,
		.command_handler = req_wifi_deinit
	},
	{
		.req_num = RPC_ID__Req_WifiStart,
		.command_handler = req_wifi_start
	},
	{
		.req_num = RPC_ID__Req_WifiStop,
		.command_handler = req_wifi_stop
	},
	{
		.req_num = RPC_ID__Req_WifiConnect,
		.command_handler = req_wifi_connect
	},
	{
		.req_num = RPC_ID__Req_WifiDisconnect,
		.command_handler = req_wifi_disconnect
	},
	{
		.req_num = RPC_ID__Req_WifiSetConfig,
		.command_handler = req_wifi_set_config
	},
	{
		.req_num = RPC_ID__Req_WifiGetConfig,
		.command_handler = req_wifi_get_config
	},
	{
		.req_num = RPC_ID__Req_WifiScanStart,
		.command_handler = req_wifi_scan_start
	},
	{
		.req_num = RPC_ID__Req_WifiScanStop,
		.command_handler = req_wifi_scan_stop
	},
	{
		.req_num = RPC_ID__Req_WifiScanGetApNum,
		.command_handler = req_wifi_scan_get_ap_num
	},
	{
		.req_num = RPC_ID__Req_WifiScanGetApRecord,
		.command_handler = req_wifi_scan_get_ap_record
	},
	{
		.req_num = RPC_ID__Req_WifiScanGetApRecords,
		.command_handler = req_wifi_scan_get_ap_records
	},
	{
		.req_num = RPC_ID__Req_WifiClearApList,
		.command_handler = req_wifi_clear_ap_list
	},
	{
		.req_num = RPC_ID__Req_WifiRestore,
		.command_handler = req_wifi_restore
	},
	{
		.req_num = RPC_ID__Req_WifiClearFastConnect,
		.command_handler = req_wifi_clear_fast_connect
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetApInfo,
		.command_handler = req_wifi_sta_get_ap_info
	},
	{
		.req_num = RPC_ID__Req_WifiDeauthSta,
		.command_handler = req_wifi_deauth_sta
	},
	{
		.req_num = RPC_ID__Req_WifiSetStorage,
		.command_handler = req_wifi_set_storage
	},
	{
		.req_num = RPC_ID__Req_WifiSetProtocol,
		.command_handler = req_wifi_set_protocol
	},
	{
		.req_num = RPC_ID__Req_WifiGetProtocol,
		.command_handler = req_wifi_get_protocol
	},
	{
		.req_num = RPC_ID__Req_WifiSetBandwidth,
		.command_handler = req_wifi_set_bandwidth
	},
	{
		.req_num = RPC_ID__Req_WifiGetBandwidth,
		.command_handler = req_wifi_get_bandwidth
	},
	{
		.req_num = RPC_ID__Req_WifiSetChannel,
		.command_handler = req_wifi_set_channel
	},
	{
		.req_num = RPC_ID__Req_WifiGetChannel,
		.command_handler = req_wifi_get_channel
	},
	{
		.req_num = RPC_ID__Req_WifiSetCountryCode,
		.command_handler = req_wifi_set_country_code
	},
	{
		.req_num = RPC_ID__Req_WifiGetCountryCode,
		.command_handler = req_wifi_get_country_code
	},
	{
		.req_num = RPC_ID__Req_WifiSetCountry,
		.command_handler = req_wifi_set_country
	},
	{
		.req_num = RPC_ID__Req_WifiGetCountry,
		.command_handler = req_wifi_get_country
	},
	{
		.req_num = RPC_ID__Req_WifiApGetStaList,
		.command_handler = req_wifi_ap_get_sta_list
	},
	{
		.req_num = RPC_ID__Req_WifiApGetStaAid,
		.command_handler = req_wifi_ap_get_sta_aid
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetRssi,
		.command_handler = req_wifi_sta_get_rssi
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetAid,
		.command_handler = req_wifi_sta_get_aid
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetNegotiatedPhymode,
		.command_handler = req_wifi_sta_get_negotiated_phymode
	},
#if H_PRESENT_IN_ESP_IDF_5_4_0
	{
		.req_num = RPC_ID__Req_WifiSetProtocols,
		.command_handler = req_wifi_set_protocols
	},
	{
		.req_num = RPC_ID__Req_WifiGetProtocols,
		.command_handler = req_wifi_get_protocols
	},
	{
		.req_num = RPC_ID__Req_WifiSetBandwidths,
		.command_handler = req_wifi_set_bandwidths
	},
	{
		.req_num = RPC_ID__Req_WifiGetBandwidths,
		.command_handler = req_wifi_get_bandwidths
	},
	{
		.req_num = RPC_ID__Req_WifiSetBand,
		.command_handler = req_wifi_set_band
	},
	{
		.req_num = RPC_ID__Req_WifiGetBand,
		.command_handler = req_wifi_get_band
	},
	{
		.req_num = RPC_ID__Req_WifiSetBandMode,
		.command_handler = req_wifi_set_band_mode
	},
	{
		.req_num = RPC_ID__Req_WifiGetBandMode,
		.command_handler = req_wifi_get_band_mode
	},
#endif
	{
		.req_num = RPC_ID__Req_WifiSetInactiveTime,
		.command_handler = req_wifi_set_inactive_time
	},
	{
		.req_num = RPC_ID__Req_WifiGetInactiveTime,
		.command_handler = req_wifi_get_inactive_time
	},
	{
		.req_num = RPC_ID__Req_GetCoprocessorFwVersion,
		.command_handler = req_get_coprocessor_fw_version
	},
	{
		.req_num = RPC_ID__Req_SetDhcpDnsStatus,
		.command_handler = req_set_dhcp_dns_status
	},
	{
		.req_num = RPC_ID__Req_GetDhcpDnsStatus,
		.command_handler = req_get_dhcp_dns_status
	},
#if CONFIG_SOC_WIFI_HE_SUPPORT
	{
		.req_num = RPC_ID__Req_WifiStaTwtConfig,
		.command_handler = req_wifi_sta_twt_config
	},
	{
		.req_num = RPC_ID__Req_WifiStaItwtSetup,
		.command_handler = req_wifi_sta_itwt_setup
	},
	{
		.req_num = RPC_ID__Req_WifiStaItwtTeardown,
		.command_handler = req_wifi_sta_itwt_teardown
	},
	{
		.req_num = RPC_ID__Req_WifiStaItwtSuspend,
		.command_handler = req_wifi_sta_itwt_suspend
	},
	{
		.req_num = RPC_ID__Req_WifiStaItwtGetFlowIdStatus,
		.command_handler = req_wifi_sta_itwt_get_flow_id_status
	},
	{
		.req_num = RPC_ID__Req_WifiStaItwtSendProbeReq,
		.command_handler = req_wifi_sta_itwt_send_probe_req
	},
	{
		.req_num = RPC_ID__Req_WifiStaItwtSetTargetWakeTimeOffset,
		.command_handler = req_wifi_sta_itwt_set_target_wake_time_offset
	},
#endif // CONFIG_SOC_WIFI_HE_SUPPORT
#if H_WIFI_ENTERPRISE_SUPPORT
	{
		.req_num = RPC_ID__Req_WifiStaEnterpriseEnable,
		.command_handler = req_wifi_sta_enterprise_enable
	},
	{
		.req_num = RPC_ID__Req_WifiStaEnterpriseDisable,
		.command_handler = req_wifi_sta_enterprise_disable
	},
	{
		.req_num = RPC_ID__Req_EapSetIdentity,
		.command_handler = req_eap_set_identity
	},
	{
		.req_num = RPC_ID__Req_EapClearIdentity,
		.command_handler = req_eap_clear_identity
	},
	{
		.req_num = RPC_ID__Req_EapSetUsername,
		.command_handler = req_eap_set_username
	},
	{
		.req_num = RPC_ID__Req_EapClearUsername,
		.command_handler = req_eap_clear_username
	},
	{
		.req_num = RPC_ID__Req_EapSetPassword,
		.command_handler = req_eap_set_password
	},
	{
		.req_num = RPC_ID__Req_EapClearPassword,
		.command_handler = req_eap_clear_password
	},
	{
		.req_num = RPC_ID__Req_EapSetNewPassword,
		.command_handler = req_eap_set_new_password
	},
	{
		.req_num = RPC_ID__Req_EapClearNewPassword,
		.command_handler = req_eap_clear_new_password
	},
	{
		.req_num = RPC_ID__Req_EapSetCaCert,
		.command_handler = req_eap_set_ca_cert
	},
	{
		.req_num = RPC_ID__Req_EapClearCaCert,
		.command_handler = req_eap_clear_ca_cert
	},
	{
		.req_num = RPC_ID__Req_EapSetCertificateAndKey,
		.command_handler = req_eap_set_certificate_and_key
	},
	{
		.req_num = RPC_ID__Req_EapClearCertificateAndKey,
		.command_handler = req_eap_clear_certificate_and_key
	},
	{
		.req_num = RPC_ID__Req_EapGetDisableTimeCheck,
		.command_handler = req_eap_get_disable_time_check
	},
	{
		.req_num = RPC_ID__Req_EapSetTtlsPhase2Method,
		.command_handler = req_eap_set_ttls_phase2_method
	},
	{
		.req_num = RPC_ID__Req_EapSetSuitebCertification,
		.command_handler = req_eap_set_suiteb_certification
	},
	{
		.req_num = RPC_ID__Req_EapSetPacFile,
		.command_handler = req_eap_set_pac_file
	},
	{
		.req_num = RPC_ID__Req_EapSetFastParams,
		.command_handler = req_eap_set_fast_params
	},
	{
		.req_num = RPC_ID__Req_EapUseDefaultCertBundle,
		.command_handler = req_eap_use_default_cert_bundle
	},
#if H_GOT_EAP_OKC_SUPPORT
	{
		.req_num = RPC_ID__Req_WifiSetOkcSupport,
		.command_handler = req_wifi_set_okc_support
	},
#endif
#if H_GOT_EAP_SET_DOMAIN_NAME
	{
		.req_num = RPC_ID__Req_EapSetDomainName,
		.command_handler = req_eap_set_domain_name
	},
#endif
	{
		.req_num = RPC_ID__Req_EapSetDisableTimeCheck,
		.command_handler = req_eap_set_disable_time_check
	},
#if H_GOT_SET_EAP_METHODS_API
	{
		.req_num = RPC_ID__Req_EapSetEapMethods,
		.command_handler = req_eap_set_eap_methods
	},
#endif
#endif
#if H_DPP_SUPPORT
	{
		.req_num = RPC_ID__Req_SuppDppInit,
		.command_handler = req_supp_dpp_init
	},
	{
		.req_num = RPC_ID__Req_SuppDppDeinit,
		.command_handler = req_supp_dpp_deinit
	},
	{
		.req_num = RPC_ID__Req_SuppDppDeinit,
		.command_handler = req_supp_dpp_deinit
	},
	{
		.req_num = RPC_ID__Req_SuppDppBootstrapGen,
		.command_handler = req_supp_dpp_bootstrap_gen
	},
	{
		.req_num = RPC_ID__Req_SuppDppStartListen,
		.command_handler = req_supp_dpp_start_listen,
	},
	{
		.req_num = RPC_ID__Req_SuppDppStopListen,
		.command_handler = req_supp_dpp_stop_listen,
	},
#endif
	{
		.req_num = RPC_ID__Req_IfaceMacAddrSetGet,
		.command_handler = req_iface_mac_addr_set_get
	},
	{
		.req_num = RPC_ID__Req_IfaceMacAddrLenGet,
		.command_handler = req_iface_mac_addr_len_get
	},
	{
		.req_num = RPC_ID__Req_FeatureControl,
		.command_handler = req_feature_control
	},
};


static int lookup_req_handler(int req_id)
{
	for (int i = 0; i < sizeof(req_table)/sizeof(esp_rpc_req_t); i++) {
		if (req_table[i].req_num == req_id) {
			return i;
		}
	}
	return -1;
}

static esp_err_t esp_rpc_command_dispatcher(
		Rpc *req, Rpc *resp,
		void *priv_data)
{
	esp_err_t ret = ESP_OK;
	int req_index = 0;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters in command");
		return ESP_FAIL;
	}

	if ((req->msg_id <= RPC_ID__Req_Base) ||
		(req->msg_id >= RPC_ID__Req_Max)) {
		ESP_LOGE(TAG, "Invalid command request lookup");
		goto err_not_supported;
	}

	ESP_LOGI(TAG, "Received Req [0x%x]", req->msg_id);

	req_index = lookup_req_handler(req->msg_id);
	if (req_index < 0) {
		ESP_LOGE(TAG, "Invalid command handler lookup");
		goto err_not_supported;
	}

	ret = req_table[req_index].command_handler(req, resp, priv_data);
	if (ret) {
		ESP_LOGE(TAG, "Error executing command handler");
		goto err_cmd_error;
	}

	return ESP_OK;
 err_not_supported:
	// response ID Resp_Base means RPC Request was not supported
	resp->msg_id = RPC_ID__Resp_Base;
 err_cmd_error:
	return ESP_OK;
}

/* use rpc__free_unpacked to free memory
 * For RPC structure to be freed correctly with no memory leaks:
 * - n_xxx must be set to number of 'repeated xxx' structures in RPC msg
 * - xxx_case must be set for 'oneof xxx' structures in RPC msg
 * - xxx.len must be set for 'bytes xxx' or 'string xxx' in RPC msg
 */
static void esp_rpc_cleanup(Rpc *resp)
{
	if (resp) {
		rpc__free_unpacked(resp, NULL);
	}
}

esp_err_t data_transfer_handler(uint32_t session_id,const uint8_t *inbuf,
		ssize_t inlen, uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
	Rpc *req = NULL;
	esp_err_t ret = ESP_OK;

	Rpc *resp = (Rpc *)calloc(1, sizeof(Rpc)); // resp deallocated in esp_rpc_cleanup()
	if (!resp) {
		ESP_LOGE(TAG, "%s calloc failed", __func__);
		return ESP_FAIL;
	}

	if (!inbuf || !outbuf || !outlen) {
		ESP_LOGE(TAG,"Buffers are NULL");
		return ESP_FAIL;
	}

	req = rpc__unpack(NULL, inlen, inbuf);
	if (!req) {
		ESP_LOGE(TAG, "Unable to unpack config data");
		return ESP_FAIL;
	}

	rpc__init (resp);
	resp->msg_type = RPC_TYPE__Resp;
	resp->msg_id = req->msg_id - RPC_ID__Req_Base + RPC_ID__Resp_Base;
	resp->uid = req->uid;
	resp->payload_case = resp->msg_id;
	ESP_LOGI(TAG, "Resp_MSGId for req[0x%x] is [0x%x], uid %ld", req->msg_id, resp->msg_id, resp->uid);
	ret = esp_rpc_command_dispatcher(req,resp,NULL);
	if (ret) {
		ESP_LOGE(TAG, "Command dispatching not happening");
		goto err;
	}

	rpc__free_unpacked(req, NULL);

	*outlen = rpc__get_packed_size (resp);
	if (*outlen <= 0) {
		ESP_LOGE(TAG, "Invalid encoding for response");
		goto err;
	}

	// ESP_LOGE(TAG, "len %" PRIi16, *outlen);

	*outbuf = (uint8_t *)calloc(1, *outlen);
	if (!*outbuf) {
		ESP_LOGE(TAG, "No memory allocated for outbuf");
		esp_rpc_cleanup(resp);
		return ESP_ERR_NO_MEM;
	}

	rpc__pack (resp, *outbuf);

	//printf("Resp outbuf:\n");
	//ESP_LOG_BUFFER_HEXDUMP("Resp outbuf", *outbuf, *outlen, ESP_LOG_INFO);

	esp_rpc_cleanup(resp);
	return ESP_OK;

err:
	esp_rpc_cleanup(resp);
	return ESP_FAIL;
}

/* Function ESPInit Notification */
static esp_err_t rpc_evt_ESPInit(Rpc *ntfy)
{
	RpcEventESPInit *ntfy_payload = NULL;

	ESP_LOGI(TAG,"event ESPInit");
	ntfy_payload = (RpcEventESPInit *)
		calloc(1,sizeof(RpcEventESPInit));
	if (!ntfy_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__event__espinit__init(ntfy_payload);
	ntfy->payload_case = RPC__PAYLOAD_EVENT_ESP_INIT;
	ntfy->event_esp_init = ntfy_payload;

	return ESP_OK;
}

static esp_err_t rpc_evt_heartbeat(Rpc *ntfy)
{
	RpcEventHeartbeat *ntfy_payload = NULL;


	ntfy_payload = (RpcEventHeartbeat*)
		calloc(1,sizeof(RpcEventHeartbeat));
	if (!ntfy_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__event__heartbeat__init(ntfy_payload);

	ntfy_payload->hb_num = hb_num;

	ntfy->payload_case = RPC__PAYLOAD_EVENT_HEARTBEAT;
	ntfy->event_heartbeat = ntfy_payload;

	return ESP_OK;

}

static esp_err_t rpc_evt_sta_scan_done(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	WifiEventStaScanDone *p_c_scan = NULL;
	wifi_event_sta_scan_done_t * p_a = (wifi_event_sta_scan_done_t*)data;

	NTFY_TEMPLATE(RPC_ID__Event_StaScanDone,
			RpcEventStaScanDone, event_sta_scan_done,
			rpc__event__sta_scan_done__init);

	NTFY_ALLOC_ELEMENT(WifiEventStaScanDone, ntfy_payload->scan_done,
			wifi_event_sta_scan_done__init);
	p_c_scan = ntfy_payload->scan_done;

	p_c_scan->status = p_a->status;
	p_c_scan->number = p_a->number;
	p_c_scan->scan_id = p_a->scan_id;

err:
	return ESP_OK;
}

static esp_err_t rpc_evt_sta_connected(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	WifiEventStaConnected *p_c = NULL;
	wifi_event_sta_connected_t * p_a = (wifi_event_sta_connected_t*)data;

	NTFY_TEMPLATE(RPC_ID__Event_StaConnected,
			RpcEventStaConnected, event_sta_connected,
			rpc__event__sta_connected__init);

	NTFY_ALLOC_ELEMENT(WifiEventStaConnected, ntfy_payload->sta_connected,
			wifi_event_sta_connected__init);

	p_c = ntfy_payload->sta_connected;

	NTFY_COPY_BYTES(p_c->ssid, p_a->ssid, sizeof(p_a->ssid));

	p_c->ssid_len = p_a->ssid_len;

	NTFY_COPY_BYTES(p_c->bssid, p_a->bssid, sizeof(p_a->bssid));

	p_c->channel = p_a->channel;
	p_c->authmode = p_a->authmode;
	p_c->aid = p_a->aid;

err:
	return ESP_OK;
}

static esp_err_t rpc_evt_sta_disconnected(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	WifiEventStaDisconnected *p_c = NULL;
	wifi_event_sta_disconnected_t * p_a = (wifi_event_sta_disconnected_t*)data;

	NTFY_TEMPLATE(RPC_ID__Event_StaDisconnected,
			RpcEventStaDisconnected, event_sta_disconnected,
			rpc__event__sta_disconnected__init);

	NTFY_ALLOC_ELEMENT(WifiEventStaDisconnected, ntfy_payload->sta_disconnected,
			wifi_event_sta_disconnected__init);

	p_c = ntfy_payload->sta_disconnected;

	NTFY_COPY_BYTES(p_c->ssid, p_a->ssid, sizeof(p_a->ssid));

	p_c->ssid_len = p_a->ssid_len;

	NTFY_COPY_BYTES(p_c->bssid, p_a->bssid, sizeof(p_a->bssid));

	p_c->reason = p_a->reason;
	p_c->rssi = p_a->rssi;

err:
	return ESP_OK;
}

static esp_err_t rpc_evt_ap_staconn_conn_disconn(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	ESP_LOGD(TAG, "%s event:%u",__func__,event_id);

	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		NTFY_TEMPLATE(RPC_ID__Event_AP_StaConnected,
				RpcEventAPStaConnected, event_ap_sta_connected,
				rpc__event__ap__sta_connected__init);

		wifi_event_ap_staconnected_t * p_a = (wifi_event_ap_staconnected_t *)data;

		NTFY_COPY_BYTES(ntfy_payload->mac, p_a->mac, sizeof(p_a->mac));

		ntfy_payload->aid = p_a->aid;
		ntfy_payload->is_mesh_child = p_a->is_mesh_child;

		return ESP_OK;

	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		NTFY_TEMPLATE(RPC_ID__Event_AP_StaDisconnected,
				RpcEventAPStaDisconnected, event_ap_sta_disconnected,
				rpc__event__ap__sta_disconnected__init);

		wifi_event_ap_stadisconnected_t * p_a = (wifi_event_ap_stadisconnected_t *)data;

		NTFY_COPY_BYTES(ntfy_payload->mac, p_a->mac, sizeof(p_a->mac));

		ntfy_payload->aid = p_a->aid;
		ntfy_payload->is_mesh_child = p_a->is_mesh_child;
		ntfy_payload->reason = p_a->reason;

		return ESP_OK;
	}
	return ESP_FAIL;
}

#if CONFIG_SOC_WIFI_HE_SUPPORT
static esp_err_t rpc_evt_itwt_setup(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	wifi_event_sta_itwt_setup_t *p_a = (wifi_event_sta_itwt_setup_t*)data;
	RpcEventStaItwtSetup *p_c = NULL;

	ESP_LOGI(TAG, "%s event:%u",__func__,event_id);

	NTFY_TEMPLATE(RPC_ID__Event_StaItwtSetup,
			RpcEventStaItwtSetup, event_sta_itwt_setup,
			rpc__event__sta_itwt_setup__init);

	NTFY_ALLOC_ELEMENT(WifiItwtSetupConfig, ntfy_payload->config,
			wifi_itwt_setup_config__init);

	p_c = ntfy_payload;

	p_c->config->setup_cmd = p_a->config.setup_cmd;

	if (p_a->config.trigger)
		H_SET_BIT(WIFI_ITWT_CONFIG_1_trigger_BIT, p_c->config->bitmask_1);

	if (p_a->config.flow_type)
		H_SET_BIT(WIFI_ITWT_CONFIG_1_flow_type_BIT, p_c->config->bitmask_1);

	/* WIFI_ITWT_CONFIG_1_flow_id_BIT is three bits wide */
	if (p_a->config.flow_id & 0x07)
		p_c->config->bitmask_1 |= (p_a->config.flow_id & 0x07) << WIFI_ITWT_CONFIG_1_flow_id_BIT;

	/* WIFI_ITWT_CONFIG_1_wake_invl_expn_BIT is five bits wide */
	if (p_a->config.wake_invl_expn & 0x1F)
		p_c->config->bitmask_1 |= (p_a->config.wake_invl_expn & 0x1F) << WIFI_ITWT_CONFIG_1_wake_invl_expn_BIT;

	if (p_a->config.wake_duration_unit)
		H_SET_BIT(WIFI_ITWT_CONFIG_1_wake_duration_unit_BIT, p_c->config->bitmask_1);

#if H_DECODE_WIFI_RESERVED_FIELD
	WIFI_ITWT_CONFIG_1_SET_RESERVED_VAL(p_a->config.reserved, p_c->config->bitmask_1)
#endif

	p_c->config->min_wake_dura = p_a->config.min_wake_dura;
	p_c->config->wake_invl_mant = p_a->config.wake_invl_mant;
	p_c->config->twt_id = p_a->config.twt_id;
	p_c->config->timeout_time_ms = p_a->config.timeout_time_ms;
	p_c->status = p_a->status;
	p_c->reason = p_a->reason;
	p_c->target_wake_time = p_a->target_wake_time;

 err:
	return ESP_OK;
}

static esp_err_t rpc_evt_itwt_teardown(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	wifi_event_sta_itwt_teardown_t *p_a = (wifi_event_sta_itwt_teardown_t*)data;
	RpcEventStaItwtTeardown *p_c = NULL;

	ESP_LOGI(TAG, "%s event:%u",__func__,event_id);

	NTFY_TEMPLATE(RPC_ID__Event_StaItwtTeardown,
			RpcEventStaItwtTeardown, event_sta_itwt_teardown,
			rpc__event__sta_itwt_teardown__init);

	p_c = ntfy_payload;

	p_c->flow_id = p_a->flow_id;
	p_c->status = p_a->status;

	return ESP_OK;
}

static esp_err_t rpc_evt_itwt_suspend(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	wifi_event_sta_itwt_suspend_t *p_a = (wifi_event_sta_itwt_suspend_t*)data;
	RpcEventStaItwtSuspend *p_c = NULL;
	int i;
	int num_elements = sizeof(p_a->actual_suspend_time_ms) / sizeof(p_a->actual_suspend_time_ms[0]);

	ESP_LOGI(TAG, "%s event:%u",__func__,event_id);

	NTFY_TEMPLATE(RPC_ID__Event_StaItwtSuspend,
			RpcEventStaItwtSuspend, event_sta_itwt_suspend,
			rpc__event__sta_itwt_suspend__init);

	p_c = ntfy_payload;

	p_c->status = p_a->status;
	p_c->flow_id_bitmap = p_a->flow_id_bitmap;

	p_c->actual_suspend_time_ms = calloc(num_elements, sizeof(p_a->actual_suspend_time_ms[0]));
	if (!p_c->actual_suspend_time_ms) {
		ESP_LOGE(TAG,"resp: malloc failed for ntfy_payload->actual_suspend_time_ms");
        ntfy_payload->resp = RPC_ERR_MEMORY_FAILURE;                         \
		goto err;
	}

	for (i = 0; i < num_elements; i++) {
		p_c->actual_suspend_time_ms[i] = p_a->actual_suspend_time_ms[i];
	}
	p_c->n_actual_suspend_time_ms = num_elements;
 err:
	return ESP_OK;
}

static esp_err_t rpc_evt_itwt_probe(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	wifi_event_sta_itwt_probe_t *p_a = (wifi_event_sta_itwt_probe_t*)data;
	RpcEventStaItwtProbe *p_c = NULL;

	ESP_LOGI(TAG, "%s event:%u",__func__,event_id);

	NTFY_TEMPLATE(RPC_ID__Event_StaItwtProbe,
			RpcEventStaItwtProbe, event_sta_itwt_probe,
			rpc__event__sta_itwt_probe__init);

	p_c = ntfy_payload;

	p_c->status = p_a->status;
	p_c->reason = p_a->reason;

	return ESP_OK;
}
#endif

static esp_err_t rpc_evt_Event_WifiEventNoArgs(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	NTFY_TEMPLATE(RPC_ID__Event_WifiEventNoArgs,
			RpcEventWifiEventNoArgs, event_wifi_event_no_args,
			rpc__event__wifi_event_no_args__init);

	int32_t event_id = (int32_t)*data;
	ESP_LOGI(TAG, "Sending Wi-Fi event [%ld]", event_id);

	ntfy_payload->event_id = event_id;

	ntfy_payload->resp = SUCCESS;
	return ESP_OK;
}
#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
static esp_err_t rpc_evt_Event_DhcpDnsStatus(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	NTFY_TEMPLATE(RPC_ID__Event_DhcpDnsStatus,
			RpcEventDhcpDnsStatus, event_dhcp_dns,
			rpc__event__dhcp_dns_status__init);

	rpc_dhcp_dns_status_t * p_a = (rpc_dhcp_dns_status_t*)data;

	ntfy_payload->iface = p_a->iface;
	ntfy_payload->net_link_up = p_a->net_link_up;
	ntfy_payload->dhcp_up = p_a->dhcp_up;
	ntfy_payload->dns_up = p_a->dns_up;
	ntfy_payload->dns_type = p_a->dns_type;

	NTFY_COPY_BYTES(ntfy_payload->dhcp_ip, p_a->dhcp_ip, sizeof(p_a->dhcp_ip));
	NTFY_COPY_BYTES(ntfy_payload->dhcp_nm, p_a->dhcp_nm, sizeof(p_a->dhcp_nm));
	NTFY_COPY_BYTES(ntfy_payload->dhcp_gw, p_a->dhcp_gw, sizeof(p_a->dhcp_gw));
	NTFY_COPY_BYTES(ntfy_payload->dns_ip, p_a->dns_ip, sizeof(p_a->dns_ip));

	ntfy_payload->resp = SUCCESS;
	return ESP_OK;
}
#endif

#if H_DPP_SUPPORT
void dpp_enrollee_event_cb(esp_supp_dpp_event_t event, void *data)
{
#if H_SUPP_DPP_SUPPORT
	switch (event) {
	case ESP_SUPP_DPP_URI_READY:
		if (data != NULL) {
			// data is a URI to be passed back to host
			int len = strlen(data) + 1; // include terminating null in data
			send_event_data_to_host(RPC_ID__Event_SuppDppUriReady, data, len);
		} else {
			ESP_LOGE(TAG, "ESP_SUPP_DPP_URI_READY with no URI data");
		}
		break;
	case ESP_SUPP_DPP_CFG_RECVD:
		if (data != NULL) {
			// pass wifi config back to host
			send_event_data_to_host(RPC_ID__Event_SuppDppCfgRecvd, data, sizeof(wifi_config_t));
		} else {
			ESP_LOGE(TAG, "ESP_SUPP_DPP_URICFG_RECVD with no wifi config data");
		}
		break;
	case ESP_SUPP_DPP_FAIL:
		{
			// reason code is given, not pointer to reason code
			// see https://github.com/espressif/esp-idf/blob/7912b04e6bdf8c9aeea88baff9e46794d04e4200/examples/wifi/wifi_easy_connect/dpp-enrollee/main/dpp_enrollee_main.c#L96
			int reason = (int)data;
			send_event_data_to_host(RPC_ID__Event_SuppDppFail, &reason, sizeof(int));
		}
		break;
	default:
		{
			ESP_LOGE(TAG, "Unknown ESP_SUPP event");
		}
		break;
	}
#else // H_SUPP_DPP_SUPPORT
	ESP_LOGW(TAG, "DPP Supplicant Callback not supported: ignoring event");
#endif // H_SUPP_DPP_SUPPORT
}

static esp_err_t rpc_evt_supp_dpp_uri_ready(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	uint8_t *uri = (uint8_t *)data;
	int uri_len = len;

	NTFY_TEMPLATE(RPC_ID__Event_SuppDppUriReady,
			RpcEventSuppDppUriReady, event_supp_dpp_uri_ready,
			rpc__event__supp_dpp_uri_ready__init);

	NTFY_COPY_BYTES(ntfy_payload->qrcode, uri, uri_len);

	ntfy_payload->resp = SUCCESS;
	return ESP_OK;
}

static esp_err_t rpc_evt_supp_dpp_cfg_recvd(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	wifi_config_t *config = (wifi_config_t *)data;
	wifi_sta_config_t *sta_config = &(config->sta);
	esp_err_t res;

	NTFY_TEMPLATE(RPC_ID__Event_SuppDppCfgRecvd,
			RpcEventSuppDppCfgRecvd, event_supp_dpp_cfg_recvd,
			rpc__event__supp_dpp_cfg_recvd__init);
	NTFY_ALLOC_ELEMENT(WifiConfig, ntfy_payload->cfg, wifi_config__init);
	ntfy_payload->cfg->u_case = WIFI_CONFIG__U_STA;

	res = copy_wifi_sta_cfg_to_rpc_struct(ntfy_payload,
			PAYLOAD_TYPE_RPC_EVENT_SUPP_DPP_GET_CONFIG, sta_config);

	if (res == ESP_OK) {
		ntfy_payload->resp = SUCCESS;
		return ESP_OK;
	} else {
		ESP_LOGE(TAG, "NTFY: copy_wifi_sta_cfg_to_rpc_struct() FAILED");
		return res;
	}
 err:
	return ESP_FAIL;
}

static esp_err_t rpc_evt_supp_dpp_fail(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	NTFY_TEMPLATE(RPC_ID__Event_SuppDppFail,
			RpcEventSuppDppFail, event_supp_dpp_fail,
			rpc__event__supp_dpp_fail__init);

	int *reason = (int *)data;

	ntfy_payload->reason = *reason;
	ntfy_payload->resp = SUCCESS;
	return ESP_OK;
}

#if H_WIFI_DPP_SUPPORT
static esp_err_t rpc_evt_wifi_dpp_uri_ready(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	wifi_event_dpp_uri_ready_t *uri = (wifi_event_dpp_uri_ready_t *)data;
	int uri_len = uri->uri_data_len;

	NTFY_TEMPLATE(RPC_ID__Event_WifiDppUriReady,
			RpcEventWifiDppUriReady, event_wifi_dpp_uri_ready,
			rpc__event__wifi_dpp_uri_ready__init);

	NTFY_COPY_BYTES(ntfy_payload->qrcode, uri->uri, uri_len);

	ntfy_payload->resp = SUCCESS;
	return ESP_OK;
}

static esp_err_t rpc_evt_wifi_dpp_cfg_recvd(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	wifi_config_t *config = (wifi_config_t *)data;
	wifi_sta_config_t *sta_config = &(config->sta);
	esp_err_t res;

	NTFY_TEMPLATE(RPC_ID__Event_WifiDppCfgRecvd,
			RpcEventWifiDppCfgRecvd, event_wifi_dpp_cfg_recvd,
			rpc__event__wifi_dpp_cfg_recvd__init);
	NTFY_ALLOC_ELEMENT(WifiConfig, ntfy_payload->cfg, wifi_config__init);
	ntfy_payload->cfg->u_case = WIFI_CONFIG__U_STA;

	res = copy_wifi_sta_cfg_to_rpc_struct(ntfy_payload,
			PAYLOAD_TYPE_RPC_EVENT_WIFI_DPP_GET_CONFIG, sta_config);

	if (res == ESP_OK) {
		ntfy_payload->resp = SUCCESS;
		return ESP_OK;
	} else {
		ESP_LOGE(TAG, "NTFY: copy_wifi_sta_cfg_to_rpc_struct() FAILED");
		return res;
	}
 err:
	return ESP_FAIL;
}

static esp_err_t rpc_evt_wifi_dpp_fail(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	NTFY_TEMPLATE(RPC_ID__Event_WifiDppFail,
			RpcEventWifiDppFail, event_wifi_dpp_fail,
			rpc__event__wifi_dpp_fail__init);

	int *reason = (int *)data;

	ntfy_payload->reason = *reason;
	ntfy_payload->resp = SUCCESS;
	return ESP_OK;
}
#endif // H_WIFI_DPP_SUPPORT
#endif // H_DPP_SUPPORT

esp_err_t rpc_evt_handler(uint32_t session_id,const uint8_t *inbuf,
		ssize_t inlen, uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
	int ret = SUCCESS;

	Rpc *ntfy = (Rpc *)calloc(1, sizeof(Rpc)); // ntfy deallocated in esp_rpc_cleanup()
	if (!ntfy) {
		ESP_LOGE(TAG, "%s calloc failed", __func__);
		return ESP_FAIL;
	}

	if (!outbuf || !outlen) {
		ESP_LOGE(TAG,"Buffers are NULL");
		return ESP_FAIL;
	}

	rpc__init (ntfy);
	ntfy->msg_id = session_id;
	ntfy->msg_type = RPC_TYPE__Event;

	switch ((int)ntfy->msg_id) {
		case RPC_ID__Event_ESPInit : {
			ret = rpc_evt_ESPInit(ntfy);
			break;
		} case RPC_ID__Event_Heartbeat: {
			ret = rpc_evt_heartbeat(ntfy);
			break;
		} case RPC_ID__Event_AP_StaConnected: {
			ret = rpc_evt_ap_staconn_conn_disconn(ntfy, inbuf, inlen, WIFI_EVENT_AP_STACONNECTED);
			break;
		} case RPC_ID__Event_AP_StaDisconnected: {
			ret = rpc_evt_ap_staconn_conn_disconn(ntfy, inbuf, inlen, WIFI_EVENT_AP_STADISCONNECTED);
			break;
		} case RPC_ID__Event_StaScanDone: {
			ret = rpc_evt_sta_scan_done(ntfy, inbuf, inlen, WIFI_EVENT_SCAN_DONE);
			break;
		} case RPC_ID__Event_StaConnected: {
			ret = rpc_evt_sta_connected(ntfy, inbuf, inlen, WIFI_EVENT_STA_CONNECTED);
			break;
		} case RPC_ID__Event_StaDisconnected: {
			ret = rpc_evt_sta_disconnected(ntfy, inbuf, inlen, WIFI_EVENT_STA_DISCONNECTED);
			break;
#if CONFIG_SOC_WIFI_HE_SUPPORT
		} case RPC_ID__Event_StaItwtSetup: {
			ret = rpc_evt_itwt_setup(ntfy, inbuf, inlen, WIFI_EVENT_ITWT_SETUP);
			break;
		} case RPC_ID__Event_StaItwtTeardown: {
			ret = rpc_evt_itwt_teardown(ntfy, inbuf, inlen, WIFI_EVENT_ITWT_TEARDOWN);
			break;
		} case RPC_ID__Event_StaItwtSuspend: {
			ret = rpc_evt_itwt_suspend(ntfy, inbuf, inlen, WIFI_EVENT_ITWT_SUSPEND);
			break;
		} case RPC_ID__Event_StaItwtProbe: {
			ret = rpc_evt_itwt_probe(ntfy, inbuf, inlen, WIFI_EVENT_ITWT_PROBE);
			break;
#endif
		} case RPC_ID__Event_WifiEventNoArgs: {
			ret = rpc_evt_Event_WifiEventNoArgs(ntfy, inbuf, inlen);
			break;
#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
		} case RPC_ID__Event_DhcpDnsStatus: {
			ret = rpc_evt_Event_DhcpDnsStatus(ntfy, inbuf, inlen);
			break;
#endif
#if H_SUPP_DPP_SUPPORT
		} case RPC_ID__Event_SuppDppUriReady: {
			ret = rpc_evt_supp_dpp_uri_ready(ntfy, inbuf, inlen);
			break;
		} case RPC_ID__Event_SuppDppCfgRecvd: {
			ret = rpc_evt_supp_dpp_cfg_recvd(ntfy, inbuf, inlen);
			break;
		} case RPC_ID__Event_SuppDppFail: {
			ret = rpc_evt_supp_dpp_fail(ntfy, inbuf, inlen);
			break;
#endif // H_SUPP_DPP_SUPPORT
#if H_WIFI_DPP_SUPPORT
		} case RPC_ID__Event_WifiDppUriReady: {
			ret = rpc_evt_wifi_dpp_uri_ready(ntfy, inbuf, inlen);
			break;
		} case RPC_ID__Event_WifiDppCfgRecvd: {
			ret = rpc_evt_wifi_dpp_cfg_recvd(ntfy, inbuf, inlen);
			break;
		} case RPC_ID__Event_WifiDppFail: {
			ret = rpc_evt_wifi_dpp_fail(ntfy, inbuf, inlen);
			break;
#endif
		} default: {
			ESP_LOGE(TAG, "Incorrect/unsupported Ctrl Notification[%u]\n",ntfy->msg_id);
			goto err;
			break;
		}
	}

	if (ret) {
		ESP_LOGI(TAG, "notification[%u] not sent\n", ntfy->msg_id);
		goto err;
	}

	*outlen = rpc__get_packed_size (ntfy);
	if (*outlen <= 0) {
		ESP_LOGE(TAG, "Invalid encoding for notify");
		goto err;
	}

	*outbuf = (uint8_t *)calloc(1, *outlen);
	if (!*outbuf) {
		ESP_LOGE(TAG, "No memory allocated for outbuf");
		esp_rpc_cleanup(ntfy);
		return ESP_ERR_NO_MEM;
	}

	rpc__pack (ntfy, *outbuf);

	//printf("event outbuf:\n");
	//ESP_LOG_BUFFER_HEXDUMP("event outbuf", *outbuf, *outlen, ESP_LOG_INFO);

	esp_rpc_cleanup(ntfy);
	return ESP_OK;

err:
	if (!*outbuf) {
		free(*outbuf);
		*outbuf = NULL;
	}
	esp_rpc_cleanup(ntfy);
	return ESP_FAIL;
}
