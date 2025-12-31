/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __HOST_POWER_SAVE_H__
#define __HOST_POWER_SAVE_H__

#include <stdint.h>
#include "sdkconfig.h"
#include "interface.h"

#if defined(CONFIG_ESP_HOSTED_HOST_POWER_SAVE_ENABLED)
  #define H_HOST_PS_ALLOWED 1
  extern uint8_t power_save_on;
#else
  #define H_HOST_PS_ALLOWED 0
#endif

#if H_HOST_PS_ALLOWED && defined(CONFIG_ESP_HOSTED_HOST_DEEP_SLEEP_ALLOWED)
  #define H_HOST_PS_DEEP_SLEEP_ALLOWED 1
#else
  #define H_HOST_PS_DEEP_SLEEP_ALLOWED 0
#endif

#if H_HOST_PS_DEEP_SLEEP_ALLOWED && CONFIG_ESP_HOSTED_HOST_WAKEUP_GPIO == -1
  #error "CONFIG_HOST_WAKEUP_GPIO is not configured. Either disable host power save or configure the host wakeup GPIO pin"
#else
  #define H_HOST_WAKE_UP_GPIO CONFIG_ESP_HOSTED_HOST_WAKEUP_GPIO
#endif

#if defined(CONFIG_ESP_HOSTED_UNLOAD_BUS_DRIVER_DURING_HOST_SLEEP)
  #define H_PS_UNLOAD_BUS_WHILE_PS 1
#else
  #define H_PS_UNLOAD_BUS_WHILE_PS 0
#endif


int host_power_save_init(void (*host_wakeup_callback)(void));
int host_power_save_deinit(void);
int is_host_wakeup_needed(interface_buffer_handle_t *buf_handle);
int wakeup_host_mandate(uint32_t timeout_ms);
int wakeup_host(uint32_t timeout_ms);
int host_power_save_alert(uint32_t ps_evt);
static inline int is_host_power_saving(void)
{
#if H_HOST_PS_ALLOWED
	return power_save_on;
#else
	return 0;
#endif
}


#endif
