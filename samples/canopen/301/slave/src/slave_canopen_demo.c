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
#include "hpm_canopen.h"
#include "user_config.h"
#include "OD.h"

#define MCHTMR_CLK_NAME (clock_mchtmr0)
#define TMR_TASK_INTERVAL   (1000)
#define BITRATE    500000
#define NMT_CONTROL   CO_NMT_ERR_ON_ERR_REG | CO_ERR_REG_GENERIC_ERR | CO_ERR_REG_COMMUNICATION
#define FIRST_HB_TIME        500
#define SDO_SRV_TIMEOUT_TIME 1000
#define SDO_CLI_TIMEOUT_TIME 500
#define SDO_CLI_BLOCK        false
#define OD_STATUS_BITS       NULL

uint8_t led_green;
CO_t *co[MAX_CANOPEN_DEVICE];
struct canopen_context canptr[MAX_CANOPEN_DEVICE];
hpm_can_config_t hpm_canopen_config[MAX_CANOPEN_DEVICE] = {0};
hpm_can_data_t hpm_canopen_data[MAX_CANOPEN_DEVICE] = {0};
struct device hpm_canopen_dev[MAX_CANOPEN_DEVICE] = {0};

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

/* if MAX_CANOPEN_DEVICE > 1, should add IRQ declare */
SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, hpm_canopen_isr);
void hpm_canopen_isr(void)
{
    for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
        canopen_irq_handler((struct device *)&hpm_canopen_dev[i]);
    }
}

int main(void)
{
    CO_ReturnError_t err;
    uint32_t heap_memory_used;
    CO_NMT_reset_cmd_t reset = CO_RESET_NOT;
    uint8_t pending_node_id = 10;    /* read from dip switches or nonvolatile memory, configurable by LSS slave */
    uint8_t pending_node_id_group[MAX_CANOPEN_DEVICE];
    uint8_t active_node_id_group[MAX_CANOPEN_DEVICE];
    uint16_t pending_bitrate = 125; /* read from dip switches or nonvolatile memory, configurable by LSS slave */

    uint64_t timestamp[MAX_CANOPEN_DEVICE];
    uint32_t elapsed[MAX_CANOPEN_DEVICE];
    uint32_t timeout;
    uint32_t mchtmr_freq;

    board_init();
    board_init_led_pins();

    mchtmr_freq = clock_get_frequency(MCHTMR_CLK_NAME);

    CO_config_t *config_ptr = NULL;

    for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
        /* initialize CAN controller */
        canopen_controller_init(&canptr[i], &s_can_info[i], (uint32_t)BITRATE, i);

        co[i] = CO_new(config_ptr, &heap_memory_used);
        if (co[i] == NULL) {
            printf("Error: Can't allocate memory\n");
            return 0;
        } else {
            printf("Allocated %u bytes for CANopen objects\n", heap_memory_used);
        }

        pending_node_id_group[i] = pending_node_id + i;
    }

    while (reset != CO_RESET_APP) {
        /* CANopen communication reset - initialize CANopen objects *******************/
        printf("CANopenNode - Reset communication...\n");

        for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
            elapsed[i] =  0U; /* milliseconds */
            co[i]->CANmodule->CANnormal = false;

            /* Enter CAN configuration. */
            CO_CANsetConfigurationMode((void *)&canptr[i]);
            CO_CANmodule_disable(co[i]->CANmodule);

            /* initialize CANopen */
            err = CO_CANinit(co[i], &canptr[i], pending_bitrate);
            if (err != CO_ERROR_NO) {
                printf("Error: CAN initialization failed: %d\n", err);
                return 0;
            }

            CO_LSS_address_t lss_address = {.identity = {.vendorID = OD_PERSIST_COMM.x1018_identity.vendor_ID,
                                                    .productCode = OD_PERSIST_COMM.x1018_identity.productCode,
                                                    .revisionNumber = OD_PERSIST_COMM.x1018_identity.revisionNumber,
                                                    .serialNumber = OD_PERSIST_COMM.x1018_identity.serialNumber}};
            err = CO_LSSinit(co[i], &lss_address, &pending_node_id_group[i], &pending_bitrate);
            if (err != CO_ERROR_NO) {
                printf("Error: LSS slave initialization failed: %d\n", err);
                return 0;
            }

            active_node_id_group[i] = pending_node_id_group[i];
            uint32_t err_info = 0;

            err = CO_CANopenInit(co[i],                   /* CANopen object */
                                 NULL,                 /* alternate NMT */
                                 NULL,                 /* alternate em */
                                 OD,                   /* Object dictionary */
                                 OD_STATUS_BITS,       /* Optional OD_statusBits */
                                 NMT_CONTROL,          /* CO_NMT_control_t */
                                 FIRST_HB_TIME,        /* firstHBTime_ms */
                                 SDO_SRV_TIMEOUT_TIME, /* SDOserverTimeoutTime_ms */
                                 SDO_CLI_TIMEOUT_TIME, /* SDOclientTimeoutTime_ms */
                                 SDO_CLI_BLOCK,        /* SDOclientBlockTransfer */
                                 active_node_id_group[i], &err_info);
            if (err != CO_ERROR_NO && err != CO_ERROR_NODE_ID_UNCONFIGURED_LSS) {
                if (err == CO_ERROR_OD_PARAMETERS) {
                    printf("Error: Object Dictionary entry 0x%X\n", err_info);
                } else {
                    printf("Error: CANopen initialization failed: %d\n", err);
                }
                return 0;
            }

            err = CO_CANopenInitPDO(co[i], co[i]->em, OD, active_node_id_group[i], &err_info);
            if (err != CO_ERROR_NO) {
                if (err == CO_ERROR_OD_PARAMETERS) {
                    printf("Error: Object Dictionary entry 0x%X\n", err_info);
                } else {
                    printf("Error: PDO initialization failed: %d\n", err);
                }
                return 0;
            }

            if (!co[i]->nodeIdUnconfigured) {
            } else {
                printf("CANopenNode - Node-id not initialized\n");
            }

            /* start CAN */
            CO_CANsetNormalMode(co[i]->CANmodule);

            reset = CO_RESET_NOT;

            printf("CANopenNode - Slave Mode Running...\n");
        }

        while (reset == CO_RESET_NOT) {
            timeout = 1U; /* default timeout in milliseconds */
            /* loop for normal program execution ******************************************/

            /* CANopen process */
            for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
                timestamp[i] = mchtmr_get_count(HPM_MCHTMR);
                reset = CO_process(co[i], false, (uint16_t)elapsed[i] * 1000, &timeout);
                led_green = CO_LED_GREEN(co[i]->LEDs, CO_LED_CANopen);
                board_led_write(led_green);

    #if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
                bool_t sync_was;

                /* Process Sync */
                sync_was = CO_process_SYNC(co[i], TMR_TASK_INTERVAL, NULL);

                /* Read inputs */
                CO_process_RPDO(co[i], sync_was, TMR_TASK_INTERVAL, NULL);
                /* Write outputs */
                CO_process_TPDO(co[i], sync_was, TMR_TASK_INTERVAL, NULL);
    #endif

                if (timeout > 0) {
                    board_delay_ms(timeout);
                    elapsed[i] = (mchtmr_get_count(HPM_MCHTMR) - timestamp[i])*1000 / mchtmr_freq;
                } else {
                    elapsed[i] = 0U;
                }

            }
        }
    }

    /* program exit ***************************************************************/

    /* delete objects from memory */
    for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
        CO_CANsetConfigurationMode((void *)&canptr[i]);
        CO_delete(co[i]);
    }

    printf("CANopenNode finished\n");

    /* will not execute this line unless hardware_reset */
    hpm_canopen_reboot();
}
