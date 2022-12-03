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

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/url.h"
#include "mp4.h"
#include <stdio.h>
#include "h264_stream.h"
#include "app.h"
#include "hpm_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define APP_ERPC_READY_EVENT_DATA (1U)
#if !defined(SH_MEM_TOTAL_SIZE)
#define SH_MEM_TOTAL_SIZE (6144U)
#endif

#define APP_CONSOLE_BASE HPM_UART0
#define APP_CONSOLE_CLK_NAME clock_uart0
#define APP_CONSOLE_BAUDRATE (115200UL)
#define APP_CONSOLE_TYPE console_type_uart


/*******************************************************************************
 * Variables
 ******************************************************************************/
static ATTR_RPMSG char rpmsg_lite_base[SH_MEM_TOTAL_SIZE];

volatile app_config_t app_config = {
    .h264_data = NULL,
    .h264_data_len = 0,
    .flag_data_output = 0,
    .flag_decode = 0,
    .output_addr = 0,
    .got_picture = 0,
};
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

extern void init_heap5_mem(void);


static void SignalReady(void)
{
    /* Signal the other core we are ready by triggering the event and passing the APP_ERPC_READY_EVENT_DATA */
    (void)ipc_tigger_event(ipc_remote_start_event, APP_ERPC_READY_EVENT_DATA);
}

static void init_rpmsg_share_mem_noncacheable(void)
{
    extern uint32_t __rpmsg_share_mem_start__[];
    extern uint32_t __rpmsg_share_mem_end__[];

    uint32_t start_addr = (uint32_t)__rpmsg_share_mem_start__;
    uint32_t end_addr = (uint32_t)__rpmsg_share_mem_end__;
    uint32_t length = end_addr - start_addr;

    if (length == 0) {
        return;
    }

    /* Ensure the address and the length are power of 2 aligned */
    assert((length & (length - 1U)) == 0U);
    assert((start_addr & (length - 1U)) == 0U);

    pmp_entry_t pmp_entry;
    pmp_entry.pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
    pmp_entry.pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
    pmp_entry.pma_addr = PMA_NAPOT_ADDR(start_addr, length);
    pmp_entry.pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);

    pmp_config_entry(&pmp_entry, 1u);
}
erpc_service_t service;
int erpc_loop(void)
{
    /* process message */
    erpc_status_t status = erpc_server_poll();

    /* handle error status */
    if (status != (erpc_status_t)kErpcStatus_Success) {
        /* removing the service from the server */
        erpc_remove_service_from_server(service);
        destroy_MatrixMultiplyService_service(service);

        /* stop erpc server */
        erpc_server_stop();

        /* print error description */
        erpc_server_deinit();

        /* exit program loop */
        return -1;
    }
    return 1;
}

static void app_board_init_console(void)
{
    console_config_t cfg;

    /* Configure the UART clock to 24MHz */
    clock_set_source_divider(APP_CONSOLE_CLK_NAME, clk_src_osc24m, 1U);

    cfg.type = APP_CONSOLE_TYPE;
    cfg.base = (uint32_t) APP_CONSOLE_BASE;
    cfg.src_freq_in_hz = clock_get_frequency(APP_CONSOLE_CLK_NAME);
    cfg.baudrate = APP_CONSOLE_BAUDRATE;

    init_uart_pins((UART_Type *) cfg.base);

    if (status_success != console_init(&cfg)) {
        /* failed to  initialize debug console */
        while (1) {
        }
    }

}

/*!
 * @brief Main function
 */
int main(void)
{
    /* use uart13 as console*/
    app_board_init_console();
    init_heap5_mem();
    board_init_pmp();
    init_rpmsg_share_mem_noncacheable();
    board_init_led_pins();
    ipc_init();
    ipc_enable_event_interrupt(2u);

    int ret = h264_decode_init();
    if (ret) {
        printf("error while h264_decode_init %d\r\n", ret);
        while(1);
    }

    /* RPMsg-Lite transport layer initialization */
    erpc_transport_t transport;

    transport = erpc_transport_rpmsg_lite_remote_init(101U, 100U, (void *)(char *)rpmsg_lite_base,
        ERPC_TRANSPORT_RPMSG_LITE_LINK_ID, SignalReady, NULL);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(transport);

    /* eRPC server side initialization */
    (void)erpc_server_init(transport, message_buffer_factory);

    /* adding the service to the server */
    service = create_MatrixMultiplyService_service();
    erpc_add_service_to_server(service);

    while (1) {
        core1_decode_loop();
    }
}
