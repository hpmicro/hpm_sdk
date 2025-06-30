/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_ppor_drv.h"
#include "hpm_canopen.h"

#define MCHTMR_CLK_NAME (clock_mchtmr0)
#define CONFIG_CANOPEN_NODE_ID 10
#define TMR_TASK_INTERVAL   (1000)
#define activeNodeId    0xA
#define BitRate    500000

CO_t *CO; /* CANopen object */
struct device hpm_canopen_dev = {0};
struct canopen_context can;

volatile struct hpm_master_receive_buf canopen_rx_buf = {0};

uint32_t find_slave_device(void)
{
    struct can_frame frame;

    if (canopen_rx_buf.has_received_message) {
        canopen_rx_buf.has_received_message = false;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
        if (canopen_rx_buf.rx_buf.use_ext_id) {
            frame.id = canopen_rx_buf.rx_buf.ext_id;
        } else {
            frame.id = canopen_rx_buf.rx_buf.std_id;
        }
        int8_t command = canopen_rx_buf.rx_buf.data_8[0];
#else
        frame.id = canopen_rx_buf.rx_buf.id;
        int8_t command = canopen_rx_buf.rx_buf.data[0];
#endif
        switch (command) {
        case CO_NMT_INITIALIZING:
        case CO_NMT_PRE_OPERATIONAL:
            printf("find device, id:%x, status preoptional\n", frame.id);
            return frame.id;
        case CO_NMT_OPERATIONAL:
            printf("find device, id:%x, status optional\n", frame.id);
            return frame.id;
        case CO_NMT_STOPPED:
            printf("find device, id:%x, status stopped\n", frame.id);
            return frame.id;
        case CO_NMT_UNKNOWN:
        default:
            break;
        }
    }
    return 0xFFFE;
}

CO_ReturnError_t set_slave_device_mode(CO_CANmodule_t *CANmodule)
{
    CO_ReturnError_t ret = CO_ERROR_TX_UNCONFIGURED;
    struct can_frame frame;
    uint8_t data[8] = {0};
    uint8_t state = 0;
    int err = 0;

    frame.id = 0x0;
    frame.dlc = 2;
    frame.flags = 0;
    data[0] = 0x01;
    memcpy(frame.data, &data, frame.dlc);

    while (state != CO_NMT_OPERATIONAL) {
        (void)can_send(CANmodule->CANptr, &frame, K_NO_WAIT, NULL, CANmodule);
        if (err != 0) {
            ret = CO_ERROR_TX_UNCONFIGURED;
        }

        board_delay_ms(1);

        if (canopen_rx_buf.has_received_message) {
            canopen_rx_buf.has_received_message = false;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
            state = canopen_rx_buf.rx_buf.data_8[0];
#else
            state = canopen_rx_buf.rx_buf.data[0];
#endif
        }
        board_delay_ms(1000);
    }
    return ret;
}

void PDO_test(uint32_t node_id)
{
    struct can_frame frame;
    uint8_t PDO_received_msg = 0;

    /* receive slave PDO message */
    while (PDO_received_msg != 0xAA) {
        if (canopen_rx_buf.has_received_message) {
            canopen_rx_buf.has_received_message = false;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
            if (canopen_rx_buf.rx_buf.use_ext_id) {
                frame.id = canopen_rx_buf.rx_buf.ext_id;
            } else {
                frame.id = canopen_rx_buf.rx_buf.std_id;
            }
#else
            frame.id = canopen_rx_buf.rx_buf.id;
#endif
            if (frame.id == node_id + 0x180) {
#ifdef HPMSOC_HAS_HPMSDK_MCAN
                PDO_received_msg = canopen_rx_buf.rx_buf.data_8[0];
#else
                PDO_received_msg = canopen_rx_buf.rx_buf.data[0];
#endif
            }
        }
    }

    printf("PDO test successfully!\n");
}

void SDO_test(CO_CANmodule_t *CANmodule, uint32_t node_id)
{
    struct can_frame frame;
    uint8_t data[8] = {0};
    int err;
    uint8_t SDO[8] = {0};
    uint32_t slave_HBtime = 0;

    /* read SDO index 1017 and it means slave HBtime */
    frame.id = 0x600 + node_id;
    frame.dlc = 8;
    frame.flags = 0;
    data[0] = 0x40;
    data[1] = 0x17;
    data[2] = 0x10;
    memcpy(frame.data, &data, frame.dlc);

    err = can_send(CANmodule->CANptr, &frame, K_NO_WAIT, NULL, CANmodule);
    if (err != 0) {
        printf("send SDO read message failed!\n");
    }

    while (frame.id != node_id + 0x580) {
        if (canopen_rx_buf.has_received_message) {
            canopen_rx_buf.has_received_message = false;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
            if (canopen_rx_buf.rx_buf.use_ext_id) {
                frame.id = canopen_rx_buf.rx_buf.ext_id;
            } else {
                frame.id = canopen_rx_buf.rx_buf.std_id;
            }
#else
            frame.id = canopen_rx_buf.rx_buf.id;
#endif
            if (frame.id == node_id + 0x580) {
#ifdef HPMSOC_HAS_HPMSDK_MCAN
                SDO[4] = canopen_rx_buf.rx_buf.data_8[4];
                SDO[5] = canopen_rx_buf.rx_buf.data_8[5];
#else
                SDO[4] = canopen_rx_buf.rx_buf.data[4];
                SDO[5] = canopen_rx_buf.rx_buf.data[5];
#endif
            }
        }
    }

    slave_HBtime = SDO[4] + (SDO[5] << 8);

    printf("slave HBtime: %d ms\n", slave_HBtime);

    /* configure slave HBtime as 2s */
    frame.id = 0x600 + node_id;
    frame.dlc = 8;
    frame.flags = 0;
    data[0] = 0x2B;
    data[1] = 0x17;
    data[2] = 0x10;
    data[4] = 0xD0;
    data[5] = 0x07;
    memcpy(frame.data, &data, frame.dlc);

    err = can_send(CANmodule->CANptr, &frame, K_NO_WAIT, NULL, CANmodule);
    if (err != 0) {
        printf("send SDO read message failed!\n");
    }

    while ((frame.id != node_id + 0x580) && (SDO[0] != 0x60)) {
        if (canopen_rx_buf.has_received_message) {
            canopen_rx_buf.has_received_message = false;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
            if (canopen_rx_buf.rx_buf.use_ext_id) {
                frame.id = canopen_rx_buf.rx_buf.ext_id;
            } else {
                frame.id = canopen_rx_buf.rx_buf.std_id;
            }
#else
            frame.id = canopen_rx_buf.rx_buf.id;
#endif
            if (frame.id == node_id + 0x580) {
#ifdef HPMSOC_HAS_HPMSDK_MCAN
                SDO[0] = canopen_rx_buf.rx_buf.data_8[0];
#else
                SDO[0] = canopen_rx_buf.rx_buf.data[0];
#endif
            }
        }
    }

    /* read again to verify slave HBtime change successfully */
    frame.id = 0x600 + node_id;
    frame.dlc = 8;
    frame.flags = 0;
    data[0] = 0x40;
    data[1] = 0x17;
    data[2] = 0x10;
    memcpy(frame.data, &data, frame.dlc);

    err = can_send(CANmodule->CANptr, &frame, K_NO_WAIT, NULL, CANmodule);
    if (err != 0) {
        printf("send SDO read message failed!\n");
    }

    while (frame.id != node_id + 0x580) {
        if (canopen_rx_buf.has_received_message) {
            canopen_rx_buf.has_received_message = false;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
            if (canopen_rx_buf.rx_buf.use_ext_id) {
                frame.id = canopen_rx_buf.rx_buf.ext_id;
            } else {
                frame.id = canopen_rx_buf.rx_buf.std_id;
            }
#else
            frame.id = canopen_rx_buf.rx_buf.id;
#endif
            if (frame.id == node_id + 0x580) {
#ifdef HPMSOC_HAS_HPMSDK_MCAN
                SDO[4] = canopen_rx_buf.rx_buf.data_8[4];
                SDO[5] = canopen_rx_buf.rx_buf.data_8[5];
#else
                SDO[4] = canopen_rx_buf.rx_buf.data[4];
                SDO[5] = canopen_rx_buf.rx_buf.data[5];
#endif
            }
        }
    }

    slave_HBtime = SDO[4] + (SDO[5] << 8);

    printf("slave HBtime: %d ms\n", slave_HBtime);

    printf("SDO test successfully!\n");
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, hpm_canopen_isr);
void hpm_canopen_isr(void)
{
    canopen_irq_handler(&hpm_canopen_dev);
}

void enter_master_mode(void)
{
    CO_ReturnError_t err;
    uint32_t activeSlaveNodeId = 0;
    uint32_t activedevice = 0xFFFE;

    printf("enter master mode and find slave device ~~\n");

    /* Enter CAN configuration. */
    CO_CANsetConfigurationMode((void *)&can);
    CO_CANmodule_disable(CO->CANmodule);

    err = CO_CANinit(CO, &can, (uint16_t)BitRate);
    if (err != CO_ERROR_NO) {
        printf("Error: CAN initialization failed: %d\n", err);
    }

    CO_CANsetNormalMode(CO->CANmodule);

    do {
        activedevice = find_slave_device();
    } while (activedevice == 0xFFFE);

    activeSlaveNodeId = activedevice - 0x700;

    CO_CANsetConfigurationMode(CO->CANmodule);

    CO_CANsetNormalMode(CO->CANmodule);

    /* set slave device to optional state */
    set_slave_device_mode(CO->CANmodule);
    printf("slave enter to optional mode\n");

    /* receive PDO message */
    PDO_test(activeSlaveNodeId);

    /* read SDO message and print it */
    SDO_test(CO->CANmodule, activeSlaveNodeId);

    printf("canopen test done\n");

    while (1) {
    }
}


int main(void)
{
    uint32_t heapMemoryUsed;
    CO_config_t *config_ptr = NULL;

    board_init();
    board_init_led_pins();

    CO = CO_new(config_ptr, &heapMemoryUsed);
    if (CO == NULL) {
        printf("Error: Can't allocate memory\n");
        return 0;
    } else {
        printf("Allocated %u bytes for CANopen objects\n", heapMemoryUsed);
    }

    canopen_init(&can, BOARD_APP_CAN_BASE, (uint32_t)BitRate);

    enter_master_mode();
}
