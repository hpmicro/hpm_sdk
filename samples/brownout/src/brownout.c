/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "pinmux.h"
#include "board.h"
#include "hpm_gpio_drv.h"

SDK_DECLARE_EXT_ISR_M(IRQn_BROWNOUT, brownout_isr)
void brownout_isr(void)
{
    gpio_write_pin(BOARD_BROWNOUT_INDICATE_GPIO_CTRL, \
                   GPIO_GET_PORT_INDEX(BOARD_BROWNOUT_INDICATE_PIN), \
                   GPIO_GET_PIN_INDEX(BOARD_BROWNOUT_INDICATE_PIN), \
                   1);
}

int main(void)
{
    board_init();
    board_init_brownout_indicate_pin();
    intc_m_enable_irq_with_priority(IRQn_BROWNOUT, 1);
    printf("GPIO will output high when brownout interrupt occurs\r\n");
    while (1) {
    }
    return 0;
}
