/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_mchtmr_drv.h"
#include "sd_fatfs_common.h"
#include <stdio.h>
#include "hpm_uart_drv.h"


#define TEST_UART HPM_UART13
#define TEST_UART_IRQ IRQn_UART13
#define TEST_UART_CLK_NAME clock_uart13
#define TEST_UART_DMA_CONTROLLER BOARD_APP_HDMA
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX
#define PER_TRANSFER_SIZE     (1024)

FIL h264_file;

static void send_h264_stream_file(const char *infilename);
static void h264_stream_tx_init(void);

int main(void)
{
    FRESULT ff_result = FR_DISK_ERR;

    board_init();

    h264_stream_tx_init();

    mchtmr_init_counter(HPM_MCHTMR, 0);

    while (FR_OK != ff_result) {
        ff_result = sd_mount_fs();
    }

    char *filepath_in = "out.h264";

    do {
        send_h264_stream_file(filepath_in);
    } while (1);

    return 0;
}
void send_h264_stream_file(const char *infilename)
{
    printf("send file '%s'\n", infilename);
    unsigned int bw_wh;
    /* Check input/output media file */
    FRESULT result;
    result = f_open(&h264_file, infilename, FA_READ | FA_OPEN_EXISTING);
    if (result != FR_OK) {
        printf("Could not open '%s', erro = %d\n", infilename, result);
        return;
    }

    unsigned char* h264_buf = malloc(1024 * 1024 * 3);
    while (1) {
        result = f_read(&h264_file, h264_buf, 1024 * 1024 * 3, &bw_wh);
        if (result != FR_OK) {
            printf("fread error %d, size %d\n", result, bw_wh);
            break;
        } else {
            while (1) {
                printf("fread %d bytres\n", bw_wh);
                for(int i = 0; i < bw_wh / PER_TRANSFER_SIZE; i++) {
                    uart_send_data(TEST_UART, h264_buf + PER_TRANSFER_SIZE * i, PER_TRANSFER_SIZE);
                    if (i % (128) == 0)
                        printf(".\n");
                    uint64_t tick = mchtmr_get_count(HPM_MCHTMR);
                    while (mchtmr_get_count(HPM_MCHTMR) - tick < 24E4) {
                            __asm volatile("nop");
                    }
                }
                printf("wait\n");
                uint64_t tic = mchtmr_get_count(HPM_MCHTMR);
                while (mchtmr_get_count(HPM_MCHTMR) - tic < 24E6 * 5) {
                        __asm volatile("nop");
                }
            }
        }
    }
    f_close(&h264_file);
}



void h264_stream_tx_init(void)
{

    uart_config_t config = {0};
    hpm_stat_t stat;

    uart_default_config(TEST_UART, &config);
    config.baudrate = 1000000;
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    board_init_uart(TEST_UART);
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }

    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, 1);


}