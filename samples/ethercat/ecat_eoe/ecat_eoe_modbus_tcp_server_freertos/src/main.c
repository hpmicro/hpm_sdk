/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "ecat_def.h"
#include "ecatappl.h"
#include "ecatslv.h"
#include "applInterface.h"
#include "eoe.h"
#include "hpm_ecat_hw.h"

#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "eoe_ethernetif.h"
#include "modbus_application.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

extern struct netif eoe_netif;

#define LWIP_TASK_PRIORITY (configMAX_PRIORITIES - 3)
void lwip_task(void *pvParameters);

#define ECAT_TASK_PRIORITY (configMAX_PRIORITIES - 2)
void ecat_task(void *pvParameters);

int main(void)
{
    hpm_stat_t stat;
    board_init();
    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */
    printf("EtherCAT EOE ModBus TCP Server sample with LwIP stack and Freertos.\n");

    /* ESC hardware init, LwIP and SSC share the same tick clock */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        printf("Init ESC peripheral and related devices(EEPROM/PHY) failed!\n");
        return 0;
    }

    xTaskCreate(lwip_task, "lwip_task", configMINIMAL_STACK_SIZE * 2, NULL, LWIP_TASK_PRIORITY, NULL);

    xTaskCreate(ecat_task, "ecat_task", configMINIMAL_STACK_SIZE * 2, NULL, ECAT_TASK_PRIORITY, NULL);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    while (1) {
    }
}

void lwip_task(void *pvParameters)
{
    (void)pvParameters;

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);
    eoe_netif_config(&eoe_netif);
    modbus_tcp_server_init(); /* ModBus TCP Server init */

    vTaskDelete(NULL);
}

void ecat_task(void *pvParameters)
{
    (void)pvParameters;

    MainInit(); /* SSC Initialize the stack */

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
    pAPPL_EEPROM_Read  = ecat_eeprom_emulation_read;
    pAPPL_EEPROM_Write = ecat_eeprom_emulation_write;
    pAPPL_EEPROM_Reload = ecat_eeprom_emulation_reload;
    pAPPL_EEPROM_Store  = ecat_eeprom_emulation_store;
#endif

    /* ECAT EoE */
    pAPPL_EoeReceive = eoe_receive;
    pAPPL_EoeSettingInd = eoe_setip;

    /* Create basic mapping */
    APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);

    /* Set stack run flag */
    bRunApplication = TRUE;
    /* Execute the stack */
    while (bRunApplication == TRUE) {
        MainLoop();
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}



