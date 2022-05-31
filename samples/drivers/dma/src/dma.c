/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dram_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_l1c_drv.h"

#define SIZE_PER_TEST   (0x00020000UL)
#define DST_ADDRESS     BOARD_SDRAM_ADDRESS + (BOARD_SDRAM_SIZE / 16)
#define SRC_ADDRESS     (0x010c0000UL)
#define TEST_DATA_LENGTH_IN_BYTE (0x2000000UL)

#ifndef TEST_DMA_CONTROLLER
#define TEST_DMA_CONTROLLER HPM_XDMA
#endif

#ifndef TEST_DMA_IRQ
#define TEST_DMA_IRQ BOARD_APP_XDMA_IRQ
#endif

#ifndef TEST_DMA_CHANNEL
#define TEST_DMA_CHANNEL (0U)
#endif

#ifndef USE_IRQ
#define USE_IRQ (1)
#endif

#ifndef DRAM_CLOCK_NAME
#define DRAM_CLOCK_NAME clock_dram
#endif

#ifndef TIMER_CLOCK_NAME
#define TIMER_CLOCK_NAME clock_mchtmr0
#endif

uint32_t timer_freq_in_hz;

volatile bool dma_transfer_done = false;
volatile bool dma_transfer_error = false;

static void reset_transfer_status(void)
{
    dma_transfer_done = false;
    dma_transfer_error = false;
}

static void prepare_test_data(uint8_t *buffer, uint32_t size_in_byte)
{
    uint32_t i = 0;
    for (i = 0; i < size_in_byte; i++) {
        buffer[i] = i;
    }
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)buffer);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)buffer + size_in_byte);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

static uint32_t compare_buffers(uint8_t *expected, uint8_t *actual, uint32_t size, bool verbose)
{
    uint32_t i, errors;
    printf("compare data between source address 0x%x and destination address 0x%x: ",
            (uint32_t *)expected, (uint32_t *)actual);
    if (verbose) {
        printf("\n");
    }
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)actual);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)actual + size);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
    }
    for(i = 0, errors = 0; i < size; i++) {
        if (*(expected + i) != *(actual + i)) {
            if (verbose) {
                printf("[%x]: expected: 0x%x, actual: 0x%x\n",
                        i, *(expected + i), *(actual + i));
            }
            errors++;
        }
    }
    if (errors) {
        printf(" ! [%d] errors encounted in total\n",errors);
    } else {
        printf(" all data matches\n");
    }
    return errors;
}

/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_linked_descriptor_t descriptors[4];

void isr_dma(void)
{
    volatile hpm_stat_t stat;
    dma_transfer_done = true;
    stat = dma_check_transfer_status(TEST_DMA_CONTROLLER, TEST_DMA_CHANNEL);
    if (0 == (stat & DMA_CHANNEL_STATUS_TC)) {
        dma_transfer_error = true;
    }
}

SDK_DECLARE_EXT_ISR_M(TEST_DMA_IRQ, isr_dma)

#define OFFSET_PER_DESCRIPTOR 0x1000
void test_chained_transfer(bool verbose)
{
    uint32_t i, errors;
    dma_channel_config_t ch_config = {0};

    intc_m_enable_irq_with_priority(TEST_DMA_IRQ, 1);

    prepare_test_data((uint8_t *)SRC_ADDRESS, SIZE_PER_TEST);
    for (i = 0; i < ARRAY_SIZE(descriptors); i++) {
        descriptors[i].trans_size = (SIZE_PER_TEST / ARRAY_SIZE(descriptors)) >> DMA_TRANSFER_WIDTH_BYTE;
        descriptors[i].src_addr = core_local_mem_to_sys_address(HPM_CORE0, SRC_ADDRESS + i * OFFSET_PER_DESCRIPTOR);
        descriptors[i].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, DST_ADDRESS + i * SIZE_PER_TEST);
        descriptors[i].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_BYTE)
                | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_BYTE)
                | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_8T);
        if (i == ARRAY_SIZE(descriptors) - 1) {
            descriptors[i].linked_ptr = 0;
        } else {
            descriptors[i].linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors[i+1]);
        }
    }
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, SRC_ADDRESS);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, DST_ADDRESS);
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_8T;
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.size_in_byte = SIZE_PER_TEST / ARRAY_SIZE(descriptors);
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors[0]);

    reset_transfer_status();
    if (status_success != dma_setup_channel(TEST_DMA_CONTROLLER, TEST_DMA_CHANNEL, &ch_config)) {
        printf(" dma setup channel failed\n");
        return;
    }
    printf(" dma setup channel done\n");

    while (!dma_transfer_done) {
        __asm("nop");
    }

    if (dma_transfer_error) {
        printf(" chained transfer failed\n");
        return;
    }
    for (i = 0; i < ARRAY_SIZE(descriptors); i++) {
        errors = compare_buffers((uint8_t *)descriptors[i].src_addr, (uint8_t *)descriptors[i].dst_addr, descriptors[i].trans_size << DMA_TRANSFER_WIDTH_BYTE, false);
        if (!errors) {
            printf(" [%d]: data match\n", i);
        } else {
            printf(" [%d]: !!! data mismatch\n", i);
        }
    }
}

void test_unchained_transfer(uint32_t src, uint32_t dst, bool verbose)
{
    uint64_t elapsed = 0, now;
    hpm_stat_t stat;
    uint32_t errors, burst_len_in_byte;
#if USE_IRQ
    intc_m_enable_irq_with_priority(TEST_DMA_IRQ, 1);
#endif

    for (int32_t i = DMA_NUM_TRANSFER_PER_BURST_1024T; i >= 0; i--) {
        reset_transfer_status();
        burst_len_in_byte = (1 << i) * (1 << DMA_SOC_TRANSFER_WIDTH_MAX);
        src += burst_len_in_byte;
        prepare_test_data((uint8_t *)src, SIZE_PER_TEST);

        printf("dma transferring data from 0x%x to 0x%x, burst size: %d bytes\n",
                src, dst, burst_len_in_byte);
        now = mchtmr_get_count(HPM_MCHTMR);
        stat = dma_start_memcpy(TEST_DMA_CONTROLLER, 0,
                (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, dst),
                (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, src),
                SIZE_PER_TEST, burst_len_in_byte);
        if (stat != status_success) {
            printf("failed to start dma transfer\n");
            continue;
        }

#if USE_IRQ
        while (!dma_transfer_done) {
            __asm("nop");
        }
#else
        hpm_stat_t stat;
        do {
            stat = dma_check_transfer_status(TEST_DMA_CONTROLLER, TEST_DMA_CHANNEL);
        } while ((stat & DMA_CHANNEL_STATUS_ONGOING));
        if (0 == (stat & DMA_CHANNEL_STATUS_TC)) {
            dma_transfer_error = true;
        }
#endif
        elapsed += (mchtmr_get_count(HPM_MCHTMR) - now);
        printf("rw throughput: %.2f KB/s\n",
                (double) (SIZE_PER_TEST >> 10) * timer_freq_in_hz / elapsed);

        if (dma_transfer_error) {
            printf(" dma transfer failed\n");
            continue;
        }

        errors = compare_buffers((uint8_t *)src, (uint8_t *)dst, SIZE_PER_TEST, false);
        if (errors) {
            printf("compare failed: %d errors\n", errors);
            continue;
        }
    }
}

int main(void)
{
    board_init();
    board_init_console();

    timer_freq_in_hz = clock_get_frequency(TIMER_CLOCK_NAME);

    printf("dma example start\n");

    printf("unchained transfer\n");
    printf("write testing\n");
    test_unchained_transfer(SRC_ADDRESS, DST_ADDRESS, false);
    printf("read testing\n");
    test_unchained_transfer(DST_ADDRESS, SRC_ADDRESS, false);

    printf("chained transfer\n");
    test_chained_transfer(false);

    printf("dma example end\n");
    return 0;
}
