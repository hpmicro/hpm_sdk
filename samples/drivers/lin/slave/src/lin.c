/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * LIN Slave Example
 * This example demonstrates LIN slave functionality:
 * 1. Responds to master's requests
 * 2. Handles data transmission (ID: 0x30)
 * 3. Handles data reception (ID: 0x31)
 * 4. Supports sleep mode and wakeup
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_lin_drv.h"

/* LIN controller configuration */
#define TEST_LIN                BOARD_LIN
#define TEST_LIN_CLOCK_NAME     BOARD_LIN_CLK_NAME
#define TEST_LIN_IRQ            BOARD_LIN_IRQ

#define LIN_DATA_MAX_LENGTH  (8U)

/* LIN frame identifiers */
#define TEST_LIN_RECEIVE_ID   (0x31U)  /* slave action: receive 8 bytes  */
#define TEST_LIN_TRANSMIT_ID  (0x30U)  /* slave action: transmit 8 bytes */

/* Data buffers for LIN communication */
uint8_t sent_buff[LIN_DATA_MAX_LENGTH];      /* Buffer for data to be sent */
uint8_t receive_buff[LIN_DATA_MAX_LENGTH];   /* Buffer for received data */

/* LIN transfer status flags */
volatile bool lin_complete;           /* Set when LIN transfer completes successfully */
volatile bool lin_wake_up;            /* Set when LIN wakeup signal is received */
volatile bool lin_error;              /* Set when LIN transfer error occurs */
volatile bool lin_data_req;           /* Set when master requests data transfer */
volatile bool lin_bus_idle_timeout;   /* Set when bus is idle for configured timeout */

/*
 * Helper function to print LIN data buffer contents
 * @param count: Number of bytes to print
 * @param buff: Pointer to data buffer
 */
static void printf_lin_data(uint8_t count, uint8_t *buff)
{
    assert(count <= LIN_DATA_MAX_LENGTH);
    for (uint8_t i = 0; i < count; i++) {
        printf("%X ", *(buff++));
    }
    printf("\n");
}

/*
 * Configure and start LIN slave response based on received ID
 * @param ptr: LIN controller instance
 * @param id: Received frame identifier
 */
static void lin_slave_respond_id(LIN_Type *ptr, uint8_t id)
{
    lin_trans_config_t config = {0};
    switch (id) {
    case TEST_LIN_RECEIVE_ID:
        config.transmit = false;               /* Configure as receive operation */
        config.data_buff = receive_buff;
        break;
    case TEST_LIN_TRANSMIT_ID:
        config.transmit = true;                /* Configure as transmit operation */
        config.data_buff = sent_buff;
        break;
    default:
        lin_slave_stop(TEST_LIN);             /* Stop on unknown ID */
        return;
    }

    config.id = id;
    config.data_length_from_id = true;        /* Get data length from ID field */
    config.enhanced_checksum = true;          /* Use enhanced checksum mode */
    lin_slave_transfer(ptr, &config);         /* Start non-blocking transfer */
}

/*
 * LIN Interrupt Service Routine
 * Handles:
 * - Data requests from master
 * - Transfer errors
 * - Bus idle timeout
 * - Wakeup events
 * - Transfer completion
 */
SDK_DECLARE_EXT_ISR_M(TEST_LIN_IRQ, lin_isr)
void lin_isr(void)
{
    volatile uint32_t status;
    volatile uint8_t id;
    status = lin_get_status(TEST_LIN);

    /* Handle data request from master */
    if (status & LIN_STATE_DATA_REQ_MASK) {
        id = lin_get_id(TEST_LIN);
        /* Process ID and configure response */
        lin_slave_respond_id(TEST_LIN, id);
        lin_data_req = true;
        status = lin_get_status(TEST_LIN);
    } else if (status & LIN_STATE_ERROR_MASK) {
        lin_error = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LIN_STATE_BUS_IDLE_TV_MASK) {
        lin_bus_idle_timeout = true;
        lin_sleep(TEST_LIN);                  /* Enter sleep mode on bus timeout */
    } else if (status & LIN_STATE_WAKEUP_MASK) {
        lin_wake_up = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LIN_STATE_COMPLETE_MASK) {
        lin_complete = true;
    }

    lin_reset_interrupt(TEST_LIN);
}

int main(void)
{
    hpm_stat_t stat;
    uint32_t freq;
    uint8_t data_length;

    /* Initialize board hardware */
    board_init();
    board_init_lin_pins(TEST_LIN);
    board_init_lin_clock(TEST_LIN);  /* Configure LIN clock (20MHz) */
    intc_m_enable_irq_with_priority(TEST_LIN_IRQ, 1);

    printf("LIN slave example\n");

    /* Initialize transmit data buffer with descending values */
    for (uint8_t i = 0; i < LIN_DATA_MAX_LENGTH; i++) {
        sent_buff[i] = LIN_DATA_MAX_LENGTH - 1 - i;
    }

    /* Configure LIN slave timing */
    freq = clock_get_frequency(TEST_LIN_CLOCK_NAME);
    stat = lin_slave_configure_timing(TEST_LIN, freq);
    if (stat != status_success) {
        printf("Config LIN slave timing failed\n");
    }

    /* Main event loop */
    while (1) {
        if (lin_complete) {
            lin_complete = false;
            lin_data_req = false;
            uint8_t id = lin_get_id(TEST_LIN);
            switch (id) {
            case TEST_LIN_RECEIVE_ID:
                data_length = lin_get_data_length(TEST_LIN);
                printf("ID: %X, receive %d bytes\n", id, data_length);
                /* Copy received data from LIN registers to buffer */
                for (uint8_t i = 0; i < data_length; i++) {
                    receive_buff[i] = lin_get_data_byte(TEST_LIN, i);
                }
                printf_lin_data(data_length, receive_buff);
                break;
            case TEST_LIN_TRANSMIT_ID:
                data_length = lin_get_data_length(TEST_LIN);
                printf("ID: %X, sent %d bytes\n", id, data_length);
                printf_lin_data(data_length, sent_buff);
                break;
            default:
                printf("UNKNOWN ID: %X, no response\n", id);
            }
        } else if (lin_wake_up) {
            lin_wake_up = false;
            printf("LIN wakeup\n");
        } else if (lin_error) {
            lin_error = false;
            printf("LIN transfer error\n");
        } else if (lin_bus_idle_timeout) {
            lin_bus_idle_timeout = false;
            printf("LIN bus timeout, will enter sleep mode\n");
        }
    }

    return 0;
}
