/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_mono_drv.h"

#define LED_FLASH_PERIOD_IN_MS 300
#define NUM 10

bool isIncrease(uint64_t a[], int n)
{
    if (n == 1)
        return true;
    return (a[n-1] == a[n-2] + 1) && isIncrease(a, n-1);
}

int main(void)
{
    int i = 0;
    uint64_t mono_value[NUM] = {0};

    board_init();
    board_init_led_pins();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("mono test start!\n");

    for (i = 0; i < NUM; i++) {
        mono_value[i] = mono_get_counter(HPM_MONO);
        mono_update(HPM_MONO);
    }

    if (isIncrease(mono_value, NUM)) {
        printf("mono test pass!\n");
    } else {
        printf("mono test fail!\n");
    }

    printf("mono test end!\n");

    while (1) {
    }

    return 0;
}
