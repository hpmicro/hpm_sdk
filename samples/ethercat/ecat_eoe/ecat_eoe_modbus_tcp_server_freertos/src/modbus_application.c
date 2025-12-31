/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stdio.h"
#include "modbus_application.h"
#include "modbus_tcp_server.h"
#include "modbus_mem.h"
#include "agile_modbus.h"
#include "agile_modbus_tcp.h"
#include "agile_modbus_slave_util.h"
#include "FreeRTOS.h"
#include "task.h"

#define APP_MIN_RECV_LEN (8U)

/* Function declarations */
static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info);
static void modbus_application_task(void *pvParameters);
static void print_hold_register_val(void);

/* External references */
extern const agile_modbus_slave_util_map_t hold_register_maps[1];

/* Modbus slave utility configuration */
const agile_modbus_slave_util_t slave_util = {
    NULL, 0, NULL, 0, hold_register_maps, sizeof(hold_register_maps) / sizeof(hold_register_maps[0]), NULL, 0, addr_check, NULL, NULL
};

/* Modbus context buffers */
ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE agile_modbus_tcp_t ctx_tcp;

/* Global variables */
static TaskHandle_t modbus_task_handle = NULL;
static bool modbus_initialized = false;

/**
 * @brief Initialize Modbus TCP server application
 *
 * This function initializes the complete Modbus TCP server system including:
 * - Network TCP server initialization
 * - Modbus protocol stack initialization
 * - Modbus memory management with mutex protection
 * - Modbus application task creation
 *
 * The function sets up the Modbus slave with the configured address and
 * creates a task to handle Modbus protocol processing.
 */
void modbus_tcp_server_init(void)
{
    agile_modbus_t *ctx = &ctx_tcp._ctx;

    if (modbus_initialized) {
        return;
    }

    if (network_tcp_server_init() != status_success) {
        printf("ModBus TCP Server init fail\n");
        return;
    }

    agile_modbus_tcp_init(&ctx_tcp, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, MODBUS_SLAVE_ADDRESS);

    /* Initialize Modbus memory with mutex protection */
    modbus_mem_init();

    /* Create Modbus task */
    if (xTaskCreate(modbus_application_task, "Modbus_App_Task", MODBUS_APP_TASK_STACK_SIZE, NULL, MODBUS_APP_TASK_PRIORITY, &modbus_task_handle) != pdPASS) {
        printf("Failed to create Modbus task\n");
        return;
    }

    modbus_initialized = true;
    printf("ModBus TCP Server init success, slave address: %d\n", MODBUS_SLAVE_ADDRESS);
}

/**
 * @brief Check Modbus slave address
 *
 * This function validates if the received Modbus message is intended
 * for this slave. It checks against the configured slave address and
 * broadcast address.
 *
 * @param ctx Modbus context
 * @param slave_info Slave information structure
 * @return int 0 if address is valid, negative error code otherwise
 */
static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info)
{
    int slave = slave_info->sft->slave;
    if ((slave != ctx->slave) && (slave != AGILE_MODBUS_BROADCAST_ADDRESS) && (slave != 0xFF))
        return -AGILE_MODBUS_EXCEPTION_UNKNOW;

    return 0;
}

/**
 * @brief Modbus application task for protocol processing
 *
 * This task handles the main Modbus protocol processing loop:
 * - Receives Modbus data from the network layer
 * - Processes Modbus messages using the agile_modbus library
 * - Sends responses back to the client
 * - Handles various error conditions and connection states
 *
 * @param pvParameters Task parameters (unused)
 */
static void modbus_application_task(void *pvParameters)
{
    int recv_len, send_len;
    int rc;

    (void)pvParameters;

    while (1) {
        /* Try to receive Modbus data from network */
        recv_len = network_tcp_s_receive(ctx_tcp._ctx.read_buf, APP_MIN_RECV_LEN);

        if (recv_len > 0) {
            /* printf("Received Modbus data, length: %d\n", recv_len); */

            /* Process the received Modbus message */
            rc = agile_modbus_slave_handle(&ctx_tcp._ctx, recv_len, 0, agile_modbus_slave_util_callback, &slave_util, NULL);

            if (rc > 0) {
                printf("Parse ok, recv master msg len: %d\n", recv_len);
                print_hold_register_val();

                /* Send response */
                send_len = network_tcp_s_send(ctx_tcp._ctx.send_buf, rc);
                if (send_len < 0) {
                    printf("Failed to send response, error: %d\n", send_len);
                }
            } else {
                printf("Parse failed, code: %d\n", rc);
            }
        } else if (recv_len == -2) {
            printf("Wait for client connect...\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else if (recv_len == -3) {
            /* Timeout - no data received, this is normal */
            /* Add a small delay to prevent busy waiting */
            vTaskDelay(pdMS_TO_TICKS(10));
        } else if (recv_len < 0) {
            /* Error occurred */
            printf("Receive error: %d\n", recv_len);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        /* Small delay to prevent busy waiting */
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/**
 * @brief Print hold register values
 *
 * This function reads and prints the current values of the first 10
 * hold registers. It demonstrates how to access Modbus register data
 * through the register mapping system.
 */
static void print_hold_register_val(void)
{
    uint16_t register_val[10];
    hold_register_maps[0].get(register_val, 10);

    printf("modbus hold registers val: ");
    for (uint16_t i = 0; i < 10; i++) {
        printf("%d ", register_val[i]);
    }
    printf("\n");
}
