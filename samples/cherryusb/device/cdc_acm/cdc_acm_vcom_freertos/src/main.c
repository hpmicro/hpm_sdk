/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "usb_osal.h"
#include <stdio.h>
#include "board.h"
#include "usb_config.h"
#include "usb_osal.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern volatile bool dtr_enable;
extern void cdc_acm_init(uint8_t busid, uint32_t reg_base);
extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);
extern void destroy_demaphore_tx_done(void);
#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)

static void task1(void *pvParameters)
{
    (void)pvParameters;

    uint32_t u = 0;
    printf("[cherryusb cdc acm with freertos sample]: task started.\n");

    while (u < 2) {
        if (dtr_enable) {
            u++;
            vTaskDelay(50);
            cdc_acm_data_send_with_dtr_test(0);
        } else {
            vTaskDelay(100);
        }
    }
    destroy_demaphore_tx_done();
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb cdc_acm device freertos sample.\n");

    cdc_acm_init(0, CONFIG_HPM_USBD_BASE);

    if (usb_osal_thread_create("task1", 8192U, task1_PRIORITY, task1, NULL) == NULL) {
        perror("Task1 creation failed!.\n");
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
