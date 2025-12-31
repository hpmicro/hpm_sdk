/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_HOSTED_MISC_H__
#define __ESP_HOSTED_MISC_H__

#include <stdbool.h>
#include "esp_mac.h"

/**
  * @brief  Initialise the BT Controller on the co-processor
  *
  * @return ESP_OK on success
  */
esp_err_t esp_hosted_bt_controller_init(void);

/**
  * @brief  Deinitialise the BT Controller on the co-processor
  *
  * @param  mem_release. Also releases memory used by controller. Once released, the BT controller cannot reuse the memory and cannot be initialised
  *
  * @return ESP_OK on success
  */
esp_err_t esp_hosted_bt_controller_deinit(bool mem_release);

/**
  * @brief  Enables the BT Controller on the co-processor. Call only after initialising the BT controller.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_hosted_bt_controller_enable(void);

/**
  * @brief  Disables the BT Controller on the co-processor. Call before deinitialising the BT controller.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_hosted_bt_controller_disable(void);

/**
  * @brief  Set custom MAC address of the interface.
  *
  * This function allows you to overwrite the MAC addresses of the
  * interfaces set by the base MAC address.
  *
  * @param  mac  MAC address, length: 6 bytes/8 bytes.
  *         length: 6 bytes for MAC-48
  *                 8 bytes for EUI-64(used for ESP_MAC_IEEE802154 type)
  * @param  mac_len  Length of the mac array
  * @param  type  Type of MAC address
  *
  * @return ESP_OK on success
  */
esp_err_t esp_hosted_iface_mac_addr_set(uint8_t *mac, size_t mac_len, esp_mac_type_t type);

/**
  * @brief  Read MAC address of the interface.
  *
  * @param  mac base MAC address, length: 6 bytes/8 bytes.
  *         length: 6 bytes for MAC-48
  *                 8 bytes for EUI-64(used for IEEE 802.15.4)
  * @param  mac_len  Length of the mac array
  * @param  type Type of MAC address
  *
  * @return ESP_OK on success
  */
esp_err_t esp_hosted_iface_mac_addr_get(uint8_t *mac, size_t mac_len, esp_mac_type_t type);

/**
  * @brief  Return the size of the MAC type in bytes.
  *
  * @param  type  Type of MAC address
  *
  * @return 0 MAC type not found (not supported)
  *         6 bytes for MAC-48.
  *         8 bytes for EUI-64.
  */
size_t esp_hosted_iface_mac_addr_len_get(esp_mac_type_t type);

#endif
