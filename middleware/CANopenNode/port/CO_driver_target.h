/*
 * Copyright (c) 2019 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CANOPENNODE_CO_DRIVER_H
#define CANOPENNODE_CO_DRIVER_H

/*
 * Zephyr RTOS CAN driver interface and configuration for CANopenNode
 * CANopen protocol stack.
 *
 * See CANopenNode/stack/drvTemplate/CO_driver.h for API description.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h" /* float32_t, float64_t */
#include "stdbool.h"

#   ifndef __INT_MAX__
#    define __INT_MAX__ 2147483647
#   endif
#   undef INT_MIN
#   define INT_MIN (-INT_MAX-1)
#   undef INT_MAX
#   define INT_MAX __INT_MAX__

#define INT_MAX		__INT_MAX__

/* Use static variables instead of calloc() */
#define CO_USE_GLOBALS

/* Use Zephyr provided crc16 implementation */
#define CO_USE_OWN_CRC16

/* Use SDO buffer size from Kconfig */
#define CO_SDO_BUFFER_SIZE 32

/* Use trace buffer size from Kconfig */
#define CO_TRACE_BUFFER_SIZE_FIXED CONFIG_CANOPENNODE_TRACE_BUFFER_SIZE

#define CO_CONFIG_SRDO 0x1
#define CO_CONFIG_LEDS 0x1
#define CO_CONFIG_LSS 0x0
#define CO_CONFIG_GTW 0x0
#define CO_CONFIG_TRACE 0x0
#define CO_OD_STORAGE 0x0
#define CO_CONFIG_SDO_CLI 0x0
#define CO_CONFIG_TIME 0x0
#define CO_CONFIG_CRC16 0x1

#ifdef CONFIG_CANOPENNODE_LEDS
#define CO_USE_LEDS 1
#endif

#define CONFIG_LITTLE_ENDIAN 0x1
#define CO_SWAP_16(x) x
#define CO_SWAP_32(x) x
#define CO_SWAP_64(x) x

#ifdef CONFIG_LITTLE_ENDIAN
#define CO_LITTLE_ENDIAN
#else
#define CO_BIG_ENDIAN
#endif

typedef bool          bool_t;
typedef char          char_t;
typedef unsigned char oChar_t;
typedef unsigned char domain_t;

typedef struct canopen_rx_msg {
	uint8_t data[8];
	uint16_t ident;
	uint8_t DLC;
} CO_CANrxMsg_t;

typedef void (*CO_CANrxBufferCallback_t)(void *object,
					 const CO_CANrxMsg_t *message);

typedef struct {
	int filter_id;
    uint16_t ident;
    uint16_t mask;
    void *object;
    void (*CANrx_callback)(void *object, void *message);
} CO_CANrx_t;

typedef struct {
    uint32_t ident;
    uint8_t DLC;
    uint8_t data[8];
	volatile bool_t rtr;
    volatile bool_t bufferFull;
    volatile bool_t syncFlag;
} CO_CANtx_t;

typedef struct {
    void *CANptr;
    CO_CANrx_t *rxArray;
    uint16_t rxSize;
    CO_CANtx_t *txArray;
    uint16_t txSize;
    uint16_t CANerrorStatus;
    volatile bool_t configured;
    volatile bool_t CANnormal;
    volatile bool_t useCANrxFilters;
    volatile bool_t bufferInhibitFlag;
    volatile bool_t firstCANtxMessage;
    volatile uint16_t CANtxCount;
    uint32_t errOld;
    int32_t errinfo;
} CO_CANmodule_t;

void canopen_send_lock(void);
void canopen_send_unlock(void);
#define CO_LOCK_CAN_SEND()   canopen_send_lock()
#define CO_UNLOCK_CAN_SEND() canopen_send_unlock()

void canopen_emcy_lock(void);
void canopen_emcy_unlock(void);
#define CO_LOCK_EMCY()   canopen_emcy_lock()
#define CO_UNLOCK_EMCY() canopen_emcy_unlock()

void canopen_od_lock(void);
void canopen_od_unlock(void);
#define CO_LOCK_OD()   canopen_od_lock()
#define CO_UNLOCK_OD() canopen_od_unlock()

/*
 * CANopenNode RX callbacks run in interrupt context, no memory
 * barrier needed.
 */
#define CO_MemoryBarrier()
#define CO_FLAG_READ(rxNew) ((rxNew) != NULL)
#define CO_FLAG_SET(rxNew) {CO_MemoryBarrier(); rxNew = (void *)1L; }
#define CO_FLAG_CLEAR(rxNew) {CO_MemoryBarrier(); rxNew = NULL; }

#ifdef __cplusplus
}
#endif

#endif /* CANOPENNODE_CO_DRIVER_H */
