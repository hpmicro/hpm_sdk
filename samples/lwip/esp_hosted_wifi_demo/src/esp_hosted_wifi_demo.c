/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"



/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "esp_hosted_wifi_lwip.h"
#include "port_esp_hosted_host_spi.h"
#include "port_esp_hosted_host_os.h"
#include "esp_wlan_ops.h"

#include "hpm_dma_mgr.h"
#include "hpm_spi.h"
#include "shell.h"

#include "hpm_l1c_drv.h"

#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)

#define TEST_SPI                    BOARD_APP_SPI_BASE

volatile bool led_on;

port_esp_hosted_spi_context_t g_spi_ctx;

volatile static bool is_wifi_stopped;
char *wifi_password;
char *wifi_ssid;
static SemaphoreHandle_t wifi_reconnect_sem = NULL;
static TaskHandle_t wifi_reconnect_task_handle = NULL;

static void wifi_reconnect_task(void *param);

SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, shell_uart_isr)
#if (BOARD_APP_ESP_HOSTED_GPIO_HANDSHAKE_IRQ == BOARD_APP_ESP_HOSTED_GPIO_DATA_READY_IRQ)
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ESP_HOSTED_GPIO_HANDSHAKE_IRQ, isr_gpio)
void isr_gpio(void)
{
    hosted_gpio_isr_handler(CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN);
    hosted_gpio_isr_handler(CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN);
}
#else
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ESP_HOSTED_GPIO_HANDSHAKE_IRQ, isr_gpio_handshake)
void isr_gpio_handshake(void)
{
    hosted_gpio_isr_handler(CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN);
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ESP_HOSTED_GPIO_DATA_READY_IRQ, isr_gpio_data_ready)
void isr_gpio_data_ready(void)
{
    hosted_gpio_isr_handler(CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN);
}
#endif

void hosted_wifi_event_callback(hpm_esp_hosted_wifi_event_t event, void *data, uint32_t data_len)
{
    wifi_event_sta_disconnected_t *disconnect_info;
    if (event == hosted_wifi_evt_scan_report) {
        hpm_esp_hosted_wifi_info_t *ap_info = (hpm_esp_hosted_wifi_info_t *)data;
        printf("SSID:%s\r\n", ap_info->ssid);
    } else if (event == hosted_wifi_evt_scan_done) {
        printf("\r\n Scan wifi done\r\n");
    } else if ((event == hosted_wifi_evt_disconnect) && (data_len != 0) && (is_wifi_stopped == false)) {
        disconnect_info = (wifi_event_sta_disconnected_t *)data;
        if ((disconnect_info->reason == WIFI_REASON_ASSOC_NOT_AUTHED) || (disconnect_info->reason == WIFI_REASON_ASSOC_LEAVE)) {
            if (wifi_reconnect_sem) {
                xSemaphoreGive(wifi_reconnect_sem);
            }
        }
    } else if (event == hosted_wifi_evt_connect) {
        is_wifi_stopped = false;
    }
}

/* Wi-Fi commands */
static int wifi(uint32_t argc, char **argv)
{
    hpm_esp_hosted_wifi_commands(argc, &argv[0]);
    if (argc > 3 && strcmp(argv[1], "join") == 0) {
        wifi_ssid = argv[2];
        wifi_password = argv[3];
    }
    if (argc > 1 && strcmp(argv[1], "stop") == 0) {
        is_wifi_stopped = true;
    }
    return 0;
}
CSH_CMD_EXPORT(wifi, "wifi [commands]");

static int mem(uint32_t argc, char **argv)
{
    size_t total = configTOTAL_HEAP_SIZE;
    size_t free = xPortGetFreeHeapSize();
    size_t min_free = xPortGetMinimumEverFreeHeapSize();
    size_t used = total - free;
    size_t max_used = total - min_free;

    printf("total memory: %u\n", (unsigned int)total);
    printf("used memory : %u\n", (unsigned int)used);
    printf("maximum allocated memory: %u\n", (unsigned int)max_used);
    return 0;
}
CSH_CMD_EXPORT(mem, "mem: show heap usage like RT-Thread");

static int thread(uint32_t argc, char **argv)
{
    const UBaseType_t max_tasks = 30;
    TaskStatus_t task_status_array[max_tasks];
    UBaseType_t task_count, i;

    task_count = uxTaskGetSystemState(task_status_array, max_tasks, NULL);

    printf("Name                          State     Prio   Stack  Num\n");
    for (i = 0; i < task_count; i++) {
        const char *state_str;
        switch (task_status_array[i].eCurrentState) {
        case eReady:
            state_str = "Ready";
            break;
        case eRunning:
            state_str = "Running";
            break;
        case eBlocked:
            state_str = "Blocked";
            break;
        case eSuspended:
            state_str = "Suspended";
            break;
        case eDeleted:
            state_str = "Deleted";
            break;
        default:
            state_str = "?";
            break;
        }
        printf("%-28s %-10s %2u     %4u   %2u\n",
            task_status_array[i].pcTaskName,
            state_str,
            (unsigned int)task_status_array[i].uxCurrentPriority,
            (unsigned int)task_status_array[i].usStackHighWaterMark,
            (unsigned int)task_status_array[i].xTaskNumber);
    }
    return 0;
}
CSH_CMD_EXPORT(thread, "tasks: show all FreeRTOS tasks info");

static void task_init(void *pvParameters)
{
    (void)pvParameters;
    uart_config_t shell_uart_config = { 0 };
    uart_default_config(BOARD_CONSOLE_UART_BASE, &shell_uart_config);
    shell_uart_config.src_freq_in_hz = clock_get_frequency(BOARD_CONSOLE_UART_CLK_NAME);
    shell_uart_config.baudrate = 115200;
    if (status_success != uart_init(BOARD_CONSOLE_UART_BASE, &shell_uart_config)) {
        /* uart failed to be initialized */
        printf("Failed to initialize uart\r\n");
        while (1) {
        }
    }
    /* irq must be enabled after shell_init() */
    uart_enable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(BOARD_CONSOLE_UART_IRQ, 1);
    /* shell_init() must be called in-task */
    if (0 != shell_init(BOARD_CONSOLE_UART_BASE, false)) {
        /* shell failed to be initialized */
        printf("Failed to initialize shell\r\n");
        while (1) {
        }
    }
    board_init_spi_clock(TEST_SPI);
    board_init_spi_pins_with_gpio_as_cs(TEST_SPI);
    dma_mgr_init();
    g_spi_ctx.spi_ptr = TEST_SPI;
    g_spi_ctx.use_soft_cs = false;
    esp_hosted_spi_register(&g_spi_ctx);
    hw_esp_wlan_init(NULL);
    wifi_event_register_callbacks(hosted_wifi_event_callback);
    if (wifi_reconnect_sem == NULL) {
        wifi_reconnect_sem = xSemaphoreCreateBinary();
    }
    if (wifi_reconnect_task_handle == NULL) {
        xTaskCreate(wifi_reconnect_task, "wifi_reconnect", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &wifi_reconnect_task_handle);
    }
    vTaskDelete(NULL);
}

static void wifi_reconnect_task(void *param)
{
    (void)param;
    while (1) {
        if (xSemaphoreTake(wifi_reconnect_sem, portMAX_DELAY) == pdTRUE) {
            hpm_stat_t sta;
            printf("WiFi reconnect task: try to join...\r\n");
            sta = hpm_esp_hosted_wifi_join((uint8_t *)wifi_ssid, (uint8_t *)wifi_password,
                                           (uint8_t)strlen(wifi_ssid), (uint8_t)strlen(wifi_password));
            if (sta == status_success) {
                printf("Reconnecting to SSID: %s ...\r\n", wifi_ssid);
            } else {
                printf("Failed to start STA reconnection\r\n");
            }
        }
    }
}

int main(void)
{
    board_init();
    is_wifi_stopped = false;
    intc_m_enable_irq_with_priority(BOARD_APP_ESP_HOSTED_GPIO_HANDSHAKE_IRQ, 1);
    intc_m_enable_irq_with_priority(BOARD_APP_ESP_HOSTED_GPIO_DATA_READY_IRQ, 1);
    if (xTaskCreate(task_init, "task_init", configMINIMAL_STACK_SIZE + 256U, NULL, task1_PRIORITY, NULL) != pdPASS) {
        printf("Task1 creation failed!.\n");
        for (;;) {
            ;
        }
    }
    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}
