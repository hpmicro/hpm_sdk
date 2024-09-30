/*
 * Copyright (c) 2024 HPMicro
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
#include "hpm_ecat_hw.h"
#include "cia402appl.h"
#if defined(CONFIG_CIA402_USING_ACTUAL_MOTOR) && CONFIG_CIA402_USING_ACTUAL_MOTOR
#include "bldc_foc.h"
#endif

int main(void)
{
    hpm_stat_t stat;
    board_init();
    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */
    printf("EtherCAT CiA402 motor sample\n");

    /* hardware init */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        printf("Init ESC peripheral and related devices(EEPROM/PHY) failed!\n");
        return 0;
    }

    board_delay_ms(1000);

#if defined(CONFIG_CIA402_USING_ACTUAL_MOTOR) && CONFIG_CIA402_USING_ACTUAL_MOTOR
    motor_function_init(); /* motor function init */
#endif

    MainInit(); /* SSC Initilize the stack */

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
    pAPPL_EEPROM_Read  = ecat_eeprom_emulation_read;
    pAPPL_EEPROM_Write = ecat_eeprom_emulation_write;
    pAPPL_EEPROM_Reload = ecat_eeprom_emulation_reload;
    pAPPL_EEPROM_Store  = ecat_eeprom_emulation_store;
#endif

    /*Initialize Axes structures*/
    CiA402_Init();

    /* Create basic mapping */
    APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);

    /* Set stack run flag */
    bRunApplication = TRUE;
    /* Execute the stack */
    while (bRunApplication == TRUE) {
        MainLoop();
    }

    CiA402_DeallocateAxis();

    /* hardware deinit */

    return 0;
}
