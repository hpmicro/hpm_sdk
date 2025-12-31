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
#include "eoe_ethernetif.h"
#include "tcp_echo.h"

extern struct netif eoe_netif;

int main(void)
{
    hpm_stat_t stat;
    board_init();
    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */
    printf("EtherCAT EOE TCP echo sample with LwIP stack.\n");

    /* ESC hardware init, LwIP and SSC share the same tick clock */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        printf("Init ESC peripheral and related devices(EEPROM/PHY) failed!\n");
        return 0;
    }

    /* LwIP init */
    printf("\nLwIP Version: %s\n", LWIP_VERSION_STRING);
    lwip_init();
    eoe_netif_config(&eoe_netif);
    tcp_echo_init();

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
    #if defined(LWIP_TIMERS) && LWIP_TIMERS
        sys_check_timeouts();
    #endif
    }

    /* hardware deinit */

    return 0;
}



