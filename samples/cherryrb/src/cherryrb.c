/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include "chry_ringbuffer.h"
#include "board.h"

#define producer_PRIORITY    (configMAX_PRIORITIES - 4U)
#define consumer_PRIORITY    (configMAX_PRIORITIES - 5U)

chry_ringbuffer_t rb;
ATTR_PLACE_AT_NONCACHEABLE uint8_t mempool[1024];

static void thread_producer(void *param)
{
    char *data = "hello world";
    while (1) {
        uint32_t len = chry_ringbuffer_write(&rb, data, 11);
        if (11 == len) {
            printf("[P] write success\r\n");
        } else {
            printf("[P] write failed, only write %d byte\r\n", len);
        }
        vTaskDelay(1000);
    }
}


static void thread_consumer(void *param)
{
    char data[64];
    while (1) {
        uint32_t len = chry_ringbuffer_read(&rb, data, 11);
        if (len) {
            printf("[C] read success, read %d byte\r\n", len);
            data[11] = '\0';
            printf("%s\r\n", data);
        } else {
            printf("[C] read failed, no data in ringbuffer\r\n");
        }
        vTaskDelay(1000);
    }
}


int main(void)
{
    board_init();

    if (0 == chry_ringbuffer_init(&rb, mempool, 1024)) {
        printf("chry_ringbuffer_init success\r\n");
    } else {
        printf("chry_ringbuffer_init error\r\n");
    }
    if (xTaskCreate(thread_producer, "thread_producer", configMINIMAL_STACK_SIZE + 256U, NULL, producer_PRIORITY, NULL) != pdPASS) {
        printf("thread_producer creation failed!.\n");
        for (;;) {
            ;
        }
    }
    if (xTaskCreate(thread_consumer, "thread_consumer", configMINIMAL_STACK_SIZE + 256U, NULL, consumer_PRIORITY, NULL) != pdPASS) {
        printf("thread_consumer creation failed!.\n");
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

