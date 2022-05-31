/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_pmp_drv.h"

/**
 * @brief PMP example Configuration List
 *        See the description of the following macro in the hpm_pmp_drv.h for more details
 *         PMP_CFG
 *         PMP_ADDR
 *         PMA_CFG
 *         PMA_ADDR
 */
const pmp_entry_t pmp_example_cfg_list[] = {
    {
        /* Address range: 0-0x1FFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK) },
        .pmp_addr = PMP_NAPOT_ADDR(0, SIZE_1MB * 512U),
        .pma_cfg = { .val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_NON_BUF, AMO_EN) }, /* Non-Cacheable */
        .pma_addr = PMA_NAPOT_ADDR(0, SIZE_1MB * 512U),
    },

    {
        /* Address range: 0x2000_0000 - 0x20001_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_EN, WRITE_DIS, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK) },
        .pmp_addr = PMP_NAPOT_ADDR(0x20000000UL, SIZE_1KB * 128U),
        .pma_cfg = { .val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_WT_READ_ALLOC, AMO_EN) },/* Cacheable */
        .pma_addr = PMA_NAPOT_ADDR(0x20000000UL, SIZE_1KB * 128U),
    },
    {
        /* Address range: 0x20002_0000 - 0x2FFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_DIS, WRITE_DIS, EXECUTE_DIS, ADDR_MATCH_TOR, REG_UNLOCK) }, /* Invalid */
        .pmp_addr = PMP_TOR_ADDR(0x30000000U),
    },
    {
        /* Address range: 0x3000_0000 - 0x3FFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_DIS, ADDR_MATCH_NAPOT, REG_UNLOCK) },
        .pmp_addr = PMP_NAPOT_ADDR(0x30000000UL, SIZE_1MB * 256U),
        .pma_cfg = { .val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_DEV_NON_BUF, AMO_EN) }, /* Device memory */
        .pma_addr = PMA_NAPOT_ADDR(0x30000000UL, SIZE_1MB * 256U),
    },
    {
        /* Address range: 0x4000_0000 - 0x4FFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK) },
        .pmp_addr = PMP_NAPOT_ADDR(0x40000000UL, SIZE_1MB * 256U),
        .pma_cfg = { .val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_WT_READ_ALLOC, AMO_EN) }, /* Write-through */
        .pma_addr = PMA_NAPOT_ADDR(0x40000000UL, SIZE_1MB * 256U),
    },
    {
        /* Disable access from 0x5000_0000 - 0x7FFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_DIS, WRITE_DIS, EXECUTE_DIS, ADDR_MATCH_TOR, REG_UNLOCK) }, /* Invalid */
        .pmp_addr = PMP_TOR_ADDR(0x80000000UL),
    },
    {
        /* Address range: 0x8000_0000 - 0x9FFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK) },
        .pmp_addr = PMP_NAPOT_ADDR(0x80000000UL, SIZE_1MB * 512U),
        .pma_cfg = { .val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_WB_READ_WRITE_ALLOC, AMO_EN) }, /* Write-back */
        .pma_addr = PMA_NAPOT_ADDR(0x80000000UL, SIZE_1MB * 512U),
    },
    /* Disable access from 0xA000_0000 - 0xDFFF_FFFF */
    {
        .pmp_cfg = { .val = PMP_CFG(READ_DIS, WRITE_DIS, EXECUTE_DIS, ADDR_MATCH_TOR, REG_UNLOCK) }, /* Invalid */
        .pmp_addr = PMP_TOR_ADDR(0xE0000000UL),
    },
    {
        /* Address range: 0xE000_0000 - 0xFFFF_FFFF */
        .pmp_cfg = { .val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_DIS, ADDR_MATCH_NAPOT, REG_UNLOCK) },
        .pmp_addr = PMP_NAPOT_ADDR(0xE0000000UL, SIZE_1MB * 512),
        .pma_cfg = { .val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_DEV_NON_BUF, AMO_EN) }, /* Device memory */
        .pma_addr = PMA_NAPOT_ADDR(0xE0000000UL, SIZE_1MB * 512),
    }};

int main(void)
{
    board_init();

    printf("PMP example\n");

    hpm_stat_t status = pmp_config(pmp_example_cfg_list, ARRAY_SIZE(pmp_example_cfg_list));
    if (status == status_success) {
        printf("PMP configuration completed\n");
    }
    else {
        printf("PMP configuration failed due to error\n");
    }


    while (1) {

    }

    return 0;
}
