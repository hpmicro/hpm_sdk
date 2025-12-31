/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * All the APIs declared here are internal only APIs, it can only be used by
 * espressif internal modules, such as SSC, LWIP, esp-netif etc, espressif
 * customers are not recommended to use them.
 *
 * If someone really want to use specified APIs declared in here, please contact
 * espressif AE/developer to make sure you know the limitations or risk of
 * the API, otherwise you may get unexpected behavior!!!
 *
 */

#include "esp_hosted_log.h"

/**
  * @brief  free the rx buffer which allocated by wifi driver
  *
  * @param  void* buffer: rx buffer pointer
  */
void esp_wifi_internal_free_rx_buffer(void* buffer);

/**
  * @brief  transmit the buffer via wifi driver
  *
  * This API makes a copy of the input buffer and then forwards the buffer
  * copy to WiFi driver.
  *
  * @param  wifi_interface_t wifi_if : wifi interface id
  * @param  void *buffer : the buffer to be transmit
  * @param  uint16_t len : the length of buffer
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF : WiFi interface is invalid
  *    - ESP_ERR_WIFI_CONN : WiFi interface is not created, e.g. send the data to STA while WiFi mode is AP mode
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started
  *    - ESP_ERR_WIFI_STATE : WiFi internal state is not ready, e.g. WiFi is not started
  *    - ESP_ERR_WIFI_NOT_ASSOC : WiFi is not associated
  *    - ESP_ERR_WIFI_TX_DISALLOW : WiFi TX is disallowed, e.g. WiFi hasn't pass the authentication
  *    - ESP_ERR_WIFI_POST : caller fails to post event to WiFi task
  */
int esp_wifi_internal_tx(wifi_interface_t wifi_if, void *buffer, uint16_t len);

/**
  * @brief     The net stack buffer reference counter callback function
  *
  */
typedef void (*wifi_netstack_buf_ref_cb_t)(void *netstack_buf);

/**
  * @brief     The net stack buffer free callback function
  *
  */
typedef void (*wifi_netstack_buf_free_cb_t)(void *netstack_buf);

/**
  * @brief  transmit the buffer by reference via wifi driver
  *
  * This API firstly increases the reference counter of the input buffer and
  * then forwards the buffer to WiFi driver. The WiFi driver will free the buffer
  * after processing it. Use esp_wifi_internal_tx() if the uplayer buffer doesn't
  * supports reference counter.
  *
  * @param  wifi_if : wifi interface id
  * @param  buffer : the buffer to be transmit
  * @param  len : the length of buffer
  * @param  netstack_buf : the netstack buffer related to buffer
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF : WiFi interface is invalid
  *    - ESP_ERR_WIFI_CONN : WiFi interface is not created, e.g. send the data to STA while WiFi mode is AP mode
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started
  *    - ESP_ERR_WIFI_STATE : WiFi internal state is not ready, e.g. WiFi is not started
  *    - ESP_ERR_WIFI_NOT_ASSOC : WiFi is not associated
  *    - ESP_ERR_WIFI_TX_DISALLOW : WiFi TX is disallowed, e.g. WiFi hasn't pass the authentication
  *    - ESP_ERR_WIFI_POST : caller fails to post event to WiFi task
  */
esp_err_t esp_wifi_internal_tx_by_ref(wifi_interface_t ifx, void *buffer, size_t len, void *netstack_buf);

/**
  * @brief  register the net stack buffer reference increasing and free callback
  *
  * @param  ref : net stack buffer reference callback
  * @param  free: net stack buffer free callback
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - others  : failed to register the callback
  */
esp_err_t esp_wifi_internal_reg_netstack_buf_cb(wifi_netstack_buf_ref_cb_t ref, wifi_netstack_buf_free_cb_t free);

/**
  * @brief     The WiFi RX callback function
  *
  *            Each time the WiFi need to forward the packets to high layer, the callback function will be called
  */
typedef esp_err_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void *eb);

/**
  * @brief     Set the WiFi RX callback
  *
  * @attention 1. Currently we support only one RX callback for each interface
  *
  * @param     wifi_interface_t ifx : interface
  * @param     wifi_rxcb_t fn : WiFi RX callback
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn);

/**
  * @brief     Notify WIFI driver that the station got ip successfully
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_set_sta_ip(void);
