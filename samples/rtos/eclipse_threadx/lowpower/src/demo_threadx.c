/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
 * threads of different priorities, using a message queue, semaphore, mutex, event flags group,
 * byte pool, and block pool.
 */

#include "tx_api.h"
#include "board.h"
#include <stdio.h>
#define DEMO_STACK_SIZE 1024
#define DEMO_BYTE_POOL_SIZE 9120
#define DEMO_BLOCK_POOL_SIZE 100

/* Define the ThreadX object control blocks...  */

TX_THREAD thread_0;
TX_BYTE_POOL byte_pool_0;
TX_BLOCK_POOL block_pool_0;
UCHAR memory_area[DEMO_BYTE_POOL_SIZE];

/* Define the counters used in the demo application...  */

ULONG thread_0_counter;

/* Define thread prototypes.  */

void thread_0_entry(ULONG thread_input);

/* Define main entry point.  */

int main(void)
{
    board_init();
    board_init_gpio_pins();
#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, BOARD_LED_ON_LEVEL);
#endif
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */

void tx_application_define(void *first_unused_memory)
{

    CHAR *pointer = TX_NULL;
    (void)first_unused_memory;

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
        pointer, DEMO_STACK_SIZE,
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Release the block back to the pool.  */
    tx_block_release(pointer);
}

/* Define the test threads.  */

void thread_0_entry(ULONG thread_input)
{
    TX_INTERRUPT_SAVE_AREA
    (void)thread_input;
    TX_DISABLE;
    printf("%s\n", __func__);
    TX_RESTORE;
    /* This thread simply sits in while-forever-sleep loop.  */
    while (1) {


        /* Print results.  */
        printf("**** ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro\n");
        printf(" soc will enter lowpower mode during tx_thread_sleep: %lu\n\n", thread_0_counter);
        /* Sleep for 10 ticks.  */
        tx_thread_sleep(50);

        /* Increment the thread counter.  */
        thread_0_counter++;
    }
}
