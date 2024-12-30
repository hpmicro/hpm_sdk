/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_misc.h"
#include "hpm_dmav2_drv.h"
#include "hpm_interrupt.h"

/* Define */

#define TEST_BUFFER_SIZE           64   /* Fixed burst max size: XDMA 128 bytes, HDMA 64 bytes */
#define TEST_DMA_CONFIG_BURST_SIZE DMA_NUM_TRANSFER_PER_BURST_16T
#define TEST_BURST_SIZE            (1 << TEST_DMA_CONFIG_BURST_SIZE)
#define TEST_DMA_CH                31

/* dma buffer should be 4-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(128) uint8_t s_burst_buf[TEST_BUFFER_SIZE];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(128) uint8_t s_normal_buf[TEST_BUFFER_SIZE * TEST_BURST_SIZE];
volatile bool dma_transfer_done;
volatile bool dma_transfer_check_err;

static void prepare_test_data(void);
static void test_dma_src_addr_fix_burst_non_swap(void);
static void test_dma_src_addr_fix_burst_swap_by_byte(void);
static void test_dma_src_addr_fix_burst_swap_by_half_word(void);
static void test_dma_src_addr_fix_burst_swap_by_swap_table(void);

int main(void)
{
    board_init();

    intc_m_enable_irq_with_priority(TEST_DMA_IRQ, 1);

    test_dma_src_addr_fix_burst_non_swap();
    test_dma_src_addr_fix_burst_swap_by_byte();
    test_dma_src_addr_fix_burst_swap_by_half_word();
    test_dma_src_addr_fix_burst_swap_by_swap_table();

    while (1) {
        ;
    }

    return 0;
}

SDK_DECLARE_EXT_ISR_M(TEST_DMA_IRQ, dma_isr)
void dma_isr(void)
{
    uint32_t stat;

    stat = dma_check_transfer_status(TEST_DMA_CONTROLLER, TEST_DMA_CH);
    if (0 != (stat & DMA_CHANNEL_STATUS_ERROR)) {
        printf("dma transfer failure!\n");
    }

    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        dma_transfer_done = true;
    }
}

/*
 * Static Function Definition
 */
static void prepare_test_data(void)
{
    for (uint32_t i = 0; i < sizeof(s_burst_buf); i++) {
        s_burst_buf[i] = i;
    }

    for (uint32_t i = 0; i < sizeof(s_normal_buf); i++) {
        s_normal_buf[i] = 0;
    }
}

static void test_dma_src_addr_fix_burst_non_swap(void)
{
    hpm_stat_t stat = status_success;
    dma_channel_config_t ch_config = { 0 };

    printf("1. start dma src address fix burst none swap transfer!\n");

    dma_transfer_done = false;
    prepare_test_data();

    dma_default_channel_config(TEST_DMA_CONTROLLER, &ch_config);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_burst_buf[0]);
    ch_config.dst_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_normal_buf[0]);
    ch_config.burst_opt = DMA_SRC_BURST_OPT_STANDAND_SIZE;
    ch_config.src_burst_size = TEST_DMA_CONFIG_BURST_SIZE; /* SrcBurstSize x SrcWidth = 16 * 4 = 64 bytes*/
    ch_config.size_in_byte = TEST_BUFFER_SIZE * TEST_BURST_SIZE;
    ch_config.en_src_burst_in_fixed_trans = true;
    ch_config.en_dst_burst_in_fixed_trans = false;
    stat = dma_setup_channel(TEST_DMA_CONTROLLER, TEST_DMA_CH, &ch_config, true);
    if (stat != status_success) {
        printf("1. dma set channel failure!\n");
        return;
    }

    while (!dma_transfer_done) {
        ;
    }

    dma_transfer_check_err = false;
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE * TEST_BURST_SIZE; i++) {
        if (s_burst_buf[i % TEST_BUFFER_SIZE] != s_normal_buf[i]) {
            dma_transfer_check_err = true;
            printf("1. dma check swap byte failure! index:%d\n", i);
            break;
        }
    }

    if (!dma_transfer_check_err) {
        printf("1. dma transfer check ok!\n");
    }
}

static void test_dma_src_addr_fix_burst_swap_by_byte(void)
{
    hpm_stat_t stat = status_success;
    dma_channel_config_t ch_config = { 0 };

    printf("2. start dma src address fix burst swap by byte transfer!\n");

    dma_transfer_done = false;
    prepare_test_data();

    dma_default_channel_config(TEST_DMA_CONTROLLER, &ch_config);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_burst_buf[0]);
    ch_config.dst_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_normal_buf[0]);
    ch_config.burst_opt = DMA_SRC_BURST_OPT_STANDAND_SIZE;
    ch_config.src_burst_size = TEST_DMA_CONFIG_BURST_SIZE; /* SrcBurstSize x SrcWidth = 16 * 4 = 64 bytes*/
    ch_config.size_in_byte = TEST_BUFFER_SIZE * TEST_BURST_SIZE;
    ch_config.en_src_burst_in_fixed_trans = true;
    ch_config.en_dst_burst_in_fixed_trans = false;
    ch_config.swap_mode = DMA_SWAP_MODE_BYTE;
    stat = dma_setup_channel(TEST_DMA_CONTROLLER, TEST_DMA_CH, &ch_config, true);
    if (stat != status_success) {
        printf("2. dma set channel failure!\n");
        return;
    }

    while (!dma_transfer_done) {
        ;
    }

    dma_transfer_check_err = false;
    for (uint32_t i = 0; i < (TEST_BUFFER_SIZE * TEST_BURST_SIZE) / 4; i++) {
        if ((s_burst_buf[(4 * i) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 3]) || (s_burst_buf[(4 * i + 1) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 2]) ||
            (s_burst_buf[(4 * i + 2) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 1]) || (s_burst_buf[(4 * i + 3) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i])) {
            dma_transfer_check_err = true;
            printf("2. dma check swap byte failure! index:%d\n", i);
            break;
        }
    }

    if (!dma_transfer_check_err) {
        printf("2. dma transfer check ok!\n");
    }
}

static void test_dma_src_addr_fix_burst_swap_by_half_word(void)
{
    hpm_stat_t stat = status_success;
    dma_channel_config_t ch_config = { 0 };

    printf("3. start dma src address fix burst swap by half word transfer!\n");

    dma_transfer_done = false;
    prepare_test_data();

    dma_default_channel_config(TEST_DMA_CONTROLLER, &ch_config);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_burst_buf[0]);
    ch_config.dst_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_normal_buf[0]);
    ch_config.burst_opt = DMA_SRC_BURST_OPT_STANDAND_SIZE;
    ch_config.src_burst_size = TEST_DMA_CONFIG_BURST_SIZE; /* SrcBurstSize x SrcWidth = 16 * 4 = 64 bytes*/
    ch_config.size_in_byte = TEST_BUFFER_SIZE * TEST_BURST_SIZE;
    ch_config.en_src_burst_in_fixed_trans = true;
    ch_config.en_dst_burst_in_fixed_trans = false;
    ch_config.swap_mode = DMA_SWAP_MODE_HALF_WORD;
    stat = dma_setup_channel(TEST_DMA_CONTROLLER, TEST_DMA_CH, &ch_config, true);
    if (stat != status_success) {
        printf("3. dma set channel failure!\n");
        return;
    }

    while (!dma_transfer_done) {
        ;
    }

    dma_transfer_check_err = false;
    for (uint32_t i = 0; i < (TEST_BUFFER_SIZE * TEST_BURST_SIZE) / 4; i++) {
        if ((s_burst_buf[(4 * i) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 2]) || (s_burst_buf[(4 * i + 1) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 3]) ||
            (s_burst_buf[(4 * i + 2) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i]) || (s_burst_buf[(4 * i + 3) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 1])) {
            dma_transfer_check_err = true;
            printf("3. dma check swap half word failure! index:%d\n", i);
            break;
        }
    }

    if (!dma_transfer_check_err) {
        printf("3. dma transfer check ok!\n");
    }
}

static void test_dma_src_addr_fix_burst_swap_by_swap_table(void)
{
    hpm_stat_t stat = status_success;
    dma_channel_config_t ch_config = { 0 };

    printf("4. start dma src address fix burst swap by swap table transfer!\n");

    dma_transfer_done = false;
    prepare_test_data();

    dma_default_channel_config(TEST_DMA_CONTROLLER, &ch_config);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_burst_buf[0]);
    ch_config.dst_addr = core_local_mem_to_sys_address(0, (uint32_t)&s_normal_buf[0]);
    ch_config.burst_opt = DMA_SRC_BURST_OPT_STANDAND_SIZE;
    ch_config.src_burst_size = TEST_DMA_CONFIG_BURST_SIZE; /* SrcBurstSize x SrcWidth = 16 * 4 = 64 bytes*/
    ch_config.size_in_byte = TEST_BUFFER_SIZE * TEST_BURST_SIZE;
    ch_config.en_src_burst_in_fixed_trans = true;
    ch_config.en_dst_burst_in_fixed_trans = false;
    ch_config.swap_mode = DMA_SWAP_MODE_TABLE;
    ch_config.swap_table = 0x64752031; /* order */
    stat = dma_setup_channel(TEST_DMA_CONTROLLER, TEST_DMA_CH, &ch_config, true);
    if (stat != status_success) {
        printf("4. dma set channel failure!\n");
        return;
    }

    while (!dma_transfer_done) {
        ;
    }

    dma_transfer_check_err = false;
    for (uint32_t i = 0; i < (TEST_BUFFER_SIZE * TEST_BURST_SIZE) / 4; i++) {
        if ((s_burst_buf[(4 * i + 1) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i]) || (s_burst_buf[(4 * i + 3) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 1]) ||
            (s_burst_buf[(4 * i + 0) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 2]) ||
            (s_burst_buf[(4 * i + 2) % TEST_BUFFER_SIZE] != s_normal_buf[4 * i + 3])) {
            dma_transfer_check_err = true;
            printf("4. dma check swap table failure! index:%d\n", i);
            break;
        }
    }

    if (!dma_transfer_check_err) {
        printf("4. dma transfer check ok!\n");
    }
}
