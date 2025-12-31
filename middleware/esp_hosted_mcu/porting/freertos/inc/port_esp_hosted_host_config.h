/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ESP_HOSTED_CONFIG_H__
#define __ESP_HOSTED_CONFIG_H__


#include "esp_err.h"
#include "board.h"
#include "hpm_common.h"
#include "hpm_gpio_drv.h"
#ifdef CONFIG_ESP_HOSTED_ENABLED
#define H_ESP_HOSTED_HOST 1
#endif

// to allow external code to override Hosted Functions if required
#define H_WEAK_REF __attribute__((weak))

#define H_TRANSPORT_NONE 0
#define H_TRANSPORT_SDIO 1
#define H_TRANSPORT_SPI_HD 2
#define H_TRANSPORT_SPI 3
#define H_TRANSPORT_UART 4

/* This file is to tune the main ESP-Hosted configurations.
 * In case you are not sure of some value, Let it be default.
 **/

#define H_GPIO_LOW 0
#define H_GPIO_HIGH 1

#define H_ENABLE 1
#define H_DISABLE 0


/* Slave chipset */
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32
#define CONFIG_SLAVE_IDF_TARGET_ESP32 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32S2
#define CONFIG_SLAVE_IDF_TARGET_ESP32S2 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C3
#define CONFIG_SLAVE_IDF_TARGET_ESP32C3 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32S3
#define CONFIG_SLAVE_IDF_TARGET_ESP32S3 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C6
#define CONFIG_SLAVE_IDF_TARGET_ESP32C6 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C2
#define CONFIG_SLAVE_IDF_TARGET_ESP32C2 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C61
#define CONFIG_SLAVE_IDF_TARGET_ESP32C61 0
#endif
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C5
#define CONFIG_SLAVE_IDF_TARGET_ESP32C5 0
#endif

/* Define H_SLAVE_TARGET_XXX macros if not already defined */
#ifndef H_SLAVE_TARGET_ESP32
#define H_SLAVE_TARGET_ESP32   (CONFIG_SLAVE_IDF_TARGET_ESP32)
#endif
#ifndef H_SLAVE_TARGET_ESP32S2
#define H_SLAVE_TARGET_ESP32S2 (CONFIG_SLAVE_IDF_TARGET_ESP32S2)
#endif
#ifndef H_SLAVE_TARGET_ESP32C3
#define H_SLAVE_TARGET_ESP32C3 (CONFIG_SLAVE_IDF_TARGET_ESP32C3)
#endif
#ifndef H_SLAVE_TARGET_ESP32S3
#define H_SLAVE_TARGET_ESP32S3 (CONFIG_SLAVE_IDF_TARGET_ESP32S3)
#endif
#ifndef H_SLAVE_TARGET_ESP32C6
#define H_SLAVE_TARGET_ESP32C6 (CONFIG_SLAVE_IDF_TARGET_ESP32C6)
#endif
#ifndef H_SLAVE_TARGET_ESP32C2
#define H_SLAVE_TARGET_ESP32C2 (CONFIG_SLAVE_IDF_TARGET_ESP32C2)
#endif
#ifndef H_SLAVE_TARGET_ESP32C61
#define H_SLAVE_TARGET_ESP32C61 (CONFIG_SLAVE_IDF_TARGET_ESP32C61)
#endif
#ifndef H_SLAVE_TARGET_ESP32C5
#define H_SLAVE_TARGET_ESP32C5 (CONFIG_SLAVE_IDF_TARGET_ESP32C5)
#endif

/* WiFi NAN support */
#ifndef CONFIG_SOC_WIFI_NAN_SUPPORT
#if H_SLAVE_TARGET_ESP32 || H_SLAVE_TARGET_ESP32S2 || H_SLAVE_TARGET_ESP32C3 || \
    H_SLAVE_TARGET_ESP32S3 || H_SLAVE_TARGET_ESP32C6 || H_SLAVE_TARGET_ESP32C2 || \
    H_SLAVE_TARGET_ESP32C61 || H_SLAVE_TARGET_ESP32C5
#define CONFIG_SOC_WIFI_NAN_SUPPORT 1
#else
#define CONFIG_SOC_WIFI_NAN_SUPPORT 0
#endif
#endif

/* WiFi enabled */
#ifndef CONFIG_SOC_WIFI_ENABLED
#if H_SLAVE_TARGET_ESP32 || H_SLAVE_TARGET_ESP32S2 || H_SLAVE_TARGET_ESP32C3 || \
    H_SLAVE_TARGET_ESP32S3 || H_SLAVE_TARGET_ESP32C6 || H_SLAVE_TARGET_ESP32C2 || \
    H_SLAVE_TARGET_ESP32C61 || H_SLAVE_TARGET_ESP32C5
#define CONFIG_SOC_WIFI_ENABLED 1
#else
#define CONFIG_SOC_WIFI_ENABLED 0
#endif
#endif

/* 5G WiFi support only for ESP32C5 */
#ifndef CONFIG_SOC_WIFI_SUPPORT_5G
#if H_SLAVE_TARGET_ESP32C5
#define CONFIG_SOC_WIFI_SUPPORT_5G 1
#else
#define CONFIG_SOC_WIFI_SUPPORT_5G 0
#endif
#endif

#ifndef CONFIG_SOC_WIFI_HE_SUPPORT
#if H_SLAVE_TARGET_ESP32C6
#define CONFIG_SOC_WIFI_HE_SUPPORT 1
#else
#define CONFIG_SOC_WIFI_HE_SUPPORT 0
#endif
#endif

#ifndef CONFIG_SOC_WIFI_SUPPORTED
#if H_SLAVE_TARGET_ESP32 || H_SLAVE_TARGET_ESP32S2 || H_SLAVE_TARGET_ESP32C3 || \
    H_SLAVE_TARGET_ESP32S3 || H_SLAVE_TARGET_ESP32C6 || H_SLAVE_TARGET_ESP32C2 || \
    H_SLAVE_TARGET_ESP32C61 || H_SLAVE_TARGET_ESP32C5
#define CONFIG_SOC_WIFI_SUPPORTED 1
#else
#define CONFIG_SOC_WIFI_SUPPORTED 0
#endif
#endif

#if H_SLAVE_TARGET_ESP32C2
#define ESP_WIFI_MAX_CONN_NUM          (4) /**< max number of stations which can connect to ESP32C2 soft-AP */
#elif H_SLAVE_TARGET_ESP32 || H_SLAVE_TARGET_ESP32C6 ||  H_SLAVE_TARGET_ESP32C5 || H_SLAVE_TARGET_ESP32C61 || H_SLAVE_TARGET_ESP32C3
#define ESP_WIFI_MAX_CONN_NUM         (10) /**< max number of stations which can connect to
            ESP32C3/ESP32C6/ESP32C5/ESP32C61 soft-AP */
#elif H_SLAVE_TARGET_ESP32 || H_SLAVE_TARGET_ESP32S3 || H_SLAVE_TARGET_ESP32S2
#define ESP_WIFI_MAX_CONN_NUM                                                  \
    (15) /**< max number of stations which can connect to                      \
            ESP32/ESP32S3/ESP32S2 soft-AP */
#else
#error "Unknown Slave Target"
#endif

#ifndef CONFIG_ESP_HOSTED_USE_MEMPOOL
#define CONFIG_ESP_HOSTED_USE_MEMPOOL 0
#endif
#if CONFIG_ESP_HOSTED_USE_MEMPOOL
#define H_USE_MEMPOOL 1
#endif

#ifndef H_MAX_SYNC_RPC_REQUESTS
#define H_MAX_SYNC_RPC_REQUESTS 8
#endif
#ifndef H_MAX_ASYNC_RPC_REQUESTS
#define H_MAX_ASYNC_RPC_REQUESTS 8
#endif

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL  ESP_LOG_INFO
#endif
#ifndef CONFIG_LOG_MAXIMUM_LEVEL
#define CONFIG_LOG_MAXIMUM_LEVEL  ESP_LOG_INFO
#endif

#ifndef CONFIG_LOG_COLORS
#define CONFIG_LOG_COLORS 1
#endif
#ifndef CONFIG_LOW_MEMORY_HOST
#define CONFIG_LOW_MEMORY_HOST 0
#endif

#ifndef CONFIG_ESP_SUPP_DPP_SUPPORT
#define CONFIG_ESP_SUPP_DPP_SUPPORT 0
#endif
#define H_SUPP_DPP_SUPPORT  CONFIG_ESP_SUPP_DPP_SUPPORT

#ifndef CONFIG_ESP_WIFI_DPP_SUPPORT
#define CONFIG_ESP_WIFI_DPP_SUPPORT 0
#endif
#define H_WIFI_DPP_SUPPORT CONFIG_ESP_WIFI_DPP_SUPPORT

#ifndef CONFIG_ESP_HOSTED_WIFI_MAX_RX_SIZE
#define CONFIG_ESP_HOSTED_WIFI_MAX_RX_SIZE (3200U)
#endif

#ifndef CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT
#define CONFIG_ESP_HOSTED_WIFI_NETIF_COUNT  (2U)
#endif

#undef H_TRANSPORT_IN_USE

#ifdef CONFIG_ESP_HOSTED_SPI_HOST_INTERFACE
#define H_TRANSPORT_IN_USE H_TRANSPORT_SPI
/*  -------------------------- SPI Master Config start ---------------------- */
/*  Pins in use. The SPI Master can use the GPIO mux,
    so feel free to change these if needed.
*/

/* SPI config */

#ifndef CONFIG_ESP_HOSTED_SPI_HANDSHAKE_ACTIVE_HIGH
#define CONFIG_ESP_HOSTED_SPI_HANDSHAKE_ACTIVE_HIGH 1
#endif
#define H_HANDSHAKE_ACTIVE_HIGH CONFIG_ESP_HOSTED_SPI_HANDSHAKE_ACTIVE_HIGH

#ifndef CONFIG_ESP_HOSTED_SPI_DATAREADY_ACTIVE_HIGH
#define CONFIG_ESP_HOSTED_SPI_DATAREADY_ACTIVE_HIGH 1
#endif
#define H_DATAREADY_ACTIVE_HIGH CONFIG_ESP_HOSTED_SPI_DATAREADY_ACTIVE_HIGH


#if H_HANDSHAKE_ACTIVE_HIGH
#define H_HS_VAL_ACTIVE H_GPIO_HIGH
#define H_HS_VAL_INACTIVE H_GPIO_LOW
#define H_HS_INTR_EDGE gpio_interrupt_trigger_edge_rising
#else
#define H_HS_VAL_ACTIVE H_GPIO_LOW
#define H_HS_VAL_INACTIVE H_GPIO_HIGH
#define H_HS_INTR_EDGE gpio_interrupt_trigger_edge_falling
#endif

#if H_DATAREADY_ACTIVE_HIGH
#define H_DR_VAL_ACTIVE H_GPIO_HIGH
#define H_DR_VAL_INACTIVE H_GPIO_LOW
#define H_DR_INTR_EDGE gpio_interrupt_trigger_edge_rising
#else
#define H_DR_VAL_ACTIVE H_GPIO_LOW
#define H_DR_VAL_INACTIVE H_GPIO_HIGH
#define H_DR_INTR_EDGE gpio_interrupt_trigger_edge_falling
#endif

#ifndef CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_Port
#define CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_Port NULL
#endif
#ifndef CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN
#define CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN 0
#endif
#define H_GPIO_HANDSHAKE_Port CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_Port
#define H_GPIO_HANDSHAKE_Pin CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN

#ifndef CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_Port
#define CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_Port NULL
#endif
#ifndef CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN
#define CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN 0
#endif
#define H_GPIO_DATA_READY_Port CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_Port
#define H_GPIO_DATA_READY_Pin CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN

#ifndef H_GPIO_MOSI_Port
#define H_GPIO_MOSI_Port NULL
#endif

#ifndef H_GPIO_MOSI_Pin
#define H_GPIO_MOSI_Pin 0
#endif

#ifndef H_GPIO_MISO_Port
#define H_GPIO_MISO_Port NULL
#endif

#ifndef H_GPIO_MISO_Pin
#define H_GPIO_MISO_Pin 0
#endif

#ifndef H_GPIO_SCLK_Port
#define H_GPIO_SCLK_Port 0
#endif

#ifndef H_GPIO_SCLK_Pin
#define H_GPIO_SCLK_Pin 0
#endif

#ifndef H_GPIO_CS_Port
#define H_GPIO_CS_Port NULL
#endif

#ifndef CONFIG_ESP_HOSTED_SPI_GPIO_CS_PIN
#define CONFIG_ESP_HOSTED_SPI_GPIO_CS_PIN 0
#endif
#define H_GPIO_CS_Pin CONFIG_ESP_HOSTED_SPI_GPIO_CS_PIN

#ifndef H_SPI_TX_Q
#define H_SPI_TX_Q 2000
#endif
#ifndef H_SPI_RX_Q
#define H_SPI_RX_Q 2000
#endif

#ifndef CONFIG_ESP_HOSTED_SPI_MODE
#define CONFIG_ESP_HOSTED_SPI_MODE 2
#endif
#define H_SPI_MODE CONFIG_ESP_HOSTED_SPI_MODE


#ifndef CONFIG_ESP_HOSTED_SPI_CLOCK_FREQ_MHZ
#define CONFIG_ESP_HOSTED_SPI_CLOCK_FREQ_MHZ 20
#endif
#define H_SPI_FD_CLK_MHZ CONFIG_ESP_HOSTED_SPI_CLOCK_FREQ_MHZ

/*  -------------------------- SPI Master Config end ------------------------ */
#endif

#ifdef CONFIG_ESP_HOSTED_SDIO_HOST_INTERFACE
#define H_TRANSPORT_IN_USE H_TRANSPORT_SDIO
/*  -------------------------- SDIO Host Config start ----------------------- */

#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
#define H_SDIO_SOC_USE_GPIO_MATRIX
#endif

#define H_SDIO_CLOCK_FREQ_KHZ CONFIG_ESP_HOSTED_SDIO_CLOCK_FREQ_KHZ
#define H_SDIO_BUS_WIDTH CONFIG_ESP_HOSTED_SDIO_BUS_WIDTH
#define H_SDMMC_HOST_SLOT CONFIG_ESP_HOSTED_SDIO_SLOT

#define H_SDIO_PORT_CLK NULL
#define H_SDIO_PORT_CMD NULL
#define H_SDIO_PORT_D0 NULL
#define H_SDIO_PORT_D1 NULL
#define H_SDIO_PORT_D2 NULL
#define H_SDIO_PORT_D3 NULL

#ifdef H_SDIO_SOC_USE_GPIO_MATRIX
#define H_SDIO_PIN_CLK CONFIG_ESP_HOSTED_SDIO_PIN_CLK
#define H_SDIO_PIN_CMD CONFIG_ESP_HOSTED_SDIO_PIN_CMD
#define H_SDIO_PIN_D0 CONFIG_ESP_HOSTED_SDIO_PIN_D0
#define H_SDIO_PIN_D1 CONFIG_ESP_HOSTED_SDIO_PIN_D1
#if (H_SDIO_BUS_WIDTH == 4)
#define H_SDIO_PIN_D2 CONFIG_ESP_HOSTED_SDIO_PIN_D2
#define H_SDIO_PIN_D3 CONFIG_ESP_HOSTED_SDIO_PIN_D3
#else
#define H_SDIO_PIN_D2 -1
#define H_SDIO_PIN_D3 -1
#endif
#else
#define H_SDIO_PIN_CLK -1
#define H_SDIO_PIN_CMD -1
#define H_SDIO_PIN_D0 -1
#define H_SDIO_PIN_D1 -1
#if (H_SDIO_BUS_WIDTH == 4)
#define H_SDIO_PIN_D2 -1
#define H_SDIO_PIN_D3 -1
#else
#define H_SDIO_PIN_D2 -1
#define H_SDIO_PIN_D3 -1
#endif
#endif

#define H_SDIO_TX_Q CONFIG_ESP_HOSTED_SDIO_TX_Q_SIZE
#define H_SDIO_RX_Q CONFIG_ESP_HOSTED_SDIO_RX_Q_SIZE

#define H_SDIO_CHECKSUM CONFIG_ESP_HOSTED_SDIO_CHECKSUM

#define H_SDIO_HOST_STREAMING_MODE 1
#define H_SDIO_ALWAYS_HOST_RX_MAX_TRANSPORT_SIZE 2
#define H_SDIO_OPTIMIZATION_RX_NONE 3

#ifdef CONFIG_ESP_HOSTED_SDIO_OPTIMIZATION_RX_STREAMING_MODE
#define H_SDIO_HOST_RX_MODE H_SDIO_HOST_STREAMING_MODE
#elif defined(CONFIG_ESP_HOSTED_SDIO_OPTIMIZATION_RX_MAX_SIZE)
#define H_SDIO_HOST_RX_MODE H_SDIO_ALWAYS_HOST_RX_MAX_TRANSPORT_SIZE
#else
/* Use this if unsure */
#define H_SDIO_HOST_RX_MODE H_SDIO_OPTIMIZATION_RX_NONE
#endif

// Pad transfer len for host operation
#define H_SDIO_TX_LEN_TO_TRANSFER(x) ((x + 3) & (~3))
#define H_SDIO_RX_LEN_TO_TRANSFER(x) ((x + 3) & (~3))

/* Do Block Mode only transfers
 *
 * When enabled, SDIO only uses block mode transfers for higher
 * throughput. Data lengths are padded to multiples of ESP_BLOCK_SIZE.
 *
 * This is safe for the SDIO slave:
 * - for Host Tx: slave will ignore extra data sent by Host
 * - for Host Rx: slave will send extra 0 data, ignored by Host
 */
#define H_SDIO_TX_BLOCK_ONLY_XFER (1)
#define H_SDIO_RX_BLOCK_ONLY_XFER (1)

// workarounds for some SDIO transfer errors that may occur
#if 0
    /* Below workarounds could be enabled for non-ESP MCUs to test first
     * Once everything is stable, can disable workarounds and test again
     * */
#define H_SDIO_TX_LIMIT_XFER_SIZE_WORKAROUND  // limit transfer to one
                                              // ESP_BLOCK_SIZE at a time
#define H_SDIO_RX_LIMIT_XFER_SIZE_WORKDAROUND // limit transfer to one
                                              // ESP_BLOCK_SIZE at a time
#endif

#if defined(H_SDIO_TX_LIMIT_XFER_SIZE_WORKAROUND)
#define H_SDIO_TX_BLOCKS_TO_TRANSFER(x) (1)
#else
#define H_SDIO_TX_BLOCKS_TO_TRANSFER(x) (x / ESP_BLOCK_SIZE)
#endif

#if defined(H_SDIO_RX_LIMIT_XFER_SIZE_WORKDAROUND)
#define H_SDIO_RX_BLOCKS_TO_TRANSFER(x) (1)
#else
#define H_SDIO_RX_BLOCKS_TO_TRANSFER(x) (x / ESP_BLOCK_SIZE)
#endif

/*  -------------------------- SDIO Host Config end ------------------------- */
#endif

#ifdef CONFIG_ESP_HOSTED_SPI_HD_HOST_INTERFACE
#define H_TRANSPORT_IN_USE H_TRANSPORT_SPI_HD
/*  -------------------------- SPI_HD Host Config start -----------------------
 */

#define H_SPI_HD_HOST_INTERFACE 1

enum {
    H_SPI_HD_CONFIG_2_DATA_LINES,
    H_SPI_HD_CONFIG_4_DATA_LINES,
};

#if CONFIG_ESP_HOSTED_SPI_HD_DR_ACTIVE_HIGH
#define H_SPI_HD_DATAREADY_ACTIVE_HIGH 1
#else
#define H_SPI_HD_DATAREADY_ACTIVE_HIGH 0
#endif

#if H_SPI_HD_DATAREADY_ACTIVE_HIGH
#define H_SPI_HD_DR_VAL_ACTIVE H_GPIO_HIGH
#define H_SPI_HD_DR_VAL_INACTIVE H_GPIO_LOW
#define H_SPI_HD_DR_INTR_EDGE gpio_interrupt_trigger_edge_rising
#else
#define H_SPI_HD_DR_VAL_ACTIVE H_GPIO_LOW
#define H_SPI_HD_DR_VAL_INACTIVE H_GPIO_HIGH
#define H_SPI_HD_DR_INTR_EDGE gpio_interrupt_trigger_edge_falling
#endif

#define H_SPI_HD_HOST_NUM_DATA_LINES                                           \
    CONFIG_ESP_HOSTED_SPI_HD_INTERFACE_NUM_DATA_LINES

#define H_SPI_HD_PORT_D0 NULL
#define H_SPI_HD_PORT_D1 NULL
#define H_SPI_HD_PORT_D2 NULL
#define H_SPI_HD_PORT_D3 NULL
#define H_SPI_HD_PORT_CS NULL
#define H_SPI_HD_PORT_CLK NULL

#define H_SPI_HD_PIN_D0 CONFIG_ESP_HOSTED_SPI_HD_GPIO_D0
#define H_SPI_HD_PIN_D1 CONFIG_ESP_HOSTED_SPI_HD_GPIO_D1
#if (CONFIG_ESP_HOSTED_SPI_HD_INTERFACE_NUM_DATA_LINES == 4)
#define H_SPI_HD_PIN_D2 CONFIG_ESP_HOSTED_SPI_HD_GPIO_D2
#define H_SPI_HD_PIN_D3 CONFIG_ESP_HOSTED_SPI_HD_GPIO_D3
#else
#define H_SPI_HD_PIN_D2 -1
#define H_SPI_HD_PIN_D3 -1
#endif

#define H_SPI_HD_PIN_CS CONFIG_ESP_HOSTED_SPI_HD_GPIO_CS
#define H_SPI_HD_PIN_CLK CONFIG_ESP_HOSTED_SPI_HD_GPIO_CLK
#define H_SPI_HD_PORT_DATA_READY NULL
#define H_SPI_HD_PIN_DATA_READY CONFIG_ESP_HOSTED_SPI_HD_GPIO_DATA_READY

#define H_SPI_HD_CLK_MHZ CONFIG_ESP_HOSTED_SPI_HD_CLK_FREQ
#define H_SPI_HD_MODE CONFIG_ESP_HOSTED_SPI_HD_MODE
#define H_SPI_HD_TX_QUEUE_SIZE CONFIG_ESP_HOSTED_SPI_HD_TX_Q_SIZE
#define H_SPI_HD_RX_QUEUE_SIZE CONFIG_ESP_HOSTED_SPI_HD_RX_Q_SIZE

#define H_SPI_HD_CHECKSUM CONFIG_ESP_HOSTED_SPI_HD_CHECKSUM

#define H_SPI_HD_NUM_COMMAND_BITS 8
#define H_SPI_HD_NUM_ADDRESS_BITS 8
#define H_SPI_HD_NUM_DUMMY_BITS 8

/*  -------------------------- SPI_HD Host Config end -------------------------
 */
#else
#define H_SPI_HD_HOST_INTERFACE 0
#endif

#ifdef CONFIG_ESP_HOSTED_UART_HOST_INTERFACE
#define H_TRANSPORT_IN_USE H_TRANSPORT_UART
/*  -------------------------- UART Host Config start -------------------------
 */

#define H_UART_HOST_TRANSPORT 1

#define H_UART_PORT CONFIG_ESP_HOSTED_UART_PORT
#define H_UART_NUM_DATA_BITS CONFIG_ESP_HOSTED_UART_NUM_DATA_BITS
#define H_UART_PARITY CONFIG_ESP_HOSTED_UART_PARITY
#define H_UART_START_BITS 1
#define H_UART_STOP_BITS CONFIG_ESP_HOSTED_UART_STOP_BITS
#define H_UART_FLOWCTRL UART_HW_FLOWCTRL_DISABLE
#define H_UART_CLK_SRC UART_SCLK_DEFAULT

#define H_UART_CHECKSUM CONFIG_ESP_HOSTED_UART_CHECKSUM
#define H_UART_BAUD_RATE CONFIG_ESP_HOSTED_UART_BAUDRATE
#define H_UART_PIN_TX CONFIG_ESP_HOSTED_UART_PIN_TX
#define H_UART_PORT_TX NULL
#define H_UART_PIN_RX CONFIG_ESP_HOSTED_UART_PIN_RX
#define H_UART_PORT_RX NULL
#define H_UART_TX_QUEUE_SIZE CONFIG_ESP_HOSTED_UART_TX_Q_SIZE
#define H_UART_RX_QUEUE_SIZE CONFIG_ESP_HOSTED_UART_RX_Q_SIZE

/*  -------------------------- UART Host Config end ------------------------- */
#else
#define H_UART_HOST_TRANSPORT 0
#endif

/* Generic reset pin config */
#ifndef CONFIG_ESP_HOSTED_RESET_GPIO_PIN_RESET
#define CONFIG_ESP_HOSTED_RESET_GPIO_PIN_RESET 0
#endif
#define H_GPIO_PIN_RESET CONFIG_ESP_HOSTED_RESET_GPIO_PIN_RESET


#ifndef H_GPIO_PORT_RESET
#define H_GPIO_PORT_RESET NULL
#endif

/* If Reset pin is Enable, it is Active High.
 * If it is RST, active low */
#ifndef CONFIG_ESP_HOSTED_RESET_GPIO_ACTIVE_LOW
#define CONFIG_ESP_HOSTED_RESET_GPIO_ACTIVE_LOW 0
#endif

#if CONFIG_ESP_HOSTED_RESET_GPIO_ACTIVE_LOW
#define H_RESET_ACTIVE_HIGH 0
#else
#define H_RESET_ACTIVE_HIGH 1
#endif

#if H_RESET_ACTIVE_HIGH
#define H_RESET_VAL_ACTIVE H_GPIO_HIGH
#define H_RESET_VAL_INACTIVE H_GPIO_LOW
#else
#define H_RESET_VAL_ACTIVE H_GPIO_LOW
#define H_RESET_VAL_INACTIVE H_GPIO_HIGH
#endif

/* --------------------- Common slave reset strategy ------------------- */

#if defined(CONFIG_ESP_HOSTED_SLAVE_RESET_ON_EVERY_HOST_BOOTUP)
/* Always reset the slave when host boots up
 * This ensures a clean transport state and prevents any inconsistent states,
 * but causes the slave to reboot every time the host boots up
 */
#define H_SLAVE_RESET_ON_EVERY_HOST_BOOTUP 1
#define H_SLAVE_RESET_ONLY_IF_NECESSARY 0
#elif defined(CONFIG_ESP_HOSTED_SLAVE_RESET_ONLY_IF_NECESSARY)
/* Only reset the slave if initialization fails
 * This reduces slave reboots but assumes the slave interface is in a consistent
 * state. If initialization fails, the host will assume the slave is in an
 * inconsistent or deinitialized state and will reset it.
 */
#define H_SLAVE_RESET_ON_EVERY_HOST_BOOTUP 0
#define H_SLAVE_RESET_ONLY_IF_NECESSARY 1
#else
/* Default to always reset for backward compatibility */
#define H_SLAVE_RESET_ON_EVERY_HOST_BOOTUP 1
#define H_SLAVE_RESET_ONLY_IF_NECESSARY 0
#endif

#if !defined(CONFIG_ESP_SDIO_HOST_INTERFACE) && H_SLAVE_RESET_ONLY_IF_NECESSARY
#error                                                                         \
    "Invalid combination. H_SLAVE_RESET_ONLY_IF_NECESSARY is only supported for SDIO host interface, for now"
#endif

/* Auto-restart on communication failure */
#ifdef CONFIG_ESP_HOSTED_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE
/* Enable host auto-restart if communication with slave is lost
 * When enabled, the host will reset itself to recover the connection
 * if the slave becomes non-responsive for the configured timeout period.
 * This acts as a safeguard in case the slave does not issue the first event.
 */
#define H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE 1
#else
/* Disable host auto-restart on communication failure */
#define H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE 0
#endif

#if defined(CONFIG_ESP_HOSTED_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE_TIMEOUT)
/* Timeout in seconds before host restarts due to no communication
 * Maximum time that the host will wait for a response from the slave
 * before triggering an automatic restart.
 */
#define H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE_TIMEOUT                     \
    CONFIG_ESP_HOSTED_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE_TIMEOUT
#else
/* Default timeout value (-1 means disabled) */
#define H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE_TIMEOUT -1
#endif

#if H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE &&                              \
    H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE_TIMEOUT == -1
#error                                                                         \
    "Invalid combination. Host Restart No Communication With Slave is enabled but timeout is not configured"
#endif

#if H_SLAVE_RESET_ON_EVERY_HOST_BOOTUP && H_SLAVE_RESET_ONLY_IF_NECESSARY
#error                                                                         \
    "Invalid combination. Reset on every bootup and reset only if necessary cannot be enabled at the same time"
#endif

#if H_SLAVE_RESET_ON_EVERY_HOST_BOOTUP &&                                      \
    H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE
#error                                                                         \
    "Invalid combination. H_HOST_RESTART_NO_COMMUNICATION_WITH_SLAVE should not be logically required if H_SLAVE_RESET_ONLY_IF_NECESSARY is enabled"
#endif

#define TIMEOUT_PSERIAL_RESP 30

#define PRE_FORMAT_NEWLINE_CHAR ""
#define POST_FORMAT_NEWLINE_CHAR "\n"

#define USE_STD_C_LIB_MALLOC 0

#ifdef CONFIG_HOST_TO_ESP_WIFI_DATA_THROTTLE
#define H_WIFI_TX_DATA_THROTTLE_LOW_THRESHOLD                                  \
    CONFIG_ESP_HOSTED_TO_WIFI_DATA_THROTTLE_LOW_THRESHOLD
#define H_WIFI_TX_DATA_THROTTLE_HIGH_THRESHOLD                                 \
    CONFIG_ESP_HOSTED_TO_WIFI_DATA_THROTTLE_HIGH_THRESHOLD
#else
#define H_WIFI_TX_DATA_THROTTLE_LOW_THRESHOLD 0
#define H_WIFI_TX_DATA_THROTTLE_HIGH_THRESHOLD 0
#endif

#ifndef CONFIG_ESP_HOSTED_PKT_STATS
#define CONFIG_ESP_HOSTED_PKT_STATS 0
#endif
#ifndef CONFIG_ESP_HOSTED_RAW_THROUGHPUT_TRANSPORT
#define CONFIG_ESP_HOSTED_RAW_THROUGHPUT_TRANSPORT 0
#endif
#define H_PKT_STATS CONFIG_ESP_HOSTED_PKT_STATS
/* Raw Throughput Testing */
#define H_TEST_RAW_TP CONFIG_ESP_HOSTED_RAW_THROUGHPUT_TRANSPORT

#if H_TEST_RAW_TP

#define H_RAW_TP_REPORT_INTERVAL CONFIG_ESP_HOSTED_RAW_TP_REPORT_INTERVAL
#define H_RAW_TP_PKT_LEN CONFIG_ESP_HOSTED_RAW_TP_HOST_TO_ESP_PKT_LEN

#if CONFIG_ESP_HOSTED_RAW_THROUGHPUT_TX_TO_SLAVE
#define H_TEST_RAW_TP_DIR (ESP_TEST_RAW_TP__HOST_TO_ESP)
#elif CONFIG_ESP_HOSTED_RAW_THROUGHPUT_RX_FROM_SLAVE
#define H_TEST_RAW_TP_DIR (ESP_TEST_RAW_TP__ESP_TO_HOST)
#elif CONFIG_ESP_HOSTED_RAW_THROUGHPUT_BIDIRECTIONAL
#define H_TEST_RAW_TP_DIR (ESP_TEST_RAW_TP__BIDIRECTIONAL)
#else
#error Test Raw TP direction not defined
#endif

#else
#define H_TEST_RAW_TP_DIR (ESP_TEST_RAW_TP_NONE)
#endif

/* ----------------------- Enable packet stats -------------------------------
 */

#ifndef CONFIG_ESP_PKT_STATS
#define CONFIG_ESP_PKT_STATS 0
#define CONFIG_ESP_PKT_STATS_INTERVAL_SEC 0
#endif

#if CONFIG_ESP_PKT_STATS
#define ESP_PKT_STATS 1
#define ESP_PKT_STATS_REPORT_INTERVAL CONFIG_ESP_PKT_STATS_INTERVAL_SEC
#else
#define ESP_PKT_STATS 0
#define ESP_PKT_STATS_REPORT_INTERVAL 0
#endif

#ifndef CONFIG_ESP_MEM_STATS
#define CONFIG_ESP_MEM_STATS 0
#endif

#define H_MEM_STATS CONFIG_ESP_MEM_STATS

/* ----------------- Host to slave Wi-Fi flow control ------------------------
 */
/* Bit0: slave request host to enable flow control */
#define H_EVTGRP_BIT_FC_ALLOW_WIFI BIT(0)

/* --------------------- Host Power saving -----------------------------------
 */

#if defined(CONFIG_ESP_HOSTED_HOST_POWER_SAVE_ENABLED)
#if defined(CONFIG_ESP_HOSTED_HOST_DEEP_SLEEP_ALLOWED)
#define H_HOST_PS_ALLOWED 1
#define H_HOST_PS_ALLOWED_LIGHT_SLEEP 0
#else
#define H_HOST_PS_ALLOWED 0
#endif

/* Amend later for light sleep */
#else
#define H_HOST_PS_ALLOWED 0
#endif

#if defined(CONFIG_ESP_HOSTED_HOST_WAKEUP_GPIO)
#define H_HOST_WAKEUP_GPIO_PORT NULL
#define H_HOST_WAKEUP_GPIO CONFIG_ESP_HOSTED_HOST_WAKEUP_GPIO
#else
#define H_HOST_WAKEUP_GPIO -1
#endif

#if defined(CONFIG_ESP_HOSTED_HOST_WAKEUP_GPIO_LEVEL)
#define H_HOST_WAKEUP_GPIO_LEVEL CONFIG_ESP_HOSTED_HOST_WAKEUP_GPIO_LEVEL
#else
#define H_HOST_WAKEUP_GPIO_LEVEL 1 /* High */
#endif

/* Conflict checks for host power save configuration */
#if (H_HOST_PS_ALLOWED == 1)
#if (H_HOST_WAKEUP_GPIO == -1)
#error "Conflict: Host wake-up GPIO is mandatory when deep sleep is allowed"
#endif
#endif

/* --------------------- Host CLI --------------------------------------------
 */
#ifdef CONFIG_ESP_HOSTED_CLI_ENABLED
#define H_ESP_HOSTED_CLI_ENABLED 1
#endif

/*  ---------------------- ESP-IDF Specific Config end ---------------------- */

/* --------------------- Network Split -------------------------------------- */
#ifdef CONFIG_ESP_HOSTED_NETWORK_SPLIT_ENABLED
#define H_NETWORK_SPLIT_ENABLED 1
#else
#define H_NETWORK_SPLIT_ENABLED 0
#endif

#if H_NETWORK_SPLIT_ENABLED
#if !defined(CONFIG_LWIP_TCP_LOCAL_PORT_RANGE_START) ||                        \
    !defined(CONFIG_LWIP_TCP_LOCAL_PORT_RANGE_END) ||                          \
    !defined(CONFIG_LWIP_UDP_LOCAL_PORT_RANGE_START) ||                        \
    !defined(CONFIG_LWIP_UDP_LOCAL_PORT_RANGE_END)
#error                                                                         \
    "LWIP ports at host need to configured, ensure they are exclusive and different from slave"
#endif

#define H_HOST_TCP_LOCAL_PORT_RANGE_START CONFIG_LWIP_TCP_LOCAL_PORT_RANGE_START
#define H_HOST_TCP_LOCAL_PORT_RANGE_END CONFIG_LWIP_TCP_LOCAL_PORT_RANGE_END
#define H_HOST_UDP_LOCAL_PORT_RANGE_START CONFIG_LWIP_UDP_LOCAL_PORT_RANGE_START
#define H_HOST_UDP_LOCAL_PORT_RANGE_END CONFIG_LWIP_UDP_LOCAL_PORT_RANGE_END

#define H_SLAVE_TCP_REMOTE_PORT_RANGE_START                                    \
    CONFIG_LWIP_TCP_REMOTE_PORT_RANGE_START
#define H_SLAVE_TCP_REMOTE_PORT_RANGE_END CONFIG_LWIP_TCP_REMOTE_PORT_RANGE_END
#define H_SLAVE_UDP_REMOTE_PORT_RANGE_START                                    \
    CONFIG_LWIP_UDP_REMOTE_PORT_RANGE_START
#define H_SLAVE_UDP_REMOTE_PORT_RANGE_END CONFIG_LWIP_UDP_REMOTE_PORT_RANGE_END

#endif

#define H_HOST_USES_STATIC_NETIF 0 /* yet unsupported */

#define H_ERROR_CHECK(x) do {                                                                                           \
    int err_rc_ = (x);                                                                      \
    if (unlikely(err_rc_ != 0)) {                                                           \
        ESP_LOGE("esp-hosted", "Error %d (%s) at %s:%d", err_rc_, #x, __FILE__, __LINE__);  \
        abort();                                                                            \
    }                                                                                       \
} while(0)

esp_err_t esp_hosted_set_default_config(void);
bool esp_hosted_is_config_valid(void);

#endif
