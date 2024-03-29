/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_pmp_drv.h"
#include "erpc_matrix_multiply.h"
#include "erpc_matrix_multiply_server.h"
#include "erpc_server_setup.h"
#include "rpmsg_lite.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define APP_ERPC_READY_EVENT_DATA (1U)
#if !defined(SH_MEM_TOTAL_SIZE)
#define SH_MEM_TOTAL_SIZE (6144U)
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
ATTR_SHARE_MEM char rpmsg_lite_base[SH_MEM_TOTAL_SIZE];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief erpcMatrixMultiply function implementation.
 *
 * This is the implementation of the erpcMatrixMultiply function called by the primary core.
 *
 * @param matrix1 First matrix
 * @param matrix2 Second matrix
 * @param result_matrix Result matrix
 */
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
    int32_t i, j, k;

    /* Clear the result matrix */
    for (i = 0; i < matrix_size; ++i) {
        for (j = 0; j < matrix_size; ++j) {
            result_matrix[i][j] = 0;
        }
    }

    /* Multiply two matrices */
    for (i = 0; i < matrix_size; ++i) {
        for (j = 0; j < matrix_size; ++j) {
            for (k = 0; k < matrix_size; ++k) {
                result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void erpcSwitchLightLed(void)
{
    board_led_toggle();
}

static void SignalReady(void)
{
    /* Signal the other core we are ready by triggering the event and passing the APP_ERPC_READY_EVENT_DATA */
    (void)ipc_tigger_event(ipc_remote_start_event, APP_ERPC_READY_EVENT_DATA);
}

/*!
 * @brief Main function
 */
int main(void)
{
    board_init_core1();
    board_init_led_pins();
    ipc_init();
    ipc_enable_event_interrupt(2u);

    printf("Secondary core started...\r\n");

    /* ERPC server initialization */
    erpc_server_t erpc_server;

    /* RPMsg-Lite transport layer initialization */
    erpc_transport_t transport;

    transport = erpc_transport_rpmsg_lite_remote_init(101U, 100U, (void *)(char *)rpmsg_lite_base,
        ERPC_TRANSPORT_RPMSG_LITE_LINK_ID, SignalReady, NULL);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(transport);

    /* eRPC server side initialization */
    erpc_server = erpc_server_init(transport, message_buffer_factory);

    /* adding the service to the server */
    erpc_service_t service = create_MatrixMultiplyService_service();
    erpc_add_service_to_server(erpc_server, service);

    for (;;) {
        /* process message */
        erpc_status_t status = erpc_server_poll(erpc_server);

        /* handle error status */
        if (status != (erpc_status_t)kErpcStatus_Success) {
            /* removing the service from the server */
            erpc_remove_service_from_server(erpc_server, service);
            destroy_MatrixMultiplyService_service(service);

            /* stop erpc server */
            erpc_server_stop(erpc_server);

            /* print error description */
            erpc_server_deinit(erpc_server);

            /* exit program loop */
            break;
        }
    }
}
