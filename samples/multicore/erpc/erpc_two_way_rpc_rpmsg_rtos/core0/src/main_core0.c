/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_l1c_drv.h"
#include "hpm_misc.h"
#include "hpm_pmp_drv.h"
#include "hpm_sysctl_drv.h"
#include "erpc_arbitrated_client_setup.h"
#include "erpc_server_setup.h"
#include "erpc_two_way_rpc_Core0Interface.h"
#include "erpc_two_way_rpc_Core1Interface_server.h"
#include "rpmsg_lite.h"
#include "multicore_common.h"
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define MATRIX_ITEM_MAX_VALUE (50)
#define APP_ERPC_READY_EVENT_DATA (1U)

/*******************************************************************************
 * Variables
 ******************************************************************************/
static TaskHandle_t s_client_task_handle;
static TaskHandle_t s_server_task_handle;
static erpc_transport_t s_transportArbitrator;
static erpc_transport_t s_transport;
static getNumberCallback_t s_getNumberCallbackPtr;
static erpc_server_t s_server;
static uint32_t s_number;
static volatile uint16_t eRPCReadyEventData;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief eRPC server side ready event handler
 */
static void eRPCReadyEventHandler(uint16_t eventData, void *context)
{
    (void)context;
    eRPCReadyEventData = eventData;
}

/* Implementation of RPC function increaseNumber. */
void increaseNumber(uint32_t *number)
{
    *number += 1U;
    s_number = *number;
}

/* Implementation of RPC function getGetCallbackFunction. */
void getGetCallbackFunction(getNumberCallback_t *getNumberCallbackParam)
{
    *getNumberCallbackParam = s_getNumberCallbackPtr;
}

/* Implementation of RPC function getNumberFromCore0. */
void getNumberFromCore0(uint32_t *number)
{
    *number = s_number;
    printf("getNumberFromCore0 function call: Actual number is %d\r\n", *number);
}

static void client_task(void *param)
{
    (void)param;

    int32_t core;
    uint32_t number;

    /* Register the application event before starting the secondary core */
    (void)ipc_register_event(ipc_remote_start_event, eRPCReadyEventHandler, NULL);

    printf("\r\nPrimary core started\r\n");
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);
    printf("Starting secondary core...\r\n");

    /*
     * Wait until the secondary core application signals the rpmsg remote has
     * been initialized and is ready to communicate.
     */
    while (APP_ERPC_READY_EVENT_DATA != eRPCReadyEventData) {
    };

    printf("\r\nSecondary core started...\r\n");

    /* ERPC client initialization */
    erpc_client_t erpc_client;

    /* RPMsg-Lite transport layer initialization */
    erpc_transport_t transport;

    transport = erpc_transport_rpmsg_lite_rtos_master_init(100, 101, ERPC_TRANSPORT_RPMSG_LITE_LINK_ID);

    s_transport = transport;

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(transport);

    /* eRPC client side initialization */
    erpc_client = erpc_arbitrated_client_init(transport, message_buffer_factory, &s_transportArbitrator);

    /* Add server to client is necessary when do nesting RPC call. */
    while (s_server == NULL) {
        vTaskDelay(100);
    }
    erpc_arbitrated_client_set_server(erpc_client, s_server);
    erpc_arbitrated_client_set_server_thread_id(erpc_client, (void *)s_server_task_handle);

    s_getNumberCallbackPtr = &getNumberFromCore1;

    /* Simple synchronization system. To be sure that second core server is ready before rpc call. */
    vTaskDelay(500);


    for (;;) {
        /* RPC call to set callback function to second side. */
        setGetNumberFunction(s_getNumberCallbackPtr);

        /* Not necessary to set NULL. It is only for example purposes. */
        s_getNumberCallbackPtr = NULL;

        /* RPC call to get callback function from second side. */
        getGetNumberFunction(&s_getNumberCallbackPtr);

        /* Compare received address. */
        if (s_getNumberCallbackPtr == &getNumberFromCore1) {
            core = 1;
        } else {
            core = 0;
        }

        printf("Get number from core%d:\r\n", core);
        s_getNumberCallbackPtr(&number);
        printf("Got number: %d\r\n", number);

        printf("Start ");
        if (core == 0) {
            printf("nested");
        } else {
            printf("normal");
        }
        printf(" rpc call example.\r\n");
        nestedCallGetNumber(s_getNumberCallbackPtr);
        printf("RPC call example finished.\r\n\n\n");

        if (s_getNumberCallbackPtr == &getNumberFromCore1) {
            s_getNumberCallbackPtr = &getNumberFromCore0;
        } else {
            s_getNumberCallbackPtr = &getNumberFromCore1;
        }

        vTaskDelay(700);
    }

    vTaskDelete(s_client_task_handle);
}

static void server_task(void *param)
{
    (void)param;

    /* Wait for client initialization. */
    while (s_transportArbitrator == NULL) {
        vTaskDelay(100);
    }

    /* MessageBufferFactory initialization. */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(s_transport);

    /* eRPC server initialization */
    s_server               = erpc_server_init(s_transportArbitrator, message_buffer_factory);
    erpc_service_t service = create_Core1Interface_service();
    erpc_add_service_to_server(s_server, service);

    erpc_status_t status = erpc_server_run(s_server);

    /* handle error status */
    if (status != (erpc_status_t)kErpcStatus_Success) {
        /* print error description */
        printf("Error occurred in server task. Task end with %d\r\n", status);

        /* eRPC server de-initialization */
        erpc_remove_service_from_server(s_server, service);
        destroy_Core1Interface_service(service);
        erpc_server_deinit(s_server);
    }

    vTaskDelete(s_server_task_handle);
}

/*!
 * @brief Main function
 */
int main(void)
{
    board_init();
    ipc_init();
    ipc_enable_event_interrupt(2);

    if (xTaskCreate(client_task, "APP_TASK", configMINIMAL_STACK_SIZE + 256U, NULL, tskIDLE_PRIORITY + 1U, &s_client_task_handle) != pdPASS) {
        printf("\r\nFailed to create client task\r\n");
        return -1;
    }

    if (xTaskCreate(server_task, "APP_TASK", configMINIMAL_STACK_SIZE + 256U, NULL, tskIDLE_PRIORITY + 1U, &s_server_task_handle) != pdPASS) {
        printf("\r\nFailed to create server task\r\n");
        return -1;
    }

    vTaskStartScheduler();
}
