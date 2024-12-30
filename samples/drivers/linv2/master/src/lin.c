/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_linv2_drv.h"

#define TEST_LIN                BOARD_LIN
#define TEST_LIN_CLOCK_NAME     BOARD_LIN_CLK_NAME
#define TEST_LIN_IRQ            BOARD_LIN_IRQ
#define TEST_LIN_BAUDRATE       BOARD_LIN_BAUDRATE

#define TEST_LIN_RECEIVE_ID   (0x30U)  /* master action: receive 8 bytes  */
#define TEST_LIN_TRANSMIT_ID  (0x31U)  /* master action: transmit 8 bytes */

#define LIN_DATA_MAX_LENGTH  (8U)
uint8_t sent_buff[LIN_DATA_MAX_LENGTH];
uint8_t receive_buff[LIN_DATA_MAX_LENGTH];

volatile bool lin_error;
volatile bool lin_complete;
volatile bool lin_wake_up;

SDK_DECLARE_EXT_ISR_M(TEST_LIN_IRQ, lin_isr)
void lin_isr(void)
{
    volatile uint32_t status;
    status = lin_get_control_and_status(TEST_LIN);

    if (status & LINV2_CONTROL_STATUS_ERROR_MASK) {
        lin_error = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LINV2_CONTROL_STATUS_WAKEUP_MASK) {
        lin_wake_up = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LINV2_CONTROL_STATUS_COMPLETE_MASK) {
        lin_complete = true;
        /* if receive,  load data from register */
    }

    lin_reset_interrupt(TEST_LIN);
}

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

    board_init();
    board_init_lin_pins(TEST_LIN);
    board_init_lin_clock(TEST_LIN);  /**< 20MHz */
    intc_m_enable_irq_with_priority(TEST_LIN_IRQ, 1); /**< enable interrupt */

    printf("LIN master example\n");

    /** prepare data to be sent */
    for (uint8_t i = 0; i < LIN_DATA_MAX_LENGTH; i++) {
        sent_buff[i] = i;
    }

    timing.src_freq_in_hz = clock_get_frequency(TEST_LIN_CLOCK_NAME);
    /* timing.src_freq_in_hz = 25000000U; */
    timing.baudrate = TEST_LIN_BAUDRATE;
    stat = lin_master_configure_timing(TEST_LIN, &timing);
    if (stat != status_success) {
        printf("Config LIN master timing failed\n");
    }

    lin_wakeup(TEST_LIN);
    while (!lin_wake_up) {
       __asm("nop");
    }
    printf("LIN master sent wakeup signal\n");

    /** sent frame */
    config.id = TEST_LIN_TRANSMIT_ID;
    config.transmit = true;
    config.data_length_from_id = false;
    config.data_length = 8;
    config.enhanced_checksum = true;
    config.data_buff = sent_buff;
    lin_master_transfer(TEST_LIN, &config);

    while ((!lin_complete) && (!lin_error)) {
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

    /* wait for slave controller to be ready to send data */
    board_delay_ms(10);

    /** receive frame */
    config.id = TEST_LIN_RECEIVE_ID;
    config.transmit = false;
    config.data_length_from_id = false;
    config.data_length = 8;
    config.enhanced_checksum = true;
    config.data_buff = receive_buff;
    lin_master_transfer(TEST_LIN, &config);

    while ((!lin_complete) && (!lin_error)) {
    }

    if (lin_error) {
        lin_error = false;
        printf("LIN master receive error\n");
    } else {
        lin_complete =  false;
        printf("ID: %X, receive 8 bytes\n", TEST_LIN_RECEIVE_ID);
        data_length = lin_get_data_length(TEST_LIN);
        /* load data from register into data buff */
        for (uint8_t i = 0; i < data_length; i++) {
            receive_buff[i] = lin_get_data_byte(TEST_LIN, i);
        }
        printf_lin_data(data_length, receive_buff);
    }

    while (1) {
    }

    return 0;
}
