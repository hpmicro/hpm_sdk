/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_l1c_drv.h"

#define SIZE_PER_TEST   (0x00004000UL)
#define LINKED_DESCRIPTOR_NUM 2

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4) uint8_t s_dst_buffer[LINKED_DESCRIPTOR_NUM + 1][SIZE_PER_TEST];
ATTR_PLACE_AT_WITH_ALIGNMENT(".fast_ram.non_init", 4) uint8_t s_src_buffer[SIZE_PER_TEST];
/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_linked_descriptor_t descriptors[LINKED_DESCRIPTOR_NUM];

#define DST_ADDRESS     ((uint32_t)(&s_dst_buffer[0][0]))
#define SRC_ADDRESS     ((uint32_t)(&s_src_buffer[0]))

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

#ifndef TIMER_CLOCK_NAME
#define TIMER_CLOCK_NAME clock_mchtmr0
#endif

uint32_t timer_freq_in_hz;

volatile bool dma_transfer_done;
volatile bool dma_transfer_error;
volatile bool dma_test_chain_flag;
volatile uint8_t dma_chain_tc_irq_cnt;

static void reset_transfer_status(void)
{
    dma_transfer_done = false;
    dma_transfer_error = false;
}

static void prepare_test_data(uint8_t *buffer, uint32_t size_in_byte, uint32_t magic_data)
{
    uint32_t i;
    for (i = 0; i < size_in_byte; i++) {
        buffer[i] = i + magic_data;
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

SDK_DECLARE_EXT_ISR_M(TEST_DMA_IRQ, isr_dma)
void isr_dma(void)
{
    uint32_t stat;

    stat = dma_check_transfer_status(TEST_DMA_CONTROLLER, TEST_DMA_CHANNEL);
    if (0 != (stat & DMA_CHANNEL_STATUS_ERROR)) {
        dma_transfer_error = true;
    }

    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        if (dma_test_chain_flag) {
            dma_chain_tc_irq_cnt++;
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
            if (dma_chain_tc_irq_cnt == (LINKED_DESCRIPTOR_NUM + 1)) {
#else
            if (dma_chain_tc_irq_cnt == 1) {
#endif
                dma_transfer_done = true;
            }
        } else {
            dma_transfer_done = true;
        }
    }
}

void test_chained_transfer(bool verbose)
{
    uint32_t i, errors;
    dma_channel_config_t ch_config = {0};

    dma_test_chain_flag = true;
    reset_transfer_status();

    intc_m_enable_irq_with_priority(TEST_DMA_IRQ, 1);

    prepare_test_data((uint8_t *)SRC_ADDRESS, SIZE_PER_TEST, 0x5AA5);

    dma_default_channel_config(TEST_DMA_CONTROLLER, &ch_config);

    for (i = 0; i < LINKED_DESCRIPTOR_NUM; i++) {
        ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, SRC_ADDRESS);
        ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, DST_ADDRESS + (i + 1) * SIZE_PER_TEST);
        ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_8T;
        ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
        ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
        ch_config.size_in_byte = SIZE_PER_TEST;
        if (i == (LINKED_DESCRIPTOR_NUM - 1)) {
            ch_config.linked_ptr = 0;
        } else {
            ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors[i + 1]);
        }
        if (status_success != dma_config_linked_descriptor(TEST_DMA_CONTROLLER, &descriptors[i], TEST_DMA_CHANNEL, &ch_config)) {
            printf("dma config linked descriptor failed\n");
            return;
        }
    }

    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, SRC_ADDRESS);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, DST_ADDRESS);
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_8T;
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.size_in_byte = SIZE_PER_TEST;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors[0]);
    if (status_success != dma_setup_channel(TEST_DMA_CONTROLLER, TEST_DMA_CHANNEL, &ch_config, true)) {
        printf("dma setup channel failed\n");
        return;
    }
    printf("dma setup channel done\n");

    while (!dma_transfer_done) {
        __asm("nop");
    }

    if (dma_transfer_error) {
        printf(" chained transfer failed\n");
        return;
    }

    errors = compare_buffers((uint8_t *)ch_config.src_addr, (uint8_t *)ch_config.dst_addr, ch_config.size_in_byte, verbose);
    if (!errors) {
        printf(" [%d]: data match\n", 0);
    } else {
        printf(" [%d]: !!! data mismatch\n", 0);
    }

    for (i = 0; i < LINKED_DESCRIPTOR_NUM; i++) {
        errors = compare_buffers((uint8_t *)descriptors[i].src_addr, (uint8_t *)descriptors[i].dst_addr, descriptors[i].trans_size << DMA_TRANSFER_WIDTH_BYTE, verbose);
        if (!errors) {
            printf(" [%d]: data match\n", (i + 1));
        } else {
            printf(" [%d]: !!! data mismatch\n", (i + 1));
        }
    }
}

void test_unchained_transfer(uint32_t src, uint32_t dst, bool verbose)
{
    uint64_t elapsed = 0, now;
    hpm_stat_t stat;
    uint32_t errors, burst_len_in_byte;

    dma_test_chain_flag = false;

#if USE_IRQ
    intc_m_enable_irq_with_priority(TEST_DMA_IRQ, 1);
#endif

    for (int32_t i = DMA_SOC_TRANSFER_PER_BURST_MAX(TEST_DMA_CONTROLLER); i >= 0; i--) {
        reset_transfer_status();
        burst_len_in_byte = (1 << i) * (1 << DMA_SOC_TRANSFER_WIDTH_MAX(TEST_DMA_CONTROLLER));
        prepare_test_data((uint8_t *)src, SIZE_PER_TEST, burst_len_in_byte);

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
        } while ((stat & DMA_CHANNEL_STATUS_TC) == 0);
#endif
        elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
        printf("rw throughput: %.2f KB/s\n",
                (double) (SIZE_PER_TEST >> 10) * timer_freq_in_hz / elapsed);

        if (dma_transfer_error) {
            printf("dma transfer failed\n");
            continue;
        }

        errors = compare_buffers((uint8_t *)src, (uint8_t *)dst, SIZE_PER_TEST, verbose);
        if (errors) {
            printf("compare failed: %d errors\n", errors);
            continue;
        }
    }
}

int main(void)
{
    board_init();

    timer_freq_in_hz = clock_get_frequency(TIMER_CLOCK_NAME);

    printf("\ndma example start\n");

    printf("\nunchained transfer\n");
    printf("\nwrite testing\n");
    test_unchained_transfer(SRC_ADDRESS, DST_ADDRESS, false);
    printf("\nread testing\n");
    test_unchained_transfer(DST_ADDRESS, SRC_ADDRESS, false);

    printf("\nchained transfer\n");
    test_chained_transfer(false);

    printf("\ndma example end\n");
    return 0;
}
