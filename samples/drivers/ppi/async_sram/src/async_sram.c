/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_ppi.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_pmp_drv.h"

#define SRAM_BASE_ADDR 0xF8000000U    /* Base address should be aligned by 1MB */
#define SRAM_SIZE      BOARD_PPI_ASYNC_SRAM_SIZE
#define PMP_ENTRY_IDX  7              /* greater or equal than board_init_pmp() index*/

static void rw_comparison(uint32_t start, uint32_t size_in_byte)
{
    hpm_stat_t status;
    volatile uint32_t now, elapsed;
    volatile uint32_t *u32_data_ptr;
    volatile uint16_t *u16_data_ptr;
    volatile uint8_t *u8_data_ptr;
    uint32_t u32_tmp;
    uint16_t u16_tmp;
    uint8_t u8_tmp;

    printf("comparison test: from 0x%x to 0x%x\n", start, start + size_in_byte);

    u32_data_ptr = (volatile uint32_t *)start;
    now = mchtmr_get_count(HPM_MCHTMR);
    for (uint32_t i = 0; i < (size_in_byte>>2); i++) {
        u32_data_ptr[i] = 0x12345678 + i;
    }
    elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
    printf("write u32 elapsed tick: %d\n", elapsed);
    status = status_success;
    now = mchtmr_get_count(HPM_MCHTMR);
    for (uint32_t i = 0; i < (size_in_byte>>2); i++) {
        u32_tmp = u32_data_ptr[i];
        if (u32_tmp != 0x12345678 + i) {
            printf("[%#x] data u32 mismatch, expect: %0#x, actual: %#x\n", (uint32_t)&(u32_data_ptr[i]), 0x12345678 + i, u32_tmp);
            status = status_fail;
        }
    }
    elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
    printf("read u32 elapsed tick: %d\n", elapsed);
    printf("%s u32 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");

    u16_data_ptr = (volatile uint16_t *)start;
    now = mchtmr_get_count(HPM_MCHTMR);
    for (uint32_t i = 0; i < (size_in_byte>>1); i++) {
        u16_data_ptr[i] = (uint16_t)(0x5AA5 + i);
    }
    elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
    printf("write u16 elapsed tick: %d\n", elapsed);
    status = status_success;
    now = mchtmr_get_count(HPM_MCHTMR);
    for (uint32_t i = 0; i < (size_in_byte>>1); i++) {
        u16_tmp = u16_data_ptr[i];
        if (u16_tmp != (uint16_t)(0x5AA5 + i)) {
            printf("[%#x] data u16 mismatch, expect: %0#x, actual: %#x\n", (uint32_t)&(u16_data_ptr[i]), (uint16_t)(0x5AA5 + i), u16_tmp);
            status = status_fail;
        }
    }
    elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
    printf("read u16 elapsed tick: %d\n", elapsed);
    printf("%s u16 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");

    u8_data_ptr = (volatile uint8_t *)start;
    now = mchtmr_get_count(HPM_MCHTMR);
    for (uint32_t i = 0; i < (size_in_byte); i++) {
        u8_data_ptr[i] = (i % 256);
    }
    elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
    printf("write u8 elapsed tick: %d\n", elapsed);
    status = status_success;
    now = mchtmr_get_count(HPM_MCHTMR);
    for (uint32_t i = 0; i < (size_in_byte); i++) {
        u8_tmp = u8_data_ptr[i];
        if (u8_tmp != (i % 256)) {
            printf("[%#x] data u8 mismatch, expect: %0#x, actual: %#x\n", (uint32_t)&(u8_data_ptr[i]), (i % 256), u8_tmp);
            status = status_fail;
        }
    }
    elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
    printf("read u8 elapsed tick: %d\n", elapsed);
    printf("%s u8 comparison %s\n", status == status_fail ? "!! " : "** ", status == status_fail ? "failed" : "succeeded");
}

static void init_sram_config(void)
{
    ppi_async_sram_config_t config;

    ppi_get_async_sram_defconfig(HPM_PPI, &config);
    config.base_address = SRAM_BASE_ADDR;
    config.size_in_byte = SRAM_SIZE;
    config.port_size = ppi_port_size_16bits;
    config.ad_mux_mode = BOARD_PPI_ASYNC_SRAM_AD_MUX_MODE;
    config.cs_valid_polarity = false;
    config.dm_valid_polarity = false;
    config.addr_valid_polarity = true;
    config.adv_ctrl_pin = BOARD_PPI_ASYNC_SRAM_ADV_CTRL_PIN;
    config.rel_ctrl_pin = BOARD_PPI_ASYNC_SRAM_OE_CTRL_PIN;
    config.wel_ctrl_pin = BOARD_PPI_ASYNC_SRAM_WE_CTRL_PIN;
    config.as_in_ns = 25;
    config.ah_in_ns = 25;
    config.rel_in_ns = 50;
    config.reh_in_ns = 0;
    config.wel_in_ns = 50;
    config.weh_in_ns = 0;
    config.dq_sig_sel[0] = BOARD_PPI_ASYNC_SRAM_SIG_DQ0_7;
    config.dq_sig_sel[1] = BOARD_PPI_ASYNC_SRAM_SIG_DQ8_15;
    config.dq_sig_sel[2] = BOARD_PPI_ASYNC_SRAM_SIG_DQ16_23;
    config.dq_sig_sel[3] = BOARD_PPI_ASYNC_SRAM_SIG_DQ24_31;
    ppi_config_async_sram(HPM_PPI, BOARD_PPI_ASYNC_SRAM_CS_INDEX, 0, &config);
}

static void init_pmp_for_ppi(void)
{
    uint32_t start_addr = SRAM_BASE_ADDR;
    uint32_t length = SRAM_SIZE;
    pmp_entry_t pmp_entry;

    if (length > 0) {
        /* Ensure the address and the length are power of 2 aligned */
        assert((length & (length - 1U)) == 0U);
        assert((start_addr & (length - 1U)) == 0U);
        pmp_entry.pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
        pmp_entry.pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
        pmp_entry.pma_addr = PMA_NAPOT_ADDR(start_addr, length);
        /* !!! Note: PPI memory cacheable only support "MEM_TYPE_MEM_WB_READ_WRITE_ALLOC" */
        pmp_entry.pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_WB_READ_WRITE_ALLOC, AMO_EN);
    }

    pmp_config_entry(&pmp_entry, PMP_ENTRY_IDX);
}

int main(void)
{
    board_init();
    clock_add_to_group(clock_ppi0, 0);
    init_ppi_pins();
    init_sram_config();

    printf("ppi sram example\n");

    printf("\nNoncacheable Test\n");
    rw_comparison(SRAM_BASE_ADDR, SRAM_SIZE);

    printf("\nCacheable Test\n");
    init_pmp_for_ppi();
    rw_comparison(SRAM_BASE_ADDR, SRAM_SIZE);

    while (1) {
        ;
    }
    return 0;
}
