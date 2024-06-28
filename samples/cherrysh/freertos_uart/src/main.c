/*
 * Copyright (c) 2024 HPMicro
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
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "shell.h"

SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, shell_uart_isr)

#define task_other_PRIORITY (configMAX_PRIORITIES - 3U)
#define task_start_PRIORITY (configMAX_PRIORITIES - 2U)

static void task_start(void *param);

int main(void)
{
    board_init();
    board_init_led_pins();

    if (pdPASS != xTaskCreate(task_start, "task_start", 1024U, NULL, task_start_PRIORITY, NULL)) {
        printf("Task start creation failed!\r\n");
        while (1) {
        };
    }

    vTaskStartScheduler();
    printf("Unexpected scheduler exit!\r\n");
    while (1) {
    };

    return 0;
}

static void task_other(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        shell_lock();
        printf("other task interval 5S\r\n");
        shell_unlock();
        vTaskDelay(5000);
    }
}

static void task_start(void *param)
{
    (void)param;

    printf("Try to initialize the uart\r\n"
           "  if you are using the console uart as the shell uart\r\n"
           "  failure to initialize may result in no log\r\n");

    uart_config_t shell_uart_config = { 0 };
    uart_default_config(BOARD_CONSOLE_UART_BASE, &shell_uart_config);
    shell_uart_config.src_freq_in_hz = clock_get_frequency(BOARD_CONSOLE_UART_CLK_NAME);
    shell_uart_config.baudrate = 115200;

    if (status_success != uart_init(BOARD_CONSOLE_UART_BASE, &shell_uart_config)) {
        /* uart failed to be initialized */
        printf("Failed to initialize uart\r\n");
        while (1) {
        };
    }

    printf("Initialize shell uart successfully\r\n");

    /* default password is : 12345678 */
    /* shell_init() must be called in-task */
    if (0 != shell_init(BOARD_CONSOLE_UART_BASE, true)) {
        /* shell failed to be initialized */
        printf("Failed to initialize shell\r\n");
        while (1) {
        };
    }

    printf("Initialize shell successfully\r\n");

    /* irq must be enabled after shell_init() */
    uart_enable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(BOARD_CONSOLE_UART_IRQ, 1);

    printf("Enable shell uart interrupt\r\n");

    if (pdPASS != xTaskCreate(task_other, "task_other", configMINIMAL_STACK_SIZE, NULL, task_other_PRIORITY, NULL)) {
        printf("Task other creation failed!\r\n");
        while (1) {
        };
    }

    printf("Exit start task\r\n");
    vTaskDelete(NULL);
}

static int test(int argc, char **argv)
{
	chry_shell_t *csh = (void *)argv[argc + 1];
    csh_printf(csh, "test: \r\n");
    csh_printf(csh, "argc=<%d>\r\n", argc);
    for (uint8_t i = 0; i < argc; i++) {
        csh_printf(csh, "argv[%d]:0x%08x=<%s>\r\n", i, (uintptr_t)argv[i], argv[i]);
    }

    csh_printf(csh, "argv[%d]=<0x%08x>\r\n", argc, argv[argc]);
    csh_printf(csh, "argv[%d]=<0x%08x>\r\n\r\n", argc + 1, argv[argc + 1]);

    return 0;
}
CSH_CMD_EXPORT(test,);

static int toggle_led(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    board_led_toggle();
    return 0;
}
CSH_CMD_EXPORT(toggle_led,);

static int write_led(int argc, char **argv)
{
	chry_shell_t *csh = (void *)argv[argc + 1];
    if (argc < 2) {
        csh_printf(csh, "usage: write_led <status>\r\n\r\n");
        csh_printf(csh, "  status    0 or 1\r\n\r\n");
        return -1;
    }

    board_led_write(!board_get_led_gpio_off_level() ^ (atoi(argv[1]) == 0));
    return 0;
}
CSH_CMD_EXPORT(write_led,);
