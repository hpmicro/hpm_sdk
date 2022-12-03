/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hpm_common.h"
#include "disp.h"
#include "erpc_client_setup.h"
#include "erpc_matrix_multiply.h"
#include "hpm_debug_console.h"
#include "hpm_gpio_drv.h"
#include "hpm_pdma_drv.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_l1c_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_misc.h"
#include "hpm_pmp_drv.h"
#include "hpm_sysctl_drv.h"
#include "mp4.h"
#include "rpmsg_lite.h"
#include "sd_fatfs_common.h"
#include "sec_core_img.h"
#include "uart.h"
#include "convert.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_CONSOLE_BASE HPM_UART13
#define APP_CONSOLE_CLK_NAME clock_uart13
#define APP_CONSOLE_BAUDRATE (1000000UL)
#define APP_CONSOLE_TYPE console_type_uart

#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define MATRIX_ITEM_MAX_VALUE (50)
#define APP_ERPC_READY_EVENT_DATA (1U)

#define DECODE_BUFFER_SIZE (16 * 1024)
#define PACKET_DEAL_MIN_SIZE (2 * 1024)
/*******************************************************************************
 * Variables
 ******************************************************************************/

size_t decode_buffer_avilable_size;
size_t decode_buffer_read_size = 0;
static volatile uint16_t eRPCReadyEventData;
static bool invert_led_level;
ATTR_RPMSG unsigned char h264_buf[DECODE_BUFFER_SIZE];
extern uint16_t framebuffer[2][BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT];
ff_measure_context_t s_ffMeasureContext;
ff_measure_sum_t s_ffMeasureSum;
int *pframe_data[3];

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


/* Wait stream data from uart, until it's size is >= len */
void wait_data_until_size(size_t len)
{
    while (decode_buffer_read_size < len) {
        decode_buffer_read_size += get_uart_data(h264_buf + decode_buffer_read_size, &decode_buffer_avilable_size);
    }
}

/*!
 * @brief eRPC server side ready event handler
 */
static void eRPCReadyEventHandler(uint16_t eventData, void *context)
{
    eRPCReadyEventData = eventData;
}

/* wait until core1 decode done */
void wait_core1_decode_done(void)
{
    int ret = 0;

    /* loop of get frame data and display */
    while (1) {
        ret = get_core1_frame_and_display();
        /* ret == 0 ------> core1 do not decode one frame out */
        if (ret == 0) {
            if (erpc_get_undealed_data_size() == 0) {
                /* core1 already decoded all data */
                printf("exit to get more data\n");
                return;
            }
        }
    }
}

int rx_stream_display_loop(void)
{

    decode_buffer_avilable_size = DECODE_BUFFER_SIZE;
    decode_buffer_read_size = 0;

    while (1) {
        wait_data_until_size(PACKET_DEAL_MIN_SIZE);
        printf("get %d bytes from uart\n", decode_buffer_read_size);
        printf("---------------------------------------\n\n\n");
        printf("send 0x%08x(%d) to core1\n", h264_buf, decode_buffer_read_size);
        printf("---------------------------------------\n\n\n");

        erpc_transfer_stream_data(h264_buf, decode_buffer_read_size);
        ff_time_measure_utility(kFfTimeType_Start);
        wait_core1_decode_done();
        // Reset decode buffer avilable size and read size to get a new packet.
        decode_buffer_read_size = 0;
        decode_buffer_avilable_size = DECODE_BUFFER_SIZE;
    }
    return 0;
}

int get_core1_frame_and_display(void)
{

    int **buf; /*   buf[0]: Y
                    buf[1]: U
                    buf[2] : V */

    int has_decoded_frame;
    erpc_get_decoded_frame(&buf, &has_decoded_frame);
    if (has_decoded_frame == 0)
        return 0;
    ff_time_measure_utility(kFfTimeType_DecodeVideo);
    float time_tmp = 1.0f * s_ffMeasureContext.decodeVideo_ns / 1E6;
    ff_time_measure_utility(kFfTimeType_Start);
    pframe_data[0] = buf[0];
    pframe_data[1] = buf[1];
    pframe_data[2] = buf[2];

    yuv420p_to_yuyv((unsigned char **)pframe_data, (unsigned char *)&framebuffer[1], 640, 360);

    ff_time_measure_utility(kFfTimeType_TransEnd);
    uint32_t status;
    pdma_scale(BOARD_PDMA_BASE,
        (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&framebuffer[0]),
        800,
        (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&framebuffer[1]),
        640,
        0, 0, 640, 360,
        800, 480,
        0xFF,
        display_pixel_format_ycbcr422,
        true, &status);
    ff_time_measure_utility(kFfTimeType_Pdma);
    float time_tmp1 = 1.0f * (s_ffMeasureContext.decodeVideo_TransEnd - s_ffMeasureContext.decodeVideo_TransStart) / 1E6;
    float time_tmp2 = 1.0f * (s_ffMeasureContext.decodeVideo_Pdma - s_ffMeasureContext.decodeVideo_TransEnd) / 1E6;

    printf("decode use %f ms ,trans %f pdma %f\r\n", time_tmp, time_tmp1, time_tmp2);
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

static void board_turnoff_rgb_led(void)
{
    uint8_t p11_stat;
    uint8_t p12_stat;
    uint8_t p13_stat;
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
    HPM_IOC->PAD[IOC_PAD_PB11].FUNC_CTL = IOC_PB11_FUNC_CTL_GPIO_B_11;
    HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PB12_FUNC_CTL_GPIO_B_12;
    HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PB13_FUNC_CTL_GPIO_B_13;

    HPM_IOC->PAD[IOC_PAD_PB11].PAD_CTL = pad_ctl;
    HPM_IOC->PAD[IOC_PAD_PB12].PAD_CTL = pad_ctl;
    HPM_IOC->PAD[IOC_PAD_PB13].PAD_CTL = pad_ctl;

    p11_stat = gpio_read_pin(BOARD_G_GPIO_CTRL, GPIO_DI_GPIOB, 11);
    p12_stat = gpio_read_pin(BOARD_G_GPIO_CTRL, GPIO_DI_GPIOB, 12);
    p13_stat = gpio_read_pin(BOARD_G_GPIO_CTRL, GPIO_DI_GPIOB, 13);

    invert_led_level = false;
    /*
     * check led gpio level
     */
    if ((p11_stat & p12_stat & p13_stat) == 0) {
        /* Rev B */
        invert_led_level = true;
        pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(0);
        HPM_IOC->PAD[IOC_PAD_PB11].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PB12].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PB13].PAD_CTL = pad_ctl;
    }
}

static void demo_board_init(void)
{
    board_turnoff_rgb_led();
    board_init_clock();
    app_board_init_console();
    board_init_pmp();
    board_init_ahb();
#if BOARD_SHOW_CLOCK
    board_print_clock_freq();
#endif
#if BOARD_SHOW_BANNER
    board_print_banner();
#endif

}

/*!
 * @brief Main function
 */
int main(void)
{
    demo_board_init();
    board_init_gpio_pins();
    init_button();
    board_init_lcd();
    init_rpmsg_share_mem_noncacheable();
    ipc_init();
    ipc_enable_event_interrupt(2);

    init_lcd();
    uart_dma_rx_init();

    /* Register the application event before starting the secondary core */
    (void)ipc_register_event(ipc_remote_start_event, eRPCReadyEventHandler, NULL);

    (void)printf("\r\nPrimary core started\r\n");
    if (!sysctl_is_cpu1_released(HPM_SYSCTL)) {
        printf("\n\n");
        printf("Copying secondary core image to destination memory...\n");
        uint32_t sec_core_img_sys_addr = core_local_mem_to_sys_address(HPM_CORE1, (uint32_t)SEC_CORE_IMG_START);
        memcpy((void *)sec_core_img_sys_addr, sec_core_img, sec_core_img_size);
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(sec_core_img_sys_addr);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(sec_core_img_sys_addr + sec_core_img_size);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
        sysctl_set_cpu1_entry(HPM_SYSCTL, (uint32_t)(SEC_CORE_IMG_START + 0x8000));
        sysctl_release_cpu1(HPM_SYSCTL);
    }

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

    while (1) {
        rx_stream_display_loop();
    }
}