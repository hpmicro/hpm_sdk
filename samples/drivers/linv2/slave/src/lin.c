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

#define LIN_DATA_MAX_LENGTH  (8U)

#define TEST_LIN_RECEIVE_ID   (0x31U)  /* slave action: receive 8 bytes  */
#define TEST_LIN_TRANSMIT_ID  (0x30U)  /* slave action: transmit 8 bytes */

uint8_t sent_buff[LIN_DATA_MAX_LENGTH];
uint8_t receive_buff[LIN_DATA_MAX_LENGTH];

volatile bool lin_complete;
volatile bool lin_wake_up;
volatile bool lin_error;
volatile bool lin_data_req;
volatile bool lin_bus_idle_timeout;

static void printf_lin_data(uint8_t count, uint8_t *buff)
{
    assert(count <= LIN_DATA_MAX_LENGTH);
    for (uint8_t i = 0; i < count; i++) {
        printf("%X ", *(buff++));
    }
    printf("\n");
}

static void lin_slave_respond_id(LINV2_Type *ptr, uint8_t id)
{
    lin_trans_config_t config = {0};
    switch (id) {
    case TEST_LIN_RECEIVE_ID:
        config.transmit = false;
        config.data_buff = receive_buff;
        break;
    case TEST_LIN_TRANSMIT_ID:
        config.transmit = true;
        config.data_buff = sent_buff;
        break;
    default:
        lin_slave_stop(TEST_LIN);
        return;
    }

    config.id = id;
    config.data_length = 8U;
    config.data_length_from_id = false;
    config.enhanced_checksum = true;
    lin_slave_transfer(ptr, &config);
}

SDK_DECLARE_EXT_ISR_M(TEST_LIN_IRQ, lin_isr)
void lin_isr(void)
{
    volatile uint32_t status;
    volatile uint8_t id;
    status = lin_get_control_and_status(TEST_LIN);

    /* data request */
    if (status & LINV2_CONTROL_STATUS_DATA_REQ_MASK) {
        id = lin_get_id(TEST_LIN);
        /* process id */
        lin_slave_respond_id(TEST_LIN, id);
        lin_data_req = true;
        status = lin_get_control_and_status(TEST_LIN);
    } else if (status & LINV2_CONTROL_STATUS_ERROR_MASK) {
        lin_error = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LINV2_CONTROL_STATUS_BUS_IDLE_TIMEOUT_MASK) {
        lin_bus_idle_timeout = true;
        lin_sleep(TEST_LIN);
    } else if (status & LINV2_CONTROL_STATUS_WAKEUP_MASK) {
        lin_wake_up = true;
        lin_reset_error(TEST_LIN);
    } else if (status & LINV2_CONTROL_STATUS_COMPLETE_MASK) {
        lin_complete = true;
    }

    lin_reset_interrupt(TEST_LIN);
}

int main(void)
{
    hpm_stat_t stat;
    uint32_t freq;
    uint8_t data_length;

    board_init();
    board_init_lin_pins(TEST_LIN);
    board_init_lin_clock(TEST_LIN);  /* 20MHz */
    intc_m_enable_irq_with_priority(TEST_LIN_IRQ, 1);

    printf("LIN slave example\n");
    /** prepare data to be sent */
    for (uint8_t i = 0; i < LIN_DATA_MAX_LENGTH; i++) {
        sent_buff[i] = LIN_DATA_MAX_LENGTH - 1 - i;
    }

    freq = clock_get_frequency(TEST_LIN_CLOCK_NAME);
    stat = lin_slave_configure_timing(TEST_LIN, freq);
    if (stat != status_success) {
        printf("Config LIN slave timing failed\n");
    }

    /* set enhance checksum mode */
    /* Note: configure correct checksum mode before the transmission starts */
    lin_set_checksum_mode(TEST_LIN, true);

    while (1) {
        if (lin_complete) {
            lin_complete = false;
            lin_data_req = false;
            uint8_t id = lin_get_id(TEST_LIN);
            switch (id) {
            case TEST_LIN_RECEIVE_ID:
                data_length = lin_get_data_length(TEST_LIN);
                printf("ID: %X, receive %d bytes\n", id, data_length);
                /* load data from register into data buff */
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
