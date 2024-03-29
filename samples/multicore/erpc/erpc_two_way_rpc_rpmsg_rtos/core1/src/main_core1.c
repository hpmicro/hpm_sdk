/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_pmp_drv.h"
#include "erpc_arbitrated_client_setup.h"
#include "erpc_server_setup.h"
#include "erpc_two_way_rpc_Core1Interface.h"
#include "erpc_two_way_rpc_Core0Interface_server.h"
#include "rpmsg_lite.h"
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define APP_ERPC_READY_EVENT_DATA (1U)
#if !defined(SH_MEM_TOTAL_SIZE)
#define SH_MEM_TOTAL_SIZE (6144U)
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
ATTR_SHARE_MEM char rpmsg_lite_base[SH_MEM_TOTAL_SIZE];
static TaskHandle_t s_client_task_handle;
static TaskHandle_t s_server_task_handle;
static erpc_transport_t s_transportArbitrator;
static erpc_transport_t s_transport;
static getNumberCallback_t s_getNumberCallbackPtr;
static erpc_server_t s_server;
static uint32_t s_number;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SignalReady(void)
{
    /* Signal the other core we are ready by triggering the event and passing the APP_ERPC_READY_EVENT_DATA */
    (void)ipc_tigger_event(ipc_remote_start_event, APP_ERPC_READY_EVENT_DATA);
}

/* Implementation of RPC function setGetNumberFunction. */
void setGetNumberFunction(const getNumberCallback_t getNumberCallbackParam)
{
    s_getNumberCallbackPtr = getNumberCallbackParam;
}

/* Implementation of RPC function getGetNumberFunction. */
void getGetNumberFunction(getNumberCallback_t *getNumberCallbackParam)
{
    *getNumberCallbackParam = s_getNumberCallbackPtr;
}

/* Implementation of RPC function nestedCallGetNumber. */
void nestedCallGetNumber(const getNumberCallback_t getNumberCallbackParam)
{
    uint32_t number;
    getNumberCallbackParam(&number);
}

/* Implementation of RPC function getNumberFromCore1. */
void getNumberFromCore1(uint32_t *number)
{
    *number = s_number;
}

static void client_task(void *param)
{
    (void)param;

    /* ERPC client initialization */
    erpc_client_t erpc_client;

    /* RPMsg-Lite transport layer initialization */
    erpc_transport_t transport;

    transport = erpc_transport_rpmsg_lite_rtos_remote_init(101U, 100U, (void *)(char *)rpmsg_lite_base,
                                                            ERPC_TRANSPORT_RPMSG_LITE_LINK_ID, SignalReady, NULL);

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

    /* Simple synchronization system. To be sure that first core server is ready before rpc call. */
    vTaskDelay(500);

    while (1) {
        /* This client only requests increasing of number. */
        increaseNumber(&s_number);
        vTaskDelay(500);
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

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(s_transport);

    /* eRPC server initialization */
    s_server               = erpc_server_init(s_transportArbitrator, message_buffer_factory);
    erpc_service_t service = create_Core0Interface_service();
    erpc_add_service_to_server(s_server, service);

    /* process message */
    erpc_status_t status = erpc_server_run(s_server);

    /* handle error status */
    if (status != (erpc_status_t)kErpcStatus_Success) {
        /* eRPC server de-initialization */
        erpc_remove_service_from_server(s_server, service);
        destroy_Core0Interface_service(service);
        erpc_server_deinit(s_server);
    }

    vTaskDelete(s_server_task_handle);
}

/*!
 * @brief Main function
 */
int main(void)
{
    board_init_core1();
    ipc_init();
    ipc_enable_event_interrupt(2u);

    printf("Secondary core started...\r\n");

    if (xTaskCreate(client_task, "APP_TASK", configMINIMAL_STACK_SIZE + 256U, NULL, tskIDLE_PRIORITY + 1U, &s_client_task_handle) != pdPASS) {
        /* Failed to create application task */
        return -1;
    }

    if (xTaskCreate(server_task, "APP_TASK", configMINIMAL_STACK_SIZE + 256U, NULL, tskIDLE_PRIORITY + 1U, &s_server_task_handle) != pdPASS) {
        /* Failed to create application task */
        return -1;
    }

    vTaskStartScheduler();
}
