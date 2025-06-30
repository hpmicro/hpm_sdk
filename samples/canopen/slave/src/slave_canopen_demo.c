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
#include "OD.h"
#include "CO_storage.h"

#define MCHTMR_CLK_NAME (clock_mchtmr0)
#define TMR_TASK_INTERVAL   (1000)

struct device hpm_canopen_dev = {0};
struct canopen_context CANptr;           /* CAN module address */
uint8_t LED_green;
CO_t *CO; /* CANopen object */
#define BitRate    500000

#define NMT_CONTROL   CO_NMT_ERR_ON_ERR_REG | CO_ERR_REG_GENERIC_ERR | CO_ERR_REG_COMMUNICATION
#define FIRST_HB_TIME        500
#define SDO_SRV_TIMEOUT_TIME 1000
#define SDO_CLI_TIMEOUT_TIME 500
#define SDO_CLI_BLOCK        false
#define OD_STATUS_BITS       NULL

void hpm_sys_reboot(void)
{
    /*Enable software reset*/
    ppor_reset_mask_set_source_enable(HPM_PPOR, (uint32_t)ppor_reset_software);
    ppor_sw_reset(HPM_PPOR, 1000);
    while (1) {
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, hpm_canopen_isr);
void hpm_canopen_isr(void)
{
    canopen_irq_handler(&hpm_canopen_dev);
}

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
static ODR_t storeBlank(CO_storage_entry_t *entry, CO_CANmodule_t *CANmodule)
{
    (void)entry;
    (void)CANmodule;
    return ODR_OK;
}

/*
 * Function for restoring data on "Restore default parameters" command - OD 1011
 *
 * For more information see file CO_storage.h, CO_storage_entry_t.
 */
static ODR_t restoreBlank(CO_storage_entry_t *entry, CO_CANmodule_t *CANmodule)
{
    (void)entry;
    (void)CANmodule;
    return ODR_OK;
}

CO_ReturnError_t CO_storageBlank_init(CO_storage_t *storage, CO_CANmodule_t *CANmodule, OD_entry_t *OD_1010_StoreParameters,
                     OD_entry_t *OD_1011_RestoreDefaultParam, CO_storage_entry_t *entries, uint8_t entriesCount,
                     uint32_t *storageInitError)
{
    CO_ReturnError_t ret;

    /* verify arguments */
    if (storage == NULL || entries == NULL || entriesCount == 0 || storageInitError == NULL) {
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* initialize storage and OD extensions */
    ret = CO_storage_init(storage, CANmodule, OD_1010_StoreParameters, OD_1011_RestoreDefaultParam, storeBlank,
                          restoreBlank, entries, entriesCount);
    if (ret != CO_ERROR_NO) {
        return ret;
    }

    /* initialize entries */
    *storageInitError = 0;
    for (uint8_t i = 0; i < entriesCount; i++) {
        CO_storage_entry_t *entry = &entries[i];

        /* verify arguments */
        if (entry->addr == NULL || entry->len == 0 || entry->subIndexOD < 2) {
            *storageInitError = i;
            return CO_ERROR_ILLEGAL_ARGUMENT;
        }

        /* Open a file and read data from file to entry->addr */
        /* file = open(entry->pathToFileOrPointerToMemory); */
        /* read(entry->addr, entry->len, file); */
    }

    return ret;
}
#endif /* (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE */

int main(void)
{
    CO_ReturnError_t err;
    uint32_t heapMemoryUsed;
    CO_NMT_reset_cmd_t reset = CO_RESET_NOT;
    uint8_t pendingNodeId = 10;    /* read from dip switches or nonvolatile memory, configurable by LSS slave */
    uint8_t activeNodeId = 10;     /* Copied from CO_pendingNodeId in the communication reset section */
    uint16_t pendingBitRate = 125; /* read from dip switches or nonvolatile memory, configurable by LSS slave */

    uint64_t timestamp;
    static uint32_t elapsed;
    uint32_t timeout;
    uint32_t mchtmr_freq;

    board_init();
    board_init_led_pins();

    mchtmr_freq = clock_get_frequency(MCHTMR_CLK_NAME);

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
    CO_storage_t storage;
    CO_storage_entry_t storageEntries[] = {{.addr = &OD_PERSIST_COMM,
                                            .len = sizeof(OD_PERSIST_COMM),
                                            .subIndexOD = 2,
                                            .attr = CO_storage_cmd | CO_storage_restore,
                                            .addrNV = NULL}};
    uint8_t storageEntriesCount = sizeof(storageEntries) / sizeof(storageEntries[0]);
    uint32_t storageInitError = 0;
#endif

    /* Configure microcontroller. */

    /* Allocate memory */
    CO_config_t *config_ptr = NULL;
#ifdef CO_MULTIPLE_OD
    /* example usage of CO_MULTIPLE_OD (but still single OD here) */
    CO_config_t co_config = {0};
    OD_INIT_CONFIG(co_config); /* helper macro from OD.h */
    co_config.CNT_LEDS = 1;
    co_config.CNT_LSS_SLV = 1;
    config_ptr = &co_config;
#endif /* CO_MULTIPLE_OD */
    CO = CO_new(config_ptr, &heapMemoryUsed);
    if (CO == NULL) {
        printf("Error: Can't allocate memory\n");
        return 0;
    } else {
        printf("Allocated %u bytes for CANopen objects\n", heapMemoryUsed);
    }

    canopen_init(&CANptr, BOARD_APP_CAN_BASE, (uint32_t)BitRate);

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
    err = CO_storageBlank_init(&storage, CO->CANmodule, OD_ENTRY_H1010_storeParameters,
                               OD_ENTRY_H1011_restoreDefaultParameters, storageEntries, storageEntriesCount,
                               &storageInitError);

    if (err != CO_ERROR_NO && err != CO_ERROR_DATA_CORRUPT) {
        printf("Error: Storage %d\n", storageInitError);
        return 0;
    }
#endif

    while (reset != CO_RESET_APP) {
        elapsed =  0U; /* milliseconds */
        /* CANopen communication reset - initialize CANopen objects *******************/
        printf("CANopenNode - Reset communication...\n");

        CO->CANmodule->CANnormal = false;

        /* Enter CAN configuration. */
        CO_CANsetConfigurationMode((void *)&CANptr);
        CO_CANmodule_disable(CO->CANmodule);

        /* initialize CANopen */
        err = CO_CANinit(CO, &CANptr, pendingBitRate);
        if (err != CO_ERROR_NO) {
            printf("Error: CAN initialization failed: %d\n", err);
            return 0;
        }

        CO_LSS_address_t lssAddress = {.identity = {.vendorID = OD_PERSIST_COMM.x1018_identity.vendor_ID,
                                                    .productCode = OD_PERSIST_COMM.x1018_identity.productCode,
                                                    .revisionNumber = OD_PERSIST_COMM.x1018_identity.revisionNumber,
                                                    .serialNumber = OD_PERSIST_COMM.x1018_identity.serialNumber}};
        err = CO_LSSinit(CO, &lssAddress, &pendingNodeId, &pendingBitRate);
        if (err != CO_ERROR_NO) {
            printf("Error: LSS slave initialization failed: %d\n", err);
            return 0;
        }

        activeNodeId = pendingNodeId;
        uint32_t errInfo = 0;

        err = CO_CANopenInit(CO,                   /* CANopen object */
                             NULL,                 /* alternate NMT */
                             NULL,                 /* alternate em */
                             OD,                   /* Object dictionary */
                             OD_STATUS_BITS,       /* Optional OD_statusBits */
                             NMT_CONTROL,          /* CO_NMT_control_t */
                             FIRST_HB_TIME,        /* firstHBTime_ms */
                             SDO_SRV_TIMEOUT_TIME, /* SDOserverTimeoutTime_ms */
                             SDO_CLI_TIMEOUT_TIME, /* SDOclientTimeoutTime_ms */
                             SDO_CLI_BLOCK,        /* SDOclientBlockTransfer */
                             activeNodeId, &errInfo);
        if (err != CO_ERROR_NO && err != CO_ERROR_NODE_ID_UNCONFIGURED_LSS) {
            if (err == CO_ERROR_OD_PARAMETERS) {
                printf("Error: Object Dictionary entry 0x%X\n", errInfo);
            } else {
                printf("Error: CANopen initialization failed: %d\n", err);
            }
            return 0;
        }

        err = CO_CANopenInitPDO(CO, CO->em, OD, activeNodeId, &errInfo);
        if (err != CO_ERROR_NO) {
            if (err == CO_ERROR_OD_PARAMETERS) {
                printf("Error: Object Dictionary entry 0x%X\n", errInfo);
            } else {
                printf("Error: PDO initialization failed: %d\n", err);
            }
            return 0;
        }

        if (!CO->nodeIdUnconfigured) {

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
            if (storageInitError != 0) {
                CO_errorReport(CO->em, CO_EM_NON_VOLATILE_MEMORY, CO_EMC_HARDWARE, storageInitError);
            }
#endif
        } else {
            printf("CANopenNode - Node-id not initialized\n");
        }

        /* start CAN */
        CO_CANsetNormalMode(CO->CANmodule);

        reset = CO_RESET_NOT;

        printf("CANopenNode - Running...\n");
        fflush(stdout);

        while (reset == CO_RESET_NOT) {
            timeout = 1U; /* default timeout in milliseconds */
            timestamp = mchtmr_get_count(HPM_MCHTMR);
            /* loop for normal program execution ******************************************/

            /* CANopen process */
            reset = CO_process(CO, false, (uint16_t)elapsed * 1000, &timeout);
            LED_green = CO_LED_GREEN(CO->LEDs, CO_LED_CANopen);
            board_led_write(LED_green);

#if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
            bool_t syncWas;

            /* Process Sync */
            syncWas = CO_process_SYNC(CO, TMR_TASK_INTERVAL, NULL);

            /* Read inputs */
            CO_process_RPDO(CO, syncWas, TMR_TASK_INTERVAL, NULL);
            /* Write outputs */
            CO_process_TPDO(CO, syncWas, TMR_TASK_INTERVAL, NULL);
#endif

            if (timeout > 0) {
                board_delay_ms(timeout);
                elapsed = (mchtmr_get_count(HPM_MCHTMR) - timestamp)*1000 / mchtmr_freq;
            } else {
                elapsed = 0U;
            }
        }
    }

    /* program exit ***************************************************************/
    /* stop threads */

    /* delete objects from memory */
    CO_CANsetConfigurationMode((void *)&CANptr);
    CO_delete(CO);

    printf("CANopenNode finished\n");

    /* will not execute this line unless hardware_reset */
    hpm_sys_reboot();
}
