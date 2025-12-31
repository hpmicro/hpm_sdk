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
#include "hpm_canopen.h"

#define MCHTMR_CLK_NAME (clock_mchtmr0)
#define CONFIG_CANOPEN_NODE_ID 10
#define TMR_TASK_INTERVAL   (1000)
#define BITRATE    500000

CO_t *co; /* CANopen object */
hpm_can_config_t hpm_canopen_config = {0};
hpm_can_data_t hpm_canopen_data = {0};
struct device hpm_canopen_dev = {0};
struct canopen_context can;

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
ATTR_PLACE_AT(".ahb_sram") uint32_t board_app_mcan_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
can_info_t s_can_info[] = {
    {
        .can_base = BOARD_APP_CAN_BASE,
        .irq_num = BOARD_APP_CAN_IRQn,
        .priority = 1,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &board_app_mcan_msg_buf,
        .ram_size = sizeof(board_app_mcan_msg_buf),
#endif
    },
    /* init more can base here */
};

volatile hpm_master_receive_buf_t canopen_rx_buf = {0};

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, hpm_canopen_isr);
void hpm_canopen_isr(void)
{
    canopen_irq_handler((struct device *)&hpm_canopen_dev);
}

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

CO_ReturnError_t set_slave_device_mode(CO_CANmodule_t *canmodule)
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
        (void)can_send(canmodule->CANptr, &frame, K_NO_WAIT, NULL, canmodule);
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

void pdo_test(uint32_t node_id)
{
    struct can_frame frame;
    uint8_t pdo_received_msg = 0;

    /* receive slave PDO message */
    while (pdo_received_msg != 0xAA) {
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
                pdo_received_msg = canopen_rx_buf.rx_buf.data_8[0];
#else
                pdo_received_msg = canopen_rx_buf.rx_buf.data[0];
#endif
            }
        }
    }

    printf("PDO test successfully!\n");
}

void sdo_test(CO_CANmodule_t *canmodule, uint32_t node_id)
{
    struct can_frame frame;
    uint8_t data[8] = {0};
    int err;
    uint8_t sdo[8] = {0};
    uint32_t slave_hbtime = 0;

    /* read SDO index 1017 and it means slave HBtime */
    frame.id = 0x600 + node_id;
    frame.dlc = 8;
    frame.flags = 0;
    data[0] = 0x40;
    data[1] = 0x17;
    data[2] = 0x10;
    memcpy(frame.data, &data, frame.dlc);

    err = can_send(canmodule->CANptr, &frame, K_NO_WAIT, NULL, canmodule);
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
                sdo[4] = canopen_rx_buf.rx_buf.data_8[4];
                sdo[5] = canopen_rx_buf.rx_buf.data_8[5];
#else
                sdo[4] = canopen_rx_buf.rx_buf.data[4];
                sdo[5] = canopen_rx_buf.rx_buf.data[5];
#endif
            }
        }
    }

    slave_hbtime = sdo[4] + (sdo[5] << 8);

    printf("slave HBtime: %d ms\n", slave_hbtime);

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

    err = can_send(canmodule->CANptr, &frame, K_NO_WAIT, NULL, canmodule);
    if (err != 0) {
        printf("send SDO read message failed!\n");
    }

    while ((frame.id != node_id + 0x580) && (sdo[0] != 0x60)) {
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
                sdo[0] = canopen_rx_buf.rx_buf.data_8[0];
#else
                sdo[0] = canopen_rx_buf.rx_buf.data[0];
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

    err = can_send(canmodule->CANptr, &frame, K_NO_WAIT, NULL, canmodule);
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
                sdo[4] = canopen_rx_buf.rx_buf.data_8[4];
                sdo[5] = canopen_rx_buf.rx_buf.data_8[5];
#else
                sdo[4] = canopen_rx_buf.rx_buf.data[4];
                sdo[5] = canopen_rx_buf.rx_buf.data[5];
#endif
            }
        }
    }

    slave_hbtime = sdo[4] + (sdo[5] << 8);

    printf("slave HBtime: %d ms\n", slave_hbtime);

    printf("SDO test successfully!\n");
}

void enter_master_mode(void)
{
    CO_ReturnError_t err;
    uint32_t active_slave_node_id = 0;
    uint32_t active_device = 0xFFFE;

    printf("enter master mode and find slave device ~~\n");

    /* Enter CAN configuration. */
    CO_CANsetConfigurationMode((void *)&can);
    CO_CANmodule_disable(co->CANmodule);

    err = CO_CANinit(co, &can, (uint16_t)BITRATE);
    if (err != CO_ERROR_NO) {
        printf("Error: CAN initialization failed: %d\n", err);
    }

    CO_CANsetNormalMode(co->CANmodule);

    do {
        active_device = find_slave_device();
    } while (active_device == 0xFFFE);

    active_slave_node_id = active_device - 0x700;

    CO_CANsetConfigurationMode(co->CANmodule);

    CO_CANsetNormalMode(co->CANmodule);

    /* set slave device to optional state */
    set_slave_device_mode(co->CANmodule);
    printf("slave enter to optional mode\n");

    /* receive PDO message */
    pdo_test(active_slave_node_id);

    /* read SDO message and print it */
    sdo_test(co->CANmodule, active_slave_node_id);

    printf("canopen test done\n");

    while (1) {
    }
}


int main(void)
{
    uint32_t heap_memory_used;
    CO_config_t *config_ptr = NULL;

    board_init();
    board_init_led_pins();

    co = CO_new(config_ptr, &heap_memory_used);
    if (co == NULL) {
        printf("Error: Can't allocate memory\n");
        return 0;
    } else {
        printf("Allocated %u bytes for CANopen objects\n", heap_memory_used);
    }

    canopen_controller_init(&can, &s_can_info[0], (uint32_t)BITRATE, 0);

    enter_master_mode();
}
