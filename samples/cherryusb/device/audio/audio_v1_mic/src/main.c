/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v1_mic.h"
#include "usb_config.h"

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    board_init_pdm_clock();
    init_pdm_pins();

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    i2s_enable_dma_irq_with_priority(1);

    printf("cherry usb audio v1 mic sample.\n");

    audio_v1_init(0, CONFIG_HPM_USBD_BASE);
    init_mic_i2s_pdm();

    while (1) {
        audio_v1_task(0);
    }
    return 0;
}
