/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_l1c_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_dram_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_sysctl_drv.h"

#define DRAM_EXAMPLE_PATTERN (0xA55A5AA5UL)
#define SDRAM_TEST_OFFSET 0x8000

#ifndef TIMER_CLOCK_NAME
#define TIMER_CLOCK_NAME clock_mchtmr0
#endif

#ifndef DRAM_CLOCK_NAME
#define DRAM_CLOCK_NAME clock_dram
#endif

uint32_t timer_freq_in_hz;

hpm_stat_t rw_comparison(uint32_t start, uint32_t size_in_byte)
{
    hpm_stat_t status = status_success;
    uint32_t i, delay = 0;

    printf("comparison test: from 0x%x to 0x%x\n", start, start+size_in_byte);
    for (i = 0; i < size_in_byte; i+=4)
    {
       *((uint32_t *)(start + i)) = (i << 24) | ((i + 1) << 16)
                                | ((i + 2) << 8) | ((i + 3));
    }
    
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback(start, size_in_byte);
        printf("d-cached enabled and flushed\n");
    } else {
        printf("d-cached disabled\n");
    }
    for (i = 0; i < size_in_byte; i+=4) {
       if(*((uint32_t *)(start + i)) != ((i << 24) | ((i + 1) << 16)
                                | ((i + 2) << 8) | ((i + 3))))
       {
            printf("[%x] data mismatch @ 0x%lx: %lx\n", delay, start + i, *(uint32_t *)(start + i));
            status = status_fail;
            break;
       }
    }

    printf("%s comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");

    return status;
}

void rw_throughput(uint32_t start, uint32_t size_in_byte)
{
    uint64_t elapsed = 0, now;
    register uint32_t i, j;
    register uint32_t address = start;
    register uint32_t single_loop_byte;
    register uint32_t loop;

    if (start == BOARD_SDRAM_ADDRESS) {
        loop = 1;
        single_loop_byte = size_in_byte;
    } else {
        single_loop_byte = HPM_L1C_CACHE_SIZE << 2;
        loop = size_in_byte / single_loop_byte;
    }

    if (l1c_dc_is_enabled()) {
        printf("d-cached enabled\n");
        l1c_dc_flush(start, size_in_byte);
    } else {
        printf("d-cached disabled\n");
    }

    printf("read performance @0x%x, %uKB\n", start, size_in_byte >> 10);
    for (j = 0, elapsed = 0; j < loop; j++) {
        now = mchtmr_get_count(HPM_MCHTMR);
        for (i = 0; i < single_loop_byte; i += 4)
        {
            __asm volatile ("" : : "r" (*((uint32_t *)(address + i))));
        }
        elapsed += (mchtmr_get_count(HPM_MCHTMR) - now);
    }
    printf("read throughput: %.2f KB/s\n",
            (double) (size_in_byte >> 10) * timer_freq_in_hz / elapsed);

    printf("write performance @0x%x, %uKB\n", start, size_in_byte >> 10);
    for (j = 0, elapsed = 0; j < loop; j++) {
        now = mchtmr_get_count(HPM_MCHTMR);
        for (i = 0; i < single_loop_byte; i += 4)
        {
            (*((uint32_t *)(address + i))) = DRAM_EXAMPLE_PATTERN;
        }
        elapsed += (mchtmr_get_count(HPM_MCHTMR) - now);
    }
    printf("write throughput: %.2f KB/s\n",
            (double) (size_in_byte >> 10) * timer_freq_in_hz / elapsed);

}

void check_freq(void)
{
    uint32_t freq;
    freq = sysctl_monitor_measure_frequency(HPM_SYSCTL, 0, monitor_target_clk_top_dram, false);
    printf("freq = %d\n", freq);
}

uint32_t initialize_sdram(void)
{
    uint32_t dram_clk_in_hz = board_init_dram_clock();
    dram_config_t config = {0};
    dram_sdram_config_t sdram_config = {0};

    dram_default_config(HPM_DRAM, &config);
    config.dqs = DRAM_DQS_INTERNAL;
    dram_init(HPM_DRAM, &config);

    sdram_config.bank_num = DRAM_SDRAM_BANK_NUM_4;
    sdram_config.prescaler = 0x3;
    sdram_config.burst_len_in_byte = 8;
    sdram_config.auto_refresh_count_in_one_burst = 1;
    sdram_config.col_addr_bits = DRAM_SDRAM_COLUMN_ADDR_9_BITS;
    sdram_config.cas_latency = DRAM_SDRAM_CAS_LATENCY_3;

    sdram_config.precharge_to_act_in_ns = 18;   /* Trp */
    sdram_config.act_to_rw_in_ns = 18;          /* Trcd */
    sdram_config.refresh_recover_in_ns = 70;     /* Trfc/Trc */
    sdram_config.write_recover_in_ns = 12;      /* Twr/Tdpl */
    sdram_config.cke_off_in_ns = 42;             /* Trcd */
    sdram_config.act_to_precharge_in_ns = 42;   /* Tras */

    sdram_config.self_refresh_recover_in_ns = 66;   /* Txsr */
    sdram_config.refresh_to_refresh_in_ns = 66;     /* Trfc/Trc */
    sdram_config.act_to_act_in_ns = 12;             /* Trrd */
    sdram_config.idle_timeout_in_ns = 6;
    sdram_config.cs_mux_pin = DRAM_IO_MUX_NOT_USED; 

    sdram_config.cs = BOARD_SDRAM_CS;
    sdram_config.base_address = BOARD_SDRAM_ADDRESS;
    sdram_config.size_in_byte = BOARD_SDRAM_SIZE;
    sdram_config.port_size = BOARD_SDRAM_PORT_SIZE;
    sdram_config.refresh_count = BOARD_SDRAM_REFRESH_COUNT;
    sdram_config.refresh_in_ms = BOARD_SDRAM_REFRESH_IN_MS;
    sdram_config.data_width_in_byte = BOARD_SDRAM_DATA_WIDTH_IN_BYTE;
    sdram_config.delay_cell_value = 29;

    dram_config_sdram(HPM_DRAM, dram_clk_in_hz, &sdram_config);
    return dram_clk_in_hz;
}

int main(void)
  {
    uint32_t dram_clk_in_hz;

    board_init();

    timer_freq_in_hz = clock_get_frequency(TIMER_CLOCK_NAME);
    /* board_init_sdram_1_8v_pins(); */
    board_init_sdram_pins();
    
    dram_clk_in_hz = initialize_sdram();
    printf("dram example start @ %d\n", dram_clk_in_hz);

    if (!l1c_dc_is_enabled()) {
        l1c_dc_enable();
    }
    printf("Dcache Enabled\n");
    printf("Comparison test\n");
    rw_comparison(BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE);
    printf("Testing SDRAM\n");
    rw_throughput(BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE);

    l1c_dc_writeback_all();
    l1c_dc_disable();
    printf("Dcache Disabled\n");
    printf("Comparison test\n");
    rw_comparison(BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE);
    printf("Testing SDRAM\n");
    rw_throughput(BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE);


    printf("dram example end\n");
    while(1);
    return 0;
}
