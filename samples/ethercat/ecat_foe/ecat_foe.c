/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "foe.h"
#include "applInterface.h"
#include "hpm_ecat_hw.h"
#include "hpm_ecat_foe.h"

extern bool foe_reset_request;

int main(void)
{
    hpm_stat_t stat;
    board_init();
    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */
    printf("ECAT FOE sample\n");
    printf("Write or Read file from flash by FOE\n");

    /* Config ESC with FOE function to download app */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        printf("Init ESC peripheral and related devices(EEPROM/PHY) failed!\n");
        return -1;
    }

    MainInit(); /* SSC Initialize the stack */

    stat = foe_support_init();
    if (stat != status_success) {
        printf("FOE support init failed!!\n");
        return -1;
    }

    pAPPL_FoeRead = foe_read;
    pAPPL_FoeReadData = foe_read_data;
    pAPPL_FoeWrite = foe_write;
    pAPPL_FoeWriteData = foe_write_data;

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
    pAPPL_EEPROM_Read  = ecat_eeprom_emulation_read;
    pAPPL_EEPROM_Write = ecat_eeprom_emulation_write;
    pAPPL_EEPROM_Reload = ecat_eeprom_emulation_reload;
    pAPPL_EEPROM_Store  = ecat_eeprom_emulation_store;
#endif

    /* Create basic mapping */
    APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);

    /* Set stack run flag */
    bRunApplication = TRUE;
    /* Execute the stack */
    while (bRunApplication == TRUE) {
        if (foe_reset_request) {
            printf("system reset...\n");
            foe_support_soc_reset();
        }
        MainLoop();
    }

    /* hardware deinit */

    return 0;
}

