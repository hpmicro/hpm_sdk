/*
 * Copyright (c) 2022-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "usb_osal.h"

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v2_mic_speaker.h"
#include "usb_config.h"
#include "rtconfig.h"
#include "rtt_port.h"
#include "rtthread.h"

#define AUDIO_TASK_PRIORITY (RT_THREAD_PRIORITY_MAX - 5U)

static void audio_task(void *argument)
{
    (void)argument;

    while (1) {
        audio_v2_task(0);
        usb_osal_msleep(10U);
    }
}

void rt_hw_board_init(void)
{
    board_init();
    rtt_base_init();

    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    board_init_dao_clock();
    init_dao_pins();
    board_init_pdm_clock();
    init_pdm_pins();
}

int main(void)
{
    printf("CherryUSB audio v2 mic and speaker rtthread sample.\n");

    speaker_init_i2s_dao();
    mic_init_i2s_pdm();

    i2s_enable_dma_irq_with_priority(2);
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);
    audio_v2_init(0, CONFIG_HPM_USBD_BASE);

    usb_osal_thread_t task = usb_osal_thread_create("audio", 4096U, AUDIO_TASK_PRIORITY, audio_task, NULL);
    if (task == NULL) {
        printf("audio task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    return 0;
}
