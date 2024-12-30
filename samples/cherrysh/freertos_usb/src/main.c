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
#include "shell.h"
#include "usbd_core.h"

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

static void task_start(void *param)
{
    (void)param;

    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    /* default password is : 12345678 */
    /* shell_init() must be called in-task */
    if (0 != shell_init(0, CONFIG_HPM_USBD_BASE, true)) {
        /* shell failed to be initialized */
        printf("Failed to initialize shell\r\n");
        while (1) {
        };
    }

    printf("Initialize shell successfully\r\n");

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
