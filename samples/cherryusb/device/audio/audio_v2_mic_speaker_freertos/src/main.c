/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v2_mic_speaker.h"

#define task_speaker_PRIORITY    (configMAX_PRIORITIES - 5U)
#define task_mic_PRIORITY    (configMAX_PRIORITIES - 5U)

extern void task_speaker_control(void *pvParameters);
extern void task_speaker_play(void *pvParameters);
extern void task_mic_control(void *pvParameters);
extern void task_mic_play(void *pvParameters);

extern TaskHandle_t task_handle_speaker_play;
extern TaskHandle_t task_handle_mic_play;
int main(void)
{
    board_init();
    board_init_usb_pins();

    board_init_dao_clock();
    init_dao_pins();
    board_init_pdm_clock();
    init_pdm_pins();

    printf("cherry usb audio v2 mic and speaker sample.\n");

    intc_set_irq_priority(IRQn_USB0, 2);
    i2s_enable_dma_irq_with_priority(1);

    cherryusb_audio_v2_init();
    speaker_init_i2s_dao_codec();
    mic_init_i2s_pdm();

    if (xTaskCreate(task_speaker_control, "speaker_control", configMINIMAL_STACK_SIZE + 256U, NULL, task_speaker_PRIORITY, NULL) != pdPASS) {
        printf("task_speaker_control creation failed!.\n");
        for (;;) {
            ;
        }
    }
    if (xTaskCreate(task_speaker_play, "speaker_play", configMINIMAL_STACK_SIZE + 256U, NULL, task_speaker_PRIORITY, &task_handle_speaker_play) != pdPASS) {
        printf("task_speaker_play creation failed!.\n");
        for (;;) {
            ;
        }
    }
    if (xTaskCreate(task_mic_control, "mic_control", configMINIMAL_STACK_SIZE + 256U, NULL, task_mic_PRIORITY, NULL) != pdPASS) {
        printf("task_mic_control creation failed!.\n");
        for (;;) {
            ;
        }
    }
    if (xTaskCreate(task_mic_play, "mic_play", configMINIMAL_STACK_SIZE + 256U, NULL, task_mic_PRIORITY, NULL) != pdPASS) {
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
