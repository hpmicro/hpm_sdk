/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2019 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    main.c
 *      Purpose: RTX example program
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "cmsis_os2.h"

/*  HPM example includes. */
#include "board.h"

void app_main(void *argument);
void app_msg(void *argument);

typedef struct msg_s {
    uint8_t cmd;
    uint8_t len;
    uint8_t data[8];
} msg_t;

static osThreadId_t tid_main; /* Thread id of thread: main      */
static osThreadId_t tid_msg;  /* Thread id of thread: msg       */
static osMessageQueueId_t msgQueue;
static osMemoryPoolId_t memPool;

static const osThreadAttr_t mainAttr = {
    .stack_size = 1024U,
    .priority = osPriorityNormal,
};

static const osThreadAttr_t msgAttr = {
    .stack_size = 1024U,
    .priority = osPriorityNormal,
};

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

void app_main(void *argument)
{
    (void)argument;

    osStatus_t status;
    uint32_t cnt = 0UL;
    msg_t *msg;

    while (1) {
        /* Allocate memory for the message */
        msg = osMemoryPoolAlloc(memPool, osWaitForever);
        if (msg == NULL) {
            printf("app_msg: osMemoryPoolAlloc failed.\n");
            continue;
        }

        /* Produce a new message and put it to the queue */
        ++cnt;
        msg->cmd = 1U;
        msg->len = 4U;
        *((uint32_t *)(msg->data)) = cnt;
        status = osMessageQueuePut(msgQueue, &msg, 0U, osWaitForever);
        if (status != osOK) {
            printf("app_main: osMessageQueuePut failed.\n");
        }

        /* Defer message creation */
        osDelay(osMessageQueueGetCount(msgQueue) * 100U);
    }
}

/*----------------------------------------------------------------------------
 * Application message receiver thread
 *---------------------------------------------------------------------------*/

void app_msg(void *argument)
{
    (void)argument;

    osStatus_t status;
    uint32_t cnt = 0;
    msg_t *msg;

    while (1) {
        /* Defer message processing */
        osDelay(osMessageQueueGetSpace(msgQueue) * 100U);

        /* Wait forever until a message could be received */
        status = osMessageQueueGet(msgQueue, &msg, NULL, osWaitForever);
        if (status != osOK) {
            printf("%s: osMessageQueueGet failed.\n", __func__);
        } else {
            if (msg->len == 4U) {
                cnt = *((uint32_t *)msg->data);
                printf("%s: received [cmd = %d, data = 0x%0X]\n", __func__, msg->cmd, cnt);
            }

            /* Free memory of the message */
            status = osMemoryPoolFree(memPool, msg);
            if (status != osOK) {
                printf("%s: osMemoryPoolFree failed.\n", __func__);
            }
        }
    }
}

/*----------------------------------------------------------------------------
 * Main entry
 *---------------------------------------------------------------------------*/

int main(void)
{
#ifndef CONFIG_RTTHREAD
    board_init();
#endif
    osKernelInitialize(); /* Initialize CMSIS-RTOS */

    tid_main = osThreadNew(app_main, NULL, &mainAttr); /* Create application main thread */
    if (tid_main == NULL) {
        printf("osThreadNew app_main thread failure!\n");
    }

    tid_msg = osThreadNew(app_msg, NULL, &msgAttr); /* Create message receiver thread */
    if (tid_msg == NULL) {
        printf("osThreadNew app_msg thread failure!\n");
    }

    /* Create message queue for up to 10 messages of type msg_t */
    msgQueue = osMessageQueueNew(10U, sizeof(msg_t *), NULL);
    if (msgQueue == NULL) {
        printf("osMessageQueueNew msgQueue failure!\n");
    }

    /* Create memory pool for actual message objects */
    memPool = osMemoryPoolNew(10U, sizeof(msg_t), NULL);
    if (memPool == NULL) {
        printf("osMemoryPoolNew memPool failure!\n");
    }

    osKernelStart(); /* Start thread execution */
}
