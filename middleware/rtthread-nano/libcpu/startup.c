/*
 * Copyright (c) 2021-2023 HPMicro
 *
 *
 */

#include <rtthread.h>
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"

extern int entry(void);
extern void system_init(void);

void reset_handler(void)
{
    fencei();

    /* Call platform specific hardware initialization */
    system_init();

    /* Entry function */
    entry();
}

