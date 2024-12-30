/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_canopen_test.h"

#define MCHTMR_CLK_NAME (clock_mchtmr0)
#define CONFIG_CANOPEN_NODE_ID 10
#define TMR_TASK_INTERVAL   (1000)
#define activeNodeId    0xA
#define BitRate    500000

struct device hpm_canopen_dev = {0};
static uint8_t LED;
struct canopen_context can;

void show_help(void);
void handle_canopen_test(void);
extern CO_SDO_abortCode_t odf_2102(CO_ODF_arg_t *odf_arg);

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, hpm_canopen_isr);
void hpm_canopen_isr(void)
{
    canopen_irq_handler(&hpm_canopen_dev);
}

int main(void)
{
    CO_ReturnError_t err;

    board_init();
    board_init_led_pins();

    err = CO_new(NULL);
    if (err != CO_ERROR_NO) {
        printf("CO_new failed\n");
    }

    canopen_init(&can, BOARD_APP_CAN_BASE, (uint32_t)BitRate);

    handle_canopen_test();

    /* will not execute this line unless hardware_reset */
    CO_delete(&can);
    hpm_sys_reboot();
}

void enter_slave_mode(void)
{
    uint64_t timestamp;
    static uint32_t elapsed;
    uint32_t timeout;
    uint32_t mchtmr_freq;
    CO_NMT_reset_cmd_t reset = CO_RESET_NOT;
    CO_ReturnError_t err;

    mchtmr_freq = clock_get_frequency(MCHTMR_CLK_NAME);

    while (reset != CO_RESET_APP) {
        elapsed =  0U; /* milliseconds */

        err = CO_CANinit(&can, (uint32_t)BitRate);
        if (err) {
            CO_delete(&can);
            printf("CO_init failed (err = %d)\n", err);
        }

        err = CO_CANopenInit(activeNodeId);
        if (err != CO_ERROR_NO) {
            printf("CO_init failed (err = %d)\n", err);
        }

        CO_OD_configure(CO->SDO[0], OD_2102_buttonPressCounter,
                odf_2102, NULL, 0U, 0U);

        CO_CANsetNormalMode(CO->CANmodule[0]);

        printf("enter slave mode ~ ~\n");

        while (true) {
            timeout = 1U; /* default timeout in milliseconds */
            timestamp = mchtmr_get_count(HPM_MCHTMR);
            reset = CO_process(CO, (uint16_t)elapsed * 1000, &timeout);
#if (CO_CONFIG_LEDS) & CO_CONFIG_LEDS_ENABLE
            LED = CO_LED_GREEN(CO->LEDs, CO_LED_CANopen);
            board_led_write(LED);
#endif

#if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
            bool_t syncWas;

            /* Process Sync */
            syncWas = CO_process_SYNC(CO, TMR_TASK_INTERVAL, NULL);

            /* Read inputs */
            CO_process_RPDO(CO, syncWas);
            /* Write outputs */
            CO_process_TPDO(CO, syncWas, TMR_TASK_INTERVAL, NULL);
#endif

            if (reset != CO_RESET_NOT) {
                break;
            }

            if (timeout > 0) {
                board_delay_ms(timeout);
                elapsed = (mchtmr_get_count(HPM_MCHTMR) - timestamp)*1000 / mchtmr_freq;
            } else {
                elapsed = 0U;
            }
        }

        if (reset == CO_RESET_COMM) {
            printf("Resetting communication\n");
        }
    }
    printf("Resetting device\n");
}

void enter_master_mode(void)
{
    CO_ReturnError_t err;
    uint32_t activeSlaveNodeId = 0;
    uint32_t activedevice = 0xFFFE;

    printf("enter master mode and find slave device ~~\n");

    err = CO_CANinit(&can, (uint32_t)BitRate);
    if (err) {
        CO_delete(&can);
        printf("CO_CANinit failed (err = %d)\n", err);
    }

    CO_CANsetNormalMode(CO->CANmodule[0]);

    do {
        activedevice = find_slave_device();
    } while (activedevice == 0xFFFE);

    activeSlaveNodeId = activedevice - 0x700;

    CO_CANsetConfigurationMode(CO->CANmodule[0]);

    CO_CANsetNormalMode(CO->CANmodule[0]);

    /* set slave device to optional state */
    set_slave_device_mode(CO->CANmodule[0]);
    printf("slave enter to optional mode\n");

    /* receive PDO message */
    PDO_test(CO->CANmodule[0], activeSlaveNodeId);

    /* read SDO message and print it */
    SDO_test(CO->CANmodule[0], activeSlaveNodeId);

    printf("canopen test done\n");

    while (1) {
    }

    CO_delete(&can);
}

void handle_canopen_test(void)
{
    while (true) {
        show_help();
        char option = getchar();
        putchar(option);
        putchar('\n');

        switch (option) {
        default:
            show_help();
            break;
        case '0':
            enter_master_mode();
            break;
        case '1':
            enter_slave_mode();
            break;
        }
    }
}

void show_help(void)
{
    static const char help_info[] = ""
                                    "*********************************************************************************\n"
                                    "*                                                                               *\n"
                                    "*                         CANOPEN Example Menu                                  *\n"
                                    "*                                                                               *\n"
                                    "* 0 - Master mode                                                               *\n"
                                    "* 1 - Slave mode                                                                *\n"
                                    "*                                                                               *\n"
                                    "*********************************************************************************\n";
    printf("%s\n", help_info);
}