/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/errno.h"
#include "lwip/inet.h"
#include "hpm_common.h"
#include "modbus_tcp_server.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* TCP data structure for queue transmission */
typedef struct {
    uint8_t data[MODBUS_TCP_BUFF_SIZE];
    uint16_t len;
} tcp_data_t;

/* Global variables */
static int server_socket = -1;
static int client_socket = -1;
static TaskHandle_t tcp_task_handle = NULL;
static QueueHandle_t tcp_rx_queue = NULL;
static QueueHandle_t tcp_tx_queue = NULL;
static bool is_active = false;
static bool is_initialized = false;

/* Function declarations */
static void modbus_tcp_server_task(void *pvParameters);

/**
 * @brief Initialize Modbus TCP server
 *
 * This function initializes the TCP server socket, creates queues for data transmission,
 * and starts the TCP server task. The server will listen on the configured port
 * for incoming Modbus TCP connections.
 *
 * @return hpm_stat_t status_success if initialization successful, status_fail otherwise
 */
hpm_stat_t network_tcp_server_init(void)
{
    struct sockaddr_in server_addr;
    int opt = 1;

    if (is_initialized) {
        return status_success;
    }

    /* Create server socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Failed to create server socket\n");
        return status_fail;
    }

    /* Set socket options */
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("Failed to set socket options\n");
        close(server_socket);
        return status_fail;
    }

    /* Configure server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(MODBUS_TCP_SERVER_PORT);

    /* Bind socket */
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to bind server socket\n");
        close(server_socket);
        return status_fail;
    }

    /* Listen for connections */
    if (listen(server_socket, 1) < 0) {
        printf("Failed to listen on server socket\n");
        close(server_socket);
        return status_fail;
    }

    /* Create queues */
    tcp_rx_queue = xQueueCreate(MODBUS_TCP_QUEUE_SIZE, sizeof(tcp_data_t));
    tcp_tx_queue = xQueueCreate(MODBUS_TCP_QUEUE_SIZE, sizeof(tcp_data_t));

    if (tcp_rx_queue == NULL || tcp_tx_queue == NULL) {
        printf("Failed to create queues\n");
        if (tcp_rx_queue) {
            vQueueDelete(tcp_rx_queue);
        }
        if (tcp_tx_queue) {
            vQueueDelete(tcp_tx_queue);
        }
        close(server_socket);
        return status_fail;
    }

    /* Create TCP server task */
    if (xTaskCreate(modbus_tcp_server_task, "ModbusTCP", MODBUS_TCP_TASK_STACK_SIZE, NULL, MODBUS_TCP_TASK_PRIORITY, &tcp_task_handle) != pdPASS) {
        printf("Failed to create TCP server task\n");
        vQueueDelete(tcp_rx_queue);
        vQueueDelete(tcp_tx_queue);
        close(server_socket);
        return status_fail;
    }

    printf("Modbus TCP Server Port: %d\n", MODBUS_TCP_SERVER_PORT);
    is_initialized = true;

    return status_success;
}

/**
 * @brief Receive data from TCP connection
 *
 * This function receives data from the TCP connection through the receive queue.
 * It waits for data with a timeout and returns the actual data length available.
 * If the requested length is not available, it returns an error.
 *
 * @param data Pointer to buffer where received data will be stored
 * @param len Requested data length to receive
 * @return int >0: actual data length received, <0: error code
 *         -1: parameter error, -2: connection not active, -3: timeout, -4: insufficient data
 */
int network_tcp_s_receive(uint8_t *data, int len)
{
    tcp_data_t rx_data;
    BaseType_t result;

    if (data == NULL || len <= 0) {
        return -1;
    }

    if (!is_active) {
        return -2;
    }

    /* Try to receive data from queue with timeout */
    result = xQueueReceive(tcp_rx_queue, &rx_data, pdMS_TO_TICKS(100));
    if (result != pdTRUE) {
        return -3; /* Timeout */
    }

    /* Check if we have enough data to satisfy the request */
    if ((int)rx_data.len < len) {
        /* Not enough data available, put data back and return error */
        xQueueSendToFront(tcp_rx_queue, &rx_data, 0);
        return -4; /* Insufficient data */
    }

    /* Copy all available data (same as baremetal version) */
    memcpy(data, rx_data.data, rx_data.len);

    /* Return the actual data length that was available (same as baremetal version) */
    return rx_data.len;
}

/**
 * @brief Flush receive buffer
 *
 * This function clears the receive queue, discarding all pending data.
 * Useful for resetting the connection state.
 */
void network_tcp_s_flush(void)
{
    /* Clear receive queue */
    xQueueReset(tcp_rx_queue);
}

/**
 * @brief Send data through TCP connection
 *
 * This function sends data through the TCP connection by queuing it for transmission.
 * The actual transmission is handled by the TCP server task.
 *
 * @param data Pointer to data buffer to send
 * @param len Length of data to send
 * @return int >0: data length queued for transmission, <0: error code
 *         -1: parameter error, -2: connection not active, -3: data too large, -4: queue full
 */
int network_tcp_s_send(uint8_t *data, uint32_t len)
{
    tcp_data_t tx_data;
    BaseType_t result;

    if (data == NULL || len == 0) {
        return -1;
    }

    if (!is_active) {
        return -2;
    }

    if (len > MODBUS_TCP_BUFF_SIZE) {
        return -3; /* Data too large */
    }

    /* Prepare data for transmission */
    memcpy(tx_data.data, data, len);
    tx_data.len = len;

    /* Send data to transmission queue */
    result = xQueueSend(tcp_tx_queue, &tx_data, pdMS_TO_TICKS(100));
    if (result != pdTRUE) {
        return -4; /* Queue full or timeout */
    }

    return len;
}

/**
 * @brief TCP server task for handling network communication
 *
 * This task handles the TCP server operations including:
 * - Accepting client connections
 * - Receiving data from clients and queuing it for application processing
 * - Sending data from application to clients
 * - Managing connection state and error handling
 *
 * @param pvParameters Task parameters (unused)
 */
static void modbus_tcp_server_task(void *pvParameters)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    tcp_data_t rx_data, tx_data;
    BaseType_t queue_result;
    int recv_len, send_len;
    fd_set read_fds;
    struct timeval timeout;

    (void)pvParameters;

    while (1) {
        /* Accept client connection */
        if (client_socket < 0) {
            printf("Modbus TCP Server Listening...\n");
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket >= 0) {
                is_active = true;
                printf("Client connected\n");
            } else {
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }
        }

        /* Set up file descriptor set for select */
        FD_ZERO(&read_fds);
        FD_SET(client_socket, &read_fds);

        /* Set timeout for select */
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; /* 100ms */

        /* Check for incoming data */
        if (select(client_socket + 1, &read_fds, NULL, NULL, &timeout) > 0) {
            if (FD_ISSET(client_socket, &read_fds)) {
                recv_len = recv(client_socket, rx_data.data, MODBUS_TCP_BUFF_SIZE, 0);
                if (recv_len > 0) {
                    rx_data.len = recv_len;
                    /* Send received data to application queue */
                    queue_result = xQueueSend(tcp_rx_queue, &rx_data, 0);
                    if (queue_result != pdTRUE) {
                        printf("RX queue full, dropping data\n");
                    }
                } else if (recv_len == 0) {
                    /* Client disconnected */
                    printf("Client disconnected\n");
                    close(client_socket);
                    client_socket = -1;
                    is_active = false;
                    continue;
                } else {
                    /* Error occurred */
                    printf("Recv error\n");
                    close(client_socket);
                    client_socket = -1;
                    is_active = false;
                    continue;
                }
            }
        }

        /* Check for outgoing data */
        queue_result = xQueueReceive(tcp_tx_queue, &tx_data, 0);
        if (queue_result == pdTRUE) {
            send_len = send(client_socket, tx_data.data, tx_data.len, 0);
            if (send_len < 0) {
                printf("Send error\n");
                close(client_socket);
                client_socket = -1;
                is_active = false;
            }
        }

        /* Small delay to prevent busy waiting */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
