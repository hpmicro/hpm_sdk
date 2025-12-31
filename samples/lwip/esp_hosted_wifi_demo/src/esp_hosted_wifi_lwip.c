/*
 * Copyright (c) 2025 Evlers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Change Logs:
 * Date         Author      Notes
 * 2024-12-26   Evlers      first implementation
 * 2025-9-23    HPMicro     adapted for FreeRTOS and LwIP
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_common.h"

#include "esp_hosted_wifi_lwip.h"

#include "wifi.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_hosted.h"
#include "port_esp_hosted_host_os.h"
#include "esp_wlan_ops.h"

#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#endif

DEFINE_LOG_TAG(esp_hosted_wifi_lwip);

#define ESP_HOSTED_WIFI_MTU          (1500U)

#ifndef ESP_HOSTED_WIFI_AP_SSID_NAME
#define ESP_HOSTED_WIFI_AP_SSID_NAME       "hpm_ap_test"
#endif

#ifndef ESP_HOSTED_WIFI_AP_SSID_PASSWORD
#define ESP_HOSTED_WIFI_AP_SSID_PASSWORD   "123456789"
#endif

ip_addr_t g_ipaddr[CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT];
ip_addr_t g_netmask[CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT];
ip_addr_t g_gateway[CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT];

struct netif g_esp_hosted_wifi_netif[CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT];
volatile bool g_sta_connected;

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(HPM_L1C_CACHELINE_SIZE) uint8_t g_esp_hosted_tx_buffer[CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT][CONFIG_ESP_HOSTED_WIFI_MAX_RX_SIZE];

static err_t esp_hosted_wifi_linkoutput(struct netif *netif, struct pbuf *p)
{
    uint8_t ifx_index = 0;
    wifi_interface_t ifx;
    struct pbuf *q;
    uint8_t *buffer = NULL;
    if ((p == NULL) || (netif == NULL)) {
        return ERR_ARG;
    }
    if (netif->name[0] == 'w' && netif->name[1] == '0') {
        ifx = WIFI_IF_STA;
        ifx_index = 0;
    } else if (netif->name[0] == 'w' && netif->name[1] == '1') {
        ifx = WIFI_IF_AP;
        ifx_index = 1;
    } else {
        return ERR_IF;
    }
    buffer = g_esp_hosted_tx_buffer[ifx_index];
    for (q = p; q != NULL; q = q->next) {
        g_h.funcs->_h_memcpy(buffer, q->payload, q->len);
        buffer += q->len;
    }
    if (esp_wifi_internal_tx(ifx, g_esp_hosted_tx_buffer[ifx_index], p->tot_len) != ESP_OK) {
        ESP_LOGE(TAG, "failed to send ap data to wifi drivers");
        return ERR_IF;
    }
    return ERR_OK;
}

static err_t esp_hosted_wifi_sta_if_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    netif->mtu = ESP_HOSTED_WIFI_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'w';
    netif->name[1] = '0';
    netif->output = etharp_output;
    netif->linkoutput = esp_hosted_wifi_linkoutput;
    return ERR_OK;
}

static err_t esp_hosted_wifi_ap_if_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    netif->mtu = ESP_HOSTED_WIFI_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'w';
    netif->name[1] = '1';
    netif->output = etharp_output;
    netif->linkoutput = esp_hosted_wifi_linkoutput;
    return ERR_OK;
}

static esp_err_t wifi_ap_rx(void *buffer, uint16_t len, void *eb)
{
    err_t err;
    (void)eb;
#if LWIP_TCPIP_CORE_LOCKING_INPUT
    pbuf_type type = PBUF_REF;
#else
    pbuf_type type = PBUF_POOL;
#endif
    struct pbuf *p;
    if (buffer != NULL) {
        p = pbuf_alloc(PBUF_RAW, len, type);
        if (p != NULL) {
#if LWIP_TCPIP_CORE_LOCKING_INPUT
            p->payload = buffer;
#else
            g_h.funcs->_h_memcpy(p->payload, buffer, len);
#endif
            err =  g_esp_hosted_wifi_netif[1].input(p, &g_esp_hosted_wifi_netif[1]);
            if (err != ERR_OK) {
                pbuf_free(p);
                return ESP_FAIL;
            }
        } else {
            ESP_LOGE(TAG, "No memory to alloc pbuf");
            return ESP_FAIL;
        }
        esp_wifi_internal_free_rx_buffer(buffer);
    } else {
        ESP_LOGE(TAG, "wifi ap rx buffer is NULL");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t wifi_sta_rx(void *buffer, uint16_t len, void *eb)
{
    err_t err;
    (void)eb;
#if LWIP_TCPIP_CORE_LOCKING_INPUT
    pbuf_type type = PBUF_REF;
#else
    pbuf_type type = PBUF_POOL;
#endif
    struct pbuf *p;
    if (buffer != NULL) {
        p = pbuf_alloc(PBUF_RAW, len, type);
        if (p != NULL) {
#if LWIP_TCPIP_CORE_LOCKING_INPUT
            p->payload = buffer;
#else
            g_h.funcs->_h_memcpy(p->payload, buffer, len);
#endif
            err =  g_esp_hosted_wifi_netif[0].input(p, &g_esp_hosted_wifi_netif[0]);
            if (err != ERR_OK) {
                pbuf_free(p);
                return ESP_FAIL;
            }
        } else {
            ESP_LOGE(TAG, "No memory to alloc pbuf");
            return ESP_FAIL;
        }
        esp_wifi_internal_free_rx_buffer(buffer);
    } else {
        ESP_LOGE(TAG, "wifi sta rx buffer is NULL");
        return ESP_FAIL;
    }
    return ESP_OK;
}

#if LWIP_DHCP
static void dhcp_sta_thread(void *param)
{
    struct dhcp *dhcp;
    struct netif *netif = (struct netif *)param;
    while (g_sta_connected == false) {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    if (hpm_esp_hosted_wifi_get_mac(WIFI_IF_STA, g_esp_hosted_wifi_netif[0].hwaddr) != status_success) {
        ESP_LOGE(TAG, "get sta mac address failed");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    dhcp_start(netif);
    while (1) {
        if (netif_is_up(netif) && (g_sta_connected == true)) {
            dhcp = netif_dhcp_data(netif);
            if (dhcp && (dhcp->state == DHCP_STATE_BOUND)) {
                ESP_LOGI(TAG, "netif name: %c%c", netif->name[0], netif->name[1]);
                ESP_LOGI(TAG, "IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
                ESP_LOGI(TAG, "IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
                ESP_LOGI(TAG, "IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    vTaskDelete(NULL);
}
#endif


void hw_esp_wlan_init(void *parameter)
{
    (void)parameter;
    uint8_t idx = 0;
    struct netif *netif;
    uint8_t i = 0;
    g_sta_connected = false;
    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    hpm_esp_hosted_wifi_init(WIFI_MODE_APSTA, WIFI_STORAGE_RAM);

    /* register the wifi driver rx callback */
    hpm_esp_hosted_wifi_internal_reg_rxcb(wifi_ap_rx, wifi_sta_rx);

    hpm_esp_hosted_wifi_ap(ESP_HOSTED_WIFI_AP_SSID_NAME, ESP_HOSTED_WIFI_AP_SSID_PASSWORD);

    if (hpm_esp_hosted_wifi_start() != status_success) {
        ESP_LOGE(TAG, "esp wifi start failed!");
        return;
    }

    for (i = 0; i < sizeof(g_esp_hosted_wifi_netif) / sizeof(struct netif); i++) {
        netif = &g_esp_hosted_wifi_netif[i];
        netif->hwaddr_len = 6;
        const char *mac_cfg = (i == 0) ? HPM_STRINGIFY(ESP_HOSTED_WIFI_STA_MAC_CONFIG)
                                       : HPM_STRINGIFY(ESP_HOSTED_WIFI_AP_MAC_CONFIG);
        unsigned int b[6];
        if (sscanf(mac_cfg, "%2x:%2x:%2x:%2x:%2x:%2x",
                   &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != 6) {
            ESP_LOGE(TAG, "invalid mac address format");
            return;
        }
        for (idx = 0; idx < 6; idx++) {
            netif->hwaddr[idx] = (uint8_t)b[idx];
        }
        IP4_ADDR(&g_ipaddr[i], 0, 0, 0, 0);
        IP4_ADDR(&g_netmask[i], 0, 0, 0, 0);
        IP4_ADDR(&g_gateway[i], 0, 0, 0, 0);
        if (i == 0) {
            netif = netif_add(netif, &g_ipaddr[i], &g_netmask[i], &g_gateway[i], NULL, esp_hosted_wifi_sta_if_init, tcpip_input);
        } else {
            netif = netif_add(netif, &g_ipaddr[i], &g_netmask[i], &g_gateway[i], NULL, esp_hosted_wifi_ap_if_init, tcpip_input);
        }
        netif_set_default(netif);
        while (!netif_is_up(netif)) {
        }
#if LWIP_DHCP
        if (i == 0) {
            g_sta_connected = false;
            if (xTaskCreate(dhcp_sta_thread, "dhcp_sta_thread", 512, netif, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
                ESP_LOGE(TAG, "dhcp_sta_thread creation failed");
            }
        }
#endif
    }
    netif_set_default(&g_esp_hosted_wifi_netif[0]);

    return;

}