/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_gpio_drv.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_l1c_drv.h"
#include "hpm_misc.h"
#include "hpm_pmp_drv.h"
#include "hpm_sysctl_drv.h"
#include "erpc_client_setup.h"
#include "erpc_matrix_multiply.h"
#include "rpmsg_lite.h"
#include "multicore_common.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define MATRIX_ITEM_MAX_VALUE (50)
#define APP_ERPC_READY_EVENT_DATA (1U)


/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile uint16_t eRPCReadyEventData;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void init_button(void)
{
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

/*!
 * @brief Fill matrices by random values
 */
static void fill_matrices(Matrix matrix1_ptr, Matrix matrix2_ptr)
{
    int32_t a, b;

    /* Fill both matrices by random values */
    for (a = 0; a < matrix_size; ++a) {
        for (b = 0; b < matrix_size; ++b) {
            matrix1_ptr[a][b] = rand() % MATRIX_ITEM_MAX_VALUE;
            matrix2_ptr[a][b] = rand() % MATRIX_ITEM_MAX_VALUE;
        }
    }
}

/*!
 * @brief Printing a matrix to the console
 */
static void print_matrix(Matrix matrix_ptr)
{
    int32_t a, b;

    for (a = 0; a < matrix_size; ++a) {
        for (b = 0; b < matrix_size; ++b) {
            (void)printf("%4i ", matrix_ptr[a][b]);
        }
        (void)printf("\r\n");
    }
}

/*!
 * @brief eRPC server side ready event handler
 */
static void eRPCReadyEventHandler(uint16_t eventData, void *context)
{
    (void)context;
    eRPCReadyEventData = eventData;
}

/*!
 * @brief Main function
 */
int main(void)
{
    Matrix matrix1 = { 0 }, matrix2 = { 0 }, result_matrix = { 0 };

    board_init();
    board_init_gpio_pins();
    init_button();
    ipc_init();
    ipc_enable_event_interrupt(2);

    /* Register the application event before starting the secondary core */
    (void)ipc_register_event(ipc_remote_start_event, eRPCReadyEventHandler, NULL);

    printf("\r\nPrimary core started\r\n");
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);
    printf("Starting secondary core...\r\n");

    /*
     * Wait until the secondary core application signals the rpmsg remote has
     * been initialized and is ready to communicate.
     */
    while (APP_ERPC_READY_EVENT_DATA != eRPCReadyEventData) {
    };

    printf("\r\nSecondary core started...\r\n");

    /* RPMsg-Lite transport layer initialization */
    erpc_transport_t transport;

    transport = erpc_transport_rpmsg_lite_master_init(100, 101, ERPC_TRANSPORT_RPMSG_LITE_LINK_ID);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(transport);

    /* eRPC client side initialization */
    erpc_client_init(transport, message_buffer_factory);

    for (;;) {
        /* Fill both matrices by random values */
        fill_matrices(matrix1, matrix2);

        /* Print both matrices on the console */
        (void)printf("\r\nMatrix #1");
        (void)printf("\r\n=========\r\n");
        print_matrix(matrix1);

        (void)printf("\r\nMatrix #2");
        (void)printf("\r\n=========\r\n");
        print_matrix(matrix2);

        (void)printf("\r\neRPC request is sent to the server\r\n");

        erpcMatrixMultiply(matrix1, matrix2, result_matrix);

        (void)printf("\r\nResult matrix");
        (void)printf("\r\n=============\r\n");
        (void)print_matrix(result_matrix);

        (void)printf("\r\nSwitch Light LED");
        erpcSwitchLightLed();

        (void)printf("\r\nPress the PBUTN button to initiate the next matrix "
                     "multiplication\r\n");

        /* Wait Button Pressed */
        while (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) != BOARD_BUTTON_PRESSED_VALUE) {
        }

        /* Wait Button Released */
        while (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == BOARD_BUTTON_PRESSED_VALUE) {
        }
    }
}
