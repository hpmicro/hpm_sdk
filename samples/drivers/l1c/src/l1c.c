/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_l1c_drv.h"

#define GET_BITFIELD_VALUE(type, value, prefix, bf) \
    (type)((value & prefix ## bf ## _MASK) >> prefix ## bf ## _SHIFT)
#define HPM_L1C_CFG_GET_BITFIELD_VALUE(value, bf) \
    GET_BITFIELD_VALUE(uint8_t, value, HPM_L1C_CFG_, bf)

char *ecc_scheme[] = {"No parity/ECC", "Parity", "ECC"};
char *locking_support[] = {"No locking support", "with locking support"};

void print_cache_config(char *name, uint32_t cfg)
{
    uint8_t size = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, SIZE);
    uint8_t seth = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, SETH);
    uint8_t set = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, SET);
    uint8_t way = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, WAY);
    uint8_t locking = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, LOCK);
    uint8_t ecc = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, ECC);
    uint8_t lm_ecc = HPM_L1C_CFG_GET_BITFIELD_VALUE(cfg, LM_ECC);

    if (size == 0)
    { 
        printf("no %s available\n", name);
        return;
    }

    printf("%s config:\n", name);
    printf("  set: ");
    if (seth == 1)
    {
        printf("%d\n", 1 << (5 - set));
    }
    else
    {
        printf("%d\n", 1 << (6 + set));
    }
    printf("  way: ");
    if (way == 0)
    {
        printf("direct-mapped\n");
    }
    else
    {
        printf("%d-way\n", way + 1);
    }
    printf("  size: %d bytes\n", 1 << (2 + size));
    printf("  lock: %s\n", locking_support[locking]);
    printf("  ecc: %s\n", ecc_scheme[ecc]);
    printf("  local memory ecc: %s\n", ecc_scheme[lm_ecc]);
}

int main(void)
{
    uint32_t cfg;
    board_init();
    printf("l1 cache driver example\n");

    cfg = l1c_ic_get_config();
    print_cache_config("icache", cfg);
    cfg = l1c_dc_get_config();
    print_cache_config("dcache", cfg);

    l1c_ic_enable();
    l1c_dc_enable();
    cfg = l1c_get_control();

    printf("dcache is %s\n", cfg & HPM_MCACHE_CTL_DC_EN_MASK ? "enabled" : "disabled");
    printf("icache is %s\n", cfg & HPM_MCACHE_CTL_IC_EN_MASK ? "enabled" : "disabled");

    while(1);
    return 0;
}
