/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "host_power_save.h"
#include "esp_hosted_transport_init.h"
#include "esp_hosted_interface.h"
#include "host_power_save.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include <string.h>
#include "esp_timer.h"
#include "interface.h"
static char *TAG = "host_ps";

#if H_HOST_PS_ALLOWED

  uint8_t power_save_on;

  #if H_HOST_PS_DEEP_SLEEP_ALLOWED
	SemaphoreHandle_t wakeup_sem;

	#define GPIO_HOST_WAKEUP (H_HOST_WAKE_UP_GPIO)

	/* Assuming wakup gpio neg 'level' interrupt */
	#define set_host_wakeup_gpio() gpio_set_level(GPIO_HOST_WAKEUP, 1)
	#define reset_host_wakeup_gpio() gpio_set_level(GPIO_HOST_WAKEUP, 0)
  #endif
  static void (*host_wakeup_cb)(void);
#endif

extern interface_context_t *if_context;
extern interface_handle_t *if_handle;

int is_host_wakeup_needed(interface_buffer_handle_t *buf_handle)
{
	int wakup_needed = 0;
	char reason[100] = "";
#if H_HOST_PS_ALLOWED
	uint8_t *buf_start;

	buf_start = buf_handle->payload;


#if CONFIG_ESP_SPI_HD_HOST_INTERFACE || CONFIG_ESP_UART_HOST_INTERFACE || CONFIG_ESP_SPI_HOST_INTERFACE
	/* Flow control packet cannot miss */
	if (buf_handle->wifi_flow_ctrl_en) {
		strlcpy(reason, "flow_ctl_pkt", sizeof(reason));
		wakup_needed = 1;
		goto end;
	}
#endif

	if (!buf_start) {
		/* Do not wake up */
		strlcpy(reason, "NULL_TxBuff", sizeof(reason));
		wakup_needed = 0;
		goto end;
	}

	/* Wake up for serial msg */
	switch (buf_handle->if_type) {

		case ESP_SERIAL_IF:
			  strlcpy(reason, "serial tx msg", sizeof(reason));
			  wakup_needed = 1;
			  goto end;
			  break;

		case ESP_HCI_IF:
			  strlcpy(reason, "bt tx msg", sizeof(reason));
			  wakup_needed = 1;
			  goto end;
			  break;

		case ESP_PRIV_IF:
			  strlcpy(reason, "priv tx msg", sizeof(reason));
			  wakup_needed = 1;
			  goto end;
			  break;

		case ESP_TEST_IF:
			  strlcpy(reason, "test tx msg", sizeof(reason));
			  wakup_needed = 1;
			  goto end;
			  break;

		case ESP_STA_IF:

			  /* TODO: parse packet if lwip split not configured.
			   * Decide if packets need to reach to host or not
			   **/
			  strlcpy(reason, "sta tx msg", sizeof(reason));
			  wakup_needed = 1;
			  goto end;
			  break;

		case ESP_AP_IF:
			  strlcpy(reason, "ap tx msg", sizeof(reason));
			  wakup_needed = 1;
			  goto end;
			  break;
	}

end:
#else
	strlcpy(reason, "host_ps_disabled", sizeof(reason));
	wakup_needed = 0;
#endif

	if (wakup_needed) {
		ESP_LOGI(TAG, "Wakeup needed, reason %s", reason);
	} else {
		ESP_LOGI(TAG, "Wakeup not needed");
	}
	return wakup_needed;
}


int host_power_save_init(void (*fn_host_wakeup_cb)(void))
{
#if H_HOST_PS_ALLOWED

#if H_HOST_PS_DEEP_SLEEP_ALLOWED
	assert(GPIO_HOST_WAKEUP != -1);
	/* Configuration for the OOB line */
	gpio_config_t io_conf={
		.intr_type=GPIO_INTR_DISABLE,
		.mode=GPIO_MODE_OUTPUT,
		.pin_bit_mask=(1ULL<<GPIO_HOST_WAKEUP)
	};


	ESP_LOGI(TAG, "Host wakeup: IO%u, level:%u", GPIO_HOST_WAKEUP, gpio_get_level(GPIO_HOST_WAKEUP));
	gpio_config(&io_conf);
	reset_host_wakeup_gpio();
	gpio_pulldown_en(GPIO_HOST_WAKEUP);
	ESP_LOGI(TAG, "Host wakeup: IO%u, level:%u", GPIO_HOST_WAKEUP, gpio_get_level(GPIO_HOST_WAKEUP));

	assert(wakeup_sem = xSemaphoreCreateBinary());
	xSemaphoreGive(wakeup_sem);
#endif

	host_wakeup_cb = fn_host_wakeup_cb;
#endif
	return 0;
}

int host_power_save_deinit(void)
{
#if H_HOST_PS_ALLOWED
#if H_HOST_PS_DEEP_SLEEP_ALLOWED
	if (wakeup_sem) {
		xSemaphoreTake(wakeup_sem, portMAX_DELAY);
		xSemaphoreGive(wakeup_sem);
		vSemaphoreDelete(wakeup_sem);
		wakeup_sem = NULL;
	}
#endif
	host_wakeup_cb = NULL;
#endif
	return 0;
}

#define GET_CURR_TIME_IN_MS() esp_timer_get_time()/100

/* Add new callback function for ESP Timer */
#if H_HOST_PS_ALLOWED && H_HOST_PS_DEEP_SLEEP_ALLOWED
static void clean_wakeup_gpio_timer_cb(void* arg)
{
	reset_host_wakeup_gpio();
	ESP_EARLY_LOGI(TAG, "Cleared wakeup gpio, IO%u", GPIO_HOST_WAKEUP);
}
#endif

int wakeup_host_mandate(uint32_t timeout_ms)
{
#if H_HOST_PS_ALLOWED && H_HOST_PS_DEEP_SLEEP_ALLOWED
	esp_timer_handle_t timer = NULL;
	esp_err_t ret = ESP_OK;
	uint64_t start_time = GET_CURR_TIME_IN_MS();
	uint8_t wakeup_success = 0;
	esp_timer_create_args_t timer_args = {
		.callback = &clean_wakeup_gpio_timer_cb,
		.name = "host_wakeup_timer",
	};

	ESP_LOGI(TAG, "WAKE UP Host!!!!!\n");

	do {
		set_host_wakeup_gpio();

		/* Create ESP Timer instead of FreeRTOS timer */
		ret = esp_timer_create(&timer_args, &timer);
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "Failed to create timer for host wakeup");
			break;
		}

		/* Start one-shot timer (10ms) */
		ret = esp_timer_start_once(timer, 10000); /* 10ms in microseconds */
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "Failed to start timer for host wakeup");
			esp_timer_delete(timer);
			break;
		}
		vTaskDelay(100);

		if (wakeup_sem) {
			/* wait for host resume */
			ret = xSemaphoreTake(wakeup_sem, pdMS_TO_TICKS(100));
			ESP_LOGI(TAG, "Wakeup semaphore taken");
			if (ret == pdPASS) {
				ESP_LOGI(TAG, "Wakeup semaphore given");
				xSemaphoreGive(wakeup_sem);
				wakeup_success = 1;
				break;
			}
		}

		if (GET_CURR_TIME_IN_MS() - start_time > timeout_ms) {
			/* timeout */
			ESP_LOGI(TAG, "%s:%u timeout Curr:%llu start:%llu timeout:%lu",
					__func__,__LINE__, GET_CURR_TIME_IN_MS(), start_time, timeout_ms);
			break;
		}

	} while (1);

	/* Clean up timer if it's still active */
	if (timer) {
		esp_timer_stop(timer);
		esp_timer_delete(timer);
	}

	return wakeup_success;

#else
	return 1;
#endif
}

int wakeup_host(uint32_t timeout_ms)
{

#if H_HOST_PS_ALLOWED
	int wakeup_success = 0;

	if(!is_host_power_saving()) {
		return 1;
	}

	if (!if_handle || !if_context) {
		ESP_LOGE(TAG, "Failed to wakeup, if_handle or if_context is NULL");
		return 0;
	}

	ESP_LOGI(TAG, "if_handle->state: %u", if_handle->state);
	if (if_handle->state < DEACTIVE) {
		ESP_LOGI(TAG, "%s:%u Re-Initializing driver\n", __func__, __LINE__);

		/* host wakeup mandated in sdio init */
		wakeup_success = 1;
		if_handle = if_context->if_ops->init();
		if (!if_handle) {
			ESP_LOGE(TAG, "%s:%u Failed to initialize driver\n", __func__, __LINE__);
			return ESP_FAIL;
		}
	}

	if (power_save_on) {
		wakeup_success = wakeup_host_mandate(timeout_ms);
		ESP_LOGI(TAG, "host %s woke up", is_host_power_saving() ? "not" : "");
	}

	return wakeup_success;
#else
	return 1;
#endif
}

int host_power_save_alert(uint32_t ps_evt)
{

#if H_HOST_PS_ALLOWED
	/* Running in interrupt context - Keep it short and simple */
	BaseType_t do_yeild = pdFALSE;

	if (ESP_POWER_SAVE_ON == ps_evt) {
		ESP_EARLY_LOGI(TAG, "Host Sleep");
  #if H_HOST_PS_DEEP_SLEEP_ALLOWED
		if (wakeup_sem) {
			/* Host sleeping */
			xSemaphoreTakeFromISR(wakeup_sem, &do_yeild);
		}
  #endif
		power_save_on = 1;

		if (!if_handle || !if_context || if_handle->state < DEACTIVE) {
			ESP_EARLY_LOGE(TAG, "%s:%u Failed to bring down transport", __func__, __LINE__);
		}

		if (if_handle->state >= DEACTIVE) {
			if (!if_context->if_ops || !if_context->if_ops->deinit) {
				ESP_EARLY_LOGI(TAG, "%s:%u if_context->if_ops->deinit not available", __func__, __LINE__);
			} else {
				ESP_EARLY_LOGI(TAG, "%s:%u Deinitializing driver", __func__, __LINE__);
				if_context->if_ops->deinit(if_handle);
				/* if_handle->state would be changed to DEINIT */
			}
		}
	} else if ((ESP_POWER_SAVE_OFF == ps_evt) || (ESP_OPEN_DATA_PATH == ps_evt)) {
		ESP_EARLY_LOGI(TAG, "Host Awake, transport state: %u", if_handle->state);

		power_save_on = 0;
		if (host_wakeup_cb) {
			host_wakeup_cb();
		}
  #if H_HOST_PS_DEEP_SLEEP_ALLOWED
		if (wakeup_sem) {
			xSemaphoreGiveFromISR(wakeup_sem, &do_yeild);
		}
  #endif
	} else {
		ESP_EARLY_LOGI(TAG, "Ignore event[%u]", ps_evt);
	}

	if (do_yeild == pdTRUE) {
		portYIELD_FROM_ISR();
	}
#endif
	return 0;
}

