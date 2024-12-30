/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "usb_osal.h"

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v2_mic_speaker.h"
#include "usb_config.h"

#define task_speaker_PRIORITY    (configMAX_PRIORITIES - 5U)
#define task_mic_PRIORITY    (configMAX_PRIORITIES - 5U)

extern void task_speaker_play(void *pvParameters);
extern void task_mic_play(void *pvParameters);

extern usb_osal_thread_t task_handle_speaker_play;
extern usb_osal_thread_t task_handle_mic_play;
int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    board_init_dao_clock();
    init_dao_pins();
    board_init_pdm_clock();
    init_pdm_pins();

    printf("cherry usb audio v2 mic and speaker sample.\n");

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    i2s_enable_dma_irq_with_priority(1);

    audio_v2_init(0, CONFIG_HPM_USBD_BASE);
    speaker_init_i2s_dao_codec();
    mic_init_i2s_pdm();

    task_handle_speaker_play = usb_osal_thread_create("speaker_play", 8192U, task_speaker_PRIORITY, task_speaker_play, NULL);
    if (task_handle_speaker_play == NULL) {
        printf("task_speaker_play creation failed!.\n");
        for (;;) {
            ;
        }
    }

    task_handle_mic_play = usb_osal_thread_create("mic_play", 8192U, task_mic_PRIORITY, task_mic_play, NULL);
    if (task_handle_mic_play == NULL) {
        printf("task_mic_play creation failed!.\n");
        for (;;) {
            ;
        }
    }
    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}
