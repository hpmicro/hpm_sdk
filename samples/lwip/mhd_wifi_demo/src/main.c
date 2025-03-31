/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "shell.h"
#include "lwip/tcpip.h"
#include "wmhd_api.h"
#include "hpm_ppor_drv.h"
#include "hpm_gpio_drv.h"
#include <stddef.h>
#include "httpd.h"

SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, shell_uart_isr)
SDK_DECLARE_EXT_ISR_M(BOARD_APP_SDIO_WIFI_OOB_IRQ, wmhd_host_oob_isr)
SDK_DECLARE_EXT_ISR_M(BOARD_APP_SDIO_SDXC_IRQ, wmhd_host_sdio_isr)

#define task_init_PRIORITY      (configMAX_PRIORITIES - 2U)
#define mhd_task_PRIORITY       (configMAX_PRIORITIES - 6U)
#define MHD_THREAD_STACK_SIZE   (4096)

void task_init(void *param);


int main(void)
{
    board_init();
    board_init_led_pins();

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
    intc_m_enable_irq_with_priority(BOARD_APP_SDIO_SDXC_IRQ, 1);
#endif

    if (pdPASS != xTaskCreate(task_init, "task_init", 1024U, NULL, task_init_PRIORITY, NULL)) {
        while (1) {
        }
    }

    vTaskStartScheduler();
}

void task_init(void *param)
{
    (void) param;
    /*******************************************************************************************************************
     *
     * Initialize Cherry Shell
     *
     ******************************************************************************************************************/
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

    int32_t sdio_index = wmhd_lookup_hpm_sdio_index(BOARD_APP_SDIO_WIFI_SDXC_PORT);
    if (sdio_index >= 0) {
        /* Initialize TCPIP stack */
        tcpip_init(NULL, NULL);

        /*******************************************************************************************************************
         *
         * Initialize MHD WiFi
         *
         ******************************************************************************************************************/
        uint32_t log_level = wmhd_log_msg_level(0);
        wmhd_log_msg_level(log_level);
        wmhd_gpio_config(0, (uint32_t) BOARD_APP_SDIO_WIFI_WL_REG_ON_PORT, BOARD_APP_SDIO_WIFI_WL_REG_ON_PIN);
        wmhd_gpio_config(1, (uint32_t) BOARD_APP_SDIO_WIFI_OOB_PORT, BOARD_APP_SDIO_WIFI_OOB_PIN);

        wmhd_config_hpm_sdio_instances();
        wmhd_sdio_set_slot_and_maxclk(1, 0);
        wmhd_thread_config(0, mhd_task_PRIORITY, MHD_THREAD_STACK_SIZE);
        wmhd_module_init();
    } else {
        printf("Unsupported SDIO instance!\n");
    }
    vTaskDelete(NULL);
}

/* Wi-Fi commands */
static int wifi(uint32_t argc, char **argv)
{
    chry_shell_t *csh = (void *) argv[argc + 1];
    if (argc < 2) {
        csh_printf(csh, "usage: wifi [commands]\r\n\r\n");
        return -1;
    }
    --argc;
    ++argv;
    wmhd_wifi_commands(argc, argv);
    return 0;
}

CSH_CMD_EXPORT(wifi, "wifi [commands]");

/* Reset command */
static int reset(uint32_t argc, char **argv)
{
    (void) argc;
    (void) argv;
    ppor_reset_set_cold_reset_enable(HPM_PPOR, ppor_reset_software);
    ppor_reset_clear_hot_reset_enable(HPM_PPOR, ppor_reset_software);
    ppor_sw_reset(HPM_PPOR, 1000);
    return 0;
}

CSH_CMD_EXPORT(reset, "reset the board");

/* List task command */
static int list_task(uint32_t argc, char **argv)
{
    (void) argc;
    (void) argv;
    char *pInfoBuffer = malloc(2048);
    vTaskList(pInfoBuffer);
    printf("Task Name\tState\tPRIO\tStack\tTimeToWake\r\n");
    printf("%s\r\n", pInfoBuffer);
    free(pInfoBuffer);
    return 0;
}

CSH_CMD_EXPORT_ALIAS(list_task, ps, "Show all task info");

/* Show CPU usage */
static int runtime_states(uint32_t argc, char **argv)
{
    (void) argc;
    (void) argv;
    char *pInfoBuffer = malloc(2048);
    vTaskGetRunTimeStats(pInfoBuffer);
    printf("%s\r\n", pInfoBuffer);
    free(pInfoBuffer);
    return 0;
}

CSH_CMD_EXPORT_ALIAS(runtime_states, usage, "Show the CPU Usage");

/* Get the Hi-resolution tick */
ATTR_RAMFUNC portTickType get_hires_tick(void)
{
    static uint32_t ticks_per_100us;
    if (ticks_per_100us == 0) {
        ticks_per_100us = (hpm_core_clock / 10000);
    }
    return hpm_csr_get_core_mcycle() / ticks_per_100us;
}

/* Configure the Hi-resolution tick */
void hires_tick_config(void)
{
}

/* Application exception handler */
ATTR_RAMFUNC void freertos_risc_v_application_exception_handler(void)
{
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    printf("\r\nException handler\r\n");
    char *pInfoBuffer = malloc(2048);
    vTaskList(pInfoBuffer);
    printf("%s\r\n", pInfoBuffer);
    free(pInfoBuffer);
    while (1) {
    }
}

/* Start the http server */
static int start_httpserver(uint32_t argc, char **argv)
{
    (void) argc;
    (void) argv;

    httpd_init();
    return 0;
}

CSH_CMD_EXPORT_ALIAS(start_httpserver, start_httpserver, "start http server");