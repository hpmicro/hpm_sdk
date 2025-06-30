/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * LIN Master Example
 * This example demonstrates LIN master functionality:
 * 1. Sends wakeup signal
 * 2. Transmits 8 bytes of data to slave (ID: 0x31)
 * 3. Receives 8 bytes of data from slave (ID: 0x30)
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_lin_drv.h"

/* LIN controller configuration */
#define TEST_LIN                BOARD_LIN
#define TEST_LIN_CLOCK_NAME     BOARD_LIN_CLK_NAME
#define TEST_LIN_IRQ            BOARD_LIN_IRQ
#define TEST_LIN_BAUDRATE       BOARD_LIN_BAUDRATE

/* LIN frame identifiers */
#define TEST_LIN_RECEIVE_ID   (0x30U)  /* master action: receive 8 bytes  */
#define TEST_LIN_TRANSMIT_ID  (0x31U)  /* master action: transmit 8 bytes */

/* Data buffer configuration */
#define LIN_DATA_MAX_LENGTH  (8U)
uint8_t sent_buff[LIN_DATA_MAX_LENGTH];      /* Buffer for data to be sent */
uint8_t receive_buff[LIN_DATA_MAX_LENGTH];   /* Buffer for received data */

/* LIN transfer status flags */
volatile bool lin_error;      /* Set when LIN transfer error occurs */
volatile bool lin_complete;   /* Set when LIN transfer completes successfully */
volatile bool lin_wake_up;    /* Set when LIN wakeup signal is sent/received */

/*
 * LIN Interrupt Service Routine
 * Handles:
 * - Transfer errors
 * - Wakeup events
 * - Transfer completion
 */
SDK_DECLARE_EXT_ISR_M(TEST_LIN_IRQ, lin_isr)
void lin_isr(void)
{
    volatile uint32_t status;
    status = lin_get_status(TEST_LIN);

    if (status & LIN_STATE_ERROR_MASK) {
        lin_error = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LIN_STATE_WAKEUP_MASK) {
        lin_wake_up = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LIN_STATE_COMPLETE_MASK) {
        lin_complete = true;
        /* Note: For receive operations, data should be read from registers in main loop */
    }

    lin_reset_interrupt(TEST_LIN);
}

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

int main(void)
{
    hpm_stat_t stat;
    lin_timing_t timing;
    lin_trans_config_t config;
    uint8_t data_length;

    /* Initialize board hardware */
    board_init();
    board_init_lin_pins(TEST_LIN);
    board_init_lin_clock(TEST_LIN);  /* Configure LIN clock (20MHz) */
    intc_m_enable_irq_with_priority(TEST_LIN_IRQ, 1); /* Enable LIN interrupt */

    printf("LIN master example\n");

    /* Initialize transmit data buffer with incrementing values */
    for (uint8_t i = 0; i < LIN_DATA_MAX_LENGTH; i++) {
        sent_buff[i] = i;
    }

    /* Configure LIN timing parameters */
    timing.src_freq_in_hz = clock_get_frequency(TEST_LIN_CLOCK_NAME);
    timing.baudrate = TEST_LIN_BAUDRATE;
    stat = lin_master_configure_timing(TEST_LIN, &timing);
    if (stat != status_success) {
        printf("Config LIN master timing failed\n");
    }

    /* Send wakeup signal and wait for completion */
    lin_wakeup(TEST_LIN);
    while (!lin_wake_up) {
        __asm("nop");
    }
    printf("LIN master sent wakeup signal\n");

    /* Configure and send LIN frame */
    config.id = TEST_LIN_TRANSMIT_ID;
    config.transmit = true;                    /* Configure as transmit operation */
    config.data_length_from_id = false;        /* Use explicit data length */
    config.data_length = 8;                    /* Send 8 bytes */
    config.enhanced_checksum = true;           /* Use enhanced checksum mode */
    config.data_buff = sent_buff;
    lin_master_transfer(TEST_LIN, &config);    /* Start non-blocking transfer */

    /* Wait for transfer completion or error */
    while ((!lin_complete) && (!lin_error)) {
        __asm("nop");
    }
    if (lin_error) {
        lin_error = false;
        printf("LIN master sent error\n");
    } else {
        lin_complete =  false;
        data_length = lin_get_data_length(TEST_LIN);
        printf("ID: %X, sent 8 bytes:\n", TEST_LIN_TRANSMIT_ID);
        printf_lin_data(data_length, sent_buff);
    }

    /* Add delay to ensure slave is ready for next transfer */
    board_delay_ms(10);

    /* Configure and receive LIN frame */
    config.id = TEST_LIN_RECEIVE_ID;
    config.transmit = false;                   /* Configure as receive operation */
    config.data_length_from_id = false;        /* Use explicit data length */
    config.data_length = 8;                    /* Receive 8 bytes */
    config.enhanced_checksum = true;           /* Use enhanced checksum mode */
    config.data_buff = receive_buff;
    lin_master_transfer(TEST_LIN, &config);    /* Start non-blocking transfer */

    /* Wait for transfer completion or error */
    while ((!lin_complete) && (!lin_error)) {
        __asm("nop");
    }

    if (lin_error) {
        lin_error = false;
        printf("LIN master receive error\n");
    } else {
        lin_complete =  false;
        printf("ID: %X, receive 8 bytes\n", TEST_LIN_RECEIVE_ID);
        data_length = lin_get_data_length(TEST_LIN);
        /* Copy received data from LIN registers to buffer */
        for (uint8_t i = 0; i < data_length; i++) {
            receive_buff[i] = lin_get_data_byte(TEST_LIN, i);
        }
        printf_lin_data(data_length, receive_buff);
    }

    /* Main loop */
    while (1) {
    }

    return 0;
}
